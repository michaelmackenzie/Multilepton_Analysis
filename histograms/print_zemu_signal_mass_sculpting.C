//Print mass distribution plots for the Z->e+mu signal in regions of BDT score
#include "dataplotter_clfv.C"

int print_zemu_signal_mass_sculpting(TString out_dir = "emu_mass", TString hist_dir = "nanoaods_mva_emu_dev",
                                     vector<int> years = {2016,2017,2018}) {
  //setup the datacards
  years_     = years;
  hist_dir_  = hist_dir;
  hist_tag_  = "clfv";
  useEmbed_  = 1; //use DY MC by default
  drawStats_ = 0; //don't include integrals

  //initialize the plotter
  int status = nanoaod_init("emu", hist_dir_, out_dir);
  if(status) return status;

  //setup the histogram sets of interest
  vector<int> sets = {10,11,12,13};
  const int offset = get_offset();

  //histogram processing info
  dataplotter_->rebinH_ = 2;
  dataplotter_->xMin_ = 80.;
  dataplotter_->xMax_ = 100.;
  dataplotter_->blindxmin_ = {86.};
  dataplotter_->blindxmax_ = {96.};

  //process each set
  vector<TH1*> signal_hists;
  TH1* haxis = nullptr;
  double max_val = 0.;
  TCanvas* c = new TCanvas();
  TLegend* leg = new TLegend(0.6, 0.7, 0.87, 0.87);
  leg->SetTextSize(0.05);
  leg->SetFillStyle(0);
  leg->SetFillColor(0);
  leg->SetLineColor(0);
  leg->SetLineStyle(0);
  for(int set : sets) {
    //get the signal
    auto signals = dataplotter_->get_signal("lepm", "event", set+offset);
    if(signals.size() == 0) {
      cout << "No signal found for set " << set+offset << endl;
      ++status;
      continue;
    }
    auto signal = signals[0];
    if(signal->Integral() <= 0.) continue;
    signal->Scale(1./signal->Integral());
    if(set ==  8) {signal->SetLineColor(kBlue); signal->SetTitle("Inclusive"      );}
    if(set == 10) {signal->SetLineColor(kRed ); signal->SetTitle("0 < BDT < 0.3"  );}
    if(set == 11) {signal->SetLineColor(kGreen-7); signal->SetTitle("0.3 < BDT < 0.7");}
    if(set == 12) {signal->SetLineColor(kAtlantic); signal->SetTitle("0.7 < BDT < 0.9");}
    if(set == 13) {signal->SetLineColor(kMagenta); signal->SetTitle("0.9 < BDT < 1"  );}
    signal->SetFillColor(0);

    if(!haxis) {haxis = signal; signal->Draw("hist");}
    else signal->Draw("hist same");

    max_val = max(max_val, CLFV::Utilities::H1Max(signal, dataplotter_->xMin_, dataplotter_->xMax_));
    signal_hists.push_back(signal);
    leg->AddEntry(signal, signal->GetTitle(), "L");
  }

  haxis->GetXaxis()->SetRangeUser(dataplotter_->xMin_,dataplotter_->xMax_);
  haxis->SetTitle("Signal mass distributions vs. BDT score;M_{ll} (GeV/c^{2})");
  leg->Draw();

  //Print a linear and log figure
  TString fig_name;

  haxis->GetYaxis()->SetRangeUser(max_val/1.e2,1.25*max_val);
  dataplotter_->logY_ = 0;
  fig_name = dataplotter_->GetFigureName("lepm", "bdt_regions", 8+offset, "signal");
  c->SaveAs(fig_name.Data());

  haxis->GetYaxis()->SetRangeUser(max_val/1.e2,3.*max_val);
  c->SetLogy();
  dataplotter_->logY_ = 1;
  fig_name = dataplotter_->GetFigureName("lepm", "bdt_regions", 8+offset, "signal");
  c->SaveAs(fig_name.Data());

  return status;
}
