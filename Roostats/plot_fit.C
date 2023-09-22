//Plot fit results from fit diagnostics
//create a fit diagnostics root file via:
//$> combine -M FitDiagnostics -d <input card/workspace> --saveShapes --saveWithUncertainties [additional options]

int print_hist(TDirectoryFile* dir, TString tag, TString outdir) {
  TH1* hbackground = (TH1*) dir->Get("total_background");
  TH1* hsignal     = (TH1*) dir->Get("total_signal");
  TGraphAsymmErrors* gdata = (TGraphAsymmErrors*) dir->Get("data");
  if(!hsignal || !hbackground || !gdata) {
    cout << "Data not found for tag " << tag.Data() << endl;
    return 1;
  }

  const int nbins = gdata->GetN();
  if(nbins != hbackground->GetNbinsX()) {
    cout << "Data and background have different bin numbers!\n";
    return 2;
  }

  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c", "c", 1200, 900);
  TPad* pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1. );
  TPad* pad2 = new TPad("pad2", "pad2", 0., 0. , 1., 0.3);
  pad1->Draw(); pad2->Draw();
  pad1->cd();

  gdata->SetMarkerStyle(20);
  gdata->SetMarkerSize(0.8);
  gdata->SetLineWidth(2);
  hbackground->SetLineColor(kRed);
  hbackground->SetMarkerColor(kRed);
  hbackground->SetLineWidth(2);
  hbackground->SetTitle("");
  hbackground->SetXTitle("");
  hsignal->SetLineColor(kBlue);
  hsignal->SetMarkerColor(kBlue);
  hsignal->SetLineWidth(2);

  hbackground->Draw("hist E1");
  hsignal->Draw("hist same");
  gdata->Draw("P");
  hbackground->GetYaxis()->SetRangeUser(0.1, 1.2*max(hbackground->GetMaximum(), hsignal->GetMaximum()));
  pad2->cd();

  TGraphAsymmErrors* gRatio = (TGraphAsymmErrors*) gdata->Clone("gRatio"); //data / s
  //Set the data/background ratio values
  for(int bin = 0; bin < nbins; ++bin) {
    //
    double x, y;
    gdata->GetPoint(bin, x, y);
    double err_high = gdata->GetErrorYhigh(bin);
    double err_low  = gdata->GetErrorYlow (bin);
    double bkg_v = hbackground->GetBinContent(bin+1);
    double bkg_e = hbackground->GetBinError  (bin+1);

    double val = (bkg_v > 0.) ? y / bkg_v : 0.;
    double val_high = (bkg_v > 0.) ? sqrt(err_high*err_high + bkg_e*bkg_e) / bkg_v: 0.;
    double val_low  = (bkg_v > 0.) ? sqrt(err_low *err_low  + bkg_e*bkg_e) / bkg_v: 0.;

    gRatio->SetPoint(bin, x, val);
    gRatio->SetPointEYhigh(bin, val_high);
    gRatio->SetPointEYlow (bin, val_low );
  }
  gRatio->Draw("AP");
  TH1* hRatio_s = (TH1*) hsignal->Clone("hRatio_s");
  hRatio_s->Add(hbackground);
  hRatio_s->Divide(hbackground);
  for(int ibin = 1; ibin <= hsignal->GetNbinsX(); ++ibin) {if(hbackground->GetBinContent(ibin) <= 0.1) hRatio_s->SetBinContent(ibin, 0.);}
  hRatio_s->Draw("hist same");
  TLine* line = new TLine(0., 1., nbins, 1.);
  line->SetLineColor(kRed);
  line->SetLineWidth(2);
  line->Draw("same");

  gRatio->GetXaxis()->SetRangeUser(0., nbins);
  gRatio->GetYaxis()->SetRangeUser(0.8, 1.2);
  gRatio->SetTitle(";Bins;Data/Bkg");
  gRatio->GetXaxis()->SetLabelSize(0.08);
  gRatio->GetYaxis()->SetLabelSize(0.08);
  gRatio->GetXaxis()->SetTitleSize(0.1);
  gRatio->GetYaxis()->SetTitleSize(0.1);

  c->SaveAs(Form("%s%s.png", outdir.Data(), tag.Data()));
  pad1->SetLogy();
  hbackground->GetYaxis()->SetRangeUser(0.1, 2.*max(hbackground->GetMaximum(), hsignal->GetMaximum()));
  c->SaveAs(Form("%s%s_logy.png", outdir.Data(), tag.Data()));
  delete c;
  delete gRatio;

  return 0;
}

int print_dir(TDirectoryFile* dir, TString tag, TString outdir) {
  int status(0);
  if(!dir) return 1;
  TList* list = dir->GetListOfKeys();
  if(!list) return 10;
  bool subdir(false); //whether there are sub-directories or not
  for(TObject* o : *list) {
    TObject* obj = dir->Get(o->GetName());
    if(!obj) continue;
    bool isdir = obj->InheritsFrom(TDirectoryFile::Class());
    if(isdir) {
      status += print_dir((TDirectoryFile*) obj, (tag + "_") + obj->GetName(), outdir);
      subdir = true;
    }
  }
  if(!subdir) status += print_hist(dir, tag, outdir); //histogram directory
  return status;
}

int plot_fit(TString fname, TString outdir = "figures") {
  TFile* file = TFile::Open(fname.Data(), "READ");
  if(!file) return 1;

  if(!outdir.EndsWith("/")) outdir += "/";

  TDirectoryFile *prefit = (TDirectoryFile*) file->Get("shapes_prefit");
  TDirectoryFile *fit_b  = (TDirectoryFile*) file->Get("shapes_fit_b");
  TDirectoryFile *fit_s  = (TDirectoryFile*) file->Get("shapes_fit_s");

  if(!prefit || !fit_b || !fit_s) {
    cout << "Fit directories not found!\n";
    return 2;
  }

  gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s", outdir.Data(), outdir.Data()));

  int status(0);
  status += print_dir(prefit, "prefit", outdir);
  status += print_dir(fit_b , "fit_b" , outdir);
  status += print_dir(fit_s , "fit_s" , outdir);

  cout << "Plotting status = " << status << endl;
  return status;
}
