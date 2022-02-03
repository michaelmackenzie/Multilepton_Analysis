//Script to generate Tag and Probe (TnP) electron trigger scale factors

//Fit the mass distribution with a signal and background function
double extract_nz(TH1F* hMass, double& nerror, int BkgMode, TString figname = "", TString figtitle = "") {

  const double min_mass(60.), max_mass(120.);

  //Create the observable
  RooRealVar mass("mass", "Dilepton mass", 91., min_mass, max_mass, "GeV/c^{2}");
  mass.setBins((max_mass-min_mass)/hMass->GetBinWidth(1));

  //Build the signal PDF
  RooRealVar mean ("mean" , "mean" , 91., 85., 95.); //central RooCBShape
  RooRealVar width("width", "width", 2.5, 0.1, 5. );
  RooRealVar sigma("sigma", "sigma", 2. , 0.1, 5. );
  RooRealVar alpha("alpha", "alpha", 1. , 0.1, 10.);
  RooRealVar enne ("enne" , "enne" , 5. , 0.01, 30.);

  RooRealVar mean2 ("mean2" , "mean2" , 91., 70., 100.); //additional Gaussian
  RooRealVar sigma2("sigma2", "sigma2", 5. , 0.1, 10. );

  RooCBShape  sigpdf1("sigpdf1", "sigpdf1", mass, mean, sigma, alpha, enne);
  RooGaussian sigpdf2("sigpdf2", "sigpdf2", mass, mean2, sigma2);
  RooRealVar  fracsig("fracsig", "fracsig", 0.7, 0., 1.);
  RooAddPdf   sigpdf ("sigpdf" , "sigpdf" , sigpdf1, sigpdf2, fracsig);

  //Build the background PDF
  RooAbsPdf* bkgpdf;
  if(BkgMode == 0) { //exponential PDF
    RooRealVar* tau = new RooRealVar("tau", "tau", 1., 0., 10.);
    bkgpdf = new RooExponential("bkgpdf", "bkgpdf", mass, *tau);
  } else if(BkgMode == 1) { //Bernstein polynomial
    RooRealVar* a_bkg = new RooRealVar("a_bkg", "a_bkg", 0.1, -10., 10.);
    RooRealVar* b_bkg = new RooRealVar("b_bkg", "b_bkg", 0.1, -10., 10.);
    RooRealVar* c_bkg = new RooRealVar("c_bkg", "c_bkg", 0. , -10., 10.);

    bkgpdf = new RooBernstein("bkgpdf", "bkgpdf", mass, RooArgList(*a_bkg, *b_bkg, *c_bkg));
  }

  RooRealVar N_sig("N_sig", "N_sig", hMass->Integral()*0.9, hMass->Integral()/200., hMass->Integral()*10.);
  RooRealVar N_bkg("N_bkg", "N_bkg", hMass->Integral()*0.9, 0., hMass->Integral()*10.);

  RooAddPdf totpdf("totpdf", "Total PDF", RooArgList(sigpdf,*bkgpdf), RooArgList(N_sig,N_bkg));

  //Create a data object to fit
  RooDataHist data("data",  "data", RooArgList(mass), hMass);

  //Perform the fit
  totpdf.fitTo(data, RooFit::SumW2Error(1), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1), RooFit::PrintLevel(-1));
  double nsig = N_sig.getVal();
  nerror = N_sig.getError();
  if(nsig < 0.) cout << "N(sig) < 0 for " << figname.Data() << ", " << figtitle.Data() << endl;
  nsig = max(nsig, 0.);

  //Draw the fit and write the figure to disk
  auto xframe = mass.frame(RooFit::Title(" "));
  data.plotOn(xframe);
  totpdf.plotOn(xframe, RooFit::LineColor(kBlue));
  totpdf.plotOn(xframe, RooFit::Components("sigpdf"), RooFit::LineColor(kBlue), RooFit::LineStyle(kDashed));
  totpdf.plotOn(xframe, RooFit::Components("bkgpdf"), RooFit::LineColor(kRed ), RooFit::LineStyle(kDashed));
  TCanvas* c = new TCanvas();
  xframe->Draw();
  TLatex label;
  label.SetNDC();
  label.SetTextFont(72);
  label.SetTextSize(0.03);
  label.SetTextAlign(13);
  label.SetTextAngle(0);
  label.DrawLatex(0.12, 0.88, figtitle.Data());
  label.DrawLatex(0.12, 0.83, Form("N(sig) = %.1f", nsig));
  if(figname != "")
    c->SaveAs(figname.Data());
  delete c;
  delete xframe;

  delete bkgpdf;

  return nsig;
}


void scale_factors(int year = 2016, int isMC = 0) {

  TString path = "root://cmseos.fnal.gov//store/user/mmackenz/egamma_tnp/";
  TFile* f;
  if     (isMC == 0) f = TFile::Open(Form("%sdata/EGammaTnPAnalysis_SingleEle_%i.root", path.Data(), year), "READ");
  else if(isMC == 1) f = TFile::Open(Form("%sMC/EGammaTnPAnalysis_DY50-amcnlo_%i.root", path.Data(), year), "READ");
  else if(isMC == 2) f = TFile::Open(Form("%sMC/EGammaTnPAnalysis_DY50-madgraph_%i.root", path.Data(), year), "READ");
  else {
    cout << "Undefined isMC value " << isMC << endl;
    return;
  }
  if(!f) return;

  TTree* t = (TTree*) f->Get("Events");
  if(!t) {
    cout << "Events tree not found!\n";
    return;
  }

  ///////////////////////////////////////
  // Initialize tree branch addresses
  ///////////////////////////////////////

  float tag_pt, tag_eta, tag_sc_eta, tag_phi, tag_q;
  float probe_pt, probe_eta, probe_sc_eta, probe_phi, probe_q;
  float pair_pt, pair_eta, pair_mass;
  bool  probe_triggered;
  float pu_weight(1.), gen_weight(1.);
  t->SetBranchStatus("*", 0);
  t->SetBranchStatus("tag_Ele_pt"     , 1); t->SetBranchAddress("tag_Ele_pt"     , &tag_pt         );
  t->SetBranchStatus("tag_Ele_eta"    , 1); t->SetBranchAddress("tag_Ele_eta"    , &tag_eta        );
  t->SetBranchStatus("tag_sc_eta"     , 1); t->SetBranchAddress("tag_sc_eta"     , &tag_sc_eta     );
  t->SetBranchStatus("tag_Ele_phi"    , 1); t->SetBranchAddress("tag_Ele_phi"    , &tag_phi        );
  t->SetBranchStatus("tag_Ele_q"      , 1); t->SetBranchAddress("tag_Ele_q"      , &tag_q          );
  t->SetBranchStatus("probe_Ele_pt"   , 1); t->SetBranchAddress("probe_Ele_pt"   , &probe_pt       );
  t->SetBranchStatus("probe_Ele_eta"  , 1); t->SetBranchAddress("probe_Ele_eta"  , &probe_eta      );
  t->SetBranchStatus("probe_sc_eta"   , 1); t->SetBranchAddress("probe_sc_eta"   , &probe_sc_eta   );
  t->SetBranchStatus("probe_Ele_phi"  , 1); t->SetBranchAddress("probe_Ele_phi"  , &probe_phi      );
  t->SetBranchStatus("probe_Ele_q"    , 1); t->SetBranchAddress("probe_Ele_q"    , &probe_q        );
  t->SetBranchStatus("probe_triggered", 1); t->SetBranchAddress("probe_triggered", &probe_triggered);
  t->SetBranchStatus("pair_pt"        , 1); t->SetBranchAddress("pair_pt"        , &pair_pt        );
  t->SetBranchStatus("pair_mass"      , 1); t->SetBranchAddress("pair_mass"      , &pair_mass      );
  if(isMC) {
    t->SetBranchStatus("puWeight" , 1); t->SetBranchAddress("puWeight" , &pu_weight );
    t->SetBranchStatus("genWeight", 1); t->SetBranchAddress("genWeight", &gen_weight);
  }
  ///////////////////////////////////////
  // Initialize histograms
  ///////////////////////////////////////

  double eta_bins[] = {-2.5,-2.0,-1.566,-1.4442, -0.8, 0.0, 0.8, 1.4442, 1.566, 2.0, 2.5};
  double pt_bins [] = {28,30,33,36,40,45,50,70,100,500};
  // double pt_bins [] = {28,33,38,43,46,50,70,100,150,500};
  // double eta_bins[] = {-2.4,-2.0,-1.566,-1.4442, -0.8, 0.0, 0.8, 1.4442, 1.566, 2.0, 2.4};  //matching Riccardo's binning
  // double pt_bins [] = {1,(year == 2016) ? 30. : 33.,40,50,100,200,500}; //matching Riccardo's binning
  int n_eta_bins = sizeof(eta_bins) / sizeof(*eta_bins) - 1;
  int n_pt_bins  = sizeof( pt_bins) / sizeof( *pt_bins) - 1;
  TH2F* hPass  = new TH2F("hPass" , "Passing events", n_eta_bins, eta_bins, n_pt_bins, pt_bins);
  TH2F* hFail  = new TH2F("hFail" , "Failing events", n_eta_bins, eta_bins, n_pt_bins, pt_bins);
  hPass->Sumw2(); hFail->Sumw2();
  hPass->SetXTitle("SuperCluster #eta"); hPass->SetYTitle("p_{T} (GeV/c)");
  hFail->SetXTitle("SuperCluster #eta"); hFail->SetYTitle("p_{T} (GeV/c)");
  TH1F* hTagPt    = new TH1F("htagpt"   , "Electron p_{T}", 100,  0  , 100);
  TH1F* hTagEta   = new TH1F("htageta"  , "Electron #eta" ,  50, -2.5, 2.5);
  TH1F* hProbePt  = new TH1F("hprobept" , "Electron p_{T}", 100,  0  , 100);
  TH1F* hProbeEta = new TH1F("hprobeeta", "Electron #eta" ,  50, -2.5, 2.5);

  ///////////////////////////////////////
  // Make a di-lepton resonance histogram per bin
  ///////////////////////////////////////

  std::map<int, TH1F*> massHists_pass, massHists_fail;
  for(int xbin = 0; xbin <= hPass->GetNbinsX()+1; ++xbin) {
    for(int ybin = 0; ybin <= hPass->GetNbinsY()+1; ++ybin) {
      const int mapBin = xbin + 100*ybin;
      massHists_pass[mapBin] = new TH1F(Form("zmass_pass_x-%i_y-%i", xbin, ybin), "Dilepton resonance", 60, 60., 120.); massHists_pass[mapBin]->Sumw2();
      massHists_fail[mapBin] = new TH1F(Form("zmass_fail_x-%i_y-%i", xbin, ybin), "Dilepton resonance", 60, 60., 120.); massHists_fail[mapBin]->Sumw2();
    }
  }

  ///////////////////////////////////////
  // Process the data
  ///////////////////////////////////////

  ULong64_t nentries = t->GetEntriesFast();
  cout << nentries << " events to process\n";
  float tag_pt_min = 33.;
  if(year == 2016) tag_pt_min = 28.;
  float tag_eta_max = 2.3;//2.17;
  float probe_pt_min = (year == 2016) ? 28. : 33.;
  for(ULong64_t entry = 0; entry < nentries; ++entry) {
    if(entry % 100000 == 0) printf("Processing entry %12lld (%5.1f%%)...\n", entry, entry*100./nentries);
    t->GetEntry(entry);
    if(pair_mass <= 60. || pair_mass > 120.) continue;
    if(fabs(tag_sc_eta) > tag_eta_max) continue;
    if(1.4442 <= fabs(probe_sc_eta) && fabs(probe_sc_eta) <= 1.566) continue;
    if(1.4442 <= fabs(tag_sc_eta)   && fabs(tag_sc_eta)   <= 1.566) continue;
    if(tag_pt < tag_pt_min) continue;
    if(probe_pt < probe_pt_min) continue;
    float wt = pu_weight; //*((gen_weight < 0) ? -1. : 1.);
    if(probe_triggered) hPass->Fill(probe_sc_eta, probe_pt, wt);
    else                hFail->Fill(probe_sc_eta, probe_pt, wt);
    hTagPt  ->Fill(tag_pt  , wt); hTagEta  ->Fill(tag_sc_eta  , wt);
    hProbePt->Fill(probe_pt, wt); hProbeEta->Fill(probe_sc_eta, wt);
    const int xbin = hPass->GetXaxis()->FindBin(probe_sc_eta);
    const int ybin = hPass->GetYaxis()->FindBin(probe_pt);
    const int mapBin = xbin + 100*ybin;
    if(probe_triggered) massHists_pass[mapBin]->Fill(pair_mass, wt);
    else                massHists_fail[mapBin]->Fill(pair_mass, wt);
  }

  TH2F* hRatio = (TH2F*) hPass->Clone("hRatio_cc");
  TH2F* hTotal = (TH2F*) hPass->Clone("hTotal_cc");
  hTotal->Add(hFail);
  hRatio->Divide(hTotal);
  hRatio->SetTitle("Efficiency");

  //ensure efficiency <= 1
  for(int xbin = 1; xbin <= hRatio->GetNbinsX(); ++xbin) {
    for(int ybin = 1; ybin <= hRatio->GetNbinsY(); ++ybin) {
      if     (hRatio->GetBinContent(xbin, ybin) > 1.) hRatio->SetBinContent(xbin, ybin, 0.);
      else if(hRatio->GetBinContent(xbin, ybin) < 0.) hRatio->SetBinContent(xbin, ybin, 1.);
    }
  }

  ///////////////////////////////////////
  // Generate resonance fit scales
  ///////////////////////////////////////

  TH2F* hResPass = (TH2F*) hPass->Clone("hResPass"); hResPass->Reset();
  TH2F* hResFail = (TH2F*) hFail->Clone("hResFail"); hResFail->Reset();

  TString fitdir = Form("figures/fit_%i_%i", year, isMC);
  gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s", fitdir.Data(), fitdir.Data()));
  const int bkgmode = 1; //(Mode != 2) ? 1 : 0;
  for(int xbin = 1; xbin <= hResPass->GetNbinsX(); ++xbin) {
    for(int ybin = 1; ybin <= hResPass->GetNbinsY(); ++ybin) {
      const int mapBin = xbin + 100*ybin;
      if(massHists_pass[mapBin]->Integral() <= 0.) {
        hResPass->SetBinContent(xbin, ybin, 0.);
        hResFail->SetBinContent(xbin, ybin, 0.);
        continue;
      }
      TString title = Form("%.2f #leq #eta < %.2f, %.2f #leq p_{T} < %.2f GeV/c",
                           hResPass->GetXaxis()->GetBinLowEdge(xbin), hResPass->GetXaxis()->GetBinLowEdge(xbin) + hResPass->GetXaxis()->GetBinWidth(xbin),
                           hResPass->GetYaxis()->GetBinLowEdge(ybin), hResPass->GetYaxis()->GetBinLowEdge(ybin) + hResPass->GetYaxis()->GetBinWidth(ybin));
      double pass_err, fail_err;
      hResPass->SetBinContent(xbin, ybin, extract_nz(massHists_pass[mapBin], pass_err, bkgmode, Form("%s/x-%i_y-%i_pass.png", fitdir.Data(), xbin, ybin), title));
      hResFail->SetBinContent(xbin, ybin, extract_nz(massHists_fail[mapBin], fail_err, bkgmode, Form("%s/x-%i_y-%i_fail.png", fitdir.Data(), xbin, ybin), title));
      hResPass->SetBinError(xbin, ybin, pass_err);
      hResFail->SetBinError(xbin, ybin, fail_err);
    }
  }

  TH2F* hResRatio = (TH2F*) hResPass->Clone("hRatio");
  TH2F* hResTotal = (TH2F*) hResPass->Clone("hTotal");
  hResTotal->Add(hResFail);
  hResRatio->Divide(hResTotal);
  hResRatio->SetTitle("Efficiency");

  ///////////////////////////////////////
  // Plot and save the results
  ///////////////////////////////////////

  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c", "c", 1500, 700);
  c->Divide(3,1);
  TVirtualPad* pad;
  pad = c->cd(1);
  pad->SetLeftMargin(0.13);
  pad->SetRightMargin(0.13);
  hPass->Draw("colz");
  pad->SetLogz();
  pad = c->cd(2);
  pad->SetLeftMargin(0.13);
  pad->SetRightMargin(0.13);
  hFail->Draw("colz");
  pad->SetLogz();
  pad = c->cd(3);
  pad->SetLeftMargin(0.13);
  pad->SetRightMargin(0.13);
  hRatio->Draw("colz");
  hRatio->GetZaxis()->SetRangeUser(0., 1.);

  gSystem->Exec("[ ! -d figures ] && mkdir figures");
  gSystem->Exec("[ ! -d rootfiles ] && mkdir rootfiles");
  c->SaveAs(Form("figures/eff_%i_%i_cc.png", isMC, year));
  delete c;

  c = new TCanvas("c", "c", 1500, 700);
  c->Divide(3,1);
  pad = c->cd(1);
  pad->SetLeftMargin(0.13);
  pad->SetRightMargin(0.13);
  hResPass->Draw("colz");
  pad->SetLogz();
  pad = c->cd(2);
  pad->SetLeftMargin(0.13);
  pad->SetRightMargin(0.13);
  hResFail->Draw("colz");
  pad->SetLogz();
  pad = c->cd(3);
  pad->SetLeftMargin(0.13);
  pad->SetRightMargin(0.13);
  hResRatio->Draw("colz");
  hResRatio->GetZaxis()->SetRangeUser(0., 1.);
  c->SaveAs(Form("figures/eff_%i_%i.png", isMC, year));
  delete c;

  c = new TCanvas("c", "c", 1000, 700);
  c->Divide(2,1);
  TLegend* leg = new TLegend(0.6, 0.8, 0.9, 0.9);
  leg->AddEntry(hTagPt, "Tag", "L");
  leg->AddEntry(hProbePt, "Probe", "L");
  pad = c->cd(1);
  hTagPt->SetLineWidth(2);
  hTagPt->SetLineColor(kRed);
  hTagPt->Draw("hist");
  hProbePt->SetLineWidth(2);
  hProbePt->SetLineColor(kBlue);
  hProbePt->Draw("hist same");
  leg->Draw();
  pad = c->cd(2);
  hTagEta->SetLineWidth(2);
  hTagEta->SetLineColor(kRed);
  hTagEta->Draw("hist");
  hProbeEta->SetLineWidth(2);
  hProbeEta->SetLineColor(kBlue);
  hProbeEta->Draw("hist same");
  leg->Draw();
  c->SaveAs(Form("figures/kin_%i_%i.png", isMC, year));
  delete c;

  TFile* fout = new TFile(Form("rootfiles/efficiencies_%i_%i.root", isMC, year), "RECREATE");
  hRatio->Write();
  hResRatio->Write();
  fout->Write();
  fout->Close();
}
