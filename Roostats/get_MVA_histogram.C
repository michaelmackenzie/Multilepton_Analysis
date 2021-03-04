//Script to retrieve the background and signal MVA histograms
#include "../histograms/dataplotter_ztautau.C"
#include "../interface/SystematicHist_t.hh"

int get_systematics(int set, TString hist, TFile* f, TString canvas_name) {
  int status(0);
  f->cd();
  dataplotter_->rebinH_ = 1;
  for(int isys = 0; isys < kMaxSystematics; ++isys) {
    THStack* hstack = dataplotter_->get_stack(Form("%s_%i", hist.Data(), isys), "systematic", set);
    if(!hstack) {++status; continue;}
    if(!hstack->GetStack()) {++status; continue;}
    TH1D* hbkg = (TH1D*) hstack->GetStack()->Last();
    hbkg->SetName(Form("hbkg_sys_%i", isys));
    vector<TH1D*> signals = dataplotter_->get_signal(Form("%s_%i", hist.Data(), isys), "systematic", set);
    if(signals.size() == 0) {++status; continue;}
    TCanvas* c = new TCanvas(Form("c_sys_%i", isys), Form("c_sys_%i", isys));
    hstack->Draw("hist noclear");
    for(auto h : signals) {
      h->Draw("hist same");
    }
    gSystem->Exec(Form("mkdir -p %s_sys", canvas_name.Data()));
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

int get_individual_MVA_histogram(int set = 8, TString selection = "zmutau",
                                 vector<int> years = {2016/*, 2017, 2018*/},
                                 TString base = "nanoaods_dev") {

  int status(0);
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

  //define parameters for dataplotter script
  TString selec = selection; selec.ReplaceAll("z", ""); selec.ReplaceAll("h", "");
  years_ = years;
  status = nanoaod_init(selec, base, base);
  if(status) {
    cout << "DataPlotter initialization script returned " << status << ", exiting!\n";
    return status;
  }
  int set_offset = ZTauTauHistMaker::kEMu;
  if     (selec == "mutau") set_offset = ZTauTauHistMaker::kMuTau;
  else if(selec == "etau" ) set_offset = ZTauTauHistMaker::kETau;

  dataplotter_->rebinH_ = 100;
  //get background distribution
  THStack* hstack = dataplotter_->get_stack(hist, "event", set+set_offset);
  if(!hstack) return 1;

  std::vector<TH1D*> signals = dataplotter_->get_signal(hist, "event", set+set_offset);
  for(auto h : signals) {
    if(!h) return 2;
  }

  TCanvas* c = new TCanvas();
  hstack->Draw("hist noclear");
  for(auto h : signals) {
    h->Draw("same");
  }
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
  for(auto h : signals) {
    TString hname = h->GetName();
    if(dataplotter_->signal_scales_.find(hname) != dataplotter_->signal_scales_.end())
      h->Scale(1./dataplotter_->signal_scales_[hname]);
    else
      h->Scale(1./dataplotter_->signal_scale_);
    hname.ReplaceAll("#", "");
    hname.ReplaceAll("->", "");
    hname.ToLower();
    h->SetName(hname.Data());
    h->Write();
  }
  status += get_systematics(set+set_offset, hist, fout, canvas_name);
  fout->Close();
  return status;
}

int get_MVA_histogram(int set = 8, TString selection = "zmutau",
                      vector<int> years = {2016, 2017, 2018},
                      TString base = "nanoaods_dev") {
  int status(0);
  status += get_individual_MVA_histogram(set, selection, years, base);
  if(selection.Contains("mutau"))
    status += get_individual_MVA_histogram(set, selection+"_e", years, base);
  else if(selection.Contains("etau"))
    status += get_individual_MVA_histogram(set, selection+"_mu", years, base);
  return status;
}
