//Compare Embedding and Drell-Yan MC
#include "dataplotter_clfv.C"
DataPlotter* embed_plotter_;
DataPlotter* mc_dy_plotter_;

int compare_hist(PlottingCard_t card) {
  ////////////////////////////////////////
  // Retrieve the histograms

  auto embed_hists = embed_plotter_->get_histograms(card.hist_, card.type_, card.set_, DataPlotter::kAny, "#tau#tau Embedding");
  if(embed_hists.size() == 0) return 1;
  TH1* h_embed = (TH1*) embed_hists[0]->Clone(Form("h_%s_%s_%i_embed", card.hist_.Data(), card.type_.Data(), card.set_));
  delete embed_hists[0];
  for(unsigned index = 1; index < embed_hists.size(); ++index) {h_embed->Add(embed_hists[index]); delete embed_hists[index];}
  auto mc_dy_hists = mc_dy_plotter_->get_histograms(card.hist_, card.type_, card.set_, DataPlotter::kAny, "Z->#tau#tau");
  if(mc_dy_hists.size() == 0) return 1;
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
  h_mc_dy->Draw("hist");
  h_mc_dy->SetTitle("MC Drell-Yan vs. Embedding");

  h_embed->SetLineColor(kBlue);
  h_embed->SetLineWidth(2);
  h_embed->SetFillColor(0);
  h_embed->Draw("hist same");

  double xmin = card.xmin_;
  double xmax = card.xmax_;
  if(xmin >= xmax) {xmin = h_embed->GetXaxis()->GetXmin(); xmax = h_embed->GetXaxis()->GetXmax();}

  h_mc_dy->GetXaxis()->SetRangeUser(xmin, xmax);
  if(card.ymin_ < card.ymax_) h_mc_dy->GetYaxis()->SetRangeUser(card.ymin_, card.ymax_);
  else h_mc_dy->GetYaxis()->SetRangeUser(0.1, 1.2*max(h_mc_dy->GetMaximum(), h_embed->GetMaximum()));

  TLegend leg(0.6, 0.75, 0.9, 0.9);
  leg.AddEntry(h_embed, "Embedding");
  leg.AddEntry(h_mc_dy, "MC Drell-Yan");
  leg.Draw();

  ////////////////////////////////////////
  // Plot the ratio

  pad2.cd();
  TH1* hratio = (TH1*) h_mc_dy->Clone(Form("%s_ratio", h_mc_dy->GetName()));
  hratio->Divide(h_embed);
  hratio->Draw("hist");
  hratio->SetTitle("");
  hratio->SetYTitle("MC DY / Embed");
  hratio->GetYaxis()->SetRangeUser(0.8, 1.2);
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

  TString fig_name = embed_plotter_->GetFigureName(card.type_, card.hist_, card.set_, "signal", true);
  c.SaveAs(fig_name.Data());

  //clean up
  delete hratio;
  delete h_mc_dy;
  delete h_embed;

  return 0;
}

int compare_dy_embed(TString out_dir = "tau_bdt_study", TString hist_dir = "nanoaods_mttwooverm_no_cut",
                     TString selection = "mutau", vector<int> years = {2016,2017,2018}) {

  //setup the datacards
  years_         = years;
  hist_dir_      = hist_dir;
  hist_tag_      = "clfv";
  drawStats_     = 1; //data and MC total integrals
  doStatsLegend_ = 1; //process yields in the legend

  //Initialize the Embedding dataplotter
  cout << "Initializing Embedding dataplotter\n";
  useEmbed_ = 1;
  if(nanoaod_init(selection, hist_dir, out_dir)) return 1;
  embed_plotter_ = dataplotter_;
  dataplotter_ = nullptr;

  //Initialize the MC DY dataplotter
  cout << "Initializing MC DY dataplotter\n";
  useEmbed_ = 0;
  if(nanoaod_init(selection, hist_dir, out_dir)) return 2;
  mc_dy_plotter_ = dataplotter_;
  dataplotter_ = nullptr;

  int status(0);

  //cutflow
  status += compare_hist(PlottingCard_t("onept", "lep", 1+get_offset(), 2, 20., 70.));
  status += compare_hist(PlottingCard_t("onept", "lep", 2+get_offset(), 2, 20., 70.));
  status += compare_hist(PlottingCard_t("onept", "lep", 3+get_offset(), 2, 20., 70.));
  status += compare_hist(PlottingCard_t("onept", "lep", 4+get_offset(), 2, 20., 70.));

  //nominal selection
  status += compare_hist(PlottingCard_t("onept"         , "lep"  , 8+get_offset(), 2, 20.,  70.));
  status += compare_hist(PlottingCard_t("twopt"         , "lep"  , 8+get_offset(), 2, 20.,  70.));
  status += compare_hist(PlottingCard_t("ptratio"       , "lep"  , 8+get_offset(), 2,  0.,   2.));
  status += compare_hist(PlottingCard_t("oneeta"        , "lep"  , 8+get_offset(), 2,  1.,  -1.));
  status += compare_hist(PlottingCard_t("twoeta"        , "lep"  , 8+get_offset(), 2,  1.,  -1));
  status += compare_hist(PlottingCard_t("lepm"          , "event", 8+get_offset(), 5, 40., 100.));
  status += compare_hist(PlottingCard_t("lepmestimate"  , "event", 8+get_offset(), 5, 40., 200.));
  status += compare_hist(PlottingCard_t("leppt"         , "event", 8+get_offset(), 2,  0.,  70.));
  status += compare_hist(PlottingCard_t("lepeta"        , "event", 8+get_offset(), 1,  1.,  -1.));
  status += compare_hist(PlottingCard_t("lepdeltaphi"   , "event", 8+get_offset(), 1,  1.,  -1.));
  status += compare_hist(PlottingCard_t("met"           , "event", 8+get_offset(), 2,  0.,  70.));
  status += compare_hist(PlottingCard_t("mtlep"         , "event", 8+get_offset(), 2,  0.,  70.));
  status += compare_hist(PlottingCard_t("mtone"         , "event", 8+get_offset(), 2,  0.,  70.));
  status += compare_hist(PlottingCard_t("mttwo"         , "event", 8+get_offset(), 2,  0.,  70.));
  status += compare_hist(PlottingCard_t("mtoneoverm"    , "event", 8+get_offset(), 1,  0.,  1.5));
  status += compare_hist(PlottingCard_t("mttwooverm"    , "event", 8+get_offset(), 1,  0.,  1.5));
  status += compare_hist(PlottingCard_t("onemetdeltaphi", "lep"  , 8+get_offset(), 1,  0.,  3.2));
  status += compare_hist(PlottingCard_t("twometdeltaphi", "lep"  , 8+get_offset(), 1,  0.,  3.2));
  status += compare_hist(PlottingCard_t("beta0"         , "event", 8+get_offset(), 1,  0.,   3.));
  status += compare_hist(PlottingCard_t("beta1"         , "event", 8+get_offset(), 1,  0.,   3.));
  status += compare_hist(PlottingCard_t("detectormet"   , "event", 8+get_offset(), 1,  0.,  60.));
  status += compare_hist(PlottingCard_t("nupt"          , "event", 8+get_offset(), 1,  0.,  40.));
  status += compare_hist(PlottingCard_t("runera"        , "event", 8+get_offset(), 1,  1.,  -1.));

  //60 < M < 85 selection
  status += compare_hist(PlottingCard_t("onept"         , "lep"  , 25+get_offset(), 2, 20.,  70.));
  status += compare_hist(PlottingCard_t("twopt"         , "lep"  , 25+get_offset(), 2, 20.,  70.));
  status += compare_hist(PlottingCard_t("ptratio"       , "lep"  , 25+get_offset(), 2,  0.,   2.));
  status += compare_hist(PlottingCard_t("oneeta"        , "lep"  , 25+get_offset(), 2,  1.,  -1.));
  status += compare_hist(PlottingCard_t("twoeta"        , "lep"  , 25+get_offset(), 2,  1.,  -1));
  status += compare_hist(PlottingCard_t("lepm"          , "event", 25+get_offset(), 5, 40., 100.));
  status += compare_hist(PlottingCard_t("lepmestimate"  , "event", 25+get_offset(), 5, 40., 200.));
  status += compare_hist(PlottingCard_t("leppt"         , "event", 25+get_offset(), 2,  0.,  70.));
  status += compare_hist(PlottingCard_t("lepeta"        , "event", 25+get_offset(), 1,  1.,  -1.));
  status += compare_hist(PlottingCard_t("lepdeltaphi"   , "event", 25+get_offset(), 1,  1.,  -1.));
  status += compare_hist(PlottingCard_t("met"           , "event", 25+get_offset(), 2,  0.,  70.));
  status += compare_hist(PlottingCard_t("mtlep"         , "event", 25+get_offset(), 2,  0.,  70.));
  status += compare_hist(PlottingCard_t("mtone"         , "event", 25+get_offset(), 2,  0.,  70.));
  status += compare_hist(PlottingCard_t("mttwo"         , "event", 25+get_offset(), 2,  0.,  70.));
  status += compare_hist(PlottingCard_t("mtoneoverm"    , "event", 25+get_offset(), 1,  0.,  1.5));
  status += compare_hist(PlottingCard_t("mttwooverm"    , "event", 25+get_offset(), 1,  0.,  1.5));
  status += compare_hist(PlottingCard_t("onemetdeltaphi", "lep"  , 25+get_offset(), 1,  0.,  3.2));
  status += compare_hist(PlottingCard_t("twometdeltaphi", "lep"  , 25+get_offset(), 1,  0.,  3.2));
  status += compare_hist(PlottingCard_t("beta0"         , "event", 25+get_offset(), 1,  0.,   3.));
  status += compare_hist(PlottingCard_t("beta1"         , "event", 25+get_offset(), 1,  0.,   3.));
  status += compare_hist(PlottingCard_t("detectormet"   , "event", 25+get_offset(), 1,  0.,  60.));
  status += compare_hist(PlottingCard_t("nupt"          , "event", 25+get_offset(), 1,  0.,  40.));
  status += compare_hist(PlottingCard_t("runera"        , "event", 25+get_offset(), 1,  1.,  -1.));

  //print BDT distributions
  TString mva = "mva";
  if(selection == "mutau"  ) mva = "mva1";
  if(selection == "etau"   ) mva = "mva3";
  if(selection == "emu"    ) mva = "mva5";
  if(selection == "mutau_e") mva = "mva7";
  if(selection == "etau_mu") mva = "mva9";
  status += compare_hist(PlottingCard_t(mva, "event", 8 +get_offset(), 1,  1.,  -1.));
  status += compare_hist(PlottingCard_t(mva, "event", 25+get_offset(), 1,  1.,  -1.));
  status += compare_hist(PlottingCard_t(mva, "event", 27+get_offset(), 1,  1.,  -1.));

  //gen-level Z info
  status += compare_hist(PlottingCard_t("zeta"      , "event", 8+get_offset(), 1,  1.,  -1.));
  status += compare_hist(PlottingCard_t("zmass5"    , "event", 8+get_offset(), 1, 70., 110.));
  status += compare_hist(PlottingCard_t("zleponept" , "event", 8+get_offset(), 1,  1.,  -1.));
  status += compare_hist(PlottingCard_t("zleptwopt" , "event", 8+get_offset(), 1,  1.,  -1.));
  status += compare_hist(PlottingCard_t("zleponeeta", "event", 8+get_offset(), 1,  1.,  -1.));
  status += compare_hist(PlottingCard_t("zleptwoeta", "event", 8+get_offset(), 1,  1.,  -1.));

  return 0;
}
