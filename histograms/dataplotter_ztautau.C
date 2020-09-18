// Script to plot and print distributions using a DataPlotter object for ZTauTauHistMaker histograms

DataPlotter* dataplotter_ = 0;
TString selection_ = "emu"; //current options: mutau, etau, emu, mutau_e, etau_mu, mumu
Int_t verbose_ = 0; //verbosity level
Int_t useOpenGL_ = 0;
bool  doStatsLegend_ = false;
TString hist_dir_ = "nanoaods";
TString folder_ = "nanoaods";
bool doAllEMu_ = false; //plot all emu signals (including leptonic decays) on selection_ == "emu"
bool printCDFs_ = false; //print cdf transform of MVAs
bool printLimitVsEff_ = false; //print limit gain vs signal efficiency from MVA cut
bool printMVATotBkg_ = false; //print MVA distributions as total background vs signal
bool print2Ds_ = false;
bool printSignificances_ = true;
bool printMVAPlots_ = true;
bool printSlimCounts_ = false;
bool printBlindSets_ = false;
int year_ = 2016;
bool offsetSets_ = true; //offset by selection set from ZTauTauHistMaker
int sigOverBkg_ = 0; //plot sig / bkg or data / MC (0 = data/MC, 1 = sig/MC, 2 = sig*sig/MC)

Int_t print_significance_canvases(vector<TString> hists, vector<TString> types, vector<TString> labels, vector<int> sets) {
  TCanvas* c = 0;
  if(!dataplotter_) return -2;
  
  if(hists.size() != types.size() || labels.size() != hists.size())
    return -1;
  int n = hists.size();
  for(int set : sets) {
    if(offsetSets_) {
      if(selection_ == "mutau"  ) set += ZTauTauHistMaker::kMuTau;
      if(selection_ == "etau"   ) set += ZTauTauHistMaker::kETau;
      if(selection_ == "emu"    ) set += ZTauTauHistMaker::kEMu;
      if(selection_ == "mutau_e") set += ZTauTauHistMaker::kMuTauE;
      if(selection_ == "etau_mu") set += ZTauTauHistMaker::kETauMu;
    }
    for(int i = 0; i < n; ++i) {
      TString filename = Form("sig_vsEff_%s_%s_set_%i.root", labels[i].Data(), hists[i].Data(), set);
      filename.ReplaceAll("#",""); //for ease of use in bash
      filename.ReplaceAll(" ", "");
      filename.ReplaceAll("/", "");

      c = dataplotter_->plot_significance(hists[i], types[i], set, labels[i], 0., 1., true, -1., true);
      TFile* f = new TFile(Form("canvases/%s/%s/%s", folder_.Data(), selection_.Data(), filename.Data()), "RECREATE");
      printf("Printing canvas canvases/%s/%s/%s\n" , folder_.Data(), selection_.Data(), filename.Data());
      c->Write();
      f->Write();
      DataPlotter::Empty_Canvas(c);
      delete f;
    }
  }
  return 0;
}

Int_t print_statistics(TString hist, TString type, int set, double xmin = 1., double xmax = -1.) {
  if(!dataplotter_) return -1;
  if(offsetSets_) {
    if(selection_ == "mutau"  ) set += ZTauTauHistMaker::kMuTau;
    if(selection_ == "etau"   ) set += ZTauTauHistMaker::kETau;
    if(selection_ == "emu"    ) set += ZTauTauHistMaker::kEMu;
    if(selection_ == "mutau_e") set += ZTauTauHistMaker::kMuTauE;
    if(selection_ == "etau_mu") set += ZTauTauHistMaker::kETauMu;
  }
  
  TCanvas* c = dataplotter_->plot_stack(hist, type, set);
  if(!c) return 1;
  TList* list = c->GetListOfPrimitives();
  if(!list) return 2;
  double total = 0.;
  double variance = 0.;
  for(auto o : *list) {
    try {
      auto pad = (TPad*) o;
      if(!pad) continue;
      TList* pad_list = pad->GetListOfPrimitives();
      for(auto h : *pad_list) {
	cout << h->GetName() << ": " << h->GetTitle() << ": " << h->InheritsFrom("THStack") << endl;
	if(h->InheritsFrom("THStack")) {
	  THStack* hstack = (THStack*) h;
	  TList* hist_list = hstack->GetHists();
	  for(auto hl : *hist_list) {
	    TH1F* hist = (TH1F*) hl;
	    double error = 0.;
	    double integral = 0.;
	    cout << "--> " << hist->GetTitle() << ": ";
	    if(xmin < xmax) {
	      int bin1 = hist->FindBin(xmin);
	      int bin2 = hist->FindBin(xmax);
	      integral = hist->IntegralAndError(bin1, bin2, error);
	      cout << integral << " (" << hist->GetBinLowEdge(bin1) << " < x < "
		   << hist->GetBinLowEdge(bin2) + hist->GetBinWidth(bin2) << ")";
	    } else {
	      integral = hist->IntegralAndError(0,hist->GetNbinsX()+1,error);
	      cout << integral;
	    }
	    cout <<  " +- " << error << " (" << error*100./integral << "%, nentries = " << hist->GetEntries() << ")" << endl;
	    total += integral;
	    variance += error*error;
	    delete hist;
	  }
	} else if(h->InheritsFrom("TH1F")) { //signal histograms or data
	    TH1F* hist = (TH1F*) h;
	    double error = 0.;
	    double integral = 0.;
	    cout << "--> " << hist->GetTitle() << ": ";
	    if(xmin < xmax) {
	      int bin1 = hist->FindBin(xmin);
	      int bin2 = hist->FindBin(xmax);
	      integral = hist->IntegralAndError(bin1, bin2, error);
	      cout << integral << " (" << hist->GetBinLowEdge(bin1) << " < x < "
		   << hist->GetBinLowEdge(bin2) + hist->GetBinWidth(bin2) << ")";
	    } else {
	      integral = hist->IntegralAndError(0,hist->GetNbinsX()+1,error);
	      cout << integral;
	    }
	    cout <<  " +- " << error << " (" << error*100./integral << "%, nentries = " << hist->GetEntries() << ")" << endl;	    
	}
	delete h;
      }
    } catch(exception e) {
    }
  }
  cout << "Total = " << total << " +- " << sqrt(variance)
       << " (" << sqrt(variance)/total*100. << "%)" <<  endl;
  DataPlotter::Empty_Canvas(c);
  return 0;
}

TCanvas* print_canvas(TString hist, TString type, int set, bool stacks = true, TString name = "") {
  if(!dataplotter_) {
    cout << "Dataplotter isn't initialized!\n";
    return NULL;
  }
  if(offsetSets_) {
    if(selection_ == "mutau"  ) set += ZTauTauHistMaker::kMuTau;
    if(selection_ == "etau"   ) set += ZTauTauHistMaker::kETau;
    if(selection_ == "emu"    ) set += ZTauTauHistMaker::kEMu;
    if(selection_ == "mutau_e") set += ZTauTauHistMaker::kMuTauE;
    if(selection_ == "etau_mu") set += ZTauTauHistMaker::kETauMu;
  }
  
  TCanvas* c = (stacks) ? dataplotter_->plot_stack(hist, type, set) : dataplotter_->plot_hist(hist, type, set);
  if(!c) 
    return c;
  TFile* f = TFile::Open(Form("canvases/%s/%s/%s%s%s_%i.root", folder_.Data(), selection_.Data(),\
			      (name == "") ? "" : Form("%s_", name.Data()), hist.Data(),
			      (dataplotter_->logY_ ? "_log" : ""), set), "RECREATE");
  if(!f) {
    cout << "Failed to create file!\n";
    return NULL;
  }
    
  c->Write();
  f->Write();
  cout << "Wrote file " << f->GetName() << endl;
  f->Close();
  return c;
}

TCanvas* print_canvas(TString hist, TString type, int set, double xmin, double xmax, bool stacks = true, TString name = "") {
  dataplotter_->xMin_ = xmin;
  dataplotter_->xMax_ = xmax;
  if(offsetSets_) {
    if(selection_ == "mutau"  ) set += ZTauTauHistMaker::kMuTau;
    if(selection_ == "etau"   ) set += ZTauTauHistMaker::kETau;
    if(selection_ == "emu"    ) set += ZTauTauHistMaker::kEMu;
    if(selection_ == "mutau_e") set += ZTauTauHistMaker::kMuTauE;
    if(selection_ == "etau_mu") set += ZTauTauHistMaker::kETauMu;
  }
  auto c = print_canvas(hist, type, set, stacks, name);
  dataplotter_->reset_axes();
  return c;
}

Int_t print_test(vector<int> sets, vector<double> signal_scales = {},
		 vector<int> base_rebins = {}) {

  cout << "Printing test\n";
  if(!dataplotter_) return 1;
  vector<DataPlotter::PlottingCard_t> plottingcards;
  // plotting card constructor:                       hist, type, rebin, xmin, xmax, blindmin, blindmax
  if(selection_ == "emu" || selection_.Contains("tau_")) {
    plottingcards.push_back(DataPlotter::PlottingCard_t("onept",          "lep",   2, 15.,  150. ));
    plottingcards.push_back(DataPlotter::PlottingCard_t("twopt",          "lep",   2, 15.,  150. ));
  } else {
    plottingcards.push_back(DataPlotter::PlottingCard_t("onept",          "lep",   2, 15.,  150. ));
    plottingcards.push_back(DataPlotter::PlottingCard_t("twopt",          "lep",   2, 15.,  150. ));
  }
  dataplotter_->logY_ = 0;
  int status = 0;
  if(offsetSets_) {
    for(unsigned i = 0; i < sets.size(); ++ i) {
      if(selection_ == "mutau"  ) sets[i] += ZTauTauHistMaker::kMuTau;
      if(selection_ == "etau"   ) sets[i] += ZTauTauHistMaker::kETau;
      if(selection_ == "emu"    ) sets[i] += ZTauTauHistMaker::kEMu;
      if(selection_ == "mutau_e") sets[i] += ZTauTauHistMaker::kMuTauE;
      if(selection_ == "etau_mu") sets[i] += ZTauTauHistMaker::kETauMu;
    }
  }
  status = dataplotter_->print_stacks(plottingcards, sets, signal_scales, base_rebins);
  return status;
}

//print the nanoAOD object counting selections for each selection set
Int_t print_slim_counts(vector<int> sets) {
  cout << "Printing slim counts\n";
  if(!dataplotter_) return 1;
  if(offsetSets_) {
    for(unsigned i = 0; i < sets.size(); ++ i) {
      if(selection_ == "mutau"  ) sets[i] += ZTauTauHistMaker::kMuTau;
      if(selection_ == "etau"   ) sets[i] += ZTauTauHistMaker::kETau;
      if(selection_ == "emu"    ) sets[i] += ZTauTauHistMaker::kEMu;
      if(selection_ == "mutau_e") sets[i] += ZTauTauHistMaker::kMuTauE;
      if(selection_ == "etau_mu") sets[i] += ZTauTauHistMaker::kETauMu;
    }
  }
  Int_t status = 0;
  vector<DataPlotter::PlottingCard_t> plottingcards;
  //muon cards
  plottingcards.push_back(DataPlotter::PlottingCard_t("nslimmuons"    , "event", 1, 0, 6));
  for(int i = 0; i < 24; ++i)
    plottingcards.push_back(DataPlotter::PlottingCard_t(Form("nmuoncounts%i",i)    , "event", 1, 0, 6));
  //electron cards
  plottingcards.push_back(DataPlotter::PlottingCard_t("nslimelectrons"    , "event", 1, 0, 6));
  for(int i = 0; i < 6; ++i)
    plottingcards.push_back(DataPlotter::PlottingCard_t(Form("nelectroncounts%i",i), "event", 1, 0, 6));
  //tau cards
  plottingcards.push_back(DataPlotter::PlottingCard_t("nslimtaus"    , "event", 1, 0, 6));
  for(int i = 0; i < 18; ++i)
    plottingcards.push_back(DataPlotter::PlottingCard_t(Form("ntaucounts%i",i)     , "event", 1, 0, 6));
  //print the cards
  dataplotter_->logY_ = 1;
  status = dataplotter_->print_stacks(plottingcards, sets, {}, {});
  dataplotter_->logY_ = 0;
  status = dataplotter_->print_stacks(plottingcards, sets, {}, {});
  return status;
}

//print many distributions for the selection sets
Int_t print_standard_plots(vector<int> sets, vector<double> signal_scales = {},
			   vector<int> base_rebins = {}, bool stacks = true) {
  if(!dataplotter_) return -1;
  if(offsetSets_) {
    for(unsigned i = 0; i < sets.size(); ++ i) {
      if(selection_ == "mutau"  ) sets[i] += ZTauTauHistMaker::kMuTau;
      if(selection_ == "etau"   ) sets[i] += ZTauTauHistMaker::kETau;
      if(selection_ == "emu"    ) sets[i] += ZTauTauHistMaker::kEMu;
      if(selection_ == "mutau_e") sets[i] += ZTauTauHistMaker::kMuTauE;
      if(selection_ == "etau_mu") sets[i] += ZTauTauHistMaker::kETauMu;
    }
  }
  vector<DataPlotter::PlottingCard_t> plottingcards;
  vector<DataPlotter::PlottingCard_t> mvaplottingcards; //to plot once normally and once with just total background

  // plotting card constructor:                       hist, type, rebin, xmin, xmax, blindmin, blindmax
  if(selection_ == "emu" || selection_.Contains("tau_")) {
    plottingcards.push_back(DataPlotter::PlottingCard_t("onept",          "lep",   2,10.,  100. ));
    plottingcards.push_back(DataPlotter::PlottingCard_t("twopt",          "lep",   2, 5.,  100. ));
  } else {
    plottingcards.push_back(DataPlotter::PlottingCard_t("onept",          "lep",   2, 25.,  100. ));
    plottingcards.push_back(DataPlotter::PlottingCard_t("twopt",          "lep",   2, 20.,   80. ));
  }
  plottingcards.push_back(DataPlotter::PlottingCard_t("oneeta",         "lep",   2, -3.,    5. ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("twoeta",         "lep",   2, -3.,    5. ));
  if(selection_ == "emu" || selection_.Contains("tau_")) { //emu dataset
    plottingcards.push_back(DataPlotter::PlottingCard_t("lepm",         "event", 2, 50.,   170., {84, 118}, {98, 132.} ));
  } else //etau or mutau
    plottingcards.push_back(DataPlotter::PlottingCard_t("lepm",         "event", 2, 50.,   170. ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("leppt",          "event", 2, 0.,   150. ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("lepptoverm",     "event", 2, 0.,   5.   ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("lepeta",         "event", 5, -5.,  5.   ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("lepdeltaeta",    "event", 5, 0.,   5.   ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("lepdeltaphi",    "event", 1, 0.,   5.0  ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("lepdeltar",      "event", 1, 0.,   5.   ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("pxivis0",        "event", 5, 0.,   100. ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("pxiinv0",        "event", 5, -100.,100. ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("ptauvisfrac",    "event", 5, 0.,   1.4  ));
  if(selection_ == "emu" || selection_ == "llg_study") {
    plottingcards.push_back(DataPlotter::PlottingCard_t("lepmestimate",   "event", 1, 50.,  200. ));
    plottingcards.push_back(DataPlotter::PlottingCard_t("lepmestimatetwo","event", 1, 50.,  200. ));
  } else {
    plottingcards.push_back(DataPlotter::PlottingCard_t("lepmestimate",   "event", 1, 50.,  200.));
    plottingcards.push_back(DataPlotter::PlottingCard_t("lepmestimatetwo","event", 1, 50.,  200.));
  }
  plottingcards.push_back(DataPlotter::PlottingCard_t("met",            "event", 2, 0.,   120. ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("puppmet",        "event", 2, 0.,   120. ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("covmet00",       "event", 10,100., 1000.));
  plottingcards.push_back(DataPlotter::PlottingCard_t("covmet11",       "event", 10,100., 1000.));
  plottingcards.push_back(DataPlotter::PlottingCard_t("covmet01",       "event", 1, -100.,100. ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("mtone",          "event", 4, 0.,   150. ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("mttwo",          "event", 4, 0.,   150. ));
  if(selection_ == "mutau" || selection_ == "etau" || selection_ == "llg_study") { //tau only sets
    plottingcards.push_back(DataPlotter::PlottingCard_t("twom",          "lep", 2, 0.,   3. ));
    plottingcards.push_back(DataPlotter::PlottingCard_t("taudeepantiele","event", 0, 0.,  300. ));
    plottingcards.push_back(DataPlotter::PlottingCard_t("taudeepantimu", "event", 0, 0.,   15. ));
    plottingcards.push_back(DataPlotter::PlottingCard_t("taudeepantijet","event", 0, 0.,  300. ));
    plottingcards.push_back(DataPlotter::PlottingCard_t("taugenflavor",  "event", 0, 0.,   10. ));
    plottingcards.push_back(DataPlotter::PlottingCard_t("taudecaymode",  "event", 0, 0.,   15. ));
  }
  //ignore SVfit results for now, not very helpful
  // hnames.push_back("onesvpt");        htypes.push_back("lep");   rebins.push_back(1); xmins.push_back(15.);  xmaxs.push_back(150.);
  // hnames.push_back("twosvpt");        htypes.push_back("lep");   rebins.push_back(1); xmins.push_back(15.);  xmaxs.push_back(150.);
  // hnames.push_back("onesvdeltapt");   htypes.push_back("lep");   rebins.push_back(1); xmins.push_back(15.);  xmaxs.push_back(150.);
  // hnames.push_back("twosvdeltapt");   htypes.push_back("lep");   rebins.push_back(1); xmins.push_back(15.);  xmaxs.push_back(200.);
  // hnames.push_back("lepsvm");         htypes.push_back("event"); rebins.push_back(5); xmins.push_back(0.);   xmaxs.push_back(300.);
  // hnames.push_back("lepsvdeltam");    htypes.push_back("event"); rebins.push_back(2); xmins.push_back(-10.); xmaxs.push_back(90.);
  // hnames.push_back("lepsvpt");        htypes.push_back("event"); rebins.push_back(2); xmins.push_back(0.);   xmaxs.push_back(150.);
  // hnames.push_back("lepsvdeltapt");   htypes.push_back("event"); rebins.push_back(2); xmins.push_back(-10.); xmaxs.push_back(90.);
  // hnames.push_back("lepsvptoverm");   htypes.push_back("event"); rebins.push_back(2); xmins.push_back(0.);   xmaxs.push_back(10.);

  //leading jet info
  plottingcards.push_back(DataPlotter::PlottingCard_t("jetpt",          "event", 2, 15.,  250.));
  plottingcards.push_back(DataPlotter::PlottingCard_t("jeteta",         "event", 2, -3.,  3.));
  plottingcards.push_back(DataPlotter::PlottingCard_t("jetm",           "event", 2,  0.,  50.));
  plottingcards.push_back(DataPlotter::PlottingCard_t("jetbmva",        "event", 2, -1.,  1.));
  plottingcards.push_back(DataPlotter::PlottingCard_t("jetbtag",        "event", 0,  0.,  2.));

  //jet counts
  plottingcards.push_back(DataPlotter::PlottingCard_t("njets",          "event", 0, 0.,   10. ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("njets25",        "event", 0, 0.,   10. ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("njets20",        "event", 0, 0.,   10. ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("njets25tot",     "event", 0, 0.,   10. ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("njetstot",       "event", 0, 0.,   10. ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("nbjets",         "event", 0, 0.,   10. ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("nbjetsm",        "event", 0, 0.,   10. ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("nbjetsl",        "event", 0, 0.,   10. ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("nbjets25",       "event", 0, 0.,   10. ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("nbjets25m",      "event", 0, 0.,   10. ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("nbjets25l",      "event", 0, 0.,   10. ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("nbjets20",       "event", 0, 0.,   10. ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("nbjets20m",      "event", 0, 0.,   10. ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("nbjets20l",      "event", 0, 0.,   10. ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("nbjets25tot",    "event", 0, 0.,   10. ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("nbjets25totm",   "event", 0, 0.,   10. ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("nbjets25totl",   "event", 0, 0.,   10. ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("nbjetstot",      "event", 0, 0.,   10. ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("nbjetstotm",     "event", 0, 0.,   10. ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("nbjetstotl",     "event", 0, 0.,   10. ));

  plottingcards.push_back(DataPlotter::PlottingCard_t("nfwdjets",       "event", 0, 0.,   10. ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("nphotons",       "event", 0, 0.,   5.  ));
  
  if(selection_ == "mutau") {
    mvaplottingcards.push_back(DataPlotter::PlottingCard_t("mva0",       "event", 200, -1.,   1. , 0.01, 1. )); //H MVA
    mvaplottingcards.push_back(DataPlotter::PlottingCard_t("mva1",       "event", 200, -1.,   1. , 0.01, 1. )); //Z MVA
    mvaplottingcards.push_back(DataPlotter::PlottingCard_t("mvatest0",   "event",   2, -1.,   1. , 0.01, 1. )); //H MVA
    mvaplottingcards.push_back(DataPlotter::PlottingCard_t("mvatest1",   "event",   2, -1.,   1. , 0.01, 1. )); //Z MVA
    mvaplottingcards.push_back(DataPlotter::PlottingCard_t("mvatrain0",  "event",   2, -1.,   1. , 0.01, 1. )); //H MVA
    mvaplottingcards.push_back(DataPlotter::PlottingCard_t("mvatrain1",  "event",   2, -1.,   1. , 0.01, 1. )); //Z MVA
  } else if(selection_ == "etau") {
    mvaplottingcards.push_back(DataPlotter::PlottingCard_t("mva2",       "event", 200, -1.,   1. , 0.01, 1. )); //H MVA
    mvaplottingcards.push_back(DataPlotter::PlottingCard_t("mva3",       "event", 200, -1.,   1. , 0.01, 1. )); //Z MVA
    mvaplottingcards.push_back(DataPlotter::PlottingCard_t("mvatest2",   "event",   2, -1.,   1. , 0.01, 1. )); //H MVA
    mvaplottingcards.push_back(DataPlotter::PlottingCard_t("mvatest3",   "event",   2, -1.,   1. , 0.01, 1. )); //Z MVA
    mvaplottingcards.push_back(DataPlotter::PlottingCard_t("mvatrain2",  "event",   2, -1.,   1. , 0.01, 1. )); //H MVA
    mvaplottingcards.push_back(DataPlotter::PlottingCard_t("mvatrain3",  "event",   2, -1.,   1. , 0.01, 1. )); //Z MVA
  } else if(selection_ == "emu" || selection_.Contains("tau_")) { //print all MVAs for emu dataset categories
    int mvas[] = {4,5, //H, Z -> emu
		  6,7, //H, Z -> mutau_e
		  8,9};//H, Z ->etau_mu
    for(int mva_i = 0; mva_i < sizeof(mvas)/sizeof(*mvas); ++mva_i) {
      mvaplottingcards.push_back(DataPlotter::PlottingCard_t(Form("mva%i"     ,mvas[mva_i]), "event", 200, -1.,   1. , 0.01, 1. ));
      mvaplottingcards.push_back(DataPlotter::PlottingCard_t(Form("mvatest%i" ,mvas[mva_i]), "event",   2, -1.,   1. , 0.01, 1. ));
      mvaplottingcards.push_back(DataPlotter::PlottingCard_t(Form("mvatrain%i",mvas[mva_i]), "event",   2, -1.,   1. , 0.01, 1. ));
    }
  }
    
  plottingcards.push_back(DataPlotter::PlottingCard_t("htsum",             "event", 5, 0.,   800.));
  plottingcards.push_back(DataPlotter::PlottingCard_t("ht",                "event", 5, 0.,   800.));
  plottingcards.push_back(DataPlotter::PlottingCard_t("htdeltaphi",        "event", 1, 0.,   5.0 ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("metdeltaphi",       "event", 1, 0.,   5.0 ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("leponedeltaphi",    "event", 1, 0.,   5.0 ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("leptwodeltaphi",    "event", 1, 0.,   5.0 ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("onemetdeltaphi",    "lep",   1, 0.,   5.0 ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("twometdeltaphi",    "lep",   1, 0.,   5.0 ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("leponejetdeltaphi", "event", 1, 0.,   5.0 ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("leptwojetdeltaphi", "event", 1, 0.,   5.0 ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("leponejetdeltaeta", "event", 1, 0.,   6.  ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("leptwojetdeltaeta", "event", 1, 0.,   6.  ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("leponejetdeltar",   "event", 1, 0.,   6.  ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("leptwojetdeltar",   "event", 1, 0.,   6.  ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("oned0",             "lep",   2, -0.05,0.05));
  plottingcards.push_back(DataPlotter::PlottingCard_t("oneiso",            "lep",   1, 0.,   10. ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("onereliso",         "lep",   1, 0.,   0.15));
  plottingcards.push_back(DataPlotter::PlottingCard_t("twod0",             "lep",   2, -0.05,0.05));

  plottingcards.push_back(DataPlotter::PlottingCard_t("oneid1",  "lep",   0, 0,10));
  plottingcards.push_back(DataPlotter::PlottingCard_t("twoid1",  "lep",   0, 0,50));
  plottingcards.push_back(DataPlotter::PlottingCard_t("oneid2",  "lep",   0, 0,10));
  plottingcards.push_back(DataPlotter::PlottingCard_t("twoid2",  "lep",   0, 0,10));
    
    
  plottingcards.push_back(DataPlotter::PlottingCard_t("oneslimeq",  "lep",   0, -2,4));
  plottingcards.push_back(DataPlotter::PlottingCard_t("twoslimeq",  "lep",   0, -2,4));
  plottingcards.push_back(DataPlotter::PlottingCard_t("oneslimem",  "lep",   1,  0,200));
  plottingcards.push_back(DataPlotter::PlottingCard_t("twoslimem",  "lep",   1,  0,200));
  plottingcards.push_back(DataPlotter::PlottingCard_t("oneslimemss",  "lep",   1,  0,200));
  plottingcards.push_back(DataPlotter::PlottingCard_t("twoslimemss",  "lep",   1,  0,200));
  plottingcards.push_back(DataPlotter::PlottingCard_t("oneslimemos",  "lep",   1,  0,200));
  plottingcards.push_back(DataPlotter::PlottingCard_t("twoslimemos",  "lep",   1,  0,200));

  plottingcards.push_back(DataPlotter::PlottingCard_t("oneslimmuq",  "lep",   0, -2,4));
  plottingcards.push_back(DataPlotter::PlottingCard_t("twoslimmuq",  "lep",   0, -2,4));
  plottingcards.push_back(DataPlotter::PlottingCard_t("oneslimmum",  "lep",   1,  0,200));
  plottingcards.push_back(DataPlotter::PlottingCard_t("twoslimmum",  "lep",   1,  0,200));
  plottingcards.push_back(DataPlotter::PlottingCard_t("oneslimmumss",  "lep",   1,  0,200));
  plottingcards.push_back(DataPlotter::PlottingCard_t("twoslimmumss",  "lep",   1,  0,200));
  plottingcards.push_back(DataPlotter::PlottingCard_t("oneslimmumos",  "lep",   1,  0,200));
  plottingcards.push_back(DataPlotter::PlottingCard_t("twoslimmumos",  "lep",   1,  0,200));

  plottingcards.push_back(DataPlotter::PlottingCard_t("oneslimtauq",  "lep",   0, -2,4));
  plottingcards.push_back(DataPlotter::PlottingCard_t("twoslimtauq",  "lep",   0, -2,4));
  plottingcards.push_back(DataPlotter::PlottingCard_t("oneslimtaum",  "lep",   1,  0,200));
  plottingcards.push_back(DataPlotter::PlottingCard_t("twoslimtaum",  "lep",   1,  0,200));
  plottingcards.push_back(DataPlotter::PlottingCard_t("oneslimtaumss",  "lep",   1,  0,200));
  plottingcards.push_back(DataPlotter::PlottingCard_t("twoslimtaumss",  "lep",   1,  0,200));
  plottingcards.push_back(DataPlotter::PlottingCard_t("oneslimtaumos",  "lep",   1,  0,200));
  plottingcards.push_back(DataPlotter::PlottingCard_t("twoslimtaumos",  "lep",   1,  0,200));
    
  if(selection_ == "emu" || selection_.Contains("tau_") /*leptonic tau decay*/ || selection_ == "llg_study") {
    plottingcards.push_back(DataPlotter::PlottingCard_t("twoiso",       "lep", 1, 0.,   10.  ));
    plottingcards.push_back(DataPlotter::PlottingCard_t("tworeliso",    "lep", 1, 0.,   0.15 ));
  }

  if(selection_ == "llg_study") {
    double lower[13] = { 0.,0.,0.,0.
			,0.,0.,0.,0.
			,0.,0.,0.,0.,0.};
    double upper[13] = { 400.,400.,400.,600.
			,400.,400.,400.,400.
			,600.,600.,600.,600.,600.};
    int rebins[13] =   {2, 2, 2, 5
			,2, 2, 2, 2
			,2 ,2 ,2 ,5 ,2};
    for(int i = 0; i < 13; ++i) {
      TString objmass = "objmasses";
      objmass += i;
      plottingcards.push_back(DataPlotter::PlottingCard_t(objmass.Data(), "event", 2, lower[i],  upper[i]));
    }
    plottingcards.push_back(DataPlotter::PlottingCard_t("jettwopt",       "event", 2, 20.,  250.));
    plottingcards.push_back(DataPlotter::PlottingCard_t("jettwoeta",      "event", 2, -3.,    3.));
    plottingcards.push_back(DataPlotter::PlottingCard_t("jettwom",        "event", 2,  0.,   50.));
    plottingcards.push_back(DataPlotter::PlottingCard_t("jettwobmva",     "event", 2, -1.,  1.));
    plottingcards.push_back(DataPlotter::PlottingCard_t("jettwobtag",     "event", 0,  0.,  2.));

    plottingcards.push_back(DataPlotter::PlottingCard_t("jetspt",         "event", 2,  0.,  300.));
    plottingcards.push_back(DataPlotter::PlottingCard_t("jetseta",        "event", 2, -6.,    6.));
    // jets mass = objmass0
    // plottingcards.push_back(DataPlotter::PlottingCard_t("jetsm",          "event", 2,  0.,  100.));
    plottingcards.push_back(DataPlotter::PlottingCard_t("jetsdeltar",     "event", 2,  0.,    6.));
    plottingcards.push_back(DataPlotter::PlottingCard_t("jetsdeltaeta",   "event", 2,  0.,    6.));
    plottingcards.push_back(DataPlotter::PlottingCard_t("jetsdeltaphi",   "event", 1,  0.,    4.));

    plottingcards.push_back(DataPlotter::PlottingCard_t("jetsgammapt",         "event", 2, 20.,  500.));
    plottingcards.push_back(DataPlotter::PlottingCard_t("jetsgammaeta",        "event", 2, -6.,    6.));
    // jets+gamma mass = objmass3
    // plottingcards.push_back(DataPlotter::PlottingCard_t("jetsgammam",          "event", 2,  0.,  100.));
    plottingcards.push_back(DataPlotter::PlottingCard_t("jetsgammadeltar",     "event", 2,  0.,    6.));
    plottingcards.push_back(DataPlotter::PlottingCard_t("jetsgammadeltaeta",   "event", 2,  0.,    6.));
    plottingcards.push_back(DataPlotter::PlottingCard_t("jetsgammadeltaphi",   "event", 1,  0.,    4.)); 
    //photon parameters
    plottingcards.push_back(DataPlotter::PlottingCard_t("pt",  "photon", 1,  5. ,  100.)); 
    plottingcards.push_back(DataPlotter::PlottingCard_t("eta", "photon", 1, -2.5,  2.5)); 
    plottingcards.push_back(DataPlotter::PlottingCard_t("p",   "photon", 1,  5. ,  100.)); 
    plottingcards.push_back(DataPlotter::PlottingCard_t("mva", "photon", 2,  0.1 , 1.)); 
    plottingcards.push_back(DataPlotter::PlottingCard_t("sysm","event",  2, 20., 250., {120.}, {130.} ));
    plottingcards.push_back(DataPlotter::PlottingCard_t("ntaus","event", 0, 0, 5));
  }


  
  TString label = "";
  if(selection_ == "emu")
    label = "->e#mu";
  else if(selection_.Contains("etau"))
    label = "->e#tau";
  else if(selection_.Contains("mutau"))
    label = "->#mu#tau";

  //2D histograms
  if(print2Ds_) {
    if(label != "") {
      for(int data = 0; data < 2; ++data) { //with/without data
	dataplotter_->plot_data_ = data;
	for(int logz = 0; logz < 2; ++logz) { //print log and not log z axis plots
	  dataplotter_->logZ_ = logz;
	  for(int s : sets) {
	    if(data) dataplotter_->rebinH_ = 4;
	    else 	 dataplotter_->rebinH_ = 2;

	    auto c = dataplotter_->print_single_2Dhist("pxiinvvsvis0", "event", s, ("Z"+label), 0, 80, -80, 60);
	    DataPlotter::Empty_Canvas(c);
	    c = dataplotter_->print_single_2Dhist("pxiinvvsvis0", "event", s, ("H"+label), 0, 80, -80, 60);
	    DataPlotter::Empty_Canvas(c);
	    c = dataplotter_->print_single_2Dhist("metvspt"     , "event", s, ("Z"+label), 0, 150,    0, 150);
	    DataPlotter::Empty_Canvas(c);
	    c = dataplotter_->print_single_2Dhist("metvspt"     , "event", s, ("H"+label), 0, 150,    0, 150);
	    DataPlotter::Empty_Canvas(c);
	    c = dataplotter_->print_single_2Dhist("twoptvsonept", "lep"  , s, ("Z->ee/#mu#mu"), 0, 150,    0, 150);
	    DataPlotter::Empty_Canvas(c);
	    c = dataplotter_->print_single_2Dhist("twoptvsonept", "lep"  , s, ("Z->#tau#tau"), 0, 150,    0, 150);
	    DataPlotter::Empty_Canvas(c);
	    c = dataplotter_->print_single_2Dhist("twoptvsonept", "lep"  , s, ("Z"+label), 0, 150,    0, 150);
	    DataPlotter::Empty_Canvas(c);
	    c = dataplotter_->print_single_2Dhist("twoptvsonept", "lep"  , s, ("H"+label), 0, 150,    0, 150);
	    DataPlotter::Empty_Canvas(c);
	  }
	}
      }
    } //end 2D loop
  }
  
  vector<DataPlotter::PlottingCard_t> cdfplottingcards;
  if(label != "") {
    if(selection_=="mutau") {
      cdfplottingcards.push_back(DataPlotter::PlottingCard_t("mva0", "event", ("H"+label), 1, 0., 1.7, 0.578, 1.7));
      cdfplottingcards.push_back(DataPlotter::PlottingCard_t("mva1", "event", ("Z"+label), 1, 0., 1.7, 0.578, 1.7));
    } else if(selection_ == "etau") {
      cdfplottingcards.push_back(DataPlotter::PlottingCard_t("mva2", "event", ("H"+label), 1, 0., 1.7, 0.578, 1.7));
      cdfplottingcards.push_back(DataPlotter::PlottingCard_t("mva3", "event", ("Z"+label), 1, 0., 1.7, 0.578, 1.7));
    } else if(selection_ == "emu") {
      cdfplottingcards.push_back(DataPlotter::PlottingCard_t("mva4", "event", ("H"+label), 1, 0., 1.7, 0.578, 1.7));
      cdfplottingcards.push_back(DataPlotter::PlottingCard_t("mva5", "event", ("Z"+label), 1, 0., 1.7, 0.578, 1.7));
    } else if(selection_ == "mutau_e") {
      cdfplottingcards.push_back(DataPlotter::PlottingCard_t("mva6", "event", ("H"+label), 1, 0., 1.7, 0.578, 1.7));
      cdfplottingcards.push_back(DataPlotter::PlottingCard_t("mva7", "event", ("Z"+label), 1, 0., 1.7, 0.578, 1.7));
    } else if(selection_ == "etau_mu") {
      cdfplottingcards.push_back(DataPlotter::PlottingCard_t("mva8", "event", ("H"+label), 1, 0., 1.7, 0.578, 1.7));
      cdfplottingcards.push_back(DataPlotter::PlottingCard_t("mva9", "event", ("Z"+label), 1, 0., 1.7, 0.578, 1.7));
    }
  }
  if(printCDFs_) {
    for(auto card : cdfplottingcards) {
      for(int logy = 0; logy < 2; ++logy) { //print log and not log y axis
	dataplotter_->logY_ = logy;
	for(int s : sets) {
	  card.set_ = s;
	  auto c = dataplotter_->print_cdf(card);
	  DataPlotter::Empty_Canvas(c);
	}
      }
    }
  }
  
  if(printSignificances_) {
    if(label != "") {
      for(int logy = 0; logy < 2; ++logy) { //print log and not log y axis
	dataplotter_->logY_ = logy;
	for(int s : sets) {
	  if(selection_=="mutau") {
	    auto c = dataplotter_->print_significance("mva0", "event", s, ("H"+label), -1., 1., true, -1., false, "CMS #sqrt{s}=13 TeV Limit (2.5e-3)");
	    DataPlotter::Empty_Canvas(c);
	    c = dataplotter_->print_significance("mva1", "event", s, ("Z"+label), -1., 1.3, true, -0.71, false, "LEP Limit (1.2e-5)");
	    DataPlotter::Empty_Canvas(c);
	    if(printLimitVsEff_) {
	      c = dataplotter_->print_significance("mva0", "event", s, ("H"+label), 0., 1., true, -1., true, "CMS #sqrt{s}=13 TeV Limit (2.5e-3)"); //significance vs efficiency
	      DataPlotter::Empty_Canvas(c);
	      c = dataplotter_->print_significance("mva1", "event", s, ("Z"+label), 0., 1., true, -0.71, true, "LEP Limit (1.2e-5)"); //significance vs efficiency
	      DataPlotter::Empty_Canvas(c);
	    }
	  }
	  else if(selection_=="etau") {
	    auto c = dataplotter_->print_significance("mva2", "event", s, ("H"+label), -1., 1.,true, -1., false, "CMS #sqrt{s}=13 TeV Limit (6.1e-3)");
	    DataPlotter::Empty_Canvas(c);									
	    c = dataplotter_->print_significance("mva3", "event", s, ("Z"+label), -1., 1.,true, -1., false, "LEP Limit (9.8e-6)");
	    DataPlotter::Empty_Canvas(c);
	    if(printLimitVsEff_) {
	      c = dataplotter_->print_significance("mva2", "event", s, ("H"+label), 0., 1., true, -1., true, "CMS #sqrt{s}=13 TeV Limit (6.1e-3)");
	      DataPlotter::Empty_Canvas(c);
	      c = dataplotter_->print_significance("mva3", "event", s, ("Z"+label), 0., 1., true, -1., true, "LEP Limit (9.8e-6)");
	      DataPlotter::Empty_Canvas(c);
	    }
	  }
	  else if(selection_=="emu") {
	    auto c = dataplotter_->print_significance("mva4", "event", s, ("H"+label), -1., 1., true, -1. , false, "CMS #sqrt{s}=8 TeV Limit (1.1e-4)");
	    DataPlotter::Empty_Canvas(c);
	    c = dataplotter_->print_significance("mva5", "event", s, ("Z"+label), -1., 1., true, -.44, false, "CMS #sqrt{s}=8 TeV Limit (7.3e-7)");
	    DataPlotter::Empty_Canvas(c);
	    if(printLimitVsEff_) {
	      c = dataplotter_->print_significance("mva4", "event", s, ("H"+label), 0., 1., true, -1. , true, "CMS #sqrt{s}=8 TeV Limit (1.1e-4)");
	      DataPlotter::Empty_Canvas(c);
	      c = dataplotter_->print_significance("mva5", "event", s, ("Z"+label), 0., 1., true, -.44, true, "CMS #sqrt{s}=8 TeV Limit (7.3e-7)");
	      DataPlotter::Empty_Canvas(c);
	    }
	  }
	  else if(selection_=="mutau_e") {
	    auto c = dataplotter_->print_significance("mva6", "event", s, ("H"+label), -1., 1., true, -1., false, "CMS #sqrt{s}=13 TeV Limit (2.5e-3)");
	    DataPlotter::Empty_Canvas(c);
	    c = dataplotter_->print_significance("mva7", "event", s, ("Z"+label), -1., 1.3, true, -0.71, false, "LEP Limit (1.2e-5)");
	    DataPlotter::Empty_Canvas(c);
	    if(printLimitVsEff_) {	  
	      c = dataplotter_->print_significance("mva6", "event", s, ("H"+label), 0., 1., true, -1., true, "CMS #sqrt{s}=13 TeV Limit (2.5e-3)"); //significance vs efficiency
	      DataPlotter::Empty_Canvas(c);
	      c = dataplotter_->print_significance("mva7", "event", s, ("Z"+label), 0., 1., true, -0.71, true, "LEP Limit (1.2e-5)"); //significance vs efficiency
	      DataPlotter::Empty_Canvas(c);
	    }
	  }
	  else if(selection_=="etau_mu") {
	    auto c = dataplotter_->print_significance("mva8", "event", s, ("H"+label), -1., 1.,true, -1., false, "CMS #sqrt{s}=13 TeV Limit (6.1e-3)");
	    DataPlotter::Empty_Canvas(c);									
	    c = dataplotter_->print_significance("mva9", "event", s, ("Z"+label), -1., 1.,true, -1., false, "LEP Limit (9.8e-6)");
	    DataPlotter::Empty_Canvas(c);
	    if(printLimitVsEff_) {
	      c = dataplotter_->print_significance("mva8", "event", s, ("H"+label), 0., 1., true, -1., true, "CMS #sqrt{s}=13 TeV Limit (6.1e-3)");
	      DataPlotter::Empty_Canvas(c);
	      c = dataplotter_->print_significance("mva9", "event", s, ("Z"+label), 0., 1., true, -1., true, "LEP Limit (9.8e-6)");
	      DataPlotter::Empty_Canvas(c);
	    }
	  }
	}
      }
    } //end significance loop
  }
  
  //not log plots

  dataplotter_->logY_ = 0;
  int status = 0;
  //standard plots
  status = (stacks) ? dataplotter_->print_stacks(plottingcards, sets, signal_scales, base_rebins) :
    dataplotter_->print_hists(plottingcards, sets, signal_scales, base_rebins);
  if(status) return status;
  //mva plots
  if(printMVAPlots_) {
    status = dataplotter_->print_stacks(mvaplottingcards, sets, signal_scales, base_rebins);
    if(status) return status;
  }
  //mva plots as tot bkg vs signal
  if(printMVATotBkg_) {
    dataplotter_->stack_as_hist_ = 1;
    dataplotter_->draw_statistics_ = false;
    status = dataplotter_->print_stacks(mvaplottingcards, sets, signal_scales, base_rebins);
    if(status) return status;
  }
  dataplotter_->stack_as_hist_ = 0;
  dataplotter_->draw_statistics_ = true;

  //log y plots
  dataplotter_->logY_ = 1;
  //standard plots
  status = (stacks) ? dataplotter_->print_stacks(plottingcards, sets, signal_scales, base_rebins) :
    dataplotter_->print_hists(plottingcards, sets, signal_scales, base_rebins);
  //mva plots
  if(printMVAPlots_) {
    status = dataplotter_->print_stacks(mvaplottingcards, sets, signal_scales, base_rebins);
    if(status) return status;
  }
  if(status) return status;
  //mva plots as tot bkg vs signal
  if(printMVATotBkg_) {
    dataplotter_->stack_as_hist_ = 1;
    dataplotter_->draw_statistics_ = false;
    status = dataplotter_->print_stacks(mvaplottingcards, sets, signal_scales, base_rebins);
    if(status) return status;
  }
  dataplotter_->stack_as_hist_ = 0;
  dataplotter_->draw_statistics_ = true;
  dataplotter_->logY_ = 0;
  bool prev = offsetSets_;
  offsetSets_ = false;
  if(printSlimCounts_) status += print_slim_counts(sets);
  offsetSets_ = prev;
  return status;
}

Int_t print_standard_canvases(vector<int> sets, vector<double> signal_scales = {},
			      vector<int> base_rebins = {}, bool stacks = true, TString name = "") {
  if(!dataplotter_) return -1;
  if(offsetSets_) {
    for(unsigned i = 0; i < sets.size(); ++ i) {
      if(selection_ == "mutau"  ) sets[i] += ZTauTauHistMaker::kMuTau;
      if(selection_ == "etau"   ) sets[i] += ZTauTauHistMaker::kETau;
      if(selection_ == "emu"    ) sets[i] += ZTauTauHistMaker::kEMu;
      if(selection_ == "mutau_e") sets[i] += ZTauTauHistMaker::kMuTauE;
      if(selection_ == "etau_mu") sets[i] += ZTauTauHistMaker::kETauMu;
    }
  }
  vector<TString> hnames;
  vector<TString> htypes;
  vector<int>     rebins;
  vector<double>  xmins;
  vector<double>  xmaxs;
  
  hnames.push_back("onept");          htypes.push_back("lep");   rebins.push_back(2); xmins.push_back(15.);  xmaxs.push_back(150.);
  hnames.push_back("twopt");          htypes.push_back("lep");   rebins.push_back(2); xmins.push_back(0.);   xmaxs.push_back(150.);
  hnames.push_back("lepm");           htypes.push_back("event"); rebins.push_back(2); xmins.push_back(0.);   xmaxs.push_back(200.);
  hnames.push_back("leppt");          htypes.push_back("event"); rebins.push_back(2); xmins.push_back(0.);   xmaxs.push_back(150.);
  hnames.push_back("lepmestimate");   htypes.push_back("event"); rebins.push_back(1); xmins.push_back(0.);   xmaxs.push_back(300.);
  hnames.push_back("met");            htypes.push_back("event"); rebins.push_back(2); xmins.push_back(0.);   xmaxs.push_back(150.);
  hnames.push_back("mtone");          htypes.push_back("event"); rebins.push_back(4); xmins.push_back(0.);   xmaxs.push_back(150.);
  hnames.push_back("mttwo");          htypes.push_back("event"); rebins.push_back(4); xmins.push_back(0.);   xmaxs.push_back(150.);
  hnames.push_back("njets");          htypes.push_back("event"); rebins.push_back(1); xmins.push_back(0.);   xmaxs.push_back(10.);
  int status = 0;
  for(int set : sets) {
    for(unsigned index = 0; index < hnames.size(); ++index) {
      dataplotter_->rebinH_ = rebins[index];
      dataplotter_->logY_ = 0;
      auto c = print_canvas(hnames[index], htypes[index], set, xmins[index], xmaxs[index], stacks, name);
      status += !c;
      dataplotter_->logY_ = 1;
      c = print_canvas(hnames[index], htypes[index], set, xmins[index], xmaxs[index], stacks, name);
      status += !c;
    }
  }
  dataplotter_->rebinH_ = 1;
  dataplotter_->logY_ = 0;
  return status;
}

Int_t init_dataplotter() {

  bool leptonic_tau = (selection_.Contains("tau_")); //mutau_l, etau_l
  if(dataplotter_) delete dataplotter_;
  dataplotter_ = new DataPlotter();
  if(sigOverBkg_) dataplotter_->data_over_mc_ = -sigOverBkg_;
  dataplotter_->selection_ = selection_;
  dataplotter_->folder_ = folder_;
  if(selection_ == "mutau")
    dataplotter_->qcd_scale_ = 1.059;
  else if(selection_ == "etau")
    dataplotter_->qcd_scale_ = 1.157;
  else
    dataplotter_->qcd_scale_ = 1.;
  
  dataplotter_->doStatsLegend_ = doStatsLegend_;
  if(selection_ == "emu")
    dataplotter_->signal_scale_ = 25.;
  else
    dataplotter_->signal_scale_ = 250.;
  dataplotter_->useOpenGL_ = (gROOT->IsBatch()) ? 0 : useOpenGL_;

  typedef DataPlotter::DataCard_t dcard;

  ///////////////////////////////////
  // Defining NANO AOD based files //
  ///////////////////////////////////

  std::vector<dcard> nano_cards;
  //card constructor:                filepath,                           name,                  label,              isData, xsec,  isSignal, color
  nano_cards.push_back(dcard(Form("clfv_%i_DY50"               ,year_), "DY50"               , "Drell-Yan"         , false, 6225.42, false, kRed-7));
  nano_cards.push_back(dcard(Form("clfv_%i_SingleAntiToptW"    ,year_), "SingleAntiToptW"    , "SingleTop"         , false, 34.91  , false, kCyan-7));
  nano_cards.push_back(dcard(Form("clfv_%i_SingleToptW"        ,year_), "SingleToptW"        , "SingleTop"         , false, 34.91  , false, kCyan-7));
  nano_cards.push_back(dcard(Form("clfv_%i_WWW"                ,year_), "WWW"	             , "ZZ,WZ,WWW"         , false, 0.2086 , false, kViolet-2));
  nano_cards.push_back(dcard(Form("clfv_%i_WZ"                 ,year_), "WZ"                 , "ZZ,WZ,WWW"         , false, 27.6   , false, kViolet-2));
  nano_cards.push_back(dcard(Form("clfv_%i_ZZ"                 ,year_), "ZZ"	             , "ZZ,WZ,WWW"         , false, 12.14  , false, kViolet-2));
  nano_cards.push_back(dcard(Form("clfv_%i_WW"                 ,year_), "WW"	             , "WW"                , false, 12.178 , false, kViolet-9));
  nano_cards.push_back(dcard(Form("clfv_%i_Wlnu"               ,year_), "Wlnu"               , "W+Jets"            , false, 52850.0, false, kGreen-7));
  nano_cards.push_back(dcard(Form("clfv_%i_ttbarToSemiLeptonic",year_), "ttbarToSemiLeptonic", "t#bar{t}"          , false, 365.34 , false, kYellow-7));
  nano_cards.push_back(dcard(Form("clfv_%i_ttbarlnu"           ,year_), "ttbarlnu"           , "t#bar{t}"          , false, 88.29  , false, kYellow-7));
  if(year_ == 2016) { //temporary fix
    if(selection_ == "emu") {
      nano_cards.push_back(dcard(Form("clfv_%i_ZEMu"             ,year_), "ZEMu"             , "Z->e#mu"   , false, 2075.14/0.0337*7.3e-7, true, kBlue));  
      nano_cards.push_back(dcard(Form("clfv_%i_HEMu"             ,year_), "HEMu"             , "H->e#mu"   , false, (48.61+3.766+0.5071+1.358+0.880)*3.5e-4, true, kGreen-1));
    } else if(selection_.Contains("etau")) {
      nano_cards.push_back(dcard(Form("clfv_%i_ZETau"            ,year_), "ZETau"            , "Z->e#tau"  , false, ((6225.42)/(3.*3.3658e-2))*9.8e-6, true, kBlue));  
      nano_cards.push_back(dcard(Form("clfv_%i_HETau"            ,year_), "HETau"            , "H->e#tau"  , false, (48.61+3.766+0.5071+1.358+0.880)*6.1e-3, true, kGreen-1));
    } else if(selection_.Contains("mutau")) {
      nano_cards.push_back(dcard(Form("clfv_%i_ZMuTau"           ,year_), "ZMuTau"           , "Z->#mu#tau", false, ((6225.42)/(3.*3.3658e-2))*1.2e-5, true, kBlue));  
      nano_cards.push_back(dcard(Form("clfv_%i_HMuTau"           ,year_), "HMuTau"           , "H->#mu#tau", false, (48.61+3.766+0.5071+1.358+0.880)*2.5e-3, true, kGreen-1));
    }
  } else
    nano_cards.push_back(dcard(Form("clfv_%i_Signal"           ,year_), "Signal"             , "Z->e#mu"  , false, 2075.14/0.0337*7.3e-7, true , kBlue));  
  if(selection_ != "etau" ) nano_cards.push_back(dcard(Form("clfv_%i_SingleMu"           ,year_), "SingleMu"           , "Data"     , true , 1.                   , false));  
  if(selection_ != "mutau") nano_cards.push_back(dcard(Form("clfv_%i_SingleEle"          ,year_), "SingleEle"          , "Data"     , true , 1.                   , false));  

  TString selection_dir = (leptonic_tau) ? "emu" : selection_;
  //add full name to file name
  for(unsigned index = 0; index < nano_cards.size(); ++index)
    nano_cards[index].filename_ = Form("%s/ztautau_%s_%s.hist", hist_dir_.Data(), selection_dir.Data(),
				       (nano_cards[index].filename_).Data());

  
  
  double lum = 35.92e3; //pb^-1
  if(year_ == 2017)
    lum = 41.48e3;
  else if(year_ == 2018)
    lum = 59.74e3;
  
  dataplotter_->set_luminosity(lum);
  dataplotter_->verbose_ = verbose_;

  for(auto card : nano_cards)
    dataplotter_->add_dataset(card);

  //make figure directory if it doesn't exist
  gSystem->Exec(Form("[ ! -d \"figures/%s/%s/%i\" ] && mkdir -p \"figures/%s/%s/%i\"",
		     folder_.Data(), selection_.Data(), year_, folder_.Data(), selection_.Data(), year_)); 
  dataplotter_->year_ = year_;
  return dataplotter_->init_files();
}

Int_t nanoaod_init(TString selection = "emu", TString histDir = "nanoaods", TString figureDir = "nanoaods") {
  hist_dir_ = histDir;
  folder_ = figureDir;
  selection_ = selection;
  Int_t status = init_dataplotter();
  if(!status) {
    if(selection == "emu")
      dataplotter_->signal_scale_ = 20.;
    else if(selection.Contains("_"))
      dataplotter_->signal_scale_ = 50.;
    else
      dataplotter_->signal_scale_ = 150.;
  }
  return status;
}

Int_t print_emu_dataset_plots(TString histDir = "", TString figureDir = "") {
  if(histDir != "") hist_dir_ = histDir;
  if(figureDir != "") folder_ = figureDir;
  TStopwatch* timer = new TStopwatch();  
  Int_t status = 0;
  vector<DataPlotter::PlottingCard_t> plottingcards;
  vector<int>    sets   = {  48,  49, 50,   89,  90,  91,  92};
  vector<double> scales = {250., 25., 25., 25., 25., 25., 25.}; 
  // vector<int>    rebins = {   1,   1,   2,   2,   2,   2,   2};
  doAllEMu_ = true;
  selection_ = "emu";
  init_dataplotter();
  dataplotter_->stack_as_hist_ = 1;
  dataplotter_->draw_statistics_ = false;
  dataplotter_->legend_y2_ = 0.25;

  plottingcards.push_back(DataPlotter::PlottingCard_t("mva4", "event", 200, -1.,   1. , 0.01, 1. ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("mva5", "event", 200, -1.,   1. , 0.01, 1. ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("mva6", "event", 200, -1.,   1. , 0.01, 1. ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("mva7", "event", 200, -1.,   1. , 0.01, 1. ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("mva8", "event", 200, -1.,   1. , 0.01, 1. ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("mva9", "event", 200, -1.,   1. , 0.01, 1. ));

  for(unsigned i = 0; i < sets.size(); ++i) {
    dataplotter_->signal_scale_ = scales[i];
    for(DataPlotter::PlottingCard_t card : plottingcards) {
      card.set_ = sets[i];
      TCanvas* c = dataplotter_->plot_stack(card);
      if(c)
	c->Print(Form("figures/%s/debug/stack_%s_%i_dataOverMC.png", folder_.Data(),card.hist_.Data(), sets[i]));
      else
	++status;
      DataPlotter::Empty_Canvas(c);
    }
  }

  Double_t cpuTime = timer->CpuTime();
  Double_t realTime = timer->RealTime();
  printf("Processing time: %7.2fs CPU time %7.2fs Wall time\n",cpuTime,realTime);
  if(realTime > 600. ) printf("Processing time: %7.2fmin CPU time %7.2fmin Wall time\n",cpuTime/60.,realTime/60.);

  return status;
}

//specific plots that require special ranges for X->e+mu
Int_t print_emu_cutsets() {
  Int_t status = 0;
  selection_ = "emu";
  init_dataplotter();
  vector<DataPlotter::PlottingCard_t> plottingcards;
  plottingcards.push_back(DataPlotter::PlottingCard_t("lepm", "event", 1, 75.,   110., {84, 118}, {98, 132.} ));
  plottingcards.push_back(DataPlotter::PlottingCard_t("met" , "event", 1, 0., 45.));
  plottingcards.push_back(DataPlotter::PlottingCard_t("puppmet" , "event", 1, 0., 45.));
  vector<Int_t> sets = {6,7,8};
  vector<Double_t> scales = {50.,50., 50.};
  vector<Int_t> rebins = {1,1,1};
  dataplotter_->logY_ = 1;
  status += dataplotter_->print_stacks(plottingcards, sets, scales, rebins);
  dataplotter_->logY_ = 0;
  status += dataplotter_->print_stacks(plottingcards, sets, scales, rebins);
  return status;
}

Int_t print_card_standard_sets(DataPlotter::PlottingCard_t& card) {
  vector<int> sets = {6, 7, 8};
  vector<TString> selections = {"mutau", "etau", "emu", "mutau_e", "etau_mu"};
  Int_t status = 0;
  for(TString selection : selections) {
    selection_ = selection;
    status += init_dataplotter();
    for(int index = 0; index < sets.size(); ++index) {
      card.set_ = sets[index];
      double scale = 150.;
      if     (selection_ == "mutau"  ) card.set_ += ZTauTauHistMaker::kMuTau;
      else if(selection_ == "etau"   ) card.set_ += ZTauTauHistMaker::kETau;
      else if(selection_ == "emu"    ) {scale = 100.; card.set_ += ZTauTauHistMaker::kEMu;}
      else if(selection_ == "mutau_e") {scale = 100.; card.set_ += ZTauTauHistMaker::kEMu;}
      else if(selection_ == "etau_mu") {scale = 100.; card.set_ += ZTauTauHistMaker::kEMu;}
      dataplotter_->signal_scale_ = scale;
      auto c = dataplotter_->print_stack(card);
      status += (c) ? 0 : 1;
      if(c) DataPlotter::Empty_Canvas(c);
    }
  }
  return status;
}

//configure the plots to have no data
Int_t print_blind_sets(vector<int> sets, vector<double> scales = {}, vector<int> rebins = {}) {
  Int_t status = 0;
  if(!dataplotter_) return 1;
  dataplotter_->plot_data_ = 0;
  if (sigOverBkg_ == 0) dataplotter_->data_over_mc_ = -2;
  status = print_standard_plots(sets, scales, rebins);
  dataplotter_->plot_data_ = 1;
  dataplotter_->data_over_mc_ = -sigOverBkg_;
  return status;
}

//print standard stacks for each selection
Int_t print_standard_selections(TString histDir = "", TString figureDir = "") {
  if(histDir != "") hist_dir_ = histDir;
  if(figureDir != "") folder_ = figureDir;
  TStopwatch* timer = new TStopwatch();  
  Int_t status = 0;
  offsetSets_ = true;

  selection_ = "mutau";
  status += init_dataplotter();
  status += print_standard_plots({6,7,8}, {150, 150., 150.});
  if(printBlindSets_) status += print_blind_sets({9,10}, {20.,20.}, {2, 2});
  
  selection_ = "etau";
  status += init_dataplotter();
  status += print_standard_plots({6,7,8}, {150, 150., 150.});
  if(printBlindSets_) status += print_blind_sets({9,10}, {20.,20.}, {2, 2});

  selection_ = "emu";
  status += init_dataplotter();
  status += print_standard_plots({6,7,8}, {100., 100.,100.});
  status += print_emu_cutsets();
  if(printBlindSets_) status += print_blind_sets({9,10}, {2.,2.}, {2,2});

  selection_ = "mutau_e";
  status += init_dataplotter();
  offsetSets_ = false;
  status += print_standard_plots({66,67,68}, {150., 150., 150.});
  offsetSets_ = true;
  if(printBlindSets_) status += print_blind_sets({9,10}, {2.,2.}, {2,2});

  selection_ = "etau_mu";
  status += init_dataplotter();
  offsetSets_ = false;
  status += print_standard_plots({66,67,68}, {150., 150., 150.});
  offsetSets_ = true;
  if(printBlindSets_) status += print_blind_sets({9,10}, {2.,2.}, {2,2});

  Double_t cpuTime = timer->CpuTime();
  Double_t realTime = timer->RealTime();
  printf("Processing time: %7.2fs CPU time %7.2fs Wall time\n",cpuTime,realTime);
  if(realTime > 600. ) printf("Processing time: %7.2fmin CPU time %7.2fmin Wall time\n",cpuTime/60.,realTime/60.);
  return status;
}

Int_t print_standard_canvas_selections(TString name = "", TString histDir = "", TString figureDir = "") {
  if(histDir != "") hist_dir_ = histDir;
  if(figureDir != "") folder_ = figureDir;
  TStopwatch* timer = new TStopwatch();  
  Int_t status = 0;
  selection_ = "mutau";
  status += init_dataplotter();
  status += print_standard_canvases({7,8,9,10,13,14}, {250., 250., 50., 50., 250., 250.}, {1,1,1,1,1,1}, true, name);
  selection_ = "etau";
  status += init_dataplotter();
  status += print_standard_canvases({27,28,29,30,33,34}, {250., 250., 50., 50., 250., 250.}, {1,1,1,1,1,1}, true, name);
  selection_ = "emu";
  status += init_dataplotter();
  status += print_standard_canvases({47,48,49,50,53,54,55,56}, {250., 250., 5., 5., 250., 250., 5., 5.}, {1,1,2,2,1,1,2,2}, true, name);
  selection_ = "mutau_e";
  status += init_dataplotter();
  status += print_standard_canvases({47,48}, {250., 250.}, {1, 1}, true, name);
  selection_ = "etau_mu";
  status += init_dataplotter();
  status += print_standard_canvases({47,48}, {250., 250.}, {1, 1}, true, name);
  selection_ = "mumu";
  status += init_dataplotter();
  status += print_standard_canvases({67,68, 80}, {1., 1., 1.}, {1, 1, 1}, true, name);

  Double_t cpuTime = timer->CpuTime();
  Double_t realTime = timer->RealTime();
  printf("Processing time: %7.2fs CPU time %7.2fs Wall time\n",cpuTime,realTime);
  if(realTime > 600. ) printf("Processing time: %7.2fmin CPU time %7.2fmin Wall time\n",cpuTime/60.,realTime/60.);
  return status;
}
