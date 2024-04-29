//Test the Z->e+mu MC bias estimate using the MC histogram and the fit function
int  ndata_;
bool use_ws_signal_ = true;

void make_data_card(const char* fname, const bool use_multipdf = false) {

  //Create the data card
  std::ofstream outfile;
  outfile.open(Form("%s.txt", fname));
  outfile << "# -*- mode: tcl -*-\n";
  outfile << "# Example Higgs Combine datacard\n\n";
  outfile << "imax  1 number of channels\n";
  outfile << "jmax  1 number of backgrounds\n";
  outfile << "kmax  * number of nuisance parameters\n\n";
  outfile << "-----------------------------------------------------------------------------------------------------------" << endl
          << Form("shapes * * %s.root $CHANNEL:$PROCESS\n", fname)
          << "-----------------------------------------------------------------------------------------------------------" << endl << endl;

  outfile << Form("%-12s %10s\n", "bin", "mass");
  outfile << Form("%-12s %10i\n\n", "observation", -1);
  outfile << Form("%-12s %10s %10s\n", "bin", "mass", "mass");
  outfile << Form("%-12s %10s %10s\n", "process", "signal", "bkg");
  outfile << Form("%-12s %10i %10i\n\n", "process", 0, 1);
  outfile << Form("%-12s %10.3f %10.1f\n\n", "rate", 40., (double) ndata_);

  //add an example signal uncertainty of 5%
  outfile << "-----------------------------------------------------------------------------------------------------------" << endl
          << Form("%-8s lnN %10.2f %10s\n", "sigNorm", 1.05, "-")
          << "-----------------------------------------------------------------------------------------------------------" << endl << endl;

  if(use_ws_signal_) {
    outfile << "muon_ES_shift  param  0 1\n"
            << "elec_ES_shift  param  0 1\n";
  }

  if(use_multipdf) outfile << Form("%-12s %10s\n\n", "cat", "discrete");
  outfile << Form("nbkg rateParam %s bkg 1.\n", "mass"); //allow the background to float

}

void test_bemu_mc_bias(const int set = 13, const bool use_multipdf = true){

  ///////////////////////////////////////////
  // Retrieve the workspaces

  TFile* f_pdf = TFile::Open(Form("../datacards/2016_2017_2018/combine_bemu_zemu_%i.root"            , set), "READ");
  TFile* f_mc  = TFile::Open(Form("../datacards/2016_2017_2018/combine_bemu_zemu_mc_no_zmumu_%i.root", set), "READ");
  if(!f_pdf || !f_mc) return;

  RooWorkspace* w_pdf = (RooWorkspace*) f_pdf->Get(Form("lepm_%i", set));
  RooWorkspace* w_mc  = (RooWorkspace*) f_mc ->Get(Form("lepm_%i", set));
  if(!w_pdf || !w_mc) {
    cout << "!!! Workspaces not found!\n";
    return;
  }

  ///////////////////////////////////////////
  //Retrieve the variables and PDFs

  RooRealVar* obs_pdf = (RooRealVar*) w_pdf->var(Form("lepm_%i", set));
  RooRealVar* obs_mc  = (RooRealVar*) w_mc ->var(Form("lepm_%i", set));
  if(!obs_pdf || !obs_mc) {
    cout << "Observables not found!\n";
    return;
  }
  obs_pdf->Print();
  obs_mc ->Print();

  RooDataHist* data_pdf = (RooDataHist*) w_pdf->obj("data_obs");
  RooDataHist* data_mc  = (RooDataHist*) w_mc ->obj("data_obs");
  if(!data_pdf || !data_mc) {
    cout << "Data not found!\n";
    return;
  }
  ndata_ = data_pdf->sumEntries();

  RooAbsPdf* signal_pdf = nullptr;
  RooAbsPdf* signal_mc  = nullptr;
  if(use_ws_signal_) {
    signal_pdf = w_pdf->pdf("zemu"); signal_pdf->SetName("signal");
    signal_mc  = w_mc ->pdf("zemu"); signal_mc ->SetName("signal");
  } else {
    // Double sided Crystal Ball for the signal
    RooRealVar* mean   = new RooRealVar("mean"  , "mean"  , 9.06608e+01); mean  ->setConstant(true);
    RooRealVar* sigma  = new RooRealVar("sigma" , "sigma" , 2.34308e+00); sigma ->setConstant(true);
    RooRealVar* alpha1 = new RooRealVar("alpha1", "alpha1", 1.07075e+00); alpha1->setConstant(true);
    RooRealVar* alpha2 = new RooRealVar("alpha2", "alpha2", 1.37750e+00); alpha2->setConstant(true);
    RooRealVar* enne1  = new RooRealVar("enne1" , "enne2" , 3.56827e+00); enne1 ->setConstant(true);
    RooRealVar* enne2  = new RooRealVar("enne2" , "enne2" , 2.92393e+00); enne2 ->setConstant(true);
    signal_pdf = new RooDoubleCrystalBall("signal", "signal PDF", *obs_pdf, *mean, *sigma, *alpha1, *enne1, *alpha2, *enne2);
    signal_mc  = new RooDoubleCrystalBall("signal", "signal PDF", *obs_mc , *mean, *sigma, *alpha1, *enne1, *alpha2, *enne2);
  }

  RooAbsPdf* bkg_pdf = w_pdf->pdf(Form("gaus_poly_%i_pdf_order_2", set));
  RooAbsPdf* alt_pdf = w_pdf->pdf(Form("chb_%i_order_3", set));
  if(!bkg_pdf || !alt_pdf) {
    cout << "Background pdf not found!\n";
    return;
  }

  RooAbsPdf* mc_pdf = w_mc->pdf("bkg");
  if(!mc_pdf) {
    cout << "MC pdf not found!\n";
    return;
  }

  ///////////////////////////////////////////
  // Make a plot of the PDFs and toy data

  RooPlot *plot = obs_pdf->frame();
  data_pdf->plotOn(plot);
  bkg_pdf->plotOn(plot,RooFit::LineColor(kBlue),RooFit::LineStyle(kDashed));
  mc_pdf ->plotOn(plot,RooFit::LineColor(kRed ),RooFit::LineStyle(kDashed));
  plot->SetTitle("PDF fit to toy data");

  TCanvas c("c","c", 900, 1200);
  TPad pad1("pad1", "pad1", 0., 0.3, 1., 1.0); pad1.Draw();
  TPad pad2("pad2", "pad2", 0., 0.0, 1., 0.3); pad2.Draw();
  pad1.SetBottomMargin(0.06); pad2.SetTopMargin(0.03);
  pad1.cd();
  plot->Draw();

  pad2.cd();
  gStyle->SetOptStat(0);
  TH1* h_mc   = mc_pdf  ->createHistogram("hmc"  , *obs_mc );
  TH1* h_pdf  = bkg_pdf ->createHistogram("hpdf" , *obs_pdf);
  TH1* h_data = data_pdf->createHistogram("hdata", *obs_pdf);

  h_mc ->Scale(ndata_ / h_mc ->Integral());
  h_pdf->Scale(ndata_ / h_pdf->Integral());
  h_mc ->Scale(-1.); h_mc ->Add(h_data); //set to data - model
  h_pdf->Scale(-1.); h_pdf->Add(h_data);
  h_pdf->SetLineColor(kBlue); h_pdf->SetLineWidth(2);
  h_mc ->SetLineColor(kRed ); h_mc ->SetLineWidth(2); h_mc->SetLineStyle(kDashed);
  h_pdf->Draw("hist");
  h_mc ->Draw("hist same");
  h_pdf->GetYaxis()->SetRangeUser(-5., 5.);
  h_pdf->SetTitle("");
  h_pdf->SetYTitle("Data - Model");
  h_pdf->GetXaxis()->SetLabelSize(0.08);
  h_pdf->GetYaxis()->SetLabelSize(0.08);
  h_pdf->GetYaxis()->SetTitleSize(0.08);
  h_pdf->GetYaxis()->SetTitleOffset(0.35);
  pad2.SetGrid();

  gSystem->Exec("[ ! -d figures] && mkdir figures");
  c.SaveAs(Form("figures/test_bemu_mc_bias_%i.png", set));


  ///////////////////////////////////////////
  // Make Combine data cards for the tests

  // Make a workspace with the PDF
  {
    TFile *fout = new TFile("bemu_mc_bias_pdf.root","RECREATE");
    RooWorkspace wout("mass","mass");

    //If requested, wrap the PDF in a RooMultiPdf
    if(use_multipdf) {
      RooCategory categories("cat", "cat");
      RooMultiPdf envelope("bkg", "Background PDF envelope", categories, RooArgList(*bkg_pdf, *alt_pdf));
      // RooMultiPdf envelope("bkg", "Background PDF envelope", categories, RooArgList(*bkg_pdf));
      categories.setIndex(0);
      wout.import(envelope, RooFit::RecycleConflictNodes());
    } else {
      bkg_pdf->SetName("bkg");
      wout.import(*bkg_pdf, RooFit::RecycleConflictNodes());
    }
    data_pdf->SetName("data_obs");
    wout.import(*data_pdf, RooFit::RecycleConflictNodes());
    wout.import(*signal_pdf, RooFit::RecycleConflictNodes());
    wout.Print();
    wout.Write();
    fout->Close();
    make_data_card("bemu_mc_bias_pdf", use_multipdf);
  }

  // Make a workspace with the histogram
  {
    TFile *fout = new TFile("bemu_mc_bias_hist.root","RECREATE");
    RooWorkspace wout("mass","mass");

    mc_pdf->SetName("bkg");
    data_mc->SetName("data_obs");
    wout.import(*data_mc, RooFit::RecycleConflictNodes());
    wout.import(*mc_pdf, RooFit::RecycleConflictNodes());
    wout.import(*signal_mc, RooFit::RecycleConflictNodes());
    wout.Print();
    wout.Write();
    fout->Close();
    make_data_card("bemu_mc_bias_hist");
  }

  f_pdf->Close();
  f_mc ->Close();
}
