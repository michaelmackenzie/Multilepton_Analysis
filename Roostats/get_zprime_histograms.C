//Script to retrieve the background and signal di-lepton mass histograms
#include "histograms/datacards.C"
#include "bemu_defaults.C"

CLFV::DataPlotter* dataplotter_ = nullptr;

//initialize the datasets using a DataPlotter
Int_t initialize_plotter(TString selection, TString base) {
  if(dataplotter_) delete dataplotter_;
  dataplotter_ = new DataPlotter();

  dataplotter_->include_misid_ = 0;
  dataplotter_->include_qcd_   = 1;
  dataplotter_->verbose_       = max(0, verbose_ - 1);
  dataplotter_->debug_         = max(0, verbose_ - 1);
  dataplotter_->qcd_scale_     = 1.;
  dataplotter_->embed_scale_   = embedScale_;
  dataplotter_->selection_     = selection;
  dataplotter_->folder_        = "";


  dataplotter_->signal_scale_ = 100.;

  //ensure the years are sorted
  std::sort(years_.begin(),years_.end());
  dataplotter_->years_ = years_;

  hist_tag_  = "clfv";
  hist_dir_  = base;

  std::vector<dcard> cards;
  get_datacards(cards, selection, 2); //get datacards with signal, but without scaling for visibility at x-sec level

  CrossSections xs(useUL_, ZMode_); //cross section handler

  //Calculate luminosity from year(s)
  double lum = 0.;
  for(int year : years_) {
    const double currLum = xs.GetLuminosity(year); //pb^-1
    dataplotter_->lums_[year] = currLum; //store the luminosity for the year
    lum += currLum; //add to the total luminosity
  }
  dataplotter_->set_luminosity(lum);

  int status(0);
  for(auto card : cards)
    status += dataplotter_->add_dataset(card);

  status += dataplotter_->init_files();
  return status;
}


//Get the signal, background, and data histograms in the signal selection
int get_zprime_single_histogram(int set = 11, TString selection = "zemu",
                                vector<int> years = {2016, 2017, 2018},
                                TString base = "nanoaods_zprime") {

  int status(0);
  TString hist = "lepm";

  //initialize the dataplotter
  TString selec = selection; selec.ReplaceAll("z", ""); selec.ReplaceAll("h", "");
  years_ = years;
  status = initialize_plotter(selec, base);
  if(status) {
    cout << "DataPlotter initialization returned " << status << ", exiting!\n";
    return status;
  }
  int set_offset = HistMaker::kEMu;

  dataplotter_->rebinH_ = 1;

  //get background distribution
  THStack* hstack = dataplotter_->get_stack(hist, "event", set+set_offset);
  if(!hstack) return 1;

  //get data
  TH1* hdata = dataplotter_->get_data(hist, "event", set+set_offset);
  if(!hdata) return 3;
  TH1* hdata_clone = (TH1*) hdata->Clone("hdata_clone");
  if(blind_data_) {
    int bin_lo, bin_hi;
    bin_lo = hdata_clone->FindBin(110.);
    bin_hi = hdata_clone->FindBin(500.);
    for(int bin = bin_lo; bin < bin_hi; ++bin) hdata_clone->SetBinContent(bin, 0.);
  }
  hdata_clone->Draw("same E");

  TCanvas* c = new TCanvas();
  hdata_clone->Draw("E");
  hstack->Draw("hist noclear");
  hdata_clone->Draw("same E");
  double max_val = std::max(hdata->GetMaximum(), hstack->GetMaximum());

  double xmin(80.), xmax(700.);
  hdata_clone->GetXaxis()->SetRangeUser(xmin, xmax);
  hdata_clone->GetYaxis()->SetRangeUser(0.1, 1.1*max_val);
  TString year_string;
  for(unsigned i = 0; i < years.size(); ++i) {
    if(i > 0) year_string += "_";
    year_string += years[i];
  }

  gSystem->Exec(Form("[ ! -d plots/zprime/%s ] && mkdir -p plots/zprime/%s", year_string.Data(), year_string.Data()));
  TString canvas_name = Form("plots/zprime/%s/hist_%s_%s_%i", year_string.Data(), hist.Data(), selection.Data(), set);
  c->Print(canvas_name + ".png");

  gSystem->Exec("[ ! -d histograms ] && mkdir histograms");
  TFile* fout = new TFile(Form("histograms/zprime_%s_%i_%s.hist", hist.Data(), set, year_string.Data()), "RECREATE");
  //Write the entire stack in case it's needed
  hstack->SetName("hstack");
  hstack->Write();
  //get a histogram of the background MC
  TH1* hlast = (TH1*) hstack->GetStack()->Last();
  hlast->SetName("hbackground");
  hlast->Write();
  //get just the Drell-Yan contribution of the background to correct N(Data) --> N(Z events)
  TH1* hDY = (TH1*) hlast->Clone("hDY");
  for(auto o : *(hstack->GetHists())) {
    TH1* h = (TH1*) o;
    if(TString(h->GetName()).Contains("DY")) continue;
    hDY->Add(h, -1.); //subtract off non-DY MC
  }
  hDY->Write();
  hdata->SetName("hdata");
  hdata->Write();

  fout->Close();
  return status;
}

int get_zprime_histograms(vector<int> sets = {11,12}, TString selection = "zemu",
                          vector<int> years = {2016, 2017, 2018},
                          TString base = "nanoaods_zprime") {
  useEmbed_ = 0; //bemu_embed_mode_;
  zprime_ = 1;
  int status(0);
  for(int set : sets) {
    cout << "Getting signal region histograms for set " << set << "...\n";
    status += get_zprime_single_histogram(set, selection, years, base);
  }
  return status;
}
