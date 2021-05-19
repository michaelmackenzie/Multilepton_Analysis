// Script to plot and print distributions using a DataPlotter object for ZTauTauHistMaker histograms

DataPlotter* dataplotter_ = 0;
TString selection_ = "emu"; //current options: mutau, etau, emu, mutau_e, etau_mu, mumu, ee
Int_t verbose_ = 0; //verbosity level
Int_t useOpenGL_ = 0;
bool  doStatsLegend_ = false;
TString hist_path_ = "root://cmseos.fnal.gov//store/user/mmackenz/histograms/"; //where histogram files are
TString hist_dir_ = "nanoaods_dev"; //which histogram directory to use
TString folder_ = "nanoaods_dev"; //figures output folder
bool doAllEMu_ = false; //plot all emu signals (including leptonic decays) on selection_ == "emu"
bool printCDFs_ = false; //print cdf transform of MVAs
bool printLimitVsEff_ = false; //print limit gain vs signal efficiency from MVA cut
bool printMVATotBkg_ = false; //print MVA distributions as total background vs signal
bool print2Ds_ = false;
bool printSignificances_ = false;
bool printMVAPlots_ = false;
bool printSlimCounts_ = false;
bool printBlindSets_ = false; //print sets > MVA score cut without data
vector<int> years_ = {2016, 2017, 2018}; //list of years of interest
bool offsetSets_ = true; //offset by selection set from ZTauTauHistMaker
int sigOverBkg_ = 0; //plot sig / bkg or data / MC (0 = data/MC, 1 = sig/MC, 2 = sig*sig/MC)
int doRunPeriod_ = 0; //do a specific run period of data
int useQCD_ = 0; //use qcd estimate
int useMisID_ = 1; //use Mis-ID estimate
int useUL_ = 0; //use UL dataset definitions

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
      if(selection_ == "mumu")    set += ZTauTauHistMaker::kMuMu;
      if(selection_ == "ee")      set += ZTauTauHistMaker::kEE;
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
            TH1D* hist = (TH1D*) hl;
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
        } else if(h->InheritsFrom("TH1D")) { //signal histograms or data
            TH1D* hist = (TH1D*) h;
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

//print Higgs-Combine cut-and-count data card assuming standard MVA sets
Int_t print_combine_card(TString hist, TString type, TString label, double xmin = 1., double xmax = -1.) {
  if(!dataplotter_) return -1;
  //make a directory for the card if needed
  TString year_s = "";
  for(unsigned index = 0; index < years_.size(); ++index) {
    year_s += years_[index];
    if(index < years_.size() - 1) year_s += "_";
  }
  gSystem->Exec(Form("[ ! -d datacards/%s ] && mkdir -p datacards/%s", year_s.Data(), year_s.Data()));

  //get initial selection set based on Higgs decay or not
  bool isHiggs = label.Contains("H->");
  int base_set = (isHiggs) ? 14 : 9; //FIXME: Get from HistMaker
  int max_sets = 5;
  if(selection_ == "mutau"  ) base_set += ZTauTauHistMaker::kMuTau;
  if(selection_ == "etau"   ) base_set += ZTauTauHistMaker::kETau;
  if(selection_ == "emu"    ) base_set += ZTauTauHistMaker::kEMu;
  if(selection_ == "mutau_e") base_set += ZTauTauHistMaker::kMuTauE;
  if(selection_ == "etau_mu") base_set += ZTauTauHistMaker::kETauMu;

  vector<double> bkgs, sigs;
  for(int curr_set = base_set; curr_set < base_set + max_sets; ++curr_set) {
    TCanvas* c = dataplotter_->plot_stack(hist, type, curr_set);
    if(!c) return 1;
    TList* list = c->GetListOfPrimitives();
    if(!list) return 2;
    for(auto o : *list) {
      try {
        auto pad = (TPad*) o;
        if(!pad) continue;
        TList* pad_list = pad->GetListOfPrimitives();
        bool bkg_f(false), sig_f(false);
        for(auto h : *pad_list) {
          // cout << h->GetName() << ": " << h->GetTitle() << ": " << h->InheritsFrom("THStack") << endl;
          if(h->InheritsFrom("THStack")) {
            THStack* hstack = (THStack*) h;
            TH1D* hbkg = (TH1D*) hstack->GetStack()->Last();
            int bin_lo = (xmin < xmax) ? hbkg->FindBin(xmin) : 0;
            int bin_hi = (xmin < xmax) ? hbkg->FindBin(xmax) : hbkg->GetNbinsX()+1;
            double bkg = hbkg->Integral(bin_lo, bin_hi);
            bkgs.push_back(bkg);
            bkg_f = true;
          } else if(h->InheritsFrom("TH1D")) { //signal histograms or data
            TH1D* h_curr = (TH1D*) h;
            if(TString(h_curr->GetName()).Contains(label.Data())) {
              int bin_lo = (xmin < xmax) ? h_curr->FindBin(xmin) : 0;
              int bin_hi = (xmin < xmax) ? h_curr->FindBin(xmax) : h_curr->GetNbinsX()+1;
              double sig = h_curr->Integral(bin_lo, bin_hi);
              sigs.push_back(sig);
              sig_f = true;
            }
          }
          delete h;
        }
        if(!sig_f&&bkg_f) sigs.push_back(0.);

      } catch(exception e) {
      }
    }
    DataPlotter::Empty_Canvas(c);
  }

  if(bkgs.size() != sigs.size()) {
    cout << "Background size = " << bkgs.size() << " != signal size = " << sigs.size() << endl;
    return 3;
  }

  //Re-create datacard for the process, add information
  TString name = label;
  name.ReplaceAll("#", "");
  name.ReplaceAll(" ", "");
  name.ReplaceAll("/", "");
  name.ReplaceAll("->", "");
  name.ToLower();
  cout << "Using label --> name = " << name.Data() << endl;
  TString filepath = Form("datacards/%s/counting_MVA_%s.txt", year_s.Data(), name.Data());
  gSystem->Exec(Form("echo \"# -*- mode: tcl -*-\">| %s", filepath.Data()));
  gSystem->Exec(Form("echo \"#Auto generated counting card for LFVAnalysis \n\">> %s", filepath.Data()));
  gSystem->Exec(Form("echo \"imax %2zu number of channels \">> %s", bkgs.size(), filepath.Data()));
  gSystem->Exec(Form("echo \"jmax  1 number of backgrounds \">> %s", filepath.Data()));
  gSystem->Exec(Form("echo \"kmax  0 number of nuisance parameters (sources of systematical uncertainties) \n\">> %s", filepath.Data()));

  TString bins   = "bin          ";
  TString obs    = "observation  ";
  TString bins_p = "bin          ";
  TString proc_l = "process      ";
  TString proc_c = "process      ";
  TString rate   = "rate         ";
  for(unsigned index = 0; index < bkgs.size(); ++index) {
    //Append information to combined card
    bins += Form("mva%i ", index);
    obs  +=      "  1  ";
    bins_p += Form("     mva%i       mva%i  ", index, index);
    proc_l +=      "      sig        bkg  ";
    proc_c +=      "       0          1   ";
    rate   += Form("%10.1f %10.1f ", sigs[index], bkgs[index]);

    //output information for individual channel's card
    TString filepath_i = Form("datacards/%s/counting_MVA_%s_%i.txt", year_s.Data(), name.Data(), index);
    gSystem->Exec(Form("echo \"# -*- mode: tcl -*-\">| %s", filepath_i.Data()));
    gSystem->Exec(Form("echo \"#Auto generated counting card for LFVAnalysis \n\">> %s", filepath_i.Data()));
    gSystem->Exec(Form("echo \"imax  1 number of channels \">> %s", filepath_i.Data()));
    gSystem->Exec(Form("echo \"jmax  1 number of backgrounds \">> %s", filepath_i.Data()));
    gSystem->Exec(Form("echo \"kmax  0 number of nuisance parameters (sources of systematical uncertainties) \n\">> %s", filepath_i.Data()));
    gSystem->Exec(Form("echo \"bin              mva \">> %s", filepath_i.Data()));
    gSystem->Exec(Form("echo \"observation       1 \">> %s" , filepath_i.Data()));
    gSystem->Exec(Form("echo \"bin              mva        mva \">> %s", filepath_i.Data()));
    gSystem->Exec(Form("echo \"process          sig        bkg \">> %s", filepath_i.Data()));
    gSystem->Exec(Form("echo \"process           0          1  \">> %s", filepath_i.Data()));
    gSystem->Exec(Form("echo \"rate       %10.1f %10.1f  \n\">> %s", sigs[index], bkgs[index], filepath_i.Data()));

    cout << index << ": Bkg = " << bkgs[index] << " Sig = " << sigs[index] << endl;
  }
  gSystem->Exec(Form("echo \"%s \">> %s", bins.Data(), filepath.Data()));
  gSystem->Exec(Form("echo \"%s \n\">> %s", obs.Data() , filepath.Data()));
  gSystem->Exec(Form("echo \"%s \">> %s", bins_p.Data() , filepath.Data()));
  gSystem->Exec(Form("echo \"%s \">> %s", proc_l.Data() , filepath.Data()));
  gSystem->Exec(Form("echo \"%s \n\">> %s", proc_c.Data() , filepath.Data()));
  gSystem->Exec(Form("echo \"%s \n\">> %s", rate.Data() , filepath.Data()));

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
    if(selection_ == "mumu")    set += ZTauTauHistMaker::kMuMu;
    if(selection_ == "ee")      set += ZTauTauHistMaker::kEE;
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
    if(selection_ == "mumu")    set += ZTauTauHistMaker::kMuMu;
    if(selection_ == "ee")      set += ZTauTauHistMaker::kEE;
  }
  auto c = print_canvas(hist, type, set, stacks, name);
  dataplotter_->reset_axes();
  return c;
}

Int_t print_test(vector<int> sets, vector<double> signal_scales = {},
                 vector<int> base_rebins = {}) {

  cout << "Printing test\n";
  if(!dataplotter_) return 1;
  vector<PlottingCard_t> plottingcards;
  // plotting card constructor:                       hist, type, rebin, xmin, xmax, blindmin, blindmax
  if(selection_ == "emu" || selection_.Contains("tau_")) {
    plottingcards.push_back(PlottingCard_t("onept",          "lep",   2, 15.,  150. ));
    plottingcards.push_back(PlottingCard_t("twopt",          "lep",   2, 15.,  150. ));
  } else {
    plottingcards.push_back(PlottingCard_t("onept",          "lep",   2, 15.,  150. ));
    plottingcards.push_back(PlottingCard_t("twopt",          "lep",   2, 15.,  150. ));
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
      if(selection_ == "mumu")    sets[i] += ZTauTauHistMaker::kMuMu;
      if(selection_ == "ee")      sets[i] += ZTauTauHistMaker::kEE;
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
      if(selection_ == "mumu")    sets[i] += ZTauTauHistMaker::kMuMu;
      if(selection_ == "ee")      sets[i] += ZTauTauHistMaker::kEE;
    }
  }
  Int_t status = 0;
  vector<PlottingCard_t> plottingcards;
  //muon cards
  plottingcards.push_back(PlottingCard_t("nslimmuons"    , "event", 1, 0, 6));
  for(int i = 0; i < 24; ++i)
    plottingcards.push_back(PlottingCard_t(Form("nmuoncounts%i",i)    , "event", 1, 0, 6));
  //electron cards
  plottingcards.push_back(PlottingCard_t("nslimelectrons"    , "event", 1, 0, 6));
  for(int i = 0; i < 6; ++i)
    plottingcards.push_back(PlottingCard_t(Form("nelectroncounts%i",i), "event", 1, 0, 6));
  //tau cards
  plottingcards.push_back(PlottingCard_t("nslimtaus"    , "event", 1, 0, 6));
  for(int i = 0; i < 18; ++i)
    plottingcards.push_back(PlottingCard_t(Form("ntaucounts%i",i)     , "event", 1, 0, 6));
  //print the cards
  dataplotter_->logY_ = 1;
  status = dataplotter_->print_stacks(plottingcards, sets, {}, {});
  dataplotter_->logY_ = 0;
  status = dataplotter_->print_stacks(plottingcards, sets, {}, {});
  return status;
}

//add cards for event variables
void get_event_cards(vector<PlottingCard_t> &cards) {
  cards.push_back(PlottingCard_t("ntriggered",      "event", 0, 0,  7   ));
  cards.push_back(PlottingCard_t("met",            "event", 2, 0.,   120. ));
  // cards.push_back(PlottingCard_t("puppmet",        "event", 2, 0.,   120. ));
  // cards.push_back(PlottingCard_t("covmet00",       "event", 10,100., 1000.));
  // cards.push_back(PlottingCard_t("covmet11",       "event", 10,100., 1000.));
  // cards.push_back(PlottingCard_t("covmet01",       "event", 1, -100.,100. ));
  cards.push_back(PlottingCard_t("mtone",          "event", 4, 0.,   150. ));
  cards.push_back(PlottingCard_t("mttwo",          "event", 4, 0.,   150. ));
  //relevant weights:
  cards.push_back(PlottingCard_t("puweight",        "event", 1, 0.,   1.5 ));
  cards.push_back(PlottingCard_t("oneweight",       "lep"  , 1, 0.,   1.5 ));
  cards.push_back(PlottingCard_t("twoweight",       "lep"  , 1, 0.,   1.5 ));
  cards.push_back(PlottingCard_t("onetrigweight",   "lep"  , 1, 0.,   1.5 ));
  cards.push_back(PlottingCard_t("twotrigweight",   "lep"  , 1, 0.,   1.5 ));
  cards.push_back(PlottingCard_t("btagweight",      "event", 1, 0.,   1.5 ));
  //leading jet info
  cards.push_back(PlottingCard_t("jetpt",          "event", 2, 15.,  250.));
  // cards.push_back(PlottingCard_t("jeteta",         "event", 2, -3.,  3.));
  // cards.push_back(PlottingCard_t("jetm",           "event", 2,  0.,  50.));
  // cards.push_back(PlottingCard_t("jetbmva",        "event", 2, -1.,  1.));
  // cards.push_back(PlottingCard_t("jetbtag",        "event", 0,  0.,  2.));

  //jet counts
  cards.push_back(PlottingCard_t("njets",          "event", 0, 0.,   10. ));
  cards.push_back(PlottingCard_t("njets20",        "event", 0, 0.,   10. ));
  cards.push_back(PlottingCard_t("njets201",       "event", 0, 0.,   10. ));
  cards.push_back(PlottingCard_t("nbjets",         "event", 0, 0.,   10. ));
  cards.push_back(PlottingCard_t("nbjetsm",        "event", 0, 0.,   10. ));
  cards.push_back(PlottingCard_t("nbjetsl",        "event", 0, 0.,   10. ));
  cards.push_back(PlottingCard_t("nbjets20",       "event", 0, 0.,   10. ));
  cards.push_back(PlottingCard_t("nbjets20m",      "event", 0, 0.,   10. ));
  cards.push_back(PlottingCard_t("nbjets20l",      "event", 0, 0.,   10. ));
  cards.push_back(PlottingCard_t("nbjets201",      "event", 0, 0.,   10. ));
  cards.push_back(PlottingCard_t("nbjets20m1",     "event", 0, 0.,   10. ));
  cards.push_back(PlottingCard_t("nbjets20l1",     "event", 0, 0.,   10. ));
  cards.push_back(PlottingCard_t("jetsflavor",     "event", 0, -8.,  40. ));

  cards.push_back(PlottingCard_t("nfwdjets",       "event", 0, 0.,   10. ));
  cards.push_back(PlottingCard_t("nphotons",       "event", 0, 0.,   5.  ));
  cards.push_back(PlottingCard_t("nelectrons",     "event", 0, 0.,   5.  ));
  cards.push_back(PlottingCard_t("nmuons",         "event", 0, 0.,   5.  ));
  cards.push_back(PlottingCard_t("ntaus",          "event", 0, 0.,   5.  ));

  cards.push_back(PlottingCard_t("npv",            "event", 0, 0.,   60. ));
  cards.push_back(PlottingCard_t("npu",            "event", 0, 0.,   60. ));
  cards.push_back(PlottingCard_t("npv1",           "event", 0, 0.,   60. ));
  cards.push_back(PlottingCard_t("npu1",           "event", 0, 0.,   60. ));

  // cards.push_back(PlottingCard_t("htsum",             "event", 5, 0.,   800.));
  // cards.push_back(PlottingCard_t("ht",                "event", 5, 0.,   800.));
}

//add cards from SVFit tool
void get_svfit_cards(vector<PlottingCard_t> &cards) {
  // hnames.push_back("onesvpt");        htypes.push_back("lep");   rebins.push_back(1); xmins.push_back(15.);  xmaxs.push_back(150.);
  // hnames.push_back("twosvpt");        htypes.push_back("lep");   rebins.push_back(1); xmins.push_back(15.);  xmaxs.push_back(150.);
  // hnames.push_back("onesvdeltapt");   htypes.push_back("lep");   rebins.push_back(1); xmins.push_back(15.);  xmaxs.push_back(150.);
  // hnames.push_back("twosvdeltapt");   htypes.push_back("lep");   rebins.push_back(1); xmins.push_back(15.);  xmaxs.push_back(200.);
  // hnames.push_back("lepsvm");         htypes.push_back("event"); rebins.push_back(5); xmins.push_back(0.);   xmaxs.push_back(300.);
  // hnames.push_back("lepsvdeltam");    htypes.push_back("event"); rebins.push_back(2); xmins.push_back(-10.); xmaxs.push_back(90.);
  // hnames.push_back("lepsvpt");        htypes.push_back("event"); rebins.push_back(2); xmins.push_back(0.);   xmaxs.push_back(150.);
  // hnames.push_back("lepsvdeltapt");   htypes.push_back("event"); rebins.push_back(2); xmins.push_back(-10.); xmaxs.push_back(90.);
  // hnames.push_back("lepsvptoverm");   htypes.push_back("event"); rebins.push_back(2); xmins.push_back(0.);   xmaxs.push_back(10.);

}

//add cards for lepton kinematics
void get_lep_kin_cards(vector<PlottingCard_t> &cards) {
  if(selection_ == "emu" || selection_.Contains("tau_") || selection_ == "ee" || selection_ == "mumu") {
    cards.push_back(PlottingCard_t("onept",          "lep",   2,10.,  100. ));
    cards.push_back(PlottingCard_t("onept1",         "lep",   2,10.,  100. ));
    cards.push_back(PlottingCard_t("onept3",         "lep",   2,10.,  100. ));
    cards.push_back(PlottingCard_t("onept4",         "lep",   2,10.,  100. ));
    cards.push_back(PlottingCard_t("onept5",         "lep",   2,10.,  100. ));
    cards.push_back(PlottingCard_t("onept6",         "lep",   2,10.,  100. ));
    cards.push_back(PlottingCard_t("onept7",         "lep",   2,10.,  100. ));
    cards.push_back(PlottingCard_t("onept8",         "lep",   2,10.,  100. ));
    cards.push_back(PlottingCard_t("twopt",          "lep",   2,10.,  100. ));
    cards.push_back(PlottingCard_t("twopt1",         "lep",   2,10.,  100. ));
    cards.push_back(PlottingCard_t("twopt3",         "lep",   2,10.,  100. ));
    cards.push_back(PlottingCard_t("twopt4",         "lep",   2,10.,  100. ));
    cards.push_back(PlottingCard_t("twopt5",         "lep",   2,10.,  100. ));
    cards.push_back(PlottingCard_t("twopt6",         "lep",   2,10.,  100. ));
    cards.push_back(PlottingCard_t("twopt7",         "lep",   2,10.,  100. ));
    cards.push_back(PlottingCard_t("twopt8",         "lep",   2,10.,  100. ));
  } else {
    cards.push_back(PlottingCard_t("onept",          "lep",   2, 25.,  100. ));
    cards.push_back(PlottingCard_t("twopt",          "lep",   2, 20.,   80. ));
  }
  cards.push_back(PlottingCard_t("oneeta",         "lep",   2, -3.,    5. ));
  cards.push_back(PlottingCard_t("twoeta",         "lep",   2, -3.,    5. ));
  if(selection_ == "emu" || selection_.Contains("tau_")) { //emu dataset
    cards.push_back(PlottingCard_t("lepm",         "event", 5, 50.,   170., {84, 118}, {98, 132.} ));
  } else //etau, mutau, ee, or mumu
    cards.push_back(PlottingCard_t("lepm",         "event", 5, 50.,   170. ));
  cards.push_back(PlottingCard_t("leppt",          "event", 2, 0.,   150. ));
  if(selection_ == "ee" || selection_ == "mumu") {
    cards.push_back(PlottingCard_t("lepm1",        "event", 5, 50.,  150. ));
    cards.push_back(PlottingCard_t("lepm2",        "event", 5, 50.,  150. ));
    cards.push_back(PlottingCard_t("lepm3",        "event", 1, 70.,  110. ));
    cards.push_back(PlottingCard_t("lepm4",        "event", 1,105.,  145. ));
    cards.push_back(PlottingCard_t("leppt1",       "event", 2, 0.,   150. ));
    cards.push_back(PlottingCard_t("leppt2",       "event", 2, 0.,   150. ));
  }
  cards.push_back(PlottingCard_t("lepptoverm",     "event", 2, 0.,   5.   ));
  cards.push_back(PlottingCard_t("lepeta",         "event", 5, -5.,  5.   ));
  cards.push_back(PlottingCard_t("lepdeltaeta",    "event", 5, 0.,   5.   ));
  cards.push_back(PlottingCard_t("lepdeltaphi",    "event", 1, 0.,   5.0  ));
  cards.push_back(PlottingCard_t("lepdeltar",      "event", 1, 0.,   5.   ));

  // cards.push_back(PlottingCard_t("oned0",             "lep",   2, -0.05,0.05));
  // cards.push_back(PlottingCard_t("twod0",             "lep",   2, -0.05,0.05));

  cards.push_back(PlottingCard_t("oneiso",            "lep",   1, 0.,   10. ));
  cards.push_back(PlottingCard_t("onereliso",         "lep",   1, 0.,   0.15));
  if(selection_ == "emu" || selection_.Contains("tau_") /*leptonic tau decay*/ || selection_ == "llg_study" || selection_ == "ee" || selection_ == "mumu") {
    cards.push_back(PlottingCard_t("twoiso",       "lep", 1, 0.,   10.  ));
    cards.push_back(PlottingCard_t("tworeliso",    "lep", 1, 0.,   0.15 ));
  }
  cards.push_back(PlottingCard_t("oneid1",  "lep",   0, 0,10));
  cards.push_back(PlottingCard_t("twoid1",  "lep",   0, 0,50));
  cards.push_back(PlottingCard_t("oneid2",  "lep",   0, 0,10));
  cards.push_back(PlottingCard_t("twoid2",  "lep",   0, 0,10));

  // cards.push_back(PlottingCard_t("htdeltaphi",        "event", 1, 0.,   5.0 ));
  // cards.push_back(PlottingCard_t("metdeltaphi",       "event", 1, 0.,   5.0 ));
  // cards.push_back(PlottingCard_t("leponedeltaphi",    "event", 1, 0.,   5.0 ));
  // cards.push_back(PlottingCard_t("leptwodeltaphi",    "event", 1, 0.,   5.0 ));
  cards.push_back(PlottingCard_t("onemetdeltaphi",    "lep",   1, 0.,   5.0 ));
  cards.push_back(PlottingCard_t("twometdeltaphi",    "lep",   1, 0.,   5.0 ));
  // cards.push_back(PlottingCard_t("leponejetdeltaphi", "event", 1, 0.,   5.0 ));
  // cards.push_back(PlottingCard_t("leptwojetdeltaphi", "event", 1, 0.,   5.0 ));
  // cards.push_back(PlottingCard_t("leponejetdeltaeta", "event", 1, 0.,   6.  ));
  // cards.push_back(PlottingCard_t("leptwojetdeltaeta", "event", 1, 0.,   6.  ));
  // cards.push_back(PlottingCard_t("leponejetdeltar",   "event", 1, 0.,   6.  ));
  // cards.push_back(PlottingCard_t("leptwojetdeltar",   "event", 1, 0.,   6.  ));
}

//add cards for tau variables
void get_tau_cards(vector<PlottingCard_t> &cards) {
  cards.push_back(PlottingCard_t("pxivis0",        "event", 5, 0.,   100. ));
  cards.push_back(PlottingCard_t("pxiinv0",        "event", 5, -100.,100. ));
  cards.push_back(PlottingCard_t("ptauvisfrac",    "event", 5, 0.,   1.4  ));
  cards.push_back(PlottingCard_t("alpha0",         "event", 1, 0.,   5.   ));
  cards.push_back(PlottingCard_t("alpha1",         "event", 1, 0.,   5.   ));
  cards.push_back(PlottingCard_t("alpha2",         "event", 1, 0.,   5.   ));
  cards.push_back(PlottingCard_t("alpha3",         "event", 1, 0.,   5.   ));
  cards.push_back(PlottingCard_t("deltaalpha0",    "event", 2,-3.,   6., -1., 0.5));
  cards.push_back(PlottingCard_t("deltaalpha1",    "event", 2,-3.,   6., -1., 0.5));
  cards.push_back(PlottingCard_t("deltaalpha2",    "event", 2,-3.,   8., -1., 0.5));
  cards.push_back(PlottingCard_t("deltaalpha3",    "event", 2,-3.,   8., -1., 0.5));
  cards.push_back(PlottingCard_t("deltaalpham0",   "event", 1,45., 200.   ));
  cards.push_back(PlottingCard_t("deltaalpham1",   "event", 2,45., 200.   ));
  if(selection_ == "mumu" || selection_ == "ee") {
    cards.push_back(PlottingCard_t("lepmestimate",   "event", 1, 50.,  200.));
    cards.push_back(PlottingCard_t("lepmestimatetwo","event", 1, 50.,  200.));
  } else {
    cards.push_back(PlottingCard_t("lepmestimate",   "event", 1, 50.,  200., {80., 115.}, {100., 135.}));
    cards.push_back(PlottingCard_t("lepmestimatetwo","event", 1, 50.,  200., {80., 115.}, {100., 135.}));
  }
  if(selection_ == "mutau" || selection_ == "etau" || selection_ == "llg_study") { //tau only sets
    cards.push_back(PlottingCard_t("twom",          "lep", 2, 0.,   3. ));
    cards.push_back(PlottingCard_t("taudeepantiele","event", 0, 0.,   30. ));
    cards.push_back(PlottingCard_t("taudeepantimu", "event", 0, 0.,   30. ));
    cards.push_back(PlottingCard_t("taudeepantijet","event", 0, 0.,   30. ));
    cards.push_back(PlottingCard_t("taugenflavor",  "event", 0, 10.,  30. ));
    cards.push_back(PlottingCard_t("taudecaymode",  "event", 0, 0.,   15. ));
  }
}

//add cards for the MVA distributions
void get_mva_cards(vector<PlottingCard_t> &cards) {
  if(selection_ == "mutau") {
    cards.push_back(PlottingCard_t("mva0",       "event", "H->#mu#tau",  20, -0.6,   0.6, 0.01, 1. )); //H MVA
    cards.push_back(PlottingCard_t("mva1",       "event", "Z->#mu#tau",  20, -0.6,   0.6, 0.01, 1. )); //Z MVA
    cards.push_back(PlottingCard_t("mvatest0",   "event", "H->#mu#tau",   2, -0.6,   0.6, 0.01, 1. )); //H MVA
    cards.push_back(PlottingCard_t("mvatest1",   "event", "Z->#mu#tau",   2, -0.6,   0.6, 0.01, 1. )); //Z MVA
    cards.push_back(PlottingCard_t("mvatrain0",  "event", "H->#mu#tau",   2, -0.6,   0.6, 0.01, 1. )); //H MVA
    cards.push_back(PlottingCard_t("mvatrain1",  "event", "Z->#mu#tau",   2, -0.6,   0.6, 0.01, 1. )); //Z MVA
  } else if(selection_ == "etau") {
    cards.push_back(PlottingCard_t("mva2",       "event", "H->e#tau"  ,  20, -0.6,   0.6, 0.01, 1. )); //H MVA
    cards.push_back(PlottingCard_t("mva3",       "event", "Z->e#tau"  ,  20, -0.6,   0.6, 0.01, 1. )); //Z MVA
    cards.push_back(PlottingCard_t("mvatest2",   "event", "H->e#tau"  ,   2, -0.6,   0.6, 0.01, 1. )); //H MVA
    cards.push_back(PlottingCard_t("mvatest3",   "event", "Z->e#tau"  ,   2, -0.6,   0.6, 0.01, 1. )); //Z MVA
    cards.push_back(PlottingCard_t("mvatrain2",  "event", "H->e#tau"  ,   2, -0.6,   0.6, 0.01, 1. )); //H MVA
    cards.push_back(PlottingCard_t("mvatrain3",  "event", "Z->e#tau"  ,   2, -0.6,   0.6, 0.01, 1. )); //Z MVA
  } else if(selection_ == "emu" || selection_.Contains("tau_") || selection_ == "ee" || selection_ == "mumu") { //print all MVAs for emu dataset categories
    int mvas[] = {4,5, //H, Z -> emu
                  6,7, //H, Z -> mutau_e
                  8,9};//H, Z ->etau_mu
    double blinds[] = {0.  , 0.  ,
                       0.  , 0.  ,
                       0.  , 0.  };
    TString label = (selection_ == "ee" || selection_ == "mumu") ? "" : "->" + selection_;
    label.ReplaceAll("_e", ""); label.ReplaceAll("_mu", "");
    label.ReplaceAll("mu", "#mu"); label.ReplaceAll("tau", "#tau");
    for(int mva_i = 0; mva_i < sizeof(mvas)/sizeof(*mvas); ++mva_i) {
      if(selection_ == "ee" || selection_ == "mumu") { //don't need to blind the ee/mumu channels
        cards.push_back(PlottingCard_t(Form("mva%i"     ,mvas[mva_i]), "event",  20, -0.6,   0.6));
        cards.push_back(PlottingCard_t(Form("mvatest%i" ,mvas[mva_i]), "event",   2, -0.6,   0.6));
        cards.push_back(PlottingCard_t(Form("mvatrain%i",mvas[mva_i]), "event",   2, -0.6,   0.6));
      } else {
        TString label_zh = (mva_i % 2 == 0) ? "H" : "Z";
        label_zh = label_zh + label;
        cards.push_back(PlottingCard_t(Form("mva%i"     ,mvas[mva_i]), "event", label_zh,  20, -0.6,   0.6 , blinds[mva_i], 1. ));
        cards.push_back(PlottingCard_t(Form("mvatest%i" ,mvas[mva_i]), "event", label_zh,   2, -0.6,   0.6 , blinds[mva_i], 1. ));
        cards.push_back(PlottingCard_t(Form("mvatrain%i",mvas[mva_i]), "event", label_zh,   2, -0.6,   0.6 , blinds[mva_i], 1. ));
      }
    }
  }

}

//add cards for the systematically shifted distributions
void get_sys_cards(vector<PlottingCard_t> &cards) {

  //get the histograms and signal labels of interest
  TString hist1("mva"), label1;
  TString hist2("mva"), label2;
  double xmin(1.), xmax(-1.);
  if     (selection_ == "mutau"  ) {hist1 += 0    ; hist2 += 1    ; label1 = "H->#mu#tau"; label2 = "Z->#mu#tau"; xmin = -0.6; xmax = 0.5;}
  else if(selection_ == "etau"   ) {hist1 += 2    ; hist2 += 3    ; label1 = "H->e#tau"  ; label2 = "Z->e#tau"  ; xmin = -0.6; xmax = 0.5;}
  else if(selection_ == "emu"    ) {hist1 = "lepm"; hist2 = "lepm"; label1 = "H->e#mu"   ; label2 = "Z->e#mu"   ;}
  else if(selection_ == "mutau_e") {hist1 += 6    ; hist2 += 7    ; label1 = "H->#mu#tau"; label2 = "Z->#mu#tau"; xmin = -0.6; xmax = 0.5;}
  else if(selection_ == "etau_mu") {hist1 += 8    ; hist2 += 9    ; label1 = "H->e#tau"  ; label2 = "Z->e#tau"  ; xmin = -0.6; xmax = 0.5;}

  //add a plot for each interesting systematic
  for(int i = 0; i < 11; ++i) {
    PlottingCard_t card1(hist1, "systematic", label1, 1, xmin, xmax, 1., -1.);
    PlottingCard_t card2(hist2, "systematic", label2, 1, xmin, xmax, 1., -1.);
    //add the +- 1 systematic plot
    card1.systematic_ = 1 + i*3;
    card2.systematic_ = 1 + i*3;
    cards.push_back(card1);
    cards.push_back(card2);
    //add the random systematic plot
    card1.systematic_ = 3 + i*3;
    card2.systematic_ = 3 + i*3;
    if(card1.systematic_ == 24) continue; //FIXME: Skipping missing systematic
    card1.single_systematic_ = true;
    card2.single_systematic_ = true;
    cards.push_back(card1);
    cards.push_back(card2);
  }
}

Int_t print_sys_cards(vector<int> sets) {
  if(!dataplotter_) return 1;
  vector<PlottingCard_t> cards;
  get_sys_cards(cards);
  int status = 0;
  if(offsetSets_) {
    for(unsigned i = 0; i < sets.size(); ++ i) {
      if(selection_ == "mutau"  ) sets[i] += ZTauTauHistMaker::kMuTau;
      if(selection_ == "etau"   ) sets[i] += ZTauTauHistMaker::kETau;
      if(selection_ == "emu"    ) sets[i] += ZTauTauHistMaker::kEMu;
      if(selection_ == "mutau_e") sets[i] += ZTauTauHistMaker::kMuTauE;
      if(selection_ == "etau_mu") sets[i] += ZTauTauHistMaker::kETauMu;
      if(selection_ == "mumu")    sets[i] += ZTauTauHistMaker::kMuMu;
      if(selection_ == "ee")      sets[i] += ZTauTauHistMaker::kEE;
    }
  }
  for(int iset : sets) {
    for(auto card : cards) {
      card.set_ = iset;
      dataplotter_->logY_ = 0;
      auto c = dataplotter_->print_systematic(card);
      status += (c) ? 0 : 1;
      DataPlotter::Empty_Canvas(c);
      dataplotter_->logY_ = 1;
      c = dataplotter_->print_systematic(card);
      status += (c) ? 0 : 1;
      DataPlotter::Empty_Canvas(c);
    }
  }
  return status;
}

Int_t print_tau_plots(vector<int> sets) {
  vector<PlottingCard_t> cards;
  get_tau_cards(cards);
  if(offsetSets_) {
    for(unsigned i = 0; i < sets.size(); ++ i) {
      if(selection_ == "mutau"  ) sets[i] += ZTauTauHistMaker::kMuTau;
      if(selection_ == "etau"   ) sets[i] += ZTauTauHistMaker::kETau;
      if(selection_ == "emu"    ) sets[i] += ZTauTauHistMaker::kEMu;
      if(selection_ == "mutau_e") sets[i] += ZTauTauHistMaker::kMuTauE;
      if(selection_ == "etau_mu") sets[i] += ZTauTauHistMaker::kETauMu;
      if(selection_ == "mumu")    sets[i] += ZTauTauHistMaker::kMuMu;
      if(selection_ == "ee")      sets[i] += ZTauTauHistMaker::kEE;
    }
  }
  int status(0);
  dataplotter_->logY_ = 0;
  status += dataplotter_->print_stacks(cards, sets, {}, {});
  dataplotter_->logY_ = 1;
  status += dataplotter_->print_stacks(cards, sets, {}, {});
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
      if(selection_ == "mumu")    sets[i] += ZTauTauHistMaker::kMuMu;
      if(selection_ == "ee")      sets[i] += ZTauTauHistMaker::kEE;
    }
  }
  vector<PlottingCard_t> plottingcards;
  vector<PlottingCard_t> mvaplottingcards; //to plot once normally and once with just total background

  get_lep_kin_cards(plottingcards);
  get_tau_cards(plottingcards);
  get_event_cards(plottingcards);

  //ignore SVfit results for now, not very helpful
  // get_svfit_cards(plottingcards)

  get_mva_cards(mvaplottingcards);


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
            else         dataplotter_->rebinH_ = 2;

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

  vector<PlottingCard_t> cdfplottingcards;
  if(label != "") {
    if(selection_=="mutau") {
      cdfplottingcards.push_back(PlottingCard_t("mva0", "event", ("H"+label), 1, 0., 1.7, 0.3, 1.7));
      cdfplottingcards.push_back(PlottingCard_t("mva1", "event", ("Z"+label), 1, 0., 1.7, 0.3, 1.7));
    } else if(selection_ == "etau") {
      cdfplottingcards.push_back(PlottingCard_t("mva2", "event", ("H"+label), 1, 0., 1.7, 0.3, 1.7));
      cdfplottingcards.push_back(PlottingCard_t("mva3", "event", ("Z"+label), 1, 0., 1.7, 0.3, 1.7));
    } else if(selection_ == "emu") {
      cdfplottingcards.push_back(PlottingCard_t("mva4", "event", ("H"+label), 1, 0., 1.7, 0.3, 1.7));
      cdfplottingcards.push_back(PlottingCard_t("mva5", "event", ("Z"+label), 1, 0., 1.7, 0.3, 1.7));
    } else if(selection_ == "mutau_e") {
      cdfplottingcards.push_back(PlottingCard_t("mva6", "event", ("H"+label), 1, 0., 1.7, 0.3, 1.7));
      cdfplottingcards.push_back(PlottingCard_t("mva7", "event", ("Z"+label), 1, 0., 1.7, 0.3, 1.7));
    } else if(selection_ == "etau_mu") {
      cdfplottingcards.push_back(PlottingCard_t("mva8", "event", ("H"+label), 1, 0., 1.7, 0.3, 1.7));
      cdfplottingcards.push_back(PlottingCard_t("mva9", "event", ("Z"+label), 1, 0., 1.7, 0.3, 1.7));
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
    CrossSections xsecs;
    double zxs(xsecs.GetCrossSection("Z"));
    TString hlim, zlim;
    if(selection_.Contains("mutau")) {
      hlim = Form("Current Limit %.1e", xsecs.GetCrossSection("HMuTau")/zxs);
      zlim = Form("Current Limit %.1e", xsecs.GetCrossSection("ZMuTau")/zxs);
    } else if(selection_.Contains("etau")) {
      hlim = Form("Current Limit %.1e", xsecs.GetCrossSection("HETau")/zxs);
      zlim = Form("Current Limit %.1e", xsecs.GetCrossSection("ZETau")/zxs);
    } else if(selection_.Contains("emu")) {
      hlim = Form("Current Limit %.1e", xsecs.GetCrossSection("HEMu")/zxs);
      zlim = Form("Current Limit %.1e", xsecs.GetCrossSection("ZEMu")/zxs);
    } else {
      cout << "Can't deterimine limits for selection " << selection_.Data() << "!\n";
    }
    cout << "Using Z cross-section " << zxs << " and limit labels of:\n"
         << hlim.Data() << endl << zlim.Data() << endl;
    if(label != "") {
      for(int logy = 0; logy < 2; ++logy) { //print log and not log y axis
        dataplotter_->logY_ = logy;
        for(int s : sets) {
          if(selection_=="mutau") {
            auto c = dataplotter_->print_significance("mva0", "event", s, ("H"+label), -1., 1., true, -1., false, hlim.Data());
            DataPlotter::Empty_Canvas(c);
            c = dataplotter_->print_significance("mva1", "event", s, ("Z"+label), -1., 1.3, true, -0.71, false, zlim.Data());
            DataPlotter::Empty_Canvas(c);
            if(printLimitVsEff_) {
              c = dataplotter_->print_significance("mva0", "event", s, ("H"+label), 0., 1., true, -1., true, hlim.Data()); //significance vs efficiency
              DataPlotter::Empty_Canvas(c);
              c = dataplotter_->print_significance("mva1", "event", s, ("Z"+label), 0., 1., true, -0.71, true, zlim.Data()); //significance vs efficiency
              DataPlotter::Empty_Canvas(c);
            }
          }
          else if(selection_=="etau") {
            auto c = dataplotter_->print_significance("mva2", "event", s, ("H"+label), -1., 1.,true, -1., false, hlim.Data());
            DataPlotter::Empty_Canvas(c);
            c = dataplotter_->print_significance("mva3", "event", s, ("Z"+label), -1., 1.,true, -1., false, zlim.Data());
            DataPlotter::Empty_Canvas(c);
            if(printLimitVsEff_) {
              c = dataplotter_->print_significance("mva2", "event", s, ("H"+label), 0., 1., true, -1., true, hlim.Data());
              DataPlotter::Empty_Canvas(c);
              c = dataplotter_->print_significance("mva3", "event", s, ("Z"+label), 0., 1., true, -1., true, zlim.Data());
              DataPlotter::Empty_Canvas(c);
            }
          }
          else if(selection_=="emu") {
            auto c = dataplotter_->print_significance("mva4", "event", s, ("H"+label), -1., 1., true, -1. , false, hlim.Data());
            DataPlotter::Empty_Canvas(c);
            c = dataplotter_->print_significance("mva5", "event", s, ("Z"+label), -1., 1., true, -.44, false, zlim.Data());
            DataPlotter::Empty_Canvas(c);
            if(printLimitVsEff_) {
              c = dataplotter_->print_significance("mva4", "event", s, ("H"+label), 0., 1., true, -1. , true, hlim.Data());
              DataPlotter::Empty_Canvas(c);
              c = dataplotter_->print_significance("mva5", "event", s, ("Z"+label), 0., 1., true, -.44, true, zlim.Data());
              DataPlotter::Empty_Canvas(c);
            }
          }
          else if(selection_=="mutau_e") {
            auto c = dataplotter_->print_significance("mva6", "event", s, ("H"+label), -1., 1., true, -1., false, hlim.Data());
            DataPlotter::Empty_Canvas(c);
            c = dataplotter_->print_significance("mva7", "event", s, ("Z"+label), -1., 1.3, true, -0.71, false, zlim.Data());
            DataPlotter::Empty_Canvas(c);
            if(printLimitVsEff_) {
              c = dataplotter_->print_significance("mva6", "event", s, ("H"+label), 0., 1., true, -1., true, hlim.Data()); //significance vs efficiency
              DataPlotter::Empty_Canvas(c);
              c = dataplotter_->print_significance("mva7", "event", s, ("Z"+label), 0., 1., true, -0.71, true, zlim.Data()); //significance vs efficiency
              DataPlotter::Empty_Canvas(c);
            }
          }
          else if(selection_=="etau_mu") {
            auto c = dataplotter_->print_significance("mva8", "event", s, ("H"+label), -1., 1.,true, -1., false, hlim.Data());
            DataPlotter::Empty_Canvas(c);
            c = dataplotter_->print_significance("mva9", "event", s, ("Z"+label), -1., 1.,true, -1., false, zlim.Data());
            DataPlotter::Empty_Canvas(c);
            if(printLimitVsEff_) {
              c = dataplotter_->print_significance("mva8", "event", s, ("H"+label), 0., 1., true, -1., true, hlim.Data());
              DataPlotter::Empty_Canvas(c);
              c = dataplotter_->print_significance("mva9", "event", s, ("Z"+label), 0., 1., true, -1., true, zlim.Data());
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
      if(selection_ == "mumu")    sets[i] += ZTauTauHistMaker::kMuMu;
      if(selection_ == "ee")      sets[i] += ZTauTauHistMaker::kEE;
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
  hnames.push_back("mtoneoverm");     htypes.push_back("event"); rebins.push_back(1); xmins.push_back(0.);   xmaxs.push_back(3.);
  hnames.push_back("mttwooverm");     htypes.push_back("event"); rebins.push_back(1); xmins.push_back(0.);   xmaxs.push_back(3.);
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

//initialize the dataplotter with data and MC files
Int_t init_dataplotter() {
  bool leptonic_tau = (selection_.Contains("tau_")); //mutau_l, etau_l
  if(dataplotter_) delete dataplotter_;
  dataplotter_ = new DataPlotter();
  if(sigOverBkg_) dataplotter_->data_over_mc_ = -sigOverBkg_;
  dataplotter_->selection_ = selection_;
  dataplotter_->folder_ = folder_;
  dataplotter_->doStatsLegend_ = doStatsLegend_;
  dataplotter_->useOpenGL_ = (gROOT->IsBatch()) ? 0 : useOpenGL_;

  dataplotter_->qcd_scale_ = 1.;
  dataplotter_->include_qcd_ = useQCD_;
  dataplotter_->include_misid_ = useMisID_;

  if(selection_ == "emu")
    {dataplotter_->signal_scale_ = 100.; dataplotter_->signal_scales_["H->e#mu"] = 400.;}
  else if(selection_.Contains("_"))
    {dataplotter_->signal_scale_ = 250.; dataplotter_->signal_scales_["H->#mu#tau"] = 300.; dataplotter_->signal_scales_["H->e#tau"] = 300.;}
  else if(selection_.Contains("tau"))
    {dataplotter_->signal_scale_ = 150.; dataplotter_->signal_scales_["H->#mu#tau"] = 250.; dataplotter_->signal_scales_["H->e#tau"] = 250.;}
  else if(selection_ == "ee" || selection_ == "mumu")
    dataplotter_->signal_scale_ = 2.e4;

  typedef DataCard_t dcard;

  ///////////////////////////////////
  // Defining NANO AOD based files //
  ///////////////////////////////////
  //cross section handler
  CrossSections xs(useUL_);

  std::vector<dcard> cards;
  std::vector<bool> combines;
  bool oneDY = false; //for faster scale factor testing/debugging
  for(int year : years_) {
    //card constructor:    filepath,              name,                  label,      isData,                   xsec               ,  isSignal,year,  color,   combine extension samples
    cards.push_back(dcard("DY50"               , "DY50"               , "Drell-Yan", false, xs.GetCrossSection("DY50"               ), false, year, kRed-7   , !oneDY&&!useUL_&&year!=2018));
    if(!oneDY && year != 2018 && !useUL_) {
      cards.push_back(dcard("DY50-ext"         , "DY50-ext"           , "Drell-Yan", false, xs.GetCrossSection("DY50"               ), false, year, kRed-7   , true));
    }
    cards.push_back(dcard("SingleAntiToptW"    , "SingleAntiToptW"    , "Top"      , false, xs.GetCrossSection("SingleAntiToptW"    ), false, year, kYellow-7));
    cards.push_back(dcard("SingleToptW"        , "SingleToptW"        , "Top"      , false, xs.GetCrossSection("SingleToptW"        ), false, year, kYellow-7));
    cards.push_back(dcard("ttbarToHadronic"    , "ttbarToHadronic"    , "Top"      , false, xs.GetCrossSection("ttbarToHadronic")    , false, year, kYellow-7));
    cards.push_back(dcard("ttbarToSemiLeptonic", "ttbarToSemiLeptonic", "Top"      , false, xs.GetCrossSection("ttbarToSemiLeptonic"), false, year, kYellow-7));
    cards.push_back(dcard("ttbarlnu"           , "ttbarlnu"           , "Top"      , false, xs.GetCrossSection("ttbarlnu"           ), false, year, kYellow-7));
    if(useUL_ == 0)
    cards.push_back(dcard("WWW"                , "WWW"                , "Other VB" , false, xs.GetCrossSection("WWW"                ), false, year, kViolet-9));
    cards.push_back(dcard("WZ"                 , "WZ"                 , "Other VB" , false, xs.GetCrossSection("WZ"                 ), false, year, kViolet-9));
    cards.push_back(dcard("ZZ"                 , "ZZ"                 , "Other VB" , false, xs.GetCrossSection("ZZ"                 ), false, year, kViolet-9));
    cards.push_back(dcard("WW"                 , "WW"                 , "Other VB" , false, xs.GetCrossSection("WW"                 ), false, year, kViolet-9));
    cards.push_back(dcard("Wlnu"               , "Wlnu"               , "Other VB" , false, xs.GetCrossSection("Wlnu"               ), false, year, kViolet-9, !useUL_&&year!=2018));
    if(year != 2018 && !useUL_){
      cards.push_back(dcard("Wlnu-ext"         , "Wlnu-ext"           , "Other VB" , false, xs.GetCrossSection("Wlnu"               ), false, year, kViolet-9, true));
    }
    if(selection_ == "emu") {
      cards.push_back(dcard("ZEMu"             , "ZEMu"             , "Z->e#mu"   , false, xs.GetCrossSection("ZEMu"  ), true, year, kBlue   ));
      cards.push_back(dcard("HEMu"             , "HEMu"             , "H->e#mu"   , false, xs.GetCrossSection("HEMu"  ), true, year, kGreen-1));
    } else if(selection_.Contains("etau") || selection_ == "ee") {
      cards.push_back(dcard("ZETau"            , "ZETau"            , "Z->e#tau"  , false, xs.GetCrossSection("ZETau" ), true, year, kBlue   ));
      cards.push_back(dcard("HETau"            , "HETau"            , "H->e#tau"  , false, xs.GetCrossSection("HETau" ), true, year, kGreen-1));
    } else if(selection_.Contains("mutau") || selection_ == "mumu") {
      cards.push_back(dcard("ZMuTau"           , "ZMuTau"           , "Z->#mu#tau", false, xs.GetCrossSection("ZMuTau"), true, year, kBlue   ));
      cards.push_back(dcard("HMuTau"           , "HMuTau"           , "H->#mu#tau", false, xs.GetCrossSection("HMuTau"), true, year, kGreen-1));
    }
    //Add data
    if(doRunPeriod_ == 0) {
      if(selection_ != "etau"  && selection_!="ee"  ) cards.push_back(dcard("SingleMu" , "SingleMu" , "Data", true , 1., false, year));
      if(selection_ != "mutau" && selection_!="mumu") cards.push_back(dcard("SingleEle", "SingleEle", "Data", true , 1., false, year));
    } else {
      std::vector<TString> periods;
      if(year == 2016) periods = {"B", "C", "D", "E", "F", "G", "H"};
      if(year == 2017) periods = {"B", "C", "D", "E", "F"};
      if(year == 2018) periods = {"A", "B", "C", "D"};
      for(int period = 0; period < periods.size(); ++period) {
        if(year == 2016 && ((doRunPeriod_ == 1 && period > 4) || (doRunPeriod_ == 2 && period < 5))) continue;
        if(year == 2017 && ((doRunPeriod_ == 1 && period > 2) || (doRunPeriod_ == 2 && period < 3))) continue;
        if(year == 2018 && ((doRunPeriod_ == 1 && period > 1) || (doRunPeriod_ == 2 && period < 2))) continue;
        TString p_name = "Run"; p_name += periods[period];
        TString muon_name = "SingleMuon" + p_name;
        TString electron_name = "SingleMuon" + p_name;
        if(selection_ != "etau"  && selection_!="ee"  ) cards.push_back(dcard(muon_name.Data()    , muon_name.Data()    , "Data", true , 1., false, year));
        if(selection_ != "mutau" && selection_!="mumu") cards.push_back(dcard(electron_name.Data(), electron_name.Data(), "Data", true , 1., false, year));
      }
    }
  } //end years loop


  TString selection_dir = (leptonic_tau) ? "emu" : selection_;
  //add full name to file name
  for(unsigned index = 0; index < cards.size(); ++index) {
    //Update Wlnu cross section if combining to account for the two samples
    if(cards[index].combine_) {
      TString name = cards[index].name_;
      bool isext = name.Contains("-ext");
      name.ReplaceAll("-ext", "");
      long num1 = xs.GetGenNumber(name       , cards[index].year_); //get number of events per sample
      long num2 = xs.GetGenNumber(name+"-ext", cards[index].year_);
      if(isext && num1 > 0 && num2 > 0) {
        double frac = (num2*1./(num1+num2));
        cout << "Multiplying the " << name.Data() << "-ext cross section by " << frac
             << " to combine the " << cards[index].year_ << " extension samples!\n";
        cards[index].xsec_ *= frac;
      } else if(num1 > 0 && num2 > 0){
        double frac = (num1*1./(num1+num2));
        cout << "Multiplying the " << name.Data() << " cross section by " << frac
             << " to combine the " << cards[index].year_ << " extension samples!\n";
        cards[index].xsec_ *= frac;
      } else
        cout << "ERROR: Didn't find generation numbers for combining with sample name " << name.Data() << endl;
    }
    //update file path
    cards[index].filename_ = Form("%s/ztautau_%s_clfv_%i_%s.hist", (hist_path_+hist_dir_).Data(), selection_dir.Data(),
                                  cards[index].year_, (cards[index].filename_).Data());
  } //end file name loop

  //Calculate luminosity from year(s)
  double lum = 0.;
  for(int year : years_) {
    double currLum = 0.;
    if(year == 2016) {
      if(doRunPeriod_ == 0)
        currLum = 35.92e3; //pb^-1
      else if(doRunPeriod_ == 1)
        currLum = (5.892 + 2.646 + 4.353 + 4.117 + 3.174)*1.e3; //B-F
      else if(doRunPeriod_ == 2)
        currLum = (7.540 + 8.606)*1.e3; //G-H
    }
    else if(year == 2017) currLum = 41.48e3;
    else if(year == 2018) currLum = 59.74e3;
    dataplotter_->lums_[year] = currLum; //store the luminosity for the year
    lum += currLum; //add to the total luminosity
  }

  dataplotter_->set_luminosity(lum);
  dataplotter_->verbose_ = verbose_;

  for(auto card : cards)
    dataplotter_->add_dataset(card);

  //make figure directory if it doesn't exist
  TString year_dir = "";
  for(unsigned index = 0; index < years_.size(); ++index) {
    year_dir += years_[index];
    if(index != years_.size() - 1) year_dir += "_";
  }
  gSystem->Exec(Form("[ ! -d \"figures/%s/%s/%s\" ] && mkdir -p \"figures/%s/%s/%s\"",
                     folder_.Data(), selection_.Data(), year_dir.Data(), folder_.Data(), selection_.Data(), year_dir.Data()));
  dataplotter_->years_ = years_;
  return dataplotter_->init_files();
}

Int_t nanoaod_init(TString selection = "emu", TString histDir = "nanoaods", TString figureDir = "nanoaods") {
  if(histDir != "") hist_dir_ = histDir;
  if(figureDir != "") folder_ = figureDir;
  if(hist_dir_.Contains("_UL")) useUL_ = 1;
  else                          useUL_ = 0;

  selection_ = selection;
  useMisID_ = selection == "mutau" || selection == "etau";
  useQCD_ = !useMisID_ && selection != "mumu" && selection != "ee";
  offsetSets_ = selection != "etau_mu" && selection != "mutau_e";
  Int_t status = init_dataplotter();
  return status;
}

Int_t print_mva_selections(TString histDir = "", TString figureDir = "") {
  if(histDir != "") hist_dir_ = histDir;
  if(figureDir != "") folder_ = figureDir;
  vector<TString> selections = {"mutau", "etau", "emu", "mutau_e", "etau_mu"};
  int status(0);
  for(TString selection : selections) {
    status += nanoaod_init(selection, "", "");
    if(status) continue;
    vector<PlottingCard_t> cards;
    get_mva_cards(cards);
    int offset = 0;
    if     (selection == "mutau"  ) offset = ZTauTauHistMaker::kMuTau;
    else if(selection == "etau"   ) offset = ZTauTauHistMaker::kETau;
    else if(selection == "emu"    ) offset = ZTauTauHistMaker::kEMu;
    else if(selection == "etau_mu") offset = ZTauTauHistMaker::kEMu;
    else if(selection == "mutau_e") offset = ZTauTauHistMaker::kEMu;
    else if(selection == "ee"     ) offset = ZTauTauHistMaker::kEE;
    else if(selection == "mumu"   ) offset = ZTauTauHistMaker::kMuMu;
    dataplotter_->print_stacks(cards,{7+offset,8+offset},{},{});
  }
  return status;
}

Int_t print_emu_dataset_plots(TString histDir = "", TString figureDir = "") {
  if(histDir != "") hist_dir_ = histDir;
  if(figureDir != "") folder_ = figureDir;
  TStopwatch* timer = new TStopwatch();
  Int_t status = 0;
  vector<PlottingCard_t> plottingcards;
  vector<int>    sets   = {  48,  49, 50,   89,  90,  91,  92};
  vector<double> scales = {250., 25., 25., 25., 25., 25., 25.};
  // vector<int>    rebins = {   1,   1,   2,   2,   2,   2,   2};
  doAllEMu_ = true;
  selection_ = "emu";
  init_dataplotter();
  dataplotter_->stack_as_hist_ = 1;
  dataplotter_->draw_statistics_ = false;
  dataplotter_->legend_y2_ = 0.25;

  plottingcards.push_back(PlottingCard_t("mva4", "event", 200, -1.,   1. , 0.01, 1. ));
  plottingcards.push_back(PlottingCard_t("mva5", "event", 200, -1.,   1. , 0.01, 1. ));
  plottingcards.push_back(PlottingCard_t("mva6", "event", 200, -1.,   1. , 0.01, 1. ));
  plottingcards.push_back(PlottingCard_t("mva7", "event", 200, -1.,   1. , 0.01, 1. ));
  plottingcards.push_back(PlottingCard_t("mva8", "event", 200, -1.,   1. , 0.01, 1. ));
  plottingcards.push_back(PlottingCard_t("mva9", "event", 200, -1.,   1. , 0.01, 1. ));

  for(unsigned i = 0; i < sets.size(); ++i) {
    dataplotter_->signal_scale_ = scales[i];
    for(PlottingCard_t card : plottingcards) {
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
  vector<PlottingCard_t> plottingcards;
  plottingcards.push_back(PlottingCard_t("lepm", "event", 1, 75.,   110., {84, 118}, {98, 132.} ));
  // plottingcards.push_back(PlottingCard_t("met" , "event", 1, 0., 45.));
  // plottingcards.push_back(PlottingCard_t("puppmet" , "event", 1, 0., 45.));
  vector<Int_t> sets = {7+ZTauTauHistMaker::kEMu,8+ZTauTauHistMaker::kEMu};
  vector<Double_t> scales = {50.,50.};
  vector<Int_t> rebins = {1,1};
  dataplotter_->logY_ = 1;
  status += dataplotter_->print_stacks(plottingcards, sets, scales, rebins);
  dataplotter_->logY_ = 0;
  status += dataplotter_->print_stacks(plottingcards, sets, scales, rebins);
  return status;
}

//print cards for each selection
Int_t print_cards_standard_sets(vector<PlottingCard_t> cards, bool doSameFlavor = false) {
  vector<int> sets = {7,8};
  vector<TString> selections = {"mutau", "etau", "emu", "mutau_e", "etau_mu"};
  if(doSameFlavor) {selections.push_back("mumu"); selections.push_back("ee");}
  Int_t status = 0;
  for(TString selection : selections) {
    status += nanoaod_init(selection, "", "");
    for(int index = 0; index < sets.size(); ++index) {
      int set = sets[index];
      if     (selection_ == "mutau"  ) set += ZTauTauHistMaker::kMuTau;
      else if(selection_ == "etau"   ) set += ZTauTauHistMaker::kETau;
      else if(selection_ == "emu"    ) set += ZTauTauHistMaker::kEMu;
      else if(selection_ == "mutau_e") set += ZTauTauHistMaker::kEMu;
      else if(selection_ == "etau_mu") set += ZTauTauHistMaker::kEMu;
      else if(selection_ == "mumu")    set += ZTauTauHistMaker::kMuMu;
      else if(selection_ == "ee"  )    set += ZTauTauHistMaker::kEE;
      for(auto card : cards) {
        card.set_ = set;
        auto c = dataplotter_->print_stack(card);
        status += (c) ? 0 : 1;
        if(c) DataPlotter::Empty_Canvas(c);
      }
    }
  }
  return status;
}

Int_t print_card_standard_sets(PlottingCard_t card, bool doSameFlavor = false) {
  return print_cards_standard_sets({card}, doSameFlavor);
}

//print a card for each year and full Run-II
Int_t print_card_standard_years(PlottingCard_t card) {
  vector<int> sets = {7,8};
  Int_t status = 0;
  for(int year_i = 0; year_i < 4; ++year_i) {
    if     (year_i == 0) years_ = {2016};
    else if(year_i == 1) years_ = {2017};
    else if(year_i == 2) years_ = {2018};
    else if(year_i == 3) years_ = {2016, 2017, 2018};
    status += init_dataplotter();
    for(int index = 0; index < sets.size(); ++index) {
      card.set_ = sets[index];
      double scale = 150.;
      if     (selection_ == "mutau"  ) card.set_ += ZTauTauHistMaker::kMuTau;
      else if(selection_ == "etau"   ) card.set_ += ZTauTauHistMaker::kETau;
      else if(selection_ == "emu"    ) {scale = 100.; card.set_ += ZTauTauHistMaker::kEMu;}
      else if(selection_ == "mutau_e") {scale = 100.; card.set_ += ZTauTauHistMaker::kEMu;}
      else if(selection_ == "etau_mu") {scale = 100.; card.set_ += ZTauTauHistMaker::kEMu;}
      else if(selection_ == "mumu") {scale = 1.e4; card.set_ += ZTauTauHistMaker::kMuMu;}
      else if(selection_ == "ee"  ) {scale = 1.e4; card.set_ += ZTauTauHistMaker::kEE;}
      // dataplotter_->signal_scale_ = scale;
      auto c = dataplotter_->print_stack(card);
      status += (c) ? 0 : 1;
      if(c) DataPlotter::Empty_Canvas(c);
    }
  }
  return status;
}

//print mutau QCD mystery plots
Int_t print_mutau_qcd_plots() {
  std::vector<PlottingCard_t> cards;
  cards.push_back(PlottingCard_t("lepm"          , "event", 8, 2, 45., 200.));
  cards.push_back(PlottingCard_t("lepdeltar"     , "event", 8, 1,  0.,   5.));
  cards.push_back(PlottingCard_t("taudecaymode"  , "event", 8, 0,  0.,  15.));
  cards.push_back(PlottingCard_t("taudeepantiele", "event", 8, 0,  0.,  30.));
  cards.push_back(PlottingCard_t("taudeepantimu" , "event", 8, 0,  0.,  30.));
  cards.push_back(PlottingCard_t("taudeepantijet", "event", 8, 0,  0.,  30.));
  cards.push_back(PlottingCard_t("onept"         , "lep"  , 8, 2, 25., 100.));
  cards.push_back(PlottingCard_t("twopt"         , "lep"  , 8, 2, 20.,  80.));
  cards.push_back(PlottingCard_t("twoeta"        , "lep"  , 8, 2, -3.,   5.));
  cards.push_back(PlottingCard_t("twoid1"        , "lep"  , 8, 0,  0.,  50.));
  cards.push_back(PlottingCard_t("twoid2"        , "lep"  , 8, 0,  0.,  10.));
  std::vector<int> sets = {8};
  int status(0);
  doRunPeriod_ = 0;
  years_ = {2016};
  nanoaod_init("mutau", "nanoaods_dev", "nanoaods_dev");
  status += dataplotter_->print_stacks(cards, sets, {}, {});
  years_ = {2017};
  nanoaod_init("mutau", "nanoaods_dev", "nanoaods_dev");
  status += dataplotter_->print_stacks(cards, sets, {}, {});
  years_ = {2018};
  nanoaod_init("mutau", "nanoaods_dev", "nanoaods_dev");
  status += dataplotter_->print_stacks(cards, sets, {}, {});
  years_ = {2016};
  doRunPeriod_ = 1;
  nanoaod_init("mutau", "nanoaods_dev", "nanoaods_dev_1");
  status += dataplotter_->print_stacks(cards, sets, {}, {});
  years_ = {2016};
  doRunPeriod_ = 2;
  nanoaod_init("mutau", "nanoaods_dev", "nanoaods_dev_2");
  status += dataplotter_->print_stacks(cards, sets, {}, {});
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

//print QCD ABCD method plots
Int_t print_qcd_plots() {
  Int_t status = 0;
  if(!dataplotter_) return 1;
  int prev = dataplotter_->include_qcd_;
  dataplotter_->include_qcd_ = 0;
  int offset = 0;
  if     (selection_ == "mutau"  ) offset = ZTauTauHistMaker::kMuTau;
  else if(selection_ == "etau"   ) offset = ZTauTauHistMaker::kETau;
  else if(selection_ == "emu"    ) offset = ZTauTauHistMaker::kEMu;
  else if(selection_ == "mutau_e") offset = ZTauTauHistMaker::kEMu;
  else if(selection_ == "etau_mu") offset = ZTauTauHistMaker::kEMu;
  else if(selection_ == "mumu")    offset = ZTauTauHistMaker::kMuMu;
  else if(selection_ == "ee"  )    offset = ZTauTauHistMaker::kEE;
  vector<int> sets = {4+offset, 4+offset+ZTauTauHistMaker::fQcdOffset,
                      8+offset, 8+offset+ZTauTauHistMaker::fQcdOffset};
  vector<PlottingCard_t> cards;
  cards.push_back(PlottingCard_t("leppt", "event", 2,  0., 170.));
  cards.push_back(PlottingCard_t("lepm" , "event", 2, 50., 170.));
  bool evenodd = true; //for alternating
  double nss, nos, erros, errss;
  for(int set : sets) {
    for(PlottingCard_t card : cards) {
      card.set_ = set;
      TCanvas* c;
      c = dataplotter_->print_stack(card);
      if(c) {
        TVirtualPad* c1 = (TVirtualPad*) c->GetListOfPrimitives()->At(0);
        TH1D* hMC   = (TH1D*) c1->GetPrimitive(Form("uncertainty_%s_%i", card.hist_.Data(), card.set_));
        TH1D* hData = (TH1D*) c1->GetPrimitive(Form("hData_%s_%i", card.hist_.Data(), card.set_));
        if(!hMC || !hData) cout << "MC or Data histogram not found!\n";
        else {
          double errmc, errdata;
          double nmc   = hMC->IntegralAndError(0, hMC->GetNbinsX()+1, errmc);
          double ndata = hData->IntegralAndError(0, hData->GetNbinsX()+1, errdata);
          double ndiff = ndata - nmc;
          double errdiff = sqrt(errmc*errmc + errdata*errdata);
          cout << "N(Data) - N(MC) = " << ndata << " +- " << errdata
               << "  -  " << nmc << " +- " << errmc << " = " << ndiff
               << " +- " << errdiff << endl
               << "(N(Data) - N(MC)) / N(Data) = " << ndiff/ndata << endl;
          if(evenodd) {nos = ndiff; erros = errdiff;}
          else {nss = ndiff; errss = errdiff;}
        }
      } else
        status += 1;
      DataPlotter::Empty_Canvas(c);
    }
    if(!evenodd) {
      printf("OS / SS = %.5f +- %.5f\n", nos/nss, nos/nss*sqrt(errss*errss/nss/nss + erros*erros/nos/nos));
    }
    evenodd = !evenodd;
  }
  dataplotter_->include_qcd_ = prev;
  return status;
}

//print MisID method plots
Int_t print_misid_plots() {
  Int_t status = 0;
  if(!dataplotter_) return 1;
  dataplotter_->include_qcd_ = 0;
  int offset = 0;
  if     (selection_ == "mutau"  ) offset = ZTauTauHistMaker::kMuTau;
  else if(selection_ == "etau"   ) offset = ZTauTauHistMaker::kETau;
  else if(selection_ == "emu"    ) offset = ZTauTauHistMaker::kEMu;
  else if(selection_ == "mutau_e") offset = ZTauTauHistMaker::kEMu;
  else if(selection_ == "etau_mu") offset = ZTauTauHistMaker::kEMu;
  else if(selection_ == "mumu")    offset = ZTauTauHistMaker::kMuMu;
  else if(selection_ == "ee"  )    offset = ZTauTauHistMaker::kEE;
  vector<int> sets = {
                      35+offset,
                      8+offset,
                      // 34+offset,
                      // 36+offset,
                      // 34+offset+ZTauTauHistMaker::fQcdOffset,
                      // 36+offset+ZTauTauHistMaker::fMisIDOffset,
                      35+offset+ZTauTauHistMaker::fMisIDOffset,
                      8 +offset+ZTauTauHistMaker::fMisIDOffset,
                      35+offset+ZTauTauHistMaker::fQcdOffset,
                      8 +offset+ZTauTauHistMaker::fQcdOffset,
                      35+offset+ZTauTauHistMaker::fQcdOffset+ZTauTauHistMaker::fMisIDOffset,
                      8 +offset+ZTauTauHistMaker::fQcdOffset+ZTauTauHistMaker::fMisIDOffset
  };

  vector<PlottingCard_t> cards;
  cards.push_back(PlottingCard_t("leppt"         , "event", 2,  0., 170.));
  if(selection_ == "emu" || selection_.Contains("_"))
    cards.push_back(PlottingCard_t("lepm"          , "event", 2, 50., 170., {84, 118}, {98, 132}));
  else
    cards.push_back(PlottingCard_t("lepm"          , "event", 2, 50., 170.));
  cards.push_back(PlottingCard_t("twopt"         , "lep"  , 2, 20., 100.));
  cards.push_back(PlottingCard_t("twoeta"        , "lep"  , 2, -3.,   5.));
  cards.push_back(PlottingCard_t("taudecaymode"  , "event", 0,  0.,  15.));
  cards.push_back(PlottingCard_t("taudecaymode1" , "event", 0,  0.,  15.));
  cards.push_back(PlottingCard_t("taugenflavor"  , "event", 0, 10.,  40.));
  cards.push_back(PlottingCard_t("taudeepantijet", "event", 0,  0.,  30.));
  cards.push_back(PlottingCard_t("taudeepantiele", "event", 0,  0.,  30.));
  cards.push_back(PlottingCard_t("onept"         , "lep"  , 2, 20., 120.));
  cards.push_back(PlottingCard_t("oneeta"        , "lep"  , 2, -3.,   5.));
  cards.push_back(PlottingCard_t("met"           , "event", 2,  0., 120.));
  cards.push_back(PlottingCard_t("mtone"         , "event", 4,  0., 150.));
  cards.push_back(PlottingCard_t("mttwo"         , "event", 4,  0., 150.));
  cards.push_back(PlottingCard_t("mtlep"         , "event", 4,  0., 150.));
  cards.push_back(PlottingCard_t("lepdeltaphi"   , "event", 1,  0.,   5.));
  cards.push_back(PlottingCard_t("lepdeltaeta"   , "event", 5,  0.,   5.));
  cards.push_back(PlottingCard_t("lepdeltar"     , "event", 5,  0.,   5.));
  cards.push_back(PlottingCard_t("njets"         , "event", 1,  0.,  10.));
  for(int qcd = 0; qcd < 2; ++qcd) {
    dataplotter_->include_qcd_ = qcd;
    dataplotter_->include_misid_ = 1 - qcd;
    cout << "Printing sets using QCD = " << qcd << endl;
    for(int set : sets) {
      //skip MC estimated fake tau set for emu
      if(set % 100 == 35) continue;
      for(PlottingCard_t card : cards) {
        card.set_ = set;
        TCanvas* c;
        dataplotter_->logY_ = (card.hist_ == "njets");
        c = dataplotter_->print_stack(card);
        if(c) {
          DataPlotter::Empty_Canvas(c);
        }
      }
    }
  }
  return status;
}

//print MisID plots for each year
Int_t print_all_misid_plots() {
  int status(0);

  years_ = {2016};
  nanoaod_init(selection_, hist_dir_, folder_);
  status += print_misid_plots();

  years_ = {2017};
  nanoaod_init(selection_, hist_dir_, folder_);
  status += print_misid_plots();

  years_ = {2018};
  nanoaod_init(selection_, hist_dir_, folder_);
  status += print_misid_plots();

  years_ = {2016, 2017, 2018};
  nanoaod_init(selection_, hist_dir_, folder_);
  status += print_misid_plots();

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
  useQCD_ = 0;
  useMisID_ = 1;
  status += init_dataplotter();
  status += print_standard_plots({7,8});
  if(printBlindSets_) status += print_blind_sets({9,10}, {20.,20.}, {2, 2});

  selection_ = "etau";
  useQCD_ = 0;
  useMisID_ = 1;
  status += init_dataplotter();
  status += print_standard_plots({7,8});
  if(printBlindSets_) status += print_blind_sets({9,10}, {20.,20.}, {2, 2});

  selection_ = "emu";
  useQCD_ = 1;
  useMisID_ = 0;
  status += init_dataplotter();
  status += print_standard_plots({7,8});
  // status += print_emu_cutsets();
  if(printBlindSets_) status += print_blind_sets({9,10,11,12}, {2.,2.,10.,10.}, {2,2,2,2});

  selection_ = "mutau_e";
  useQCD_ = 1;
  useMisID_ = 0;
  status += init_dataplotter();
  offsetSets_ = false;
  status += print_standard_plots({207,208});
  offsetSets_ = true;
  if(printBlindSets_) status += print_blind_sets({9,10}, {2.,2.}, {2,2});

  selection_ = "etau_mu";
  useQCD_ = 1;
  useMisID_ = 0;
  status += init_dataplotter();
  offsetSets_ = false;
  status += print_standard_plots({207,208});
  offsetSets_ = true;
  if(printBlindSets_) status += print_blind_sets({9,10}, {2.,2.}, {2,2});

  selection_ = "mumu";
  useQCD_ = 0;
  useMisID_ = 0;
  status += init_dataplotter();
  status += print_standard_plots({7, 8}, {2.e4, 2.e4});

  selection_ = "ee";
  useQCD_ = 0;
  useMisID_ = 0;
  status += init_dataplotter();
  status += print_standard_plots({7, 8}, {2.e4, 2.e4});

  Double_t cpuTime = timer->CpuTime();
  Double_t realTime = timer->RealTime();
  printf("Processing time: %7.2fs CPU time %7.2fs Wall time\n",cpuTime,realTime);
  if(realTime > 600. ) printf("Processing time: %7.2fmin CPU time %7.2fmin Wall time\n",cpuTime/60.,realTime/60.);
  return status;
}

//print the standard selection for all years (and combinations) of interest
Int_t print_all_years(TString histDir = "", TString figureDir = "") {
  int status(0);
  years_ = {2016};
  status += print_standard_selections(histDir, figureDir);
  years_[0] = 2017;
  status += print_standard_selections(histDir, figureDir);
  years_[0] = 2018;
  status += print_standard_selections(histDir, figureDir);
  years_ = {2016, 2017, 2018};
  status += print_standard_selections(histDir, figureDir);
  return status;
}

Int_t print_hist_all_selections(PlottingCard_t card, bool offset_lep_tau = false,
                                TString histDir = "", TString figureDir = "") {
  int status(0), orig_set(card.set_);
  TCanvas* c = 0;
  selection_ = "mutau";
  status += init_dataplotter();
  card.set_ = orig_set + ZTauTauHistMaker::kMuTau;
  c = dataplotter_->print_stack(card);
  status += c == 0;
  // if(c) DataPlotter::Empty_Canvas(c);

  selection_ = "etau";
  status += init_dataplotter();
  card.set_ = orig_set + ZTauTauHistMaker::kETau;
  dataplotter_->print_stack(card);
  status += c == 0;
  // if(c) DataPlotter::Empty_Canvas(c);

  selection_ = "emu";
  status += init_dataplotter();
  card.set_ = orig_set + ZTauTauHistMaker::kEMu;
  dataplotter_->print_stack(card);
  status += c == 0;
  // if(c) DataPlotter::Empty_Canvas(c);

  selection_ = "mutau_e";
  status += init_dataplotter();
  if(offset_lep_tau)
    card.set_ = orig_set + ZTauTauHistMaker::kMuTauE;
  else
    card.set_ = orig_set + ZTauTauHistMaker::kEMu;
  dataplotter_->print_stack(card);
  status += c == 0;
  // if(c) DataPlotter::Empty_Canvas(c);

  selection_ = "etau_mu";
  status += init_dataplotter();
  if(offset_lep_tau)
    card.set_ = orig_set + ZTauTauHistMaker::kETauMu;
  else
    card.set_ = orig_set + ZTauTauHistMaker::kEMu;
  dataplotter_->print_stack(card);
  status += c == 0;
  // if(c) DataPlotter::Empty_Canvas(c);

  // selection_ = "mumu";
  // status += init_dataplotter();
  // card.set_ = orig_set + ZTauTauHistMaker::kMuMu;
  // dataplotter_->print_stack(card);
  // status += c == 0;
  // // if(c) DataPlotter::Empty_Canvas(c);

  // selection_ = "ee";
  // status += init_dataplotter();
  // card.set_ = orig_set + ZTauTauHistMaker::kEE;
  // dataplotter_->print_stack(card);
  // status += c == 0;
  // // if(c) DataPlotter::Empty_Canvas(c);

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


//Print cut-and-count cards for Higgs Combine
Int_t print_combine_cards() {
  int status(0);
  selection_ = "mutau";
  status += init_dataplotter();
  status += print_combine_card("ntriggered", "event", "Z->#mu#tau");
  status += print_combine_card("ntriggered", "event", "H->#mu#tau");
  selection_ = "etau";
  status += init_dataplotter();
  status += print_combine_card("ntriggered", "event", "Z->e#tau");
  status += print_combine_card("ntriggered", "event", "H->e#tau");
  selection_ = "emu";
  status += init_dataplotter();
  status += print_combine_card("lepm", "event", "Z->e#mu",  86.,  96.); //do in mass window
  status += print_combine_card("lepm", "event", "H->e#mu", 120., 130.); //do in mass window
  selection_ = "mutau_e";
  status += init_dataplotter();
  status += print_combine_card("ntriggered", "event", "Z->#mu#tau");
  status += print_combine_card("ntriggered", "event", "H->#mu#tau");
  selection_ = "etau_mu";
  status += init_dataplotter();
  status += print_combine_card("ntriggered", "event", "Z->e#tau");
  status += print_combine_card("ntriggered", "event", "H->e#tau");

  return status;
}
