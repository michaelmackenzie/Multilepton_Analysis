//Script to compare the mumu and ee scale factors

int verbose_ = 0;

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


int compare_scales(int year = 2016) {

  TFile* fmumu = new TFile(Form("rootfiles/z_pt_vs_m_scales_mumu_%i.root", year), "READ");
  TFile* fee   = new TFile(Form("rootfiles/z_pt_vs_m_scales_ee_%i.root"  , year), "READ");

  if(!fmumu || !fee) return 1;
  gStyle->SetPaintTextFormat(".2f");

  TH2D* hmumu = (TH2D*) fmumu->Get("hGenRatio");
  if(!hmumu) {
    cout << "Muon histogram not found!\n";
    return 2;
  }
  hmumu->SetName("hmumu");

  TH2D* hee = (TH2D*) fee->Get("hGenRatio");
  if(!hee) {
    cout << "Electron histogram not found!\n";
    return 2;
  }
  hee->SetName("hee");

  ///////////////////////////
  // Draw the cross-checks //
  ///////////////////////////
  gStyle->SetOptStat(0);
  TVirtualPad* pad;
  TPaletteAxis* palette;
  TCanvas* c = new TCanvas("c_cross", "c_cross", 1500, 500);
  c->Divide(3,1);

  pad = c->cd(1);
  pad->SetRightMargin(0.15);
  set_z_range(hmumu);
  hmumu->Draw("colz text");
  hmumu->SetTitle("#mu#mu weights");
  hmumu->GetYaxis()->SetMoreLogLabels(kTRUE);
  pad->SetLogy();
  pad->Update();
  palette= (TPaletteAxis*) hmumu->GetListOfFunctions()->FindObject("palette");
  palette->SetX1NDC(0.86);
  palette->SetX2NDC(0.91);

  pad = c->cd(2);
  pad->SetRightMargin(0.15);
  set_z_range(hee);
  hee->Draw("colz text");
  hee->SetTitle("ee weights");
  hee->GetYaxis()->SetMoreLogLabels(kTRUE);
  pad->SetLogy();
  pad->Update();
  palette= (TPaletteAxis*) hee->GetListOfFunctions()->FindObject("palette");
  palette->SetX1NDC(0.86);
  palette->SetX2NDC(0.91);

  TH2D* hratio = (TH2D*) hmumu->Clone("hratio");
  hratio->Divide(hee);
  set_z_range(hratio);
  pad = c->cd(3);
  pad->SetRightMargin(0.15);
  hratio->Draw("colz text");
  hratio->GetYaxis()->SetMoreLogLabels(kTRUE);
  hratio->SetTitle("#mu#mu weights / ee weights");
  pad->SetLogy();

  c->Print(Form("figures/pt_vs_m_weight_comparison_%i.png", year));

  return 0;
}
