//Script to retrieve the background and signal di-lepton mass histograms
#include "histograms/datacards.C"
// #include "../interface/GlobalConstants.h"
#include "bemu_defaults.C"

int overall_rebin_ = 1;
TH1* hbkg_;
vector<TH1*> hsigs_;
int test_sys_ = -1; //set to systematic number if debugging/inspecting it
double xmin_;
double xmax_;
bool do_systematics_   = true ; //retrieve systematically shifted templates
bool sig_sys_only_     = true ; //only process systematic templates for signal processes
bool allow_sys_errors_ = true ; //if there are missing systematic histograms, clone the default
TH1* hDefault_; //store the default histogram in case of missing systematics
TH1* hDY_;

CLFV::DataPlotter* dataplotter_ = nullptr;

//initialize the datasets using a DataPlotter
Int_t initialize_plotter(TString selection, TString base) {
  if(dataplotter_) delete dataplotter_;
  dataplotter_ = new DataPlotter();

  useWG_ = 0; //FIXME: Remove this
  dataplotter_->include_misid_ = selection == "mutau" || selection == "etau";
  dataplotter_->include_qcd_   = !dataplotter_->include_misid_ && selection != "mumu" && selection != "ee";
  dataplotter_->verbose_       = max(0, verbose_ - 1);
  dataplotter_->debug_         = max(0, verbose_ - 1);
  dataplotter_->qcd_scale_     = 1.;
  dataplotter_->embed_scale_   = embedScale_;
  dataplotter_->selection_     = selection;
  dataplotter_->folder_        = "";


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

int get_same_flavor_systematics(int set, TString hist, TFile* f) {
  int status(0);
  f->cd();
  dataplotter_->rebinH_ = overall_rebin_;

  //Loop through each systematic, writing MC histograms
  for(int isys = (test_sys_ >= 0 ? test_sys_ : 0); isys < (test_sys_ >= 0 ? test_sys_ + 1 : kMaxSystematics); ++isys) {
    cout << "Retrieving same-flavor systematic " << isys << "...\n";
    //Get background for the systematic
    THStack* hstack = dataplotter_->get_stack(Form("%s_%i", hist.Data(), isys), "systematic", set);
    TH1 *hbkg(0), *hDY(0);
    if(!hstack || !hstack->GetStack()) {
      ++status;
      if(!allow_sys_errors_)
        continue;
      else
        cout << "Missing same-flavor histogram for set " << set << " and sys " << isys << endl;
      hbkg = (TH1*) hDefault_->Clone(Form("hbkg_sys_%i", isys));
      hDY  = (TH1*) hDY_     ->Clone(Form("hDY_sys_%i" , isys));
    } else {
      //add up components of the Z->ll/Bkg
      for(auto o : *(hstack->GetHists())) {
        TH1* h = (TH1*) o;
        if(TString(h->GetName()).Contains("Z->ee/#mu#mu")) {
          if(hDY) hDY->Add(h);
          else   {hDY = h; hDY->SetName(Form("hDY_sys_%i", isys));}
        } else {
          if(hbkg) hbkg->Add(h);
          else    {hbkg = h; hbkg->SetName(Form("hbkg_sys_%i", isys));}
        }
      }
    }
    if(test_sys_ >= 0) {
      cout << "Nominal background: " << hbkg_->Integral()
           << " shifted background: " << hbkg->Integral() << endl;
    }

    hbkg->Write();
    hDY->Write();
    // f->Flush();
  }
  return status;
}

int get_systematics(int set, TString hist, TFile* f, TString canvas_name) {
  int status(0);
  f->cd();
  dataplotter_->rebinH_ = overall_rebin_;
  hbkg_->SetLineColor(kRed-3);
  hbkg_->SetFillColor(0);

  //Loop through each systematic, creating PDFs and example figures
  for(int isys = (test_sys_ >= 0 ? test_sys_ : 0); isys < (test_sys_ >= 0 ? test_sys_ + 1 : kMaxSystematics); ++isys) {
    // cout << "Retrieving histograms for systematic " << isys << endl;
    //Get background for the systematic
    THStack* hstack = (sig_sys_only_) ? nullptr : dataplotter_->get_stack(Form("%s_%i", hist.Data(), isys), "systematic", set);
    if(!sig_sys_only_ && !hstack) {++status; continue;}
    if(!sig_sys_only_ && !hstack->GetStack()) {++status; continue;}
    TH1* hbkg = (sig_sys_only_) ? (TH1*) hbkg_->Clone(Form("hbkg_sys_%i", isys)) : (TH1*) hstack->GetStack()->Last();
    hbkg->SetName(Form("hbkg_sys_%i", isys));

    //Get the signals
    vector<TH1*> signals = dataplotter_->get_signal(Form("%s_%i", hist.Data(), isys), "systematic", set);
    if(signals.size() == 0) {
      cout << "No signal template for systematic " << isys << endl;
      ++status;
      continue;
    }
    // //get data
    // TH1* hdata = dataplotter_->get_data(Form("%s_%i", hist.Data(), isys), "systematic", set);
    // if(!hdata) {++status; continue;}

    //Create an example plot with the systematic shift + ratio plot
    TCanvas* c = new TCanvas(Form("c_sys_%i", isys), Form("c_sys_%i", isys), 1200, 900);
    TPad* pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1. );
    TPad* pad2 = new TPad("pad2", "pad2", 0., 0. , 1., 0.3);
    pad1->Draw(); pad2->Draw();
    pad1->cd();

    if(test_sys_ >= 0) {
      cout << "Nominal background: " << hbkg_->Integral()
           << " shifted background: " << hbkg->Integral() << endl;
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
      TString hname = h->GetName();
      if(dataplotter_->signal_scales_.find(hname) != dataplotter_->signal_scales_.end())
        h->Scale(1./dataplotter_->signal_scales_[hname]);
      else
        h->Scale(1./dataplotter_->signal_scale_);
      TGraph* g_sig = dataplotter_->get_errors(hnom, h, 0);
      hnom->SetFillColor(0);
      hnom->SetLineWidth(2);
      hnom->Draw("hist same");
      g_sig->SetLineColor(hnom->GetLineColor());
      g_sig->SetFillStyle(3001);
      g_sig->SetFillColor(g_sig->GetLineColor());
      g_sig->Draw("P2");
      if(test_sys_ >= 0) {
        cout << "Nominal signal " << index << ": " << hnom->Integral()
             << " shifted signal: " << h->Integral() << endl;
        g_sig->Print();
      }
    }
    g_bkg->GetXaxis()->SetRangeUser(xmin_, xmax_);
    g_bkg->SetTitle("");

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
    g_r_bkg->GetXaxis()->SetRangeUser(xmin_, xmax_);
    g_r_bkg->GetXaxis()->SetLabelSize(0.08);
    g_r_bkg->GetYaxis()->SetLabelSize(0.08);
    g_r_bkg->GetYaxis()->SetRangeUser(min(0.995, 1. + 1.15*(r_min - 1.)), max(1.005, 1. + 1.15*(r_max - 1.)));
    g_r_bkg->SetTitle("");

    if(isys == 0) gSystem->Exec(Form("mkdir -p %s_sys", canvas_name.Data()));
    c->Print(Form("%s_sys/sys_%i.png", canvas_name.Data(), isys));
    for(auto h : signals) {
      TString hname = h->GetName();
      hname.ReplaceAll("#", "");
      hname.ReplaceAll("->", "");
      hname.ReplaceAll(Form("_%s_%i", hist.Data(), set), "");
      hname.ToLower();
      hname += "_sys_"; hname += isys;
      h->SetName(hname.Data());
      h->Write();
    }
    hbkg->Write();
    f->Flush();
    delete c;
  }
  return status;
}

int get_same_flavor_histogram(int set, TString selection, vector<int> years, TString base) {

  int status(0);
  TString hist = "lepm";

  //initialize the dataplotter
  years_ = years;
  status = initialize_plotter(selection, base);
  if(status) {
    cout << "DataPlotter initialization returned " << status << ", exiting!\n";
    return status;
  }

  int set_offset = (selection == "mumu") ? HistMaker::kMuMu : CLFVHistMaker::kEE;

  dataplotter_->rebinH_ = overall_rebin_;

  //get background distribution
  THStack* hstack = dataplotter_->get_stack(hist, "event", set+set_offset);
  if(!hstack) return 1;

  //get data
  TH1* hdata = dataplotter_->get_data(hist, "event", set+set_offset);
  if(!hdata) return 2;

  /////////////////////////////////////////////////
  // Plot the results
  /////////////////////////////////////////////////

  TCanvas* c = new TCanvas();
  hdata->Draw("E");
  hstack->Draw("same hist noclear");
  hdata->Draw("same E");
  hdata->Draw("same E");
  hdata->GetXaxis()->SetRangeUser(xmin_,xmax_);
  hdata->GetYaxis()->SetRangeUser(0.9,1.1*hstack->GetMaximum());

  TString year_string;
  for(unsigned i = 0; i < years.size(); ++i) {
    if(i > 0) year_string += "_";
    year_string += years[i];
  }

  gSystem->Exec(Form("[ ! -d plots/latest_production/%s ] && mkdir -p plots/latest_production/%s", year_string.Data(), year_string.Data()));
  TString canvas_name = Form("plots/latest_production/%s/hist_%s_%s_%i", year_string.Data(), hist.Data(), selection.Data(), set);
  c->Print(canvas_name + ".png");

  gSystem->Exec("[ ! -d histograms ] && mkdir histograms");
  TFile* fout = new TFile(Form("histograms/%s_%s_%i_%s.hist", selection.Data(), hist.Data(), set, year_string.Data()), "RECREATE");
  hstack->SetName("hstack");
  hstack->Write();
  TH1* hlast = (TH1*) hstack->GetStack()->Last();
  hlast->SetName("hbackground");
  hlast->Write();
  hdata->SetName("hdata");
  hdata->Write();

  hDefault_ = (TH1*) hlast->Clone("hDefault");
  hDY_ = (TH1*) hlast->Clone("hDYDefault");
  if(do_systematics_)
    status += get_same_flavor_systematics(set+set_offset, hist, fout);
  delete hDefault_;
  fout->Close();
  return status;

}

//Get the signal, background, and data histograms in the signal selection
int get_bemu_single_histogram(int set = 8, TString selection = "zemu",
                              vector<int> years = {2016, 2017, 2018},
                              TString base = "nanoaods_dev") {

  hsigs_ = {};
  int status(0);
  TString hist = "lepm";

  //initialize the dataplotter
  TString selec = selection; selec.ReplaceAll("z", ""); selec.ReplaceAll("h", "");
  years_ = years;
  status = initialize_plotter(selec, base);
  if(status) {
    cout << "DataPlotter initialization returned " << status << ", exiting!\n";
    return status;
  }
  int set_offset = HistMaker::kEMu;

  dataplotter_->rebinH_ = overall_rebin_;

  if(test_sys_ >= 0) dataplotter_->verbose_ = 1;

  //get background distribution
  if(set == 13 || set == 18) {dataplotter_->signal_scale_ = 20.; dataplotter_->signal_scales_["H->e#mu"] = 50.;}
  THStack* hstack = dataplotter_->get_stack(hist, "event", set+set_offset);
  if(!hstack) return 1;

  //get signal distributions
  std::vector<TH1*> signals = dataplotter_->get_signal(hist, "event", set+set_offset);
  for(auto h : signals) {
    if(!h) return 2;
  }

  //get data
  TH1* hdata = dataplotter_->get_data(hist, "event", set+set_offset);
  if(!hdata) return 3;
  TH1* hdata_clone = (TH1*) hdata->Clone("hdata_clone");
  if(blind_data_) {
    int bin_lo, bin_hi;
    bin_lo = hdata_clone->FindBin(86.);
    bin_hi = hdata_clone->FindBin(96.);
    for(int bin = bin_lo; bin < bin_hi; ++bin) hdata_clone->SetBinContent(bin, 0.);
    bin_lo = hdata_clone->FindBin(120.);
    bin_hi = hdata_clone->FindBin(130.);
    for(int bin = bin_lo; bin < bin_hi; ++bin) hdata_clone->SetBinContent(bin, 0.);
  }
  hdata_clone->Draw("same E");

  TCanvas* c = new TCanvas();
  hdata_clone->Draw("E");
  hstack->Draw("hist noclear");
  hdata_clone->Draw("same E");
  double max_val = std::max(hdata->GetMaximum(), hstack->GetMaximum());
  for(auto h : signals) {
    h->Draw("hist same");
    max_val = std::max(max_val, h->GetMaximum());
  }

  if(selection == "zemu") {xmin_ = 70.; xmax_ = 110.;}
  else if(selection == "hemu") {xmin_ = 110.; xmax_ = 150.;}
  else {xmin_ = 50.; xmax_ = 170.;}

  hdata_clone->GetXaxis()->SetRangeUser(xmin_, xmax_);
  hdata_clone->GetYaxis()->SetRangeUser(0.1, 1.1*max_val);
  // hstack->SetMaximum(1.1*max_val);
  TString year_string;
  for(unsigned i = 0; i < years.size(); ++i) {
    if(i > 0) year_string += "_";
    year_string += years[i];
  }

  gSystem->Exec(Form("[ ! -d plots/latest_production/%s ] && mkdir -p plots/latest_production/%s", year_string.Data(), year_string.Data()));
  TString canvas_name = Form("plots/latest_production/%s/hist_%s_%s_%i", year_string.Data(), hist.Data(), selection.Data(), set);
  c->Print(canvas_name + ".png");

  gSystem->Exec("[ ! -d histograms ] && mkdir histograms");
  TFile* fout = new TFile(Form("histograms/%s_%s_%i_%s.hist", selection.Data(), hist.Data(), set, year_string.Data()), "RECREATE");
  //Write the entire stack in case it's needed
  hstack->SetName("hstack");
  hstack->Write();
  //get a histogram of the background MC
  TH1* hlast = (TH1*) hstack->GetStack()->Last();
  hlast->SetName("hbackground");
  hlast->Write();
  hbkg_ = (TH1*) hlast->Clone("hbkg_");
  //get just the Drell-Yan contribution of the background to correct N(Data) --> N(Z events)
  TH1* hDY = (TH1*) hlast->Clone("hDY");
  for(auto o : *(hstack->GetHists())) {
    TH1* h = (TH1*) o;
    if(TString(h->GetName()).Contains("DY")) continue;
    hDY->Add(h, -1.); //subtract off non-DY MC
  }
  hDY->Write();
  for(auto h : signals) {
    TString hname = h->GetName();
    cout << "Signal " << hname.Data() << " has an integral of " << h->Integral() << " before re-scaling and ";
    if(dataplotter_->signal_scales_.find(hname) != dataplotter_->signal_scales_.end())
      h->Scale(1./dataplotter_->signal_scales_[hname]);
    else
      h->Scale(1./dataplotter_->signal_scale_);
    cout << h->Integral() << " after\n";
    hname.ReplaceAll("#", "");
    hname.ReplaceAll("->", "");
    hname.ReplaceAll(Form("_%s_%i", hist.Data(), set+set_offset), "");
    hname.ToLower();
    h->SetName(hname.Data());
    hsigs_.push_back((TH1*) h->Clone(Form("hsig_%s_", hname.Data()))); //store the signal for plotting against systematic
    h->Write();
  }
  hdata->SetName("hdata");
  hdata->Write();

  if(do_systematics_)
    status += get_systematics(set+set_offset, hist, fout, canvas_name);
  fout->Close();
  return status;
}

int get_bemu_histogram(vector<int> sets, TString selection = "zemu",
                       vector<int> years = {2016, 2017, 2018},
                       TString base = "nanoaods_dev",
                       bool do_same_flavor = true) {
  useEmbed_ = bemu_embed_mode_;
  int status(0);
  for(int set : sets) {
    cout << "Getting signal region histograms for set " << set << "...\n";
    status += get_bemu_single_histogram(set, selection, years, base);
  }
  if(do_same_flavor && use_same_flavor_) {
    useEmbed_ = 0;
    const int set = 8; //normalization set
    cout << "Getting mumu region histograms for set " << set << "...\n";
    status += get_same_flavor_histogram(set, "mumu", years, base);
    cout << "Getting ee region histograms for set " << set << "...\n";
    status += get_same_flavor_histogram(set, "ee", years, base);
  }
  return status;
}
