//Compare Tau anti-jet ID scale factors between MC and Embedding

int compare_tau_ids(const int year = 2018) {

  TString path = gSystem->Getenv("CMSSW_BASE");
  path += "/src/TauPOG/TauIDSFs/data/TauID_SF_pt_DeepTau2017v2p1VS";

  TFile* fEmbed = TFile::Open(Form("%sjet_%i%s_EMB.root", path.Data(), year, (year == 2016) ? "Legacy" : "ReReco"));
  TFile* fDY    = TFile::Open(Form("%sjet_%i%s.root"    , path.Data(), year, (year == 2016) ? "Legacy" : "ReReco"));

  if(!fEmbed || !fDY) return 1;

  TF1* fID_Embed = (TF1*) fEmbed->Get("Tight_cent");
  TF1* fID_DY    = (TF1*) fDY   ->Get("Tight_cent");
  if(!fID_Embed || !fID_DY) {
    cout << "ID scale factor functions not found!\n";
    return 2;
  }

  fID_Embed->SetName("fEmbed");
  fID_DY   ->SetName("fDY");

  TCanvas* c = new TCanvas("c", "c", 1000, 1100);
  TPad* pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1.0); pad1->Draw();
  TPad* pad2 = new TPad("pad2", "pad2", 0., 0.0, 1., 0.3); pad2->Draw();
  pad1->SetBottomMargin(0.05); pad2->SetTopMargin(0.03);

  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  pad1->cd();
  pad1->SetGrid();

  fID_Embed->SetLineColor(kRed);
  fID_Embed->SetLineWidth(2);
  fID_DY   ->SetLineColor(kBlue);
  fID_DY   ->SetLineWidth(2);
  fID_DY   ->SetLineStyle(kDashed);

  fID_Embed->Draw();
  fID_DY->Draw("same");

  pad1->SetLogx();
  fID_Embed->GetYaxis()->SetRangeUser(0.7, 1.3);
  fID_Embed->GetXaxis()->SetMoreLogLabels(true);
  fID_Embed->SetTitle("Hadronic Tau anti-jet ID scale factors");

  TLegend* leg = new TLegend(0.7, 0.75, 0.9, 0.9);
  leg->AddEntry(fID_DY, "MC", "L");
  leg->AddEntry(fID_Embed, "Embedding", "L");
  leg->Draw();

  pad2->cd();
  pad2->SetGrid();
  TF1* fRatio = new TF1("fRatio", "fDY/fEmbed", fID_Embed->GetXmin(), fID_Embed->GetXmax());
  fRatio->Draw();
  pad2->SetLogx();
  fRatio->GetYaxis()->SetRangeUser(0.7, 1.3);
  fRatio->GetXaxis()->SetMoreLogLabels(true);
  fRatio->GetYaxis()->SetTitle("MC / Embed");
  fRatio->SetTitle("");
  fRatio->GetYaxis()->SetTitleSize(0.13);
  fRatio->GetYaxis()->SetTitleOffset(0.30);
  fRatio->GetYaxis()->SetLabelSize(0.08);
  fRatio->GetXaxis()->SetLabelSize(0.08);

  c->SaveAs(Form("figures/tau_jet_ID_%i.png", year));

  return 0;
}
