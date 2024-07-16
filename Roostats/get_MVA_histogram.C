//Script to retrieve the background and signal MVA histograms
#include "histograms/datacards.C"

#include "mva_defaults.C"

int overall_rebin_ = 0;
TH1* hbkg_;
THStack* hstack_;
vector<TH1*> hsigs_;
int  test_sys_        =    -1; //set to systematic number if debugging/inspecting it
int  blind_data_      =     1; //1: don't plot high score data; 2: set data bins > MVA score level to 0
bool skip_shape_sys_  = false; //skip shape systematic retrieval
int  use_dev_mva_     =     0; //1: use the extra MVA hist for development, mvaX_1; 2: use the CDF transformed hist, mvaX_2
bool do_same_flavor_  = false; //retrieve Z->ll control region data
bool separate_years_  = true ; //retrieve and store histograms by year
bool use_lep_tau_set_ = true ; //use kETauMu/kMuTauE offsets instead of kEMu
bool print_sys_plots_ = true ; //print systematic figures
bool save_            = true ; //save output histogram files
bool use_sys_name_    = true ; //save the systematic figure using the expected systematic name

//copy histogram files locally, process them, then delete them
bool copy_local_ = false;

//plotting parameters
double xmin_ = -1.;
double xmax_ =  1.;
bool   logy_ = false;

CLFV::DataPlotter* dataplotter_ = nullptr;

//---------------------------------------------------------------------------------------------------------------------
//perform some sanity checks on input histograms
Bool_t check_histogram(TH1* h) {
  for(int ibin = 1; ibin <= h->GetNbinsX(); ++ibin) {
    const double binc = h->GetBinContent(ibin);
    const double bine = h->GetBinError  (ibin);
    if(binc < 0.) {
      cout << __func__ << ": " << h->GetName() << " --> Bin " << ibin << " < 0 content " << binc << endl;
      return false;
    }
    if(binc > 0. && bine <= 0.) {
      cout << __func__ << ": " << h->GetName() << " --> Bin " << ibin << " content but no error (" << binc
           << ", " << bine << ")" << endl;
      return false;
    }
    if(binc == 0. && bine > 0.) { //add a small factor to make this non-zero
      h->SetBinContent(ibin, 1.e-10);
    }
  }
  return true;
}

//---------------------------------------------------------------------------------------------------------------------
//cleanup the temporary histogram holding area
Int_t remove_local(TString selection) {
  selection.ReplaceAll("z", ""); //remove signal label from selection
  selection.ReplaceAll("h", "");

  //get the datacards for the selection
  std::vector<dcard> cards;
  get_datacards(cards, selection, 2); //get datacards with signal, but without scaling for visibility at x-sec level

  //temporary directory for histogram storage
  const char* local_storage = "local_histograms";

  //copy over each file
  for(dcard& card : cards) {
    const char* outname = Form("%s_%s_%i_%s.hist", hist_tag_.Data(), selection.Data(), card.year_, card.name_.Data());
    printf("Removing %s\n", outname);
    gSystem->Exec(Form("rm %s/%s", local_storage, outname));
  }
  return 0;
}

//---------------------------------------------------------------------------------------------------------------------
//copy datasets locally for faster access
Int_t copy_local(TString selection, TString base) {

  //get the datacards for the selection
  std::vector<dcard> cards;
  get_datacards(cards, selection, 2); //get datacards with signal, but without scaling for visibility at x-sec level

  //make a temporary directory for histogram storage
  const char* local_storage = "local_histograms";
  gSystem->Exec(Form("[ ! -d %s ] && mkdir %s", local_storage, local_storage));

  //copy over each file
  for(dcard& card : cards) {
    const char* outname = Form("%s_%s_%i_%s.hist", hist_tag_.Data(), selection.Data(), card.year_, card.name_.Data());
    printf("Copying %-45s: %s\n", outname, card.filename_.Data());
    // gSystem->Exec(Form("while [ 1 ]; do; timeout 20 xrdcp -f %s %s/%s; if [ $? -eq 0 ]; then; break; else; echo Re-attempting to copy!; fi; done;",
    //                    card.filename_.Data(), local_storage, outname));
    gSystem->Exec(Form("while [ 1 ]; do timeout 20 xrdcp -f %s %s/%s; if [ $? -eq 0 ]; then break; else echo Re-attempting to copy!; fi; done",
                       card.filename_.Data(), local_storage, outname));
  }
  hist_path_ = "./";
  hist_dir_  = local_storage;
  return 0;
}

//---------------------------------------------------------------------------------------------------------------------
//initialize the datasets using a DataPlotter
Int_t initialize_plotter(TString selection, TString base) {
  if(dataplotter_) delete dataplotter_;
  dataplotter_ = new DataPlotter();

  dataplotter_->include_misid_ = selection == "mutau" || selection == "etau";
  dataplotter_->include_qcd_   = !dataplotter_->include_misid_ && selection != "mumu" && selection != "ee";
  dataplotter_->verbose_       = max(0, verbose_ - 1);
  dataplotter_->debug_         = max(0, verbose_ - 1);
  dataplotter_->qcd_scale_     = 1.;
  dataplotter_->embed_scale_   = embedScale_;
  dataplotter_->selection_     = selection;
  dataplotter_->folder_        = "";
  dataplotter_->include_empty_ = 1; //ensure even an empty histogram is included in the stacks
  dataplotter_->clip_negative_ = 1; //ensure no negative process rate bins
  dataplotter_->include_signal_subtraction_ = signal_in_fakes_cr_; //subtract the signal at the example scale from fake lepton control region

  if(signal_in_fakes_cr_) { //use more normal, consistent branching fractions for these tests
    if(selection == "emu")
      {dataplotter_->signal_scale_ = 10.; dataplotter_->signal_scales_["H->e#mu"] = 40.;}
    else if(selection.Contains("_"))
      {dataplotter_->signal_scale_ = 10.; dataplotter_->signal_scales_["H->#mu#tau"] = 10.; dataplotter_->signal_scales_["H->e#tau"] = 10.;}
    else if(selection.Contains("tau"))
      {dataplotter_->signal_scale_ = 10.; dataplotter_->signal_scales_["H->#mu#tau"] = 10.; dataplotter_->signal_scales_["H->e#tau"] = 10.;}
    else if(selection == "ee" || selection == "mumu")
      dataplotter_->signal_scale_ = 1000.;
  } else { //use more visible branching fractions
    if(selection == "emu")
      {dataplotter_->signal_scale_ = 100.; dataplotter_->signal_scales_["H->e#mu"] = 400.;}
    else if(selection.Contains("_"))
      {dataplotter_->signal_scale_ = 250.; dataplotter_->signal_scales_["H->#mu#tau"] = 300.; dataplotter_->signal_scales_["H->e#tau"] = 300.;}
    else if(selection.Contains("tau"))
      {dataplotter_->signal_scale_ = 150.; dataplotter_->signal_scales_["H->#mu#tau"] = 250.; dataplotter_->signal_scales_["H->e#tau"] = 250.;}
    else if(selection == "ee" || selection == "mumu")
      dataplotter_->signal_scale_ = 2.e4;
  }
  //ensure the years are sorted
  std::sort(years_.begin(),years_.end());
  dataplotter_->years_ = years_;

  hist_tag_  = "clfv";
  hist_dir_  = base;

  if(copy_local_) {
    int status = copy_local(selection, base);
    if(status) {
      cout << "Local histogram copying failed with status " << status << endl;
      return status;
    }
  }

  //include Higgs backgrounds
  higgsBkg_  = higgs_bkg_;
  combineHB_ = 1; //combine H->tautau and H->WW

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

//-----------------------------------------------------------------------------------------------------------
// Plot the statistical uncertainty in each histogram
int plot_stat_sys(THStack* stack, vector<TH1*> sigs, TH1* hdata, TString name) {
  if(!stack || stack->GetNhists() == 0 || sigs.size() == 0 || !hdata) {
    cout << __func__ << ": Input histograms not all defined!\n";
    return 1;
  }
  TCanvas* c = new TCanvas("c_stat", "c_stat", 1200, 900);
  TPad* pad1 = new TPad("pad1_stat", "pad1_stat", 0., 0.3, 1., 1. );
  TPad* pad2 = new TPad("pad2_stat", "pad2_stat", 0., 0. , 1., 0.3);
  pad1->Draw(); pad2->Draw();
  pad1->SetBottomMargin(0.06);
  pad2->SetTopMargin(0.03);
  pad1->cd();

  TH1* hbkg = (TH1*) stack->GetStack()->Last()->Clone("hbkg_stat");
  hbkg->SetLineColor(kRed);
  hbkg->SetLineStyle(kDashed);
  hbkg->SetLineWidth(2);
  hbkg->SetMarkerSize(0);

  TLegend* leg = new TLegend(0.1, 0.7, 0.9, 0.9);
  leg->SetNColumns(2);
  auto data = (TH1*) hdata->Clone("data_stat");
  data->SetTitle("Statistical uncertainties");
  data->Draw("E1");
  leg->AddEntry(data, "Data");
  for(auto sig : sigs) {sig->Draw("E1 same"); leg->AddEntry(sig);}
  hbkg->Draw("E1 same"); leg->AddEntry(hbkg, "Total Bkg", "L");
  for(auto obj : *(stack->GetHists())) {
    auto h = (TH1*) obj;
    const int o_color(h->GetLineColor());
    if(o_color == kYellow - 7) { //make the Top histogram more visible
      h->SetLineColor(kOrange);
      h->SetFillColor(kOrange);
      h->SetMarkerColor(kOrange);
    }
    obj->Draw("E1 same");
    leg->AddEntry(obj);
  }
  data->GetYaxis()->SetRangeUser(0.5, (logy_) ? 4.*data->GetMaximum() : 1.3*data->GetMaximum());
  if(logy_) pad1->SetLogy();
  leg->Draw();

  pad2->cd();
  std::vector<TH1*> to_clean;
  TH1* hdata_r = (TH1*) hbkg->Clone("hdata_stat_r"); //predicted stat. uncertainty of data from Asimov template
  hdata_r->Divide(hbkg);
  double max_diff = 0.;
  for(int ibin = 1; ibin <= hbkg->GetNbinsX(); ++ibin) {
    //assign sqrt(N predicted) as predicted data uncertainty
    const double error = (hbkg->GetBinContent(ibin) > 0.) ? 1./sqrt(hbkg->GetBinContent(ibin)) : 0.;
    max_diff = max(error, max_diff);
    if(hbkg->GetBinContent(ibin) > 0.) hdata_r->SetBinError(ibin, error);
    else {
      hbkg->SetBinContent(ibin, 0.);
      hbkg->SetBinError  (ibin, 0.);
    }
  }
  hdata_r->SetLineColor(data->GetLineColor());
  hdata_r->SetMarkerColor(data->GetMarkerColor());
  hdata_r->Draw("E1");
  to_clean.push_back(hbkg);
  to_clean.push_back(hdata_r);
  for(auto sig : sigs) {
    TH1* sig_u = (TH1*) sig->Clone(Form("%s_u", sig->GetName()));
    TH1* sig_d = (TH1*) sig->Clone(Form("%s_d", sig->GetName()));
    for(int ibin = 1; ibin <= sig->GetNbinsX(); ++ibin) {
      const double error = (sig->GetBinContent(ibin) > 0.) ? sig->GetBinError(ibin)/sig->GetBinContent(ibin) : 0.;
      max_diff = max(error, max_diff);
      if(sig->GetBinContent(ibin) > 0.) sig_u->SetBinContent(ibin, 1. + error);
      if(sig->GetBinContent(ibin) > 0.) sig_d->SetBinContent(ibin, 1. - error);
    }
    sig_u->SetFillColor(0);
    sig_d->SetFillColor(0);
    sig_u->Draw("hist same");
    sig_d->Draw("hist same");
    to_clean.push_back(sig_u);
    to_clean.push_back(sig_d);
  }

  //stat uncertainty for each background component
  TList hist_list;
  for(auto obj : *(stack->GetHists())) hist_list.Add(obj);
  hist_list.Add(hbkg); //add the total background template to the list
  for(auto obj : hist_list) {
    TH1* h = (TH1*) obj;
    TH1* h_u = (TH1*) h->Clone(Form("%s_u", h->GetName()));
    TH1* h_d = (TH1*) h->Clone(Form("%s_d", h->GetName()));
    for(int ibin = 1; ibin <= h->GetNbinsX(); ++ibin) {
      const double error = (hbkg->GetBinContent(ibin) > 0.) ? h->GetBinError(ibin)/hbkg->GetBinContent(ibin) : 0.;
      max_diff = max(error, max_diff);
      if(hbkg->GetBinContent(ibin) > 0.) h_u->SetBinContent(ibin, 1. + error);
      if(hbkg->GetBinContent(ibin) > 0.) h_d->SetBinContent(ibin, 1. - error);
    }
    h_u->SetFillColor(0);
    h_d->SetFillColor(0);
    h_u->Draw("hist same");
    h_d->Draw("hist same");
    to_clean.push_back(h_u);
    to_clean.push_back(h_d);
  }

  hdata_r->GetXaxis()->SetRangeUser(xmin_, xmax_);
  hdata_r->GetYaxis()->SetRangeUser(1. - 1.05*max_diff, 1. + 1.05*max_diff);
  hdata_r->GetXaxis()->SetLabelSize(0.09);
  hdata_r->GetYaxis()->SetLabelSize(0.09);
  hdata_r->GetYaxis()->SetTitleSize(0.1);
  hdata_r->GetYaxis()->SetTitleOffset(0.35);
  hdata_r->SetTitle("");
  hdata_r->SetYTitle("Stat. uncertainty");
  c->SaveAs(name.Data());
  for(auto h : to_clean) delete h;
  delete data;
  delete c;

  return 0;
}

// //---------------------------------------------------------------------------------------------------------------------
// // Print a two-sided uncertainty plot
// int print_systematic_canvas(TH1* nominal, TH1* data,
//                             THStack* stack_up, THStack* stack_down,
//                             TH1* nom_sig, TH1* sig_up, TH1* sig_down,
//                             const char* name, const char* fig_name) {
//   //Check the inputs
//   if(!data || !stack_up || !stack_down || !nom_sig || !sig_up || !sig_down) return 1;
//   if(!nominal) nominal = hbkg_;

//   //Make the canvas
//   TCanvas* c = new TCanvas(name, name, 1200, 900);
//   TPad* pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1. );
//   TPad* pad2 = new TPad("pad2", "pad2", 0., 0. , 1., 0.3);
//   pad1->SetBottomMargin(0.06);
//   pad2->SetTopMargin(0.03);
//   pad1->Draw(); pad2->Draw();
//   pad1->cd();

//   //Draw the total stack / data on top using the up templates just to see major contributions
//   data->Draw("E1");
//   stack_up->Draw("hist noclear same");
//   sig_up->Draw("hist same");
//   data->Draw("E1 same");
//   data->GetXaxis()->SetRangeUser(xmin_,xmax_);
//   data->GetYaxis()->SetRangeUser((logy_) ? 0.5 : 0.9,((logy_) ? 2 : 1.1)*max(data->GetMaximum(), ((TH1*) stack_up->GetStack()->Last())->GetMaximum()));
//   data->SetTitle("");
//   if(logy_) pad1->SetLogy();

//   //Draw ratio plots on the bottom
//   pad2->cd();
//   TH1* data_ratio = (TH1*) data->Clone("data_ratio"); data_ratio->Divide(nominal);
//   TH1* bkg_ratio_up   = (TH1*) stack_up  ->GetStack()->Last()->Clone("bkg_ratio_up"  ); bkg_ratio_up  ->Divide(nominal);
//   TH1* bkg_ratio_down = (TH1*) stack_down->GetStack()->Last()->Clone("bkg_ratio_down"); bkg_ratio_down->Divide(nominal);
//   TH1* sig_ratio_up   = (TH1*) sig_up  ->Clone("sig_ratio_up"  ); sig_ratio_up  ->Divide(nom_sig);
//   TH1* sig_ratio_down = (TH1*) sig_down->Clone("sig_ratio_down"); sig_ratio_down->Divide(nom_sig);

//   double min_r = min(    Utilities::H1Min(bkg_ratio_up, xmin_, xmax_), Utilities::H1Min(bkg_ratio_down, xmin_, xmax_));
//   min_r = min(min_r, min(Utilities::H1Min(sig_ratio_up, xmin_, xmax_), Utilities::H1Min(sig_ratio_down, xmin_, xmax_)));
//   double max_r = max(    Utilities::H1Max(bkg_ratio_up, xmin_, xmax_), Utilities::H1Max(bkg_ratio_down, xmin_, xmax_));
//   max_r = max(max_r, max(Utilities::H1Max(sig_ratio_up, xmin_, xmax_), Utilities::H1Max(sig_ratio_down, xmin_, xmax_)));

//   min_r = min(0.95, min_r - 0.05*(1. - min_r));
//   max_r = max(1.05, max_r + 0.05*(max_r - 1.));

//   bkg_ratio_up->SetLineColor(kRed-3);
//   bkg_ratio_up->SetLineWidth(2);
//   bkg_ratio_up->SetFillColor(0);
//   sig_ratio_up->SetLineWidth(2);
//   sig_ratio_up->SetFillColor(0);

//   data_ratio->Draw("E1");
//   sig_ratio_up->Draw("hist same");
//   bkg_ratio_up->Draw("hist same");
//   data_ratio->GetYaxis()->SetRangeUser(min_r, max_r);
//   data_ratio->GetXaxis()->SetRangeUser(xmin_, xmax_);
//   data_ratio->GetXaxis()->SetLabelSize(0.08);
//   data_ratio->GetYaxis()->SetLabelSize(0.08);

//   //Print the figure
//   c->SaveAs(fig_name);

//   delete c;
//   delete bkg_ratio_up;
//   delete bkg_ratio_down;
//   delete sig_ratio_up;
//   delete sig_ratio_down;
//   delete data_ratio;

//   return 0;
// }

//---------------------------------------------------------------------------------------------------------------------
// Print a one-sided uncertainty plot
int print_systematic_canvas(TH1* nominal, TH1* data, THStack* stack, TH1* nom_sig, TH1* sig,
                            const char* name, const char* fig_name) {
  //Check the inputs
  if(!data || !stack || !nom_sig || !sig) return 1;
  if(!nominal) nominal = hbkg_;

  //Make the canvas
  TCanvas* c = new TCanvas(name, name, 1200, 900);
  TPad* pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1. );
  TPad* pad2 = new TPad("pad2", "pad2", 0., 0. , 1., 0.3);
  pad1->SetBottomMargin(0.06);
  pad2->SetTopMargin(0.03);
  pad1->Draw(); pad2->Draw();
  pad1->cd();

  //Draw the total stack / data on top
  data->Draw("E1");
  stack->Draw("hist noclear same");
  sig->Draw("hist same");
  data->Draw("E1 same");
  data->GetXaxis()->SetRangeUser(xmin_,xmax_);
  const double max_val = max(Utilities::H1Max(data, xmin_, xmax_), Utilities::H1Max((TH1*) stack->GetStack()->Last(), xmin_, xmax_));
  data->GetYaxis()->SetRangeUser((logy_) ? 0.5 : 0.9, ((logy_) ? 2 : 1.1)*max_val);
  data->SetTitle("");
  if(logy_) pad1->SetLogy();

  //Draw ratio plots on the bottom
  pad2->cd();
  vector<TH1*> h_to_delete;
  TH1* bkg_ratio = (TH1*) stack->GetStack()->Last()->Clone("bkg_ratio");
  bkg_ratio->Divide(nominal);
  TH1* sig_ratio = (TH1*) sig->Clone("sig_ratio");
  sig_ratio->Divide(nom_sig);
  TH1* data_ratio = (TH1*) data->Clone("data_ratio");
  data_ratio->Divide(nominal);
  h_to_delete.push_back(bkg_ratio);
  h_to_delete.push_back(sig_ratio);
  h_to_delete.push_back(data_ratio);

  const double max_bkg_val(Utilities::H1Max(bkg_ratio, xmin_, xmax_)), min_bkg_val(max(Utilities::H1MinAbove(bkg_ratio, 0.1), Utilities::H1Min(bkg_ratio, xmin_, xmax_)));
  const double max_sig_val(Utilities::H1Max(sig_ratio, xmin_, xmax_)), min_sig_val(Utilities::H1MinAbove(sig_ratio, 0.1)); //Utilities::H1Min(sig_ratio, xmin_, xmax_)));
  double min_r = min((min_bkg_val <= 0.) ? 0.95 : min_bkg_val, (min_sig_val <= 0.) ? 0.95 : min_sig_val); //try to catch cases with val = 0
  double max_r = max(max_bkg_val, max_sig_val);
  if(TString(fig_name).Contains("JER")) {
    cout << fig_name << ": min_sig = " << min_sig_val << ", max_sig = " << max_sig_val << ", min_r = " << min_r << ", max_r " << max_r << endl;
  }
  min_r = max(0., min(0.95, min_r - 0.05*(1. - min_r))); //set the ratio bounds to be at least 5% with a small buffer
  max_r = min(2., max(1.05, max_r + 0.05*(max_r - 1.)));

  bkg_ratio->SetLineColor(kRed-3);
  bkg_ratio->SetLineWidth(2);
  bkg_ratio->SetFillColor(0);
  sig_ratio->SetLineWidth(2);
  sig_ratio->SetFillColor(0);

  data_ratio->Draw("E1");
  sig_ratio->Draw("hist same");
  bkg_ratio->Draw("hist same");
  data_ratio->GetYaxis()->SetRangeUser(min_r, max_r);
  data_ratio->GetXaxis()->SetRangeUser(xmin_, xmax_);
  data_ratio->GetXaxis()->SetLabelSize(0.08);
  data_ratio->GetYaxis()->SetLabelSize(0.08);

  //Draw each stack component ratio
  const int nhists = stack->GetNhists();
  for(int ihist = 0; ihist < nhists; ++ihist) {
    TH1* h     = (TH1*) stack  ->GetHists()->At(ihist)->Clone(Form("h_stack_ratio_%i", ihist));
    TH1* h_nom = (TH1*) hstack_->GetHists()->At(ihist);
    h->Add(h_nom, -1); //subtract off the nominal to get the shift
    //Add the nominal and then divide by it to get the relative effect on the total
    h->Add(nominal);
    h->Divide(nominal);
    int color = h_nom->GetLineColor();
    if(color == kYellow - 7) color = kOrange; //make some colors more visible
    if(color == kGreen  - 7) color = kGreen - 3;
    h->SetLineColor(color);
    h->SetLineWidth(3);
    h->SetFillColor(0);
    h->SetLineStyle(kDashed);
    h->Draw("hist same");
    h_to_delete.push_back(h);
  }

  //Print the figure
  c->SaveAs(fig_name);

  delete c;
  for(auto h : h_to_delete) delete h;

  return 0;
}

//---------------------------------------------------------------------------------------------------------------------
int get_systematics(int set, TString hist, TH1* hdata, TFile* f, TString canvas_name) {
  int status(0);
  f->cd();
  dataplotter_->rebinH_ = overall_rebin_;
  hbkg_->SetLineColor(kRed-3); //nominal background
  hbkg_->SetFillColor(0);
  TH1* hdata_ratio = (TH1*) hdata->Clone("hdata_ratio"); //data ratio doesn't change
  hdata_ratio->Divide(hbkg_);

  CLFV::Systematics systematics;

  if(print_sys_plots_) gSystem->Exec(Form("mkdir -p %s_sys", canvas_name.Data()));

  //Loop through each systematic, creating PDFs and example figures
  const int sys_start = (test_sys_ >= 0) ? test_sys_     : (skip_shape_sys_) ? kMaxSystematics : 0;
  const int sys_max   = (test_sys_ >= 0) ? test_sys_ + 1 : (use_scale_sys_ ) ? kMaxSystematics + kMaxScaleSystematics : kMaxSystematics;
  for(int isys = sys_start; isys < sys_max; ++isys) {
    if(isys > 0 && systematics.GetName(isys) == "") continue; //only retrieve defined systematics

    THStack* hstack = nullptr;
    vector<TH1*> signals;
    if(isys >= kMaxSystematics) { //normalization systematics
      //retrieve the systematic scale factor
      CLFV::ScaleUncertainty_t sys_scale = systematics.GetScale(isys);
      if(sys_scale.name_ == "") { //not implemented
        cout << __func__ << " Systematic " << isys << " (" << systematics.GetName(isys).Data() << ") not implemented\n";
        continue;
      }
      if(sys_scale.scale_ <= 0.) {
        cout << __func__ << ": Warning! Scale uncertainty " << isys << "(" << sys_scale.name_.Data() << ") is <= 0 (" << sys_scale.scale_ << ")\n";
      }
      if(test_sys_ >= 0 || verbose_ > 0) {
        printf("%s: Scale systematic %3i (%s): Scale = %.3f, data = %o, mc = %o, embed = %o\n", __func__, isys,
               sys_scale.name_.Data(), sys_scale.scale_, sys_scale.data_, sys_scale.mc_, sys_scale.embed_);
        sys_scale.Print();
      }
      if(test_sys_ >= 0) dataplotter_->verbose_ = 7;
      //Get background for the systematic
      hstack = dataplotter_->get_stack(Form("%s_0", hist.Data()), "systematic", set, &sys_scale);
      //Get the signals
      signals = dataplotter_->get_signal(Form("%s_0", hist.Data()), "systematic", set, &sys_scale);
    } else {
      //Get background for the systematic
      hstack = dataplotter_->get_stack(Form("%s_%i", hist.Data(), max(0, isys)), "systematic", set);
      //Get the signals
      signals = dataplotter_->get_signal(Form("%s_%i", hist.Data(), max(0, isys)), "systematic", set);
    }
    if(!hstack) {++status; continue;}
    if(!hstack->GetStack()) {++status; continue;}
    hstack->SetName(Form("hstack_sys_%i", isys));
    TH1* hbkg = (TH1*) hstack->GetStack()->Last();
    hbkg->SetName(Form("hbkg_sys_%i", isys));
    if(signals.size() == 0) {++status; continue;}

    //Check the histograms
    bool pass_check = true;
    for(auto obj : *(hstack->GetHists())) {
      if(!check_histogram((TH1*) obj)) {++status; pass_check = false;}
    }
    for(auto obj : signals) {
      if(!check_histogram((TH1*) obj)) {++status; pass_check = false;}
    }
    if(!pass_check) continue;


    /////////////////////////////////////////////////
    // Plot the shifted distributions

    if(print_sys_plots_) {
      TString fig_name ;
      if(use_sys_name_ && isys >= 0) {
        TString name = systematics.GetName(isys);
        if(name == "") fig_name = Form("%s_sys/sys_%i.png", canvas_name.Data(), isys);
        else           fig_name = Form("%s_sys/sys_%s.png", canvas_name.Data(), Form("%s_%s", name.Data(), (systematics.IsUp(isys)) ? "up" : "down"));
      } else {
        fig_name = Form("%s_sys/sys_%i.png", canvas_name.Data(), isys);
      }
      print_systematic_canvas(hbkg_, hdata, hstack, hsigs_[0], signals[0], Form("c_sys_%i", isys), fig_name.Data());
      // TCanvas* c = new TCanvas(Form("c_sys_%i", isys), Form("c_sys_%i", isys), 1200, 900);
      // TPad* pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1. );
      // TPad* pad2 = new TPad("pad2", "pad2", 0., 0. , 1., 0.3);
      // pad1->Draw(); pad2->Draw();
      // pad1->cd();

      // //Get the systematic shift as a TGraph
      // TGraph* g_bkg = dataplotter_->get_errors(hbkg_, hbkg, 0);
      // g_bkg->SetFillColor(kRed-3);
      // g_bkg->SetLineWidth(2);
      // g_bkg->SetLineColor(kRed-3);
      // g_bkg->SetFillStyle(3001);
      // g_bkg->Draw("AP2");
      // hbkg_->Draw("hist same");
      // g_bkg->Draw("P2");

      // double ymax = max(hdata->GetMaximum(), hbkg_->GetMaximum());
      // //Get the signal systematic shifts
      // for(unsigned index = 0; index < signals.size(); ++index) {
      //   auto h = signals[index];
      //   auto hnom = hsigs_[index];
      //   TGraph* g_sig = dataplotter_->get_errors(hnom, h, 0);
      //   hnom->SetFillColor(0);
      //   hnom->SetLineWidth(2);
      //   hnom->Draw("hist same");
      //   g_sig->SetLineColor(hnom->GetLineColor());
      //   g_sig->SetFillStyle(3001);
      //   g_sig->SetFillColor(g_sig->GetLineColor());
      //   g_sig->Draw("P2");
      //   ymax = max(ymax, g_sig->GetMaximum());
      // }
      // hdata->Draw("same E");
      // g_bkg->GetXaxis()->SetRangeUser(xmin_,xmax_);
      // g_bkg->GetYaxis()->SetRangeUser((logy_) ? 0.5 : 0.9,(logy_) ? 2*ymax : 1.1*ymax);
      // g_bkg->SetTitle("");
      // if(logy_) pad1->SetLogy();

      // //Draw the ratio plot
      // pad2->cd();
      // double r_min, r_max;
      // TGraph* g_r_bkg = dataplotter_->get_errors(hbkg_, hbkg, 0, true, r_min, r_max);
      // g_r_bkg->SetFillColor(kRed-3);
      // g_r_bkg->SetLineWidth(2);
      // g_r_bkg->SetLineColor(kRed-3);
      // g_r_bkg->SetFillStyle(3001);
      // g_r_bkg->Draw("ALE2");
      // g_r_bkg->SetName(Form("g_r_bkg_sys_%i", isys));

      // for(unsigned index = 0; index < signals.size(); ++index) {
      //   auto h = signals[index];
      //   auto hnom = hsigs_[index];
      //   double r_min_s, r_max_s;
      //   TGraph* g_sig = dataplotter_->get_errors(hnom, h, 0, true, r_min_s, r_max_s);
      //   r_min = min(r_min, r_min_s);
      //   r_max = max(r_max, r_max_s);
      //   g_sig->SetLineColor(hnom->GetLineColor());
      //   if(index == 0) {
      //     g_sig->SetFillColor(g_sig->GetLineColor());
      //     g_sig->SetFillStyle(3004);
      //     g_sig->Draw("PL");
      //     g_sig->Draw("PLE2");
      //   } else {
      //     g_sig->Draw("PL");
      //   }
      // }
      // hdata_ratio->Draw("same E");
      // g_r_bkg->GetXaxis()->SetRangeUser(xmin_,xmax_);
      // g_r_bkg->GetXaxis()->SetLabelSize(0.08);
      // g_r_bkg->GetYaxis()->SetLabelSize(0.08);
      // g_r_bkg->GetYaxis()->SetRangeUser(max(0.5, min(0.95, 1. + 1.15*(r_min - 1.))), min(1.5, max(1.05, 1. + 1.15*(r_max - 1.))));
      // g_r_bkg->SetTitle("");

      // if(use_sys_name_ && isys >= 0) {
      //   TString name = systematics.GetName(isys);
      //   if(name == "") c->SaveAs(Form("%s_sys/sys_%i.png", canvas_name.Data(), isys));
      //   else           c->SaveAs(Form("%s_sys/sys_%s.png", canvas_name.Data(), Form("%s_%s", name.Data(), (systematics.IsUp(isys)) ? "up" : "down")));
      // } else {
      //   c->SaveAs(Form("%s_sys/sys_%i.png", canvas_name.Data(), isys));
      // }
      // delete c;
    } //end if(print_sys_plots_)

    /////////////////////////////////////////////////
    // Write the shifted histograms to disk

    for(auto h : signals) {
      TString hname = h->GetName();
      if(dataplotter_->signal_scales_.find(hname) != dataplotter_->signal_scales_.end())
        h->Scale(1./dataplotter_->signal_scales_[hname]);
      else
        h->Scale(1./dataplotter_->signal_scale_);
      hname = hname(0,hname.First('_'));
      hname.ReplaceAll("#", "");
      hname.ReplaceAll("->", "");
      // hname.ReplaceAll(Form("_%s_%i_%i", hist.Data(), isys, set), "");
      hname.ToLower();
      hname += "_sys_"; hname += isys;
      h->SetName(hname.Data());
      h->Write();
    }
    hbkg->Write();
    hstack->Write();
    // f->Flush();
  }
  return status;
}

//---------------------------------------------------------------------------------------------------------------------
int get_individual_MVA_histogram(int set = 8, TString selection = "zmutau",
                                 vector<int> years = {2016, 2017, 2018},
                                 TString base = "nanoaods_dev") {

  ///////////////////////////////////////////////
  // Initialize setup

  hsigs_ = {};
  int status(0);

  //Get the name of the MVA for this selection
  TString hist;
  if     (selection == "hmutau"  ) hist = "mva0";
  else if(selection == "zmutau"  ) hist = "mva1";
  else if(selection == "hetau"   ) hist = "mva2";
  else if(selection == "zetau"   ) hist = "mva3";
  else if(selection == "hemu"    ) hist = "mva4";
  else if(selection == "zemu"    ) hist = "mva5";
  else if(selection == "hmutau_e") hist = "mva6";
  else if(selection == "zmutau_e") hist = "mva7";
  else if(selection == "hetau_mu") hist = "mva8";
  else if(selection == "zetau_mu") hist = "mva9";
  else {
    cout << "Unidentified selection " << selection.Data() << endl;
    return -1;
  }
  if(use_dev_mva_ > 0) hist += Form("_%i", use_dev_mva_);

  //define parameters for dataplotter script
  TString selec = selection; selec.ReplaceAll("z", ""); selec.ReplaceAll("h", "");
  useEmbed_ = (embed_mode_ == 2) ? !selection.Contains("_") : embed_mode_; //whether or not to use embedding
  years_ = years;
  status = initialize_plotter(selec, base);
  if(status) {
    cout << "DataPlotter initialization returned " << status << ", exiting!\n";
    return status;
  }

  int set_offset = (use_lep_tau_set_) ? (selec.Contains("mutau")) ? HistMaker::kMuTauE : HistMaker::kETauMu : HistMaker::kEMu;
  if     (selec == "mutau") set_offset = HistMaker::kMuTau;
  else if(selec == "etau" ) set_offset = HistMaker::kETau;

  dataplotter_->rebinH_ = overall_rebin_;

  ///////////////////////////////////////////////
  // Get the distributions

  //get background distribution
  THStack* hstack = dataplotter_->get_stack(hist, "event", set+set_offset);
  if(!hstack || hstack->GetNhists() == 0) return 1;

  //get the signal vector
  std::vector<TH1*> signals = dataplotter_->get_signal(hist, "event", set+set_offset);
  for(auto h : signals) {
    if(!h) return 2;
  }
  if(signals.size() == 0) return 2;

  //get the data
  TH1* hdata = dataplotter_->get_data(hist, "event", set+set_offset);
  if(!hdata) return 3;
  hdata->SetName("hdata");
  TH1* hdata_plot = nullptr; //for plotting (potentially blind) data

  //determine an appropriate plotting range and if log or linear plot
  TH1* hlast = (TH1*) hstack->GetStack()->Last()->Clone("hbackground");
  const int nbins = hlast->GetNbinsX();
  xmin_ = hlast->GetBinLowEdge(1);
  xmax_ = hlast->GetBinLowEdge(1) - 1.;
  int ndec(0), nnonzero(0);
  for(int ibin = 1; ibin <= nbins; ++ibin) {
    const double binc = hlast->GetBinContent(ibin);
    //if haven't found a non-zero bin yet, update xmin, else update xmax
    if(binc <= 0. && xmax_ < xmin_) xmin_ = hlast->GetBinLowEdge(ibin);
    else if(binc > 0.) xmax_ = hlast->GetBinLowEdge(ibin) + hlast->GetBinWidth(ibin);
    if(binc > 0.) {
      ++nnonzero;
      if(ibin < nbins && hlast->GetBinContent(ibin+1)/binc < 0.95) ++ndec;
    }
  }
  logy_ = ndec > 0.75*nnonzero; //if generally an exponential-like drop, plot in log
  logy_ |= hlast->GetBinLowEdge(1) > -0.5; //FIXME: CDF flag based on x-axis range to use log y

  //blind high MVA score region if desired
  hdata_plot = hdata; //default to plotting the same data
  if(blind_data_) {
    double mva_cut = (xmin_ > -0.1) ? 0.5 : 0.;
    int bin_start = hdata->FindBin(mva_cut);
    if(blind_data_ == 1) { //only blind the plots
      hdata_plot = (TH1*) hdata->Clone("hdata_blinded");
      for(int bin = bin_start; bin <= hdata_plot->GetNbinsX() + 1; ++bin) {
        hdata_plot->SetBinContent(bin, 0.);
        hdata_plot->SetBinError(bin, 0.5);
      }
    } else if(blind_data_ == 2) { //blind the data itself
      for(int bin = bin_start; bin <= hdata->GetNbinsX() + 1; ++bin) {
        hdata->SetBinContent(bin, 0.);
        hdata->SetBinError(bin, 0.5);
      }
    }
  }

  ///////////////////////////////////////////////
  // Make an initial plot of the results

  TCanvas* c = new TCanvas("c", "c", 1300, 1200);
  TPad* pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1. );
  TPad* pad2 = new TPad("pad2", "pad2", 0., 0. , 1., 0.3);
  pad1->Draw(); pad2->Draw();
  pad1->SetBottomMargin(0.06);
  pad2->SetTopMargin(0.03);
  pad1->cd();

  TLegend* leg = new TLegend(0.1, 0.7, 0.9, 0.9);
  leg->SetNColumns(2);

  double ymax = max(hdata_plot->GetMaximum(), hstack->GetMaximum());
  hdata_plot->Draw("E");
  leg->AddEntry(hdata_plot, "Data", "PL");
  hstack->Draw("same hist noclear");
  for(auto h : signals) {
    h->Draw("hist same");
    leg->AddEntry(h, h->GetTitle(), "F");
    ymax = max(ymax, h->GetMaximum());
  }
  hdata_plot->Draw("same E");
  for(auto o : *(hstack->GetHists())) {
    leg->AddEntry((TH1*) o, o->GetTitle(), "F");
  }
  leg->Draw();

  cout << "Plotting parameters: xmin = " << xmin_ << " xmax = " << xmax_ << " logy = " << logy_ << endl;

  hdata_plot->GetXaxis()->SetRangeUser(xmin_,xmax_);
  logy_ |= use_dev_mva_ == 2;
  if(logy_) {
    hdata_plot->GetYaxis()->SetRangeUser(0.5,max(1., (pow(10, 0.4*std::log10(ymax))*ymax)));
    pad1->SetLogy();
  } else {
    hdata_plot->GetYaxis()->SetRangeUser(0.9,1.2*ymax);
  }

  //make a ratio plot
  pad2->cd();
  TH1* hratio = (TH1*) hdata_plot->Clone("data_ratio");
  hratio->Divide(hlast);
  hratio->Draw("E1");
  hratio->SetTitle("");
  hratio->SetYTitle("Data/Bkg");
  hratio->GetYaxis()->SetRangeUser(0.5, 1.5);
  hratio->GetXaxis()->SetRangeUser(xmin_, xmax_);
  hratio->GetYaxis()->SetTitleSize(0.12);
  hratio->GetYaxis()->SetTitleOffset(0.40);
  hratio->GetYaxis()->SetLabelSize(0.08);
  hratio->GetXaxis()->SetLabelSize(0.08);
  TLine line(xmin_, 1., xmax_, 1.);
  line.SetLineColor(kBlack);
  line.SetLineStyle(kDashed);
  line.SetLineWidth(2);
  line.Draw("same");
  pad2->SetGrid();


  ///////////////////////////////////////////////
  // Save the results

  TString year_string;
  for(unsigned i = 0; i < years.size(); ++i) {
    if(i > 0) year_string += "_";
    year_string += years[i];
  }

  //remove dev label for systematics/naming, since they're not defined
  if(use_dev_mva_ > 0) hist.ReplaceAll(Form("_%i", use_dev_mva_), "");

  gSystem->Exec(Form("[ ! -d plots/latest_production/%s ] && mkdir -p plots/latest_production/%s", year_string.Data(), year_string.Data()));
  TString canvas_name = Form("plots/latest_production/%s/hist_%s_%s_%i", year_string.Data(), hist.Data(), selection.Data(), set);
  c->SaveAs(canvas_name + ".png");
  delete hratio;

  //print statistical uncertainty plot
  plot_stat_sys(hstack, signals, hdata_plot, canvas_name + "_stat.png");

  //open file to save histograms to
  gSystem->Exec("[ ! -d histograms ] && mkdir histograms");
  TFile* fout = new TFile((save_) ? Form("histograms/%s_%s_%i_%s.hist", selection.Data(), hist.Data(), set, year_string.Data()) : "TMP.root", "RECREATE");
  hdata->Write();
  hstack->SetName("hstack");
  hstack->Write();
  hlast->Write();
  hbkg_ = (TH1*) hlast->Clone("hbkg_");
  hstack_ = (THStack*) hstack->Clone("hstack_");

  //write each signal in the standard way (e.g. H->e#mu --> hemu, Z->#mu#tau --> zmutau)
  for(auto h : signals) {
    TString hname = h->GetName();
    hname.ReplaceAll("#", "");
    hname.ReplaceAll("->", "");
    if(use_dev_mva_ > 0)
      hname.ReplaceAll(Form("_%s_%i_%i", hist.Data(), use_dev_mva_, set+set_offset), "");
    else
      hname.ReplaceAll(Form("_%s_%i", hist.Data(), set+set_offset), "");
    hname.ToLower();
    h->SetName(hname.Data());
    hsigs_.push_back((TH1*) h->Clone(Form("hsig_%s_", hname.Data()))); //store the signal for plotting against systematic
    //remove the scale factor applied to the signal
    if(dataplotter_->signal_scales_.find(hname) != dataplotter_->signal_scales_.end()) {
      h->Scale(1./dataplotter_->signal_scales_[hname]);
    } else {
      h->Scale(1./dataplotter_->signal_scale_);
    }
    //save each signal
    h->Write();
  }

  //get the systematics if needed
  if(!ignore_sys_)
    status += get_systematics(set+set_offset, hist, hdata_plot, fout, canvas_name);

  //save the results
  fout->Write();
  fout->Close();
  delete fout;

  //if copied files locally, remove them
  if(copy_local_) {
    remove_local(selection);
  }

  return status;
}

int get_MVA_histogram(vector<int> sets = {25,26,27,28}, TString selection = "zmutau",
                      vector<int> years = {2016, 2017, 2018},
                      TString base = "nanoaods_dev",
                      int had_only = 0 /*0: all; 1: hadronic; -1: leptonic; -2: same-flavor*/) {
  int status(0);
  if(use_dev_mva_ == 1) overall_rebin_ = 50;
  if(use_dev_mva_ == 2) overall_rebin_ = 4;
  useLepTauSet_ = use_lep_tau_set_;
  includeHiggs_ = selection.BeginsWith("h");

  if(embed_mode_ != 1) cout << "!!! Warning! Using non-standard embedding mode " << embed_mode_ << endl;

  //loop through the year list if separating years
  for(int iyear = 0; iyear < ((separate_years_) ? years.size() : 1); ++iyear) {
    vector<int> years_use;
    if(separate_years_) years_use = {years[iyear]}; //do a single year at a time
    else                years_use = years;

    //loop through the histogram set list for fitting in categories
    for(int set : sets) {
      //get the hadronic tau region
      if(had_only >= 0) status += get_individual_MVA_histogram(set, selection, years_use, base);
      //get the leptonic tau region
      if(test_sys_ < 0 && (had_only == 0 || had_only == -1)) { //only do one selection if debugging
        if(selection.Contains("mutau"))
          status += get_individual_MVA_histogram(set, selection+"_e", years_use, base);
        else if(selection.Contains("etau"))
          status += get_individual_MVA_histogram(set, selection+"_mu", years_use, base);
      }
    }
  }
  // gApplication->Terminate(status); //speed up the ending of the processing
  return status;
}
