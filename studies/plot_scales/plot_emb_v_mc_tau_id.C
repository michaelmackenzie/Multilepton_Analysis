//Compare the Embedding and MC measured Tau ID corrections

int plot_emb_v_mc_tau_id(int year) {
  TString path = gSystem->Getenv("CMSSW_BASE");
  path += "/src/TauPOG/TauIDSFs/data/TauID_SF_pt_DeepTau2017v2p1VSjet_";
  path += year;
  if(year == 2016) path += "Legacy";
  else             path += "ReReco";

  TFile* f_mc  = TFile::Open((path + ".root"    ).Data(), "READ");
  TFile* f_emb = TFile::Open((path + "_EMB.root").Data(), "READ");
  if(!f_mc || !f_emb) return 1;

  TF1* mc  = (TF1*) f_mc ->Get("Tight_cent");
  TF1* emb = (TF1*) f_emb->Get("Tight_cent");
  if(!mc || !emb) {
    cout << __func__ << ": Functions not found for year " << year << endl;
    return 2;
  }

  const double xbins[] = {20., 25., 30., 35., 40., 100.};
  const int nxbins = sizeof(xbins)/sizeof(*xbins) - 1;
  TH1* hratio = new TH1F(Form("mc_v_emb_ratio_%i", year), "", nxbins, xbins);
  for(int index = 0; index < nxbins; ++index) {
    const double x = (xbins[index] + xbins[index+1])/2.;
    const double r = emb->Eval(x) / mc->Eval(x);
    hratio->SetBinContent(index+1, r);
  }

  TCanvas c("c", "c", 1000, 1100);
  TPad pad1("pad1", "pad1", 0., 0.3, 1., 1.0); pad1.Draw();
  TPad pad2("pad2", "pad2", 0., 0.0, 1., 0.3); pad2.Draw();
  pad1.SetBottomMargin(0.05); pad2.SetTopMargin(0.03);

  pad1.cd();
  mc ->SetNpx(10000);
  emb->SetNpx(10000);
  gStyle->SetOptStat(0);

  mc ->SetLineColor(kBlue);
  mc ->SetLineWidth(2);
  mc ->Draw("L");
  emb->SetLineColor(kRed);
  emb->SetLineWidth(2);
  emb->Draw("L same");

  mc->SetTitle("MC vs. Embedding Tau ID SFs");
  mc->GetXaxis()->SetRangeUser(20., 100.);
  mc->GetYaxis()->SetRangeUser(0.80, 1.10);

  TLegend leg(0.6, 0.7, 0.9, 0.9);
  leg.AddEntry(mc , "MC");
  leg.AddEntry(emb, "Embedding");
  leg.Draw();

  pad2.cd();
  hratio->SetLineWidth(2);
  hratio->SetLineColor(kBlue);
  hratio->SetYTitle("Embed / MC");
  hratio->Draw("hist");
  hratio->GetYaxis()->SetTitleSize(0.13);
  hratio->GetYaxis()->SetTitleOffset(0.35);
  hratio->GetYaxis()->SetLabelSize(0.08);
  hratio->GetXaxis()->SetLabelSize(0.08);

  c.SaveAs(Form("figures/emb_v_mc_tau_id_%i.png", year));

  delete hratio;
  f_mc->Close();
  f_emb->Close();

  return 0;
}
