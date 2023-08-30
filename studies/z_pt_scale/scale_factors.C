//Script to define scale factors to apply to MC
//to correct the Z pT vs Mass distribution
#include "histograms/datacards.C"

//use the dataplotter to manage normalizations and initializations
DataPlotter* dataplotter_ = 0;
int verbose_           = 0;
bool rebin_            = false; //rebin fixed with histogram to variable width
bool rebinGen_         = false; //rebin for the gen-level histograms
bool useCorrectedHist_ = false; //use the MC histogram with the Drell-Yan weights
bool useOnlyPt_        = false; //don't fit in mass --> Make 1 bin for entire range

TString selection_;

//----------------------------------------------------------------------------------------------------------------------------------
//set reasonable z-range
void set_z_range(TH2* h, const double min_default = 0.8, const double max_default = 1.2) {
  double min_z(1.e20), max_z(-1.e20);
  for(int xbin = 1; xbin <= h->GetNbinsX(); ++xbin) {
    for(int ybin = 1; ybin <= h->GetNbinsY(); ++ybin) {
      const double binc = h->GetBinContent(xbin, ybin);
      if(binc > 1.e-4) min_z = std::min(min_z, binc);
      max_z = std::max(max_z, binc);
    }
  }
  if(min_z < max_z) {
    min_z -= 0.1*(max_z - min_z);
    max_z += 0.1*(max_z - min_z);
    h->GetZaxis()->SetRangeUser(min_z, max_z);
  }
  else h->GetZaxis()->SetRangeUser(min_default, max_default);
}


//----------------------------------------------------------------------------------------------------------------------------------
//draw a clone of a 2D histogram that shows the bottom bin
void draw_2d_log(TH2* h, TString opt, double zmin = 1., double zmax = -1.) {
  const double y1 = h->GetYaxis()->GetBinLowEdge(1);
  if(y1 > 1e-3) {h->Draw(opt.Data()); return;}
  const double binw = h->GetYaxis()->GetBinWidth(1);
  vector<double> ys = {y1 + binw*0.4};
  for(int ibin = 1; ibin <= h->GetNbinsY(); ++ibin) {
    ys.push_back(h->GetYaxis()->GetBinLowEdge(ibin) + h->GetYaxis()->GetBinWidth(ibin));
  }
  const double* xbins = h->GetXaxis()->GetXbins()->GetArray();
  TH2* h2 = new TH2D(Form("%s_tmp", h->GetName()), h->GetTitle(), h->GetNbinsX(), xbins, h->GetNbinsY(), &ys[0]);
  for(int xbin = 1; xbin <= h->GetNbinsX(); ++xbin) {
    for(int ybin = 1; ybin <= h->GetNbinsY(); ++ybin) {
      h2->SetBinContent(xbin, ybin, h->GetBinContent(xbin, ybin));
      h2->SetBinError(xbin, ybin, h->GetBinError(xbin, ybin));
    }
  }
  h2->SetXTitle(h->GetXaxis()->GetTitle());
  h2->SetYTitle(h->GetYaxis()->GetTitle());
  if(zmin > zmax) set_z_range(h2);
  else h2->GetZaxis()->SetRangeUser(zmin, zmax);
  h2->GetYaxis()->SetMoreLogLabels(kTRUE);
  h2->Draw(opt.Data());
}

//----------------------------------------------------------------------------------------------------------------------------------
// make a 1D stack plot
void print_stack(TString hist, TString type, int set, bool useMuon, int year) {
  THStack* hstack = dataplotter_->get_stack(hist, type, set);
  TH1*     hdata  = dataplotter_->get_data(hist, type, set);
  if(!hstack || !hdata) return nullptr;
  TCanvas* c = new TCanvas(Form("c_%s_%s_%i", hist.Data(), type.Data(), set),
                           Form("c_%s_%s_%i", hist.Data(), type.Data(), set),
                           1000, 1200);
  TPad* pad1 = new TPad("pad1", "pad1", 0., 0.25, 1., 1.0 ); pad1->Draw();
  TPad* pad2 = new TPad("pad2", "pad2", 0., 0.  , 1., 0.25); pad2->Draw();
  pad1->SetBottomMargin(0.04);
  pad2->SetTopMargin(0.03);

  pad1->cd();
  hstack->Draw("hist noclear");
  hdata->Draw("E same");
  hstack->SetMaximum(1.2*std::max(hstack->GetMaximum(), hdata->GetMaximum()));
  hstack->SetTitle("");
  pad1->SetGrid();

  pad2->cd();
  TH1* hratio = (TH1*) hdata->Clone("hratio");
  hratio->Divide((TH1*) hstack->GetStack()->Last());
  hratio->SetTitle("");
  hratio->Draw("E");
  hratio->GetYaxis()->SetRangeUser(0.5, 1.5);
  hratio->GetXaxis()->SetLabelSize(0.1);
  hratio->GetYaxis()->SetLabelSize(0.1);
  TLine* line = new TLine(hratio->GetXaxis()->GetXmin(), 1., hratio->GetXaxis()->GetXmax(), 1.);
  line->SetLineColor(kRed);
  line->SetLineWidth(2);
  line->Draw("same");
  hratio->Draw("E same");
  pad2->SetGrid();
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);

  if(c) {
    c->Print(Form("figures/%i/s_%s_%s_%i.png", years_[0], (useMuon) ? "mumu" : "ee", hist.Data(), year));
    pad1->SetLogy();
    c->Print(Form("figures/%i/s_%s_%s_%i_log.png", years_[0], (useMuon) ? "mumu" : "ee", hist.Data(), year));
    delete c;
  } else cout << "Canvas not found for stack " << hist.Data() << "/" << type.Data() << "/" << set << endl;
}

//----------------------------------------------------------------------------------------------------------------------------------
// make a plot of the di-lepton pT with an uncertainty band
void print_unc_band(int set, bool useMuon, int year, int bin = -1) {
  TString hist("lepptvsm0"), sys("lepptvsm3"), type("event");
  dataplotter_->rebinH_ = 0;
  THStack* hsys2D  = dataplotter_->get_stack(sys , type, set);
  THStack* hbkg2D  = dataplotter_->get_stack(hist, type, set);
  TH2*     hdata2D = (TH2*) dataplotter_->get_data(hist, type, set);
  if(!hbkg2D || !hsys2D || !hdata2D) return nullptr;
  //Project into the pT dimension
  const int nx = hdata2D->GetNbinsX();
  int bin_low(1), bin_high(nx);
  if(bin > 0 && bin <= nx) {bin_low = bin; bin_high = bin;} //if projecting a single bin
  TH1* hsys  = ((TH2*) hsys2D->GetStack()->Last())->ProjectionY("_sys_py", bin_low, bin_high);
  TH1* hbkg  = ((TH2*) hbkg2D->GetStack()->Last())->ProjectionY("_bkg_py", bin_low, bin_high);
  TH1* hdata = hdata2D->ProjectionY("_data_py", bin_low, bin_high);
  if(!hbkg || !hsys || !hdata) return nullptr;
  TH1* hsys_down = (TH1*) hbkg->Clone(Form("%s_down", hsys->GetName()));
  hsys_down->Add(hbkg); hsys_down->Add(hsys, -1.); //hbkg + (hbkg - hsys) = 2*hbkg - hsys
  cout << __func__ << ": Data / MC = " << hdata->Integral() << " / " << hbkg->Integral()
       << " (" << hsys->Integral() << " - " << hsys_down->Integral() << ")\n";
  TCanvas* c = new TCanvas(Form("c_unc_band_%i", set),
                           Form("c_unc_band_%i", set),
                           1000, 1200);
  TPad* pad1 = new TPad("pad1", "pad1", 0., 0.25, 1., 1.0 ); pad1->Draw();
  TPad* pad2 = new TPad("pad2", "pad2", 0., 0.  , 1., 0.25); pad2->Draw();
  pad1->SetBottomMargin(0.04);
  pad2->SetTopMargin(0.03);

  pad1->cd();
  hbkg     ->SetLineWidth(2); hbkg     ->SetLineColor(kBlue); hbkg     ->SetFillColor(0);
  hsys     ->SetLineWidth(2); hsys     ->SetLineColor(kRed ); hsys     ->SetFillColor(0);
  hsys_down->SetLineWidth(2); hsys_down->SetLineColor(kRed ); hsys_down->SetFillColor(0);
  hbkg->Draw("hist");
  hsys->Draw("hist same");
  hsys_down->Draw("hist same");
  hdata->Draw("E same");
  double max_val = std::max(hdata->GetMaximum(), std::max(hsys->GetMaximum(), hsys_down->GetMaximum()));
  hbkg->GetYaxis()->SetRangeUser(0.1, 1.2*max_val);
  hbkg->SetTitle("");
  pad1->SetGrid();

  pad2->cd();
  TH1* hratio = (TH1*) hdata->Clone("hratio");
  hratio->Divide(hbkg);
  TH1* hup_r = (TH1*) hdata->Clone("hup_r");
  hup_r->Divide(hsys); hup_r->SetLineColor(kRed); hup_r->SetLineWidth(2); hup_r->SetFillColor(0);
  TH1* hdown_r = (TH1*) hdata->Clone("hdown_r");
  hdown_r->Divide(hsys_down); hdown_r->SetLineColor(kRed); hdown_r->SetLineWidth(2); hdown_r->SetFillColor(0);
  hratio->SetTitle("");
  hratio->Draw("E");
  hup_r->Draw("hist same");
  hdown_r->Draw("hist same");
  TLine* line = new TLine(hratio->GetXaxis()->GetXmin(), 1., hratio->GetXaxis()->GetXmax(), 1.);
  line->SetLineColor(kRed);
  line->SetLineWidth(2);
  line->SetLineStyle(kDashed);
  line->Draw("same");
  hratio->Draw("E same");
  hratio->GetYaxis()->SetRangeUser(0.7, 1.3);
  hratio->GetXaxis()->SetLabelSize(0.1);
  hratio->GetYaxis()->SetLabelSize(0.1);
  pad2->SetGrid();
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);

  pad1->SetLogx();
  pad2->SetLogx();

  //Add the mass range to the plot
  pad1->cd();
  TLatex label;
  label.SetNDC();
  label.SetTextFont(72);
  label.SetTextSize(0.04);
  label.SetTextAlign(13);
  label.DrawLatex(0.13, 0.85,  Form("%.1f < M_{ll} < %.1f",
                                    hdata2D->GetXaxis()->GetBinLowEdge(bin_low),
                                    hdata2D->GetXaxis()->GetBinLowEdge(bin_high) + hdata2D->GetXaxis()->GetBinWidth(bin_high)));


  if(c) {
    //check if doing a single bin
    if(bin > 0 && bin <= nx) {
      c->Print(Form("figures/%i/%s_leppt_unc_band_%i/bin_%i.png", years_[0], (useMuon) ? "mumu" : "ee", year, bin));
      pad1->SetLogy();
      hbkg->GetYaxis()->SetRangeUser(1.e-4*max_val, 5.*max_val);
      c->Print(Form("figures/%i/%s_leppt_unc_band_%i/bin_%i_log.png", years_[0], (useMuon) ? "mumu" : "ee", year, bin));
      delete c;
      delete hbkg, hdata, hsys, hsys_down, hratio, hup_r, hdown_r, hdata2D, hbkg2D, hsys2D, pad1, pad2;
    } else {
      c->Print(Form("figures/%i/%s_leppt_unc_band_%i.png", years_[0], (useMuon) ? "mumu" : "ee", year));
      pad1->SetLogy();
      hbkg->GetYaxis()->SetRangeUser(1.e-4*max_val, 5.*max_val);
      c->Print(Form("figures/%i/%s_leppt_unc_band_%i_log.png", years_[0], (useMuon) ? "mumu" : "ee", year));
      delete c;
      delete hbkg, hdata, hsys, hsys_down, hratio, hup_r, hdown_r, hdata2D, hbkg2D, hsys2D, pad1, pad2;
      //print each bin's projection
      const char* dir = Form("figures/%i/%s_leppt_unc_band_%i", years_[0], (useMuon) ? "mumu" : "ee", year);
      gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s", dir, dir));
      for(int ibin = 1; ibin <= nx; ++ibin) print_unc_band(set, useMuon, year, ibin);
    }
  } else cout << "Canvas not found for uncertainy band in set " << set << " (bin = " << bin << ")\n";
}

//----------------------------------------------------------------------------------------------------------------------------------
//create a histogram with variable bin widths from fixed with histogram
TH2* create_rebinned_histogram(TH2* h, const int& nbinsx, const double* xbins,
                                const int& nbinsy, const double* ybins) {
  if(!h) return NULL;
  TH2* hBinned = new TH2D(Form("%s_binned", h->GetName()), h->GetTitle(), nbinsx, xbins, nbinsy, ybins);
  int nx(0);
  for(int ix = 0; ix <= h->GetNbinsX()+1; ++ix) {
    if(ix == 0) nx = 0; //underflow
    else if(ix == h->GetNbinsX()+1) nx = nbinsx+1; //overflow
    else if(nx >= nbinsx+1) nx = nbinsx+1;
    else {
      while(h->GetXaxis()->GetBinLowEdge(ix) >= xbins[nx]) ++nx;
    }
    if(nx >= nbinsx+1) nx = nbinsx+1;
    int ny(0);
    for(int iy = 0; iy <= h->GetNbinsY()+1; ++iy) {
      if(iy == 0) ny = 0;
      else if(iy == h->GetNbinsY()+1) ny = nbinsy+1; //overflow
      else if(ny >= nbinsy+1) ny = nbinsy+1;
      else {
        while(h->GetYaxis()->GetBinLowEdge(iy) >= ybins[ny]) ++ny;
      }
      if(ny >= nbinsy+1) ny = nbinsy+1;
      double binc  = hBinned->GetBinContent(nx,ny);
      double bine  = hBinned->GetBinError(nx,ny);
      double origc = h->GetBinContent(ix,iy);
      double orige = h->GetBinError(ix,iy);
      hBinned->SetBinContent(nx, ny, binc+origc);
      hBinned->SetBinError(nx, ny, sqrt(bine*bine+orige*orige));
      if(verbose_ > 1) cout << "Bin old: (" << ix << ", " << iy << ") = "
                            << origc << "+-" << orige
                            <<" bin new: ("
                            << nx << ", " << ny << ") = "
                            << hBinned->GetBinContent(nx,ny) << "+-" << hBinned->GetBinError(nx,ny)
                            << endl
                            << "Bin range old: " << h->GetXaxis()->GetBinLowEdge(ix) << " < x < "
                            << h->GetXaxis()->GetBinLowEdge(ix)+h->GetXaxis()->GetBinWidth(ix)
                            << " " << h->GetYaxis()->GetBinLowEdge(iy) << " < y < "
                            << h->GetYaxis()->GetBinLowEdge(iy)+h->GetYaxis()->GetBinWidth(iy) << endl
                            << "Bin range new: " << hBinned->GetXaxis()->GetBinLowEdge(nx) << " < x < "
                            << hBinned->GetXaxis()->GetBinLowEdge(nx)+hBinned->GetXaxis()->GetBinWidth(nx)
                            << " " << hBinned->GetYaxis()->GetBinLowEdge(ny) << " < y < "
                            << hBinned->GetYaxis()->GetBinLowEdge(ny)+hBinned->GetYaxis()->GetBinWidth(ny) << endl;
    }
  }
  if(verbose_ > 0) cout << "Integral before rebinning = " << h->Integral()
                        << ", integral after rebinning = " << hBinned->Integral() << endl;
  return hBinned;
}

//----------------------------------------------------------------------------------------------------------------------------------
void get_pt_binning(double* binning, int &nbins, int type = 0) {
  if(type == 0) { // reco info
    nbins = 58;
    double bins[] = { 0.  , 0.5 , 1.  , 1.5 , 2.  ,
                      2.5 , 2.75, 3.  , 3.25, 3.5 ,
                      3.75, 4.  , 4.25, 4.5 , 4.75,
                      5.  , 5.25, 5.5 , 5.75, 6.  ,
                      6.25, 6.5 , 6.75, 7.  , 7.25,
                      7.5 , 7.75, 8.  , 8.5 , 9.  ,
                      9.5 , 10. , 10.5, 11. , 11.5,
                      12. , 12.5, 13. , 13.5, 14. ,
                      14.5, 15. , 16. , 17. , 18. ,
                      19. , 20. , 22. , 24. , 26. ,
                      28. , 30. , 33. , 43. , 60. ,
                      80. , 105., 150.,
                      1000.};
    for(int bin = 0; bin <= nbins; ++bin)
      binning[bin] = bins[bin];
  } else if (type == 1 || type == 2) { //Gen-level info
    nbins = 66;
    const double bins[] = { 0.  , 0.5 , 1.  , 1.5 , 2.  ,
                            2.5 , 2.75, 3.  , 3.25, 3.5 ,
                            3.75, 4.  , 4.25, 4.5 , 4.75,
                            5.  , 5.25, 5.5 , 5.75, 6.  ,
                            6.25, 6.5 , 6.75, 7.  , 7.25,
                            7.5 , 7.75, 8.  , 8.5 , 9.  ,
                            9.5 , 10. , 10.5, 11. , 11.5,
                            12. , 12.5, 13. , 13.5, 14. ,
                            14.5, 15. , 16. , 17. , 18. ,
                            19. , 20. , 22. , 24. , 26. ,
                            28. , 30. , 33. , 36. , 40. ,
                            43. , 46. , 50. , 55. , 60. ,
                            65. , 70. , 75. , 80. , 88. ,
                            105.,
                            1000.};
    for(int bin = 0; bin <= nbins; ++bin)
      binning[bin] = bins[bin];

  } else
    cout << "ERROR! Undefined pT binning type " << type << endl;
}

//----------------------------------------------------------------------------------------------------------------------------------
void get_mass_binning(double* binning, int &nbins, int type = 0) {
  if(useOnlyPt_) { //set mass to be 1 bin
    nbins = 1;
    binning[0] = 0.;
    binning[1] = 1000.;
    return;
  }
  if(type == 0) { //reco info
      nbins = 60;
      double bins[] = { 0.   , 60.  , 70.  , 74.  , 77.  ,
                        79.  , 81.  , 82.  , 83.  , 84.  ,
                        84.75, 85.5 , 86.  , 86.5 , 87.  ,
                        87.5 , 87.75, 88.  , 88.25, 88.5 ,
                        88.75, 89.  , 89.25, 89.5 , 89.75,
                        90.  , 90.15, 90.3 , 90.45, 90.6 ,
                        90.75, 90.9 , 91.05, 91.2 , 91.35,
                        91.5 , 91.75, 92.  , 92.25, 92.5 ,
                        92.75, 93.  , 93.25, 93.5 , 94.  ,
                        94.5 , 95.  , 95.5 , 96.  , 96.5 ,
                        97.  , 98.  , 99.  , 100. , 103. ,
                        106. , 110. , 115. , 120. , 130. ,
                        1000.};
    for(int bin = 0; bin <= nbins; ++bin)
      binning[bin] = bins[bin];
  } else if(type == 1 || type == 2) { //gen-level info
    nbins = 55;
    const double bins[] = { 0.   , 55.  , 60.  , 65.  , 70.  ,
                            74.  , 77.  , 79.  , 81.  , 82.  ,
                            83.  , 84.  , 84.75, 85.5 , 86.  ,
                            86.5 , 87.  , 87.5 , 87.75, 88.  ,
                            88.25, 88.5 , 88.75, 89.  , 89.25,
                            89.5 , 89.75, 90.  , 90.3 , 90.6 ,
                            90.9 , 91.2 , 91.5 , 91.75, 92.  ,
                            92.25, 92.5 , 92.75, 93.  , 93.25,
                            93.5 , 94.  , 94.5 , 95.  , 95.5 ,
                            96.  , 96.5 , 97.  , 98.  , 99.  ,
                            100. , 103. , 106. , 110. , 115. ,
                            1000.};
    for(int bin = 0; bin <= nbins; ++bin)
      binning[bin] = bins[bin];
  } else
    cout << "ERROR! Undefined mass binning type " << type << endl;
}

//----------------------------------------------------------------------------------------------------------------------------------
TH2* get_histogram(int set, int type = 0, bool isdata = false) {
  TH2* h = 0;
  unsigned nfiles = dataplotter_->inputs_.size();
  TString hname;

  bool bkg = false;
  if(type < 0 and !isdata) {bkg = true; type = abs(type) - 1;}

  if(type == 0)       hname = (!isdata && useCorrectedHist_) ? "lepptvsm2" : "lepptvsm1"; //MC
  else if(type == 1)  hname = (useCorrectedHist_) ? "zptvsm0" : "zptvsm1"; //Uncorrected Gen-level histogram
  else if(type == 2)  hname = "zptvsm2"; //Corrected with reco scale factors Gen-level histogram
  else if(type == 3)  hname = "lepptvsm0"; //corrected reco with gen weights
  //Electron channel systematic histograms
  else if(type == 10) hname = (useCorrectedHist_) ? "lepptvsm4" : "lepptvsm1"; //MC
  else if(type == 11) hname = (useCorrectedHist_) ? "zptvsm3" : "zptvsm1"; //Uncorrected Gen-level histogram
  else if(type == 12) hname = "zptvsm4"; //Corrected with reco scale factors Gen-level histogram
  else if(type == 13) hname = "lepptvsm3"; //corrected reco with gen weights

  if(verbose_ > 0) cout << "Getting 2D MC histograms named " << hname.Data() << " with bkg = " << bkg << endl;

  for(unsigned d = 0; d < nfiles; ++d) {
    auto input = dataplotter_->inputs_[d];
    if(input.isData_ && !isdata) continue; //get data separately
    else if(!input.isData_ && isdata) continue;
    TString hpath = Form("event_%i/%s", set, hname.Data());
    const bool isDY = input.name_.Contains("DY");
    if(!isdata && ((bkg && isDY) || (!bkg && !isDY))) continue; //if getting background, skip DY, else skip non-DY
    if(verbose_ > 0) cout << "Retrieving histogram " << hpath.Data() << " for " << input.name_.Data()
                          << " with bkg = " << bkg << ", isDY = " << isDY << " and isdata = " << isdata << endl;
    TH2* hTmp = (TH2*) input.data_->Get(hpath.Data());
    if(!hTmp) {
      if(verbose_ > 0) cout << "Histogram " << hname.Data() << " for " << input.name_.Data() << " not found!\n";
      continue;
    }
    hTmp = (TH2*) hTmp->Clone(Form("tmp_%s", hTmp->GetName()));
    hTmp->Scale(input.scale_);
    if(!h) h = hTmp;
    else {h->Add(hTmp); delete hTmp;}
  }

  //loop through and ensure all bins are >= 0
  for(int xbin = 1; xbin <= h->GetNbinsX(); ++xbin) {
    for(int ybin = 1; ybin <= h->GetNbinsY(); ++ybin) {
      if(h->GetBinContent(xbin, ybin) < 0.) h->SetBinContent(xbin, ybin, 0.);
    }
  }
  return h;
}

//-------------------------------------------------------------------------------------------------------------------------------
//initialize the files and scales using a DataPlotter
Int_t initialize_plotter(TString base, int year) {
  if(dataplotter_) delete dataplotter_;
  dataplotter_ = new DataPlotter();
  dataplotter_->include_qcd_ = 0;
  dataplotter_->include_misid_ = 0;
  dataplotter_->verbose_ = max(0, verbose_ - 1);
  dataplotter_->qcd_scale_ = 1.;
  years_ = {year};

  useEmbed_ = 0; //correction factors for DY MC, not embedding
  hist_tag_ = "zpt"; //tag for the ZPtHistMaker

  std::vector<dcard> cards;
  get_datacards(cards, selection_, true);

  CrossSections xs(useUL_, ZMode_); //cross section handler
  dataplotter_->set_luminosity(xs.GetLuminosity(year));
  if(verbose_ > 0) cout << "--- Dataplotter luminosity for " << year << " = " << dataplotter_->lum_ << endl;

  int status(0);
  for(auto card : cards)
    status += dataplotter_->add_dataset(card);

  status += dataplotter_->init_files();
  return status;
}

//----------------------------------------------------------------------------------------------------------------------------------
TCanvas* scale_factors(bool useMuon = true, int set = 7, int year = 2016, TString path = "nanoaods_zpt") {
  hist_dir_ = path;
  selection_ = (useMuon) ? "mumu" : "ee";

  //Get data histogram
  TString baseName = "clfv_";
  baseName += (useMuon) ? "mumu_clfv_" : "ee_clfv_";
  baseName += year;
  baseName += "_";

  if(verbose_ > 0) std::cout << "Initializing the dataplotter" << std::endl;
  if(initialize_plotter(baseName, year)) {
    cout << "Dataplotter initialization failed!\n";
    return nullptr;
  }

  int setAbs = (useMuon) ? set+CLFVHistMaker::kMuMu : set+CLFVHistMaker::kEE;
  if(verbose_ > 0) std::cout << "Retrieving data histogram" << std::endl;
  TH2* hData = get_histogram(setAbs, 0, true);
  if(!hData) {
    cout << "2D data histogram not found!\n";
    return nullptr;
  }
  if(hData->Integral() <= 0.) {
    cout << "No data found in data histogram!\n";
    return nullptr;
  }
  hData->SetName("hData");


  /////////////////////////
  // Get reco histograms //
  /////////////////////////

  if(verbose_ > 0) std::cout << "Getting the MC histogram" << std::endl;
  TH2* hMC = get_histogram(setAbs, 0+10*!useMuon);
  if(!hMC) {
    cout << "Failed to retrieve the MC histogram!\n";
    return NULL;
  }
  if(hMC->Integral() <= 0.) {
    cout << "No data found in reco MC histogram!\n";
    return NULL;
  }
  TH2* hBkg = get_histogram(setAbs, -1-10*!useMuon);
  if(!hBkg) {
    cout << "Failed to retrieve the background histogram!\n";
    return NULL;
  }
  if(hBkg->Integral() <= 0.) {
    cout << "No data found in reco Bkg histogram!\n";
    return NULL;
  }
  hBkg->SetName("hBkg");
  //subtract the non-DY from the data
  hData->Add(hBkg,-1);
  //ensure all bins have non-negative entries
  for(int binx = 1; binx <= hData->GetNbinsX(); ++binx) {
    for(int biny = 1; biny <= hData->GetNbinsY(); ++biny) {
      if(hData->GetBinContent(binx,biny) < 0.) hData->SetBinContent(binx, biny, 0.);
    }
  }

  //get corrected histogram for cross-checks
  bool useCorrPrev = useCorrectedHist_;
  useCorrectedHist_ = true;
  TH2* hMCCorr = get_histogram(setAbs, 0+10*!useMuon);
  useCorrectedHist_ = useCorrPrev;
  if(!hMCCorr) {
    cout << "Failed to retrieve the corrected MC histogram!\n";
    return NULL;
  }
  hMCCorr->SetName("hMCCorr");

  //get reco corrected with gen weights for cross-checks
  TH2* hMCGenCorr = get_histogram(setAbs, 3+10*!useMuon);
  if(!hMCGenCorr) {
    cout << "Failed to retrieve the gen-level corrected reco MC histogram!\n";
    return NULL;
  }
  hMCGenCorr->SetName("hMCGenCorr");

  ////////////////////////
  // Get gen histograms //
  ////////////////////////
  //gen-level before corrections
  TH2* hGenOrig = get_histogram(setAbs, 1+10*!useMuon);
  if(!hGenOrig) {
    cout << "Failed to retrieve the Uncorrected Gen-level histogram!\n";
    return NULL;
  }
  hGenOrig->SetName("hGenOrig");

  //gen-level after reco corrections
  TH2* hGenCorr = get_histogram(setAbs, 2+10*!useMuon);
  if(!hGenCorr) {
    cout << "Failed to retrieve the corrected Gen-level histogram!\n";
    return NULL;
  }
  hGenCorr->SetName("hGenCorr");

  //gen-level after gen corrections
  useCorrectedHist_ = true;
  TH2* hGenCorr2 = get_histogram(setAbs, 1+10*!useMuon);
  useCorrectedHist_ = useCorrPrev;
  if(!hGenCorr2) {
    cout << "Failed to retrieve the gen-weight corrected Gen-level histogram!\n";
    return NULL;
  }
  hGenCorr2->SetName("hGenCorr2");

  //rebin the histograms if needed
  int n_pt_rec_bins = 100;
  int n_pt_gen_bins = 100;
  double pt_rec_bins[n_pt_rec_bins+1];
  get_pt_binning(pt_rec_bins, n_pt_rec_bins,0);
  double pt_gen_bins[n_pt_gen_bins+1];
  get_pt_binning(pt_gen_bins, n_pt_gen_bins,1);
  int n_mass_rec_bins = 100;
  int n_mass_gen_bins = 100;
  double mass_rec_bins[n_mass_rec_bins+1];
  get_mass_binning(mass_rec_bins, n_mass_rec_bins,0);
  double mass_gen_bins[n_mass_gen_bins+1];
  get_mass_binning(mass_gen_bins, n_mass_gen_bins,1);
  if(verbose_ > 1) {
    cout << "Z pT reco binning:\n";
    for(int bin = 0; bin <= n_pt_rec_bins; ++bin)
      cout << "bin " << bin << ": " << pt_rec_bins[bin] << endl;
    cout << "Z pT gen binning:\n";
    for(int bin = 0; bin <= n_pt_gen_bins; ++bin)
      cout << "bin " << bin << ": " << pt_gen_bins[bin] << endl;
    cout << "Z mass reco binning:\n";
    for(int bin = 0; bin <= n_mass_rec_bins; ++bin)
      cout << "bin " << bin << ": " << mass_rec_bins[bin] << endl;
    cout << "Z mass gen binning:\n";
    for(int bin = 0; bin <= n_mass_gen_bins; ++bin)
      cout << "bin " << bin << ": " << mass_gen_bins[bin] << endl;
  }
  if(rebin_ && verbose_ > 1) cout << "Rebinning data histogram\n";
  TH2* hDataRebinned     = (!rebin_   ) ? hData     : create_rebinned_histogram(hData     , n_mass_rec_bins, mass_rec_bins, n_pt_rec_bins, pt_rec_bins);
  TH2* hMCRebinned       = (!rebin_   ) ? hMC       : create_rebinned_histogram(hMC       , n_mass_rec_bins, mass_rec_bins, n_pt_rec_bins, pt_rec_bins);
  TH2* hMCCorrRebinned   = (!rebin_   ) ? hMCCorr   : create_rebinned_histogram(hMCCorr   , n_mass_rec_bins, mass_rec_bins, n_pt_rec_bins, pt_rec_bins);
  TH2* hMCGenCorrRebinned= (!rebin_   ) ? hMCGenCorr: create_rebinned_histogram(hMCGenCorr, n_mass_rec_bins, mass_rec_bins, n_pt_rec_bins, pt_rec_bins);
  TH2* hGenOrigRebinned  = (!rebinGen_) ? hGenOrig  : create_rebinned_histogram(hGenOrig  , n_mass_gen_bins, mass_gen_bins, n_pt_gen_bins, pt_gen_bins);
  TH2* hGenCorrRebinned  = (!rebinGen_) ? hGenCorr  : create_rebinned_histogram(hGenCorr  , n_mass_gen_bins, mass_gen_bins, n_pt_gen_bins, pt_gen_bins);
  TH2* hGenCorr2Rebinned = (!rebinGen_) ? hGenCorr2 : create_rebinned_histogram(hGenCorr2 , n_mass_gen_bins, mass_gen_bins, n_pt_gen_bins, pt_gen_bins);

  TString c_name = (useCorrectedHist_) ? "c_fixed_zpt" : "c_zpt";
  TCanvas* c = new TCanvas(c_name.Data(), c_name.Data(), 1600, 900);
  c->Divide(3,2);

  ///////////////////////
  //  Draw reco plots  //
  ///////////////////////

  TVirtualPad* pad = c->cd(1);
  hDataRebinned->SetTitle("Data: Di-lepton pT vs Mass");
  hDataRebinned->SetXTitle("Mass (GeV/c^{2})");
  hDataRebinned->SetYTitle("pT (GeV/c)");
  draw_2d_log(hDataRebinned, "colz");
  if(!useOnlyPt_)
    pad->SetLogx();
  pad->SetLogy();
  pad->SetLogz();
  pad = c->cd(2);
  const double int_mc = hMCRebinned->Integral(0,hMCRebinned->GetNbinsX()+1, 0, hMCRebinned->GetNbinsY()+1); //integral including under/overflow
  const double int_data = hDataRebinned->Integral(0,hDataRebinned->GetNbinsX()+1, 0, hDataRebinned->GetNbinsY()+1);
  const double data_mc_scale =  int_data / int_mc;

  cout << "Data / MC = " << int_data  << " / " << int_mc << " = " << data_mc_scale << endl;
  hMCRebinned->Scale(data_mc_scale);
  hMCRebinned->SetTitle("MC: Di-lepton pT vs Mass");
  hMCRebinned->SetXTitle("Mass (GeV/c^{2})");
  hMCRebinned->SetYTitle("pT (GeV/c)");
  draw_2d_log(hMCRebinned, "colz");
  if(!useOnlyPt_)
    pad->SetLogx();
  pad->SetLogy();
  pad->SetLogz();
  pad = c->cd(3);
  TH2* hRatioShape = (TH2*) hDataRebinned->Clone("hRatioNorm");
  hRatioShape->Divide(hMCRebinned);
  hRatioShape->SetTitle("Normalized Data/MC: Di-lepton pT vs Mass");
  gStyle->SetPaintTextFormat(".2f");
  draw_2d_log(hRatioShape, "colz text");
  if(!useOnlyPt_)
    pad->SetLogx();
  pad->SetLogy();
  pad = c->cd(4);

  ///////////////////////
  //  Draw Gen plots   //
  ///////////////////////

  hGenOrigRebinned->SetTitle("Uncorrected Gen-level: Z pT vs Mass");
  hGenOrigRebinned->SetXTitle("Mass (GeV/c^{2})");
  hGenOrigRebinned->SetYTitle("pT (GeV/c)");
  draw_2d_log(hGenOrigRebinned, "colz");
  if(!useOnlyPt_)
    pad->SetLogx();
  pad->SetLogy();
  pad->SetLogz();
  pad = c->cd(5);

  hGenCorrRebinned->SetTitle("Corrected Gen-level: Z pT vs Mass");
  hGenCorrRebinned->SetXTitle("Mass (GeV/c^{2})");
  hGenCorrRebinned->SetYTitle("pT (GeV/c)");
  draw_2d_log(hGenCorrRebinned, "colz");
  if(!useOnlyPt_)
    pad->SetLogx();
  pad->SetLogy();
  pad->SetLogz();
  pad = c->cd(6);

  TH2* hGenRatio = (TH2*) hGenCorrRebinned->Clone("hGenRatio");
  hGenRatio->Divide(hGenOrigRebinned);
  hGenRatio->SetTitle("Corrected Gen-level / Uncorrected Gen-level: Z pT vs Mass");
  draw_2d_log(hGenRatio, "colz text");
  if(!useOnlyPt_)
    pad->SetLogx();
  pad->SetLogy();

  //save the images
  gSystem->Exec(Form("[ ! -d figures/%i ] && mkdir -p figures/%i", years_[0], years_[0]));
  gSystem->Exec("[ ! -d rootfiles ] && mkdir rootfiles");
  if(!useCorrectedHist_) {
    c->Print(Form("figures/pt_vs_m_%s_%i.png", (useMuon) ? "mumu" : "ee", year));
    //save the scale factor histogram
    TFile* fout = new TFile(Form("rootfiles/z_pt_vs_m_scales_%s_%i.root", (useMuon) ? "mumu" : "ee", year), "RECREATE");
    fout->cd();
    hRatioShape->Write();
    hGenRatio->Write();
    fout->Close();
  }

  ///////////////////////////
  // Draw the cross-checks //
  ///////////////////////////

  TCanvas* c2 = new TCanvas("c_cross", "c_cross", 1500, 500);
  c2->Divide(3,1);

  //Draw reco cross-check
  pad = c2->cd(1);
  double int_mc_corr = hMCCorrRebinned->Integral(0,hMCCorrRebinned->GetNbinsX()+1, 0, hMCCorrRebinned->GetNbinsY()+1);
  cout << "Corrected MC / Original MC = " << int_mc_corr << " / " << int_mc << " = " << int_mc_corr/int_mc << endl;
  hMCCorrRebinned->Scale(data_mc_scale);
  hMCCorrRebinned->Divide(hDataRebinned); //should exactly match
  hMCCorrRebinned->SetTitle("Reco-weighted Reco MC / Data");
  draw_2d_log(hMCCorrRebinned, "colz text");
  if(!useOnlyPt_)
    pad->SetLogx();
  pad->SetLogy();

  //Draw gen cross-check
  pad = c2->cd(2);
  hGenCorr2Rebinned->Divide(hGenCorrRebinned); //should exactly match
  hGenCorr2Rebinned->SetTitle("Gen-weighted Gen MC / Reco-weighted Gen MC");
  draw_2d_log(hGenCorr2Rebinned, "colz text");
  if(!useOnlyPt_)
    pad->SetLogx();
  pad->SetLogy();

  //Draw gen corrected reco cross-check
  pad = c2->cd(3);
  double int_mc_gencorr = hMCGenCorrRebinned->Integral(0,hMCGenCorrRebinned->GetNbinsX()+1, 0, hMCGenCorrRebinned->GetNbinsY()+1);
  cout << "Gen-Corrected MC / Original MC = " << int_mc_gencorr << " / " << int_mc << " = " << int_mc_gencorr/int_mc << endl;
  hMCGenCorrRebinned->Scale(data_mc_scale);
  hMCGenCorrRebinned->Divide(hDataRebinned); //should hopefully match
  hMCGenCorrRebinned->SetTitle("Gen-weighted Reco MC / Data");
  draw_2d_log(hMCGenCorrRebinned, "colz text");
  if(!useOnlyPt_)
    pad->SetLogx();
  pad->SetLogy();
  c2->Print(Form("figures/pt_vs_m_crosscheck_%s_%i.png", (useMuon) ? "mumu" : "ee", year));


  //////////////////////////////////////////////////////////////////////////////////////
  // Draw distributions with/without corrections

  for(int i = 0; i <= 5; ++i) {
    print_stack((i > 0) ? Form("zpt%i", i) : "zpt", "event", setAbs, useMuon, year);
    print_stack((i > 0) ? Form("zmass%i", i) : "zmass", "event", setAbs, useMuon, year);
    if(i < 3) print_stack((i > 0) ? Form("leppt%i", i) : "leppt", "event", setAbs, useMuon, year);
    if(i < 3) print_stack((i > 0) ? Form("lepm%i", i) : "lepm", "event", setAbs, useMuon, year);
  }

  //////////////////////////////////////////////////////////////////////////////////////
  // Plot the correction with the uncertainty band

  print_unc_band(setAbs, useMuon, year);

  return c;
}
