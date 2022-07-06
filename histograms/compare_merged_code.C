//Compare a few example distributions before/after merging the ntupling code
TString path_ = "root://cmseos.fnal.gov//store/user/mmackenz/histograms";
TString old_hist_ = "nanoaods_premerge_05_27_22";
TString new_hist_ = "nanoaods_02";
TString selection_;
int year_;

using namespace CLFV;

vector<TFile*> data_files(bool orig) {
  vector<TFile*> files;
  if(orig) {
    if(selection_.Contains("mu"))
      files.push_back(TFile::Open(Form("%s/%s/clfv_%s_clfv_%i_SingleMu.hist", path_.Data(), old_hist_.Data(), selection_.Data(), year_), "READ"));
    if(selection_.Contains("e"))
      files.push_back(TFile::Open(Form("%s/%s/clfv_%s_clfv_%i_SingleEle.hist", path_.Data(), old_hist_.Data(), selection_.Data(), year_), "READ"));
  } else {
    vector<TString> runs;
    if     (year_ == 2016) runs = {"B", "C", "D", "E", "F", "G", "H"};
    else if(year_ == 2017) runs = {"B", "C", "D", "E", "F"};
    else if(year_ == 2018) runs = {"A", "B", "C", "D"};
    for(TString run : runs) {
      if(selection_.Contains("mu"))
        files.push_back(TFile::Open(Form("%s/%s/clfv_%s_%i_SingleMuon-%s.hist", path_.Data(), new_hist_.Data(), selection_.Data(), year_, run.Data()), "READ"));
      if(selection_.Contains("e"))
        files.push_back(TFile::Open(Form("%s/%s/clfv_%s_%i_SingleElectron-%s.hist", path_.Data(), new_hist_.Data(), selection_.Data(), year_, run.Data()), "READ"));
    }
  }
  return files;
}

TH1* get_data_hist(TString hist, TString type, int set, vector<TFile*> files) {
  TH1* h = nullptr;
  for(TFile* f : files) {
    TH1* htmp = (TH1*) f->Get(Form("Data/%s_%i/%s", type.Data(), set, hist.Data()));
    if(!htmp) {
      cout << "Histogram " << hist.Data() << "/" << type.Data() << "/" << set
           << " not found in file " << f->GetName() << endl;
      return nullptr;
    }
    if(!h) h = (TH1*) htmp->Clone(Form("%s_%s_%i", hist.Data(), type.Data(), set));
    else h->Add(htmp);
    delete htmp;
  }
  h->Sumw2();
  return h;
}

int compare_data_hists(TString hist, TString type, int set, int rebin, double xmin, double xmax,
                       TString title, TString xtitle,
                       vector<TFile*> old_files, vector<TFile*> new_files) {

  TH1* hold = get_data_hist(hist, type, set, old_files);
  if(!hold) return 3;
  hold->SetName(Form("%s_old", hold->GetName()));
  TH1* hnew = get_data_hist(hist, type, set, new_files);
  if(!hnew) return 3;
  hnew->SetName(Form("%s_new", hnew->GetName()));

  if(rebin > 1) {
    hold->Rebin(rebin);
    hnew->Rebin(rebin);
  }

  TCanvas* c = new TCanvas("c", "c", 1000, 900);
  TPad* pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1.0); pad1->Draw();
  TPad* pad2 = new TPad("pad2", "pad2", 0., 0.0, 1., 0.3); pad2->Draw();
  pad1->SetBottomMargin(0.05);
  pad2->SetTopMargin(0.03);
  pad2->SetBottomMargin(0.25);

  pad1->cd();
  hold->Draw("hist");
  hold->SetLineWidth(2);
  hnew->Draw("same E1");
  hnew->SetMarkerStyle(20);
  hnew->SetMarkerSize(0.8);
  hnew->SetMarkerColor(kRed);
  hnew->SetLineColor(kRed);
  hnew->SetLineWidth(2);
  hold->GetYaxis()->SetRangeUser(0.1, 1.1*std::max(hold->GetMaximum(), hnew->GetMaximum()));
  hold->SetTitle(title.Data());
  hold->SetXTitle("");
  if(xmin < xmax) hold->GetXaxis()->SetRangeUser(xmin, xmax);
  pad1->SetGrid();

  pad2->cd();
  TH1* hratio = (TH1*) hold->Clone("hratio");
  hratio->Divide(hnew);
  hratio->Draw("E1");
  hratio->GetYaxis()->SetRangeUser(0.5, 1.5);
  hratio->SetTitle("");
  hratio->SetXTitle(xtitle.Data());
  hratio->SetYTitle("Old / New");
  hratio->GetXaxis()->SetLabelSize(0.08);
  hratio->GetYaxis()->SetLabelSize(0.08);
  hratio->GetXaxis()->SetTitleSize(0.1);
  hratio->GetYaxis()->SetTitleSize(0.1);
  hratio->GetYaxis()->SetTitleOffset(0.5);
  TLine* line = new TLine((xmin < xmax) ? xmin : hratio->GetXaxis()->GetXmin(), 1., (xmin < xmax) ? xmax : hratio->GetXaxis()->GetXmax(), 1.);
  line->SetLineColor(kRed);
  line->SetLineWidth(2);
  line->Draw("same");
  pad2->SetGrid();

  c->SaveAs(Form("figures/compare_merged_code/%i/%s_%s_%s_%i.png", year_, selection_.Data(), hist.Data(), type.Data(), set));
  delete hold;
  delete hnew;
  delete hratio;
  delete pad1;
  delete pad2;
  delete c;

  return 0;
}

int compare_merged_code(TString selection = "emu", int year = 2016) {
  selection_ = selection;
  year_ = year;

  vector<TFile*> old_data = data_files(true);
  vector<TFile*> new_data = data_files(false);

  if(old_data.size() == 0 || new_data.size() == 0) {
    cout << "Not all data found\n";
    return 1;
  }
  for(TFile* f : old_data) {
    if(!f) return 2;
  }
  for(TFile* f : new_data) {
    if(!f) return 2;
  }

  gSystem->Exec(Form("mkdir -p figures/compare_merged_code/%i/", year_));
  gStyle->SetOptStat(0);
  int status = 0;
  int offset = 0;
  if     (selection_ == "emu"  ) offset = CLFVHistMaker::kEMu;
  else if(selection_ == "etau" ) offset = CLFVHistMaker::kETau;
  else if(selection_ == "mutau") offset = CLFVHistMaker::kMuTau;

  status += compare_data_hists("onept"  , "lep"  , offset + 8, 2, 10., 100., "", "p_{T}^{1}"        , old_data, new_data);
  status += compare_data_hists("twopt"  , "lep"  , offset + 8, 2, 10., 100., "", "p_{T}^{2}"        , old_data, new_data);
  status += compare_data_hists("leppt"  , "event", offset + 8, 2,  0., 100., "", "p_{T}^{ll}"       , old_data, new_data);
  status += compare_data_hists("met"    , "event", offset + 8, 1,  0.,  70., "", "MET"              , old_data, new_data);
  status += compare_data_hists("mtone"  , "event", offset + 8, 2,  0., 100., "", "M_{T}(l_{1},MET)" , old_data, new_data);
  status += compare_data_hists("mttwo"  , "event", offset + 8, 2,  0., 100., "", "M_{T}(l_{2},MET)" , old_data, new_data);
  status += compare_data_hists("njets20", "event", offset + 8, 1,  0.,   5., "", "N(jets)"          , old_data, new_data);
  status += compare_data_hists("jetpt"  , "event", offset + 8, 1, 20., 120., "", "Leading jet p_{T}", old_data, new_data);
  if(selection_ != "emu")
    status += compare_data_hists("lepm" , "event", offset + 8, 5, 50., 170., "", "M_{ll}"           , old_data, new_data);

  return status;
}
