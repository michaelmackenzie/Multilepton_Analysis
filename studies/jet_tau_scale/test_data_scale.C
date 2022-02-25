//Test Data derived scale factors on Data

//use the dataplotter to manage normalizations and initializations
DataPlotter* dataplotter_ = 0;
int verbose_ = 0;
int year_;
TString selection_;

bool useTauRegion_ = true; //use etau or mutau same-sign region for correction
int splitWJ_ = 1; //whether to use jet binned w+jets samples or not
int splitDY_ = 1; //whether to use ll split DY50 samples or not

//initialize the files and scales using a DataPlotter
Int_t initialize_plotter(TString base, TString path, int year) {
  if(dataplotter_) delete dataplotter_;
  dataplotter_ = new DataPlotter();
  dataplotter_->include_qcd_ = 0;
  dataplotter_->include_misid_ = 0;
  dataplotter_->verbose_ = verbose_;

  typedef DataCard_t dcard;
  std::vector<dcard> cards;
  //card constructor:    filepath,                             name,                  label,              isData, xsec,  isSignal
  CrossSections xs; //cross section handler
  dataplotter_->set_luminosity(xs.GetLuminosity(year));
  TString dyname = (year == 2017) ? "DY50-ext" : "DY50-amc";
  if(splitDY_) {
    cards.push_back(dcard(path+base+dyname+"-1.hist"        , dyname+"-1"          , "Drell-Yan", false, xs.GetCrossSection("DY50"               ), false, year));
    cards.push_back(dcard(path+base+dyname+"-2.hist"        , dyname+"-2"          , "Drell-Yan", false, xs.GetCrossSection("DY50"               ), false, year));
  } else {
    cards.push_back(dcard(path+base+dyname+".hist"          , dyname               , "Drell-Yan", false, xs.GetCrossSection("DY50"               ), false, year));
  }
  cards.push_back(dcard(path+base+"SingleAntiToptW.hist"    , "SingleAntiToptW"    , "Top"      , false, xs.GetCrossSection("SingleAntiToptW"    ), false, year, kYellow-7));
  cards.push_back(dcard(path+base+"SingleToptW.hist"        , "SingleToptW"        , "Top"      , false, xs.GetCrossSection("SingleToptW"        ), false, year, kYellow-7));
  cards.push_back(dcard(path+base+"ttbarToSemiLeptonic.hist", "ttbarToSemiLeptonic", "Top"      , false, xs.GetCrossSection("ttbarToSemiLeptonic"), false, year, kYellow-7));
  cards.push_back(dcard(path+base+"ttbarlnu.hist"           , "ttbarlnu"           , "Top"      , false, xs.GetCrossSection("ttbarlnu"           ), false, year, kYellow-7));
  cards.push_back(dcard(path+base+"WWW.hist"                , "WWW"                , "Other VB" , false, xs.GetCrossSection("WWW"                ), false, year, kViolet-9));
  cards.push_back(dcard(path+base+"WZ.hist"                 , "WZ"                 , "Other VB" , false, xs.GetCrossSection("WZ"                 ), false, year, kViolet-9));
  cards.push_back(dcard(path+base+"ZZ.hist"                 , "ZZ"                 , "Other VB" , false, xs.GetCrossSection("ZZ"                 ), false, year, kViolet-9));
  cards.push_back(dcard(path+base+"WW.hist"                 , "WW"                 , "Other VB" , false, xs.GetCrossSection("WW"                 ), false, year, kViolet-9));
  double wxs = xs.GetCrossSection("Wlnu");
  double ngen1 = xs.GetGenNumber("Wlnu", year);
  double ngen2 = xs.GetGenNumber("Wlnu-ext", year);
  if(splitWJ_) {
    if(year != 2018) {
      cards.push_back(dcard(path+base+"Wlnu-0.hist"         , "Wlnu"               , "Other VB"          , false, wxs*(ngen1)/(ngen1+ngen2)                , false, year));
      cards.push_back(dcard(path+base+"Wlnu-ext-0.hist"     , "Wlnu"               , "Other VB"          , false, wxs*(ngen2)/(ngen1+ngen2)                , false, year));
    } else {
      cards.push_back(dcard(path+base+"Wlnu-0.hist"         , "Wlnu"               , "Other VB"          , false, wxs                                      , false, year));
    }
    cards.push_back(dcard(path+base+"Wlnu-1J.hist"          , "Wlnu-1J"            , "Other VB"          , false, xs.GetCrossSection("Wlnu-1J"            ), false, year));
    cards.push_back(dcard(path+base+"Wlnu-2J.hist"          , "Wlnu-2J"            , "Other VB"          , false, xs.GetCrossSection("Wlnu-2J"            ), false, year));
    cards.push_back(dcard(path+base+"Wlnu-3J.hist"          , "Wlnu-3J"            , "Other VB"          , false, xs.GetCrossSection("Wlnu-3J"            ), false, year));
    if(year != 2017) {
      cards.push_back(dcard(path+base+"Wlnu-4J.hist"        , "Wlnu-4J"            , "Other VB"          , false, xs.GetCrossSection("Wlnu-4J"            ), false, year));
    } else {
      cards.push_back(dcard(path+base+"Wlnu-4.hist"         , "Wlnu"               , "Other VB"          , false, wxs*(ngen1)/(ngen1+ngen2)                , false, year));
      cards.push_back(dcard(path+base+"Wlnu-ext-4.hist"     , "Wlnu"               , "Other VB"          , false, wxs*(ngen2)/(ngen1+ngen2)                , false, year));
    }
  } else {
    if(year != 2018) {
      cards.push_back(dcard(path+base+"Wlnu.hist"             , "Wlnu"               , "Other VB" , false, wxs*(ngen1)/(ngen1+ngen2)                   , false, year));
      cards.push_back(dcard(path+base+"Wlnu-ext.hist"         , "Wlnu"               , "Other VB" , false, wxs*(ngen2)/(ngen1+ngen2)                   , false, year));
    } else {
      cards.push_back(dcard(path+base+"Wlnu.hist"             , "Wlnu"               , "Other VB" , false, wxs                                         , false, year));
    }
  }
  if(!base.Contains("etau") && !base.Contains("ee"))
    cards.push_back(dcard(path+base+"SingleMu.hist", "SingleMu", "Data", true, 1., false, year));
  if(!base.Contains("mutau") && !base.Contains("mumu"))
    cards.push_back(dcard(path+base+"SingleEle.hist", "SingleEle", "Data", true, 1., false, year));

  int status(0);
  for(auto card : cards)
    status += dataplotter_->add_dataset(card);

  status += dataplotter_->init_files();
  return status;
}

TH1D* get_misid_hist(TString hist, TString type, int set) {
  THStack* hstack = dataplotter_->get_stack(hist, type, set);
  if(!hstack) return NULL;
  TH1D* hbkg = (TH1D*) hstack->GetStack()->Last();
  TH1D* hMisID = dataplotter_->get_data(hist,type,set);
  hMisID->Add(hbkg, -1.);
  double norm = hMisID->Integral();
  for(int ibin = 1; ibin <= hMisID->GetNbinsX(); ++ibin) {
    if(hMisID->GetBinContent(ibin) < 0.) hMisID->SetBinContent(ibin, 0.);
  }
  hMisID->Scale(norm/hMisID->Integral());
  hMisID->SetName(Form("hmisid_%s_%s_%i", hist.Data(), type.Data(), set));
  hMisID->SetTitle(dataplotter_->misid_label_.Data());
  hMisID->SetLineColor(dataplotter_->misid_color_);
  hMisID->SetLineWidth(2);
  hMisID->SetFillColor(dataplotter_->misid_color_);
  hMisID->SetFillStyle(1001);
  return hMisID;
}

TCanvas* make_2d_canvas(int set1, int set2, PlottingCard_t card, bool print, TH2D* &hData, TH2D* &hMisID) {
  dataplotter_->include_qcd_ = 0;
  dataplotter_->include_misid_ = 0;
  TH2D* hbkg = dataplotter_->get_background_2D(card.hist_, card.type_, set1); //tight MC backgrounds
  if(!hbkg) return NULL;
  dataplotter_->include_qcd_ = 0;
  dataplotter_->include_misid_ = 1;
  hMisID = dataplotter_->get_data_2D(card.hist_, card.type_, set2); //jet --> tau estimate
  if(!hMisID) return NULL;
  hData = dataplotter_->get_data_2D(card.hist_, card.type_, set1); //tight Data
  if(!hData) return NULL;
  hData->Add(hbkg, -1.);
  double norm = hData->Integral();
  for(int xbin = 1; xbin < hData->GetNbinsX(); ++xbin) {
    for(int ybin = 1; ybin < hData->GetNbinsY(); ++ybin) {
      if(hData->GetBinContent(xbin, ybin) < 0.) hData->SetBinContent(xbin, ybin, 0.);
    }
  }
  hbkg = dataplotter_->get_background_2D(card.hist_, card.type_, set2); //loose MC backgrounds
  // hMisID->Scale(norm/hMisID->Integral());
  hMisID->Add(hbkg, -1.);
  norm = hMisID->Integral();
  for(int xbin = 1; xbin < hMisID->GetNbinsX(); ++xbin) {
    for(int ybin = 1; ybin < hMisID->GetNbinsY(); ++ybin) {
      if(hMisID->GetBinContent(xbin, ybin) < 0.) hMisID->SetBinContent(xbin, ybin, 0.);
    }
  }
  hMisID->Scale(norm/hMisID->Integral());
  TCanvas* c = new TCanvas("c1","c1",1500,600);
  c->Divide(3,1);
  TVirtualPad* pad;
  pad = c->cd(1);
  pad->SetRightMargin(0.12);
  hData->SetTitle("Data");
  hData->SetXTitle("p_T");
  hData->SetYTitle("#DeltaR");
  hData->Draw("colz");
  if(card.xmin_ < card.xmax_) hData->GetXaxis()->SetRangeUser(card.xmin_, card.xmax_);
  if(card.ymin_ < card.ymax_) hData->GetYaxis()->SetRangeUser(card.ymin_, card.ymax_);
  pad = c->cd(2);
  pad->SetRightMargin(0.12);
  hMisID->SetTitle("Jet#rightarrow#tau estimate");
  hMisID->SetXTitle("p_T");
  hMisID->SetYTitle("#DeltaR");
  hMisID->Draw("colz");
  if(card.xmin_ < card.xmax_) hMisID->GetXaxis()->SetRangeUser(card.xmin_, card.xmax_);
  if(card.ymin_ < card.ymax_) hMisID->GetYaxis()->SetRangeUser(card.ymin_, card.ymax_);
  pad = c->cd(3);
  pad->SetRightMargin(0.12);
  TH2D* hRatio = (TH2D*) hData->Clone("hRatio");
  hRatio->SetTitle("Data / Estimate");
  hRatio->Divide(hMisID);
  hRatio->Draw("colz");
  if(hRatio->GetMaximum() > 1.5) hRatio->GetZaxis()->SetRangeUser(0.5, 1.5);
  if(print) {
    TString dir = Form("figures/test_data_%s_%i_%i/", selection_.Data(), year_, set1);
    gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s", dir.Data(), dir.Data()));
    c->Print(Form("%s%s_%s.png", dir.Data(), card.type_.Data(), card.hist_.Data()));
  }
  return c;
}

  //subtract MC tight estimate from Data
TCanvas* make_canvas(int set1, int set2, PlottingCard_t card, bool print, TH1D* &hData, TH1D* &hMisID) {
  dataplotter_->plot_data_ = 0;
  dataplotter_->include_qcd_ = 0;
  dataplotter_->include_misid_ = 0;
  dataplotter_->rebinH_ = card.rebin_;
  dataplotter_->doStatsLegend_ = false;
  THStack* hstack = dataplotter_->get_stack(card.hist_, card.type_, set1); //tight MC backgrounds
  if(!hstack) return NULL;
  hMisID = get_misid_hist(card.hist_, card.type_, set2); //jet --> tau estimate
  if(!hMisID) return NULL;
  hData = dataplotter_->get_data(card.hist_, card.type_, set1); //tight Data
  if(!hData) return NULL;

  //subtract MC tight estimate from Data
  hData->Add((TH1D*) hstack->GetStack()->Last(), -1.);
  double norm = hData->Integral();
  for(int ibin = 1; ibin < hData->GetNbinsX(); ++ibin) {
    if(hData->GetBinContent(ibin) < 0.) hData->SetBinContent(ibin, 0.);
  }
  hData->Scale(norm/hData->Integral());

  TCanvas* c = new TCanvas("c1","c1",1200,800);
  TPad* pad1 = new TPad("pad1", "pad1", 0., 0.35, 1., 1.0 );
  TPad* pad2 = new TPad("pad2", "pad2", 0., 0.0 , 1., 0.35);
  pad1->SetBottomMargin(0.03);
  pad1->SetTopMargin(0.03);
  pad2->SetTopMargin(0.03);
  pad2->SetBottomMargin(0.3);
  pad1->Draw(); pad2->Draw();
  pad1->cd();
  hData->Draw("E1");
  hMisID->Draw("hist same");
  hData->GetXaxis()->SetRangeUser(card.xmin_, card.xmax_);
  hData->GetYaxis()->SetRangeUser(0.1, 1.1*max(hMisID->GetMaximum(), hData->GetMaximum()));
  hData->Draw("E1 sames");
  hData->GetXaxis()->SetLabelSize(0.);
  hData->GetYaxis()->SetLabelSize(0.06);
  hData->GetXaxis()->SetTitleSize(0.157);
  hData->GetYaxis()->SetTitleSize(0.157);
  hData->SetTitle("");

  TLegend* leg = new TLegend(0.61, 0.65, 0.9, 0.97);
  leg->AddEntry(hData, "Tight Data - MC #tau");
  leg->AddEntry(hMisID);
  leg->SetTextSize(0.06);
  leg->Draw();

  pad1->SetGrid();

  //Add labels with the event number and predicted number
  TLatex datalabel;
  datalabel.SetNDC();
  datalabel.SetTextFont(72);
  datalabel.SetTextSize(0.07);
  datalabel.SetTextAlign(13);
  datalabel.DrawLatex(0.65, 0.62,  Form("Data       : %9.1f", hData->Integral()));
  datalabel.DrawLatex(0.65, 0.54 , Form("Estimate: %9.1f", hMisID->Integral()));

  //Draw the ratio plot
  pad2->cd();
  TH1D* hratio = (TH1D*) hData->Clone(Form("hRatio_%s", hData->GetName()));
  hratio->Divide(hMisID);
  hratio->Draw("E1");
  pad2->Update();

  TString xtitle = card.hist_;
  xtitle.ReplaceAll("jettau", "");
  xtitle.ReplaceAll("1", ""); xtitle.ReplaceAll("2", ""); xtitle.ReplaceAll("3", "");
  bool addone = xtitle.Contains("one");
  bool addtwo = xtitle.Contains("two");
  xtitle.ReplaceAll("one","");
  xtitle.ReplaceAll("two","");
  xtitle.ReplaceAll("eta", "#eta"); xtitle.ReplaceAll("phi", "#phi"); xtitle.ReplaceAll("pt", "p_{T}");
  if(addone) xtitle = xtitle + "^{1}";
  if(addtwo) xtitle = xtitle + "^{2}";
  xtitle.ReplaceAll("taus", "#tau ");
  xtitle.ReplaceAll("dm", "Decay Mode");
  hratio->GetYaxis()->SetRangeUser(0.5, 1.5);
  hratio->GetXaxis()->SetRangeUser(card.xmin_, card.xmax_);
  hratio->SetXTitle(xtitle.Data());
  hratio->SetYTitle("Data / MC");
  hratio->SetTitle("");
  hratio->GetXaxis()->SetTitleOffset(0.75);
  hratio->GetYaxis()->SetTitleOffset(0.4);
  hratio->GetXaxis()->SetLabelSize(0.1);
  hratio->GetYaxis()->SetLabelSize(0.09);
  hratio->GetXaxis()->SetTitleSize(0.157);
  hratio->GetYaxis()->SetTitleSize(0.11);

  double xmin = (card.xmin_ < card.xmax_) ? card.xmin_ : hratio->GetBinLowEdge(1);
  double xmax = (card.xmin_ < card.xmax_) ? card.xmax_ : hratio->GetBinLowEdge(hratio->GetNbinsX()) + hratio->GetBinWidth(hratio->GetNbinsX());
  TLine* line = new TLine(xmin, 1., xmax, 1.);
  line->SetLineColor(kRed);
  line->SetLineWidth(2);
  line->Draw("sames");
  pad2->SetGrid();
  if(print) {
    TString dir = Form("figures/test_data_%s_%i_%i/", selection_.Data(), year_, set1);
    gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s", dir.Data(), dir.Data()));
    c->Print(Form("%s%s_%s.png", dir.Data(), card.type_.Data(), card.hist_.Data()));
  }
  return c;
}

TCanvas* make_canvas(int set1, int set2, PlottingCard_t card, bool print) {
  TH1D *hData, *hMC;
  return make_canvas(set1, set2, card, print, hData, hMC);
}
TCanvas* make_2d_canvas(int set1, int set2, PlottingCard_t card, bool print) {
  TH2D *hData, *hMC;
  return make_2d_canvas(set1, set2, card, print, hData, hMC);
}

int test_data_scale(TString selection = "mutau", TString process = "WJets", int set1 = 31, int set2 = 2031,
                    int year = 2016, TString path = "nanoaods_dev") {

  //////////////////////
  // Initialize files //
  //////////////////////
  path = "root://cmseos.fnal.gov//store/user/mmackenz/histograms/" + path + "/";
  year_=year;
  selection_=selection;
  //get the absolute value of the sets, offsetting by the selection
  int offset = 0;
  if(selection == "mutau")      offset = CLFVHistMaker::kMuTau;
  else if(selection == "etau" ) offset = CLFVHistMaker::kETau ;
  else if(selection == "emu"  ) offset = CLFVHistMaker::kEMu  ;
  else if(selection == "mumu" ) offset = CLFVHistMaker::kMuMu ;
  else if(selection == "ee"   ) offset = CLFVHistMaker::kEE   ;
  int setAbs1 = set1 + offset;
  int setAbs2 = set2 + offset;

  //construct the general name of each file, not including the sample name
  TString baseName = "clfv_" + selection + "_clfv_";
  baseName += year;
  baseName += "_";

  //initialize the data files
  if(verbose_ > 0) std::cout << "Initializing the dataplotter" << std::endl;
  if(initialize_plotter(baseName, path, year)) {
    cout << "Dataplotter initialization failed!\n";
    return 1;
  }

  ////////////////////////
  // Perform closure test
  ////////////////////////

  TH1D *hData, *hMC;
  TFile* f = new TFile(Form("rootfiles/jet_to_tau_lead_pt_correction_%s_%i_%i.root", selection.Data(), set1, year), "RECREATE");
  f->cd();

  //total tau pT range
  make_canvas(setAbs1, setAbs2, PlottingCard_t("jettauonept" , "lep"  , 0), true, hData, hMC);
  if(!hData || !hMC) return 1;
  TH1D* hRatio = (TH1D*) hData->Clone("PtScale");
  hRatio->Scale(hMC->Integral() / hData->Integral());
  hRatio->Divide(hMC);
  hRatio->Write();

  //tau pT up to 30 GeV/c
  make_canvas(setAbs1, setAbs2, PlottingCard_t("jettauonept1" , "lep"  , 0), true, hData, hMC);
  if(!hData || !hMC) return 1;
  TH1D* hRatio_1 = (TH1D*) hData->Clone("PtScale_1");
  hRatio_1->Scale(hMC->Integral() / hData->Integral());
  hRatio_1->Divide(hMC);
  hRatio_1->Write();

  //tau pT 30-45 GeV/c
  make_canvas(setAbs1, setAbs2, PlottingCard_t("jettauonept2" , "lep"  , 0, 0,  0., 200.), true, hData, hMC);
  if(!hData || !hMC) return 1;
  TH1D* hRatio_2 = (TH1D*) hData->Clone("PtScale_2");
  hRatio_2->Scale(hMC->Integral() / hData->Integral());
  hRatio_2->Divide(hMC);
  hRatio_2->Write();

  //tau pT 45+ GeV/c
  make_canvas(setAbs1, setAbs2, PlottingCard_t("jettauonept3" , "lep"  , 0), true, hData, hMC);
  if(!hData || !hMC) return 1;
  TH1D* hRatio_3 = (TH1D*) hData->Clone("PtScale_3");
  hRatio_3->Scale(hMC->Integral() / hData->Integral());
  hRatio_3->Divide(hMC);
  hRatio_3->Write();


  TH1D* hRatio_dm;
  //DM 0
  make_canvas(setAbs1, setAbs2, PlottingCard_t("jettauonept_1" , "lep"  , 0), true, hData, hMC);
  hRatio_dm = (TH1D*) hData->Clone("PtScale_DM0");
  hRatio_dm->Scale(hMC->Integral() / hData->Integral());
  hRatio_dm->Divide(hMC);
  hRatio_dm->Write();

  //DM 1
  make_canvas(setAbs1, setAbs2, PlottingCard_t("jettauonept_2" , "lep"  , 0), true, hData, hMC);
  hRatio_dm = (TH1D*) hData->Clone("PtScale_DM1");
  hRatio_dm->Scale(hMC->Integral() / hData->Integral());
  hRatio_dm->Divide(hMC);
  hRatio_dm->Write();

  //DM 10
  make_canvas(setAbs1, setAbs2, PlottingCard_t("jettauonept_3" , "lep"  , 0), true, hData, hMC);
  hRatio_dm = (TH1D*) hData->Clone("PtScale_DM2");
  hRatio_dm->Scale(hMC->Integral() / hData->Integral());
  hRatio_dm->Divide(hMC);
  hRatio_dm->Write();

  //DM 11
  make_canvas(setAbs1, setAbs2, PlottingCard_t("jettauonept_4" , "lep"  , 0), true, hData, hMC);
  hRatio_dm = (TH1D*) hData->Clone("PtScale_DM3");
  hRatio_dm->Scale(hMC->Integral() / hData->Integral());
  hRatio_dm->Divide(hMC);
  hRatio_dm->Write();

  //tau eta correction
  make_canvas(setAbs1, setAbs2, PlottingCard_t("jettautwoeta" , "lep"  , 0), true, hData, hMC);
  hRatio_dm = (TH1D*) hData->Clone("EtaScale");
  hRatio_dm->Scale(hMC->Integral() / hData->Integral());
  hRatio_dm->Divide(hMC);
  hRatio_dm->Write();

  //pT vs delta R correction
  TH2D *hData_2d, *hMC_2d, *hRatio_2d;
  make_2d_canvas(setAbs1, setAbs2, PlottingCard_t("jettauoneptvsr", "lep", 0), true, hData_2d, hMC_2d);
  hRatio_2d = (TH2D*) hData_2d->Clone("PtVsRScale");
  hRatio_2d->Scale(hMC_2d->Integral() / hData_2d->Integral());
  hRatio_2d->Divide(hMC_2d);
  hRatio_2d->Write();

  make_2d_canvas(setAbs1, setAbs2, PlottingCard_t("jettautwoptvsonept", "lep", 0), true, hData_2d, hMC_2d);
  hRatio_2d = (TH2D*) hData_2d->Clone("Pt2Vs1Scale");
  hRatio_2d->Scale(hMC_2d->Integral() / hData_2d->Integral());
  hRatio_2d->Divide(hMC_2d);
  hRatio_2d->Write();

  make_canvas(setAbs1, setAbs2, PlottingCard_t("jettauonemetdeltaphi", "lep", 0), true, hData, hMC);
  hData = (TH1D*) hData->Clone("OneMetDeltaPhi");
  hData->Scale(hMC->Integral() / hData->Integral());
  hData->Divide(hMC);
  hData->Write();

  make_canvas(setAbs1, setAbs2, PlottingCard_t("jettauonemetdeltaphi_0", "lep", 0), true, hData, hMC);
  hData = (TH1D*) hData->Clone("OneMetDeltaPhi0");
  hData->Scale(hMC->Integral() / hData->Integral());
  hData->Divide(hMC);
  hData->Write();

  make_canvas(setAbs1, setAbs2, PlottingCard_t("jettauonemetdeltaphi_1", "lep", 0), true, hData, hMC);
  hData = (TH1D*) hData->Clone("OneMetDeltaPhi1");
  hData->Scale(hMC->Integral() / hData->Integral());
  hData->Divide(hMC);
  hData->Write();

  make_canvas(setAbs1, setAbs2, PlottingCard_t("jettauonemetdeltaphi_2", "lep", 0), true, hData, hMC);
  hData = (TH1D*) hData->Clone("OneMetDeltaPhi2");
  hData->Scale(hMC->Integral() / hData->Integral());
  hData->Divide(hMC);
  hData->Write();

  make_canvas(setAbs1, setAbs2, PlottingCard_t("jettauonemetdeltaphi_3", "lep", 0), true, hData, hMC);
  hData = (TH1D*) hData->Clone("OneMetDeltaPhi3");
  hData->Scale(hMC->Integral() / hData->Integral());
  hData->Divide(hMC);
  hData->Write();

  f->Close();

  //print relevant plots
  make_canvas(setAbs1, setAbs2, PlottingCard_t("jettautwometdeltaphi", "lep", 0), true);
  make_2d_canvas(setAbs1, setAbs2, PlottingCard_t("jettautwoptvsr", "lep", 0), true);
  make_2d_canvas(setAbs1, setAbs2, PlottingCard_t("twoptvsonept"  , "lep", 0), true);
  make_canvas(setAbs1, setAbs2, PlottingCard_t("onept"      , "lep"  , 0, 2,  20., 120.), true);
  make_canvas(setAbs1, setAbs2, PlottingCard_t("onereliso"  , "lep"  , 0, 2,   0.,  0.2), true);
  make_canvas(setAbs1, setAbs2, PlottingCard_t("onept11"    , "lep"  , 0, 2,  20., 120.), true);
  make_canvas(setAbs1, setAbs2, PlottingCard_t("oneeta"     , "lep"  , 0, 2,  -3.,   3.), true);
  make_canvas(setAbs1, setAbs2, PlottingCard_t("twopt"      , "lep"  , 0, 2,  20., 100.), true);
  make_canvas(setAbs1, setAbs2, PlottingCard_t("twopt11"    , "lep"  , 0, 2,  20., 120.), true);
  make_canvas(setAbs1, setAbs2, PlottingCard_t("twoeta"     , "lep"  , 0, 2,  -3.,   3.), true);
  make_canvas(setAbs1, setAbs2, PlottingCard_t("tausdm"     , "event", 0, 1,   0.,  12.), true);
  make_canvas(setAbs1, setAbs2, PlottingCard_t("tauspt"     , "event", 0, 1,  20., 100.), true);
  make_canvas(setAbs1, setAbs2, PlottingCard_t("mtone"      , "event", 0, 2,   0., 150.), true);
  make_canvas(setAbs1, setAbs2, PlottingCard_t("mttwo"      , "event", 0, 2,   0., 150.), true);
  make_canvas(setAbs1, setAbs2, PlottingCard_t("mtlep"      , "event", 0, 2,   0., 150.), true);
  make_canvas(setAbs1, setAbs2, PlottingCard_t("ntaus"      , "event", 0, 1,   0.,   7.), true);
  make_canvas(setAbs1, setAbs2, PlottingCard_t("njets"      , "event", 0, 1,   0.,   7.), true);
  make_canvas(setAbs1, setAbs2, PlottingCard_t("lepdeltar"  , "event", 0, 2,   0.,   6.), true);
  make_canvas(setAbs1, setAbs2, PlottingCard_t("lepdeltaphi", "event", 0, 2,   0.,   4.), true);
  make_canvas(setAbs1, setAbs2, PlottingCard_t("lepm"       , "event", 0, 5,  50., 170.), true);
  make_canvas(setAbs1, setAbs2, PlottingCard_t("jettauoner" , "lep"  , 0               ), true);
  make_canvas(setAbs1, setAbs2, PlottingCard_t("jettautwor" , "lep"  , 0               ), true);
  make_canvas(setAbs1, setAbs2, PlottingCard_t("jettautwopt", "lep"  , 0               ), true);
  make_canvas(setAbs1, setAbs2, PlottingCard_t("onemetdeltaphi", "lep", 0, 1,  0.,   4.), true);
  make_canvas(setAbs1, setAbs2, PlottingCard_t("twometdeltaphi", "lep", 0, 1,  0.,   4.), true);
  return 0;
}
