//Combine the Data and MC efficiency measurements into a scale factor file

//fit the scale factors in bins of eta as a function of pT
void fit_scales(TH2F* h, int year) {
  //x-axis is the super cluster eta axis
  gSystem->Exec(Form("[ ! -d figures/scale_fits_%i ] && mkdir -p figures/scale_fits_%i", year, year));
  TVirtualFitter::SetMaxIterations( 1e6 );
  gStyle->SetOptFit(1111);
  for(int xbin = 1; xbin <= h->GetNbinsX(); ++xbin) {
    TH1D* hPt = h->ProjectionY("tmp", xbin, xbin);
    TF1* f;
    // f = new TF1(Form("%s_fit_%i", h->GetName(), xbin), "[offset] + [scale]/(x + [xoffset])", 28., 999.);
    // f->SetParLimits(f->GetParNumber("xoffset"), -28., 1.e5);
    // f->SetParameters(1., 1., -25.);
    f = new TF1(Form("%s_fit_%i", h->GetName(), xbin), "[slope]*std::atan([scale]*(x+[xoffset]))+[offset]", 28., 999.);
    f->SetParameters(0., 1., 1., -30.);
    // f = new TF1(Form("%s_fit_%i", h->GetName(), xbin), "[slope]*std::log([scale]*(x+[xoffset]))/x+[offset]", 28., 999.);
    // f->SetParLimits(f->GetParNumber("xoffset"), -28., 1.e5);
    // f->SetParameters(0., 1., 1., -10.);
    hPt->Fit(f, "R");
    TCanvas* c = new TCanvas();
    hPt->SetMarkerSize(0.8);
    hPt->SetMarkerStyle(20);
    hPt->SetLineColor(kBlue);
    hPt->SetLineWidth(2);
    hPt->Draw("E");

    c->SaveAs(Form("figures/scale_fits_%i/%s_bin_%i.png", year, h->GetName(), xbin));
    f->Write();
    delete hPt;
  }
}

void combine_efficiencies(int year) {

  TFile* fData = TFile::Open(Form("rootfiles/efficiencies_0_%i.root", year), "READ");
  TFile* fMC   = TFile::Open(Form("rootfiles/efficiencies_1_%i.root", year), "READ");
  TH2F* hData  = (TH2F*) fData->Get("hRatio");
  if(!hData) {
    cout << "Data histogram not found!\n";
    return;
  }
  hData->SetName("EGamma_EffData2D");
  TH2F* hMC  = (TH2F*) fMC->Get("hRatio");
  if(!hMC) {
    cout << "MC histogram not found!\n";
    return;
  }
  hMC->SetName("EGamma_EffMC2D");

  TH2F* hScale = (TH2F*) hData->Clone("EGamma_SF2D");
  hScale->Divide(hMC);
  hScale->SetTitle("Data Efficiency / MC Efficiency");

  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c", "c", 1000, 700);
  hScale->Draw("colz");
  hScale->GetZaxis()->SetRangeUser(0.2, 1.5);

  c->SaveAs(Form("figures/scales_%i.png", year));
  c->SetLogy();
  c->SaveAs(Form("figures/scales_log_%i.png", year));

  TFile* fout = new TFile(Form("rootfiles/egamma_trigger_eff_%i.root", year), "RECREATE");
  fout->cd();
  hData->Write();
  hMC->Write();
  hScale->Write();
  // fit_scales(hData, year);
  // fit_scales(hMC, year);
  // fit_scales(hScale, year);
  fout->Write();
  fout->Close();
}
