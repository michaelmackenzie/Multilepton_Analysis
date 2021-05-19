//Script to compare the mumu and ee scale factors

int verbose_ = 0;

int compare_scales(int year = 2016) {

  TFile* fmumu = new TFile(Form("rootfiles/z_pt_vs_m_scales_mumu_%i.root", year), "READ");
  TFile* fee   = new TFile(Form("rootfiles/z_pt_vs_m_scales_ee_%i.root"  , year), "READ");

  if(!fmumu || !fee) return 1;

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
  hmumu->Draw("colz");
  hmumu->SetTitle("#mu#mu weights");
  pad->SetLogy();
  pad->Update();
  palette= (TPaletteAxis*) hmumu->GetListOfFunctions()->FindObject("palette");
  palette->SetX1NDC(0.86);
  palette->SetX2NDC(0.91);

  pad = c->cd(2);
  pad->SetRightMargin(0.15);
  hee->Draw("colz");
  hee->SetTitle("ee weights");
  pad->SetLogy();
  pad->Update();
  palette= (TPaletteAxis*) hee->GetListOfFunctions()->FindObject("palette");
  palette->SetX1NDC(0.86);
  palette->SetX2NDC(0.91);

  TH2D* hratio = (TH2D*) hmumu->Clone("hratio");
  hratio->Divide(hee);
  hratio->GetZaxis()->SetRangeUser(0.8, 1.2);
  pad = c->cd(3);
  pad->SetRightMargin(0.15);
  hratio->Draw("colz");
  hratio->SetTitle("#mu#mu weights / ee weights");
  pad->SetLogy();

  c->Print(Form("figures/pt_vs_m_weight_comparison_%i.png", year));

  return 0;
}
