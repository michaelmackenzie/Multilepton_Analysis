//Script to fit the MVA background distribution using pre-defined MVA histograms
#include "DataInfo.C"
#include "../interface/SystematicHist_t.hh"

bool doConstraints_ = true; //adding in systematics
bool includeSignalInFit_ = true; //fit background specturm with signal shape in PDF
bool addSignalToToyMC_ = false; // inject signal to the generated data distribution
TString selection_; //signal, e.g. "zmutau"

RooRealVar* br_sig_; //fields common to systematics and nominal PDFs
RooRealVar* mva_;
RooRealVar* bxs_var_;
RooRealVar* lum_var_;
double      xs_sig_;
RooGaussian* br_sig_constr_;
RooCategory* categories_;

int spline_order_ = 0; //for interpolating the histogram --> PDF

//create a PDF for each systematic variation
Int_t get_systematics(vector<vector<TH1D*>>& sig, vector<vector<TH1D*>>& bkg, RooWorkspace& ws) {
  int status(0);
  if(sig.size() == 0) return 1;
  unsigned nsys = sig[0].size();
  unsigned ncat = sig.size();
  for(unsigned isys = 0; isys < nsys; ++isys) {
    cout << "Getting PDFs for systematic " << isys << endl;
    vector<RooDataHist*> bkgMVADatas, sigMVADatas;
    vector<RooHistPdf*>  bkgMVAPDFs , sigMVAPDFs ;
    vector<RooAddPdf*>   totMVAPDFs;
    vector<RooRealVar*>  eff_nominals, n_bkgs;
    vector<RooFormulaVar*>  n_sigs;
    //create a PDF for each category
    bool skip = false;
    for(unsigned index = 0; index < ncat; ++index) {
      if(!bkg[index][isys] || !sig[index][isys]) {
        cout << "Systematic " << isys << " category " << index
             << " histograms not defined!" << endl;
        ++status;
        skip = true;
        break;
      }
      cout << " Initializing histogram datasets for category " << index << endl;
      bkgMVADatas.push_back(new RooDataHist(Form("bkgMVAData_%i_sys_%i", index, isys),
                                            "Background MVA Data",
                                            RooArgList(*mva_), bkg[index][isys]));
      sigMVADatas.push_back(new RooDataHist(Form("sigMVAData_%i_sys_%i", index, isys),
                                            "Signal MVA Data",
                                            RooArgList(*mva_), sig[index][isys]));
      bkgMVAPDFs .push_back(new RooHistPdf (Form("bkgMVAPDF_%i_sys_%i" , index, isys),
                                            "Background MVA PDF",
                                            RooArgSet(*mva_),
                                            *bkgMVADatas[index],
                                            spline_order_));
      sigMVAPDFs .push_back(new RooHistPdf (Form("sigMVAPDF_%i_sys_%i" , index, isys),
                                            "Signal MVA PDF",
                                            RooArgSet(*mva_),
                                            *sigMVADatas[index],
                                            spline_order_));
      cout << " Created individual PDFs for category " << index << endl;
      //initialize variables
      double eff_signal = sig[index][isys]->Integral()/(lum_var_->getVal()*xs_sig_);
      eff_nominals.push_back(new RooRealVar(Form("eff_nominal_%i_sys_%i", index, isys),
                                            "eff_nominal", eff_signal));
      n_sigs.push_back(new RooFormulaVar(Form("n_sig_%i_sys_%i", index, isys),
                                         "@0*@1*@2*@3",
                                         RooArgList(*br_sig_, *lum_var_, *bxs_var_,
                                                    *eff_nominals[index])
                                         )
                       );
      n_bkgs.push_back(new RooRealVar(Form("n_bkg_%i_sys_%i", index, isys),
                                      "n_bkg", bkg[index][isys]->Integral(), 0., 1.e8));

      //construct the total PDF for this category
      auto totpdf = new RooAddPdf(Form("totMVAPDF_%i_sys_%i", index, isys),
                                  Form("totMVAPDF_%i_sys_%i", index, isys),
                                  RooArgList(*sigMVAPDFs[index], *bkgMVAPDFs[index]),
                                  RooArgList(*n_sigs[index], *n_bkgs[index]));
      // auto totpdfsys = new RooProdPdf(Form("totMVAPDF_%i_sys_%i", index, isys),
      //                                 Form("totMVAPDF_%i_sys_%i", index, isys),
      //                                 RooArgList(*totpdf, *br_sig_constr_));
      totMVAPDFs.push_back(totpdf);
      cout << " Created total PDFs for category " << index << endl;
    } //end category loop

    if(skip) continue;
    cout << " Creating total PDF for systematic " << isys << endl;
    //create simultaneous pdf containing all categories
    RooSimultaneous totPDF(Form("totPDF_sys_%i", isys), "The Total PDF", *categories_);
    for(unsigned index = 0; index < totMVAPDFs.size(); ++index) {
        totPDF.addPdf(*totMVAPDFs[index], Form("%s_%i", selection_.Data(), index));
    }
    cout << " Created total PDF for systematic " << isys << endl;

    //import the result
    ws.import(totPDF);
  } //end systematic loop

  return status;
}

Int_t fit_background_MVA_binned(int set = 8, TString selection = "zmutau",
                                vector<int> years = {2016, 2017, 2018},
                                int seed = 90) {
  int status(0);
  selection_ = selection;
  double sys_unc = 0.; //systematic uncertainty on branching fraction
  double stat_unc = 0.3; //fractional statistical uncertainty on branching fraction
  vector<TString> hists;
  if     (selection == "hmutau"  ) {hists = {"mva0", "mva6"}; stat_unc = 2.8e-4/5.e-3; sys_unc = 0.20*stat_unc;}
  else if(selection == "zmutau"  ) {hists = {"mva1", "mva7"}; stat_unc = 1.3e-6/5.e-6; sys_unc = 0.87*stat_unc;}
  else if(selection == "hetau"   ) {hists = {"mva2", "mva8"}; stat_unc = 3.5e-4/5.e-3; sys_unc = 0.20*stat_unc;}
  else if(selection == "zetau"   ) {hists = {"mva3", "mva9"}; stat_unc = 1.5e-6/5.e-6; sys_unc = 1.40*stat_unc;}
  else if(selection == "hemu"    ) {hists = {"mva4"};         stat_unc = 1.5e-6/5.e-6; sys_unc = 0.20*stat_unc;}
  else if(selection == "zemu"    ) {hists = {"mva5"};         stat_unc = 1.5e-6/5.e-6; sys_unc = 0.20*stat_unc;}
  else {
    cout << "Unidentified selection " << selection.Data() << endl;
    return -1;
  }

  //create list of data selections
  TString selec = selection; selec.ReplaceAll("z", ""); selec.ReplaceAll("h", "");
  vector<TString> selecs;
  if(selec.Contains("tau")) selecs = {selection, selection+((selection.Contains("mu")) ? "_e" : "_mu")};
  else selecs = {selec};

  //determine the signal name
  TString signame = selection; signame.ReplaceAll("z", "Z"); signame.ReplaceAll("h", "H");
  signame.ReplaceAll("m", "M"); signame.ReplaceAll("e", "E"); signame.ReplaceAll("t", "T");

  TString year_string = "";
  for(unsigned i = 0; i < years.size(); ++i) {
    if(i > 0) year_string += "_";
    year_string += years[i];
  }

  vector<TH1D*> hmva_bkgs, hmva_sigs;
  vector<vector<TH1D*>> hsys_bkg, hsys_sig;
  for(unsigned index = 0; index < selecs.size(); ++index) {
    TString bkg_name = "histograms/" + selecs[index] + "_" + hists[index] + "_";
    bkg_name += set;
    bkg_name += "_" + year_string;
    bkg_name += ".hist";
    TFile* f_bkg = TFile::Open(bkg_name.Data(), "READ");
    if(!f_bkg) return 1;
    TH1D* hmva_bkg = (TH1D*) f_bkg->Get("hbackground");
    if(!hmva_bkg) return 2;
    TString selec_hname = selection;
    selec_hname.ReplaceAll("_e", "");
    selec_hname.ReplaceAll("_mu", "");
    TH1D* hmva_sig = (TH1D*) f_bkg->Get(selec_hname.Data());
    if(!hmva_sig) return 3;
    hmva_bkg->SetName(Form("hbkg_%i", index));
    hmva_sig->SetName(Form("hsig_%i", index));
    hmva_bkgs.push_back(hmva_bkg);
    hmva_sigs.push_back(hmva_sig);

    //Load systematic varied mva histograms
    hsys_bkg.push_back({}); hsys_sig.push_back({});
    for(int isys = 0; isys < kMaxSystematics; ++isys) {
      hsys_bkg[index].push_back((TH1D*) f_bkg->Get(Form("hbkg_sys_%i", isys)));
      hsys_sig[index].push_back((TH1D*) f_bkg->Get(Form("%s_sys_%i", selec_hname.Data(), isys)));
      if(hsys_bkg[index][isys]) hsys_bkg[index][isys]->SetName(Form("hbkg_%i_sys_%i", index, isys));
      else cout << "Systematic background " << isys << " not found!\n";
      if(hsys_sig[index][isys]) hsys_sig[index][isys]->SetName(Form("hsig_%i_sys_%i", index, isys));
      else cout << "Systematic signal " << isys << " not found!\n";
    }
    f_bkg->ls(); f_bkg->Print();
  }

  cout << "All file elements retrieved!\n";

  //create a histogram for the PDF
  double xmin(hmva_bkgs[0]->GetBinLowEdge(1)), xmax(hmva_bkgs[0]->GetBinLowEdge(hmva_bkgs[0]->GetNbinsX()) + hmva_bkgs[0]->GetBinWidth(hmva_bkgs[0]->GetNbinsX()));
  RooRealVar mva("mva","MVA score", xmin, xmax);
  mva.setBins(hmva_bkgs[0]->GetNbinsX());
  mva_ = &mva;
  vector<RooDataHist*> bkgMVADatas, sigMVADatas;
  vector<RooHistPdf*>  bkgMVAPDFs , sigMVAPDFs ;
  for(unsigned index = 0; index < hmva_bkgs.size(); ++index) {
    cout << "Starting index " << index << ": sig int = " << hmva_sigs[index]->Integral() << " and bkg int = "
         << hmva_bkgs[index]->Integral() << endl;
    bkgMVADatas.push_back(new RooDataHist(Form("bkgMVAData_%i", index), "Background MVA Data", RooArgList(mva), hmva_bkgs   [index]));
    sigMVADatas.push_back(new RooDataHist(Form("sigMVAData_%i", index), "Signal MVA Data"    , RooArgList(mva), hmva_sigs   [index]));
    bkgMVAPDFs .push_back(new RooHistPdf (Form("bkgMVAPDF_%i" , index), "Background MVA PDF" , RooArgSet (mva), *bkgMVADatas[index], spline_order_));
    sigMVAPDFs .push_back(new RooHistPdf (Form("sigMVAPDF_%i" , index), "Signal MVA PDF"     , RooArgSet (mva), *sigMVADatas[index], spline_order_));
    cout << "Finished index " << index << endl;
  }

  cout << "Individual PDFs constructed!\n";

  /////////////////////////////////
  // Define relevant parameters  //
  /////////////////////////////////

  //Luminosity based parameters
  CrossSections xs;
  double lum = 0.;
  for(int year : years) {
    double lum_year = xs.GetLuminosity(year);
    lum += lum_year;
  }

  //Get total boson cross section and set total luminosity
  double bxs = xs.GetCrossSection((selection.Contains("z")) ? "Z" : "H");

  //Parameters common to all categories
  RooRealVar br_sig("br_sig", "br_sig", 0., -0.01, 0.01); br_sig_ = &br_sig;
  RooRealVar lum_var("lum_var", "lum_var", lum); lum_var_ = &lum_var;
  RooRealVar bxs_var("bxs_var", "bxs_var", bxs); bxs_var_ = &bxs_var;
  RooRealVar br_sig_kappa("br_sig_kappa", "br_sig_kappa", 1. + sys_unc, 0., 2. + sys_unc); br_sig_kappa.setConstant(1);
  RooRealVar br_sig_beta("br_sig_beta", "br_sig_beta", 0., -10., 10.);
  RooFormulaVar br_sig_eff("br_sig_eff", "@0 * pow(@1, @2)", RooArgList(br_sig, br_sig_kappa, br_sig_beta));
  RooRealVar zero("zero", "zero", 0., -1., 1.); zero.setConstant(1);
  RooRealVar one("one", "one", 1., 0., 2.); one.setConstant(1);
  RooGaussian   br_sig_constr("br_sig_constr", "br_sig_constr", br_sig_beta, zero, one); //constraint on br_sig systematic
  if(!doConstraints_) br_sig_beta.setConstant(1);
  br_sig_constr_ = &br_sig_constr;
  xs_sig_ = xs.GetCrossSection(signame.Data());
  cout << "Global variables defined!\n";

  /////////////////////////////////
  //      Define Full PDFs       //
  /////////////////////////////////

  //Create each individual PDF
  vector<RooAbsPdf*> totMVAPDFs;
  RooCategory categories(selection.Data(), selection.Data()); //for creating a data hist
  categories_ = &categories;
  map<string, RooDataHist*> dataCategoryMap;
  vector<RooRealVar*> eff_nominals, n_bkgs;
  vector<RooFormulaVar*> n_sigs;
  for(unsigned index = 0; index < bkgMVAPDFs.size(); ++index) {
    double eff_signal = hmva_sigs[index]->Integral()/(lum*xs_sig_);
    eff_nominals.push_back(new RooRealVar(Form("eff_nominal_%i", index),
                                          "eff_nominal", eff_signal));
    n_sigs.push_back(new RooFormulaVar(Form("n_sig_%i", index), "@0*@1*@2*@3",
                                       RooArgList(br_sig_eff, lum_var, bxs_var,
                                                  *eff_nominals[index])));
    n_bkgs.push_back(new RooRealVar(Form("n_bkg_%i", index), "n_bkg", hmva_bkgs[index]->Integral(), 0., 1.e8));
    cout << "Index " << index << " Nominal signal efficiency = " << eff_signal << endl;
    //Signal + main background PDF
    RooAbsPdf* totpdf = new RooAddPdf(Form((doConstraints_) ? "totMVAPDF0_%i" : "totMVAPDF_%i", index),
                                Form((doConstraints_) ? "totMVAPDF0_%i" : "totMVAPDF_%i", index),
                                RooArgList(*sigMVAPDFs[index], *bkgMVAPDFs[index]),
                                RooArgList(*n_sigs[index], *n_bkgs[index]));
    RooAbsPdf* totpdfsys = new RooProdPdf(Form("totMVAPDF_%i", index), Form("totMVAPDF_%i", index),
                                    RooArgList(*totpdf, br_sig_constr));
    totMVAPDFs.push_back((doConstraints_) ? totpdfsys : totpdf);
    string category = Form("%s_%i", selection.Data(), index);
    categories.defineType(category.c_str(), index);
    //generate some toy bkg + sig
    cout << "Generating background data for index " << index << "!\n";
    RooDataHist* bkg_mva_gen = bkgMVAPDFs[index]->generateBinned(RooArgSet(mva), hmva_bkgs[index]->Integral());
    cout << "Generating signal data for index " << index << "!\n";
    if(addSignalToToyMC_) {
      RooDataHist* sig_mva_gen = sigMVAPDFs[index]->generateBinned(RooArgSet(mva), hmva_sigs[index]->Integral());
      bkg_mva_gen->add(*sig_mva_gen);
    }
    dataCategoryMap[category] = bkg_mva_gen;
  }

  cout << "Total PDFs defined!\n";

  //Create the total, simultaneous PDF
  RooSimultaneous totPDF("totPDF", "The Total PDF", categories);
  for(unsigned index = 0; index < totMVAPDFs.size(); ++index) {
    totPDF.addPdf(*totMVAPDFs[index], Form("%s_%i", selection.Data(), index));
  }

  cout << "Total combined PDF defined!\n";

  RooDataHist combined_data("combined_data", "combined_data", mva, categories, dataCategoryMap);

  cout << "Combined data defined!\n";

  if(!includeSignalInFit_) {br_sig.setVal(0.); br_sig.setConstant(1);}

  // totPDF.fitTo(combined_data, RooFit::Extended(1));

  cout << "Total PDF fit finished!\n";

  //Plot PDF
  br_sig.setVal(xs_sig_ / bxs);
  totPDF.Print();
  br_sig.Print();
  gSystem->Exec(Form("[ ! -d plots/latest_production/%s ] && mkdir -p plots/latest_production/%s", year_string.Data(), year_string.Data()));
  for(unsigned index = 0; index < totMVAPDFs.size(); ++index) {
    auto xframe = mva.frame(50);
    // combined_data.plotOn(xframe, RooFit::Cut(Form("%s==%i",selection.Data(),index)));
    double prev_val = bxs_var.getVal();
    double sig_scale = (signame.Contains("H")) ? 200. : 100.;
    bxs_var.setVal(sig_scale*prev_val); //scale signal boson cross section, since br_sig has limits

    totPDF.plotOn(xframe, RooFit::Components(Form("sigMVAPDF_%i", index)),
                  RooFit::Slice(categories, Form("%s_%i", selection.Data(),index)), RooFit::ProjWData(combined_data));
    totPDF.plotOn(xframe, RooFit::Components(Form("bkgMVAPDF_%i", index)), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed),
                  RooFit::Slice(categories, Form("%s_%i", selection.Data(),index)), RooFit::ProjWData(combined_data));
    auto c1 = new TCanvas();
    xframe->Draw();
    xframe->GetXaxis()->SetRangeUser(-0.8, 0.3);
    TLegend* leg = new TLegend(0.6, 0.7, 0.9, 0.9);
    leg->AddEntry((TH1*) (c1->GetPrimitive(Form("totMVAPDF_%i_Norm[mva]_Comp[sigMVAPDF_%i]", index, index))), Form("Signal (x%.0f)", sig_scale), "L");
    leg->AddEntry((TH1*) (c1->GetPrimitive(Form("totMVAPDF_%i_Norm[mva]_Comp[bkgMVAPDF_%i]", index, index))), "Background", "L");
    leg->Draw();
    c1->SaveAs(Form("plots/latest_production/%s/hist_background_mva_category_%i_%s_%i.png", year_string.Data(), index, selection.Data(), set));
    c1->SetLogy();
    c1->SaveAs(Form("plots/latest_production/%s/hist_background_mva_category_%i_%s_%i_log.png", year_string.Data(), index, selection.Data(), set));
    bxs_var.setVal(prev_val);
  }

  gSystem->Exec("[ ! -d workspaces ] && mkdir workspaces");
  TFile* fOut = new TFile(Form("workspaces/hist_background_mva_%s_%s_%i.root", selection.Data(), year_string.Data(), set), "RECREATE");
  fOut->cd();
  RooWorkspace ws("ws");
  ws.import(totPDF);
  ws.import(combined_data);
  get_systematics(hsys_sig, hsys_bkg, ws);
  ws.Write();
  fOut->Close();
  delete fOut;

  return status;
}
