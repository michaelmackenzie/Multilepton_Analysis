//Studying signal yield changes due to ntuple changes
vector<TFile*> files_;
vector<int> nentries_;
vector<TString> names_;
int Mode_ = 0; //histogram mode
int year_ = 2016;

int compare_hist(TString hist = "eventweight", TString type = "event", int set = 8,
                 int rebin = 1, double xmin = 1., double xmax = -1., bool print = true) {
  int status(0);
  const char* selec = (Mode_ == 0) ? "etau_mu" : (Mode_ == 1) ? "mutau_e" : "emu";
  const char* signal = (Mode_ == 0) ? "ZETau" : (Mode_ == 1) ? "ZMuTau" : "ZEMu";
  if(files_.size() == 0) {
    const char* path = "/eos/uscms/store/user/mmackenz/histograms";
    files_.push_back(TFile::Open(Form("%s/nanoaods_no_emu_trig/clfv_%s_%i_%s.hist", path, selec, year_, signal), "READ"));
    names_.push_back("no e#mu");
    if(!files_.back()) return 1;
    files_.push_back(TFile::Open(Form("%s/nanoaods_emu_trig/clfv_%s_%i_%s.hist", path, selec, year_, signal), "READ"));
    names_.push_back("e#mu");
    if(!files_.back()) return 1;
    for(auto f : files_) {
      TH1* events = (TH1*) f->Get("events");
      nentries_.push_back(events->GetBinContent(1));
    }
  }
  for(auto f : files_) {
    if(!f) return 1;
  }

  TH1* hists[files_.size()];
  TH1* haxis = nullptr;
  const int colors[10] = {kBlue, kRed, kYellow-6, kGreen-2, kViolet-6,
                          kOrange, kAtlantic, kGreen, kBlue-2, kRed-6};
  if(Mode_ == 0) set += 400;
  if(Mode_ == 1) set += 300;
  if(Mode_ == 2) set += 200;
  printf("%s/%s/%i:\n", hist.Data(), type.Data(), set);
  for(unsigned index = 0; index < files_.size(); ++index) {
    auto f = files_[index];
    TH1* h = (TH1*) f->Get(Form("Data/%s_%i/%s", type.Data(), set, hist.Data()));
    if(!h) {
      printf("Histogram %s/%s/%i not found in file %s\n", hist.Data(), type.Data(), set, f->GetName());
      hists[index] = nullptr;
      continue;
    }
    h = (TH1*) h->Clone(Form("%s_%i", h->GetName(), (int) index));
    if(!haxis) haxis = h;
    hists[index] = h;
    h->SetLineColor(colors[index]);
    h->SetLineWidth(2);
    h->Scale(195400.0/nentries_[index]); //Scaling to the nominal ntuple gen size
    printf("%i: integral = %.1f, mean = %.3f, std dev = %.3f, entries = %.0f\n", index,
           h->Integral(), h->GetMean(), h->GetStdDev(), h->GetEntries());
    if(rebin > 1) h->Rebin(rebin);
  }
  if(!haxis) return 2;
  if(!print) return status;

  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas();
  TPad* pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1.0); pad1->Draw();
  TPad* pad2 = new TPad("pad2", "pad2", 0., 0.0, 1., 0.3); pad2->Draw();
  pad2->SetTopMargin(0.03);
  pad1->cd();

  TLegend* leg = new TLegend(0.75, 0.6, 0.9, 0.9);
  haxis->Draw("hist");
  double max_val = haxis->GetMaximum();
  for(unsigned index = 0; index < files_.size(); ++index) {
    auto h = hists[index];
    if(!h) continue;
    leg->AddEntry(h, Form("%s", names_[index].Data()));
    if(h == haxis) continue;
    max_val = max(max_val, h->GetMaximum());
    h->Draw("hist same");
  }
  haxis->GetYaxis()->SetRangeUser(0., 1.1*max_val);
  if(xmin < xmax) haxis->GetXaxis()->SetRangeUser(xmin, xmax);
  pad1->SetGrid();
  leg->Draw();

  pad2->cd();
  TH1* haxis_r = (TH1*) haxis->Clone(Form("%s_r", haxis->GetName()));
  haxis_r->Divide(haxis);
  haxis_r->Draw("hist");
  for(unsigned index = 0; index < files_.size(); ++index) {
    auto h = hists[index];
    if(!h) continue;
    if(h == haxis) {
      haxis_r->SetYTitle(Form("%s / %s", names_[1].Data(), names_[0].Data()));
      continue;
    }
    auto h_r = (TH1*) h->Clone(Form("%s_r", h->GetName()));
    h_r->Divide(haxis);
    h_r->Draw("hist same");
  }
  haxis_r->GetYaxis()->SetRangeUser(0.5, 1.5);
  if(xmin < xmax) haxis_r->GetXaxis()->SetRangeUser(xmin, xmax);
  haxis_r->GetXaxis()->SetLabelSize(0.1);
  haxis_r->GetYaxis()->SetLabelSize(0.1);
  haxis_r->GetYaxis()->SetTitleSize(0.13);
  haxis_r->GetYaxis()->SetTitleOffset(0.3);
  haxis_r->SetTitle("");
  pad2->SetGrid();

  const char* out_dir = Form("emu_trig_figures/%s/%i", selec, year_);
  gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s", out_dir, out_dir));
  c->SaveAs(Form("%s/%s_%s_%i.png", out_dir, type.Data(), hist.Data(), set));
  return status;
}


int compare_emu_trig(int Mode = 0, int year = 2016) {
  Mode_ = Mode;
  year_ = year;
  int status(0);
  status += compare_hist("lepm"         , "event", 8, 5,  40., 110., true);
  // status += compare_hist("mva1_1"       , "event", 8,30, -0.6,  0.5, true);
  status += compare_hist("onept"        , "lep"  , 8, 1,  10., 100., true);
  status += compare_hist("twopt"        , "lep"  , 8, 1,  10., 100., true);
  status += compare_hist("leppt"        , "event", 8, 2,   0., 100., true);
  status += compare_hist("oneeta"       , "lep"  , 8, 1, -2.4,  2.4, true);
  status += compare_hist("twoeta"       , "lep"  , 8, 1, -2.4,  2.4, true);
  status += compare_hist("lepeta"       , "event", 8, 2, -5.0,  5.0, true);
  return status;
}
