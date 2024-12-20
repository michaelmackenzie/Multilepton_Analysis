//Plot the fraction of the background that is tau_mu tau_mu in the mumu selection
#include "dataplotter_clfv.C"

int test_mumu_tau_contamination(TString hist_dir = "nanoaods_mva_dev") {
  //setup the datacards
  years_ = {2016,2017,2018};
  hist_dir_ = hist_dir;
  useEmbed_ = 0;
  int status = nanoaod_init("mumu", hist_dir_, "mumu_contamination");
  if(status) return status;

  //determine the set to plot
  const int set = 8 + get_offset();

  //get the BDT score distribution
  TH1* h_mumu   = dataplotter_->get_process("Z->ee/#mu#mu"  , "lepm", "event", set);
  TH1* h_tautau = dataplotter_->get_process("Z->#tau#tau", "lepm", "event", set);
  if(!h_mumu || !h_tautau) return 10;

  if(h_mumu->Integral() <= 0. || h_tautau->Integral() <= 0.) {
    cout << "Integrals are not both positive\n";
    return 2;
  }

  //Draw the results
  h_mumu->SetLineColor(kBlue);
  h_tautau->SetLineColor(kRed);

  h_mumu->SetFillColor(0);
  h_tautau->SetFillColor(0);

  h_mumu->SetLineWidth(2);
  h_tautau->SetLineWidth(2);

  TCanvas* c = new TCanvas("c","c",1000,1000);
  TPad* pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1.0); pad1->Draw();
  TPad* pad2 = new TPad("pad2", "pad2", 0., 0.0, 1., 0.3); pad2->Draw();
  pad1->SetBottomMargin(0.05); pad2->SetTopMargin(0.03);

  pad1->cd();
  pad1->SetGrid();

  h_mumu->Draw("hist");
  h_tautau->Draw("hist same");

  h_mumu->GetXaxis()->SetRangeUser(40., 120.);
  h_mumu->GetYaxis()->SetRangeUser(10., 5.e7);
  pad1->SetLogy();

  TLegend* leg = new TLegend(0.1, 0.75, 0.4, 0.9);
  leg->AddEntry(h_mumu  , "Z->#mu#mu"  );
  leg->AddEntry(h_tautau, "Z->#tau#tau");
  leg->Draw();

  h_mumu->SetTitle("");
  h_mumu->SetXTitle("");

  pad2->cd();
  TH1* h_ratio = (TH1*) h_tautau->Clone("h_ratio");
  TH1* h_tot = (TH1*) h_mumu->Clone("h_tot");
  h_tot->Add(h_tautau);
  h_ratio->Divide(h_tot);
  h_ratio->Draw("E1");
  h_ratio->SetTitle("");
  h_ratio->SetXTitle("");
  h_ratio->SetYTitle("#tau#tau / Total");
  h_ratio->GetYaxis()->SetTitleSize(0.12);
  h_ratio->GetYaxis()->SetTitleOffset(0.40);
  h_ratio->GetYaxis()->SetLabelSize(0.08);
  h_ratio->GetXaxis()->SetLabelSize(0.08);
  h_ratio->GetXaxis()->SetRangeUser(40., 120.);
  h_ratio->GetYaxis()->SetRangeUser(0.01, 0.5);
  pad2->SetLogy();
  pad2->SetGrid();

  c->SaveAs(Form("figures/mumu_contamination/mumu/2016_2017_2018/mumu_contamination.png"));
  return 0;
}
