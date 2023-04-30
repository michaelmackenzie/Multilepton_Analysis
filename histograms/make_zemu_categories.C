//Create Z->e+mu categories
#include "dataplotter_clfv.C"

int make_zemu_categories(TString path = "nanoaods_mva_nosys", unsigned max_cat = 5) {
  years_={2016,2017,2018}; useEmbed_ = 0; hist_tag_ = "clfv"; splitDY_ = 1; splitWJ_ = 1; useLepTauSet_ = 1; includeHiggs_ = 0;
  int status = nanoaod_init("emu", path.Data());
  if(status) return status;

  //Retrieve the signal and background histograms
  dataplotter_->signal_scale_ = 0.01;
  const char* hist = "lepmvsmva0";
  const int set = 208;
  dataplotter_->rebinH_ = 0;
  vector<TH1*> sig_list = dataplotter_->get_signal(hist, "event", set);
  if(sig_list.size() == 0) {
    cout << "Signal histogram list is empty!\n";
    return 2;
  }
  TH2* hsig2D = (TH2*) sig_list[0];

  THStack* bkg_stack = dataplotter_->get_stack(hist, "event", set);
  if(!bkg_stack || bkg_stack->GetNhists() == 0) {
    cout << "Background stack is empty!\n";
    return 3;
  }
  TH2* hbkg2D = (TH2*) bkg_stack->GetStack()->Last()->Clone("bkg2D");

  const float min_mass(86.), max_mass(96.);
  int bin_min(hsig2D->GetYaxis()->FindBin(min_mass + 1.e-3)), bin_max(hsig2D->GetYaxis()->FindBin(max_mass - 1.e-3));
  const float ymin(hsig2D->GetYaxis()->GetBinLowEdge(bin_min)), ymax(hsig2D->GetYaxis()->GetBinUpEdge(bin_max));
  cout << "Using mass range " << ymin << " to " << ymax << endl;

  TH1* hsig = hsig2D->ProjectionX("_px", bin_min, bin_max);
  TH1* hbkg = hbkg2D->ProjectionX("_px", bin_min, bin_max);

  //Create categories using cut-and-count limit estimates
  const double min_bkg = 20.; //control minimum background events allowed in a category
  CLFV::CreateCategories cats(min_bkg, 1);

  for(unsigned ncat = 1; ncat <= max_cat; ++ncat) {
    cout << "Creating " << ncat << " categories:\n";
    std::vector<Double_t> cuts, sigs;
    cats.FindCategories(hsig, hbkg, ncat, cuts, sigs);
  }
  return 0;
}
