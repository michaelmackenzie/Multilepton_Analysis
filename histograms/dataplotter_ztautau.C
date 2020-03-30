

DataPlotter* dataplotter_ = 0;
TString selection_ = "mutau"; //current options: mutau, etau, emu, mutau_e, etau_mu, mumu
Int_t useOpenGL_ = 0;
bool  doStatsLegend_ = false;
TString hist_dir_ = "ztautau";
TString folder_ = "clfv_zdecays";

Int_t print_significance_canvases(vector<TString> hists, vector<TString> types, vector<TString> labels, vector<int> sets) {
  TCanvas* c = 0;
  if(!dataplotter_) return -2;
  
  if(hists.size() != types.size() || labels.size() != hists.size())
    return -1;
  int n = hists.size();
  for(int set : sets) {
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
      delete c;
      delete f;
    }
  }
  return 0;
}

Int_t print_statistics(TString hist, TString type, int set) {
  if(!dataplotter_) return -1;
  
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
	    double integral = hist->IntegralAndError(0,hist->GetNbinsX()+1,error);
	    cout << "--> " << hist->GetTitle() << ": " << integral
		 <<  " +- " << error << " (" << error*100./integral << "%, nentries = " << hist->GetEntries() << ")" << endl;
	    total += integral;
	    variance += error*error;
	  }
	}
      }
    } catch(exception e) {
    }
  }
  cout << "Total = " << total << " +- " << sqrt(variance)
       << " (" << sqrt(variance)/total*100. << "%)" <<  endl;

  return 0;
}

TCanvas* print_canvas(TString hist, TString type, int set, bool stacks = true, TString name = "") {
  if(!dataplotter_) {
    cout << "Dataplotter isn't initialized!\n";
    return NULL;
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
  auto c = print_canvas(hist, type, set, stacks, name);
  dataplotter_->reset_axes();
  return c;
}
Int_t print_standard_plots(vector<int> sets, vector<double> signal_scales = {},
			   vector<int> base_rebins = {}, bool stacks = true) {
  if(!dataplotter_) return -1;
  vector<TString> hnames;
  vector<TString> htypes;
  vector<int>     rebins;
  vector<double>  xmins;
  vector<double>  xmaxs;
  
  hnames.push_back("onept");          htypes.push_back("lep");   rebins.push_back(2); xmins.push_back(15.);  xmaxs.push_back(150.);
  hnames.push_back("twopt");          htypes.push_back("lep");   rebins.push_back(2); xmins.push_back(15.);  xmaxs.push_back(150.);
  hnames.push_back("lepm");           htypes.push_back("event"); rebins.push_back(2); xmins.push_back(0.);   xmaxs.push_back(200.);
  hnames.push_back("leppt");          htypes.push_back("event"); rebins.push_back(2); xmins.push_back(0.);   xmaxs.push_back(150.);
  hnames.push_back("lepptoverm");     htypes.push_back("event"); rebins.push_back(2); xmins.push_back(0.);   xmaxs.push_back(10.);
  hnames.push_back("lepeta");         htypes.push_back("event"); rebins.push_back(1); xmins.push_back(-5.);  xmaxs.push_back(5.);
  hnames.push_back("lepdeltaeta");    htypes.push_back("event"); rebins.push_back(1); xmins.push_back(0.);   xmaxs.push_back(5.);
  hnames.push_back("lepdeltaphi");    htypes.push_back("event"); rebins.push_back(1); xmins.push_back(0.);   xmaxs.push_back(3.5);
  hnames.push_back("lepdeltar");      htypes.push_back("event"); rebins.push_back(1); xmins.push_back(0.);   xmaxs.push_back(5.);
  hnames.push_back("pxivis0");        htypes.push_back("event"); rebins.push_back(5); xmins.push_back(0.);   xmaxs.push_back(100.);
  hnames.push_back("pxiinv0");        htypes.push_back("event"); rebins.push_back(5); xmins.push_back(-100.);xmaxs.push_back(100.);
  hnames.push_back("ptauvisfrac");    htypes.push_back("event"); rebins.push_back(5); xmins.push_back(0.);   xmaxs.push_back(1.4);
  hnames.push_back("lepmestimate");   htypes.push_back("event"); rebins.push_back(1); xmins.push_back(0.);   xmaxs.push_back(300.);
  hnames.push_back("lepmestimatetwo");htypes.push_back("event"); rebins.push_back(1); xmins.push_back(0.);   xmaxs.push_back(300.);
  hnames.push_back("met");            htypes.push_back("event"); rebins.push_back(2); xmins.push_back(0.);   xmaxs.push_back(150.);
  hnames.push_back("covmet00");       htypes.push_back("event"); rebins.push_back(10);xmins.push_back(100.); xmaxs.push_back(1000.);
  hnames.push_back("covmet11");       htypes.push_back("event"); rebins.push_back(10);xmins.push_back(100.); xmaxs.push_back(1000.);
  hnames.push_back("covmet01");       htypes.push_back("event"); rebins.push_back(1); xmins.push_back(-100.);xmaxs.push_back(100.);
  hnames.push_back("mtone");          htypes.push_back("event"); rebins.push_back(4); xmins.push_back(0.);   xmaxs.push_back(150.);
  hnames.push_back("mttwo");          htypes.push_back("event"); rebins.push_back(4); xmins.push_back(0.);   xmaxs.push_back(150.);
  if(selection_ == "mutau" || selection_ == "etau") {
    hnames.push_back("twom");         htypes.push_back("lep");   rebins.push_back(5); xmins.push_back(0.);   xmaxs.push_back(10.);
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
  
  hnames.push_back("njets");          htypes.push_back("event"); rebins.push_back(1); xmins.push_back(0.);   xmaxs.push_back(10.);
  hnames.push_back("nfwdjets");       htypes.push_back("event"); rebins.push_back(1); xmins.push_back(0.);   xmaxs.push_back(10.);
  hnames.push_back("nbjets");         htypes.push_back("event"); rebins.push_back(1); xmins.push_back(0.);   xmaxs.push_back(10.);
  hnames.push_back("nphotons");       htypes.push_back("event"); rebins.push_back(1); xmins.push_back(0.);   xmaxs.push_back(5.);
  if(selection_ == "mutau") {
    hnames.push_back("mva0");         htypes.push_back("event"); rebins.push_back(200); xmins.push_back(-1.);  xmaxs.push_back(1.);
    hnames.push_back("mva1");         htypes.push_back("event"); rebins.push_back(200); xmins.push_back(-1.);  xmaxs.push_back(2.0);
  } else if(selection_ == "etau") {
    hnames.push_back("mva2");         htypes.push_back("event"); rebins.push_back(200); xmins.push_back(-1.);  xmaxs.push_back(1.);
    hnames.push_back("mva3");         htypes.push_back("event"); rebins.push_back(200); xmins.push_back(-1.);  xmaxs.push_back(1.);
  } else if(selection_ == "emu") {
    hnames.push_back("mva4");         htypes.push_back("event"); rebins.push_back(200); xmins.push_back(-1.);  xmaxs.push_back(1.);
    hnames.push_back("mva5");         htypes.push_back("event"); rebins.push_back(200); xmins.push_back(-1.);  xmaxs.push_back(1.);
  }
  
  hnames.push_back("htsum");          htypes.push_back("event"); rebins.push_back(5); xmins.push_back(0.);   xmaxs.push_back(800.);
  hnames.push_back("ht");             htypes.push_back("event"); rebins.push_back(5); xmins.push_back(0.);   xmaxs.push_back(800.);
  hnames.push_back("htdeltaphi");     htypes.push_back("event"); rebins.push_back(1); xmins.push_back(0.);   xmaxs.push_back(3.5);
  hnames.push_back("metdeltaphi");    htypes.push_back("event"); rebins.push_back(1); xmins.push_back(0.);   xmaxs.push_back(3.5);
  hnames.push_back("leponedeltaphi"); htypes.push_back("event"); rebins.push_back(1); xmins.push_back(0.);   xmaxs.push_back(3.5);
  hnames.push_back("leptwodeltaphi"); htypes.push_back("event"); rebins.push_back(1); xmins.push_back(0.);   xmaxs.push_back(3.5);
  hnames.push_back("onemetdeltaphi"); htypes.push_back("lep");   rebins.push_back(1); xmins.push_back(0.);   xmaxs.push_back(3.5);
  hnames.push_back("twometdeltaphi"); htypes.push_back("lep");   rebins.push_back(1); xmins.push_back(0.);   xmaxs.push_back(3.5);

  hnames.push_back("oned0");          htypes.push_back("lep");   rebins.push_back(2); xmins.push_back(-0.05); xmaxs.push_back(0.05);
  hnames.push_back("oneiso");         htypes.push_back("lep");   rebins.push_back(1); xmins.push_back(0.);   xmaxs.push_back(10.);
  hnames.push_back("onereliso");      htypes.push_back("lep");   rebins.push_back(1); xmins.push_back(0.);   xmaxs.push_back(0.15);
  hnames.push_back("twod0");          htypes.push_back("lep");   rebins.push_back(2); xmins.push_back(-0.05); xmaxs.push_back(0.05);
  if(selection_ == "emu" || selection_.Contains("_") /*leptonic tau decay*/) {
    hnames.push_back("twoiso");       htypes.push_back("lep");   rebins.push_back(1); xmins.push_back(0.);   xmaxs.push_back(10.);
    hnames.push_back("tworeliso");    htypes.push_back("lep");   rebins.push_back(1); xmins.push_back(0.);   xmaxs.push_back(0.15);
  }
  TString label = "";
  if(selection_ == "emu")
    label = "->e#mu";
  else if(selection_.Contains("etau"))
    label = "->e#tau";
  else if(selection_.Contains("mutau"))
    label = "->#mu#tau";

  //2D histograms
  if(label != "") {
    for(int data = 0; data < 2; ++data) { //with/without data
      dataplotter_->plot_data_ = data;
      for(int logz = 0; logz < 2; ++logz) { //print log and not log z axis plots
	dataplotter_->logZ_ = logz;
	for(int s : sets) {
	  if(data) dataplotter_->rebinH_ = 4;
	  else 	 dataplotter_->rebinH_ = 2;

	  auto c = dataplotter_->print_single_2Dhist("pxiinvvsvis0", "event", s, ("Z"+label), 0, 80, -80, 60);
	  delete c;
	  c = dataplotter_->print_single_2Dhist("pxiinvvsvis0", "event", s, ("H"+label), 0, 80, -80, 60);
	  delete c;
	  c = dataplotter_->print_single_2Dhist("metvspt"     , "event", s, ("Z"+label), 0, 150,    0, 150);
	  delete c;
	  c = dataplotter_->print_single_2Dhist("metvspt"     , "event", s, ("H"+label), 0, 150,    0, 150);
	  delete c;
	  c = dataplotter_->print_single_2Dhist("twoptvsonept", "lep"  , s, ("Z->ee/#mu#mu"), 0, 150,    0, 150);
	  delete c;
	  c = dataplotter_->print_single_2Dhist("twoptvsonept", "lep"  , s, ("Z->#tau#tau"), 0, 150,    0, 150);
	  delete c;
	  c = dataplotter_->print_single_2Dhist("twoptvsonept", "lep"  , s, ("Z"+label), 0, 150,    0, 150);
	  delete c;
	  c = dataplotter_->print_single_2Dhist("twoptvsonept", "lep"  , s, ("H"+label), 0, 150,    0, 150);
	  delete c;
	}
      }
    }
  } //end 2D loop
  
  //print cdf transform and significance vs bdt score plots
  // vector<TString> sighists;
  // vector<TString> sigtypes;
  // vector<TString> siglabels;
  // if(selection_ == "mutau") {
  //   sighists.push_back("mva0"); sigtypes.push_back("event"); siglabels.push_back("H"+label);
  //   sighists.push_back("mva1"); sigtypes.push_back("event"); siglabels.push_back("Z"+label);
  // } else if(selection_ == "etau") {
  //   sighists.push_back("mva2"); sigtypes.push_back("event"); siglabels.push_back("H"+label);
  //   sighists.push_back("mva3"); sigtypes.push_back("event"); siglabels.push_back("Z"+label);
  // } else if(selection_ == "emu") {
  //   sighists.push_back("mva4"); sigtypes.push_back("event"); siglabels.push_back("H"+label);
  //   sighists.push_back("mva5"); sigtypes.push_back("event"); siglabels.push_back("Z"+label);
  // }
  // print_significance_canvases(sighists, sigtypes, siglabels, sets);
  
  if(label != "") {
    for(int logy = 0; logy < 2; ++logy) { //print log and not log axis
      dataplotter_->logY_ = logy;
      for(int s : sets) {
	if(selection_=="mutau") {
	  auto c = dataplotter_->print_cdf("mva0", "event", s, ("H"+label), 0., 1.7);
	  delete c;
	  c = dataplotter_->print_cdf("mva1", "event", s, ("Z"+label), 0., 1.7);
	  delete c;
	  c = dataplotter_->print_significance("mva0", "event", s, ("H"+label), -1., 1.);
	  delete c;
	  c = dataplotter_->print_significance("mva1", "event", s, ("Z"+label), -1., 1.3, true, 0.71);
	  delete c;
	  c = dataplotter_->print_significance("mva0", "event", s, ("H"+label), 0., 1., true, -1., true); //significance vs efficiency
	  delete c;
	  c = dataplotter_->print_significance("mva1", "event", s, ("Z"+label), 0., 1., true, 0.71, true); //significance vs efficiency
	  delete c;
	}
	else if(selection_=="etau") {
	  auto c = dataplotter_->print_cdf("mva2", "event", s, ("H"+label), 0., 1.7);
	  delete c;
	  c = dataplotter_->print_cdf("mva3", "event", s, ("Z"+label), 0., 1.7);
	  delete c;
	  c = dataplotter_->print_significance("mva2", "event", s, ("H"+label), -1., 1.);
	  delete c;
	  c = dataplotter_->print_significance("mva3", "event", s, ("Z"+label), -1., 1.);
	  delete c;
	  c = dataplotter_->print_significance("mva2", "event", s, ("H"+label), 0., 1., true, -1., true);
	  delete c;
	  c = dataplotter_->print_significance("mva3", "event", s, ("Z"+label), 0., 1., true, -1., true);
	  delete c;
	}
	else if(selection_=="emu") {
	  auto c = dataplotter_->print_cdf("mva4", "event", s, ("H"+label), 0., 1.7);
	  delete c;
	  c = dataplotter_->print_cdf("mva5", "event", s, ("Z"+label), 0., 1.7);
	  delete c;
	  c = dataplotter_->print_significance("mva4", "event", s, ("H"+label), -1., 1.);
	  delete c;
	  c = dataplotter_->print_significance("mva5", "event", s, ("Z"+label), -1., 1., true, 0.44);
	  delete c;
	  c = dataplotter_->print_significance("mva4", "event", s, ("H"+label), 0., 1., true, -1., true);
	  delete c;
	  c = dataplotter_->print_significance("mva5", "event", s, ("Z"+label), 0., 1., true, 0.44, true);
	  delete c;
	}
      }
    }
  } //end significance loop
  
  dataplotter_->logY_ = 0;
  int status = (stacks) ? dataplotter_->print_stacks(hnames, htypes, sets, xmaxs, xmins, rebins, signal_scales, base_rebins) :
    dataplotter_->print_hists(hnames, htypes, sets, xmaxs, xmins, rebins, signal_scales, base_rebins);

  if(status) return status;
  dataplotter_->logY_ = 1;
  status = (stacks) ? dataplotter_->print_stacks(hnames, htypes, sets, xmaxs, xmins, rebins, signal_scales, base_rebins) :
    dataplotter_->print_hists(hnames, htypes, sets, xmaxs, xmins, rebins, signal_scales, base_rebins);
  dataplotter_->logY_ = 0;
  return status;
}

Int_t print_standard_canvases(vector<int> sets, vector<double> signal_scales = {},
			      vector<int> base_rebins = {}, bool stacks = true, TString name = "") {
  if(!dataplotter_) return -1;
  vector<TString> hnames;
  vector<TString> htypes;
  vector<int>     rebins;
  vector<double>  xmins;
  vector<double>  xmaxs;
  
  hnames.push_back("onept");          htypes.push_back("lep");   rebins.push_back(2); xmins.push_back(15.);  xmaxs.push_back(150.);
  hnames.push_back("twopt");          htypes.push_back("lep");   rebins.push_back(2); xmins.push_back(15.);  xmaxs.push_back(150.);
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

  bool leptonic_tau = (selection_.Contains("_")); //mutau_l, etau_l
  dataplotter_ = new DataPlotter();
  dataplotter_->selection_ = selection_;
  dataplotter_->folder_ = folder_;
  if(selection_ == "mutau")
    dataplotter_->qcd_scale_ = 1.059;
  else if(selection_ == "etau")
    dataplotter_->qcd_scale_ = 1.157;
  else
    dataplotter_->qcd_scale_ = 1.;
  
  dataplotter_->doStatsLegend_ = doStatsLegend_;
  dataplotter_->signal_scale_ = 250.;
  dataplotter_->useOpenGL_ = (gROOT->IsBatch()) ? 0 : useOpenGL_;

  //dataset names
  TString names [50]; //for event histogram
  TString fnames[50]; //for getting file (usually the same)
  TString labels[50]; //for combining categories
  bool    signal[50]; //for drawing separately
  bool    isDY  [50]; 
  
  names[0]  = "zjets_m-50_amcatnlo"     ;fnames[0]  = "1_bltTree_zjets_m-50_amcatnlo"     ;labels[0]  = "Z->#tau#tau"   ; signal[0]  = false  ; isDY[0]  = true   ;
  names[1]  = "zjets_m-50_amcatnlo"     ;fnames[1]  = "2_bltTree_zjets_m-50_amcatnlo"     ;labels[1]  = "Z->ee/#mu#mu"  ; signal[1]  = false  ; isDY[1]  = true   ;
  names[2]  = "zjets_m-10to50_amcatnlo" ;fnames[2]  = "1_bltTree_zjets_m-10to50_amcatnlo" ;labels[2]  = "Z->#tau#tau"   ; signal[2]  = false  ; isDY[2]  = true   ;
  names[3]  = "zjets_m-10to50_amcatnlo" ;fnames[3]  = "2_bltTree_zjets_m-10to50_amcatnlo" ;labels[3]  = "Z->ee/#mu#mu"  ; signal[3]  = false  ; isDY[3]  = true   ;
  names[4]  = "ttbar_inclusive"         ;fnames[4]  = "bltTree_ttbar_inclusive"           ;labels[4]  = "Top"           ; signal[4]  = false  ; isDY[4]  = false  ;
  names[5]  = "t_tw"                    ;fnames[5]  = "bltTree_t_tw"                      ;labels[5]  = "Top"           ; signal[5]  = false  ; isDY[5]  = false  ;
  names[6]  = "tbar_tw"                 ;fnames[6]  = "bltTree_tbar_tw"                   ;labels[6]  = "Top"           ; signal[6]  = false  ; isDY[6]  = false  ;
  names[7]  = "zjets_m-50"              ;fnames[7]  = "bltTree_zjets_m-50"                ;labels[7]  = "Z+Jets"        ; signal[7]  = false  ; isDY[7]  = true   ;
  names[8]  = "zjets_m-10to50"          ;fnames[8]  = "bltTree_zjets_m-10to50"            ;labels[8]  = "Z+Jets"        ; signal[8]  = false  ; isDY[8]  = true   ;
  names[9]  = "z1jets_m-50"             ;fnames[9]  = "bltTree_z1jets_m-50"               ;labels[9]  = "Z+Jets"        ; signal[9]  = false  ; isDY[9]  = true   ;
  names[10] = "z1jets_m-10to50"         ;fnames[10] = "bltTree_z1jets_m-10to50"           ;labels[10] = "Z+Jets"        ; signal[10] = false  ; isDY[10] = true   ;
  names[11] = "z2jets_m-50"             ;fnames[11] = "bltTree_z2jets_m-50"               ;labels[11] = "Z+Jets"        ; signal[11] = false  ; isDY[11] = true   ;
  names[12] = "z2jets_m-10to50"         ;fnames[12] = "bltTree_z2jets_m-10to50"           ;labels[12] = "Z+Jets"        ; signal[12] = false  ; isDY[12] = true   ;
  names[13] = "z3jets_m-50"             ;fnames[13] = "bltTree_z3jets_m-50"               ;labels[13] = "Z+Jets"        ; signal[13] = false  ; isDY[13] = true   ;
  names[14] = "z3jets_m-10to50"         ;fnames[14] = "bltTree_z3jets_m-10to50"           ;labels[14] = "Z+Jets"        ; signal[14] = false  ; isDY[14] = true   ;
  names[15] = "z4jets_m-50"             ;fnames[15] = "bltTree_z4jets_m-50"               ;labels[15] = "Z+Jets"        ; signal[15] = false  ; isDY[15] = true   ;
  names[16] = "z4jets_m-10to50"         ;fnames[16] = "bltTree_z4jets_m-10to50"           ;labels[16] = "Z+Jets"        ; signal[16] = false  ; isDY[16] = true   ;
  names[17] = "w1jets"                  ;fnames[17] = "bltTree_w1jets"                    ;labels[17] = "W+Jets"        ; signal[17] = false  ; isDY[17] = false  ;
  names[18] = "w2jets"                  ;fnames[18] = "bltTree_w2jets"                    ;labels[18] = "W+Jets"        ; signal[18] = false  ; isDY[18] = false  ;
  names[19] = "w3jets"                  ;fnames[19] = "bltTree_w3jets"                    ;labels[19] = "W+Jets"        ; signal[19] = false  ; isDY[19] = false  ;
  names[20] = "w4jets"                  ;fnames[20] = "bltTree_w4jets"                    ;labels[20] = "W+Jets"        ; signal[20] = false  ; isDY[20] = false  ;
  names[21] = "wjets"                   ;fnames[21] = "bltTree_wjets"                     ;labels[21] = "W+Jets"        ; signal[21] = false  ; isDY[21] = false  ;
  names[22] = "wjets_ext1"              ;fnames[22] = "bltTree_wjets_ext1"                ;labels[22] = "W+Jets"        ; signal[22] = false  ; isDY[22] = false  ;
  names[23] = "wjets_ext2"              ;fnames[23] = "bltTree_wjets_ext2"                ;labels[23] = "W+Jets"        ; signal[23] = false  ; isDY[23] = false  ;
  names[24] = "wjets_total"             ;fnames[24] = "bltTree_wjets_total"               ;labels[24] = "W+Jets"        ; signal[24] = false  ; isDY[24] = false  ;
  names[25] = "ww"                      ;fnames[25] = "bltTree_ww"                        ;labels[25] = "Diboson"       ; signal[25] = false  ; isDY[25] = false  ;
  names[26] = "wz_2l2q"                 ;fnames[26] = "bltTree_wz_2l2q"                   ;labels[26] = "Diboson"       ; signal[26] = false  ; isDY[26] = false  ;
  names[27] = "wz_3lnu"                 ;fnames[27] = "bltTree_wz_3lnu"                   ;labels[27] = "Diboson"       ; signal[27] = false  ; isDY[27] = false  ;
  names[28] = "zz_2l2nu"                ;fnames[28] = "bltTree_zz_2l2nu"                  ;labels[28] = "Diboson"       ; signal[28] = false  ; isDY[28] = false  ;
  names[29] = "zz_2l2q"                 ;fnames[29] = "bltTree_zz_2l2q"                   ;labels[29] = "Diboson"       ; signal[29] = false  ; isDY[29] = false  ;
  names[30] = "zz_4l"                   ;fnames[30] = "bltTree_zz_4l"                     ;labels[30] = "Diboson"       ; signal[30] = false  ; isDY[30] = false  ;
  names[31] = "hzg_gluglu"              ;fnames[31] = "bltTree_hzg_gluglu"                ;labels[31] = "H->Zg"         ; signal[31] = false  ; isDY[31] = false  ;
  names[32] = "hzg_tth"                 ;fnames[32] = "bltTree_hzg_tth"                   ;labels[32] = "H->Zg"         ; signal[32] = false  ; isDY[32] = false  ;
  names[33] = "hzg_vbf"                 ;fnames[33] = "bltTree_hzg_vbf"                   ;labels[33] = "H->Zg"         ; signal[33] = false  ; isDY[33] = false  ;
  names[34] = "hzg_wminus"              ;fnames[34] = "bltTree_hzg_wminus"                ;labels[34] = "H->Zg"         ; signal[34] = false  ; isDY[34] = false  ;
  names[35] = "hzg_wplus"               ;fnames[35] = "bltTree_hzg_wplus"                 ;labels[35] = "H->Zg"         ; signal[35] = false  ; isDY[35] = false  ;
  names[36] = "hzg_zh"                  ;fnames[36] = "bltTree_hzg_zh"                    ;labels[36] = "H->Zg"         ; signal[36] = false  ; isDY[36] = false  ;
  names[37] = "htautau_gluglu"          ;fnames[37] = "bltTree_htautau_gluglu"            ;labels[37] = "H->#tau#tau"   ; signal[37] = false  ; isDY[37] = false  ;
  names[38] = "zetau"                   ;fnames[38] = "bltTree_zetau"                     ;labels[38] = "Z->e#tau"      ; signal[38] = true   ; isDY[38] = false  ;
  names[39] = "zmutau"                  ;fnames[39] = "bltTree_zmutau"                    ;labels[39] = "Z->#mu#tau"    ; signal[39] = true   ; isDY[39] = false  ;
  names[40] = "zemu"                    ;fnames[40] = "bltTree_zemu"                      ;labels[40] = "Z->e#mu"       ; signal[40] = true   ; isDY[40] = false  ;
  names[41] = "hetau"                   ;fnames[41] = "bltTree_hetau"                     ;labels[41] = "H->e#tau"      ; signal[41] = true   ; isDY[41] = false  ;
  names[42] = "hmutau"                  ;fnames[42] = "bltTree_hmutau"                    ;labels[42] = "H->#mu#tau"    ; signal[42] = true   ; isDY[42] = false  ;
  names[43] = "hemu"                    ;fnames[43] = "bltTree_hemu"                      ;labels[43] = "H->e#mu"       ; signal[43] = true   ; isDY[43] = false  ;

  int process[50];
  for(int i = 0; i < sizeof(process)/sizeof(*process); ++i)
    process[i]=0;

  //flags to include the set or not
  process[0]  = 1; //"zjets_m-50_amcatnlo"     
  process[1]  = 1; //"zjets_m-50_amcatnlo"     
  process[2]  = 1; //"zjets_m-10to50_amcatnlo" 
  process[3]  = 1; //"zjets_m-10to50_amcatnlo" 
  process[4]  = 1; //"ttbar_inclusive"         
  process[5]  = 1; //"t_tw"                    
  process[6]  = 1; //"tbar_tw"                 
  process[7]  = 0; //"zjets_m-50"              
  process[8]  = 0; //"zjets_m-10to50"          
  process[9]  = 0; //"z1jets_m-50"             
  process[10] = 0; //"z1jets_m-10to50"         
  process[11] = 0; //"z2jets_m-50"             
  process[12] = 0; //"z2jets_m-10to50"         
  process[13] = 0; //"z3jets_m-50"             
  process[14] = 0; //"z3jets_m-10to50"         
  process[15] = 0; //"z4jets_m-50"             
  process[16] = 0; //"z4jets_m-10to50"         
  process[17] = 1; //"w1jets"                  
  process[18] = 1; //"w2jets"                  
  process[19] = 1; //"w3jets"                  
  process[20] = 1; //"w4jets"                  
  process[21] = 0; //"wjets"                  
  process[22] = 0; //"wjets_ext1"                  
  process[23] = 0; //"wjets_ext2"                  
  process[24] = 0; //"wjets_total"                  
  process[25] = 1; //"ww"                      
  process[26] = 1; //"wz_2l2q"                 
  process[27] = 1; //"wz_3lnu"                 
  process[28] = 1; //"zz_2l2nu"                
  process[29] = 1; //"zz_2l2q"                 
  process[30] = 1; //"zz_4l"                   
  process[31] = 0; //"hzg_gluglu"              
  process[32] = 0; //"hzg_tth"                 
  process[33] = 0; //"hzg_vbf"                 
  process[34] = 0; //"hzg_wminus"              
  process[35] = 0; //"hzg_wplus"               
  process[36] = 0; //"hzg_zh"                  
  process[37] = 1; //"htautau_gluglu"                  
  process[38] = (selection_.Contains("etau" )) ? 1 : 0; //"zetau"
  process[39] = (selection_.Contains("mutau")) ? 1 : 0; //"zmutau"
  process[40] = (selection_ == "emu"  ) ? 1 : 0; //"zemu"
  process[41] = (selection_.Contains("etau" )) ? 1 : 0; //"hetau"
  process[42] = (selection_.Contains("mutau")) ? 1 : 0; //"hmutau"
  process[43] = (selection_ == "emu"  ) ? 1 : 0; //"hemu"

  int nWJetSamples = 0; //if averaging W+Jets samples
  if(process[17] || process[18] || process[19] || process[20]) //madgraph jet binned samples
    ++nWJetSamples;
  if(process[21]) //amcnlo sample
    ++nWJetSamples;
  if(process[22]) //amcnlo sample ext1
    ++nWJetSamples;
  if(process[23]) //amcnlo sample ext2
    ++nWJetSamples;
  if(nWJetSamples == 0) nWJetSamples = 1; // avoid divide by 0
  
  Double_t xsec[50];
  //Taken from https://twiki.cern.ch/twiki/bin/viewauth/CMS/SummaryTable1G25ns     
  xsec[0]  =  6225.42; //6803.2;   //6225.42  ; //5765.4;    //"zjets_m-50_amcatnlo"     
  xsec[1]  =  6225.42; //6803.2;   //6225.42  ; //5765.4;    //"zjets_m-50_amcatnlo"     
  xsec[2]  =  18610. ; //21959.8; //18610.  ;         //"zjets_m-10to50_amcatnlo" 
  xsec[3]  =  18610. ; //21959.8; //18610.  ;         //"zjets_m-10to50_amcatnlo" 
  xsec[4]  =  831.76;                 //"ttbar_inclusive"         
  xsec[5]  =  35.85;	               //"t_tw"                    
  xsec[6]  =  35.85;	               //"tbar_tw"                 
  xsec[7]  =  6803.2;	               //"zjets_m-50"              
  xsec[8]  =  21959.8;	               //"zjets_m-10to50"          
  xsec[9]  =  1198.9;	               //"z1jets_m-50"             
  xsec[10] =  855.5;		       //"z1jets_m-10to50"         
  xsec[11] =  390.6;		       //"z2jets_m-50"             
  xsec[12] =  466.1;		       //"z2jets_m-10to50"         
  xsec[13] =  113.3;		       //"z3jets_m-50"             
  xsec[14] =  114.5;		       //"z3jets_m-10to50"         
  xsec[15] =  60.2;		       //"z4jets_m-50"             
  xsec[16] =  36.4;		       //"z4jets_m-10to50"         
  xsec[17] =  11486.53 / nWJetSamples; //9493.;		       //"w1jets"                  
  xsec[18] =  3775.2   / nWJetSamples; //3120.;		       //"w2jets"                  
  xsec[19] =  1139.82  / nWJetSamples; //942.3;		       //"w3jets"                  
  xsec[20] =  655.82   / nWJetSamples; //524.1;		       //"w4jets"                  
  xsec[21] =  61526.7  / nWJetSamples; 		       //"wjets"     //https://cms-gen-dev.cern.ch/xsdb/?searchQuery=DAS=WJetsToLNu_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8      
  xsec[22] =  61526.7  / nWJetSamples; 		       //"wjets_ext1"   // 61526.7 from SummaryTable1G25ns#W_jets               
  xsec[23] =  61526.7  / nWJetSamples; 		       //"wjets_ext2"                  
  xsec[24] =  61526.7; 		       //"wjets_total"                  
  xsec[25] =  12.178;	               //"ww"                      
  xsec[26] =  5.595;		       //"wz_2l2q"                 
  xsec[27] =  4.42965;	               //"wz_3lnu"                 
  xsec[28] =  0.564;		       //"zz_2l2nu"                
  xsec[29] =  3.22;		       //"zz_2l2q"                 
  xsec[30] =  1.212;		       //"zz_4l"
  //Higgs branching ratios: https://twiki.cern.ch/twiki/bin/view/LHCPhysics/CERNYellowReportPageBR2014
  //Higgs production xsecs: https://twiki.cern.ch/twiki/bin/view/LHCPhysics/CERNHLHE2019
  //Z decay to leptons fraction: http://pdg.lbl.gov/2012/listings/rpp2012-list-z-boson.pdf
  // --> Br(Z->ll) * Br(h->Zg) * xsec(X -> h)
  xsec[31] =    3.*3.3658/100.*1.54e-3* 48.61;	       //"hzg_gluglu"              
  xsec[32] =    3.*3.3658/100.*1.54e-3* 0.5071;	       //"hzg_tth"                 
  xsec[33] =    3.*3.3658/100.*1.54e-3* 3.766;	       //"hzg_vbf"                 
  xsec[34] =    3.*3.3658/100.*1.54e-3* 1.358/2.;	       //"hzg_wminus"              
  xsec[35] =    3.*3.3658/100.*1.54e-3* 1.358/2.;	       //"hzg_wplus"               
  xsec[36] =    3.*3.3658/100.*1.54e-3* 0.880;	       //"hzg_zh"                  
  xsec[37] =                   6.32e-2* 48.61;	       //"htautau_gluglu"                  
  xsec[38] =    ((6225.42+18610.)/(3.*3.3658e-2))*9.8e-6*161497./(2.e3*498); //zetau  z->ll / br(ll) * br(etau, CL=95) *N(accepted)/N(Gen) http://pdg.lbl.gov/2018/listings/rpp2018-list-z-boson.pdf
  xsec[39] =    ((6225.42+18610.)/(3.*3.3658e-2))*1.2e-5*152959./(2.e3*497); //zmutau z->ll / br(ll) * br(mutau, CL=95)*N(accepted)/N(Gen) http://pdg.lbl.gov/2018/listings/rpp2018-list-z-boson.pdf
  xsec[40] =    ((6225.42+18610.)/(3.*3.3658e-2))*7.3e-7*186670./(2.e3*596); //zemu   z->ll / br(ll) * br(emu, CL=95)  *N(accepted)/N(Gen) http://pdg.lbl.gov/2018/listings/rpp2018-list-z-boson.pdf
  xsec[41] = (48.61+3.766+0.5071+1.358+0.880)*6.1e-3*418794./(487.*1e3); //hetau  xsec(higgs,glu+vbf)*br(etau, CL=95) *N(accepted)/N(Gen) http://pdg.lbl.gov/2019/listings/rpp2019-list-higgs-boson.pdf
  xsec[42] = (48.61+3.766+0.5071+1.358+0.880)*2.5e-3*388243./(453.*1e3); //hmutau xsec(higgs,glu+vbf)*br(mutau, CL=95)*N(accepted)/N(Gen) http://pdg.lbl.gov/2019/listings/rpp2019-list-higgs-boson.pdf
  xsec[43] = (48.61+3.766+0.5071+1.358+0.880)*3.5e-4*34429./(88.*500); //hemu   xsec(higgs,glu+vbf)*br(emu, CL=95)  *N(accepted)/N(Gen) http://pdg.lbl.gov/2019/listings/rpp2019-list-higgs-boson.pdf


  vector<TString> files;
  TString selection_dir = (leptonic_tau) ? "emu" : selection_;
  for(int i = 0; i < sizeof(process)/sizeof(*process); ++i) {
    if(process[i]) {
      files.push_back(Form("%s/ztautau_%s_%s.hist",hist_dir_.Data(),selection_dir.Data(),fnames[i].Data()));
    } else
      files.push_back(Form(""));
  }

  const char* dNamesMu[] = {
    "muon_2016B",
    "muon_2016C",
    "muon_2016D",
    "muon_2016E",
    "muon_2016F",
    "muon_2016G",
    "muon_2016H",
  };
  
  const char* dNamesE[] = {
    "electron_2016B",
    "electron_2016C",
    "electron_2016D",
    "electron_2016E",
    "electron_2016F",
    "electron_2016G",
    "electron_2016H"
  };
  
  // const char* dFiles[] = {Form("tree_%s.hist",dNames[0])};
  vector<TString> dFiles;
  vector<TString> dNames;
  for(int i = 0; i < sizeof(dNamesMu)/sizeof(*dNamesMu); ++i) {
    if(selection_ == "etau" ) {
      dNames.push_back(dNamesE[i]);
      dFiles.push_back(Form("%s/ztautau_%s_bltTree_%s.hist",hist_dir_.Data(),selection_.Data(),dNamesE[i]));
    } else if(selection_ == "mutau" || selection_ == "mumu") {
      dNames.push_back(dNamesMu[i]);
      dFiles.push_back(Form("%s/ztautau_%s_bltTree_%s.hist",hist_dir_.Data(),selection_.Data(),dNamesMu[i]));
    } else if(selection_ == "emu" || leptonic_tau) {
      dNames.push_back(dNamesMu[i]);
      dFiles.push_back(Form("%s/ztautau_%s_bltTree_%s.hist",hist_dir_.Data(),selection_dir.Data(),dNamesMu[i]));
      dNames.push_back(dNamesE[i]);
      dFiles.push_back(Form("%s/ztautau_%s_bltTree_%s.hist",hist_dir_.Data(),selection_dir.Data(),dNamesE[i]));
    }
    
  }
  
  const Double_t dLum[] = {
    5.75e3  ,
    2.573e3  ,
    4.242e3  ,
    4.025e3  ,
    3.105e3  ,
    7.576e3  ,
    8.651e3  
                          };//, (36.4e3-5.3e3)}; //taken from CMS AN-16-458

  double lum    = accumulate(begin(dLum), end(dLum), 0, plus<double>());; //pb^-1
  dataplotter_->set_luminosity(lum);

  for(int i = 0; i < files.size(); ++i) {
    if(files[i] != "") dataplotter_->add_dataset(files[i], names[i], labels[i], false, xsec[i], signal[i]);
  }
  
  for(int i = 0; i < dFiles.size(); ++i)
    if(dFiles[i] != "") dataplotter_->add_dataset(dFiles[i], dNames[i], "Data", true, 1., false);

  return dataplotter_->init_files();
}


Int_t print_standard_selections(TString histDir = "", TString figureDir = "") {
  if(histDir != "") hist_dir_ = histDir;
  if(figureDir != "") folder_ = figureDir;
  TStopwatch* timer = new TStopwatch();  
  Int_t status = 0;
  selection_ = "mutau";
  status += init_dataplotter();
  status += print_standard_plots({7,8,9,10,13,14}, {250., 250., 50., 50., 250., 250.});
  selection_ = "etau";
  status += init_dataplotter();
  status += print_standard_plots({27,28,29,30,33,34}, {250., 250., 50., 50., 250., 250.});
  selection_ = "emu";
  status += init_dataplotter();
  status += print_standard_plots({47,48,49,50,53,54,55,56}, {250., 250., 5., 5., 250., 250., 5., 5.}, {1,1,2,2,1,1,2,2});
  selection_ = "mutau_e";
  status += init_dataplotter();
  status += print_standard_plots({47,48}, {250., 250.});
  selection_ = "etau_mu";
  status += init_dataplotter();
  status += print_standard_plots({47,48}, {250., 250.});
  selection_ = "mumu";
  status += init_dataplotter();
  status += print_standard_plots({67,68, 80}, {1., 1., 1.});

  Double_t cpuTime = timer->CpuTime();
  Double_t realTime = timer->RealTime();
  printf("Processing time: %7.2fs CPU time %7.2fs Wall time\n",cpuTime,realTime);
  if(realTime > 600. ) printf("Processing time: %7.2fmin CPU time %7.2fmin Wall time\n",cpuTime/60.,realTime/60.);
  return status;
}

Int_t print_standard_canvas_selections(TString name = "", TString histDir = "", TString figureDir = "") {
  if(histDir != "") hist_dir = histDir;
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
