//Compare Embedding and Drell-Yan MC
#include "dataplotter_clfv.C"
DataPlotter* embed_plotter_;
DataPlotter* mc_dy_plotter_;

int compare_hist(PlottingCard_t card, const bool fit_ratio = false) {
  ////////////////////////////////////////
  // Retrieve the histograms

  TString embed_name = (selection_ == "mumu") ? "#mu#mu Embedding" : (selection_ == "ee") ? "ee Embedding" : "#tau#tau Embedding";
  auto embed_hists = embed_plotter_->get_histograms(card.hist_, card.type_, card.set_, DataPlotter::kAny, embed_name);
  if(embed_hists.size() == 0) {
    cout << "No Embedding histogram found for " << card.hist_.Data() << "/" << card.type_.Data() << "/" << card.set_ << endl;
    return 1;
  }
  TH1* h_embed = (TH1*) embed_hists[0]->Clone(Form("h_%s_%s_%i_embed", card.hist_.Data(), card.type_.Data(), card.set_));
  delete embed_hists[0];
  for(unsigned index = 1; index < embed_hists.size(); ++index) {h_embed->Add(embed_hists[index]); delete embed_hists[index];}
  TString mc_dy_name = (selection_ == "mumu") ? "Z->ee/#mu#mu" : (selection_ == "ee") ? "Z->ee/#mu#mu" : "Z->#tau#tau";
  auto mc_dy_hists = mc_dy_plotter_->get_histograms(card.hist_, card.type_, card.set_, DataPlotter::kAny, mc_dy_name);
  if(mc_dy_hists.size() == 0) {
    cout << "No MC DY histogram found for " << card.hist_.Data() << "/" << card.type_.Data() << "/" << card.set_ << endl;
    return 1;
  }
  TH1* h_mc_dy = (TH1*) mc_dy_hists[0]->Clone(Form("h_%s_%s_%i_mc_dy", card.hist_.Data(), card.type_.Data(), card.set_));
  delete mc_dy_hists[0];
  for(unsigned index = 1; index < mc_dy_hists.size(); ++index) {h_mc_dy->Add(mc_dy_hists[index]); delete mc_dy_hists[index];}

  //Rebin if requested
  if(card.rebin_ > 1) {
    h_embed->Rebin(card.rebin_);
    h_mc_dy->Rebin(card.rebin_);
  }

  ////////////////////////////////////////
  // Plot the histograms

  TCanvas c("c","c",900,1200);
  TPad pad1("pad1", "pad1", 0., 0.3, 1., 1.0); pad1.Draw();
  TPad pad2("pad2", "pad2", 0., 0.0, 1., 0.3); pad2.Draw();
  pad1.SetBottomMargin(0.05);
  pad2.SetTopMargin   (0.05);
  pad2.SetBottomMargin(0.22);

  pad1.cd();
  h_mc_dy->SetLineColor(kRed);
  h_mc_dy->SetLineWidth(2);
  h_mc_dy->SetFillColor(0);
  h_mc_dy->Draw("hist E1");
  h_mc_dy->SetTitle("MC Drell-Yan vs. Embedding");

  h_embed->SetLineColor(kBlue);
  h_embed->SetLineWidth(2);
  h_embed->SetFillColor(0);
  h_embed->Draw("hist E1 same");

  double xmin = card.xmin_;
  double xmax = card.xmax_;
  if(xmin >= xmax) {xmin = h_embed->GetXaxis()->GetXmin(); xmax = h_embed->GetXaxis()->GetXmax();}

  h_mc_dy->GetXaxis()->SetRangeUser(xmin, xmax);
  if(card.ymin_ < card.ymax_) h_mc_dy->GetYaxis()->SetRangeUser(card.ymin_, card.ymax_);
  else {
    if(card.hist_.EndsWith("eloss")) {
      h_mc_dy->GetYaxis()->SetRangeUser(0.5, 3.*max(h_mc_dy->GetMaximum(), h_embed->GetMaximum()));
      pad1.SetLogy();
    } else {
      h_mc_dy->GetYaxis()->SetRangeUser(0.1, 1.25*max(h_mc_dy->GetMaximum(), h_embed->GetMaximum()));
    }
  }

  TLegend leg(0.4, 0.75, 0.9, 0.9);
  leg.AddEntry(h_embed, Form("Embedding: %.0f"   , h_embed->Integral(0, h_embed->GetNbinsX()+1)));
  leg.AddEntry(h_mc_dy, Form("MC Drell-Yan: %.0f", h_mc_dy->Integral(0, h_mc_dy->GetNbinsX()+1)));
  leg.Draw();

  ////////////////////////////////////////
  // Plot the ratio

  pad2.cd();
  TH1* hratio = (TH1*) h_mc_dy->Clone(Form("%s_ratio", h_mc_dy->GetName()));
  hratio->Divide(h_embed);
  hratio->SetLineWidth(2);
  hratio->SetMarkerSize(0.8);
  hratio->SetMarkerStyle(20);
  hratio->SetMarkerColor(kBlack);
  hratio->Draw("E1");
  hratio->SetTitle("");
  hratio->SetYTitle("MC DY / Embed");
  hratio->GetYaxis()->SetRangeUser(0.7, 1.3);
  hratio->GetXaxis()->SetLabelSize(0.07);
  hratio->GetYaxis()->SetLabelSize(0.07);
  hratio->GetYaxis()->SetTitleSize(0.10);
  hratio->GetYaxis()->SetTitleOffset(0.50);
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

  const int fit_min = max(xmin, hratio->GetBinLowEdge(hratio->FindFirstBinAbove(0.)));
  TF1* fit_func = (fit_ratio) ? new TF1("fit_func", "[0] + [1]*x", fit_min, xmax) : nullptr;
  TH1* h_1s = nullptr;
  if(fit_ratio) {
    gStyle->SetOptFit(0);
    fit_func->SetParameters(1., 0.);
    hratio->Fit(fit_func, "R");

    //Get the 1 sigma band
    h_1s = (TH1*) hratio->Clone("h_1s");
    h_1s->Reset();
    (TVirtualFitter::GetFitter())->GetConfidenceIntervals(h_1s, 0.68);
    h_1s->SetFillColor(kGreen);
    h_1s->SetMarkerSize(0);
    h_1s->Draw("E3 same");
    fit_func->Draw("same");
    hratio->Draw("E1 same");

    //draw the fit results
    TLatex label;
    label.SetNDC();
    label.SetTextFont(72);
    label.SetTextAlign(13);
    label.SetTextAngle(0);
    label.SetTextSize(0.06);
    label.DrawLatex(0.55, 0.30, Form("b = %.2f#pm %.2f; m = %.3f#pm %.3f",
                                     fit_func->GetParameter(0), fit_func->GetParError(0),
                                     fit_func->GetParameter(1), fit_func->GetParError(1)));
  }

  TString fig_name = embed_plotter_->GetFigureName(card.type_, card.hist_, card.set_, "signal", true);
  c.SaveAs(fig_name.Data());

  //clean up
  delete hratio;
  delete h_mc_dy;
  delete h_embed;
  if(fit_ratio) delete fit_func;
  if(h_1s) delete h_1s;

  return 0;
}

int compare_dy_embed(TString out_dir = "compare_dy_embed", TString hist_dir = "nanoaods_embed_met",
                     TString selection = "mutau", vector<int> years = {2018}) {

  //setup the datacards
  years_         = years;
  hist_dir_      = hist_dir;
  hist_tag_      = "clfv";
  drawStats_     = 1; //data and MC total integrals
  doStatsLegend_ = 1; //process yields in the legend
  skipData_      = 1; //don't need data samples

  //Initialize the Embedding dataplotter
  cout << "Initializing Embedding dataplotter\n";
  useEmbed_ = 2;
  if(nanoaod_init(selection, hist_dir, out_dir)) return 1;
  embed_plotter_ = dataplotter_;
  dataplotter_ = nullptr;

  //Initialize the MC DY dataplotter
  cout << "Initializing MC DY dataplotter\n";
  useEmbed_ = 0;
  if(nanoaod_init(selection, hist_dir, out_dir)) return 2;
  mc_dy_plotter_ = dataplotter_;
  dataplotter_ = nullptr;

  const bool is_sf = selection == "mumu" || selection == "ee";

  int status(0);

  cout << "Printing figures\n";

  // //cutflow
  // status += compare_hist(PlottingCard_t("onept", "lep", 1+get_offset(), 2, 20., 70.));
  // status += compare_hist(PlottingCard_t("onept", "lep", 2+get_offset(), 2, 20., 70.));
  // status += compare_hist(PlottingCard_t("onept", "lep", 3+get_offset(), 2, 20., 70.));
  // status += compare_hist(PlottingCard_t("onept", "lep", 4+get_offset(), 2, 20., 70.));

  vector<int> sets;
  if(is_sf) sets = {8};
  else      sets = {8, 25, 27};

  for(int set : sets) {
    status += compare_hist(PlottingCard_t("onept"         , "lep"  , set+get_offset(), 2, 20.,  70.));
    status += compare_hist(PlottingCard_t("twopt"         , "lep"  , set+get_offset(), 2, (is_sf) ? 10. : 20.,  70.));
    status += compare_hist(PlottingCard_t("ptratio"       , "lep"  , set+get_offset(), 2,  0.,   2.));
    status += compare_hist(PlottingCard_t("oneptrelerr"   , "lep"  , set+get_offset(), 1,  1.,  -1.));
    status += compare_hist(PlottingCard_t("twoptrelerr"   , "lep"  , set+get_offset(), 1,  1.,  -1.));
    status += compare_hist(PlottingCard_t("oneeta"        , "lep"  , set+get_offset(), 2,  1.,  -1.));
    status += compare_hist(PlottingCard_t("twoeta"        , "lep"  , set+get_offset(), 2,  1.,  -1));
    status += compare_hist(PlottingCard_t("lepm"          , "event", set+get_offset(), 5, 40., 100.));
    status += compare_hist(PlottingCard_t("lepmestimate"  , "event", set+get_offset(), 2, 40., 200.));
    status += compare_hist(PlottingCard_t("leppt"         , "event", set+get_offset(), 2,  0.,  70.));
    status += compare_hist(PlottingCard_t("lepeta"        , "event", set+get_offset(), 2,  1.,  -1.));
    status += compare_hist(PlottingCard_t("lepphi"        , "event", set+get_offset(), 2,  1.,  -1.));
    status += compare_hist(PlottingCard_t("lepdeltaphi"   , "event", set+get_offset(), 2, 0.5,  3.2));
    status += compare_hist(PlottingCard_t("met"           , "event", set+get_offset(), 2,  0.,  70.));
    status += compare_hist(PlottingCard_t("mtlep"         , "event", set+get_offset(), 2,  0.,  70.));
    status += compare_hist(PlottingCard_t("mtone"         , "event", set+get_offset(), 2,  0.,  70.));
    status += compare_hist(PlottingCard_t("mttwo"         , "event", set+get_offset(), 2,  0.,  70.));
    status += compare_hist(PlottingCard_t("mtoneoverm"    , "event", set+get_offset(), 1,  0.,  1.5));
    status += compare_hist(PlottingCard_t("mttwooverm"    , "event", set+get_offset(), 1,  0.,  1.5));
    status += compare_hist(PlottingCard_t("metdotone"     , "event", set+get_offset(), 1,-50.,  50.));
    status += compare_hist(PlottingCard_t("metdottwo"     , "event", set+get_offset(), 1,-50.,  50.));
    status += compare_hist(PlottingCard_t("metdotzone"    , "event", set+get_offset(), 1,-50.,  50.));
    status += compare_hist(PlottingCard_t("metdotztwo"    , "event", set+get_offset(), 1,-50.,  50.));
    status += compare_hist(PlottingCard_t("onemetdeltaphi", "lep"  , set+get_offset(), 1,  0.,  3.2));
    status += compare_hist(PlottingCard_t("twometdeltaphi", "lep"  , set+get_offset(), 1,  0.,  3.2));
    status += compare_hist(PlottingCard_t("metdeltaphi"   , "event", set+get_offset(), 1,  0.,  3.2));
    status += compare_hist(PlottingCard_t("metoverleppt"  , "event", set+get_offset(), 1,  1.,  -1.));
    status += compare_hist(PlottingCard_t("beta0"         , "event", set+get_offset(), 2,  0.5,   2.5));
    status += compare_hist(PlottingCard_t("beta1"         , "event", set+get_offset(), 2,  0.5,   2.5));
    status += compare_hist(PlottingCard_t("detectormet"   , "event", set+get_offset(), 1,  0.,  60.));
    status += compare_hist(PlottingCard_t("nupt"          , "event", set+get_offset(), 1,  0.,  40.));
    status += compare_hist(PlottingCard_t("runera"        , "event", set+get_offset(), 1,  1.,  -1.));
    status += compare_hist(PlottingCard_t("npv"           , "event", set+get_offset(), 1,  0.,  60.));

    //gen-level Z info
    status += compare_hist(PlottingCard_t("zeta"        , "event", set+get_offset(), 1,  1.,  -1.));
    status += compare_hist(PlottingCard_t("zmass5"      , "event", set+get_offset(), 1, 70., 110.));
    status += compare_hist(PlottingCard_t("zleponept"   , "event", set+get_offset(), 1,  1.,  -1.));
    status += compare_hist(PlottingCard_t("zleptwopt"   , "event", set+get_offset(), 1,  1.,  -1.));
    status += compare_hist(PlottingCard_t("zleponeeta"  , "event", set+get_offset(), 2,  1.,  -1.));
    status += compare_hist(PlottingCard_t("zleptwoeta"  , "event", set+get_offset(), 2,  1.,  -1.));
    status += compare_hist(PlottingCard_t("zleponeeloss", "event", set+get_offset(), (is_sf) ? 1 : 5,  0.,  (is_sf) ? 0.1 : 1.));
    status += compare_hist(PlottingCard_t("zleptwoeloss", "event", set+get_offset(), (is_sf) ? 1 : 5,  0.,  (is_sf) ? 0.1 : 1.));
  }

  //print BDT distributions
  TString mva = "mva1"; //default to zmutau_h BDT
  if(selection == "mutau"  ) mva = "mva1";
  if(selection == "etau"   ) mva = "mva3";
  if(selection == "emu"    ) mva = "mva5";
  if(selection == "mutau_e") mva = "mva7";
  if(selection == "etau_mu") mva = "mva9";
  status += compare_hist(PlottingCard_t(mva, "event", 8 +get_offset(), 1,  0.25,  -1.), true);
  if(!is_sf) {
    status += compare_hist(PlottingCard_t(mva, "event", 25+get_offset(), 1,  1.,  -1.), true);
    status += compare_hist(PlottingCard_t(mva, "event", 26+get_offset(), 1,  1.,  -1.));
    status += compare_hist(PlottingCard_t(mva, "event", 27+get_offset(), 1,  1.,  -1.), true);
    status += compare_hist(PlottingCard_t(mva, "event", 29+get_offset(), 1,  1.,  -1.));
    if(!selection.Contains("_"))
      status += compare_hist(PlottingCard_t(mva, "event", 28+get_offset(), 1,  1.,  -1.), true);
  }
  if(selection == "mutau") { //debug regions
    for(int set = 30; set <= 48; ++set) status += compare_hist(PlottingCard_t(mva, "event", set+get_offset(), 1,  1.,  -1.));
    status += compare_hist(PlottingCard_t(mva, "event", 2008+get_offset(), 1,  1.,  -1.));
  }

  return status;
}
