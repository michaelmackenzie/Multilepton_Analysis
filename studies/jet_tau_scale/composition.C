//Script to determine the composition of j->tau regions
#include "histograms/datacards.C"

//use the dataplotter to manage normalizations and initializations
DataPlotter* dataplotter_ = 0;
int     verbose_     = 0;
TString name_           ; //figures directory
int     setqcd_         ; //set to estimate QCD background in
TString selection_      ;

//-----------------------------------------------------------------------------------------------------------------------------------------
// Estimate the QCD using Data - MC
TH1* get_qcd(PlottingCard_t card) {
  TString hist = card.hist_;
  TString type = card.type_;
  const int set = setqcd_;
  THStack* hstack = dataplotter_->get_stack(hist, type, set);
  if(!hstack || hstack->GetNhists() == 0) return nullptr;
  TH1* hdata = dataplotter_->get_data(hist, type, set);
  if(!hdata) return nullptr;
  TH1* hlast = (TH1*) hstack->GetStack()->Last();
  TH1* hQCD = (TH1*) hdata->Clone(Form("hComposition_%s_QCD", hist.Data()));
  hQCD->Add(hlast, -1.);
  //ensure no negative QCD contributions
  for(int ibin = 1; ibin <= hQCD->GetNbinsX(); ++ibin) {
    if(hQCD->GetBinContent(ibin) < 0.) hQCD->SetBinContent(ibin, 0.);
  }
  // delete hdata;
  // delete hstack;
  return hQCD;
}

//-----------------------------------------------------------------------------------------------------------------------------------------
void make_composition(PlottingCard_t card, bool printHists = false, bool debug = false) {
  TString hist = card.hist_;
  TString type = card.type_;
  const int set = card.set_;
  if(card.rebin_ > 1) dataplotter_->rebinH_ = card.rebin_;
  else dataplotter_->rebinH_ = 1;

  //Get MC stack
  THStack* hstack = dataplotter_->get_stack(hist, type, set);
  if(!hstack || hstack->GetNhists() == 0) return;

  //Get QCD histogram
  TH1* hQCD = get_qcd(card);
  if(!hQCD) return;
  hQCD->SetLineColor(dataplotter_->qcd_color_);
  hQCD->SetFillColor(dataplotter_->qcd_color_);

  if(debug) cout << "Composition for: hist = " << hist.Data() << " type = " << type.Data() << endl
                 << "Integrals:\n QCD = " << hQCD->Integral() << endl;
  //Get the total expectation
  TH1* htot = (TH1*) hstack->GetStack()->Last()->Clone("htot");
  htot->Add(hQCD);

  //Divide by the total expectation for fractional composition
  THStack* comp = new THStack(Form("comp_%s", hist.Data()), Form("comp_%s", hist.Data()));
  for(auto o : *hstack->GetHists()) {
    TH1* h = (TH1*) o->Clone(Form("hComposition_%s_%s", hist.Data(), o->GetTitle()));
    if(debug) cout << " " << h->GetTitle() << " = " << h->Integral() << endl;
    h->Divide(htot);
    // for(int ibin = 1; ibin <= h->GetNbinsX(); ++ibin) {
    //   const double binc = h->GetBinContent(ibin);
    //   if(binc < 0. || binc > 1.) h->SetBinContent(ibin, std::min(1., std::max(0., binc)));
    // }
    if(printHists) h->Write();
    comp->Add(h);
  }
  if(debug) cout << " Total = " << htot->Integral() << endl;
  //for plotting purposes, clone qcd
  TH1* hQCD_orig = (TH1*) hQCD->Clone("QCD_orig");
  hQCD->Divide(htot);
  if(printHists) hQCD->Write();
  comp->Add(hQCD);

  TString xtitle = card.hist_;
  xtitle.ReplaceAll("jettau", "");
  xtitle.ReplaceAll("_", "");
  xtitle.ReplaceAll("1", ""); xtitle.ReplaceAll("0", ""); xtitle.ReplaceAll("2", ""); xtitle.ReplaceAll("3", "");
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
  const double max_v = std::max(hstack->GetMaximum(), hQCD_orig->GetMaximum());
  hstack->SetMaximum(max_v*3.);
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

//-----------------------------------------------------------------------------------------------------------------------------------------
//initialize the files and scales using a DataPlotter
Int_t initialize_plotter(int year) {
  if(dataplotter_) delete dataplotter_;
  dataplotter_ = new DataPlotter();
  dataplotter_->include_qcd_   = 0;
  dataplotter_->include_misid_ = 0;
  dataplotter_->verbose_       = max(0, verbose_ - 1);
  dataplotter_->qcd_scale_     = 1.;
  dataplotter_->embed_scale_   = embedScale_;
  dataplotter_->doStatsLegend_ = false;
  years_ = {year};
  useEmbed_ = 0;

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

//-----------------------------------------------------------------------------------------------------------------------------------------
//setup everything
Int_t init(TString selection = "mutau", int year = 2016, TString path = "nanoaods_dev") {
  selection_ = selection;
  hist_dir_ = path;
  hist_tag_ = "jtt";
  //initialize the data files
  if(verbose_ > 0) std::cout << "Initializing the dataplotter" << std::endl;
  if(initialize_plotter(year)) {
    cout << "Dataplotter initialization failed!\n";
    return 1;
  }
  return 0;
}

//-----------------------------------------------------------------------------------------------------------------------------------------
//Generate the plots and scale factors
Int_t composition(TString selection = "mutau", int setmc = 2042, int setqcd = 3035, int year = 2016, TString path = "nanoaods_dev") {
  //////////////////////
  // Initialize files //
  //////////////////////
  if(init(selection, year, path)) return 1;

  //get the absolute value of the set, offsetting by the selection
  int set_offset = 0;
  if     (selection == "mutau") set_offset = CLFVHistMaker::kMuTau;
  else if(selection == "etau" ) set_offset = CLFVHistMaker::kETau ;
  else if(selection == "emu"  ) set_offset = CLFVHistMaker::kEMu  ;
  else if(selection == "mumu" ) set_offset = CLFVHistMaker::kMuMu ;
  else if(selection == "ee"   ) set_offset = CLFVHistMaker::kEE   ;
  const int setmcAbs  = setmc  + set_offset;
  const int setqcdAbs = setqcd + set_offset;
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
  TFile* fOut = new TFile(Form("rootfiles/jet_to_tau_comp_%s_%i_%i_%i.root", selection.Data(), setmc, setqcd, year), "RECREATE");
  make_composition(PlottingCard_t("twopt", "lep", setmcAbs, 2, 20., 150.), false);
  make_composition(PlottingCard_t("jettautwopt", "lep", setmcAbs), false);
  make_composition(PlottingCard_t("jettauonept", "lep", setmcAbs), false);
  make_composition(PlottingCard_t("jettautwor", "lep", setmcAbs), false);
  make_composition(PlottingCard_t("jettauoner", "lep", setmcAbs), false);
  make_composition(PlottingCard_t("jettauonemetdeltaphi", "lep", setmcAbs, 1, 0., 4.), true);
  make_composition(PlottingCard_t("jettautwometdeltaphi", "lep", setmcAbs, 1, 0., 4.), true);
  make_composition(PlottingCard_t("jettauonemetdphicomp", "lep", setmcAbs, 1, 1.,-1.), true);
  make_composition(PlottingCard_t("jettautwometdphicomp", "lep", setmcAbs, 1, 1.,-1.), true);
  make_composition(PlottingCard_t("jettaumtonecomp"     , "lep", setmcAbs, 1, 1.,-1.), true);
  make_composition(PlottingCard_t("jettaumttwocomp"     , "lep", setmcAbs, 1, 1.,-1.), true);
  make_composition(PlottingCard_t("jettauoneptcomp"     , "lep", setmcAbs, 1, 1.,-1.), true);
  make_composition(PlottingCard_t("jettautwoptcomp"     , "lep", setmcAbs, 1, 1.,-1.), true);
  make_composition(PlottingCard_t("mtone", "event", setmcAbs, 5, 0., 150.), false);
  make_composition(PlottingCard_t("mttwo", "event", setmcAbs, 5, 0., 150.), false);
  make_composition(PlottingCard_t("mtlep", "event", setmcAbs, 5, 0., 150.), false);
  make_composition(PlottingCard_t("taudecaymode", "event", setmcAbs, 0, 1., -1.), false);

  //Decay mode dependent compositions
  make_composition(PlottingCard_t("jettauonemetdeltaphi_0", "lep", setmcAbs, 1, 0., 4.), false);
  make_composition(PlottingCard_t("jettauonemetdeltaphi_1", "lep", setmcAbs, 1, 0., 4.), false);
  make_composition(PlottingCard_t("jettauonemetdeltaphi_2", "lep", setmcAbs, 1, 0., 4.), false);
  make_composition(PlottingCard_t("jettauonemetdeltaphi_3", "lep", setmcAbs, 1, 0., 4.), false);

  fOut->Close();
  return 0;
}
