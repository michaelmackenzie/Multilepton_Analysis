//Script to define scale factors to apply to MC
//to correct the MET recoil
#include "histograms/datacards.C"

//use the dataplotter to manage normalizations and initializations
DataPlotter* dataplotter_ = 0;
int verbose_           = 0;

TString selection_;

//----------------------------------------------------------------------------------------------------------------------------------
TF1* fit_hist(TH1* hist, int mode) {
  return nullptr;
}

//----------------------------------------------------------------------------------------------------------------------------------
// isdata: 0 = MC; 1 = data; -1 = background MC
TH1* get_histogram(TString hist, TString type, int set, int isdata) {
  TH1* h = nullptr;
  if(isdata == 1) h = dataplotter_->get_data(hist, type, set);
  else {
    THStack* hstack = dataplotter_->get_stack(hist, type, set);
    if(!hstack || hstack->GetNhists() == 0) return nullptr;
    TH1* hBkg = nullptr;
    for(int ihist = 0; ihist < hstack->GetNhists(); ++ihist) {
      TH1* hloop = (TH1*) hstack->GetHists()->At(ihist);
      TString label = hloop->GetTitle();
      if(label.Contains("ZJets")) { //Drell-Yan
        if(!h) h = hloop;
        else h->Add(hloop);
      } else {
        if(!hBkg) hBkg = hloop;
        else hBkg->Add(hloop);
      }
    }
    if(isdata == -1) h = hBkg;
    if(!h) return nullptr;
  }
  if(!h) return nullptr;
  h = (TH1*) h->Clone(Form("%s_%s_%i_%s", hist.Data(), type.Data(), set, (isdata == 1) ? "data" : ((isdata == -1) ? "bkg" : "dy")));
  return h;
}

//-------------------------------------------------------------------------------------------------------------------------------
Int_t make_figure(TString hist, TString type, int set, TString path) {
  TH1* hData = get_histogram(hist, type, set,  1);
  TH1* hMC   = get_histogram(hist, type, set,  0);
  TH1* hBkg  = get_histogram(hist, type, set, -1);
  if(!hData || !hMC || !hBkg) {
    cout << "Histograms for " << hist.Data() << "/" << type.Data() << "/" << set
         << " not found\n";
    return 1;
  }
  if(hData->Integral() <= 0. || hMC->Integral() <= 0. || hBkg->Integral() <= 0.) {
    cout << "No data found in " << hist.Data() << "/" << type.Data() << "/" << set
         << " histograms\n";
    return 2;
  }
  hData->Add(hBkg, -1.); //subtract off the non-Drell-Yan backgrounds

  ///////////////////////
  //  Draw plots

  TCanvas* c = new TCanvas("c", "c", 1000, 1100);
  TPad* pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1.0); pad1->Draw();
  TPad* pad2 = new TPad("pad2", "pad2", 0., 0.0, 1., 0.3); pad2->Draw();
  pad1->SetBottomMargin(0.05); pad2->SetTopMargin(0.03);

  pad1->cd();
  pad1->SetGrid();
  hMC->SetTitle("");
  hMC->Draw("hist");
  hData->Draw("E1 same");

  pad2->cd();
  pad2->SetGrid();
  auto hRatio = (TH1*) hData->Clone(Form("hRatio_%s", hist.Data()));
  hRatio->Divide(hMC);
  hRatio->Draw("E1");
  hRatio->SetTitle("");
  hRatio->GetYaxis()->SetTitle("Data/MC");
  hRatio->GetYaxis()->SetTitleSize(0.13);
  hRatio->GetYaxis()->SetTitleOffset(0.30);
  hRatio->GetYaxis()->SetLabelSize(0.08);
  hRatio->GetXaxis()->SetLabelSize(0.08);
  hRatio->GetYaxis()->SetRangeUser(0.5, 1.5);

  TLine* line = new TLine(hRatio->GetXaxis()->GetXmin(), 1., hRatio->GetXaxis()->GetXmax(), 1.);
  line->SetLineWidth(2);
  line->SetLineColor(kRed);
  line->Draw("same");

  c->SaveAs(Form("%s/%s_%i.png", path.Data(), hist.Data(), 1000*(set/1000) + set%100));

  // const double n_data = hData_U1->Integral() + hData_U1->GetBinContent(0) + hData_U1->GetBinContent(hData_U1->GetNbinsX()+1);
  // const double n_mc   = hMC_U1->Integral() + hMC_U1->GetBinContent(0) + hMC_U1->GetBinContent(hMC_U1->GetNbinsX()+1);
  // const double data_mc_scale = n_data / n_mc;
  // cout << "Data / MC = " << n_data  << " / " << n_mc << " = " << data_mc_scale << endl;
  delete c;
  delete hRatio;
  delete line;

  return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------
//initialize the files and scales using a DataPlotter
Int_t initialize_plotter(TString base, int year) {
  if(dataplotter_) delete dataplotter_;
  dataplotter_ = new DataPlotter();
  dataplotter_->include_qcd_ = 0;
  dataplotter_->include_misid_ = 0;
  dataplotter_->verbose_ = max(0, verbose_ - 1);
  dataplotter_->qcd_scale_ = 1.;
  years_ = {year};

  useEmbed_ = 0; //correction factors for DY MC, not embedding
  // hist_tag_ = "met"; //tag for the RecoilHistMaker
  hist_tag_ = "clfv"; //tag for the RecoilHistMaker

  std::vector<dcard> cards;
  get_datacards(cards, selection_, true);

  CrossSections xs(useUL_, ZMode_); //cross section handler
  dataplotter_->set_luminosity(xs.GetLuminosity(year));
  if(verbose_ > 0) cout << "--- Dataplotter luminosity for " << year << " = " << dataplotter_->lum_ << endl;

  int status(0);
  for(auto card : cards)
    status += dataplotter_->add_dataset(card);

  status += dataplotter_->init_files();
  return status;
}

//----------------------------------------------------------------------------------------------------------------------------------
Int_t scale_factors(bool useMuon = true, int set = 7, int year = 2016, TString path = "nanoaods_met") {
  hist_dir_ = path;
  selection_ = (useMuon) ? "mumu" : "ee";

  //Get data histogram
  TString baseName = "clfv_";
  baseName += (useMuon) ? "mumu_clfv_" : "ee_clfv_";
  baseName += year;
  baseName += "_";

  if(verbose_ > 0) std::cout << "Initializing the dataplotter" << std::endl;
  if(initialize_plotter(baseName, year)) {
    cout << "Dataplotter initialization failed!\n";
    return 1;
  }

  int setAbs = (useMuon) ? set+CLFVHistMaker::kMuMu : set+CLFVHistMaker::kEE;

  ///////////////////////
  //  Draw plots

  TString base = Form("figures/%s_%i", (useMuon) ? "mumu" : "ee", year);
  gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s", base.Data(), base.Data()));
  gStyle->SetOptStat(0);

  Int_t status(0);

  status += make_figure("metu1"  , "event", setAbs, base);
  status += make_figure("metu2"  , "event", setAbs, base);
  status += make_figure("met"    , "event", setAbs, base);
  status += make_figure("met"    , "event", (setAbs - set + 8), base);
  status += make_figure("leppt"  , "event", setAbs, base);
  status += make_figure("zpt5"   , "event", setAbs, base);
  status += make_figure("njets20", "event", setAbs, base);

  return status;
}
