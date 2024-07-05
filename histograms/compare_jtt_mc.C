//Compare data-driven and MC j-->tau estimates
#include "dataplotter_clfv.C"
DataPlotter* data_plotter_;
DataPlotter* mc_plotter_;

int compare_hist(PlottingCard_t card, bool add_data = true) {
  ////////////////////////////////////////
  // Retrieve the histograms

  //get the purely data-driven histogram
  auto s_dd = data_plotter_->get_stack(card.hist_, card.type_, card.set_);
  if(!s_dd || s_dd->GetNhists() == 0) {
    cout << "No data-driven histogram found for " << card.hist_.Data() << "/" << card.type_.Data() << "/" << card.set_ << endl;
    return 1;
  }
  auto h_dd = (TH1*) s_dd->GetStack()->Last()->Clone(Form("h_%s_%s_%i_dd", card.hist_.Data(), card.type_.Data(), card.set_));
  delete s_dd;

  //get the MC histogram
  auto s_mc = mc_plotter_->get_stack(card.hist_, card.type_, card.set_);
  if(!s_mc || s_mc->GetNhists() == 0) {
    cout << "No MC histogram found for " << card.hist_.Data() << "/" << card.type_.Data() << "/" << card.set_ << endl;
    delete h_dd;
    return 1;
  }
  auto h_mc = (TH1*) s_mc->GetStack()->Last()->Clone(Form("h_%s_%s_%i_mc", card.hist_.Data(), card.type_.Data(), card.set_));
  delete s_mc;

  auto data = data_plotter_->get_data(card.hist_, card.type_, card.set_);
  if(!data) {
    delete h_dd;
    delete h_mc;
    cout << "No data histogram found for " << card.hist_.Data() << "/" << card.type_.Data() << "/" << card.set_ << endl;
    return 1;
  }
  data = (TH1*) data->Clone(Form("h_%s_%s_%i_data", card.hist_.Data(), card.type_.Data(), card.set_));

  if(h_dd->Integral() <= 0. || h_mc->Integral() <= 0. || data->Integral() <= 0.) {
    cout << "A zero integral for " << card.hist_.Data() << "/" << card.type_.Data() << "/" << card.set_ << endl;
    delete h_dd;
    delete h_mc;
    delete data;
    return 1;
  }
  //Rebin if requested
  if(card.rebin_ > 1) {
    h_dd->Rebin(card.rebin_);
    h_mc->Rebin(card.rebin_);
    data->Rebin(card.rebin_);
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
  h_mc->SetLineColor(kRed);
  h_mc->SetMarkerColor(kRed);
  h_mc->SetLineWidth(3);
  h_mc->SetFillColor(0);
  h_mc->Draw("hist E1");
  h_mc->SetTitle("Template using MC vs. data j-->#tau");

  h_dd->SetLineColor(kBlue);
  h_dd->SetMarkerColor(kBlue);
  h_dd->SetLineWidth(3);
  h_dd->SetFillColor(0);
  h_dd->Draw("hist E1 same");

  if(add_data) data->Draw("E1 same");

  double xmin = card.xmin_;
  double xmax = card.xmax_;
  if(xmin >= xmax) {xmin = h_dd->GetXaxis()->GetXmin(); xmax = h_dd->GetXaxis()->GetXmax();}

  h_mc->GetXaxis()->SetRangeUser(xmin, xmax);
  if(card.ymin_ < card.ymax_) h_mc->GetYaxis()->SetRangeUser(card.ymin_, card.ymax_);
  else {
    h_mc->GetYaxis()->SetRangeUser(0.1, 1.30*max(h_mc->GetMaximum(), h_dd->GetMaximum()));
  }

  TLegend leg(0.5, 0.75, 0.9, 0.9);
  leg.AddEntry(h_dd, Form("Data-driven: %.0f", h_dd->Integral(0, h_dd->GetNbinsX()+1)));
  leg.AddEntry(h_mc, Form("MC        : %.0f" , h_mc->Integral(0, h_mc->GetNbinsX()+1)));
  leg.AddEntry(data, Form("Data      : %.0f" , data->Integral(0, data->GetNbinsX()+1)));
  leg.Draw();

  ////////////////////////////////////////
  // Plot the ratio

  pad2.cd();
  TH1* hratio = (TH1*) h_mc->Clone(Form("%s_ratio", h_mc->GetName()));
  hratio->Divide(h_dd);
  hratio->SetLineWidth(2);
  hratio->SetMarkerSize(0.8);
  hratio->SetMarkerStyle(20);
  hratio->SetMarkerColor(kRed);
  hratio->Draw("E1");
  hratio->SetTitle("");
  hratio->SetYTitle("MC / Data-driven");
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

  TH1* data_ratio = (TH1*) data->Clone(Form("%s_ratio", data->GetName()));
  data_ratio->Divide(h_dd);
  if(add_data) data_ratio->Draw("E1 same");


  TLine line(xmin, 1., xmax, 1.);
  line.SetLineColor(kBlack);
  line.SetLineWidth(2);
  line.SetLineStyle(kDashed);
  line.Draw("same");

  TString fig_name = data_plotter_->GetFigureName(card.type_, card.hist_, card.set_, "signal", true);
  c.SaveAs(fig_name.Data());

  //clean up
  delete hratio;
  delete h_mc;
  delete h_dd;
  delete data;

  return 0;
}

int compare_jtt_mc(TString out_dir = "compare_jtt_mc",
                   TString data_dir = "nanoaods_embed_met",
                   TString mc_dir = "nanoaods_embed_met_mc_jtt",
                   TString selection = "mutau", vector<int> years = {2016,2017,2018}) {

  //setup the datacards
  years_         = years;
  hist_tag_      = "clfv";
  useEmbed_      = 1;

  //Initialize the Embedding dataplotter
  cout << "Initializing data-driven dataplotter\n";
  if(nanoaod_init(selection, data_dir, out_dir)) return 1;
  data_plotter_ = dataplotter_;
  dataplotter_ = nullptr;

  //Initialize the MC DY dataplotter
  cout << "Initializing MC j-->tau dataplotter\n";
  if(nanoaod_init(selection, mc_dir, out_dir)) return 2;
  mc_plotter_ = dataplotter_;
  dataplotter_ = nullptr;

  int status(0);

  cout << "Printing figures\n";

  vector<int> sets = {8, 25, 26, 27, 28, 1008, 1025, 1026, 1027, 1028};

  //Determine the BDT name
  TString mva = "mva1"; //default to zmutau_h BDT
  if(selection == "mutau"  ) mva = "mva1";
  if(selection == "etau"   ) mva = "mva3";
  if(selection == "emu"    ) mva = "mva5";
  if(selection == "mutau_e") mva = "mva7";
  if(selection == "etau_mu") mva = "mva9";

  for(int set : sets) {
    status += compare_hist(PlottingCard_t("onept"         , "lep"  , set+get_offset(), 2, 20.,  70.));
    status += compare_hist(PlottingCard_t("twopt"         , "lep"  , set+get_offset(), 2, 20.,  70.));
    status += compare_hist(PlottingCard_t("ptratio"       , "lep"  , set+get_offset(), 2,  0.,   2.));
    status += compare_hist(PlottingCard_t("oneeta"        , "lep"  , set+get_offset(), 2,  1.,  -1.));
    status += compare_hist(PlottingCard_t("twoeta"        , "lep"  , set+get_offset(), 2,  1.,  -1));
    status += compare_hist(PlottingCard_t("lepm"          , "event", set+get_offset(),10, 40., 170.));
    status += compare_hist(PlottingCard_t("lepmestimate"  , "event", set+get_offset(), 5, 40., 200.));
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
    status += compare_hist(PlottingCard_t("onemetdeltaphi", "lep"  , set+get_offset(), 2,  0.,  3.2));
    status += compare_hist(PlottingCard_t("twometdeltaphi", "lep"  , set+get_offset(), 2,  0.,  3.2));
    status += compare_hist(PlottingCard_t("metdeltaphi"   , "event", set+get_offset(), 2,  0.,  3.2));
    status += compare_hist(PlottingCard_t("beta0"         , "event", set+get_offset(), 5, 0.5,  2.5));
    status += compare_hist(PlottingCard_t("beta1"         , "event", set+get_offset(), 5, 0.5,  2.5));
    status += compare_hist(PlottingCard_t(mva             , "event", set+get_offset(), 1,  1.,  -1.), false); //don't add data for the BDT plots
  }

  return status;
}
