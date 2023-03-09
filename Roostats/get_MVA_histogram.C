//Script to retrieve the background and signal MVA histograms
#include "histograms/datacards.C"

#include "mva_defaults.C"

int overall_rebin_ = 0;
TH1* hbkg_;
vector<TH1*> hsigs_;
int  test_sys_        = -1; //set to systematic number if debugging/inspecting it
bool blind_data_      = true; //set data bins > MVA score level to 0
bool ignore_sys_      = false; //don't get systematics
bool get_scale_sys_   = true; //get normalization uncertainty histograms
bool skip_shape_sys_  = false; //skip shape systematic retrieval
int  use_dev_mva_     = 0; //1: use the extra MVA hist for development, mvaX_1; 2: use the CDF transformed hist, mvaX_2
bool do_same_flavor_  = false; //retrieve Z->ll control region data
bool separate_years_  = true; //retrieve and store histograms by year
bool use_lep_tau_set_ = true; //use kETauMu/kMuTauE offsets instead of kEMu
bool print_sys_plots_ = true; //print systematic figures

//plotting parameters
double xmin_ = -1.;
double xmax_ =  1.;
bool   logy_ = false;

CLFV::DataPlotter* dataplotter_ = nullptr;

//initialize the datasets using a DataPlotter
Int_t initialize_plotter(TString selection, TString base) {
  if(dataplotter_) delete dataplotter_;
  dataplotter_ = new DataPlotter();

  dataplotter_->include_misid_ = selection == "mutau" || selection == "etau";
  dataplotter_->include_qcd_   = !dataplotter_->include_misid_ && selection != "mumu" && selection != "ee";
  dataplotter_->verbose_       = max(0, verbose_ - 1);
  dataplotter_->debug_         = max(0, verbose_ - 1);
  dataplotter_->qcd_scale_     = 1.;
  dataplotter_->embed_scale_   = embedScale_;
  dataplotter_->selection_     = selection;
  dataplotter_->folder_        = "";
  dataplotter_->include_empty_ = 1; //ensure even an empty histogram is included in the stacks
  dataplotter_->clip_negative_ = 1; //ensure no negative process rate bins

  if(selection == "emu")
    {dataplotter_->signal_scale_ = 100.; dataplotter_->signal_scales_["H->e#mu"] = 400.;}
  else if(selection.Contains("_"))
    {dataplotter_->signal_scale_ = 250.; dataplotter_->signal_scales_["H->#mu#tau"] = 300.; dataplotter_->signal_scales_["H->e#tau"] = 300.;}
  else if(selection.Contains("tau"))
    {dataplotter_->signal_scale_ = 150.; dataplotter_->signal_scales_["H->#mu#tau"] = 250.; dataplotter_->signal_scales_["H->e#tau"] = 250.;}
  else if(selection == "ee" || selection == "mumu")
    dataplotter_->signal_scale_ = 2.e4;

  //ensure the years are sorted
  std::sort(years_.begin(),years_.end());
  dataplotter_->years_ = years_;

  hist_tag_  = "clfv";
  hist_dir_  = base;

  std::vector<dcard> cards;
  get_datacards(cards, selection, 2); //get datacards with signal, but without scaling for visibility at x-sec level

  CrossSections xs(useUL_, ZMode_); //cross section handler

  //Calculate luminosity from year(s)
  double lum = 0.;
  for(int year : years_) {
    const double currLum = xs.GetLuminosity(year); //pb^-1
    dataplotter_->lums_[year] = currLum; //store the luminosity for the year
    lum += currLum; //add to the total luminosity
  }
  dataplotter_->set_luminosity(lum);

  int status(0);
  for(auto card : cards)
    status += dataplotter_->add_dataset(card);

  status += dataplotter_->init_files();
  return status;
}

int get_same_flavor_systematics(int set, TString hist, TH1* hdata, TFile* f) {
  int status(0);
  f->cd();
  dataplotter_->rebinH_ = 1;
  CLFV::Systematics systematics;

  //Loop through each systematic, creating PDFs and example figures
  for(int isys = (test_sys_ >= 0 ? test_sys_ : 0); isys < (test_sys_ >= 0 ? test_sys_ + 1 : kMaxSystematics); ++isys) {
    if(systematics.GetName(isys) == "") continue; //only retrieve defined systematics

    cout << "Getting same flavor systematic " << isys << " for set " << set << endl;
    //Get background for the systematic
    THStack* hstack = dataplotter_->get_stack(Form("%s_%i", hist.Data(), isys), "systematic", set);
    if(!hstack) {++status; continue;}
    if(!hstack->GetStack()) {++status; continue;}
    vector<TH1*> signals = dataplotter_->get_signal(Form("%s_%i", hist.Data(), isys), "systematic", set);
    if(signals.size() == 0) {++status; continue;}

    hstack->SetName(Form("hstack_sys_%i", isys));
    TH1* hbkg = (TH1*) hstack->GetStack()->Last();
    hbkg->SetName(Form("hbkg_sys_%i", isys));
    hbkg->Write();
    hstack->Write();

    for(auto h : signals) {
      TString hname = h->GetName();
      if(dataplotter_->signal_scales_.find(hname) != dataplotter_->signal_scales_.end())
        h->Scale(1./dataplotter_->signal_scales_[hname]);
      else
        h->Scale(1./dataplotter_->signal_scale_);
      hname.ReplaceAll("#", "");
      hname.ReplaceAll("->", "");
      hname.ReplaceAll(Form("_%s_%i_%i", hist.Data(), isys, set), "");
      hname.ToLower();
      hname += "_sys_"; hname += isys;
      h->SetName(hname.Data());
      h->Write();
    }
    // f->Flush();
  }
  return status;
}


int get_same_flavor_histogram(int set = 8, TString selection = "mumu",
                              vector<int> years = {2016, 2017, 2018},
                              TString base = "nanoaods_dev") {

  int status(0);
  TString hist = "lepm";

  if(selection != "mumu" && selection != "ee") {
    cout << "Undefined same flavor selection " << selection.Data() << endl;
    return 1;
  }

  //initialize the dataplotter
  years_ = years;
  status = initialize_plotter(selection, base);
  if(status) {
    cout << "DataPlotter initialization returned " << status << ", exiting!\n";
    return status;
  }

  int set_offset = HistMaker::kMuMu;
  if     (selection == "ee") set_offset = HistMaker::kEE;

  dataplotter_->rebinH_ = 1;
  //get background distribution
  THStack* hstack = dataplotter_->get_stack(hist, "event", set+set_offset);
  if(!hstack) return 1;

  std::vector<TH1*> signals = dataplotter_->get_signal(hist, "event", set+set_offset);
  if(signals.size() == 0) return 2;
  for(auto h : signals) {
    if(!h) return 2;
  }

  TH1* hdata = dataplotter_->get_data(hist, "event", set+set_offset);
  if(!hdata) return 3;
  hdata->SetName("hdata");

  TString year_string;
  for(unsigned i = 0; i < years.size(); ++i) {
    if(i > 0) year_string += "_";
    year_string += years[i];
  }

  gSystem->Exec("[ ! -d histograms ] && mkdir histograms");
  TFile* fout = new TFile(Form("histograms/%s_CR_lepm_%i_%s.hist", selection.Data(), set, year_string.Data()), "RECREATE");
  hdata->Write();
  hstack->SetName("hstack");
  hstack->Write();
  TH1* hlast = (TH1*) hstack->GetStack()->Last();
  hlast->SetName("hbackground");
  hlast->Write();

  for(auto h : signals) {
    TString hname = h->GetName();
    hname.ReplaceAll("#", "");
    hname.ReplaceAll("->", "");
    hname.ReplaceAll(Form("_%s_%i", hist.Data(), set+set_offset), "");
    hname.ToLower();
    h->SetName(hname.Data());
    if(dataplotter_->signal_scales_.find(hname) != dataplotter_->signal_scales_.end())
      h->Scale(1./dataplotter_->signal_scales_[hname]);
    else
      h->Scale(1./dataplotter_->signal_scale_);
    h->Write();
  }

  if(!ignore_sys_)
    status += get_same_flavor_systematics(set+set_offset, hist, hdata, fout);
  fout->Write();
  fout->Close();
  return status;
}

int get_systematics(int set, TString hist, TH1* hdata, TFile* f, TString canvas_name) {
  int status(0);
  f->cd();
  dataplotter_->rebinH_ = overall_rebin_;
  hbkg_->SetLineColor(kRed-3);
  hbkg_->SetFillColor(0);
  TH1* hdata_ratio = (TH1*) hdata->Clone("hdata_ratio");
  hdata_ratio->Divide(hbkg_);

  CLFV::Systematics systematics;

  if(print_sys_plots_) gSystem->Exec(Form("mkdir -p %s_sys", canvas_name.Data()));

  //Loop through each systematic, creating PDFs and example figures
  const int sys_start = (test_sys_ >= 0) ? test_sys_     : (skip_shape_sys_) ? kMaxSystematics : -2;
  const int sys_max   = (test_sys_ >= 0) ? test_sys_ + 1 : (get_scale_sys_ ) ? kMaxSystematics + kMaxScaleSystematics : kMaxSystematics;
  for(int isys = sys_start; isys < sys_max; ++isys) {
    if(isys > 0 && systematics.GetName(isys) == "") continue; //only retrieve defined systematics

    THStack* hstack = nullptr;
    vector<TH1*> signals;
    if(isys >= kMaxSystematics) { //normalization systematics
      //retrieve the systematic scale factor
      CLFV::ScaleUncertainty_t sys_scale = systematics.GetScale(isys);
      if(sys_scale.name_ == "") { //not implemented
        cout << __func__ << " Systematic " << isys << " (" << systematics.GetName(isys).Data() << ") not implemented\n";
        continue;
      }
      if(sys_scale.scale_ <= 0.) {
        cout << __func__ << ": Warning! Scale uncertainty " << isys << "(" << sys_scale.name_.Data() << ") is <= 0 (" << sys_scale.scale_ << ")\n";
      }
      printf("%s: Scale systematic %3i (%s): Scale = %.3f, data = %o, mc = %o, embed = %o\n", __func__, isys,
             sys_scale.name_.Data(), sys_scale.scale_, sys_scale.data_, sys_scale.mc_, sys_scale.embed_);
      sys_scale.Print();
      if(test_sys_ >= 0) dataplotter_->verbose_ = 7;
      //Get background for the systematic
      hstack = dataplotter_->get_stack(Form("%s_0", hist.Data()), "systematic", set, &sys_scale);
      //Get the signals
      signals = dataplotter_->get_signal(Form("%s_0", hist.Data()), "systematic", set, &sys_scale);
    } else {
      //Get background for the systematic
      hstack = dataplotter_->get_stack(Form("%s_%i", hist.Data(), max(0, isys)), "systematic", set);
      //Get the signals
      signals = dataplotter_->get_signal(Form("%s_%i", hist.Data(), max(0, isys)), "systematic", set);
    }
    if(!hstack) {++status; continue;}
    if(!hstack->GetStack()) {++status; continue;}
    hstack->SetName(Form("hstack_sys_%i", isys));
    TH1* hbkg = (TH1*) hstack->GetStack()->Last();
    hbkg->SetName(Form("hbkg_sys_%i", isys));
    if(signals.size() == 0) {++status; continue;}


    //Create an example plot with the systematic shift + ratio plot
    if(print_sys_plots_) {
      TCanvas* c = new TCanvas(Form("c_sys_%i", isys), Form("c_sys_%i", isys), 1200, 900);
      TPad* pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1. );
      TPad* pad2 = new TPad("pad2", "pad2", 0., 0. , 1., 0.3);
      pad1->Draw(); pad2->Draw();
      pad1->cd();

      //if isys < 0, plot the statistical uncertainty
      if(isys < 0) {
        for(int ibin = 1; ibin <= hbkg->GetNbinsX(); ++ibin) {
          double bin_val = hbkg->GetBinContent(ibin);
          double bin_err = hbkg->GetBinError(ibin);
          hbkg->SetBinContent(ibin, (isys == -2) ? bin_val + bin_err : bin_val - bin_err);
          for(TH1* signal : signals) {
            bin_val = signal->GetBinContent(ibin);
            bin_err = signal->GetBinError(ibin);
            signal->SetBinContent(ibin, (isys == -2) ? bin_val + bin_err : bin_val - bin_err);
          }
        }
      }

      //Get the systematic shift as a TGraph
      TGraph* g_bkg = dataplotter_->get_errors(hbkg_, hbkg, 0);
      g_bkg->SetFillColor(kRed-3);
      g_bkg->SetLineWidth(2);
      g_bkg->SetLineColor(kRed-3);
      g_bkg->SetFillStyle(3001);
      g_bkg->Draw("AP2");
      hbkg_->Draw("hist same");
      g_bkg->Draw("P2");

      //Get the signal systematic shifts
      for(unsigned index = 0; index < signals.size(); ++index) {
        auto h = signals[index];
        auto hnom = hsigs_[index];
        TGraph* g_sig = dataplotter_->get_errors(hnom, h, 0);
        hnom->SetFillColor(0);
        hnom->SetLineWidth(2);
        hnom->Draw("hist same");
        g_sig->SetLineColor(hnom->GetLineColor());
        g_sig->SetFillStyle(3001);
        g_sig->SetFillColor(g_sig->GetLineColor());
        g_sig->Draw("P2");
      }
      hdata->Draw("same E");
      g_bkg->GetXaxis()->SetRangeUser(xmin_,xmax_);
      g_bkg->SetTitle("");
      if(logy_) pad1->SetLogy();

      //Draw the ratio plot
      pad2->cd();
      double r_min, r_max;
      TGraph* g_r_bkg = dataplotter_->get_errors(hbkg_, hbkg, 0, true, r_min, r_max);
      g_r_bkg->SetFillColor(kRed-3);
      g_r_bkg->SetLineWidth(2);
      g_r_bkg->SetLineColor(kRed-3);
      g_r_bkg->SetFillStyle(3001);
      g_r_bkg->Draw("ALE2");
      g_r_bkg->SetName(Form("g_r_bkg_sys_%i", isys));

      for(unsigned index = 0; index < signals.size(); ++index) {
        auto h = signals[index];
        auto hnom = hsigs_[index];
        double r_min_s, r_max_s;
        TGraph* g_sig = dataplotter_->get_errors(hnom, h, 0, true, r_min_s, r_max_s);
        r_min = min(r_min, r_min_s);
        r_max = max(r_max, r_max_s);
        g_sig->SetLineColor(hnom->GetLineColor());
        if(index == 0) {
          g_sig->SetFillColor(g_sig->GetLineColor());
          g_sig->SetFillStyle(3004);
          g_sig->Draw("PL");
          g_sig->Draw("PLE2");
        } else {
          g_sig->Draw("PL");
        }
      }
      hdata_ratio->Draw("same E");
      g_r_bkg->GetXaxis()->SetRangeUser(xmin_,xmax_);
      g_r_bkg->GetXaxis()->SetLabelSize(0.08);
      g_r_bkg->GetYaxis()->SetLabelSize(0.08);
      g_r_bkg->GetYaxis()->SetRangeUser(max(0.5, min(0.95, 1. + 1.15*(r_min - 1.))), min(1.5, max(1.05, 1. + 1.15*(r_max - 1.))));
      g_r_bkg->SetTitle("");

      c->Print(Form("%s_sys/sys_%i.png", canvas_name.Data(), isys));
      delete c;
    } //end if(print_sys_plots_)

    for(auto h : signals) {
      TString hname = h->GetName();
      if(dataplotter_->signal_scales_.find(hname) != dataplotter_->signal_scales_.end())
        h->Scale(1./dataplotter_->signal_scales_[hname]);
      else
        h->Scale(1./dataplotter_->signal_scale_);
      hname = hname(0,hname.First('_'));
      hname.ReplaceAll("#", "");
      hname.ReplaceAll("->", "");
      // hname.ReplaceAll(Form("_%s_%i_%i", hist.Data(), isys, set), "");
      hname.ToLower();
      hname += "_sys_"; hname += isys;
      h->SetName(hname.Data());
      h->Write();
    }
    hbkg->Write();
    hstack->Write();
    // f->Flush();
  }
  return status;
}

int get_individual_MVA_histogram(int set = 8, TString selection = "zmutau",
                                 vector<int> years = {2016, 2017, 2018},
                                 TString base = "nanoaods_dev") {

  ///////////////////////////////////////////////
  // Initialize setup

  hsigs_ = {};
  int status(0);

  //Get the name of the MVA for this selection
  TString hist;
  if     (selection == "hmutau"  ) hist = "mva0";
  else if(selection == "zmutau"  ) hist = "mva1";
  else if(selection == "hetau"   ) hist = "mva2";
  else if(selection == "zetau"   ) hist = "mva3";
  else if(selection == "hemu"    ) hist = "mva4";
  else if(selection == "zemu"    ) hist = "mva5";
  else if(selection == "hmutau_e") hist = "mva6";
  else if(selection == "zmutau_e") hist = "mva7";
  else if(selection == "hetau_mu") hist = "mva8";
  else if(selection == "zetau_mu") hist = "mva9";
  else {
    cout << "Unidentified selection " << selection.Data() << endl;
    return -1;
  }
  if(use_dev_mva_ > 0) hist += Form("_%i", use_dev_mva_);

  //define parameters for dataplotter script
  TString selec = selection; selec.ReplaceAll("z", ""); selec.ReplaceAll("h", "");
  useEmbed_ = (embed_mode_ == 2) ? !selection.Contains("_") : embed_mode_; //whether or not to use embedding
  years_ = years;
  status = initialize_plotter(selec, base);
  if(status) {
    cout << "DataPlotter initialization returned " << status << ", exiting!\n";
    return status;
  }

  int set_offset = (use_lep_tau_set_) ? (selec.Contains("mutau")) ? HistMaker::kMuTauE : HistMaker::kETauMu : HistMaker::kEMu;
  if     (selec == "mutau") set_offset = HistMaker::kMuTau;
  else if(selec == "etau" ) set_offset = HistMaker::kETau;

  dataplotter_->rebinH_ = overall_rebin_;

  ///////////////////////////////////////////////
  // Get the distributions

  //get background distribution
  THStack* hstack = dataplotter_->get_stack(hist, "event", set+set_offset);
  if(!hstack || hstack->GetNhists() == 0) return 1;

  //get the signal vector
  std::vector<TH1*> signals = dataplotter_->get_signal(hist, "event", set+set_offset);
  for(auto h : signals) {
    if(!h) return 2;
  }
  if(signals.size() == 0) return 2;

  //get the data
  TH1* hdata = dataplotter_->get_data(hist, "event", set+set_offset);
  if(!hdata) return 3;
  hdata->SetName("hdata");

  //determine an appropriate plotting range and if log or linear plot
  TH1* hlast = (TH1*) hstack->GetStack()->Last()->Clone("hbackground");
  const int nbins = hlast->GetNbinsX();
  xmin_ = hlast->GetBinLowEdge(1);
  xmax_ = hlast->GetBinLowEdge(1) - 1.;
  int ndec(0), nnonzero(0);
  for(int ibin = 1; ibin <= nbins; ++ibin) {
    const double binc = hlast->GetBinContent(ibin);
    //if haven't found a non-zero bin yet, update xmin, else update xmax
    if(binc <= 0. && xmax_ < xmin_) xmin_ = hlast->GetBinLowEdge(ibin);
    else if(binc > 0.) xmax_ = hlast->GetBinLowEdge(ibin) + hlast->GetBinWidth(ibin);
    if(binc > 0.) {
      ++nnonzero;
      if(ibin < nbins && hlast->GetBinContent(ibin+1) < 0.8) ++ndec;
    }
  }
  logy_ = ndec > 0.75*nnonzero; //if generally an exponential-like drop, plot in log

  //blind high MVA score region if desired
  if(blind_data_) {
    double mva_cut = (xmin_ > -0.1) ? 0.5 : 0.;
    int bin_start = hdata->FindBin(mva_cut);
    for(int bin = bin_start; bin <= hdata->GetNbinsX() + 1; ++bin) {
      hdata->SetBinContent(bin, 0.);
      hdata->SetBinError(bin, 0.5);
    }
  }

  ///////////////////////////////////////////////
  // Make an initial plot of the results

  TCanvas* c = new TCanvas();

  cout << "Plotting parameters: xmin = " << xmin_ << " xmax = " << xmax_ << " logy = " << logy_ << endl;
  hdata->Draw("E");
  hstack->Draw("same hist noclear");
  for(auto h : signals) {
    h->Draw("hist same");
  }
  hdata->Draw("same E");

  hdata->GetXaxis()->SetRangeUser(xmin_,xmax_);
  logy_ |= use_dev_mva_ == 2;
  if(logy_) {
    hdata->GetYaxis()->SetRangeUser(0.5,2*hstack->GetMaximum());
    c->SetLogy();
  } else {
    hdata->GetYaxis()->SetRangeUser(0.9,1.1*hstack->GetMaximum());
  }

  ///////////////////////////////////////////////
  // Save the results

  TString year_string;
  for(unsigned i = 0; i < years.size(); ++i) {
    if(i > 0) year_string += "_";
    year_string += years[i];
  }

  //remove dev label for systematics/naming, since they're not defined
  if(use_dev_mva_ > 0) hist.ReplaceAll(Form("_%i", use_dev_mva_), "");

  gSystem->Exec(Form("[ ! -d plots/latest_production/%s ] && mkdir -p plots/latest_production/%s", year_string.Data(), year_string.Data()));
  TString canvas_name = Form("plots/latest_production/%s/hist_%s_%s_%i", year_string.Data(), hist.Data(), selection.Data(), set);
  c->Print(canvas_name + ".png");

  gSystem->Exec("[ ! -d histograms ] && mkdir histograms");
  TFile* fout = new TFile(Form("histograms/%s_%s_%i_%s.hist", selection.Data(), hist.Data(), set, year_string.Data()), "RECREATE");
  hdata->Write();
  hstack->SetName("hstack");
  hstack->Write();
  hlast->Write();
  hbkg_ = (TH1*) hlast->Clone("hbkg_");

  //write each signal in the standard way (e.g. H->e#mu --> hemu, Z->#mu#tau --> zmutau)
  for(auto h : signals) {
    TString hname = h->GetName();
    hname.ReplaceAll("#", "");
    hname.ReplaceAll("->", "");
    if(use_dev_mva_ > 0)
      hname.ReplaceAll(Form("_%s_%i_%i", hist.Data(), use_dev_mva_, set+set_offset), "");
    else
      hname.ReplaceAll(Form("_%s_%i", hist.Data(), set+set_offset), "");
    hname.ToLower();
    h->SetName(hname.Data());
    hsigs_.push_back((TH1*) h->Clone(Form("hsig_%s_", hname.Data()))); //store the signal for plotting against systematic
    //remove the scale factor applied to the signal
    if(dataplotter_->signal_scales_.find(hname) != dataplotter_->signal_scales_.end()) {
      h->Scale(1./dataplotter_->signal_scales_[hname]);
    } else {
      h->Scale(1./dataplotter_->signal_scale_);
    }
    //save each signal
    h->Write();
  }

  //get the systematics if needed
  if(!ignore_sys_)
    status += get_systematics(set+set_offset, hist, hdata, fout, canvas_name);

  //save the results
  fout->Write();
  fout->Close();
  return status;
}

int get_MVA_histogram(vector<int> sets = {8}, TString selection = "zmutau",
                      vector<int> years = {2016, 2017, 2018},
                      TString base = "nanoaods_dev",
                      int had_only = 0 /*0: all; 1: hadronic; -1: leptonic; -2: same-flavor*/) {
  int status(0);
  if(use_dev_mva_ == 1) overall_rebin_ = 50;
  if(use_dev_mva_ == 2) overall_rebin_ = 4;
  useLepTauSet_ = use_lep_tau_set_;
  includeHiggs_ = selection.BeginsWith("h");

  if(embed_mode_ != 1) cout << "!!! Warning! Using non-nominal embedding mode " << embed_mode_ << endl;

  //loop through the year list if separating years
  for(int iyear = 0; iyear < ((separate_years_) ? years.size() : 1); ++iyear) {
    vector<int> years_use;
    if(separate_years_) years_use = {years[iyear]}; //do a single year at a time
    else                years_use = years;
    //loop through the histogram set list for fitting in categories
    for(int set : sets) {
      //get the hadronic tau region
      if(had_only >= 0) status += get_individual_MVA_histogram(set, selection, years_use, base);
      //get the leptonic tau region
      if(test_sys_ < 0 && (had_only == 0 || had_only == -1)) { //only do one selection if debugging
        if(selection.Contains("mutau"))
          status += get_individual_MVA_histogram(set, selection+"_e", years_use, base);
        else if(selection.Contains("etau"))
          status += get_individual_MVA_histogram(set, selection+"_mu", years_use, base);
      }
      //get the same-flavor control regions
      if(do_same_flavor_ && (had_only == 0 || had_only == -2)) {
        cout << "Getting mumu histograms for set " << set << endl;
        status += get_same_flavor_histogram(set, "mumu", years_use, base);
        cout << "Getting ee histograms for set " << set << endl;
        status += get_same_flavor_histogram(set, "ee"  , years_use, base);
      }
    }
  }
  return status;
}
