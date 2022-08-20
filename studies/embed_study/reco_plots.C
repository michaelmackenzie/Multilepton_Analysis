//overlay DY50 and embedding histograms

int year_;
TString selec_;

double nembed_;
double ndy_;
int nembed_mini_;
double dy_xsec_;
double dy_neg_frac_;
double lum_;

//-------------------------------------------------------------------------------------------------------------
int make_figure(TH1* hEmbed, TH1* hDY, TString name, int rebin = 1, double xmin = 1., double xmax = -1.) {
  const double dy_scale = dy_xsec_ * lum_ / (ndy_ * (1. - 2.*dy_neg_frac_)); //x-sec * lumi / (N(processed) * (1. - 2*neg_frac))
  hDY->Scale(dy_scale);
  if(hEmbed->Integral() <= 0. || hDY->Integral() <= 0.) {
    cout << "Histogram " << name.Data() << " has <= 0 integrals! Embed = "
         << hEmbed->Integral() << ", DY = " << hDY->Integral() << endl;
    return 2;
  }
  if(rebin > 1) {hEmbed->Rebin(rebin); hDY->Rebin(rebin);}

  TCanvas* c = new TCanvas("c", "c", 1000, 1100);
  TPad* pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1.0); pad1->Draw();
  TPad* pad2 = new TPad("pad2", "pad2", 0., 0.0, 1., 0.3); pad2->Draw();
  pad1->SetBottomMargin(0.05); pad2->SetTopMargin(0.03);

  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  pad1->cd();
  pad1->SetGrid();
  hEmbed->SetLineColor(kRed);
  hEmbed->SetLineWidth(2);
  hDY->SetLineWidth(2);
  hDY->SetMarkerStyle(20);
  hDY->SetMarkerSize(0.8);
  hEmbed->Draw("hist");
  hDY->Draw("E1 same");
  hEmbed->GetYaxis()->SetRangeUser(0., 1.2*max(hEmbed->GetMaximum(), hDY->GetMaximum()));
  if(xmin < xmax) hEmbed->GetXaxis()->SetRangeUser(xmin, xmax);

  //Add N(DY) and N(Embed)
  TLatex label;
  label.SetNDC();
  label.SetTextFont(72);
  label.SetTextSize(0.04);
  label.SetTextAlign(13);
  label.SetTextAngle(0);
  label.DrawLatex(0.1, 0.89, Form("N(DY)    = %10.0f", hDY->Integral()    + hDY->GetBinContent(0) + hDY->GetBinContent(hDY->GetNbinsX()+1)));
  label.DrawLatex(0.1, 0.84, Form("N(Embed) = %10.0f", hEmbed->Integral() + hEmbed->GetBinContent(0) + hEmbed->GetBinContent(hEmbed->GetNbinsX()+1)));

  TLegend* leg = new TLegend(0.7, 0.75, 0.9, 0.9);
  leg->AddEntry(hDY, "DY");
  leg->AddEntry(hEmbed, "Embedding");
  leg->Draw();

  pad2->cd();
  pad2->SetGrid();
  TH1* hRatio = (TH1*) hDY->Clone("hRatio");
  hRatio->Divide(hEmbed);
  hRatio->Draw("E1");
  hRatio->SetTitle("");
  hRatio->GetYaxis()->SetTitle("DY/Embed");
  hRatio->GetYaxis()->SetTitleSize(0.13);
  hRatio->GetYaxis()->SetTitleOffset(0.30);
  hRatio->GetYaxis()->SetLabelSize(0.08);
  hRatio->GetXaxis()->SetLabelSize(0.08);
  hRatio->GetYaxis()->SetRangeUser(0.5, 1.5);
  if(xmin < xmax) hRatio->GetXaxis()->SetRangeUser(xmin, xmax);

  if(xmin > xmax) {xmin = hRatio->GetXaxis()->GetXmin(); xmax = hRatio->GetXaxis()->GetXmax();}
  TLine* line = new TLine(xmin, 1., xmax, 1.);
  line->SetLineWidth(2);
  line->SetLineColor(kRed);
  line->Draw("same");

  c->SaveAs(Form("figures/%s_reco_%i/%s.png", selec_.Data(), year_, name.Data()));

  delete c;
  delete hRatio;
  delete line;

  return 0;
}

//-------------------------------------------------------------------------------------------------------------
int make_figure(TString name, TString type, int set, TFile* fDY, vector<TFile*> fEmbed, vector<double> embed_xs,
                int rebin = 1, double xmin = 1., double xmax = -1.) {
  TH1* hDY    = (TH1*) fDY   ->Get(Form("Data/%s_%i/%s", type.Data(), set, name.Data()));
  TH1* hEmbed = nullptr;
  for(unsigned index = 0; index < fEmbed.size(); ++index) {
    TH1* h = (TH1*) fEmbed[index]->Get(Form("Data/%s_%i/%s", type.Data(), set, name.Data()));
    if(!h) return 1;
    h->Scale(embed_xs[index]);
    if(hEmbed) hEmbed->Add(h);
    else       hEmbed = (TH1*) h->Clone(Form("embed_%s_%s_%i", name.Data(), type.Data(), set));
  }
  if(!hEmbed || !hDY) return 1;
  return make_figure(hEmbed, hDY, Form("%s_%s_%i", type.Data(), name.Data(), set), rebin, xmin, xmax);
}

//-------------------------------------------------------------------------------------------------------------
int reco_plots(const TString selec = "emu", const int year = 2018, TString path = "nanoaods_02") {
  TString final_state;
  if(selec == "emu") final_state = "EMu";
  else if(selec == "etau") final_state = "ETau";
  else if(selec == "mutau") final_state = "MuTau";
  else {
    cout << "Unknown selection " << selec.Data() << endl;
    return -1;
  }

  year_ = year;
  selec_ = selec;
  gSystem->Exec(Form("[ ! -d figures/%s_reco_%i ] && mkdir -p figures/%s_reco_%i", selec.Data(), year, selec.Data(), year));

  vector<TString> runs = {};
  if     (year == 2016) runs = {"B", "C", "D", "E", "F", "G", "H"};
  else if(year == 2017) runs = {"B", "C", "D", "E", "F"};
  else if(year == 2018) runs = {"A", "B", "C", "D"};
  else return -1;

  vector<TFile*> fEmbeds;
  vector<double> embed_scales;
  CrossSections xs;
  for(TString run : runs) {
    TFile* fEmbed = TFile::Open(Form("root://cmseos.fnal.gov//store/user/mmackenz/histograms/%s/clfv_%s_%i_Embed-%s-%s.hist",
                                     path.Data(), selec.Data(), year, final_state.Data(), run.Data()), "READ");
    if(!fEmbed) return 1;
    fEmbeds.push_back(fEmbed);
    embed_scales.push_back(xs.GetCrossSection(Form("Embed-%s-%s", final_state.Data(), run.Data()), year));
  }

  TFile* fDY = TFile::Open(Form("root://cmseos.fnal.gov//store/user/mmackenz/histograms/%s/clfv_%s_%i_DY50-%s-1.hist",
                                path.Data(), selec.Data(), year, (year == 2017) ? "ext" : "amc"), "READ");
  if(fEmbeds.size() == 0 || !fDY) return 1;

  TH1* hDYNorm = (TH1*) fDY->Get("events");
  if(!hDYNorm) return 2;
  ndy_ = hDYNorm->GetBinContent(1);

  dy_xsec_ = 6077.22;
  if(year == 2016) {
    nembed_mini_ = 26367000;
    dy_neg_frac_ = 0.1367;
    lum_ = 36.33e3;
  } else if(year == 2017) {
    nembed_mini_ = 34683979;
    dy_neg_frac_ = 0.1611;
    lum_ = 41.48e3;
  } else if(year == 2018) {
    nembed_mini_ = 48428591;
    dy_neg_frac_ = 0.163;
    lum_ = 59.83e3;
  }

  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

  /////////////////////////////////////
  // Make figures

  int offset = 0;
  if(selec == "etau") offset = 100;
  else if(selec == "emu") offset = 200;

  make_figure("onept"       ,   "lep",  offset + 8, fDY, fEmbeds, embed_scales, 2, 20., 100.);
  make_figure("twopt"       ,   "lep",  offset + 8, fDY, fEmbeds, embed_scales, 2, 20., 100.);
  make_figure("leppt"       , "event",  offset + 8, fDY, fEmbeds, embed_scales, 2,  0., 100.);
  make_figure("lepm"        , "event",  offset + 8, fDY, fEmbeds, embed_scales, 4);
  make_figure("zpt"         , "event",  offset + 8, fDY, fEmbeds, embed_scales, 1,  0., 100.);
  make_figure("mcera"       , "event",  offset + 8, fDY, fEmbeds, embed_scales);
  make_figure("met"         , "event",  offset + 8, fDY, fEmbeds, embed_scales, 2,  0.,  60.);
  make_figure("mtone"       , "event",  offset + 8, fDY, fEmbeds, embed_scales, 2,  0., 100.);
  make_figure("mttwo"       , "event",  offset + 8, fDY, fEmbeds, embed_scales, 2,  0., 100.);
  make_figure("mtlep"       , "event",  offset + 8, fDY, fEmbeds, embed_scales, 2,  0.,  70.);
  make_figure("taudecaymode", "event",  offset + 8, fDY, fEmbeds, embed_scales);
  make_figure("jetpt"       , "event",  offset + 8, fDY, fEmbeds, embed_scales, 2, 20., 100.);
  make_figure("ht"          , "event",  offset + 8, fDY, fEmbeds, embed_scales, 2,  0.,  80.);
  make_figure("njets20"     , "event",  offset + 8, fDY, fEmbeds, embed_scales, 1,  0.,   5.);

  return 0;
}
