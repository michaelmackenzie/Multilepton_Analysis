//Script to determine the composition of j->tau regions
#include "histograms/datacards.C"

//use the dataplotter to manage normalizations and initializations
DataPlotter* dataplotter_ = 0;
int     verbose_     = 0;
TString name_           ; //figures directory
int     setqcd_         ; //set to estimate QCD background in
TString selection_      ;
bool    preserve_clip_int_ = 1; //preserve the integral of histograms when clipping negative values

//-----------------------------------------------------------------------------------------------------------------------------------------
//ensure reasonable bin values
void make_safe(TH1* h) {
  const double integral = h->Integral(0,h->GetNbinsX()+1);
  for(int ibin = 0; ibin <= h->GetNbinsX()+1; ++ibin) {
    const double binc(h->GetBinContent(ibin));
    const double bine(h->GetBinError  (ibin));
    if(!std::isfinite(binc) || !std::isfinite(bine) || binc < 0.) {
      h->SetBinContent(ibin, 0.);
      h->SetBinError  (ibin, 0.);
    }
  }
  if(preserve_clip_int_ && integral > 0.) h->Scale(integral/h->Integral(0,h->GetNbinsX()+1));
}

//-----------------------------------------------------------------------------------------------------------------------------------------
//ensure reasonable bin values (2D histograms)
void make_safe(TH2* h) {
  const double integral = h->Integral(0,h->GetNbinsX()+1, 0, h->GetNbinsY()+1);
  for(int xbin = 0; xbin <= h->GetNbinsX()+1; ++xbin) {
    for(int ybin = 0; ybin <= h->GetNbinsY()+1; ++ybin) {
      const double binc(h->GetBinContent(xbin,ybin));
      const double bine(h->GetBinError  (xbin,ybin));
      if(!std::isfinite(binc) || !std::isfinite(bine) || binc < 0.) {
        h->SetBinContent(xbin, ybin, 0.);
        h->SetBinError  (xbin, ybin, 0.);
      }
    }
  }
  if(preserve_clip_int_ && integral > 0.) h->Scale(integral/h->Integral(0,h->GetNbinsX()+1, 0, h->GetNbinsY()+1));
}

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
// Plot 1D slices of a 2D distribution
void slice_2d(std::vector<TH2*> histograms, PlottingCard_t card, const bool printHists = false, const bool debug = false) {
  TString hist = card.hist_;
  TString type = card.type_;
  const int set = card.set_;

  //check the histograms are defined
  if(histograms.size() == 0 || !histograms[0]) return;

  if(debug) {
    cout << __func__ << ": Plotting hist " << hist.Data() << endl;
    for(unsigned ihist = 0; ihist < histograms.size(); ++ihist) {
      cout << " Label " << histograms[ihist]->GetTitle() << " has integral = " << histograms[ihist]->Integral() << endl;
    }
  }

  //ensure each histogram is well defined
  for(auto h : histograms) make_safe(h);

  //construct the total histogram
  TH2* total_2d = (TH2*) histograms[0]->Clone(Form("%s_total", histograms[0]->GetName()));
  for(unsigned ihist = 1; ihist < histograms.size(); ++ihist) total_2d->Add(histograms[ihist]);

  //construct the 2D compositions
  std::vector<TH2*> compositions;
  for(unsigned ihist = 0; ihist < histograms.size(); ++ihist) {
    if(debug) cout << histograms[ihist]->GetName() << ": " << histograms[ihist]->GetTitle() << endl;
    TH2* comp = (TH2*) histograms[ihist]->Clone(Form("hComposition_%s_%s", hist.Data(), histograms[ihist]->GetTitle()));
    comp->Divide(total_2d);
    compositions.push_back(comp);
    if(printHists) comp->Write(); //save the histograms to output if requested
  }

  //loop through the x-axis bins
  const int nxbins = histograms[0]->GetNbinsX();
  for(int ixbin = 1; ixbin <= nxbins; ++ixbin) {
    if(debug) cout << " Performing slice " << ixbin << endl;

    //create a stack of the 1D projections
    THStack* stack = new THStack(Form("stack_%s_%i", hist.Data(), ixbin), Form("stack_%s_%i", hist.Data(), ixbin));
    for(unsigned ihist = 0; ihist < histograms.size(); ++ihist) {
      TH1* h = histograms[ihist]->ProjectionY(Form("%s_%i_%i", hist.Data(), ixbin, ihist), ixbin, ixbin);
      h->SetTitle(histograms[ihist]->GetTitle());
      h->SetFillColor(histograms[ihist]->GetFillColor());
      h->SetFillStyle(histograms[ihist]->GetFillStyle());
      h->SetLineColor(histograms[ihist]->GetLineColor());
      if(debug) {
        cout << "  Adding label " << h->GetTitle() << " with integral " << h->Integral() << endl;
      }
      // make_safe(h); //ensure no negative bins
      stack->Add(h);
    }

    //create a stack of the compositions
    THStack* comp  = new THStack(Form("comp_%s_%i" , hist.Data(), ixbin), Form("comp_%s_%i" , hist.Data(), ixbin));
    for(unsigned ihist = 0; ihist < histograms.size(); ++ihist) {
      TH1* h = compositions[ihist]->ProjectionY(Form("%s_%i_%i_comp", hist.Data(), ixbin, ihist), ixbin, ixbin);
      h->SetTitle(histograms[ihist]->GetTitle());
      h->SetFillColor(histograms[ihist]->GetFillColor());
      h->SetFillStyle(histograms[ihist]->GetFillStyle());
      h->SetLineColor(histograms[ihist]->GetLineColor());
      if(debug) {
        cout << "  Adding label " << h->GetTitle() << " composition with integral " << h->Integral() << endl;
      }
      // make_safe(h); //ensure no negative bins
      comp->Add(h);
    }

    TString xtitle = card.hist_;
    xtitle.ReplaceAll("jettau", "");
    xtitle.ReplaceAll("lepmvs", "");
    xtitle.ReplaceAll("_", "");
    xtitle.ReplaceAll("1", ""); xtitle.ReplaceAll("0", ""); xtitle.ReplaceAll("2", ""); xtitle.ReplaceAll("3", ""); xtitle.ReplaceAll("4", "");
    bool addone = xtitle.Contains("one");
    bool addtwo = xtitle.Contains("two");
    xtitle.ReplaceAll("one","");
    xtitle.ReplaceAll("two","");
    xtitle.ReplaceAll("delta", "#Delta");
    xtitle.ReplaceAll("comp", "");
    xtitle.ReplaceAll("eta", "#eta"); xtitle.ReplaceAll("phi", "#phi"); xtitle.ReplaceAll("pt", "p_{T}");
    if(xtitle == "mt") {
      xtitle = "M_{T}(MET,";
      if     (addone) xtitle = xtitle + "l_{1})";
      else if(addtwo) xtitle = xtitle + "l_{2})";
      else            xtitle = xtitle +    "ll)";
    } else {
      if(addone) xtitle = xtitle + "^{1}";
      if(addtwo) xtitle = xtitle + "^{2}";
    }
    xtitle.ReplaceAll("taus", "#tau ");
    xtitle.ReplaceAll("met", "MET ");
    xtitle.ReplaceAll("dm", "Decay Mode");

    TCanvas* c = new TCanvas(Form("c_%s_%i", hist.Data(), ixbin), Form("c_%s_%i", hist.Data(), ixbin), 1000, 600);
    c->Divide(2,1);
    auto pad = c->cd(1);
    stack->SetTitle(xtitle.Data());
    stack->Draw("hist noclear");
    stack->SetMaximum(stack->GetMaximum()*3.);
    pad->SetLogy();
    TLegend* leg = new TLegend(0.7, 0.7, 0.9, 0.9);
    for(auto o : *stack->GetHists()) leg->AddEntry(o, o->GetTitle(), "F");
    leg->Draw();
    c->cd(2);
    comp->SetTitle(xtitle.Data());
    comp->Draw("hist noclear");
    comp->SetMaximum(1.05);
    comp->SetMinimum(0.);
    leg->Draw();
    c->Modified(); c->Update();
    if(card.xmin_ < card.xmax_ && comp->GetXaxis()) comp->GetXaxis()->SetRangeUser(card.xmin_, card.xmax_);
    c->Modified(); c->Update();


    c->Print(Form("%scomp_%s_%s_%i.png", name_.Data(), type.Data(), hist.Data(), ixbin));
    delete c;

    for(auto o : *(stack->GetHists())) delete o;
    for(auto o : *(comp ->GetHists())) delete o;
    delete comp;
    delete stack;
  }
}

//-----------------------------------------------------------------------------------------------------------------------------------------
void make_composition(PlottingCard_t card, const bool printHists = false, const bool debug = false, const bool is_2d = false) {
  TString hist = card.hist_;
  TString type = card.type_;
  const int set = card.set_;
  if(card.rebin_ > 1 && !is_2d) dataplotter_->rebinH_ = card.rebin_;
  else dataplotter_->rebinH_ = 1;

  //Get MC stack
  THStack* hstack = dataplotter_->get_stack(hist, type, set);
  if(!hstack || hstack->GetNhists() == 0) return;

  //Get QCD histogram
  TH1* hQCD = get_qcd(card);
  if(!hQCD) return;
  hQCD->SetLineColor(dataplotter_->qcd_color_);
  hQCD->SetFillColor(dataplotter_->qcd_color_);


  //Check if a 2D plot
  if(is_2d) {
    //create a list of histograms
    std::vector<TH2*> histograms;
    for(int ihist = 0; ihist < hstack->GetNhists(); ++ihist) {
      TH2* h = (TH2*) hstack->GetHists()->At(ihist);
      histograms.push_back(h);
    }
    hQCD->SetTitle("QCD");
    histograms.push_back((TH2*) hQCD);
    slice_2d(histograms, card, printHists, debug);
    return;
  }

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
  xtitle.ReplaceAll("lepmvs", "");
  xtitle.ReplaceAll("_", "");
  xtitle.ReplaceAll("1", ""); xtitle.ReplaceAll("0", ""); xtitle.ReplaceAll("2", ""); xtitle.ReplaceAll("3", ""); xtitle.ReplaceAll("4", "");
  bool addone = xtitle.Contains("one");
  bool addtwo = xtitle.Contains("two");
  xtitle.ReplaceAll("one","");
  xtitle.ReplaceAll("two","");
  xtitle.ReplaceAll("delta", "#Delta");
  xtitle.ReplaceAll("comp", "");
  xtitle.ReplaceAll("eta", "#eta"); xtitle.ReplaceAll("phi", "#phi"); xtitle.ReplaceAll("pt", "p_{T}");
  if(xtitle == "mt") {
    xtitle = "M_{T}(MET,";
    if     (addone) xtitle = xtitle + "l_{1})";
    else if(addtwo) xtitle = xtitle + "l_{2})";
    else            xtitle = xtitle +    "ll)";
  } else {
    if(addone) xtitle = xtitle + "^{1}";
    if(addtwo) xtitle = xtitle + "^{2}";
  }
  xtitle.ReplaceAll("taus", "#tau ");
  xtitle.ReplaceAll("met", "MET ");
  xtitle.ReplaceAll("dm", "Decay Mode");

  TLegend* leg = new TLegend(0.7, 0.7, 0.9, 0.9);
  leg->AddEntry(hQCD_orig, "QCD", "F");
  for(auto o : *hstack->GetHists()) leg->AddEntry(o, o->GetTitle(), "F");

  TCanvas* c = new TCanvas(Form("c_%s", hist.Data()), Form("c_%s", hist.Data()), 1000, 600);
  c->Divide(2,1);
  auto pad = c->cd(1);
  // hstack->Add(hQCD_orig);
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
Int_t initialize_plotter(vector<int> years) {
  if(dataplotter_) delete dataplotter_;
  dataplotter_ = new DataPlotter();
  dataplotter_->include_qcd_   = 0;
  dataplotter_->include_misid_ = 0;
  dataplotter_->verbose_       = max(0, verbose_ - 1);
  dataplotter_->qcd_scale_     = 1.;
  dataplotter_->embed_scale_   = embedScale_;
  dataplotter_->doStatsLegend_ = false;
  dataplotter_->clip_negative_ = true;
  years_ = years;
  useEmbed_ = 0;

  std::vector<dcard> cards;
  get_datacards(cards, selection_, true);

  CrossSections xs(useUL_, ZMode_); //cross section handler
  double lum(0.);
  for(int year : years) {
    lum += xs.GetLuminosity(year);
    dataplotter_->lums_[year] = xs.GetLuminosity(year); //store the luminosity for the year
  }

  dataplotter_->set_luminosity(lum);
  if(verbose_ > 0) cout << "--- Dataplotter luminosity = " << dataplotter_->lum_ << endl;

  int status(0);
  for(auto card : cards)
    status += dataplotter_->add_dataset(card);

  status += dataplotter_->init_files();
  return status;
}

//-----------------------------------------------------------------------------------------------------------------------------------------
//setup everything
Int_t init(TString selection = "mutau", vector<int> years = {2016}, TString path = "nanoaods_dev") {
  selection_ = selection;
  hist_dir_ = path;
  hist_tag_ = "jtt";
  //initialize the data files
  if(verbose_ > 0) std::cout << "Initializing the dataplotter" << std::endl;
  if(initialize_plotter(years)) {
    cout << "Dataplotter initialization failed!\n";
    return 1;
  }
  return 0;
}

//-----------------------------------------------------------------------------------------------------------------------------------------
//Generate the plots and scale factors
Int_t composition(TString selection = "mutau", int setmc = 2042, int setqcd = 3035, vector<int> years = {2016}, TString path = "nanoaods_jtt") {
  //////////////////////
  // Initialize files //
  //////////////////////
  if(init(selection, years, path)) return 1;

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
  TString year_s = Form("%i", years[0]);
  for(int i = 1; i < years.size(); ++i) year_s += Form("_%i", years[i]);
  name_ = "figures/fake_tau_comp_";
  name_ += selection + "_";
  name_ += year_s;
  name_ += "_";
  name_ += setmc;
  name_ += "_";
  name_ += setqcd;
  name_ += "/";
  //ensure directories exist
  gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s", name_.Data(), name_.Data()));
  gSystem->Exec("[ ! -d rootfiles ] && mkdir rootfiles");

  //print canvases
  TFile* fOut = new TFile(Form("rootfiles/jet_to_tau_comp_%s_%i_%i_%s.root", selection.Data(), setmc, setqcd, year_s.Data()), "RECREATE");
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
  make_composition(PlottingCard_t("jettaulepmcomp"      , "lep", setmcAbs, 1, 1.,-1.), true);
  make_composition(PlottingCard_t("mtone", "event", setmcAbs, 5, 0., 150.), false);
  make_composition(PlottingCard_t("mttwo", "event", setmcAbs, 5, 0., 150.), false);
  make_composition(PlottingCard_t("mtlep", "event", setmcAbs, 5, 0., 150.), false);
  make_composition(PlottingCard_t("lepm" , "event", setmcAbs, 5,40., 170.), false);
  make_composition(PlottingCard_t("taudecaymode", "event", setmcAbs, 0, 1., -1.), false);
  make_composition(PlottingCard_t("njets20" , "event", setmcAbs, 0,0, 5), false);

  //Make 2D compositions
  make_composition(PlottingCard_t("jettaulepmvsmtonecomp", "lep", setmcAbs, 1, 1.,-1.), true, false, true);

  //Decay mode dependent compositions
  make_composition(PlottingCard_t("jettauonemetdeltaphi_0", "lep", setmcAbs, 1, 0., 4.), false);
  make_composition(PlottingCard_t("jettauonemetdeltaphi_1", "lep", setmcAbs, 1, 0., 4.), false);
  make_composition(PlottingCard_t("jettauonemetdeltaphi_2", "lep", setmcAbs, 1, 0., 4.), false);
  make_composition(PlottingCard_t("jettauonemetdeltaphi_3", "lep", setmcAbs, 1, 0., 4.), false);

  fOut->Close();
  return 0;
}
