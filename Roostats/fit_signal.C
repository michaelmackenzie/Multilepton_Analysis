//Script to morph ee and mumu data into e+mu
#include "DataInfo.C"

Int_t fit_signal(int set = 8, vector<int> years = {2016, 2017, 2018}, TString base = "../histograms/nanoaods_dev/") {

  TString hist = "lepm";
  TString year_string = "";
  for(unsigned i = 0; i < years.size(); ++i) {
    if(i > 0) year_string += "_";
    year_string += years[i];
  }

  /////////////////////
  // Get Signal Info //
  /////////////////////

  DataInfo signalInfo(set, "zemu");
  signalInfo.ReadData();
  TList* list = new TList;
  TFile* ftmp = new TFile("tmp.root", "RECREATE");
  TFile* f_zemus[years.size()];
  TTree* t_list[years.size()];
  for(unsigned i = 0; i < years.size(); ++i) {
    int year = years[i];
    TString zemu_name = base + "ztautau_emu_clfv_";
    zemu_name += year;
    zemu_name += "_ZEMu.hist";

    f_zemus[i] = TFile::Open(zemu_name.Data(), "READ");
    if(!f_zemus[i]) {
      cout << "Signal file " << zemu_name.Data() << " not found\n";
      return 3;
    }

    t_list[i] = (TTree*) f_zemus[i]->Get(Form("Data/tree_%i/tree_%i",
					     set+ZTauTauHistMaker::kEMu,
					     set+ZTauTauHistMaker::kEMu));
    if(!t_list[i]) {
      cout << "Signal tree in file " << zemu_name.Data() << " for set " << set << " not found\n";
      return 4;
    }
    ftmp->cd();
    t_list[i]->SetName(Form("tree_%i_%i", year, set+ZTauTauHistMaker::kEMu));
    t_list[i]->Write();
    list->Add(t_list[i]);
    //save weighted N(signal) count
    TH1F* h = new TH1F("h", "h", 1, -1e9, 1e9);
    t_list[i]->Draw("lepm>>h", "eventweight");
    signalInfo.datamap_[year] = h->Integral();\
    delete h;
  }
  signalInfo.WriteData(); //write signal counts
  TTree* t_zemu = TTree::MergeTrees(list);
  t_zemu->SetName("ZEMu_tree");
  
  std::cout << "All file elements retrieved!\n";
  RooRealVar lepm("lepm","di-lepton invariant mass", 75., 110.);
  RooRealVar weight("eventweight", "event weight", 1., -100., 100.);
  RooDataSet dataset("dataset", "dataset", RooArgSet(lepm,weight), RooFit::Import(*t_zemu), RooFit::WeightVar(weight));

  std::cout << "Dataset loaded!\n";

  //FIXME: change fit based on variable input
  RooRealVar mean("mean", "mean", 91., 85., 95.);
  RooRealVar width("width", "width", 2.5, 0.1, 5.);
  RooRealVar sigma("sigma", "sigma", 2., 0.1, 5.);
  RooRealVar alpha("alpha", "alpha", 1., 0.1, 10.);
  RooRealVar enne("enne", "enne", (set == 9) ? 17. : 5., 0.1, 30.);

  RooRealVar mean2("mean2", "mean2", 91., 80., 100.);
  RooRealVar sigma2("sigma2", "sigma2", 5., 0.1, 10.);

  RooCBShape sigpdf1("sigpdf1", "sigpdf1", lepm, mean, sigma, alpha, enne);
  RooGaussian sigpdf2("sigpdf2", "sigpdf2", lepm, mean2, sigma2);

  RooRealVar fracsig("fracsig", "fracsig", 0.7, 0., 1.);
  RooAddPdf sigpdf("sigpdf", "sigpdf", sigpdf1, sigpdf2, fracsig);

  RooRealVar N_sig("N_sig", "N_sig", 2e5, 1e2, 3e6);

  RooAddPdf totpdf("totpdf", "Signal PDF", RooArgList(sigpdf), RooArgList(N_sig));

  totpdf.fitTo(dataset);
  fracsig.setConstant(1); mean.setConstant(1); sigma.setConstant(1); width.setConstant(1);
  enne.setConstant(1); alpha.setConstant(1); mean2.setConstant(1); sigma2.setConstant(1); 
  
  auto xframe = lepm.frame();
  dataset.plotOn(xframe);
  totpdf.plotOn(xframe);
  totpdf.plotOn(xframe, RooFit::Components("sigpdf1"), RooFit::LineStyle(kDashed), RooFit::LineColor(kRed));
  totpdf.plotOn(xframe, RooFit::Components("sigpdf2"), RooFit::LineStyle(kDashed), RooFit::LineColor(kRed));

  auto c1 = new TCanvas();
  xframe->Draw();
  gSystem->Exec(Form("[ ! -d plots/latest_production/%s ] && mkdir -p plots/latest_production/%s", year_string.Data(), year_string.Data()));
  c1->SaveAs(Form("plots/latest_production/%s/fit_signal_%s_%i.pdf", year_string.Data(), hist.Data(), set));
  TFile* fOut = new TFile(Form("workspaces/fit_signal_%s_%s_%i.root", hist.Data(), year_string.Data(), set), "RECREATE");
  fOut->cd();
  RooWorkspace ws("ws");
  ws.import(sigpdf);
  ws.Write();
  fOut->Close();
  delete fOut;
  
  return 0;
}
