//Fit the background templates to the same-sign data
#include "bemu_fit_bkg_mc.C"
#include "bemu_bkg_functions.C"
#include "util.hh"

TString tag_           =    ""; //tag for output figure directory

vector<int> years_;
int         set_  ;

//---------------------------------------------------------------------------------------------------------------------------------------
// Figure directory
const char* get_fig_dir() {
  TString years_s = Form("%i", years_[0]);
  for(unsigned i = 1; i < years_.size(); ++i) years_s += Form("_%i", years_[i]);
  return Form("plots/latest_production/%s/zmumu_fits_%i%s", years_s.Data(), set_, tag_.Data());
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Chi^2 from data - PDF
double get_chi_square_from_diff(TH1* h) {
  double chi_sq = 0.;
  for(int ibin = 1; ibin <= h->GetNbinsX(); ++ibin) {
    const int err = h->GetBinError(ibin);
    if(err <= 0.) continue;
    chi_sq += pow(h->GetBinContent(ibin)/err, 2);
  }
  return chi_sq;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Retrieve Z->mumu from a file
TH1* get_zmumu(TFile* f) {
  //Get the MC background stack
  THStack* stack = (THStack*) f->Get("hstack");
  if(!stack) return nullptr;

  //Replace background distributions where needed
  TH1* h_zmumu = nullptr;
  for(int ihist = 0; ihist < stack->GetNhists(); ++ihist) {
    TH1* h = (TH1*) stack->GetHists()->At(ihist);
    if(TString(h->GetName()).Contains("Z->ee")) {
      h_zmumu = (TH1*) h->Clone("h_zmumu");
      break;
    }
  }
  return h_zmumu;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Main function
int fit_zmumu(int set = 13, vector<int> years = {2016,2017,2018}) {

  if(years.size() == 0) return -1;
  years_ = years;
  set_ = set;
  TString years_s = Form("%i", years_[0]);
  for(unsigned i = 1; i < years_.size(); ++i) years_s += Form("_%i", years_[i]);

  //for output figures
  const char* fig_dir = get_fig_dir();
  gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s", fig_dir, fig_dir));
  gStyle->SetOptFit(1110);

  //Open the MC histogram file
  TFile* fMC_SS = TFile::Open(Form("histograms/zemu_lepm_%i_%s.hist", set+1000, years_s.Data()), "READ");
  TFile* fMC_OS = TFile::Open(Form("histograms/zemu_lepm_%i_%s.hist", set     , years_s.Data()), "READ");
  if(!fMC_SS) return -1;

  //Get the data
  TH1* data = (TH1*) fMC_SS->Get("hdata");
  if(!data) return -1;

  //don't blind the data --> same sign region isn't sensitive
  const double xmin(70.), xmax(110.);
  const double blind_min(86.), blind_max(96.);

  //trim not used regions
  data = make_safe(data, xmin, xmax);

  //Get the Z->mumu MC
  TH1* h_zmumu_ss = get_zmumu(fMC_SS);
  TH1* h_zmumu_os = get_zmumu(fMC_OS);
  if(!h_zmumu_ss || !h_zmumu_os) {
    cout << "Unable to retrieve Z->mumu histograms!\n";
    return 2;
  }

  //Create the RooFit objects
  RooRealVar obs("obs", "M_{e#mu}", 90., xmin, xmax);
  obs.setBins(data->GetNbinsX());
  obs.setRange("full", xmin, xmax);
  obs.setRange("LowSideband", xmin, blind_min);
  obs.setRange("HighSideband", blind_max, xmax);
  obs.setRange("BlindRegion", blind_min, blind_max);
  RooDataHist data_hist("data_hist", "Data", RooArgList(obs), data);
  const double ndata = data->Integral();

  //Create a background PDF for both the Drell-Yan/tau tau/WW/ttbar vs Z->mumu
  RooAbsPdf* bkg_pdf = create_chebychev(obs, 2, set)        ; bkg_pdf->SetName("bkg_pdf"); bkg_pdf->SetTitle("Background PDF");
  RooAbsPdf* zmm_pdf = create_zmumu    (obs, set, set == 11); zmm_pdf->SetName("zmm_pdf"); zmm_pdf->SetTitle("Z->#mu#mu PDF");
  RooRealVar* n_bkg = new RooRealVar("n_bkg", "N(bkg)"      , 1., 0., 1.5*ndata);
  RooRealVar* n_zmm = new RooRealVar("n_zmm", "N(Z->#mu#mu)", 1., 0., 1.5*ndata);
  RooAddPdf* tot_pdf = new RooAddPdf("tot_pdf", "Total PDF", RooArgList(*zmm_pdf, *bkg_pdf), RooArgList(*n_zmm, *n_bkg));

  //initialize the background PDF parameters
  RooArgList bkg_vars(*(bkg_pdf->getVariables()));
  ((RooRealVar*) bkg_vars.at(0))->setVal(-0.524);
  ((RooRealVar*) bkg_vars.at(1))->setVal( 0.054);

  //Set the background fractions near MC predictions
  if(set == 11) {
    n_bkg->setVal(0.9*ndata);
    n_zmm->setVal(0.1*ndata);
  } else if(set == 12) {
    n_bkg->setVal(0.3*ndata);
    n_zmm->setVal(0.7*ndata);
  } else if(set == 13) {
    n_bkg->setVal(0.5*ndata);
    n_zmm->setVal(0.5*ndata);
  }

  // Fit the data
  auto fit_res = tot_pdf->fitTo(data_hist, RooFit::PrintLevel(-1), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1), RooFit::Range("full"), RooFit::Save());
  if(fit_res->status() != 0) { //fit failed
    cout << "##################################################" << endl
         << "## Fit failed!\n"
         << "##################################################" << endl;
    return -1;
  }

  //Plot the results
  {
    gStyle->SetOptStat(0);
    TLatex label;
    label.SetNDC();
    label.SetTextFont(72);
    label.SetTextAlign(13);
    label.SetTextAngle(0);
    label.SetTextSize(0.04);
    auto c = new TCanvas("c","c", 1200, 1200);
    TPad* pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1. ); pad1->Draw();
    TPad* pad2 = new TPad("pad2", "pad2", 0., 0. , 1., 0.3); pad2->Draw();
    pad2->SetBottomMargin(0.13); pad2->SetTopMargin(0.02);

    pad1->cd();
    auto frame = obs.frame();
    data_hist.plotOn(frame);
    tot_pdf->plotOn(frame);
    int nparams = tot_pdf->getParameters(data_hist)->getSize();
    int nentries = data_hist.numEntries();
    int ndof = nentries - nparams;
    double chi_sq = frame->chiSquare()*nentries;
    tot_pdf->plotOn(frame, RooFit::Components("bkg_pdf"), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
    tot_pdf->plotOn(frame, RooFit::Components("zmm_pdf"), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
    frame->Draw();
    auto h_diff = data_pdf_diff(&data_hist, tot_pdf, obs);
    // double chi_sq = get_chi_square_from_diff(h_diff);
    label.DrawLatex(0.60, 0.87, Form("#chi^{2}/DOF = %.2f; p = %.3f", chi_sq/ndof, TMath::Prob(chi_sq, ndof)));
    cout << "Data: chi_sq = " << chi_sq << ", ndof = " << ndof << endl;
    cout << "Var 1: " << ((RooRealVar*) RooArgList(*(zmm_pdf->getVariables())).at(5))->GetName() << endl
         << "Var 2: " << ((RooRealVar*) RooArgList(*(zmm_pdf->getVariables())).at(6))->GetName() << endl;
    label.DrawLatex(0.53, 0.82, Form("#mu = %.2f#pm%.2f; #sigma = %.2f#pm%.2f",
                                     ((RooRealVar*) RooArgList(*(zmm_pdf->getVariables())).at(5))->getVal(),
                                     ((RooRealVar*) RooArgList(*(zmm_pdf->getVariables())).at(5))->getError(),
                                     ((RooRealVar*) RooArgList(*(zmm_pdf->getVariables())).at(6))->getVal(),
                                     ((RooRealVar*) RooArgList(*(zmm_pdf->getVariables())).at(6))->getError()));

    pad2->cd();
    h_diff->SetTitle(""); h_diff->SetXTitle(""); h_diff->SetYTitle("");
    h_diff->GetXaxis()->SetLabelSize(0.08);
    h_diff->GetYaxis()->SetLabelSize(0.08);
    h_diff->SetMarkerStyle(20); h_diff->SetMarkerSize(0.8);
    h_diff->Draw("E1");
    TLine line(xmin, 0., xmax, 0.); line.SetLineWidth(2); line.SetLineColor(kBlack); line.SetLineStyle(kDashed);
    line.Draw("same");

    c->SaveAs(Form("%s/fit.png", get_fig_dir()));
    delete frame;

    //Compare the fit to the SS Z->mumu MC
    pad1->cd();
    RooDataHist zmm_hist_ss("zmm_hist_ss", "Z->#mu#mu SS MC", RooArgList(obs), h_zmumu_ss);
    frame = obs.frame();
    zmm_hist_ss.plotOn(frame);
    zmm_pdf->plotOn(frame);
    nparams = zmm_pdf->getParameters(zmm_hist_ss)->getSize();
    nentries = zmm_hist_ss.numEntries();
    ndof = nentries - nparams;
    frame->Draw();
    h_diff = data_pdf_diff(&zmm_hist_ss, zmm_pdf, obs);
    chi_sq = get_chi_square_from_diff(h_diff);
    label.DrawLatex(0.60, 0.87, Form("#chi^{2}/DOF = %.2f; p = %.3f", chi_sq/ndof, TMath::Prob(chi_sq, ndof)));
    cout << "SS MC: chi_sq = " << chi_sq << ", ndof = " << ndof << endl;

    pad2->cd();
    h_diff->SetTitle(""); h_diff->SetXTitle(""); h_diff->SetYTitle("");
    h_diff->GetXaxis()->SetLabelSize(0.08);
    h_diff->GetYaxis()->SetLabelSize(0.08);
    h_diff->SetMarkerStyle(20); h_diff->SetMarkerSize(0.8);
    h_diff->Draw("E1");
    line.Draw("same");

    c->SaveAs(Form("%s/fit_vs_mc_ss.png", get_fig_dir()));
    delete frame;


    //Compare the fit to the OS Z->mumu MC
    pad1->cd();
    RooDataHist zmm_hist_os("zmm_hist_os", "Z->#mu#mu OS MC", RooArgList(obs), h_zmumu_os);
    frame = obs.frame();
    zmm_hist_os.plotOn(frame);
    zmm_pdf->plotOn(frame);
    nparams = zmm_pdf->getParameters(zmm_hist_os)->getSize();
    nentries = zmm_hist_os.numEntries();
    ndof = nentries - nparams;
    frame->Draw();
    h_diff = data_pdf_diff(&zmm_hist_os, zmm_pdf, obs);
    chi_sq = get_chi_square_from_diff(h_diff);
    label.DrawLatex(0.60, 0.87, Form("#chi^{2}/DOF = %.2f; p = %.3f", chi_sq/ndof, TMath::Prob(chi_sq, ndof)));
    cout << "OS MC: chi_sq = " << chi_sq << ", ndof = " << ndof << endl;

    pad2->cd();
    h_diff->SetTitle(""); h_diff->SetXTitle(""); h_diff->SetYTitle("");
    h_diff->GetXaxis()->SetLabelSize(0.08);
    h_diff->GetYaxis()->SetLabelSize(0.08);
    h_diff->SetMarkerStyle(20); h_diff->SetMarkerSize(0.8);
    h_diff->Draw("E1");
    line.Draw("same");

    c->SaveAs(Form("%s/fit_vs_mc_os.png", get_fig_dir()));
    delete frame;

    delete c;
  }

  //get the data rates
  printf("Fit yields:\n");
  n_bkg->Print();
  n_zmm->Print();
  double err, yield(h_zmumu_ss->IntegralAndError(1, h_zmumu_ss->GetNbinsX(), err));
  double fit_err(n_zmm->getError()), fit_yield(n_zmm->getVal());
  printf("MC yield: %.1f +- %.1f\n --> Ratio = %.3f +- %.3f\n", yield, err,
         fit_yield/yield, fit_yield/yield*sqrt(pow(err/yield,2)+pow(fit_err/fit_yield,2)));

  cout << "----------------------------------------------------------------------------" << endl;
  auto vars = zmm_pdf->getVariables();
  auto itr = vars->createIterator();
  auto var = itr->Next();
  while(var) {
    if(TString(var->GetName()) != obs.GetName()) {
      var->Print();
    }
    var = itr->Next();
  }
  cout << "----------------------------------------------------------------------------" << endl;

  bkg_pdf->Print("tree");
  fMC_SS->Close();
  fMC_OS->Close();
  return 0;
}
