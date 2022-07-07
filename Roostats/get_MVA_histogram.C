//Script to retrieve the background and signal MVA histograms
#include "../histograms/dataplotter_clfv.C"
// #include "../interface/SystematicHist_t.hh"

int overall_rebin_ = 0;
TH1* hbkg_;
vector<TH1*> hsigs_;
int test_sys_ = -1; //set to systematic number if debugging/inspecting it
bool blind_data_ = true; //set data bins > MVA score level to 0
bool ignore_sys_ = true; //don't get systematics
bool use_dev_mva_ = false; //use the extra MVA hist for development, mva?_1
bool do_same_flavor_ = false; //retrieve Z->ll control region data

int get_same_flavor_systematics(int set, TString hist, TH1* hdata, TFile* f) {
  int status(0);
  f->cd();
  dataplotter_->rebinH_ = 1;

  //Loop through each systematic, creating PDFs and example figures
  for(int isys = (test_sys_ >= 0 ? test_sys_ : 0); isys < (test_sys_ >= 0 ? test_sys_ + 1 : kMaxSystematics); ++isys) {
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

  //define parameters for dataplotter script
  years_ = years;
  status = nanoaod_init(selection, base, base);
  if(status) {
    cout << "DataPlotter initialization script returned " << status << ", exiting!\n";
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

  //Loop through each systematic, creating PDFs and example figures
  for(int isys = (test_sys_ >= 0 ? test_sys_ : 0); isys < (test_sys_ >= 0 ? test_sys_ + 1 : kMaxSystematics); ++isys) {

    //Get background for the systematic
    THStack* hstack = dataplotter_->get_stack(Form("%s_%i", hist.Data(), isys), "systematic", set);
    if(!hstack) {++status; continue;}
    if(!hstack->GetStack()) {++status; continue;}
    hstack->SetName(Form("hstack_sys_%i", isys));
    TH1* hbkg = (TH1*) hstack->GetStack()->Last();
    hbkg->SetName(Form("hbkg_sys_%i", isys));

    //Get the signals
    vector<TH1*> signals = dataplotter_->get_signal(Form("%s_%i", hist.Data(), isys), "systematic", set);
    if(signals.size() == 0) {++status; continue;}

    //Create an example plot with the systematic shift + ratio plot
    TCanvas* c = new TCanvas(Form("c_sys_%i", isys), Form("c_sys_%i", isys), 1200, 900);
    TPad* pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1. );
    TPad* pad2 = new TPad("pad2", "pad2", 0., 0. , 1., 0.3);
    pad1->Draw(); pad2->Draw();
    pad1->cd();

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
    g_bkg->GetXaxis()->SetRangeUser(-0.6,0.3);
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
    hdata_ratio->Draw("same E");
    g_r_bkg->GetXaxis()->SetRangeUser(-0.6,0.3);
    g_r_bkg->GetXaxis()->SetLabelSize(0.08);
    g_r_bkg->GetYaxis()->SetLabelSize(0.08);
    g_r_bkg->GetYaxis()->SetRangeUser(min(0.995, 1. + 1.15*(r_min - 1.)), max(1.005, 1. + 1.15*(r_max - 1.)));
    g_r_bkg->SetTitle("");

    if(isys == 0) gSystem->Exec(Form("mkdir -p %s_sys", canvas_name.Data()));
    c->Print(Form("%s_sys/sys_%i.png", canvas_name.Data(), isys));
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
    hbkg->Write();
    hstack->Write();
    // f->Flush();
    delete c;
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
  if(use_dev_mva_) hist += "_1";

  //define parameters for dataplotter script
  TString selec = selection; selec.ReplaceAll("z", ""); selec.ReplaceAll("h", "");
  years_ = years;
  status = nanoaod_init(selec, base, base);
  if(status) {
    cout << "DataPlotter initialization script returned " << status << ", exiting!\n";
    return status;
  }
  int set_offset = HistMaker::kEMu;
  if     (selec == "mutau") set_offset = HistMaker::kMuTau;
  else if(selec == "etau" ) set_offset = HistMaker::kETau;

  dataplotter_->rebinH_ = 10*overall_rebin_;

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
  //blind high MVA score region if desired
  if(blind_data_) {
    double mva_cut = 0.;
    int bin_start = hdata->FindBin(mva_cut);
    for(int bin = bin_start; bin <= hdata->GetNbinsX(); ++bin) {
      hdata->SetBinContent(bin, 0.);
      hdata->SetBinError(bin, 0.5);
    }
  }

  ///////////////////////////////////////////////
  // Make an initial plot of the results

  TCanvas* c = new TCanvas();
  hstack->Draw("hist noclear");
  for(auto h : signals) {
    h->Draw("hist same");
  }
  hdata->Draw("same E");
  hstack->GetXaxis()->SetRangeUser(-0.6,0.3);
  hstack->GetYaxis()->SetRangeUser(0.9,1.1*hstack->GetMaximum());

  ///////////////////////////////////////////////
  // Save the results

  TString year_string;
  for(unsigned i = 0; i < years.size(); ++i) {
    if(i > 0) year_string += "_";
    year_string += years[i];
  }

  //remove dev label for systematics/naming, since they're not defined
  if(use_dev_mva_) hist.ReplaceAll("_1", "");

  gSystem->Exec(Form("[ ! -d plots/latest_production/%s ] && mkdir -p plots/latest_production/%s", year_string.Data(), year_string.Data()));
  TString canvas_name = Form("plots/latest_production/%s/hist_%s_%s_%i", year_string.Data(), hist.Data(), selection.Data(), set);
  c->Print(canvas_name + ".png");

  gSystem->Exec("[ ! -d histograms ] && mkdir histograms");
  TFile* fout = new TFile(Form("histograms/%s_%s_%i_%s.hist", selection.Data(), hist.Data(), set, year_string.Data()), "RECREATE");
  hdata->Write();
  hstack->SetName("hstack");
  hstack->Write();
  TH1* hlast = (TH1*) hstack->GetStack()->Last();
  hlast->SetName("hbackground");
  hlast->Write();
  hbkg_ = (TH1*) hlast->Clone("hbkg_");

  //write each signal in the standard way (e.g. H->e#mu --> hemu, Z->#mu#tau --> zmutau)
  for(auto h : signals) {
    TString hname = h->GetName();
    hname.ReplaceAll("#", "");
    hname.ReplaceAll("->", "");
    if(use_dev_mva_)
      hname.ReplaceAll(Form("_%s_1_%i", hist.Data(), set+set_offset), "");
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
                      TString base = "nanoaods_dev") {
  int status(0);
  for(int set : sets) {
    status += get_individual_MVA_histogram(set, selection, years, base);
    if(test_sys_ < 0) { //only do one selection if debugging
      if(selection.Contains("mutau"))
        status += get_individual_MVA_histogram(set, selection+"_e", years, base);
      else if(selection.Contains("etau"))
        status += get_individual_MVA_histogram(set, selection+"_mu", years, base);
    }
    if(do_same_flavor_) {
      cout << "Getting mumu histograms for set " << set << endl;
      status += get_same_flavor_histogram(set, "mumu", years, base);
      cout << "Getting ee histograms for set " << set << endl;
      status += get_same_flavor_histogram(set, "ee"  , years, base);
    }
  }
  return status;
}
