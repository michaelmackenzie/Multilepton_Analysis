//Print plots of the Z->e+mu vs. WW distributions in the BDT training window
#include "dataplotter_clfv.C"

int print_plot(PlottingCard_t card) {
  ////////////////////////////////////////
  // Retrieve the histograms

  auto bkg = dataplotter_->get_process("WW", card.hist_, card.type_, card.set_);
  auto sigs = dataplotter_->get_signal(card.hist_, card.type_, card.set_);
  if(sigs.size() == 0 || !bkg) return 1;
  auto sig = sigs[0];

  bkg = (TH1*) bkg->Clone(Form("bkg_%s", card.hist_.Data()));
  sig = (TH1*) sig->Clone(Form("sig_%s", card.hist_.Data()));

  //Rebin if requested
  if(card.rebin_ > 1) {
    bkg->Rebin(card.rebin_);
    sig->Rebin(card.rebin_);
  }

  bkg->Scale(1./bkg->Integral());
  sig->Scale(1./sig->Integral());

  ////////////////////////////////////////
  // Plot the histograms

  TCanvas c("c","c",900,1200);
  TPad pad1("pad1", "pad1", 0., 0.3, 1., 1.0); pad1.Draw();
  TPad pad2("pad2", "pad2", 0., 0.0, 1., 0.3); pad2.Draw();
  pad1.SetBottomMargin(0.05);
  pad2.SetTopMargin   (0.05);
  pad2.SetBottomMargin(0.22);

  pad1.cd();
  bkg->SetLineColor(kRed);
  bkg->SetLineWidth(3);
  bkg->SetFillColor(0);
  bkg->Draw("hist");
  bkg->SetTitle("");

  sig->SetLineColor(kBlue);
  sig->SetLineWidth(3);
  sig->SetFillColor(0);
  sig->Draw("hist same");

  double xmin = card.xmin_;
  double xmax = card.xmax_;
  if(xmin >= xmax) {xmin = bkg->GetXaxis()->GetXmin(); xmax = bkg->GetXaxis()->GetXmax();}
  bkg->GetXaxis()->SetRangeUser(xmin, xmax);

  if(card.ymin_ < card.ymax_) bkg->GetYaxis()->SetRangeUser(card.ymin_, card.ymax_);
  else {
    const double max_val = max(bkg->GetMaximum(), sig->GetMaximum());
    bkg->GetYaxis()->SetRangeUser(1.e-4*max_val, 1.2*max_val);
  }

  TLegend leg(0.20, 0.82, 0.85, 0.88);
  leg.SetNColumns(2);
  leg.SetLineWidth(0);
  leg.SetFillColor(0);
  leg.AddEntry(sig, "Signal");
  leg.AddEntry(bkg, "Background");
  leg.Draw();

  ////////////////////////////////////////
  // Plot the ratio

  pad2.cd();
  TH1* hratio = (TH1*) sig->Clone(Form("%s_ratio", sig->GetName()));
  hratio->Divide(bkg);
  hratio->SetLineWidth(2);
  hratio->SetMarkerSize(0.8);
  hratio->SetMarkerStyle(20);
  hratio->SetMarkerColor(kBlack);
  hratio->Draw("E1");
  hratio->SetTitle("");
  hratio->SetYTitle("Signal / Background");
  hratio->GetYaxis()->SetRangeUser(0., 3.);
  hratio->GetXaxis()->SetLabelSize(0.07);
  hratio->GetYaxis()->SetLabelSize(0.07);
  hratio->GetYaxis()->SetTitleSize(0.10);
  hratio->GetYaxis()->SetTitleOffset(0.50);
  hratio->GetXaxis()->SetRangeUser(xmin, xmax);
  hratio->GetXaxis()->SetTitleSize(0.10);
  hratio->GetXaxis()->SetTitleOffset(0.90);
  TString xtitle, ytitle, title;
  Titles::get_titles(card.hist_, card.type_, selection_, &xtitle, &ytitle, &title);
  hratio->SetXTitle(xtitle.Data());

  TLine line(xmin, 1., xmax, 1.);
  line.SetLineColor(kBlack);
  line.SetLineWidth(2);
  line.SetLineStyle(kDashed);
  line.Draw("same");

  TString fig_name = dataplotter_->GetFigureName(card.type_, card.hist_, card.set_, "signal", true);
  c.SaveAs(fig_name.Data());

  //clean up
  delete hratio;
  delete bkg;
  delete sig;

  return 0;
}

int print_emu_bdt_training_plots(TString out_dir = "emu_bdt_training", TString hist_dir = "nanoaods_emu_dev",
                                 TString selection = "emu", vector<int> years = {2016,2017,2018}) {

  //setup the datacards
  years_         = years;
  hist_dir_      = hist_dir;
  hist_tag_      = "clfv";
  skipData_      = 1; //don't need data samples
  useEmbed_      = 0;
  combineWW_     = 0; //separate out the WW sample

  cout << "Initializing the dataplotter\n";
  if(nanoaod_init(selection, hist_dir, out_dir)) return 1;

  int status(0);

  cout << "Printing figures\n";
  const int set = 24 + get_offset();
  status += print_plot(PlottingCard_t("onept"          , "lep"  , set, 2, 20.,  70.));
  status += print_plot(PlottingCard_t("twopt"          , "lep"  , set, 2, 20.,  70.));
  status += print_plot(PlottingCard_t("pttrailoverlead", "lep"  , set, 2,  0.,   1.));
  status += print_plot(PlottingCard_t("oneeta"         , "lep"  , set, 2,  1.,  -1.));
  status += print_plot(PlottingCard_t("twoeta"         , "lep"  , set, 2,  1.,  -1));
  status += print_plot(PlottingCard_t("lepm"           , "event", set, 5, 40., 100.));
  status += print_plot(PlottingCard_t("leppt"          , "event", set, 2,  0.,  70.));
  status += print_plot(PlottingCard_t("lepeta"         , "event", set, 2,  1.,  -1.));
  status += print_plot(PlottingCard_t("lepdeltaphi"    , "event", set, 2, 0.5,  3.2));
  status += print_plot(PlottingCard_t("met"            , "event", set, 2,  0.,  70.));
  status += print_plot(PlottingCard_t("mtlep"          , "event", set, 2,  0.,  70.));
  status += print_plot(PlottingCard_t("mtone"          , "event", set, 2,  0., 100.));
  status += print_plot(PlottingCard_t("mttwo"          , "event", set, 2,  0., 100.));
  status += print_plot(PlottingCard_t("mttrail"        , "event", set, 1,  0., 105.));
  status += print_plot(PlottingCard_t("mtlead"         , "event", set, 1,  0., 150.));
  status += print_plot(PlottingCard_t("onemetdeltaphi" , "lep"  , set, 1,  0.,  3.2));
  status += print_plot(PlottingCard_t("twometdeltaphi" , "lep"  , set, 1,  0.,  3.2));
  status += print_plot(PlottingCard_t("metdeltaphi"    , "event", set, 1,  0.,  3.2));
  status += print_plot(PlottingCard_t("beta0"          , "event", set, 2,  0.5,   2.5));
  status += print_plot(PlottingCard_t("beta1"          , "event", set, 2,  0.5,   2.5));
  status += print_plot(PlottingCard_t("nupt"           , "event", set, 1,  0.,  40.));
  status += print_plot(PlottingCard_t("mva5"           , "event", set, 1,  0.,  1.));


  return status;
}
