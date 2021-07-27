//Simple script to test RooFit with variable width histograms

void test_variable_width_bins(int Mode = 0) {
  double bins[] = {-10., -8., -6., -4., -3., -2.5, -2., -1.5, -1., -0.75, -0.5, -0.25, 0.,
                   0.25, 0.5, 0.75, 1., 1.5, 2., 2.5, 3., 4., 6., 8., 10.};
  int nbins = sizeof(bins)/sizeof(*bins) - 1;
  TH1D* hdata  = new TH1D("hdata" , "Data", nbins, bins);
  TH1D* hMC    = new TH1D("hMC"   , "MC"  , nbins, bins);
  TH1D* hdata2 = new TH1D("hdata2", "Data", 100, -10., 10.);
  TH1D* hMC2   = new TH1D("hMC2"  , "MC"  , 100, -10., 10.);
  TRandom3 rnd(90);
  for(int i = 0; i < 1e7; ++i) {
    hdata->Fill(rnd.Gaus());
    hMC->Fill(rnd.Gaus());
  }
  for(int i = 0; i < 1e6; ++i) {
    hdata2->Fill(rnd.Gaus());
    hMC2->Fill(rnd.Gaus());
  }

  TCanvas* c = new TCanvas("c_orig", "c_orig", 1200, 600);
  c->Divide(2,1);
  c->cd(1);
  hdata->SetMarkerStyle(20);
  hdata->SetMarkerSize(0.8);
  hdata->Draw("E");
  hMC->SetLineColor(kRed);
  hMC->SetLineWidth(2);
  hMC->Draw("same");
  c->cd(2);
  hdata2->SetMarkerStyle(20);
  hdata2->SetMarkerSize(0.8);
  hdata2->Draw("E");
  hMC2->SetLineColor(kRed);
  hMC2->SetLineWidth(2);
  hMC2->Draw("same");
  c->SaveAs("test_variable_width_bins_orig.png");

  /////////////////////////
  //Create RooHistPdf
  /////////////////////////
  c = new TCanvas("c_res", "c_res", 1000, 700);
  //Define observable variable
  RooRealVar obs("obs", "Observable", -10., 10.);
  RooBinning obs_bins(nbins, bins, "obs_bins");
  // obs.setBinning(obs_bins);
  //Create Roo objects for data/MC
  RooDataHist data ("data" , "Data", RooArgList(obs), hdata );
  RooDataHist mc   ("mc"   , "MC"  , RooArgList(obs), hMC   );
  RooDataHist data2("data2", "Data", RooArgList(obs), hdata2);
  RooDataHist mc2  ("mc2"  , "MC"  , RooArgList(obs), hMC2  );
  //Create PDF from MC
  RooHistPdf pdf  ("pdf" , "PDF" , RooArgList(obs), mc   );
  RooHistPdf pdf2 ("pdf2", "PDF" , RooArgList(obs), mc2  );
  //Create a category object, store results as a category
  RooCategory categories("categories", "categories");
  map<string, RooDataHist*> dataMap;
  categories.defineType("cat_0", 0);
  categories.defineType("cat_1", 1);
  dataMap["cat_0"] = &data;
  dataMap["cat_1"] = &data2;
  //Create a simultaneous PDF and dataset
  RooSimultaneous totPDF("totPDF", "Total PDF", categories);
  totPDF.addPdf(pdf , "cat_0");
  totPDF.addPdf(pdf2, "cat_1");
  RooDataHist combined_data("combined_data", "combined_data", obs, categories, dataMap);

  c->cd(2);
  auto xframe = obs.frame();
  if(Mode == 0) {
    data.plotOn(xframe);
    pdf.plotOn(xframe);
  } else if(Mode == 1) {
    RooAbsData::PlotOpt opt;
    opt.correctForBinWidth = true;
    data.plotOn(xframe, opt);
    pdf.plotOn(xframe);
  } else if(Mode == 2) {
    RooAbsData::PlotOpt opt;
    opt.correctForBinWidth = false;
    data.plotOn(xframe, opt);
    pdf.plotOn(xframe);
  } else if(Mode == 3) {
    combined_data.plotOn(xframe, RooFit::Cut("categories == 0"));
    totPDF.plotOn(xframe, RooFit::Slice(categories, "cat_0"), RooFit::ProjWData(combined_data));
  } else if(Mode == 4) {
    combined_data.plotOn(xframe, RooFit::Cut("categories == 1"));
    totPDF.plotOn(xframe, RooFit::Slice(categories, "cat_1"), RooFit::ProjWData(combined_data));
  }
  xframe->Draw();
  c->SaveAs(Form("test_variable_width_bins_%i.png", Mode));
}
