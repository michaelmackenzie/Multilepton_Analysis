//Script to retrieve the background and signal di-lepton mass histograms
#include "../histograms/dataplotter_ztautau.C"
#include "../interface/SystematicHist_t.hh"
int overall_rebin_ = 1;
TH1D* hbkg_;
vector<TH1D*> hsigs_;
int test_sys_ = -1; //set to systematic number if debugging/inspecting it
double xmin_;
double xmax_;
bool blind_data_ = true;
bool do_systematics_ = true;

int get_systematics(int set, TString hist, TFile* f, TString canvas_name) {
  int status(0);
  f->cd();
  dataplotter_->rebinH_ = overall_rebin_;
  hbkg_->SetLineColor(kRed-3);
  hbkg_->SetFillColor(0);

  //Loop through each systematic, creating PDFs and example figures
  for(int isys = (test_sys_ >= 0 ? test_sys_ : 0); isys < (test_sys_ >= 0 ? test_sys_ + 1 : kMaxSystematics); ++isys) {

    //Get background for the systematic
    THStack* hstack = dataplotter_->get_stack(Form("%s_%i", hist.Data(), isys), "systematic", set);
    if(!hstack) {++status; continue;}
    if(!hstack->GetStack()) {++status; continue;}
    TH1D* hbkg = (TH1D*) hstack->GetStack()->Last();
    hbkg->SetName(Form("hbkg_sys_%i", isys));

    //Get the signals
    vector<TH1D*> signals = dataplotter_->get_signal(Form("%s_%i", hist.Data(), isys), "systematic", set);
    if(signals.size() == 0) {++status; continue;}
    // //get data
    // TH1D* hdata = dataplotter_->get_data(Form("%s_%i", hist.Data(), isys), "systematic", set);
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
      if(dataplotter_->signal_scales_.find(hname) != dataplotter_->signal_scales_.end())
        h->Scale(1./dataplotter_->signal_scales_[hname]);
      else
        h->Scale(1./dataplotter_->signal_scale_);
      hname.ReplaceAll("#", "");
      hname.ReplaceAll("->", "");
      hname.ToLower();
      hname += "_sys_"; hname += isys;
      h->SetName(hname.Data());
      h->Write();
    }
    hbkg->Write();
    f->Flush();
  }
  return status;
}

int get_same_flavor_histogram(int set, TString selection, vector<int> years, TString base) {

  int status(0);
  TString hist = "lepm";

  //define parameters for dataplotter script
  years_ = years;
  status = nanoaod_init(selection, base, base);
  if(status) {
    cout << "DataPlotter initialization script returned " << status << ", exiting!\n";
    return status;
  }
  int set_offset = (selection == "mumu") ? ZTauTauHistMaker::kMuMu : ZTauTauHistMaker::kEE;

  dataplotter_->rebinH_ = overall_rebin_;

  //get background distribution
  THStack* hstack = dataplotter_->get_stack(hist, "event", set+set_offset);
  if(!hstack) return 1;

  //get data
  TH1D* hdata = dataplotter_->get_data(hist, "event", set+set_offset);
  if(!hdata) return 2;

  /////////////////////////////////////////////////
  // Plot the results
  /////////////////////////////////////////////////

  TCanvas* c = new TCanvas();
  hstack->Draw("hist noclear");
  hdata->Draw("same E");
  hstack->GetXaxis()->SetRangeUser(70., 150.);

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
  TH1D* hlast = (TH1D*) hstack->GetStack()->Last();
  hlast->SetName("hbackground");
  hlast->Write();
  hdata->SetName("hdata");
  hdata->Write();

  // if(do_systematics_)
  //   status += get_systematics(set+set_offset, hist, fout, canvas_name);
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

  //define parameters for dataplotter script
  TString selec = selection; selec.ReplaceAll("z", ""); selec.ReplaceAll("h", "");
  years_ = years;
  status = nanoaod_init(selec, base, base);
  if(status) {
    cout << "DataPlotter initialization script returned " << status << ", exiting!\n";
    return status;
  }
  int set_offset = ZTauTauHistMaker::kEMu;

  dataplotter_->rebinH_ = overall_rebin_;

  if(test_sys_ >= 0) dataplotter_->verbose_ = 1;

  //get background distribution
  if(set == 13 || set == 18) {dataplotter_->signal_scale_ = 20.; dataplotter_->signal_scales_["H->e#mu"] = 50.;}
  THStack* hstack = dataplotter_->get_stack(hist, "event", set+set_offset);
  if(!hstack) return 1;

  //get signal distributions
  std::vector<TH1D*> signals = dataplotter_->get_signal(hist, "event", set+set_offset);
  for(auto h : signals) {
    if(!h) return 2;
  }

  //get data
  TH1D* hdata = dataplotter_->get_data(hist, "event", set+set_offset);
  if(!hdata) return 3;

  TCanvas* c = new TCanvas();
  hstack->Draw("hist noclear");
  for(auto h : signals) {
    h->Draw("hist same");
  }
  TH1D* hdata_clone = (TH1D*) hdata->Clone("hdata_clone");
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

  if(selection == "zemu") {xmin_ = 70.; xmax_ = 110.;}
  else if(selection == "hemu") {xmin_ = 110.; xmax_ = 150.;}
  else {xmin_ = 50.; xmax_ = 170.;}

  hstack->GetXaxis()->SetRangeUser(xmin_, xmax_);
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
  TH1D* hlast = (TH1D*) hstack->GetStack()->Last();
  hlast->SetName("hbackground");
  hlast->Write();
  hbkg_ = (TH1D*) hlast->Clone("hbkg_");
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
    hname.ToLower();
    h->SetName(hname.Data());
    hsigs_.push_back((TH1D*) h->Clone(Form("hsig_%s_", hname.Data()))); //store the signal for plotting against systematic
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
                       TString base = "nanoaods_dev") {
  int status(0);
  for(int set : sets) {
    status += get_bemu_single_histogram(set, selection, years, base);
    status += get_same_flavor_histogram(set, "mumu", years, base);
    status += get_same_flavor_histogram(set, "ee", years, base);
  }
  return status;
}
