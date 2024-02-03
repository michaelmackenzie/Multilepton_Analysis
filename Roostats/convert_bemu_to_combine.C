//Script to prepare a rootfile and datacard for Higgs Combine tools
#include "mva_systematic_names.C"
#include "bemu_defaults.C"
#include "../interface/GlobalConstants.h"
#include "perform_f_test.C"
#include "construct_multidim.C"

#include <iostream>
#include <fstream>

using namespace CLFV;

bool useRateParams_ = false;
bool fixSignalPDF_  = true;
bool useMultiDim_   = true;
bool includeSys_    = true;
bool twoSidedSys_   = true; //write both up and down for each rate systematic
bool addESShifts_   = true; //include constrained parameters for the energy scale uncertainties
bool printPlots_    = true;
bool fitSideBands_  = true;
bool export_        = false; //if locally run, export the workspace to LPC
bool replaceRefit_  = false; //replace data with toy MC, then fit the unblinded toy data
bool save_          =  true; //save output combine workspace/cards


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

  if(includeSys_)
    get_systematics(fInput, selection, set, sig_sys, sys_names, xmin+1.e-3, xmax-1.e-3);
  fInput->Close();

  //////////////////////////////////////////////////////////////////
  // Configure the output file
  //////////////////////////////////////////////////////////////////

  TString outName = Form("combine_bemu_%s_%s.root", selection.Data(), set_string.Data());
  TFile* fOut = nullptr;
  if(save_) fOut = new TFile(("datacards/"+year_string+"/"+outName).Data(), "RECREATE");

  //////////////////////////////////////////////////////////////////
  // Configure the data card
  //////////////////////////////////////////////////////////////////

  //Create directory for the data cards if needed
  gSystem->Exec(Form("[ ! -d datacards/%s ] && mkdir -p datacards/%s", year_string.Data(), year_string.Data()));
  TString filepath = Form("datacards/%s/combine_bemu_%s_%s.txt", year_string.Data(), selection.Data(), set_string.Data());
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
  const double br_scale = 10.;
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
    RooRealVar* enne1    = new RooRealVar(Form("enne1_%i"    , set), "enne2", 5., 0.1, 30.);
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
    c->SaveAs(Form("plots/latest_production/%s/convert_bemu_%s_%i_sig_pdf.png", year_string.Data(), selection.Data(), set));
    delete xframe;
    delete c;
  }

  //////////////////////////////////////////////////////////////////
  // Fit the background distribution
  //////////////////////////////////////////////////////////////////

  RooCategory* categories = new RooCategory(Form("cat_%i", set), Form("cat_%i", set));
  int index = 0;
  // auto multiPDF = construct_simultaneous_pdf((fitSideBands_) ? *blindDataHist : *dataData , *lepm, *categories, fitSideBands_, index, set, 2);
  auto multiPDF = construct_multipdf((fitSideBands_) ? *blindDataHist : *dataData , *lepm, *categories, fitSideBands_, index, set, 2);
  std::cout << "Finished constructing the multi-PDF background model for set " << set << std::endl;
  if(categories->numTypes() < 1) {
    std::cout << "MultiPDF has no PDFs in set " << set << std::endl;
    return 5;
  }
  RooAbsPdf* bkgPDF = multiPDF->getPdf(index); //multiPDF->getPdf(Form("index_%i", index));
  categories->setIndex(index);
  cats += Form("%-8s discrete\n", categories->GetName());
  bkg_norm += Form("nbkg_%i rateParam lepm_%i bkg 1.\n", set, set);

  if(useMultiDim_) {
    multiPDF->SetName("bkg");
  } else {
    bkgPDF->SetName("bkg");
  }

  //Generate toy data to stand in for the observed data
  RooDataSet* dataset = bkgPDF->generate(RooArgSet(*lepm), data->Integral(low_bin, high_bin));
  dataset->SetName("data_obs");

  //Re-fit the PDFs to the toy MC data
  if(replaceRefit_) {
    cout << "####################################################################" << endl
         << "!!! Re-fitting to the toy MC data generated!\n"
         << "####################################################################" << endl;
    for(int ipdf = 0; ipdf < categories->numTypes(); ++ipdf) {
      auto pdf = multiPDF->getPdf(ipdf);
      pdf->fitTo(*dataset, RooFit::PrintLevel((verbose_ > 2) ? 1 : -1),
                 RooFit::Warnings(0), RooFit::PrintEvalErrors(-1));
    }
  }

  //Plot the results of the background fits
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
    if(blindData_) {
      dataData->plotOn(xframe, RooFit::Invisible());
      // dataset->plotOn(xframe, RooFit::Name("toy_data"));
    }
    else           dataData->plotOn(xframe);

    int nentries = dataData->numEntries();
    double chi_sq = get_chi_squared(*lepm, bkgPDF, *dataData, fitSideBands_, &nentries);
    bkgPDF->plotOn(xframe, RooFit::Name(bkgPDF->GetName()), RooFit::LineColor(kBlue), RooFit::NormRange("full"), RooFit::Range("full"));


    TString name = bkgPDF->GetName();
    TString title = bkgPDF->GetTitle();
    int order = ((title(title.Sizeof() - 2)) - '0');
    vector<int> colors = {kRed, kViolet-7, kGreen-7, kOrange+2, kAtlantic, kRed+2, kMagenta, kOrange, kYellow-7};
    vector<double> chi_sqs;
    vector<double> p_chi_sqs;
    chi_sqs.push_back(chi_sq / (nentries - order - 2));
    p_chi_sqs.push_back(TMath::Prob(chi_sq, nentries - order - 2));
    cout << "----------------------------------------------------------------------------" << endl
         <<title.Data() << ": chi^2 / dof = " << chi_sq << " / " << (nentries - order - 1) << " = " << chi_sqs.back()
         << " (p = " << p_chi_sqs.back() << ")" << endl;
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
    // bkgPDF->Print("tree");

    if(useMultiDim_) {
      for(int ipdf = 0; ipdf < categories->numTypes(); ++ipdf) {
        if(ipdf == index) continue;
        auto pdf = multiPDF->getPdf(ipdf); //multiPDF->getPdf(Form("index_%i", ipdf));
        name = pdf->GetName();
        title = pdf->GetTitle();
        chi_sq = get_chi_squared(*lepm, pdf, (fitSideBands_) ? *blindDataHist : *dataData, fitSideBands_);
        pdf->plotOn(xframe, RooFit::Name(pdf->GetName()), RooFit::LineColor(colors[ipdf % colors.size()]), RooFit::LineStyle(kDashed),
                    RooFit::NormRange("full"), RooFit::Range("full"));
        // pdf->Print("tree");
        order = ((title(title.Sizeof() - 2)) - '0');
        if     (title.Contains("Exponential")) order *= 2;
        else if(title.Contains("Power law"  )) order *= 2;
        else if(title.BeginsWith("Combined" )) order = 4; //2nd order poly + 0th order exp
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
    if(blindData_) {
      blindData->Draw("same E1");
      leg->AddEntry(blindData, Form("Data, N(entries) = %.0f", data->Integral(low_bin, high_bin)), "PL");
    } else {
      leg->AddEntry("data", Form("Data, N(entries) = %.0f", data->Integral(low_bin, high_bin)), "PL");
    }
    leg->AddEntry("sigPDF", Form("Signal, BR = %.1e, N(sig) = %.1f", br_sig*br_scale, sigVis->Integral(low_bin, high_bin)), "L");
    leg->AddEntry(bkgPDF->GetName(), Form("%s - #chi^{2}/DOF = %.2f, p = %.3f", bkgPDF->GetTitle(), chi_sqs[0], p_chi_sqs[0]), "L");
    if(useMultiDim_) {
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
    double norm = data->Integral(low_bin,high_bin); //N(data) in fit region
    cout << "Data: xlow = " << data->GetBinLowEdge(low_bin)
         << " xhigh = " << data->GetBinLowEdge(high_bin) + data->GetBinWidth(high_bin)
         << " nbins = " << high_bin - low_bin + 1 << " integral = " << norm << endl;

    TH1* dataDiff = bkgPDF->createHistogram("dataDiff", *lepm);
    // TH1* dataDiff = (blindData_) ? (TH1*) blindData->Clone("dataDiff") : (TH1*) data->Clone("dataDiff");
    dataDiff->SetTitle("");
    // dataDiff->SetXTitle("M_{e#mu}");
    dataDiff->SetLineColor(data->GetLineColor());
    dataDiff->SetLineWidth(data->GetLineWidth());
    dataDiff->SetMarkerStyle(data->GetMarkerStyle());
    dataDiff->SetMarkerSize(data->GetMarkerSize());
    cout << "Bkg histogram: xlow = " << dataDiff->GetBinLowEdge(1)
         << " xhigh = " << dataDiff->GetBinLowEdge(dataDiff->GetNbinsX()) + dataDiff->GetBinWidth(dataDiff->GetNbinsX())
         << " nbins = " << dataDiff->GetNbinsX() << " integral = " << dataDiff->Integral() << endl;
    TH1* sigDiff = sigPDF->createHistogram("sigDiff", *lepm);
    vector<TH1*> pdfDiffs;
    for(int ipdf = 0; ipdf < categories->numTypes(); ++ipdf) {
      if(ipdf == index) {
        continue;
      }
      // pdfDiffs.push_back((TH1*) dataDiff->Clone(Form("hPdfDiff_%i", ipdf)));
      auto pdf = multiPDF->getPdf(ipdf); //multiPDF->getPdf(Form("index_%i", ipdf));
      auto h = pdf->createHistogram(Form("hPdfDiff_%i", ipdf), *lepm);
      h->SetLineWidth(2);
      pdfDiffs.push_back(h);
      cout << "PDF: " << pdf->GetTitle() << " xlow = " << h->GetBinLowEdge(1)
           << " xhigh = " << h->GetBinLowEdge(h->GetNbinsX()) + h->GetBinWidth(h->GetNbinsX())
           << " nbins = " << h->GetNbinsX() << " integral = " << h->Integral() << endl;
      // pdfDiffs.push_back((TH1*) dataDiff->Clone(Form("hPdfDiff_%i", ipdf)));
    }
    // //First make a histogram of the PDF, due to normalization issues
    // for(int ibin = 1; ibin <= dataDiff->GetNbinsX(); ++ibin) {
    //   const double x = dataDiff->GetBinCenter(ibin);
    //   lepm->setVal(x);
    //   dataDiff->SetBinContent(ibin, bkgPDF->getVal());
    //   sigDiff ->SetBinContent(ibin, sigPDF->getVal());
    //   // for(int ipdf = 0; ipdf < categories->numTypes(); ++ipdf) {
    //   //   if(ipdf == index) {
    //   //     continue;
    //   //   }
    //   //   TH1* hPDFDiff = pdfDiffs[ipdf - (ipdf > index)];
    //   //   auto pdf = multiPDF->getPdf(ipdf); //multiPDF->getPdf(Form("index_%i", ipdf));
    //   //   hPDFDiff->SetBinContent(ibin, pdf->getVal());
    //   // }
    // }
    dataDiff->Scale(norm/dataDiff->Integral()); //set the norms equal
    sigDiff->Scale(sigVis->Integral(low_bin, high_bin) / sigDiff->Integral());
    for(int ipdf = 0; ipdf < categories->numTypes(); ++ipdf) {
      if(ipdf == index) {
        continue;
      }
      auto pdf = multiPDF->getPdf(ipdf); //multiPDF->getPdf(Form("index_%i", ipdf));
      TH1* hPDFDiff = pdfDiffs[ipdf - (ipdf > index)];
      hPDFDiff->Scale(norm/hPDFDiff->Integral());
    }

    //Set histograms to Value - Bkg PDF Value
    for(int ibin = 0; ibin <= dataDiff->GetNbinsX(); ++ibin) {
      const double x = dataDiff->GetBinCenter(ibin);
      lepm->setVal(x);
      const double val = dataDiff->GetBinContent(ibin); //Bkg PDF estimate
      const int data_bin = data->FindBin(x);
      const double data_val = data->GetBinContent(data_bin);
      if((blindData_ && x > blind_min && x < blind_max) || (data_val < 1)) {
        dataDiff->SetBinContent(ibin, 0.);
        dataDiff->SetBinError  (ibin, 0.);
      } else {
        dataDiff->SetBinContent(ibin, data_val - val);
        dataDiff->SetBinError(ibin, data->GetBinError(data_bin));
      }
      for(int ipdf = 0; ipdf < categories->numTypes(); ++ipdf) {
        if(ipdf == index) {
          continue;
        }
        TH1* hPDFDiff = pdfDiffs[ipdf - (ipdf > index)];
        // auto pdf = multiPDF->getPdf(ipdf); //multiPDF->getPdf(Form("index_%i", ipdf));
        hPDFDiff->SetBinContent(ibin, hPDFDiff->GetBinContent(ibin) - val);
        hPDFDiff->SetBinError(ibin,0.);
      }
    }
    lepm->setVal((xmin+xmax)/2.); //set to a normal value

    //Draw difference histogram
    dataDiff->Draw("E1");
    dataDiff->GetXaxis()->SetRangeUser(xmin+1.e-3, xmax-1.e-3);
    dataDiff->GetXaxis()->SetLabelSize(0.08);
    dataDiff->GetXaxis()->SetTitle("");
    dataDiff->GetYaxis()->SetLabelSize(0.08);
    dataDiff->GetYaxis()->SetTitleSize(0.08);
    dataDiff->GetYaxis()->SetTitleOffset(0.5);
    dataDiff->GetYaxis()->SetTitle("Data - Fit");
    // dataDiff->GetYaxis()->SetRangeUser(-900, 900);
    sigDiff->SetLineColor(kRed);
    sigDiff->SetLineWidth(2);
    sigDiff->Draw("hist same");
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
    c->SaveAs(Form("plots/latest_production/%s/convert_bemu_%s_%i_bkg_pdfs.png", year_string.Data(), selection.Data(), set));
    delete xframe;
    delete c;
    delete blindData;
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
  const double sig_rate = sig->IntegralAndError(low_bin, high_bin, sig_err);
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
  ++ncat;
  if(useRateParams_)
    rate   += Form("%15i", 1);
  else
    rate   += Form("%15.1f", data->Integral(low_bin, high_bin));
  if(useMultiDim_) {
    ws->import(*multiPDF, RooFit::RecycleConflictNodes());
    ws->import(*categories, RooFit::RecycleConflictNodes());
  } else {
    ws->import(*bkgPDF, RooFit::RecycleConflictNodes());
  }
  // if(useRateParams_)
  //   ws->import(*N_bkg, RooFit::RecycleConflictNodes());

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
  if(useMultiDim_) {
    outfile << Form("%s \n\n", cats.Data());
  }
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
                              vector<int> years = {2016, 2017, 2018},
                              int seed = 90) {
  Int_t status(0);
  for(int set : sets) status += convert_individual_bemu_to_combine(set, selection, years, seed);
  return status;
}
