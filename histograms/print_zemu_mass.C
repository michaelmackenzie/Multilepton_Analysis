//Print mass distribution plots for the Z->e+mu search
#include "dataplotter_clfv.C"

int print_zemu_mass(TString out_dir = "emu_mass", TString hist_dir = "nanoaods_emu",
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
  vector<int> sets = {8,10,11,12,13};
  const int offset = get_offset();

  //histogram processing info
  dataplotter_->rebinH_ = 4;
  dataplotter_->xMin_ = 70.;
  dataplotter_->xMax_ = 110.;
  dataplotter_->blindxmin_ = {86.};
  dataplotter_->blindxmax_ = {96.};

  //process each set
  for(int set : sets) {
    //get the background stack of histograms
    auto stack = dataplotter_->get_stack("lepm", "event", set+offset);
    if(!stack || stack->GetNhists() == 0) {
      cout << "No stack found for set " << set+offset << endl;
      ++status;
      continue;
    }
    //get the signal
    auto signals = dataplotter_->get_signal("lepm", "event", set+offset);
    if(signals.size() == 0) {
      cout << "No signal found for set " << set+offset << endl;
      ++status;
      continue;
    }
    auto signal = signals[0];
    //get the data
    auto data = dataplotter_->get_data("lepm", "event", set+offset);

    //blind the data
    unsigned nbins = data->GetNbinsX();
    for(unsigned bin = 1; bin <= nbins; ++bin) {
      double binlow = data->GetBinLowEdge(bin);
      double binhigh = binlow + data->GetBinWidth(bin);
      if(dataplotter_->isBlind(binlow, binhigh))
        data->SetBinContent(bin, 0.);
    }


    //make a plot of the signal and backgrounds
    TCanvas* c = new TCanvas();
    TLegend* leg = new TLegend(0.13, 0.72, 0.87, 0.87);
    leg->SetNColumns(3);
    leg->SetTextSize(0.05);
    leg->SetFillStyle(0);
    leg->SetFillColor(0);
    leg->SetLineColor(0);
    leg->SetLineStyle(0);

    double max_val = CLFV::Utilities::H1Max(signal, dataplotter_->xMin_, dataplotter_->xMax_);
    signal->SetFillColor(0);
    signal->Draw("hist");
    data->Draw("E1 same");
    leg->AddEntry(data, "Data", "PL");
    leg->AddEntry(signal, signal->GetTitle(), "L");

    TH1* bkg_tot = (TH1*) stack->GetStack()->Last()->Clone(Form("bkg_total_%i", set+offset));
    bkg_tot->SetLineColor(kBlack);
    bkg_tot->SetMarkerColor(kBlack);
    bkg_tot->SetFillColor(0);
    bkg_tot->Draw("hist same");
    leg->AddEntry(bkg_tot, "Background", "L");
    max_val = max(max_val, CLFV::Utilities::H1Max(bkg_tot, dataplotter_->xMin_, dataplotter_->xMax_));

    for(auto o : *(stack->GetHists())) {
      TH1* h = (TH1*) o;
      h->SetFillColor(0);
      if(h->GetLineColor() == kYellow - 7) h->SetLineColor(kOrange-2); //make Top more visible
      if(h->GetLineColor() == dataplotter_->qcd_color_) h->SetLineColor(dataplotter_->misid_color_); //make QCD more different
      h->Draw("hist same");
      leg->AddEntry(h, h->GetTitle(), "L");
    }
    signal->GetXaxis()->SetRangeUser(70.,110.);
    signal->GetYaxis()->SetRangeUser(0.,1.25*max_val);
    signal->SetTitle("Background and signal mass distributions;M_{ll} (GeV/c^{2})");
    leg->Draw();
    TString fig_name = dataplotter_->GetFigureName("lepm", "hists", set+offset, "signal");
    c->SaveAs(fig_name.Data());
    delete c;
  }

  return status;
}
