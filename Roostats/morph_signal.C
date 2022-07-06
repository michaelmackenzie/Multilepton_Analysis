//Script to morph ee and mumu data into e+mu
#include "DataInfo.C"

Int_t morph_signal(int set = 8, vector<int> years = {2016}, TString base = "../histograms/nanoaods_dev/") {

  TString year_string = "";
  for(unsigned i = 0; i < years.size(); ++i) {
    if(i > 0) year_string += "_";
    year_string += years[i];
  }

  ///////////////////
  // Get Muon Info //
  ///////////////////

  TString muon_name = "workspaces/fit_lepm_SameSign_Muon_";
  muon_name += year_string;
  muon_name += "_";
  muon_name += set;
  muon_name += ".root";
  TFile* f_muon = TFile::Open(muon_name.Data(), "READ");
  if(!f_muon) return 1;

  RooWorkspace* ws_muon = (RooWorkspace*) f_muon->Get("ws");
  if(!ws_muon) return -1;
  
  ///////////////////////
  // Get Electron Info //
  ///////////////////////

  TString electron_name = "workspaces/fit_lepm_SameSign_Electron_";
  electron_name += year_string;
  electron_name += "_";
  electron_name += set;
  electron_name += ".root";
  TFile* f_electron = TFile::Open(electron_name.Data(), "READ");
  if(!f_electron) return 2;

  RooWorkspace* ws_electron = (RooWorkspace*) f_electron->Get("ws");
  if(!ws_electron) return -2;

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
    TString zemu_name = base + "clfv_emu_clfv_";
    zemu_name += year;
    zemu_name += "_ZEMu.hist";

    f_zemus[i] = TFile::Open(zemu_name.Data(), "READ");
    if(!f_zemus[i]) {
      cout << "Signal file " << zemu_name.Data() << " not found\n";
      return 3;
    }

    t_list[i] = (TTree*) f_zemus[i]->Get(Form("Data/tree_%i/tree_%i",
					     set+HistMaker::kEMu,
					     set+HistMaker::kEMu));
    if(!t_list[i]) {
      cout << "Signal tree in file " << zemu_name.Data() << " for set " << set << " not found\n";
      return 4;
    }
    ftmp->cd();
    t_list[i]->SetName(Form("tree_%i_%i", year, set+HistMaker::kEMu));
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
  
  auto pdf_muon = ws_muon->pdf("sigpdf");
  auto pdf_electron = ws_electron->pdf("sigpdf");

  auto binned_muon = pdf_muon->generateBinned(RooArgSet(lepm), 1e8);
  auto binned_electron = pdf_electron->generateBinned(RooArgSet(lepm), 1e8);

  RooHistPdf pdf_muon_binned("pdf_muon_binned", "pdf_muon_binned", RooArgSet(lepm), *binned_muon);
  RooHistPdf pdf_electron_binned("pdf_electron_binned", "pdf_electron_binned", RooArgSet(lepm), *binned_electron);

  RooRealVar alpha("alpha", "alpha", 0.5); //how much of each distribution to use --> can fit to find best value
  RooIntegralMorph morph_pdf("morph_pdf", "morph_pdf", pdf_muon_binned, pdf_electron_binned, lepm, alpha);
  //
  
  auto binned_morphed = morph_pdf.generateBinned(RooArgSet(lepm), 1e8);
  RooHistPdf morph_pdf_binned("morph_pdf_binned", "morphed_pdf_binned", RooArgSet(lepm), *binned_morphed, 4);

  auto xframe = lepm.frame(80);
  dataset.plotOn(xframe);
  morph_pdf_binned.plotOn(xframe);
  pdf_muon_binned.plotOn(xframe, RooFit::LineStyle(kDashed), RooFit::LineColor(kRed));
  pdf_electron_binned.plotOn(xframe, RooFit::LineStyle(kDashed), RooFit::LineColor(kGreen));
  
  auto c1 = new TCanvas();
  xframe->Draw();
  TLegend* leg = new TLegend(0.6, 0.7, 0.9, 0.9);
  leg->AddEntry(xframe->findObject("pdf_muon_binned_Norm[lepm]"), "Muon PDF", "L");
  leg->AddEntry(xframe->findObject("pdf_electron_binned_Norm[lepm]"), "Electron PDF", "L");
  leg->AddEntry(xframe->findObject("morph_pdf_binned_Norm[lepm]"), "Signal PDF", "L");
  leg->AddEntry(xframe->findObject("h_dataset"), "Signal MC", "PL");
  leg->Draw("same");
  gSystem->Exec(Form("[ ! -d plots/latest_production/%s ] && mkdir -p plots/latest_production/%s", year_string.Data(), year_string.Data()));
  c1->SaveAs(Form("plots/latest_production/%s/compare_morphed_pdf_%i.png", year_string.Data(), set));
  TFile* fOut = new TFile(Form("workspaces/morphed_signal_%s_%i.root", year_string.Data(), set), "RECREATE");
  fOut->cd();
  RooWorkspace ws("ws");
  ws.import(morph_pdf_binned);
  ws.Write();
  fOut->Close();

  return 0;
}
