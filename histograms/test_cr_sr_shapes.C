//Compare the signal shape in a control region vs. signal region
#include "dataplotter_clfv.C"
const bool print_ = true;

int test_cr_sr_shapes(TString selection = "etau", TString hist_dir = "nanoaods_mva_dev") {
  //setup the datacards
  years_ = {2016,2017,2018};
  hist_dir_ = hist_dir;
  useEmbed_ = 1;
  int status = nanoaod_init(selection.Data(), hist_dir_, "cr_sr_contamination");
  if(status) return status;

  //set the BDT observable name
  TString hist = "mva";
  if     (selection == "mutau"  ) hist += "1";
  else if(selection == "etau"   ) hist += "3";
  else if(selection == "mutau_e") hist += "7";
  else if(selection == "etau_mu") hist += "9";
  else return -1;

  //determine the set to plot
  const int set_sr = 8 + get_offset();
  const int set_cr = (selection.Contains("_")) ? HistMaker::fQcdOffset + set_sr : HistMaker::fMisIDOffset + set_sr;

  //get the BDT score distribution
  auto hists_sr  = dataplotter_->get_signal(hist, "event", set_sr);
  auto hists_cr  = dataplotter_->get_signal(hist, "event", set_cr);
  if(hists_sr.size() == 0 || hists_cr.size() == 0) return 10;
  if(hists_sr.size() > 1 || hists_cr.size() > 1) {
    cout << "Too many histograms found\n";
    return 20;
  }
  TH1* h_sr = (TH1*) hists_sr[0];
  TH1* h_cr = (TH1*) hists_cr[0];

  if(h_sr->Integral() <= 0. || h_cr->Integral() <= 0.) {
    cout << "Integrals are not both positive\n";
    return 30;
  }

  //Draw the results
  h_sr->SetLineColor(kBlue);
  h_cr->SetLineColor(kRed);

  h_sr->SetFillColor(0);
  h_cr->SetFillColor(0);

  h_sr->SetLineWidth(2);
  h_cr->SetLineWidth(2);

  TCanvas* c = new TCanvas("c","c",1000,1000);
  TPad* pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1.0); pad1->Draw();
  TPad* pad2 = new TPad("pad2", "pad2", 0., 0.0, 1., 0.3); pad2->Draw();
  pad1->SetBottomMargin(0.05); pad2->SetTopMargin(0.03);

  pad1->cd();
  pad1->SetGrid();

  h_sr->Draw("hist");
  h_cr->Draw("hist same");

  h_sr->GetXaxis()->SetRangeUser(0., 1.);

  TLegend* leg = new TLegend(0.6, 0.7, 0.9, 0.9);
  leg->AddEntry(h_sr, "Signal region");
  leg->AddEntry(h_cr, "Control region");
  leg->Draw();

  h_sr->SetTitle("");
  h_sr->SetXTitle("");

  pad2->cd();
  TH1* h_ratio = (TH1*) h_cr->Clone("h_ratio");
  h_ratio->Divide(h_sr);
  h_ratio->Draw("E1");
  h_ratio->SetTitle("");
  h_ratio->SetXTitle("");
  h_ratio->SetYTitle("CR / SR");
  h_ratio->GetYaxis()->SetTitleSize(0.12);
  h_ratio->GetYaxis()->SetTitleOffset(0.40);
  h_ratio->GetYaxis()->SetLabelSize(0.08);
  h_ratio->GetXaxis()->SetLabelSize(0.08);
  h_ratio->GetXaxis()->SetRangeUser(0.,1.);
  //set the maximum using the ratio at a reasonable place
  const double ratio_value = h_ratio->GetBinContent(h_ratio->FindBin(0.5));
  const double ratio_error = h_ratio->GetBinError  (h_ratio->FindBin(0.5));
  h_ratio->GetYaxis()->SetRangeUser(0., max(ratio_value + 2.*ratio_error, 2.*ratio_value));

  //Fit the high BDT score region
  if(selection.Contains("_"))
    h_ratio->Fit("pol0", "R", "0", 0., 1.);
  else
    h_ratio->Fit("pol0", "R", "0", 0.5, 1.);

  //Save the figure with both linear and log scale
  h_sr->GetYaxis()->SetRangeUser(0.1, 1.2*h_sr->GetMaximum());
  if(print_) {
    c->SaveAs(Form("figures/cr_sr_contamination/%s/2016_2017_2018/cr_sr_contamination.png", selection.Data()));
    h_sr->GetYaxis()->SetRangeUser(0.1, 5.*h_sr->GetMaximum());
    pad1->SetLogy();
    c->SaveAs(Form("figures/cr_sr_contamination/%s/2016_2017_2018/cr_sr_contamination_log.png", selection.Data()));
  }
  return 0;
}
