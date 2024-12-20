//Plot the W+jets emu backgrounds in OS and SS
#include "dataplotter_clfv.C"

int test_wjets_emu_bkg(TString selection = "mutau_e", TString hist_dir = "nanoaods_mva_dev") {
  //setup the datacards
  years_ = {2016,2017,2018};
  hist_dir_ = hist_dir;
  useEmbed_ = 0; //doesn't matter
  combineVB_ = 0; //separate out W+jets MC
  int status = nanoaod_init(selection, hist_dir_, "wjets_emu_bkg_no_j0");
  if(status) return status;

  //determine the set to plot
  const int set = 8 + get_offset();

  for(TString hist : {"lepm", "mva", "onept", "twopt", "met", "mtone", "mttwo", "eleconvveto", "elelosthits"}) {
    if(hist == "mva" && selection == "mutau_e") hist += "7";
    else if(hist == "mva" && selection == "etau_mu") hist += "9";

    //set the rebinning
    if     (hist == "lepm" ) dataplotter_->rebinH_ = 5;
    else if(hist == "mtone") dataplotter_->rebinH_ = 4;
    else if(hist == "mttwo") dataplotter_->rebinH_ = 4;
    else if(hist == "mtlep") dataplotter_->rebinH_ = 4;
    else if(hist == "met"  ) dataplotter_->rebinH_ = 4;
    else if(hist == "onept") dataplotter_->rebinH_ = 2;
    else if(hist == "twopt") dataplotter_->rebinH_ = 2;
    else                     dataplotter_->rebinH_ = 1;

    //get the type
    TString type("event");
    if(hist == "eleconvveto" ) type = "lep";
    if(hist == "elelosthits" ) type = "lep";
    if(hist.BeginsWith("one")) type = "lep";
    if(hist.BeginsWith("two")) type = "lep";

    //get the distributions
    TH1* h_os   = dataplotter_->get_process("W+Jets", hist, type, set);
    TH1* h_ss   = dataplotter_->get_process("W+Jets", hist, type, set+HistMaker::fQcdOffset);
    if(!h_os || !h_ss) continue;

    if(h_os->Integral() <= 0. || h_ss->Integral() <= 0.) {
      cout << "Integrals are not both positive\n";
      continue;
    }

    //Draw the results
    h_os->SetLineColor(kBlue);
    h_ss->SetLineColor(kRed);

    h_os->SetFillColor(0);
    h_ss->SetFillColor(0);

    h_os->SetLineWidth(2);
    h_ss->SetLineWidth(2);

    //get the first/last relevant bin
    const int bin_low  = min(h_os->FindFirstBinAbove(0.), h_ss->FindFirstBinAbove(0.));
    const int bin_high = max(h_os->FindLastBinAbove (0.), h_ss->FindLastBinAbove (0.));
    h_os->GetXaxis()->SetRange(bin_low, bin_high);
    h_os->GetYaxis()->SetRangeUser(0., 1.3*max(h_os->GetMaximum(), h_ss->GetMaximum()));

    TCanvas* c = new TCanvas("c","c",1000,1000);
    TPad* pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1.0); pad1->Draw();
    TPad* pad2 = new TPad("pad2", "pad2", 0., 0.0, 1., 0.3); pad2->Draw();
    pad1->SetBottomMargin(0.05); pad2->SetTopMargin(0.03);

    pad1->cd();
    pad1->SetGrid();

    h_os->Draw("hist E1");
    h_ss->Draw("E1 same");

    TLegend* leg = new TLegend(0.6, 0.75, 0.9, 0.9);
    leg->AddEntry(h_os, Form("OS: %.1f", h_os->Integral()));
    leg->AddEntry(h_ss, Form("SS: %.1f", h_ss->Integral()));
    leg->Draw();

    h_os->SetTitle(Form("W+jets %s OS vs. SS", hist.Data()));
    h_os->SetXTitle("");

    pad2->cd();
    TH1* h_ratio = (TH1*) h_os->Clone("h_ratio");
    h_ratio->Divide(h_ss);
    h_ratio->Draw("E1");
    h_ratio->SetTitle("");
    h_ratio->SetXTitle("");
    h_ratio->SetYTitle("OS / SS");
    h_ratio->GetYaxis()->SetTitleSize(0.12);
    h_ratio->GetYaxis()->SetTitleOffset(0.40);
    h_ratio->GetYaxis()->SetLabelSize(0.08);
    h_ratio->GetXaxis()->SetLabelSize(0.08);
    h_ratio->GetXaxis()->SetRange(bin_low, bin_high);
    h_ratio->GetYaxis()->SetRangeUser(0., 3.);
    pad2->SetGrid();

    c->SaveAs(Form("figures/wjets_emu_bkg_no_j0/%s/2016_2017_2018/compare_%s.png", selection.Data(), hist.Data()));
    delete c;
  }
  return 0;
}
