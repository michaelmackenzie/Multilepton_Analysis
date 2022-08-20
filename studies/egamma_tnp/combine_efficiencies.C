//Combine the Data and MC efficiency measurements into a scale factor file

//set reasonable z-range
void set_z_range(TH2* h) {
  double min_z(1.e3), max_z(-1.e3);
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
  else h->GetZaxis()->SetRangeUser(0.8, 1.2);
}

void combine_efficiencies(int year = 2016, int WP = 0, int period = -1) {

  TFile* fData = TFile::Open(Form("rootfiles/efficiencies_%i%s_wp%i_mc0.root", year, (period > -1) ? Form("_period_%i", period) : "", WP), "READ");
  TFile* fMC   = TFile::Open(Form("rootfiles/efficiencies_%i_wp%i_mc1.root", year, WP), "READ"); //always use default period MC
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
  gStyle->SetPaintTextFormat(".2f");
  set_z_range(hScale);
  hScale->SetMarkerSize(0.8);
  hScale->Draw("colz text");
  hScale->GetYaxis()->SetMoreLogLabels(kTRUE);
  // Move the palette
  TPaletteAxis *palette = (TPaletteAxis*) hScale->GetListOfFunctions()->FindObject("palette");
  if(palette) {
    palette->SetX1NDC(0.905);
    palette->SetX2NDC(0.94 );
    palette->SetY1NDC(0.1);
    palette->SetY2NDC(0.9);
    c->Modified();
    c->Update();
  } else {
    cout << "Z-axis palette not found!\n";
  }

  c->SaveAs(Form("figures/scales_wp%i_%i%s.png", WP, year, (period > -1) ? Form("_period_%i", period) : ""));
  c->SetLogy();
  c->SaveAs(Form("figures/scales_wp%i_%i%s_log.png", WP, year, (period > -1) ? Form("_period_%i", period) : ""));

  TFile* fout = new TFile(Form("rootfiles/egamma_trigger_eff_wp%i_%i%s.root", WP, year, (period > -1) ? Form("_period_%i", period) : ""), "RECREATE");
  fout->cd();
  hData->Write();
  hMC->Write();
  hScale->Write();
  fout->Write();
  fout->Close();
}
