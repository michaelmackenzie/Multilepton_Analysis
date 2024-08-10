//Compare the W+jets 0-jet component to 1-4-jet components
#include "dataplotter_clfv.C"

int compare_wjets_0j(TString selection = "mutau_e", TString hist_dir = "nanoaods_mva_dev") {
  //setup the datacards
  years_ = {2016,2017,2018};
  hist_dir_ = hist_dir;
  useEmbed_ = 0; //doesn't matter
  combineVB_ = 0; //separate out W+jets MC from WW/WWW/ZZ/etc.
  combineWJ_ = 0; //separate W+jets 0-J from 1-4-J
  excludeWJ0J_ = 0; //keep the 0-jet sample
  doStatsLegend_ = 1; //add statistics
  int status = nanoaod_init(selection, hist_dir_, "compare_wjets_0j");
  if(status) return status;

  //determine the set to plot

  for(int region = 0; region < 2; ++region) {
    const int set = 8 + get_offset() + region*HistMaker::fQcdOffset;
    for(TString hist : {"lepm", "leppt", "mva", "onegenflavor", "twogenflavor"}) {
      if(hist == "mva" && selection == "mutau_e") hist += "7";
      else if(hist == "mva" && selection == "etau_mu") hist += "9";
      dataplotter_->blindxmin_ = {};
      dataplotter_->blindxmax_ = {};

      //set the rebinning
      dataplotter_->rebinH_ = 1;
      if     (hist == "lepm" ) dataplotter_->rebinH_ = 5;
      else if(hist == "leppt") dataplotter_->rebinH_ = 2;

      //get the type
      TString type("event");
      if(hist.BeginsWith("one")) type = "lep";
      if(hist.BeginsWith("two")) type = "lep";

      //get the distributions
      TH1* h_wjet_0j = dataplotter_->get_process("W+Jets-0J", hist, type, set);
      TH1* h_wjet_nj = dataplotter_->get_process("W+Jets"   , hist, type, set);
      if(!h_wjet_0j || !h_wjet_nj) continue;

      if(h_wjet_0j->Integral() <= 0. || h_wjet_nj->Integral() <= 0.) {
        cout << "Integrals are not both positive\n";
        continue;
      }

      //Draw the results
      h_wjet_0j->SetLineColor(kBlue);
      h_wjet_nj->SetLineColor(kRed);

      h_wjet_0j->SetFillColor(0);
      h_wjet_nj->SetFillColor(0);

      h_wjet_0j->SetLineWidth(2);
      h_wjet_nj->SetLineWidth(2);

      //get the first/last relevant bin
      const int bin_low  = min(h_wjet_0j->FindFirstBinAbove(0.), h_wjet_nj->FindFirstBinAbove(0.));
      const int bin_high = max(h_wjet_0j->FindLastBinAbove (0.), h_wjet_nj->FindLastBinAbove (0.));
      h_wjet_0j->GetXaxis()->SetRange(bin_low, bin_high);
      h_wjet_0j->GetYaxis()->SetRangeUser(0., 1.3*max(h_wjet_0j->GetMaximum(), h_wjet_nj->GetMaximum()));

      TCanvas* c = new TCanvas("c","c",1000,1000);
      TPad* pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1.0); pad1->Draw();
      TPad* pad2 = new TPad("pad2", "pad2", 0., 0.0, 1., 0.3); pad2->Draw();
      pad1->SetBottomMargin(0.05); pad2->SetTopMargin(0.03);

      pad1->cd();
      pad1->SetGrid();

      h_wjet_0j->Draw("hist E1");
      h_wjet_nj->Draw("E1 same");

      TLegend* leg = new TLegend(0.6, 0.75, 0.9, 0.9);
      leg->AddEntry(h_wjet_0j, Form("0-jets: %.1f", h_wjet_0j->Integral()));
      leg->AddEntry(h_wjet_nj, Form("N-jets: %.1f", h_wjet_nj->Integral()));
      leg->Draw();

      h_wjet_0j->SetTitle(Form("W+jets %s 0 vs. N jets", hist.Data()));
      h_wjet_0j->SetXTitle("");

      pad2->cd();
      TH1* h_ratio = (TH1*) h_wjet_0j->Clone("h_ratio");
      h_ratio->Divide(h_wjet_nj);
      h_ratio->Draw("E1");
      h_ratio->SetTitle("");
      h_ratio->SetXTitle("");
      h_ratio->SetYTitle("0-jet / N-jet");
      h_ratio->GetYaxis()->SetTitleSize(0.12);
      h_ratio->GetYaxis()->SetTitleOffset(0.40);
      h_ratio->GetYaxis()->SetLabelSize(0.08);
      h_ratio->GetXaxis()->SetLabelSize(0.08);
      h_ratio->GetXaxis()->SetRange(bin_low, bin_high);
      h_ratio->GetYaxis()->SetRangeUser(0., 3.);
      pad2->SetGrid();

      c->SaveAs(Form("figures/compare_wjets_0j/%s/2016_2017_2018/compare_%s_%i.png", selection.Data(), hist.Data(), set));
      delete c;

      //create a stack for the region
      if(hist.BeginsWith("mva") && set < 1000) {
        dataplotter_->blindxmin_ = {0.5};
        dataplotter_->blindxmax_ = {1.0};
      }
      dataplotter_->logY_ = hist.BeginsWith("mva");
      dataplotter_->print_stack(hist, type, set, h_ratio->GetXaxis()->GetBinLowEdge(bin_low), h_ratio->GetXaxis()->GetBinUpEdge(bin_high));
    }
  }
  return 0;
}
