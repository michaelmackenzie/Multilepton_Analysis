//Script to prepare a rootfile and datacard for Higgs Combine tools
#include "mva_systematic_names.C"
#include "../interface/GlobalConstants.h"
#include "perform_f_test.C"
#include "construct_multidim.C"
using namespace CLFV;

bool blindData_ = true;
bool useRateParams_ = false;
bool fixSignalPDF_ = true;
bool useMultiDim_ = true;
bool useSameFlavor_ = true;
bool includeSys_ = true;
bool printPlots_ = true;
bool fitSideBands_ = true;

//Retrieve yields for each relevant systematic
void get_systematics(TFile* f, TString label, int set, vector<double>& yields, vector<TString>& names, double xmin = 1., double xmax = -1.) {
  //offset the set number to the absolute set value
  if     (label == "zee"   || label == "eeBkg"  ) set += ZTauTauHistMaker::kEE;
  else if(label == "zmumu" || label == "mumuBkg") set += ZTauTauHistMaker::kMuMu;
  else                                            set += ZTauTauHistMaker::kEMu;

  //only add systematic names to the list once
  bool addNames = names.size() == 0;
  TString prev_name = "";

  //for each relevant systematic, record the yield
  //only add the first occurence of each systematic for now
  int bin1(-1), bin2(-1);
  for(int isys = 1; isys < kMaxSystematics; ++isys) {
    auto sys_info = systematic_name(isys, "emu");
    TString sys_name = sys_info.first;
    if(sys_name == "" || sys_name.Contains("Tau") || sys_name == prev_name) {
      prev_name = "";
      continue;
    }
    prev_name = sys_name;
    TString hist_name;
    if(set > ZTauTauHistMaker::kMuMu) hist_name = Form("%s_sys_%i", (label.Contains("Bkg")) ? "hbkg" : "hDY", isys);
    else                              hist_name = Form("%s_lepm_%i_%i_sys_%i", label.Data(), isys, set, isys);
    TH1D* h = (TH1D*) f->Get(hist_name.Data());
    if(!h) {
      cout << "!!! Systematic " << isys << " not found for set " << set << " and label " << label.Data()
           << " hist name = " << hist_name.Data() << endl;
      continue;
    }
    double yield;
    if(xmin > xmax) {
      yield = h->Integral();
    } else {
      if(bin1 < 0 || bin2 < 0) {
        bin1 = max(1, h->FindBin(xmin));
        bin2 = min(h->GetNbinsX(), h->FindBin(xmax));
      }
      yield = h->Integral(bin1, bin2);
    }
    yields.push_back(yield);
    if(addNames) names.push_back(sys_name);
  }
}

//Main conversion function
Int_t convert_bemu_to_combine(vector<int> sets = {8}, TString selection = "zemu",
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
  TString set_string = "";
  for(unsigned i = 0; i < sets.size(); ++i) {
    if(i > 0) set_string += "_";
    set_string += sets[i];
  }

  bool isHiggs = selection.Contains("h");
  const double xmin = (isHiggs) ? 110. :  70.;
  const double xmax = (isHiggs) ? 160. : 110.;

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

  vector<TH1D*> sig_hists, bkg_hists, data_hists;
  map<int, vector<double>> sig_sys; //only consider systematics on signal and control region yields for now
  vector<TString> sys_names;
  for(int set : sets) {
    TFile* fInput = TFile::Open(Form("histograms/%s_lepm_%i_%s.hist",
                                   selection.Data(), set, year_string.Data()), "READ");
    if(!fInput) return 1;

    if(verbose_ > 1) fInput->ls();

    TH1D* bkg = (TH1D*) fInput->Get("hbackground");
    if(!bkg) {
      cout << "Background histogram for set " << set << " not found\n";
      return 2;
    }
    TH1D* sig = (TH1D*) fInput->Get(selection.Data());
    if(!sig) {
      cout << "Signal histogram for set " << set << " not found\n";
      return 3;
    }
    TH1D* data = (TH1D*) fInput->Get("hdata");
    if(!bkg) {
      cout << "Data histogram for set " << set << " not found\n";
      return 4;
    }

    bkg->SetDirectory(0);
    bkg->SetName(Form("bkg_%i", set));
    bkg_hists.push_back(bkg);
    sig->SetDirectory(0);
    sig->SetName(Form("%s_%i", selection.Data(), set));
    sig_hists.push_back(sig);
    data->SetDirectory(0);
    data->SetName(Form("data_obs_%i", set));
    data_hists.push_back(data);

    sig_sys[set] = {};
    if(includeSys_)
      get_systematics(fInput, selection, set, sig_sys[set], sys_names, xmin, xmax);
    fInput->Close();
  }

  //////////////////////////////////////////////////////////////////
  // Configure the output file
  //////////////////////////////////////////////////////////////////

  TString outName = Form("combine_bemu_%s_%s.root", selection.Data(), set_string.Data());
  TFile* fOut = new TFile(("datacards/"+year_string+"/"+outName).Data(), "RECREATE");

  //////////////////////////////////////////////////////////////////
  // Configure the data card
  //////////////////////////////////////////////////////////////////

  //Create directory for the data cards if needed
  gSystem->Exec(Form("[ ! -d datacards/%s ] && mkdir -p datacards/%s", year_string.Data(), year_string.Data()));
  TString filepath = Form("datacards/%s/combine_bemu_%s_%s.txt", year_string.Data(), selection.Data(), set_string.Data());
  gSystem->Exec(Form("echo \"# -*- mode: tcl -*-\">| %s", filepath.Data()));
  gSystem->Exec(Form("echo \"#Auto generated counting card for CLFVAnalysis \">> %s", filepath.Data()));
  gSystem->Exec(Form("echo \"#Signal branching fraction used: %.3e \n\">> %s", br_sig, filepath.Data()));
  gSystem->Exec(Form("echo \"imax %2i number of channels \">> %s", (int) sets.size() + 2*useSameFlavor_, filepath.Data()));
  gSystem->Exec(Form("echo \"jmax  * number of backgrounds \">> %s", filepath.Data()));
  gSystem->Exec(Form("echo \"kmax  * number of nuisance parameters \n\">> %s", filepath.Data()));
  gSystem->Exec(Form("echo \"----------------------------------------------------------------------------------------------------------- \">> %s", filepath.Data()));
  if(!useSameFlavor_) {
    gSystem->Exec(Form("echo \"shapes * * %s \\$CHANNEL:\\$PROCESS\">> %s", outName.Data(), filepath.Data()));
  }
  //Start each line, building for each background process
  TString bins   = "bin          "; //channel definition, 1 per channel
  TString bins_p = "bin       "    ; //per process per channel channel listing
  TString proc_l = "process  "; //process definition per channel
  TString proc_c = "process  "; //process class per channel
  TString rate   = "rate     "; //process rate per channel
  TString obs    = "observation      "; //data observations, 1 per channel
  TString signorm; //for relative signal rates at example branching fraction
  TString cats; //for discrete category indices
  map<TString, TString> systematics; //map of systematic name to systematic effect line

  //////////////////////////////////////////////////////////////////
  // Add datacard info for each set
  //////////////////////////////////////////////////////////////////

  for(unsigned iset = 0; iset < sets.size(); ++iset) {
    int set = sets[iset];
    TString hist = Form("lepm_%i", set);
    RooWorkspace* ws = new RooWorkspace(hist.Data());
    // auto dir = fOut->mkdir(hist.Data());
    // dir->cd();

    if(useSameFlavor_) {
      gSystem->Exec(Form("echo \"shapes * %-10s %s \\$CHANNEL:\\$PROCESS\">> %s", hist.Data(), outName.Data(), filepath.Data()));
    }
    //////////////////////////////////////////////////////////////////
    // Retrieve the histograms for this set
    //////////////////////////////////////////////////////////////////

    TH1D* data = data_hists[iset];
    TH1D* bkg  = bkg_hists [iset];
    TH1D* sig  = sig_hists [iset];
    TH1D* blindData = (TH1D*) data->Clone("hblind_data");
    const double blind_min = (isHiggs) ? 120. : 86.;
    const double blind_max = (isHiggs) ? 130. : 96.;

    for(int ibin = blindData->FindBin(blind_min); ibin <= blindData->FindBin(blind_max); ++ibin) {
      blindData->SetBinContent(ibin, 0.);
      blindData->SetBinError  (ibin, 0.);
    }

    // Create an observable for this category
    RooRealVar* lepm = new RooRealVar(Form("lepm_%i", set), "lepm", (xmin+xmax)/2., xmin, xmax);
    int low_bin = data->FindBin(xmin+1.e-3);
    int high_bin = data->FindBin(xmax-1.e-3);
    lepm->setBins(high_bin - low_bin + 1);
    lepm->setRange("FullRange", xmin, xmax);
    lepm->setRange("LowSideband", xmin, blind_min);
    lepm->setRange("HighSideband", blind_max, xmax);

    //create RooDataHist from histograms
    RooDataHist* bkgData  = new RooDataHist("bkg_data" ,  "Background Data", RooArgList(*lepm), bkg );
    RooDataHist* sigData  = new RooDataHist("sig_data" ,  "Signal Data"    , RooArgList(*lepm), sig );
    RooDataHist* dataData = new RooDataHist("data_data",  "Data Data"      , RooArgList(*lepm), data);
    RooDataHist* blindDataHist = new RooDataHist("blind_data_hist",  "Blind Data Hist", RooArgList(*lepm), blindData);

    //////////////////////////////////////////////////////////////////
    // Fit the signal distribution
    //////////////////////////////////////////////////////////////////

    cout << "--- Performing the signal fit for set " << set << endl;

    RooRealVar* mean     = new RooRealVar(Form("mean_%i"     , set), "mean", (isHiggs) ? 125. : 91., (isHiggs) ? 120. : 85., (isHiggs) ? 130. : 95.);
    RooRealVar* sigma    = new RooRealVar(Form("sigma_%i"    , set), "sigma", 2., 0.1, 5.);
    RooRealVar* alpha    = new RooRealVar(Form("alpha_%i"    , set), "alpha", 1., 0.1, 10.);
    RooRealVar* enne     = new RooRealVar(Form("enne_%i"     , set), "enne", 5., 0.1, 30.);
    RooRealVar* mean2    = new RooRealVar(Form("mean2_%i"    , set), "mean2", lepm->getVal(), lepm->getMin(), lepm->getMax());
    RooRealVar* sigma2   = new RooRealVar(Form("sigma2_%i"   , set), "sigma2", 5., 0.1, 10.);
    RooCBShape* sigpdf1  = new RooCBShape(Form("sigpdf1_%i"  , set), "sigpdf1", *lepm, *mean, *sigma, *alpha, *enne);
    RooGaussian* sigpdf2 = new RooGaussian(Form("sigpdf2_%i" , set), "sigpdf2", *lepm, *mean2, *sigma2);
    RooRealVar* fracsig  = new RooRealVar(Form("fracsig_%i"  , set), "fracsig", 0.7, 0., 1.);
    RooAddPdf* sigPDF    = new RooAddPdf(Form("sigPDF_%i"    , set), "signal PDF", *sigpdf1, *sigpdf2, *fracsig);
    RooRealVar* N_sig    = new RooRealVar(Form("N_sig_%i"    , set), "N_sig", 2e5, 1e2, 3e6);
    RooAddPdf* totsigpdf = new RooAddPdf(Form("totSigPDF_%i" , set), "Signal PDF", RooArgList(*sigPDF), RooArgList(*N_sig));

    totsigpdf->fitTo(*sigData, RooFit::SumW2Error(1), RooFit::Extended(1));
    if(fixSignalPDF_) {
      fracsig->setConstant(1); mean->setConstant(1); sigma->setConstant(1);
      enne->setConstant(1); alpha->setConstant(1); mean2->setConstant(1); sigma2->setConstant(1);
    }

    //////////////////////////////////////////////////////////////////
    // Fit the background distribution
    //////////////////////////////////////////////////////////////////

    RooCategory* categories = new RooCategory(Form("cat_%i", set), Form("cat_%i", set));
    int index = 0;
    auto multiPDF = construct_multidim_pdf((fitSideBands_) ? *blindDataHist : *dataData , *lepm, *categories, fitSideBands_, index, set, 2);
    if(categories->numTypes() < 1) {
      cout << "MultiPDF has no PDFs in set " << set << endl;
      return 5;
    }
    RooAbsPdf* bkgPDF = multiPDF->getPdf(Form("index_%i", index));
    categories->setIndex(index);
    cats += Form("%-8s discrete\n", categories->GetName());

    if(useMultiDim_) {
      multiPDF->SetName("bkg");
    } else {
      bkgPDF->SetName("bkg");
    }

    RooRealVar* N_bkg    = new RooRealVar("bkg_norm", "N(bkg)", 2e5, 1e2, 3e6);
    RooAddPdf* totbkgpdf = new RooAddPdf(Form("toBkgPDF_%i" , set), "Background PDF", RooArgList(*bkgPDF), RooArgList(*N_bkg));
    if(fitSideBands_)
      totbkgpdf->fitTo(*dataData, RooFit::SumW2Error(1), RooFit::Extended(1), RooFit::Range("LowSideband,HighSideband"));
    else
      totbkgpdf->fitTo(*dataData, RooFit::SumW2Error(1), RooFit::Extended(1));

    //Generate toy data to stand in for the observed data
    RooDataSet* dataset = bkgPDF->generate(RooArgSet(*lepm), data->Integral(low_bin, high_bin));
    dataset->SetName("data_obs");

    //Plot the results of the background fits
    if(printPlots_) {
      TCanvas* c = new TCanvas(Form("c_%i", set), Form("c_%i", set), 1000, 1000);
      TPad* pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1. ); pad1->Draw();
      TPad* pad2 = new TPad("pad2", "pad2", 0., 0. , 1., 0.3); pad2->Draw();
      pad1->cd();
      TLegend* leg = new TLegend(0.4, 0.7, 0.9, 0.9);
      const int nentries = 40;
      auto xframe = lepm->frame(nentries);
      xframe->SetTitle("");
      if(blindData_) {
        dataData->plotOn(xframe, RooFit::Invisible());
        // dataset->plotOn(xframe, RooFit::Name("toy_data"));
      }
      else           dataData->plotOn(xframe);
      bkgPDF->plotOn(xframe, RooFit::Name(bkgPDF->GetName()), RooFit::LineColor(kBlue), RooFit::Range("FullRange"));

      double chi_sq = xframe->chiSquare() * nentries;
      TString name = bkgPDF->GetName();
      TString title = bkgPDF->GetTitle();
      int order = ((title(title.Sizeof() - 2)) - '0');
      vector<int> colors = {kRed, kYellow, kViolet, kGreen, kOrange+2, kAtlantic};
      vector<double> chi_sqs;
      chi_sqs.push_back(chi_sq / (nentries - order - 2));

      if(useMultiDim_) {
        for(int ipdf = 0; ipdf < categories->numTypes(); ++ipdf) {
          if(ipdf == index) continue;
          auto pdf = multiPDF->getPdf(Form("index_%i", ipdf));
          name = pdf->GetName();
          title = pdf->GetTitle();
          pdf->plotOn(xframe, RooFit::Name(pdf->GetName()), RooFit::LineColor(colors[ipdf % colors.size()]), RooFit::LineStyle(kDashed), RooFit::Range("FullRange"));
          chi_sq = xframe->chiSquare() * nentries;
          order = ((title(title.Sizeof() - 2)) - '0');
          if(title.Contains("Exponential")) order *= 2;
          chi_sqs.push_back(chi_sq / (nentries - order - 2));
        }
      }
      xframe->Draw();
      if(blindData_) {
        blindData->Draw("same E1");
        leg->AddEntry(blindData, Form("Data, N(entries) = %.0f", data->Integral(low_bin, high_bin)), "PL");
      } else {
        leg->AddEntry("data", Form("Data, N(entries) = %.0f", data->Integral(low_bin, high_bin)), "PL");
      }
      leg->AddEntry(bkgPDF->GetName(), Form("%s - #chi^{2}/DOF = %.2f", bkgPDF->GetTitle(), chi_sqs[0]), "L");
      if(useMultiDim_) {
        int offset = 1;
        for(int ipdf = 0; ipdf < categories->numTypes(); ++ipdf) {
          if(ipdf == index) {
            offset = 0;
            continue;
          }
          auto pdf = multiPDF->getPdf(Form("index_%i", ipdf));
          leg->AddEntry(pdf->GetName(), Form("%s - #chi^{2}/DOF = %.2f", pdf->GetTitle(), chi_sqs[ipdf+offset]), "L");
        }
      }
      leg->Draw();

      //Add Data - Fit plot
      pad2->cd();
      //Create data - background fit histogram
      double norm = data->Integral(low_bin,high_bin);
      TH1D* dataDiff = (blindData_) ? (TH1D*) blindData->Clone("dataDiff") : (TH1D*) data->Clone("dataDiff");
      dataDiff->SetTitle("Data - Background Fit");
      //First make a histogram of the PDF, due to normalization issues
      for(int ibin = low_bin; ibin <= high_bin; ++ibin) {
        double x = dataDiff->GetBinCenter(ibin);
        lepm->setVal(x);
        dataDiff->SetBinContent(ibin, bkgPDF->getVal());
      }
      norm /= dataDiff->Integral(low_bin, high_bin);
      dataDiff->Scale(norm); //set the norms equal
      vector<TH1D*> pdfDiffs;
      for(int ipdf = 0; ipdf < categories->numTypes(); ++ipdf) {
        if(ipdf == index) {
          continue;
        }
        pdfDiffs.push_back((TH1D*) dataDiff->Clone(Form("hPdfDiff_%i", ipdf)));
        auto pdf = multiPDF->getPdf(Form("index_%i", ipdf));
      }
      // TH1D* hBkgPDF = (TH1D*) dataDiff->Clone("hBkgPDF");
      for(int ibin = low_bin; ibin <= high_bin; ++ibin) {
        double x = dataDiff->GetBinCenter(ibin);
        lepm->setVal(x);
        if((blindData_ && x > blind_min && x < blind_max) || (data->GetBinContent(ibin) < 1)) {
          dataDiff->SetBinContent(ibin, 0.);
        } else {
          dataDiff->SetBinContent(ibin, data->GetBinContent(ibin) - dataDiff->GetBinContent(ibin));
          dataDiff->SetBinError(ibin, data->GetBinError(ibin));
        }
        for(int ipdf = 0; ipdf < categories->numTypes(); ++ipdf) {
          if(ipdf == index) {
            continue;
          }
          TH1D* hPDFDiff = pdfDiffs[ipdf - (ipdf > index)];
          auto pdf = multiPDF->getPdf(Form("index_%i", ipdf));
          hPDFDiff->SetBinContent(ibin, norm*(pdf->getVal() - bkgPDF->getVal()));
          hPDFDiff->SetBinError(ibin,0.);
        }
      }
      lepm->setVal((xmin+xmax)/2.);

      //Draw difference histogram
      dataDiff->Draw("E1");
      dataDiff->GetXaxis()->SetRangeUser(xmin, xmax);
      TLine* line = new TLine(xmin, 0., xmax, 0.);
      line->SetLineColor(kBlue);
      line->SetLineWidth(2);
      line->Draw("same");
      // for(int ipdf = 0; ipdf < categories->numTypes(); ++ipdf) {
      //   if(ipdf == index) {
      //     continue;
      //   }
      //   TH1D* hPDFDiff = pdfDiffs[ipdf - (ipdf > index)];
      //   hPDFDiff->SetLineColor(colors[ipdf % colors.size()]);
      //   hPDFDiff->Draw("same hist");
      // }
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
    double sig_rate = sig->Integral(low_bin, high_bin)*sig_scale;
    if(useRateParams_)
      rate   += Form("%10i", 1);
    else
      rate   += Form("%10.1f", sig_rate);
    sigPDF->SetName(selection.Data());
    ws->import(*sigPDF, RooFit::RecycleConflictNodes());
    signorm += Form("nsig_%-3i rateParam   lepm_%-3i %8s %10.1f\n", set, set, selection.Data(), sig->Integral(low_bin, high_bin)*sig_scale);

    //add the background
    bins_p += Form("%10s", hist.Data());
    proc_l += Form("%10s", "bkg");
    proc_c += Form("%10i", 1);
    if(useRateParams_)
      rate   += Form("%10i", 1);
    else
      rate   += Form("%10.1f", data->Integral(low_bin, high_bin));
    if(useMultiDim_) {
      ws->import(*multiPDF, RooFit::RecycleConflictNodes());
      ws->import(*categories, RooFit::RecycleConflictNodes());
    } else {
      ws->import(*bkgPDF, RooFit::RecycleConflictNodes());
    }
    if(useRateParams_)
      ws->import(*N_bkg, RooFit::RecycleConflictNodes());

    ws->import(*dataset);
    ws->Write();
    cout << "Best fit bkgPDF is index " << index << " (" << bkgPDF->GetTitle() << ")\n";

    //add systematic information
    for(int index = 0; index < sys_names.size(); ++index) {
      TString sys = sys_names[index];
      TString line;
      if(systematics.find(sys) == systematics.end()) {
        line = Form("%-9s lnN", sys.Data());
      } else {
        line = systematics[sys];
      }
      double yield = sig_sys[set][index]*sig_scale;
      line += Form("%9.4f     -     ", 1. + (yield - sig_rate)/sig_rate);
      systematics[sys] = line;
    }
  }

  //////////////////////////////////////////
  // Add Z->ll control regions
  //////////////////////////////////////////

  if(useSameFlavor_) {
    gSystem->Exec(Form("echo \"shapes * %-10s %s \\$CHANNEL/\\$PROCESS\">> %s", "ee"  , outName.Data(), filepath.Data()));
    gSystem->Exec(Form("echo \"shapes * %-10s %s \\$CHANNEL/\\$PROCESS\">> %s", "mumu", outName.Data(), filepath.Data()));
    const int cr_set = 8;
    const double low_mass = 70.;
    const double high_mass = 110.;
    vector<double> ee_sys, ee_bkg_sys, mumu_sys, mumu_bkg_sys;

    TFile* fee = TFile::Open(Form("histograms/ee_lepm_%i_%s.hist", cr_set, year_string.Data()), "READ");
    if(!fee) {
      return 10;
    }
    TH1D*    hdata  = (TH1D*)    fee->Get("hdata");
    THStack* hstack = (THStack*) fee->Get("hstack");
    if(!hdata || !hstack) {
      cout << "Histograms for ee control region not found!\n";
      return 11;
    }
    TH1D* hDY = 0;
    TH1D* hBkg = 0;
    for(auto o : *(hstack->GetHists())) {
      TH1D* h = (TH1D*) o;
      if(TString(h->GetName()).Contains("Z->ee/#mu#mu")) {
        if(hDY) hDY->Add(h);
        else   {hDY = h; hDY->SetName("hDY");}
      } else {
        if(hBkg) hBkg->Add(h);
        else    {hBkg = h; hBkg->SetName("hBkg");}
      }
    }
    TH1D* hZ    = new TH1D("zee" , "Z->ee count"           , 1, low_mass, high_mass);
    TH1D* hZBkg = new TH1D("zbkg", "Z->ee count backround" , 1, low_mass, high_mass);
    TH1D* hZObs = new TH1D("data_obs" , "Z->ee observation", 1, low_mass, high_mass);
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
    proc_c      += Form(" %10i %10i", 1, 1);
    rate        += Form(" %10.1f %10.1f", zrate, zbkg);
    if(includeSys_) {
      get_systematics(fee, "zee"  , cr_set, ee_sys    , sys_names, low_mass+1.e-3, high_mass-1.e-3);
      get_systematics(fee, "eeBkg", cr_set, ee_bkg_sys, sys_names, low_mass+1.e-3, high_mass-1.e-3);
      //add systematic information
      for(int index = 0; index < sys_names.size(); ++index) {
        TString sys = sys_names[index];
        TString line = systematics[sys];
        double yield_ee = ee_sys[index];
        double yield_bkg = ee_bkg_sys[index];
        line += Form("%9.4f %9.4f", 1. + (yield_ee - zrate)/zrate, 1. + (yield_bkg - zbkg)/zbkg);
        systematics[sys] = line;
      }
    }
    auto dir = fOut->mkdir("ee");
    dir->cd();
    hZ->Write();
    hZBkg->Write();
    hZObs->Write();
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
    hdata  = (TH1D*)    fmumu->Get("hdata");
    hstack = (THStack*) fmumu->Get("hstack");
    if(!hdata || !hstack) {
      cout << "Histograms for mumu control region not found!\n";
      return 21;
    }
    hDY = 0;
    hBkg = 0;
    for(auto o : *(hstack->GetHists())) {
      TH1D* h = (TH1D*) o;
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
    proc_c      += Form(" %10i %10i", 1, 1);
    rate        += Form(" %10.1f %10.1f", zrate, zbkg);
    if(includeSys_) {
      get_systematics(fmumu, "zmumu"  , cr_set, mumu_sys    , sys_names, low_mass+1.e-3, high_mass-1.e-3);
      get_systematics(fmumu, "mumuBkg", cr_set, mumu_bkg_sys, sys_names, low_mass+1.e-3, high_mass-1.e-3);
      //add systematic information
      for(int index = 0; index < sys_names.size(); ++index) {
        TString sys = sys_names[index];
        TString line = systematics[sys];
        double yield_mumu = mumu_sys[index];
        double yield_bkg = mumu_bkg_sys[index];
        line += Form("%9.4f %9.4f", 1. + (yield_mumu - zrate)/zrate, 1. + (yield_bkg - zbkg)/zbkg);
        systematics[sys] = line;
      }
    }
    fOut->cd();
    dir = fOut->mkdir("mumu");
    dir->cd();
    hZ->Write();
    hZBkg->Write();
    hZObs->Write();
    delete hDY;
    delete hBkg;
    delete hZ;
    delete hZBkg;
    delete hZObs;
    delete hdata;
    delete hstack;
    fmumu->Close();
  }
  fOut->Close();

  //Print the contents of the card
  gSystem->Exec(Form("echo \"----------------------------------------------------------------------------------------------------------- \n\">> %s", filepath.Data()));
  gSystem->Exec(Form("echo \"%s \">> %s", bins.Data(), filepath.Data()));
  gSystem->Exec(Form("echo \"%s \n\">> %s", obs.Data() , filepath.Data()));
  gSystem->Exec(Form("echo \"%s \">> %s", bins_p.Data() , filepath.Data()));
  gSystem->Exec(Form("echo \"%s \">> %s", proc_l.Data() , filepath.Data()));
  gSystem->Exec(Form("echo \"%s \n\">> %s", proc_c.Data() , filepath.Data()));
  gSystem->Exec(Form("echo \"%s \n\">> %s", rate.Data() , filepath.Data()));
  gSystem->Exec(Form("echo \"----------------------------------------------------------------------------------------------------------- \n\">> %s", filepath.Data()));
  for(int index = 0; index < systematics.size(); ++index) {
    TString sys = sys_names[index];
    TString line = systematics[sys];
    gSystem->Exec(Form("echo \"%s \">> %s", line.Data() , filepath.Data()));
  }
  gSystem->Exec(Form("echo \"\n----------------------------------------------------------------------------------------------------------- \n\">> %s", filepath.Data()));
  //Add same flavor constraint rateParams
  if(useSameFlavor_) {
    gSystem->Exec(Form("echo \"%-15s rateParam %-6s %-8s 1 [0.9,1.1]\">> %s",  "zmumu_scale", "mumu", "zmumu", filepath.Data()));
    gSystem->Exec(Form("echo \"%-15s rateParam %-6s %-8s 1 [0.9,1.1]\">> %s",  "zee_scale"  , "ee"  , "zee"  , filepath.Data()));
    gSystem->Exec(Form("echo \"%-15s rateParam %-6s %-8s sqrt(@0*@1) zmumu_scale,zee_scale\">> %s", "zll_scale", "*", selection.Data(), filepath.Data()));
    gSystem->Exec(Form("echo \"# %-15s rateParam %-6s %-8s 1 [-10,20]\">> %s",  "sig_over_zll"  , "*"  , selection.Data()  , filepath.Data()));
    gSystem->Exec(Form("echo \"\n----------------------------------------------------------------------------------------------------------- \n\">> %s", filepath.Data()));
    gSystem->Exec(Form("echo \"ee   autoMCStats 0\">> %s", filepath.Data())); //default to including MC uncertainties
    gSystem->Exec(Form("echo \"mumu autoMCStats 0\">> %s", filepath.Data())); //default to including MC uncertainties
    gSystem->Exec(Form("echo \"\n----------------------------------------------------------------------------------------------------------- \n\">> %s", filepath.Data()));
  }

  if(useRateParams_) {
    gSystem->Exec(Form("echo \"%s \n\">> %s", signorm.Data(), filepath.Data()));
  }
  if(useMultiDim_) {
    gSystem->Exec(Form("echo \"%s \n\">> %s", cats.Data(), filepath.Data()));
  }

  return status;
}
