//Script to prepare a rootfile and datacard for Higgs Combine tools
#include "mva_systematic_names.C"
#include "bemu_defaults.C"
#include "../interface/GlobalConstants.h"
// #include "perform_f_test.C"
#include "construct_multidim.C"
#include "bemu_fit_bkg_mc.C"
#include "util.hh"

#include <iostream>
#include <fstream>

using namespace CLFV;

bool useRateParams_ = false;
bool fixSignalPDF_  = true ;
bool useMultiDim_   = true ;
bool includeSys_    = false; //FIXME: restore to true (needs re-histogramming) -- flag to ignore most systematics
bool twoSidedSys_   = true ; //write both up and down for each rate systematic
bool addESShifts_   = true ; //include constrained parameters for the energy scale uncertainties
bool useMCBkg_      = false; //use the background MC to create background template PDFs
float zmumu_scale_  =   -1.; //scale to Z->ee/mumu distribution if using MC templates
bool printPlots_    = true ;
bool fitSideBands_  = true ; //fit only the data sidebands
int  replaceData_   =     1; //1: replace the data with toy MC; 2: replace the data with the MC bkg; 3: replace the data with smoothed/fit MC bkg
bool replaceRefit_  = false; //replace data with toy MC, then fit the unblinded toy data
bool export_        = false; //if locally run, export the workspace to LPC
bool save_          = true ; //save output combine workspace/cards

TString tag_; //for output cards

//Retrieve yields for each relevant systematic
void get_systematics(TFile* f, TString label, int set, vector<pair<double,double>>& yields, vector<TString>& names, double xmin = 1., double xmax = -1.) {
  //offset the set number to the absolute set value
  if     (label == "zee"   || label == "eeBkg"  ) set += HistMaker::kEE;
  else if(label == "zmumu" || label == "mumuBkg") set += HistMaker::kMuMu;
  else                                            set += HistMaker::kEMu;

  //only add systematic names to the list once
  bool addNames = names.size() == 0;
  TString prev_name = "";

  //for each relevant systematic, record the yield
  int bin1(-1), bin2(-1);
  for(int isys = 1; isys < kMaxSystematics; ++isys) {
    auto sys_info = systematic_name(isys, "emu", 2016); //FIXME: Get actual year
    TString sys_name = sys_info.first;
    //if not a defined/used systematic or if not the second of the set, continue
    if(sys_name == "" || sys_name != prev_name) {
      prev_name = sys_name;
      continue;
    }
    prev_name = sys_name; //store for the next loop
    TString hist_up, hist_down;
    //up is always the index preceding the down index
    if(set > HistMaker::kMuMu) hist_up   = Form("%s_sys_%i", (label.Contains("Bkg")) ? "hbkg" : "hDY", isys-1);
    else                       hist_up   = Form("%s_lepm_%i_%i_sys_%i", label.Data(), isys-1, set, isys-1);
    if(set > HistMaker::kMuMu) hist_down = Form("%s_sys_%i", (label.Contains("Bkg")) ? "hbkg" : "hDY", isys);
    else                       hist_down = Form("%s_lepm_%i_%i_sys_%i", label.Data(), isys, set, isys);
    TH1* h_up   = (TH1*) f->Get(hist_up.Data());
    TH1* h_down = (TH1*) f->Get(hist_down.Data());
    if(!h_up || !h_down) {
      cout << "!!! Systematic " << sys_name.Data() << "(" << isys-1 << "/" << isys << ") not found for set " << set << " and label " << label.Data()
           << " hist up name = " << hist_up.Data() << endl;
      continue;
    }
    if(sys_name.EndsWith("ES")) {
      printf("Systematic %s: (up mean, down mean) = (%.4f, %.4f)\n", sys_name.Data(), h_up->GetMean(), h_down->GetMean());
    }
    double yield_up, yield_down;
    if(xmin > xmax) {
      yield_up   = h_up  ->Integral();
      yield_down = h_down->Integral();
    } else {
      if(bin1 < 0 || bin2 < 0) {
        bin1 = std::max(1, std::min(h_up->GetNbinsX(), h_up->FindBin(xmin)));
        bin2 = std::max(1, std::min(h_up->GetNbinsX(), h_up->FindBin(xmax)));
      }
      yield_up   = h_up  ->Integral(bin1,bin2);
      yield_down = h_down->Integral(bin1,bin2);
    }
    yields.push_back(pair<double,double>(yield_up,yield_down));
    if(addNames) names.push_back(sys_name);
  }
}

//Main conversion function
Int_t convert_individual_bemu_to_combine(int set = 8, TString selection = "zemu",
                                         vector<int> years = {2016, 2017, 2018},
                                         int seed = 90) {

  //////////////////////////////////////////////////////////////////
  // Initialize relevant variables
  //////////////////////////////////////////////////////////////////

  int status(0);

  gStyle->SetOptStat(0);

  TString year_string = "";
  for(unsigned i = 0; i < years.size(); ++i) {
    if(i > 0) year_string += "_";
    year_string += years[i];
  }
  TString set_string = Form("%i", set);

  bool isHiggs = selection.Contains("h");
  const double xmin = (isHiggs) ? 110. :  70.;
  const double xmax = (isHiggs) ? 160. : 110.;
  const double signal_mass = (isHiggs) ? 125. : 91.;

  //determine the signal name and branching ratio
  TString selec = selection;  selec.ReplaceAll("_e", ""); selec.ReplaceAll("_mu", "");
  TString signame = selec;
  signame.ReplaceAll("z", "Z"); signame.ReplaceAll("h", "H");
  signame.ReplaceAll("m", "M"); signame.ReplaceAll("e", "E"); signame.ReplaceAll("t", "T");
  CrossSections xs;
  double bxs = xs.GetCrossSection(isHiggs ? "H" : "Z");
  double xs_sig = xs.GetCrossSection(signame.Data());
  double br_sig = xs_sig / bxs;
  double sig_scale = 1.;
  if(!useDefaultBr_) { //use fixed example branching fraction
    double example_br = (isHiggs) ? 1.e-4 : 1.e-7;
    sig_scale = example_br / br_sig;
    br_sig = example_br;
  }

  //////////////////////////////////////////////////////////////////
  // Read in the data
  //////////////////////////////////////////////////////////////////

  vector<pair<double,double>> sig_sys; //only consider systematics on signal and control region yields for now
  vector<TString> sys_names;
  TFile* fInput = TFile::Open(Form("histograms/%s_lepm_%i_%s.hist",
                                   selection.Data(), set, year_string.Data()), "READ");
  if(!fInput) return 1;

  if(verbose_ > 1) fInput->ls();

  TH1* bkg = (TH1*) fInput->Get("hbackground");
  if(!bkg) {
    cout << "Background histogram for set " << set << " not found\n";
    return 2;
  }
  THStack* stack_in = (THStack*) fInput->Get("hstack");
  if(!stack_in) {
    cout << "Background stack for set " << set << " not found\n";
    return 2;
  }
  TH1* sig = (TH1*) fInput->Get(selection.Data());
  if(!sig) {
    cout << "Signal histogram for set " << set << " not found\n";
    return 3;
  }
  TH1* data = (TH1*) fInput->Get("hdata");
  if(!bkg) {
    cout << "Data histogram for set " << set << " not found\n";
    return 4;
  }

  bkg->SetDirectory(0);
  bkg->SetName(Form("bkg_%i", set));
  sig->SetDirectory(0);
  sig->SetName(Form("%s_%i", selection.Data(), set));
  data->SetDirectory(0);
  data->SetName(Form("data_obs_%i", set));

  //ensure the plot directory is created
  gSystem->Exec(Form("[ ! -d plots/latest_production/%s ] && mkdir -p plots/latest_production/%s", year_string.Data(), year_string.Data()));

  //replace the background stack with smoothed/fit histograms
  THStack* stack = (useMCBkg_ || replaceData_ == 3) ? new THStack("bkg_stack", "Background stack") : stack_in;
  if(useMCBkg_ || replaceData_ == 3 || zmumu_model_) {
    TString mc_fig_dir = Form("plots/latest_production/%s/convert_bemu_%s_%i_mc_fits", year_string.Data(), selection.Data(), set);
    gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s", mc_fig_dir.Data(), mc_fig_dir.Data()));
    const int nhists(stack_in->GetNhists());
    for(int ihist = 0; ihist < nhists; ++ihist) {
      TH1* h = (TH1*) stack_in->GetHists()->At(ihist);
      h = make_safe(h, xmin, xmax);
      bool isflat(false);
      isflat |= TString(h->GetName()).Contains("QCD"  );
      isflat |= TString(h->GetName()).Contains("Top"  );
      isflat |= TString(h->GetName()).Contains("Other");
      isflat |= TString(h->GetName()).Contains("Z->ee");
      const bool isdy = TString(h->GetName()).Contains("#tau#tau");
      const bool isembed = TString(h->GetName()).Contains("Embed");
      const bool iszmumu = TString(h->GetName()).Contains("Z->ee");
      if(isflat) { //flat-ish distributions
        fit_and_replace(h, xmin, xmax, mc_fig_dir.Data(), set, 2);
      }
      const bool fit_dy_bkg = true; //whether or not to fit the Z->tautau background
      const bool smooth_hists(true); //smooth histograms that aren't fit (Embedding
      if(isdy) { //Z->tautau
        if(fit_dy_bkg) {
          fit_and_replace(h, xmin, xmax, mc_fig_dir.Data(), set, 1);
        } else if(smooth_hists) h->Smooth(1);
      }
      if(!isflat && !isdy && smooth_hists) h->Smooth(2); //any leftover histogram
      if(zmumu_scale_ >= 0. && iszmumu) h->Scale(zmumu_scale_); //scale to Z->ee/mumu distribution for systematic studies
      //scale Embedding to match the Drell-Yan MC normalization
      if(isembed) {
        if     (set == 13) h->Scale(6.52/5.70);
        else if(set == 12) h->Scale(1.78/1.66);
        else if(set == 11) h->Scale(8.94/8.43);
      }
      //Additional scaling from fitting MC to data sidebands
      const bool apply_mc_fit_scale = true;
      if(apply_mc_fit_scale) {
        if(isembed) {
          if     (set == 13) h->Scale(0.9276);
          else if(set == 12) h->Scale(1.0043);
          else if(set == 11) h->Scale(0.9819);
        }
        if(isflat && !iszmumu) {
          if     (set == 13) h->Scale(1.6084);
          else if(set == 12) h->Scale(1.2802);
          else if(set == 11) h->Scale(1.1411);
        }
      }
      h->SetDirectory(0);
      TString name = h->GetName();
      name.ReplaceAll("#","");
      name.ReplaceAll("/","");
      name.ReplaceAll("->","to");
      name.ReplaceAll(" ","");
      name.ReplaceAll(Form("_lepm_event_%i_safe_fit", set), "");
      name.ReplaceAll(Form("_lepm_%i_safe_fit", set), "");
      name.ReplaceAll("_","");
      stack->Add(h);
    }
    TCanvas* c = new TCanvas();
    stack->Draw("hist noclear");
    c->SaveAs((mc_fig_dir + "/stack.png").Data());
    delete c;
  }

  //Get the Z->mumu distribution
  TH1* h_zmumu = nullptr;
  for(auto o : *(stack->GetHists())) {
    if(TString(o->GetName()).Contains("Z->ee")) {
      h_zmumu = (TH1*) o->Clone("h_zmumu");
      h_zmumu->SetDirectory(0); //don't associate it with the open file
      break;
    }
  }
  if(zmumu_model_ && !h_zmumu) {
    printf("%s: Unable to retrieve Z->mumu histogram for set %i\n", __func__, set);
    return 10;
  }
  const float zmumu_yield = (h_zmumu) ? h_zmumu->Integral(h_zmumu->FindBin(xmin+1.e-3), h_zmumu->FindBin(xmax-1.e-3)) : 0.;

  //Get systematics if requested
  if(includeSys_)
    get_systematics(fInput, selection, set, sig_sys, sys_names, xmin+1.e-3, xmax-1.e-3);

  //Close the input file
  fInput->Close();

  //////////////////////////////////////////////////////////////////
  // Configure the output file
  //////////////////////////////////////////////////////////////////

  TString outName = Form("combine_bemu_%s%s_%s%s.root", selection.Data(), (useMCBkg_) ? "_mc" : "", set_string.Data(), tag_.Data());
  TFile* fOut = nullptr;
  if(save_) fOut = new TFile(("datacards/"+year_string+"/"+outName).Data(), "RECREATE");

  //////////////////////////////////////////////////////////////////
  // Configure the data card
  //////////////////////////////////////////////////////////////////

  //Create directory for the data cards if needed
  gSystem->Exec(Form("[ ! -d datacards/%s ] && mkdir -p datacards/%s", year_string.Data(), year_string.Data()));
  TString filepath = Form("datacards/%s/combine_bemu_%s%s_%s%s.txt", year_string.Data(), selection.Data(), (useMCBkg_) ? "_mc" : "", set_string.Data(), tag_.Data());
  std::ofstream outfile;
  if(save_)
    outfile.open(filepath.Data());
  else outfile.open("TMP.txt"); //save to a temporary card
  if(!outfile.is_open()) return 10;

  outfile << "# -*- mode: tcl -*-\n";
  outfile << "#Auto generated Higgs Combine datacard for CLFVAnalysis\n";
  outfile << Form("#Signal branching fraction used: %.3e\n\n", br_sig);
  outfile << Form("imax %2i number of channels\n", 1 + 2*use_same_flavor_);
  outfile << "jmax  * number of backgrounds\n";
  outfile << "kmax  * number of nuisance parameters\n\n";
  outfile << "----------------------------------------------------------------------------------------------------------- \n";
  if(!use_same_flavor_) {
    outfile << Form("shapes * * %s $CHANNEL:$PROCESS\n", outName.Data());
  }

  //Start each line, building for each background process
  TString bins   = "bin                    "; //channel definition, 1 per channel
  TString bins_p = "bin                 "    ; //per process per channel channel listing
  TString proc_l = "process            "; //process definition per channel
  TString proc_c = "process            "; //process class per channel
  TString rate   = "rate               "; //process rate per channel
  TString obs    = "observation                "; //data observations, 1 per channel
  TString signorm; //for relative signal rates at example branching fraction
  TString cats; //for discrete category indices
  TString bkg_norm; //for allowing the background to float
  map<TString, TString> systematics; //map of systematic name to systematic effect line

  //////////////////////////////////////////////////////////////////
  // Add datacard info for each set
  //////////////////////////////////////////////////////////////////

  int ncat = 1; //for tracking background process IDs

  TString hist = Form("lepm_%i", set);
  RooWorkspace* ws = new RooWorkspace(hist.Data());
  // auto dir = fOut->mkdir(hist.Data());
  // dir->cd();

  if(use_same_flavor_) {
    outfile << Form("shapes * %-10s %s $CHANNEL:$PROCESS\n", hist.Data(), outName.Data());
  }
  //////////////////////////////////////////////////////////////////
  // Retrieve the histograms for this set
  //////////////////////////////////////////////////////////////////

  TH1* blindData = (TH1*) data->Clone("hblind_data");
  const double blind_min = (isHiggs) ? 120. : 86.;
  const double blind_max = (isHiggs) ? 130. : 96.;
  sig->Scale(sig_scale);

  for(int ibin = blindData->FindBin(blind_min)+1; ibin < blindData->FindBin(blind_max); ++ibin) {
    blindData->SetBinContent(ibin, 0.);
    blindData->SetBinError  (ibin, 0.);
  }

  // Create an observable for this category
  RooRealVar* lepm = new RooRealVar(Form("lepm_%i", set), "M_{e#mu}", (xmin+xmax)/2., xmin, xmax);
  int low_bin  = std::max(1, std::min(data->GetNbinsX(), data->FindBin(xmin+1.e-3)));
  int high_bin = std::max(1, std::min(data->GetNbinsX(), data->FindBin(xmax-1.e-3)));
  lepm->setBins(high_bin - low_bin + 1);
  lepm->setRange("full", xmin, xmax);
  lepm->setRange("LowSideband", xmin, blind_min);
  lepm->setRange("HighSideband", blind_max, xmax);
  lepm->setRange("BlindRegion", blind_min, blind_max);

  //create RooDataHist from histograms
  RooDataHist* bkgData  = new RooDataHist("bkg_data" ,  "Background Data", RooArgList(*lepm), bkg );
  RooDataHist* sigData  = new RooDataHist("sig_data" ,  "Signal Data"    , RooArgList(*lepm), sig );
  RooDataHist* dataData = new RooDataHist("data_data",  "Data Data"      , RooArgList(*lepm), data);
  RooDataHist* blindDataHist = new RooDataHist("blind_data_hist",  "Blind Data Hist", RooArgList(*lepm), blindData);

  //create RooDataHist for drawing a more visible signal
  const double br_scale = 1.0;
  TH1* sigVis = (TH1*) sig->Clone("sigVis");
  sigVis->Scale(br_scale);
  RooDataHist* sigDataVis  = new RooDataHist("sig_data_vis" ,  "Signal Data"    , RooArgList(*lepm), sigVis);

  //////////////////////////////////////////////////////////////////
  // Fit the signal distribution
  //////////////////////////////////////////////////////////////////

  cout << "--- Performing the signal fit for set " << set << endl;

  vector<RooRealVar*> sig_pdf_vars; //signal resonance parameters
 //energy scale nuisance shifts in the resonance mean
  RooRealVar* elec_ES_shift = new RooRealVar(Form("elec_ES_shift"), "electron ES shift", 0., -5., 5.); elec_ES_shift->setConstant(true);
  RooRealVar* muon_ES_shift = new RooRealVar(Form("muon_ES_shift"), "muon ES shift"    , 0., -5., 5.); muon_ES_shift->setConstant(true);
  RooRealVar* elec_ES_size  = new RooRealVar(Form("elec_ES_size"), "electron ES size"  , 0.260/signal_mass); elec_ES_size->setConstant(true);
  RooRealVar* muon_ES_size  = new RooRealVar(Form("muon_ES_size"), "muon ES size"      , 0.075/signal_mass); muon_ES_size->setConstant(true);

  RooAbsPdf* sigPDF; //full signal PDF
  if(zemu_signal_mode_ == 0) { //Use Crystal Ball with an additional Gaussian
    RooRealVar* mean     = new RooRealVar(Form("mean_%i"     , set), "mean", signal_mass, signal_mass - 5., signal_mass + 5.);
    RooRealVar* sigma    = new RooRealVar(Form("sigma_%i"    , set), "sigma", 2., 0.1, 5.);
    RooRealVar* alpha    = new RooRealVar(Form("alpha_%i"    , set), "alpha", 1., 0.1, 10.);
    RooRealVar* enne     = new RooRealVar(Form("enne_%i"     , set), "enne", 5., 0.1, 30.);
    RooRealVar* mean2    = new RooRealVar(Form("mean2_%i"    , set), "mean2", signal_mass, lepm->getMin(), lepm->getMax());
    RooRealVar* sigma2   = new RooRealVar(Form("sigma2_%i"   , set), "sigma2", 5., 0.1, 10.);
    RooCBShape* sigpdf1  = new RooCBShape(Form("sigpdf1_%i"  , set), "sigpdf1", *lepm, *mean, *sigma, *alpha, *enne);
    RooGaussian* sigpdf2 = new RooGaussian(Form("sigpdf2_%i" , set), "sigpdf2", *lepm, *mean2, *sigma2);
    RooRealVar* fracsig  = new RooRealVar(Form("fracsig_%i"  , set), "fracsig", 0.7, 0., 1.);

    sigPDF    = new RooAddPdf(Form("sigPDF_%i"    , set), "signal PDF", *sigpdf1, *sigpdf2, *fracsig);
    sig_pdf_vars.push_back(mean);
    sig_pdf_vars.push_back(sigma);
    sig_pdf_vars.push_back(alpha);
    sig_pdf_vars.push_back(enne);
    sig_pdf_vars.push_back(mean2);
    sig_pdf_vars.push_back(sigma2);
  } else if(zemu_signal_mode_ == 1) {//Use a double-sided Crystall Ball
    RooRealVar* mean     = new RooRealVar(Form("mean_%i"     , set), "mean", signal_mass, signal_mass - 5., signal_mass + 5.);
    RooRealVar* sigma    = new RooRealVar(Form("sigma_%i"    , set), "sigma", 2., 0.1, 5.);
    RooRealVar* alpha1   = new RooRealVar(Form("alpha1_%i"   , set), "alpha1", 1., 0.1, 10.);
    RooRealVar* alpha2   = new RooRealVar(Form("alpha2_%i"   , set), "alpha2", 1., 0.1, 10.);
    RooRealVar* enne1    = new RooRealVar(Form("enne1_%i"    , set), "enne1", 5., 0.1, 30.);
    RooRealVar* enne2    = new RooRealVar(Form("enne2_%i"    , set), "enne2", 5., 0.1, 30.);
    RooFormulaVar* mean_func = new RooFormulaVar(Form("mean_func_%i", set), "mean with offset",
                                                 "@0*(1 + @1*@2 + @3*@4)", RooArgList(*mean, *elec_ES_shift, *elec_ES_size, *muon_ES_shift, *muon_ES_size));
    sigPDF  = new RooDoubleCrystalBall(Form("sigPDF_%i"  , set), "signal PDF", *lepm, *mean_func, *sigma, *alpha1, *enne1, *alpha2, *enne2);
    sig_pdf_vars.push_back(mean);
    sig_pdf_vars.push_back(sigma);
    sig_pdf_vars.push_back(alpha1);
    sig_pdf_vars.push_back(alpha2);
    sig_pdf_vars.push_back(enne1);
    sig_pdf_vars.push_back(enne2);
  }
  RooRealVar* N_sig    = new RooRealVar(Form("N_sig_%i"    , set), "N_sig", 2e5, 0, 3e6);
  RooAddPdf* totsigpdf = new RooAddPdf(Form("totSigPDF_%i" , set), "Signal PDF", RooArgList(*sigPDF), RooArgList(*N_sig));

  totsigpdf->fitTo(*sigData, RooFit::SumW2Error(1), RooFit::Extended(1));
  if(fixSignalPDF_) {
    for(auto var : sig_pdf_vars) var->setConstant(1);
  }

  if(addESShifts_) {
     elec_ES_shift->setConstant(false);
     muon_ES_shift->setConstant(false);
  }
  if(printPlots_) {
    TCanvas* c = new TCanvas(Form("c_sig_%i", set), Form("c_sig_%i", set), 1000, 1000);
    auto xframe = lepm->frame();
    xframe->SetTitle("");
    xframe->SetXTitle("M_{e#mu}");
    sigData->plotOn(xframe);
    sigPDF->plotOn(xframe);
    if(zemu_signal_mode_ == 0) {
      sigPDF->plotOn(xframe, RooFit::Components(Form("sigpdf1_%i", set)), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
      sigPDF->plotOn(xframe, RooFit::Components(Form("sigpdf2_%i", set)), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
    }
    xframe->Draw();
    c->SaveAs(Form("plots/latest_production/%s/convert_bemu_%s_%i%s_sig_pdf.png", year_string.Data(), selection.Data(), set, tag_.Data()));
    delete xframe;
    delete c;
  }

  //////////////////////////////////////////////////////////////////
  // Model Z->mumu distribution
  //////////////////////////////////////////////////////////////////

  RooRealVar* zmumu_mean   = new RooRealVar(Form("zmumu_mean_%i"  , set), "mean"  , (set == 13) ? 84.4 : (set == 12) ? 82.7 : 79.6); zmumu_mean  ->setConstant(true);
  RooRealVar* zmumu_sigma  = new RooRealVar(Form("zmumu_sigma_%i" , set), "sigma" , (set == 13) ? 4.48 : (set == 12) ? 5.14 : 6.98); zmumu_sigma ->setConstant(true);
  RooRealVar* zmumu_alpha1 = new RooRealVar(Form("zmumu_alpha1_%i", set), "alpha1", (set == 13) ? 1.22 : (set == 12) ? 0.80 : 4.13); zmumu_alpha1->setConstant(true);
  RooRealVar* zmumu_alpha2 = new RooRealVar(Form("zmumu_alpha2_%i", set), "alpha2", (set == 13) ? 1.78 : (set == 12) ? 1.85 : 1.99); zmumu_alpha2->setConstant(true);
  RooRealVar* zmumu_enne1  = new RooRealVar(Form("zmumu_enne1_%i" , set), "enne1" , (set == 13) ? 0.36 : (set == 12) ? 0.38 : 4.33); zmumu_enne1 ->setConstant(true);
  RooRealVar* zmumu_enne2  = new RooRealVar(Form("zmumu_enne2_%i" , set), "enne2" , (set == 13) ? 9.14 : (set == 12) ? 10.0 : 0.20); zmumu_enne2 ->setConstant(true);
  RooAbsPdf* zmumu  = new RooDoubleCrystalBall(Form("zmumu_%i"   , set), "Z->#mu#mu PDF", *lepm,
                                               *zmumu_mean, *zmumu_sigma, *zmumu_alpha1, *zmumu_enne1, *zmumu_alpha2, *zmumu_enne2);



  //////////////////////////////////////////////////////////////////
  // Fit the background distribution
  //////////////////////////////////////////////////////////////////

  RooCategory* categories = new RooCategory(Form("cat_%i", set), Form("cat_%i", set));
  int index = 0;
  // auto multiPDF = construct_simultaneous_pdf((fitSideBands_) ? *blindDataHist : *dataData , *lepm, *categories, fitSideBands_, index, set, 2);
  RooMultiPdf* multiPDF(nullptr);
  RooAbsPdf* bkgPDF(nullptr);
  //create a background template from the background stack
  TH1* hMCBkg = (TH1*) stack->GetStack()->Last()->Clone(Form("bkg_stack_hist_%i", set));
  hMCBkg->SetLineColor  (kBlack);
  hMCBkg->SetMarkerColor(kBlack);
  RooDataHist* bkgMCData = new RooDataHist(Form("bkgMCData_%i",set), "MC template data", *lepm, hMCBkg);
  //if using the MC templates, skip fitting the data
  if(!useMCBkg_) {
    multiPDF = construct_multipdf((fitSideBands_) ? *blindDataHist : *dataData , *lepm, *categories, fitSideBands_, index, set, 2);
    std::cout << "Finished constructing the multi-PDF background model for set " << set << std::endl;
    if(categories->numTypes() < 1) {
      std::cout << "MultiPDF has no PDFs in set " << set << std::endl;
      return 5;
    }
    bkgPDF = multiPDF->getPdf(index); //multiPDF->getPdf(Form("index_%i", index));
    categories->setIndex(index);
    cats += Form("%-8s discrete\n", categories->GetName());
    bkg_norm += Form("nbkg_%i rateParam lepm_%i bkg 1.\n", set, set);
    if(useMultiDim_) {
      multiPDF->SetName("bkg");
    } else {
      bkgPDF->SetName("bkg");
    }
  } else {
    bkgPDF = new RooHistPdf("bkg", "MC template PDF", *lepm, *bkgMCData);
  }

  const float ndata = data->Integral(low_bin, high_bin); //N(observed data)
  if(zmumu_model_) { //re-fit the PDFs to the data with the Z->mumu PDF included
    RooRealVar N_zmumu("n_zmumu", "N(Z->#mu#mu)", zmumu_yield); N_zmumu.setConstant(true); //assume a fixed number of Z->mumu events
    RooRealVar N_bkg("n_bkg", "N(bkg)", ndata, 0.1*ndata, 10.*ndata);
    RooAddPdf tot_bkg_pdf("tot_bkg_pdf", "Falling + Z->#mu#mu background", RooArgList(*bkgPDF, *zmumu), RooArgList(N_bkg, N_zmumu));
    if(fitSideBands_)
      tot_bkg_pdf.fitTo((fitSideBands_) ? *blindDataHist : *dataData, RooFit::PrintLevel(0), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1), RooFit::Range("LowSideband,HighSideband"));
    else
      tot_bkg_pdf.fitTo((fitSideBands_) ? *blindDataHist : *dataData, RooFit::PrintLevel(0), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1));
    printf("Re-fit the nominal PDF with Z->mumu included:\n");
    cout << "----------------------------------------------------------------------------" << endl;
    N_zmumu.Print();
    N_bkg.Print();
    auto vars = bkgPDF->getVariables();
    auto itr = vars->createIterator();
    auto var = itr->Next();
    while(var) {
      if(TString(var->GetName()) != lepm->GetName()) {
        var->Print();
      }
      var = itr->Next();
    }
    cout << "----------------------------------------------------------------------------" << endl;
  }

  //Generate toy data to stand in for the observed data if requested
  RooDataHist* dataset = dataData;
  if(replaceData_ == 1) dataset = bkgPDF->generateBinned(RooArgSet(*lepm), data->Integral(low_bin, high_bin));
  if(replaceData_ >= 2) {
    dataset = bkgMCData; dataData = bkgMCData; blindDataHist = bkgMCData; blindData = hMCBkg; data = hMCBkg; fitSideBands_ = 0; blind_data_ = 0; useDataBinErrors_ = true;
  }
  dataset->SetName("data_obs");

  //Re-fit the PDFs to the toy MC data
  if(!useMCBkg_ && replaceRefit_) {
    cout << "####################################################################" << endl
         << "!!! Re-fitting to the toy MC data generated!\n"
         << "####################################################################" << endl;
    for(int ipdf = 0; ipdf < categories->numTypes(); ++ipdf) {
      auto basePdf = multiPDF->getPdf(ipdf);
      //Wrap the PDF in a RooAddPdf
      RooRealVar* pdfNorm = new RooRealVar(Form("%s_norm", basePdf->GetName()), Form("%s_norm", basePdf->GetName()),
                                           hMCBkg->Integral(), 0., 2.*hMCBkg->Integral());
      RooAddPdf* pdf = new RooAddPdf(Form("%s_wrapper", basePdf->GetName()), basePdf->GetTitle(), RooArgList(*basePdf), RooArgList(*pdfNorm));
      pdf->fitTo(*dataset, RooFit::PrintLevel((verbose_ > 2) ? 1 : -1),
                 RooFit::Warnings(0), RooFit::PrintEvalErrors(-1));
      const double chi_sq = get_chi_squared(*lepm, pdf, *dataset, false);
      cout << "######################\n"
           << "### " << basePdf->GetTitle() << " has chisq = " << chi_sq << endl
           << "######################\n";
      delete pdfNorm;
      delete pdf;
    }
  }

  //////////////////////////////////////////////////////////////////
  // Plot the results of the background fits
  //////////////////////////////////////////////////////////////////

  if(printPlots_) {
    TCanvas* c = new TCanvas(Form("c_%i", set), Form("c_%i", set), 1000, 1000);
    TPad* pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1. ); pad1->Draw();
    TPad* pad2 = new TPad("pad2", "pad2", 0., 0. , 1., 0.3); pad2->Draw();
    pad2->SetBottomMargin(0.13); pad2->SetTopMargin(0.02);

    pad1->cd();
    TLegend* leg = new TLegend(0.4, 0.7, 0.9, 0.9);
    // const int nentries = 40;
    // auto xframe = lepm->frame(nentries);
    auto xframe = lepm->frame();
    xframe->SetTitle("");
    sigDataVis->plotOn(xframe, RooFit::Invisible());
    sigPDF->plotOn(xframe, RooFit::Name("sigPDF"), RooFit::LineColor(kRed), RooFit::NormRange("BlindRegion"), RooFit::Range("full"));
    if(blind_data_)
      dataData->plotOn(xframe, RooFit::Invisible());
    else
      dataData->plotOn(xframe);

    int nentries = dataData->numEntries();
    double chi_sq = get_chi_squared(*lepm, bkgPDF, *dataData, fitSideBands_, &nentries);
    bkgPDF->plotOn(xframe, RooFit::Name(bkgPDF->GetName()), RooFit::LineColor(kBlue),
                   (zmumu_model_) ? RooFit::Normalization(ndata - zmumu_yield, RooAbsReal::NumEvent) : RooFit::NormRange("full"),
                   RooFit::Range("full"));
    if(zmumu_model_)
      zmumu->plotOn(xframe, RooFit::Name(zmumu->GetName()), RooFit::LineColor(kGreen), RooFit::Normalization(zmumu_yield, RooAbsReal::NumEvent), RooFit::Range("full"));


    TString name = bkgPDF->GetName();
    TString title = bkgPDF->GetTitle();
    int order = (useMCBkg_) ? 0 : ((title(title.Sizeof() - 2)) - '0');
    vector<int> colors = {kRed, kViolet-7, kGreen-7, kOrange+2, kAtlantic, kRed+2, kMagenta, kOrange, kYellow-7};
    vector<double> chi_sqs;
    vector<double> p_chi_sqs;
    chi_sqs.push_back(chi_sq / (nentries - order - 2));
    p_chi_sqs.push_back(TMath::Prob(chi_sq, nentries - order - 2));
    cout << "----------------------------------------------------------------------------" << endl
         <<title.Data() << ": chi^2 / dof = " << chi_sq << " / " << (nentries - order - 1) << " = " << chi_sqs.back()
         << " (p = " << p_chi_sqs.back() << ")" << endl;
    if(!useMCBkg_) {
      auto vars = bkgPDF->getVariables();
      auto itr = vars->createIterator();
      auto var = itr->Next();
      while(var) {
        if(TString(var->GetName()) != lepm->GetName()) {
          var->Print();
        }
        var = itr->Next();
      }
    }
    cout << "----------------------------------------------------------------------------" << endl;
    // bkgPDF->Print("tree");

    if(!useMCBkg_ && useMultiDim_) {
      for(int ipdf = 0; ipdf < categories->numTypes(); ++ipdf) {
        if(ipdf == index) continue;
        auto pdf = multiPDF->getPdf(ipdf); //multiPDF->getPdf(Form("index_%i", ipdf));
        name = pdf->GetName();
        title = pdf->GetTitle();
        chi_sq = get_chi_squared(*lepm, pdf, (fitSideBands_) ? *blindDataHist : *dataData, fitSideBands_);
        pdf->plotOn(xframe, RooFit::Name(pdf->GetName()), RooFit::LineColor(colors[ipdf % colors.size()]), RooFit::LineStyle(kDashed),
                    RooFit::NormRange("full"),
                    RooFit::Range("full"));
        // pdf->Print("tree");
        order = ((title(title.Sizeof() - 2)) - '0');
        if     (title.Contains("Exponential")) order = 2*order - 1;
        else if(title.Contains("Power law"  )) order = 2*order - 1;
        else if(title.BeginsWith("Combined" )) order = 4; //2nd order poly + 0th order exp
        else                                   order = pdf->getVariables()->getSize()-1; //use N(params)
        chi_sqs.push_back(chi_sq / (nentries - order - 1));
        p_chi_sqs.push_back(TMath::Prob(chi_sq, nentries - order - 1));
        cout << "----------------------------------------------------------------------------" << endl
             <<title.Data() << ": chi^2 / dof = " << chi_sq << " / " << (nentries - order - 1) << " = " << chi_sqs.back()
             << " (p = " << p_chi_sqs.back() << ")" << endl;
        auto vars = pdf->getVariables();
        auto itr = vars->createIterator();
        auto var = itr->Next();
        while(var) {
          if(TString(var->GetName()) != lepm->GetName()) {
            var->Print();
          }
          var = itr->Next();
        }
        cout << "----------------------------------------------------------------------------" << endl;
      }
    }
    xframe->Draw();
    if(blind_data_) {
      blindData->Draw("same E1");
      leg->AddEntry(blindData, Form("Data, N(entries) = %.0f", data->Integral(low_bin, high_bin)), "PL");
    } else {
      leg->AddEntry(data, Form("Data, N(entries) = %.0f", data->Integral(low_bin, high_bin)), "PL");
    }
    const float nref_signal = sigVis->Integral(low_bin, high_bin);
    leg->AddEntry("sigPDF", Form("Signal, BR = %.1e, N(sig) = %.1f", br_sig*br_scale, nref_signal), "L");
    leg->AddEntry(bkgPDF->GetName(), Form("%s - #chi^{2}/DOF = %.2f, p = %.3f", bkgPDF->GetTitle(), chi_sqs[0], p_chi_sqs[0]), "L");
    if(zmumu_model_)
      leg->AddEntry(zmumu->GetName(), Form("%s", zmumu->GetTitle()), "L");
    if(!useMCBkg_ && useMultiDim_) {
      int offset = 1;
      for(int ipdf = 0; ipdf < categories->numTypes(); ++ipdf) {
        if(ipdf == index) {
          offset = 0;
          continue;
        }
        auto pdf = multiPDF->getPdf(ipdf); //multiPDF->getPdf(Form("index_%i", ipdf));
        leg->AddEntry(pdf->GetName(), Form("%s - #chi^{2}/DOF = %.2f, p = %.3f", pdf->GetTitle(), chi_sqs[ipdf+offset], p_chi_sqs[ipdf+offset]), "L");
      }
    }
    leg->Draw();
    /////////////////////////////////////////////
    //Add Data - Fit plot

    pad2->cd();

    //Create data - background fit histograms
    //update bin indices in case a clipped template is used
    low_bin = data->FindBin(xmin+1.e-3);
    high_bin = data->FindBin(xmax-1.e-3);
    const double norm = data->Integral(); //N(data) in fit region
    cout << "Data: xlow = " << data->GetBinLowEdge(low_bin)
         << " xhigh = " << data->GetBinLowEdge(high_bin) + data->GetBinWidth(high_bin)
         << " nbins = " << high_bin - low_bin + 1 << " integral = " << norm << endl;

    TH1* dataDiff = data_pdf_diff(blindDataHist, bkgPDF, *lepm, norm,
                                  (blind_data_) ? blind_min : blind_max + 1., blind_max);
    dataDiff->SetName("dataDiff");
    dataDiff->SetTitle("");
    // dataDiff->SetXTitle("M_{e#mu}");
    dataDiff->SetLineColor(data->GetLineColor());
    dataDiff->SetLineWidth(data->GetLineWidth());
    dataDiff->SetMarkerStyle(data->GetMarkerStyle());
    dataDiff->SetMarkerSize(data->GetMarkerSize());
    TH1* sigDiff = sigPDF->createHistogram("sigDiff", *lepm);
    vector<TH1*> pdfDiffs;
    for(int ipdf = 0; ipdf < categories->numTypes(); ++ipdf) {
      if(ipdf == index) {
        continue;
      }
      auto pdf = multiPDF->getPdf(ipdf);
      auto h = pdf_pdf_diff(pdf, bkgPDF, *lepm, norm);
      pdfDiffs.push_back(h);
    }
    sigDiff->Scale(nref_signal / sigDiff->Integral());

    lepm->setVal((xmin+xmax)/2.); //set to a normal value

    //Draw difference histogram
    if(replaceData_ >= 2) {dataDiff->SetMarkerStyle(20); dataDiff->SetMarkerSize(0.8); dataDiff->SetLineWidth(0); dataDiff->Draw("P");}
    else                  dataDiff->Draw("E1");
    dataDiff->GetXaxis()->SetRangeUser(xmin+1.e-3, xmax-1.e-3);
    dataDiff->GetXaxis()->SetLabelSize(0.08);
    dataDiff->GetXaxis()->SetTitle("");
    dataDiff->GetYaxis()->SetLabelSize(0.08);
    dataDiff->GetYaxis()->SetTitleSize(0.08);
    dataDiff->GetYaxis()->SetTitleOffset(0.5);
    dataDiff->GetYaxis()->SetTitle("Data - Fit");

    //add the signal
    sigDiff->SetLineColor(kRed);
    sigDiff->SetLineWidth(2);
    sigDiff->Draw("hist same");

    //add Z->mumu if being used
    if(zmumu_model_) {
      TH1* zmumu_hist = zmumu->createHistogram("zmumu_hist", *lepm);
      zmumu_hist->Scale(zmumu_yield / zmumu_hist->Integral());
      zmumu_hist->SetLineWidth(2);
      zmumu_hist->SetLineColor(kGreen);
      zmumu_hist->Draw("hist same");
    }

    //Add alternate PDFs
    for(int ipdf = 0; ipdf < categories->numTypes(); ++ipdf) {
      if(ipdf == index) {
        continue;
      }
      TH1* hPDFDiff = pdfDiffs[ipdf - (ipdf > index)];
      hPDFDiff->SetLineColor(colors[ipdf % colors.size()]);
      hPDFDiff->SetLineStyle(kDashed);
      hPDFDiff->Draw("same hist");
    }
    TLine* line = new TLine(xmin, 0., xmax, 0.);
    line->SetLineColor(kBlue);
    line->SetLineWidth(2);
    line->Draw("same");
    //print the results
    c->SaveAs(Form("plots/latest_production/%s/convert_bemu_%s_%i%s_%sbkg_pdfs.png", year_string.Data(), selection.Data(), set, tag_.Data(),
                   (useMCBkg_) ? "mc_" : (replaceData_ >= 2) ? "mcdata_" : "" ));
    delete xframe;
    delete c;
    if(replaceData_ < 2) delete blindData;
  }

  //////////////////////////////////////////////////////////////////
  // Write the results
  //////////////////////////////////////////////////////////////////

  bins += Form("lepm_%-5i", set);
  obs  += Form("%-10i", -1);

  //Add the signal first
  bins_p += Form("%10s", hist.Data());
  proc_l += Form("%10s", selection.Data());
  proc_c += Form("%10i", 0);
  double sig_err(0.);
  const double sig_rate = sig->IntegralAndError(sig->FindBin(xmin+1.e-3), sig->FindBin(xmax-1.e-3), sig_err);
  if(useRateParams_)
    rate   += Form("%10i", 1);
  else
    rate   += Form("%10.3f", sig_rate);
  sigPDF->SetName(selection.Data());

  ws->import(*sigPDF, RooFit::RecycleConflictNodes());
  signorm += Form("nsig_%-3i rateParam   lepm_%-3i %8s %10.1f\n", set, set, selection.Data(), sig->Integral(low_bin, high_bin));

  //add the background
  bins_p += Form("%15s", hist.Data());
  proc_l += Form("%15s", "bkg");
  proc_c += Form("%15i", ncat);
  if(useRateParams_)
    rate   += Form("%15i", 1);
  else {
    const float ndata = data->Integral(low_bin, high_bin);
    rate   += Form("%15.1f", (zmumu_model_) ? ndata - zmumu_yield : ndata);
  }
  ++ncat; //increment the number of processes

  if(zmumu_model_) {
    bins_p += Form("%15s", hist.Data());
    proc_l += Form("%15s", "zmumu");
    proc_c += Form("%15i", ncat);
    rate   += Form("%15.1f", zmumu_yield);
    bkg_norm += Form("zmumu_yield_%i lnN          -              -         1.10\n", set); //10% uncertainty on Z->mumu
    zmumu->SetName("zmumu");
    ws->import(*zmumu, RooFit::RecycleConflictNodes());
    ++ncat; //increment the number of processes
  }

  if(!useMCBkg_ && useMultiDim_) {
    ws->import(*multiPDF, RooFit::RecycleConflictNodes());
  } else {
    ws->import(*bkgPDF, RooFit::RecycleConflictNodes());
  }

  ws->import(*dataset);
  if(save_) ws->Write();
  cout << "Best fit bkgPDF is index " << index << " (" << bkgPDF->GetTitle() << ")\n";

  //add systematic information
  for(int index = 0; index < sys_names.size(); ++index) {
    TString sys = sys_names[index];
    TString line;
    if(systematics.find(sys) == systematics.end()) {
      line = Form("%-17s lnN", sys.Data());
    } else {
      line = systematics[sys];
    }
    if(twoSidedSys_) {
      const double yield_up   = sig_sys[index].first*sig_scale;
      const double yield_down = sig_sys[index].second*sig_scale;
      line += Form("%9.4f/%6.4f     -     ", yield_up/sig_rate, yield_down/sig_rate);
    } else { //use only the up of each systematic
      const double yield = sig_sys[index].first*sig_scale;
      line += Form("%9.4f            -     ", yield/sig_rate);
    }
    if(zmumu_model_) line += "          -   ";
    systematics[sys] = line;
  }
  //add signal MC statistics uncertainty
  {
    TString stat_line = Form("%-17s lnN", Form("signal_stats_%i", set));
    if(twoSidedSys_) {
      const double yield_up   = (sig_rate + sig_err);
      const double yield_down = (sig_rate - sig_err);
      stat_line += Form("%9.4f/%6.4f     -     ", yield_up/sig_rate, yield_down/sig_rate);
    } else { //use only the up of each systematic
      const double yield = (sig_rate + sig_err);
      stat_line += Form("%9.4f            -     ", yield/sig_rate);
    }
    if(zmumu_model_) stat_line += "          -   ";
    systematics[Form("signal_stats_%i", set)] = stat_line;
    sys_names.push_back(Form("signal_stats_%i", set));
  }
  //////////////////////////////////////////
  // Add Z->ll control regions
  //////////////////////////////////////////

  //FIXME: Only do this once, not per individual set
  if(use_same_flavor_) {
    outfile << Form("shapes * %-10s %s $CHANNEL/$PROCESS\n", "ee"  , outName.Data());
    outfile << Form("shapes * %-10s %s $CHANNEL/$PROCESS\n", "mumu", outName.Data());
    const int cr_set = 8;
    const double low_mass = 70.;
    const double high_mass = 110.;
    vector<pair<double,double>> ee_sys, ee_bkg_sys, mumu_sys, mumu_bkg_sys;

    TFile* fee = TFile::Open(Form("histograms/ee_lepm_%i_%s.hist", cr_set, year_string.Data()), "READ");
    if(!fee) {
      return 10;
    }
    TH1*     hdata  = (TH1*)     fee->Get("hdata");
    THStack* hstack = (THStack*) fee->Get("hstack");
    if(!hdata || !hstack) {
      cout << "Histograms for ee control region not found!\n";
      return 11;
    }
    TH1* hDY = 0;
    TH1* hBkg = 0;
    for(auto o : *(hstack->GetHists())) {
      TH1* h = (TH1*) o;
      if(TString(h->GetName()).Contains("Z->ee/#mu#mu")) {
        if(hDY) hDY->Add(h);
        else   {hDY = h; hDY->SetName("hDY");}
      } else {
        if(hBkg) hBkg->Add(h);
        else    {hBkg = h; hBkg->SetName("hBkg");}
      }
    }
    TH1* hZ    = new TH1D("zee" , "Z->ee count"           , 1, low_mass, high_mass);
    TH1* hZBkg = new TH1D("zbkg", "Z->ee count backround" , 1, low_mass, high_mass);
    TH1* hZObs = new TH1D("data_obs" , "Z->ee observation", 1, low_mass, high_mass);
    double error;
    hZ->SetBinContent(1, hDY->IntegralAndError(hDY->FindBin(low_mass+1.e-3), hDY->FindBin(high_mass-1.e-3), error));
    hZ->SetBinError(1, error);
    hZBkg->SetBinContent(1, hBkg->IntegralAndError(hBkg->FindBin(low_mass+1.e-3), hBkg->FindBin(high_mass-1.e-3), error));
    hZBkg->SetBinError(1, error);
    hZObs->SetBinContent(1, hdata->IntegralAndError(hdata->FindBin(low_mass+1.e-3), hdata->FindBin(high_mass-1.e-3), error));
    hZObs->SetBinError(1, error);
    double zrate = hZ->Integral();
    double zbkg  = hZBkg->Integral();
    bins        += Form(" %10s", "ee");
    obs         += Form(" %10.0f", hZObs->Integral());
    bins_p      += Form("%10s %10s", "ee", "ee");
    proc_l      += Form(" %10s %10s", "zee", "zbkg");
    proc_c      += Form(" %10i %10i", ncat, ncat + 1);
    rate        += Form(" %10.1f %10.1f", zrate, zbkg);
    ncat += 2;
    if(includeSys_) {
      get_systematics(fee, "zee"  , cr_set, ee_sys    , sys_names, low_mass+1.e-3, high_mass-1.e-3);
      get_systematics(fee, "eeBkg", cr_set, ee_bkg_sys, sys_names, low_mass+1.e-3, high_mass-1.e-3);
      //add systematic information
      for(int index = 0; index < sys_names.size(); ++index) {
        TString sys = sys_names[index];
        TString line = systematics[sys];
        double yield_ee = ee_sys[index].first;
        double yield_bkg = ee_bkg_sys[index].first;
        line += Form("%9.4f %9.4f", 1. + (yield_ee - zrate)/zrate, 1. + (yield_bkg - zbkg)/zbkg);
        systematics[sys] = line;
      }
    }
    if(save_) {
      auto dir = fOut->mkdir("ee");
      dir->cd();
      hZ->Write();
      hZBkg->Write();
      hZObs->Write();
    }
    delete hDY;
    delete hBkg;
    delete hZ;
    delete hZBkg;
    delete hZObs;
    delete hdata;
    delete hstack;
    fee->Close();

    TFile* fmumu = TFile::Open(Form("histograms/mumu_lepm_%i_%s.hist", cr_set, year_string.Data()), "READ");
    if(!fmumu) {
      return 20;
    }
    hdata  = (TH1*)     fmumu->Get("hdata");
    hstack = (THStack*) fmumu->Get("hstack");
    if(!hdata || !hstack) {
      cout << "Histograms for mumu control region not found!\n";
      return 21;
    }
    hDY = 0;
    hBkg = 0;
    for(auto o : *(hstack->GetHists())) {
      TH1* h = (TH1*) o;
      if(TString(h->GetName()).Contains("Z->ee/#mu#mu")) {
        if(hDY) hDY->Add(h);
        else   {hDY = h; hDY->SetName("hDY");}
      } else {
        if(hBkg) hBkg->Add(h);
        else    {hBkg = h; hBkg->SetName("hBkg");}
      }
    }
    hZ    = new TH1D("zmumu", "Z->mumu count"           , 1, low_mass, high_mass);
    hZBkg = new TH1D("zbkg" , "Z->mumu count backround" , 1, low_mass, high_mass);
    hZObs = new TH1D("data_obs", "Z->mumu observation"  , 1, low_mass, high_mass);
    hZ->SetBinContent(1, hDY->IntegralAndError(hDY->FindBin(low_mass+1.e-3), hDY->FindBin(high_mass-1.e-3), error));
    hZ->SetBinError(1, error);
    hZBkg->SetBinContent(1, hBkg->IntegralAndError(hBkg->FindBin(low_mass+1.e-3), hBkg->FindBin(high_mass-1.e-3), error));
    hZBkg->SetBinError(1, error);
    hZObs->SetBinContent(1, hdata->IntegralAndError(hdata->FindBin(low_mass+1.e-3), hdata->FindBin(high_mass-1.e-3), error));
    hZObs->SetBinError(1, error);
    zrate = hZ->Integral();
    zbkg  = hZBkg->Integral();
    bins        += Form(" %10s", "mumu");
    obs         += Form(" %10.0f", hZObs->Integral());
    bins_p      += Form(" %10s %10s", "mumu", "mumu");
    proc_l      += Form(" %10s %10s", "zmumu", "zbkg");
    proc_c      += Form(" %10i %10i", ncat, ncat + 1);
    rate        += Form(" %10.1f %10.1f", zrate, zbkg);
    ncat += 2;
    if(includeSys_) {
      get_systematics(fmumu, "zmumu"  , cr_set, mumu_sys    , sys_names, low_mass+1.e-3, high_mass-1.e-3);
      get_systematics(fmumu, "mumuBkg", cr_set, mumu_bkg_sys, sys_names, low_mass+1.e-3, high_mass-1.e-3);
      //add systematic information
      for(int index = 0; index < sys_names.size(); ++index) {
        TString sys = sys_names[index];
        TString line = systematics[sys];
        double yield_mumu = mumu_sys[index].first;
        double yield_bkg = mumu_bkg_sys[index].first;
        line += Form("%9.4f %9.4f", 1. + (yield_mumu - zrate)/zrate, 1. + (yield_bkg - zbkg)/zbkg);
        systematics[sys] = line;
      }
    }
    if(save_) {
      fOut->cd();
      auto dir = fOut->mkdir("mumu");
      dir->cd();
      hZ->Write();
      hZBkg->Write();
      hZObs->Write();
    }
    delete hDY;
    delete hBkg;
    delete hZ;
    delete hZBkg;
    delete hZObs;
    delete hdata;
    delete hstack;
    fmumu->Close();
  }
  if(save_) fOut->Close();

  //Print the contents of the card
  outfile << Form("----------------------------------------------------------------------------------------------------------- \n\n");
  outfile << Form("%s \n"  , bins.Data()  );
  outfile << Form("%s \n\n", obs.Data()   );
  outfile << Form("%s \n"  , bins_p.Data());
  outfile << Form("%s \n"  , proc_l.Data());
  outfile << Form("%s \n\n", proc_c.Data());
  outfile << Form("%s \n\n", rate.Data()  );
  outfile << Form("----------------------------------------------------------------------------------------------------------- \n\n");
  if(save_) outfile.close();

  //make a systematic free copy of the data card
  TString alt_card = filepath; alt_card.ReplaceAll(".txt", "_nosys.txt");
  if(save_) gSystem->Exec(Form("cp %s %s", filepath.Data(), alt_card.Data()));

  if(save_) outfile.open(filepath.Data(), std::ios_base::app); //open again, appending to the file

  for(int index = 0; index < systematics.size(); ++index) {
    TString sys = sys_names[index];
    TString line = systematics[sys];
    outfile << Form("%s \n", line.Data());
  }
  outfile << "\n----------------------------------------------------------------------------------------------------------- \n\n";
  //Add same flavor constraint rateParams
  if(use_same_flavor_) {
    outfile << Form("%-15s rateParam %-6s %-8s 1 [0.9,1.1]\n",  "zmumu_scale", "mumu", "zmumu");
    outfile << Form("%-15s rateParam %-6s %-8s 1 [0.9,1.1]\n",  "zee_scale"  , "ee"  , "zee"  );
    outfile << Form("%-15s rateParam %-6s %-8s sqrt(@0*@1) zmumu_scale,zee_scale\n", "zll_scale", "*", selection.Data());
    outfile << Form("# %-15s rateParam %-6s %-8s 1 [-10,20]\n",  "sig_over_zll"  , "*"  , selection.Data());
    outfile << Form("\n----------------------------------------------------------------------------------------------------------- \n\n");
    outfile << Form("ee   autoMCStats 0\n"); //default to including MC uncertainties
    outfile << Form("mumu autoMCStats 0\n"); //default to including MC uncertainties
    outfile << Form("\n----------------------------------------------------------------------------------------------------------- \n\n");
  }

  if(useRateParams_) {
    outfile << Form("%s \n\n", signorm.Data());
  }
  if(!useMCBkg_ && useMultiDim_) {
    outfile << Form("%s \n\n", cats.Data());
  }
  if(!useMCBkg_)
    outfile << Form("%s \n\n", bkg_norm.Data());

  //include constraints for the energy scale uncertainties
  if(addESShifts_) {
    outfile << Form("%10s param 0 1\n", muon_ES_shift->GetName());
    outfile << Form("%10s param 0 1\n", elec_ES_shift->GetName());
    outfile << "\n";
  }
  outfile.close();

  //for performing fits using local build of ROOT
  if(export_ && !TString(gSystem->Getenv("HOSTNAME")).Contains("cmslpc")) {
    TString outpath = "mmackenz@cmslpc140.fnal.gov:/uscms/home/mmackenz/nobackup/ZEMu/CMSSW_10_2_18/src/CLFVAnalysis/Roostats/imports/";
    outpath += Form("combine_%s_%s_%s.root", year_string.Data(), selection.Data(), set_string.Data());
    cout << "Exporting file " << fOut->GetName() << " to " << outpath.Data() << endl;
    gSystem->Exec(Form("scp %s %s", fOut->GetName(), outpath.Data()));
    outpath.ReplaceAll(".root", ".txt");
    cout << "Exporting file " << filepath.Data() << " to " << outpath.Data() << endl;
    gSystem->Exec(Form("scp %s %s", filepath.Data(), outpath.Data()));
  }
  return status;
}

Int_t convert_bemu_to_combine(vector<int> sets = {8}, TString selection = "zemu",
                              vector<int> years = {2016, 2017, 2018}, TString tag = "",
                              int seed = 90) {
  Int_t status(0);
  tag_ = tag;
  for(int set : sets) status += convert_individual_bemu_to_combine(set, selection, years, seed);
  return status;
}
