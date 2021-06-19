//Test MC derived scale factors on MC

//use the dataplotter to manage normalizations and initializations
DataPlotter* dataplotter_ = 0;
int verbose_ = 0;
int year_;
TString selection_;
int splitWJ_ = 1;
int splitDY_ = 1;

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

TCanvas* make_canvas(int set1, int set2, PlottingCard_t card, bool print) {
  dataplotter_->plot_data_ = 0;
  dataplotter_->include_qcd_ = 0;
  dataplotter_->include_misid_ = 0;
  dataplotter_->rebinH_ = card.rebin_;
  dataplotter_->doStatsLegend_ = false;
  THStack* hstack_loose = dataplotter_->get_stack(card.hist_, card.type_, set1+dataplotter_->misid_offset_);
  if(!hstack_loose || hstack_loose->GetNhists() == 0) return NULL;
  THStack* hstack_tight = dataplotter_->get_stack(card.hist_, card.type_, set2);
  if(!hstack_tight || hstack_tight->GetNhists() == 0) return NULL;
  TH1D* hloose = (TH1D*) hstack_loose->GetStack()->Last()->Clone(("h_"+card.hist_+"_loose").Data());
  TCanvas* c = new TCanvas("c1","c1",1200,800);
  TPad* pad1 = new TPad("pad1", "pad1", 0., 0.35, 1., 1.0 );
  TPad* pad2 = new TPad("pad2", "pad2", 0., 0.0 , 1., 0.35);
  pad1->SetBottomMargin(0.03);
  pad1->SetTopMargin(0.03);
  pad2->SetTopMargin(0.03);
  pad2->SetBottomMargin(0.3);
  pad1->Draw(); pad2->Draw();
  pad1->cd();
  hloose->SetLineColor(kBlack);
  hloose->SetFillColor(0);
  hloose->SetTitle("");
  hloose->Draw("E1");
  hstack_tight->Draw("hist noclear same");
  hloose->GetXaxis()->SetRangeUser(card.xmin_, card.xmax_);
  hloose->GetYaxis()->SetRangeUser(0.1, 1.1*max(hstack_tight->GetMaximum(), hloose->GetMaximum()));
  hloose->Draw("E1 sames");
  hloose->GetXaxis()->SetLabelSize(0.);
  hloose->GetYaxis()->SetLabelSize(0.06);
  hloose->GetXaxis()->SetTitleSize(0.157);
  hloose->GetYaxis()->SetTitleSize(0.157);

  TLegend* leg = new TLegend(0.61, 0.65, 0.9, 0.97);
  leg->AddEntry(hloose, "Weighted Loose j#rightarrow#tau");
  for(auto obj : *hstack_tight->GetStack()) {
    leg->AddEntry(obj, Form("%s: Tight j#rightarrow#tau", obj->GetTitle()), "F");
  }
  leg->SetTextSize(0.06);
  leg->Draw();

  pad1->SetGrid();

  //Add labels with the event number and predicted number
  TLatex datalabel;
  datalabel.SetNDC();
  datalabel.SetTextFont(72);
  datalabel.SetTextSize(0.07);
  datalabel.SetTextAlign(13);
  datalabel.DrawLatex(0.67, 0.62 , Form("Tight  : %10.1f", ((TH1D*) hstack_tight->GetStack()->Last())->Integral(0, hloose->GetNbinsX() + 1)));
  datalabel.DrawLatex(0.67, 0.54, Form("Loose: %10.1f", hloose->Integral(0, hloose->GetNbinsX() + 1)));

  //Draw the ratio plot
  pad2->cd();
  TH1D* hratio = (TH1D*) hstack_tight->GetStack()->Last()->Clone(("h_"+card.hist_+"_ratio").Data());
  hratio->Divide(hloose);
  hratio->SetLineColor(kBlack);
  hratio->SetFillColor(0);
  pad2->Update();

  TString xtitle = card.hist_;
  if(xtitle.Contains("one")) {xtitle.ReplaceAll("one",""); xtitle = xtitle + "^{1}";}
  if(xtitle.Contains("two")) {xtitle.ReplaceAll("two",""); xtitle = xtitle + "^{2}";}
  xtitle.ReplaceAll("eta", "#eta"); xtitle.ReplaceAll("phi", "#phi"); xtitle.ReplaceAll("pt", "p_{T}");
  if(xtitle.Contains("metdelta")) {xtitle.ReplaceAll("metdelta", ""); xtitle.ReplaceAll("}", ""); xtitle = "#Delta" + xtitle + ",MET}";}
  hratio->Draw("E1");
  hratio->GetYaxis()->SetRangeUser(0.5, 1.5);
  hratio->GetXaxis()->SetRangeUser(card.xmin_, card.xmax_);
  hratio->SetXTitle(xtitle.Data());
  hratio->SetYTitle("Tight / Scaled Loose");
  hratio->SetTitle("");
  hratio->GetXaxis()->SetTitleOffset(0.75);
  hratio->GetYaxis()->SetTitleOffset(0.4);
  hratio->GetXaxis()->SetLabelSize(0.1);
  hratio->GetYaxis()->SetLabelSize(0.09);
  hratio->GetXaxis()->SetTitleSize(0.157);
  hratio->GetYaxis()->SetTitleSize(0.11);

  TLine* line = new TLine(card.xmin_, 1., card.xmax_, 1.);
  line->SetLineColor(kRed);
  line->SetLineWidth(2);
  line->Draw("sames");
  pad2->SetGrid();
  if(print) {
    TString dir = Form("figures/test_mc_%s_%i_%i/", selection_.Data(), year_, set1);
    gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s", dir.Data(), dir.Data()));
    c->Print(Form("%s%s_%s.png", dir.Data(), card.type_.Data(), card.hist_.Data()));
  }
  return c;
}

int test_mc_scale(TString selection = "mutau", int year = 2016, int set1 = 41, int set2 = 41,
                  TString path = "nanoaods_dev") {

  //////////////////////
  // Initialize files //
  //////////////////////
  path = "root://cmseos.fnal.gov//store/user/mmackenz/histograms/" + path + "/";

  //get the absolute value of the sets, offsetting by the selection
  int offset = 0;
  if(selection == "mutau")      offset = ZTauTauHistMaker::kMuTau;
  else if(selection == "etau" ) offset = ZTauTauHistMaker::kETau ;
  else if(selection == "emu"  ) offset = ZTauTauHistMaker::kEMu  ;
  else if(selection == "mumu" ) offset = ZTauTauHistMaker::kMuMu ;
  else if(selection == "ee"   ) offset = ZTauTauHistMaker::kEE   ;
  int setAbs1 = set1 + offset;
  int setAbs2 = set2 + offset;
  selection_ = selection;
  year_ = year;
  //construct the general name of each file, not including the sample name
  TString baseName = "ztautau_" + selection + "_clfv_";
  baseName += year;
  baseName += "_";

  //initialize the data files
  if(verbose_ > 0) std::cout << "Initializing the dataplotter" << std::endl;
  if(initialize_plotter(baseName, path, year)) {
    cout << "Dataplotter initialization failed!\n";
    return 1;
  }

  //print relevant plots
  make_canvas(setAbs1, setAbs2, PlottingCard_t("onept" , "lep", 0, 5, 20., 120.), true);
  make_canvas(setAbs1, setAbs2, PlottingCard_t("oneeta", "lep", 0, 5, -3.,   3.), true);
  make_canvas(setAbs1, setAbs2, PlottingCard_t("twopt" , "lep", 0, 5, 20., 100.), true);
  make_canvas(setAbs1, setAbs2, PlottingCard_t("twoeta", "lep", 0, 5, -3.,   3.), true);
  make_canvas(setAbs1, setAbs2, PlottingCard_t("taudecaymode", "event", 0, 1, 0,   12.), true);
  make_canvas(setAbs1, setAbs2, PlottingCard_t("njets", "event", 0, 1, 0.,   7.), true);
  make_canvas(setAbs1, setAbs2, PlottingCard_t("mtone", "event", 0, 5, 0., 150.), true);
  make_canvas(setAbs1, setAbs2, PlottingCard_t("mttwo", "event", 0, 5, 0., 150.), true);
  make_canvas(setAbs1, setAbs2, PlottingCard_t("mtlep", "event", 0, 5, 0., 150.), true);
  make_canvas(setAbs1, setAbs2, PlottingCard_t("met"  , "event", 0, 2, 0., 100.), true);
  make_canvas(setAbs1, setAbs2, PlottingCard_t("onemetdeltaphi", "lep", 0, 2, 0., 4.), true);
  make_canvas(setAbs1, setAbs2, PlottingCard_t("twometdeltaphi", "lep", 0, 2, 0., 4.), true);
  make_canvas(setAbs1, setAbs2, PlottingCard_t("jettautwopt"   , "lep", 0, 1, 0., 200.), true);
  make_canvas(setAbs1, setAbs2, PlottingCard_t("jettautwoeta"  , "lep", 0, 1,-3., 3.), true);
  return 0;
}
