//Script to determine the composition of j->tau regions

//use the dataplotter to manage normalizations and initializations
DataPlotter* dataplotter_ = 0;
int verbose_ = 0;
int splitWJ_ = 1; //whether to use jet binned w+jets samples or not
TString name_; //figures directory
int setqcd_; //set to estimate QCD background in

TH1D* get_qcd(PlottingCard_t card) {
 TString hist = card.hist_;
  TString type = card.type_;
  int set = setqcd_;
  THStack* hstack = dataplotter_->get_stack(hist, type, set);
  if(!hstack) return NULL;
  TH1D* hdata = dataplotter_->get_data(hist, type, set);
  if(!hdata) return NULL;
  TH1D* hlast = (TH1D*) hstack->GetStack()->Last();
  //ensure no negative QCD contributions by making data at least equal to MC
  for(int ibin = 1; ibin <= hdata->GetNbinsX(); ++ibin) {
    if(hlast->GetBinContent(ibin) > hdata->GetBinContent(ibin)) hdata->SetBinContent(ibin, hlast->GetBinContent(ibin));
  }
  TH1D* hQCD = (TH1D*) hdata->Clone(Form("hComposition_%s_QCD", hist.Data()));
  hQCD->Add(hlast, -1.);
  // delete hdata;
  // delete hstack;
  return hQCD;
}
void make_composition(PlottingCard_t card, bool printHists = false, bool debug = false) {
  TString hist = card.hist_;
  TString type = card.type_;
  int set = card.set_;
  if(card.rebin_ > 1) dataplotter_->rebinH_ = card.rebin_;
  else dataplotter_->rebinH_ = 1;
  //Get MC stack
  THStack* hstack = dataplotter_->get_stack(hist, type, set);
  //Get QCD histogram
  TH1D* hQCD = get_qcd(card);
  hQCD->SetLineColor(dataplotter_->qcd_color_);
  hQCD->SetFillColor(dataplotter_->qcd_color_);
  if(debug) cout << "Composition for: hist = " << hist.Data() << " type = " << type.Data() << endl
                 << "Integrals:\n QCD = " << hQCD->Integral() << endl;
  //Get the total expectation
  TH1D* htot = (TH1D*) hstack->GetStack()->Last()->Clone("htot");
  htot->Add(hQCD);

  //Divide by the total expectation for fractional composition
  THStack* comp = new THStack(Form("comp_%s", hist.Data()), Form("comp_%s", hist.Data()));
  for(auto o : *hstack->GetHists()) {
    TH1D* h = (TH1D*) o->Clone(Form("hComposition_%s_%s", hist.Data(), o->GetTitle()));
    if(debug) cout << " " << h->GetTitle() << " = " << h->Integral() << endl;
    h->Divide(htot);
    if(printHists) h->Write();
    comp->Add(h);
  }
  if(debug) cout << " Total = " << htot->Integral() << endl;
  //for plotting purposes, clone qcd
  TH1D* hQCD_orig = (TH1D*) hQCD->Clone("QCD_orig");
  hQCD->Divide(htot);
  if(printHists) hQCD->Write();
  comp->Add(hQCD);

  TString xtitle = card.hist_;
  xtitle.ReplaceAll("jettau", "");
  xtitle.ReplaceAll("1", ""); xtitle.ReplaceAll("2", ""); xtitle.ReplaceAll("3", "");
  bool addone = xtitle.Contains("one");
  bool addtwo = xtitle.Contains("two");
  xtitle.ReplaceAll("one","");
  xtitle.ReplaceAll("two","");
  xtitle.ReplaceAll("delta", "#Delta");
  xtitle.ReplaceAll("eta", "#eta"); xtitle.ReplaceAll("phi", "#phi"); xtitle.ReplaceAll("pt", "p_{T}");
  if(addone) xtitle = xtitle + "^{1}";
  if(addtwo) xtitle = xtitle + "^{2}";
  xtitle.ReplaceAll("taus", "#tau ");
  xtitle.ReplaceAll("met", "MET ");
  xtitle.ReplaceAll("dm", "Decay Mode");

  TLegend* leg = new TLegend(0.7, 0.7, 0.9, 0.9);
  leg->AddEntry(hQCD_orig, "QCD", "F");
  for(auto o : *hstack->GetHists()) leg->AddEntry(o, o->GetTitle(), "F");

  TCanvas* c = new TCanvas(Form("c_%s", hist.Data()), Form("c_%s", hist.Data()), 1000, 600);
  c->Divide(2,1);
  auto pad = c->cd(1);
  hstack->Draw("hist noclear");
  // pad->Modified(); c->Modified();
  // pad->Update(); c->Update();
  // gPad->Modified(); gPad->Update();
  // hstack->GetXaxis()->SetTitle(xtitle.Data());
  hstack->SetTitle(xtitle.Data());
  hQCD_orig->SetFillStyle(3003);
  hQCD_orig->Draw("hist same");
  pad->SetLogy();
  leg->Draw();
  c->cd(2);
  comp->Draw("hist noclear");
  // comp->GetXaxis()->SetTitle(xtitle.Data());
  comp->SetTitle(xtitle.Data());
  // comp->GetYaxis()->SetTitle("Fraction of CR");
  comp->SetMaximum(1.05);
  comp->SetMinimum(0.);
  leg->Draw();
  c->Modified(); c->Update();
  if(card.xmin_ < card.xmax_ && comp->GetXaxis()) comp->GetXaxis()->SetRangeUser(card.xmin_, card.xmax_);
  c->Modified(); c->Update();
  c->Print(Form("%scomp_%s_%s.png", name_.Data(), type.Data(), hist.Data()));
}

//initialize the files and scales using a DataPlotter
Int_t initialize_plotter(TString base, TString path, int year) {
  if(dataplotter_) delete dataplotter_;
  dataplotter_ = new DataPlotter();
  dataplotter_->include_qcd_ = 0;
  dataplotter_->verbose_ = verbose_;
  dataplotter_->doStatsLegend_ = 0;

  typedef DataCard_t dcard;
  std::vector<dcard> cards;
  //card constructor:    filepath,                             name,                  label,              isData, xsec,  isSignal
  CrossSections xs; //cross section handler
  dataplotter_->set_luminosity(xs.GetLuminosity(year));
  if(verbose_ > 0) cout << "--- Dataplotter luminosity for " << year << " = " << dataplotter_->lum_ << endl;
  if(year == 2017) {
    cards.push_back(dcard(path+base+"DY50-ext-1.hist"       , "DY50-ext-1"         , "ZJets"         , false, xs.GetCrossSection("DY50"               ), false, year, kRed - 7));
    cards.push_back(dcard(path+base+"DY50-ext-2.hist"       , "DY50-ext-2"         , "ZJets"         , false, xs.GetCrossSection("DY50"               ), false, year, kRed - 7));
  } else {
    cards.push_back(dcard(path+base+"DY50-amc-1.hist"       , "DY50-amc-1"         , "ZJets"         , false, xs.GetCrossSection("DY50"               ), false, year, kRed - 7));
    cards.push_back(dcard(path+base+"DY50-amc-2.hist"       , "DY50-amc-2"         , "ZJets"         , false, xs.GetCrossSection("DY50"               ), false, year, kRed - 7));
  }
  cards.push_back(dcard(path+base+"SingleAntiToptW.hist"    , "SingleAntiToptW"    , "Top"         , false, xs.GetCrossSection("SingleAntiToptW"    ), false, year, kYellow - 7));
  cards.push_back(dcard(path+base+"SingleToptW.hist"        , "SingleToptW"        , "Top"         , false, xs.GetCrossSection("SingleToptW"        ), false, year, kYellow - 7));
  cards.push_back(dcard(path+base+"WWW.hist"                , "WWW"                , "WJets"         , false, xs.GetCrossSection("WWW"                ), false, year, kGreen - 2));
  cards.push_back(dcard(path+base+"WZ.hist"                 , "WZ"                 , "WJets"         , false, xs.GetCrossSection("WZ"                 ), false, year, kGreen - 2));
  cards.push_back(dcard(path+base+"ZZ.hist"                 , "ZZ"                 , "WJets"         , false, xs.GetCrossSection("ZZ"                 ), false, year, kGreen - 2));
  cards.push_back(dcard(path+base+"WW.hist"                 , "WW"                 , "WJets"         , false, xs.GetCrossSection("WW"                 ), false, year, kGreen - 2));
  double wxs = xs.GetCrossSection("Wlnu");
  double ngen1 = xs.GetGenNumber("Wlnu", year);
  double ngen2 = xs.GetGenNumber("Wlnu-ext", year);
  if(splitWJ_) {
    if(year != 2018) {
      cards.push_back(dcard(path+base+"Wlnu-0.hist"         , "Wlnu"               , "WJets"          , false, wxs*(ngen1)/(ngen1+ngen2)                , false, year, kViolet - 9));
      cards.push_back(dcard(path+base+"Wlnu-ext-0.hist"     , "Wlnu"               , "WJets"          , false, wxs*(ngen2)/(ngen1+ngen2)                , false, year, kViolet - 9));
    } else {
      cards.push_back(dcard(path+base+"Wlnu-0.hist"         , "Wlnu"               , "WJets"          , false, wxs                                      , false, year, kViolet - 9));
    }
    cards.push_back(dcard(path+base+"Wlnu-1J.hist"          , "Wlnu-1J"            , "WJets"          , false, xs.GetCrossSection("Wlnu-1J"            ), false, year, kViolet - 9));
    cards.push_back(dcard(path+base+"Wlnu-2J.hist"          , "Wlnu-2J"            , "WJets"          , false, xs.GetCrossSection("Wlnu-2J"            ), false, year, kViolet - 9));
    cards.push_back(dcard(path+base+"Wlnu-3J.hist"          , "Wlnu-3J"            , "WJets"          , false, xs.GetCrossSection("Wlnu-3J"            ), false, year, kViolet - 9));
    if(year != 2017) {
      cards.push_back(dcard(path+base+"Wlnu-4J.hist"        , "Wlnu-4J"            , "WJets"          , false, xs.GetCrossSection("Wlnu-4J"            ), false, year, kViolet - 9));
    } else {
      cards.push_back(dcard(path+base+"Wlnu-4.hist"         , "Wlnu"               , "WJets"          , false, wxs*(ngen1)/(ngen1+ngen2)                , false, year, kViolet - 9));
      cards.push_back(dcard(path+base+"Wlnu-ext-4.hist"     , "Wlnu"               , "WJets"          , false, wxs*(ngen2)/(ngen1+ngen2)                , false, year, kViolet - 9));
    }
  } else {
    if(year != 2018) {
      cards.push_back(dcard(path+base+"Wlnu.hist"             , "Wlnu"               , "WJets" , false, wxs*(ngen1)/(ngen1+ngen2)                   , false, year, kViolet - 9));
      cards.push_back(dcard(path+base+"Wlnu-ext.hist"         , "Wlnu"               , "WJets" , false, wxs*(ngen2)/(ngen1+ngen2)                   , false, year, kViolet - 9));
    } else {
      cards.push_back(dcard(path+base+"Wlnu.hist"             , "Wlnu"               , "WJets" , false, wxs                                         , false, year, kViolet - 9));
    }
  }
  cards.push_back(dcard(path+base+"ttbarToSemiLeptonic.hist", "ttbarToSemiLeptonic", "Top"          , false, xs.GetCrossSection("ttbarToSemiLeptonic"), false, year));
  cards.push_back(dcard(path+base+"ttbarlnu.hist"           , "ttbarlnu"           , "Top"          , false, xs.GetCrossSection("ttbarlnu"           ), false, year));
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


//Generate the plots and scale factors
Int_t composition(TString selection = "mutau", int setmc = 42, int setqcd = 35, int year = 2016, TString path = "nanoaods_dev") {

  //////////////////////
  // Initialize files //
  //////////////////////
  path = "root://cmseos.fnal.gov//store/user/mmackenz/histograms/" + path + "/";

  //get the absolute value of the set, offsetting by the selection
  int setmcAbs = setmc;
  if(selection == "mutau")      setmcAbs += ZTauTauHistMaker::kMuTau;
  else if(selection == "etau" ) setmcAbs += ZTauTauHistMaker::kETau ;
  else if(selection == "emu"  ) setmcAbs += ZTauTauHistMaker::kEMu  ;
  else if(selection == "mumu" ) setmcAbs += ZTauTauHistMaker::kMuMu ;
  else if(selection == "ee"   ) setmcAbs += ZTauTauHistMaker::kEE   ;
  int setqcdAbs = setqcd + (setmcAbs - setmc);

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
  dataplotter_->include_qcd_ = 0;
  dataplotter_->include_misid_ = 0;
  setqcd_ = setqcdAbs;

  //////////////////////
  //  Make the comps  //
  //////////////////////

  //construct general figure name
  name_ = "figures/fake_tau_comp_";
  name_ += selection + "_";
  name_ += year;
  name_ += "_";
  name_ += setmc;
  name_ += "_";
  name_ += setqcd;
  name_ += "/";
  //ensure directories exist
  gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s", name_.Data(), name_.Data()));
  gSystem->Exec("[ ! -d rootfiles ] && mkdir rootfiles");

  //print canvases
  TFile* fOut = new TFile(Form("rootfiles/jet_to_tau_comp_%s_%i_%i.root", selection.Data(), setqcd, year), "RECREATE");
  make_composition(PlottingCard_t("twopt", "lep", setmcAbs, 2, 20., 150.), true);
  make_composition(PlottingCard_t("jettautwopt", "lep", setmcAbs), true);
  make_composition(PlottingCard_t("jettauonept", "lep", setmcAbs), true);
  make_composition(PlottingCard_t("jettautwor", "lep", setmcAbs), true);
  make_composition(PlottingCard_t("jettauoner", "lep", setmcAbs), true);
  make_composition(PlottingCard_t("jettauonemetdeltaphi", "lep", setmcAbs, 2, 0., 4.), true);
  make_composition(PlottingCard_t("jettautwometdeltaphi", "lep", setmcAbs, 2, 0., 4.), true);
  make_composition(PlottingCard_t("mtone", "event", setmcAbs, 5, 0., 150.), true);
  make_composition(PlottingCard_t("mttwo", "event", setmcAbs, 5, 0., 150.), true);
  make_composition(PlottingCard_t("mtlep", "event", setmcAbs, 5, 0., 150.), true);

  // make_composition(PlottingCard_t("jettauonemetdeltaphi", "lep", setmcAbs, 2, 0., 4.), false, true);

  fOut->Close();
  delete fOut;
  return 0;
}
