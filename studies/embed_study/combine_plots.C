//overlay DY50 and embedding histograms

TString base_;

//-------------------------------------------------------------------------------------------------------------
int make_2d_figure(TH2* hEmbed, TH2* hDY, bool logy = false) {
  TString name = hDY->GetName();
  if(hEmbed->Integral() <= 0. || hDY->Integral() <= 0.) {
    cout << "Histogram " << name.Data() << " has <= 0 integrals! Embed = "
         << hEmbed->Integral() << ", DY = " << hDY->Integral() << endl;
    return 2;
  }
  TCanvas* c = new TCanvas("c", "c", 1000, 800);

  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

  TH2* hRatio = (TH2*) hDY->Clone("hRatio");
  hRatio->Divide(hEmbed);
  gStyle->SetPaintTextFormat(".2f");
  hRatio->Draw("colz text");
  hRatio->SetTitle("");
  hRatio->SetTitle("DY/Embed");
  hRatio->GetZaxis()->SetRangeUser(0.5, 1.5);
  if(logy) hRatio->GetYaxis()->SetMoreLogLabels(kTRUE);
  if(logy) c->SetLogy();

  c->SaveAs(Form("figures/%s/%s.png", base_.Data(), name.Data()));

  delete c;
  delete hRatio;

  return 0;
}

//-------------------------------------------------------------------------------------------------------------
int make_figure(TH1* hEmbed, TH1* hDY) {
  TString name = hDY->GetName();

  if(hEmbed->Integral() <= 0. || hDY->Integral() <= 0.) {
    cout << "Histogram " << name.Data() << " has <= 0 integrals! Embed = "
         << hEmbed->Integral() << ", DY = " << hDY->Integral() << endl;
    return 2;
  }
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

  TLine* line = new TLine(hRatio->GetXaxis()->GetXmin(), 1., hRatio->GetXaxis()->GetXmax(), 1.);
  line->SetLineWidth(2);
  line->SetLineColor(kRed);
  line->Draw("same");

  c->SaveAs(Form("figures/%s/%s.png", base_.Data(), name.Data()));

  delete c;
  delete hRatio;
  delete line;

  return 0;
}

//-------------------------------------------------------------------------------------------------------------
int make_2d_figure(TString name, TFile* fEmbed, TFile* fDY, bool logy = false) {
  TH2* hEmbed = (TH2*) fEmbed->Get(name.Data());
  TH2* hDY    = (TH2*) fDY   ->Get(name.Data());
  if(!hEmbed || !hDY) return 1;
  return make_2d_figure(hEmbed, hDY, logy);
}

//-------------------------------------------------------------------------------------------------------------
int make_figure(TString name, TFile* fEmbed, TFile* fDY) {
  TH1* hEmbed = (TH1*) fEmbed->Get(name.Data());
  TH1* hDY    = (TH1*) fDY   ->Get(name.Data());
  if(!hEmbed || !hDY) return 1;
  return make_figure(hEmbed, hDY);
}

//-------------------------------------------------------------------------------------------------------------
int combine_plots(const TString selec = "emu", const int year = 2018, const bool tight = false) {

  base_ = Form("%s_combined%s_%i", selec.Data(), (tight) ? "_tight" : "", year);
  gSystem->Exec(Form("[ ! -d figures/%s ] && mkdir -p figures/%s", base_.Data(), base_.Data()));

  TFile* fEmbed = TFile::Open(Form("histograms/%s_embed%s_%i.hist", selec.Data(), (tight) ? "_tight" : "", year), "READ");
  TFile* fDY    = TFile::Open(Form("histograms/%s_dy50%s_%i.hist" , selec.Data(), (tight) ? "_tight" : "", year), "READ");
  if(!fEmbed || !fDY) return 1;

  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

  TH1* hscale = (TH1*) fDY->Get("hzeta")->Clone("ZEtaUnfolding");
  if(!hscale) return 10;
  TH1* hembed = (TH1*) fEmbed->Get("hzeta")->Clone("EmbedZUnfolding");
  hscale->Rebin(5);
  hembed->Rebin(5);

  make_figure("honept"    , fEmbed, fDY);
  make_figure("htwopt"    , fEmbed, fDY);
  make_figure("honeeta"   , fEmbed, fDY);
  make_figure("htwoeta"   , fEmbed, fDY);
  make_figure("hmuonpt"   , fEmbed, fDY);
  make_figure("htaupt"    , fEmbed, fDY);
  make_figure("hmuoneta"  , fEmbed, fDY);
  make_figure("htaueta"   , fEmbed, fDY);
  make_figure("htauonept" , fEmbed, fDY);
  make_figure("htautwopt" , fEmbed, fDY);
  make_figure("htauoneeta", fEmbed, fDY);
  make_figure("htautwoeta", fEmbed, fDY);
  make_figure("hleadpt"   , fEmbed, fDY);
  make_figure("htrailpt"  , fEmbed, fDY);
  make_figure("hleadeta"  , fEmbed, fDY);
  make_figure("htraileta" , fEmbed, fDY);
  make_figure("hzpt"      , fEmbed, fDY);
  make_figure("hzmass"    , fEmbed, fDY);
  make_figure("hzeta"     , fEmbed, fDY);
  make_figure("hdileppt"  , fEmbed, fDY);
  make_figure("hdilepmass", fEmbed, fDY);
  make_figure("hdilepeta" , fEmbed, fDY);
  make_figure("hlogweight", fEmbed, fDY);
  make_2d_figure("hzetavspt", fEmbed, fDY);

  gSystem->Exec("[ ! -d rootfiles ] && mkdir rootfiles");
  TFile* fout = new TFile(Form("rootfiles/embedding_unfolding_%s_%i.root", selec.Data(), year), "RECREATE");
  make_figure(hembed, hscale);
  //match them in the low eta region to not change the rate there
  const double match_scale = hscale->Integral(hscale->FindBin(-1.9), hscale->FindBin(1.9)) / hembed->Integral(hembed->FindBin(-1.9), hembed->FindBin(1.9));
  hembed->Scale(match_scale);
  hscale->Divide(hembed);
  hscale->Write();

  TH2* hscale2D = (TH2*) fDY->Get("hzetavspt")->Clone("ZEtaVsPtUnfolding");
  if(!hscale2D) return 10;
  TH2* hembed2D = (TH2*) fEmbed->Get("hzetavspt")->Clone("EmbedZ2DUnfolding");
  hscale2D->Divide(hembed2D);
  hscale2D->Write();

  fout->Close();

  return 0;
}
