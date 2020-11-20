//Script to create histograms of mu+mu and e+e data
#include "DataInfo.C"

Int_t create_same_flavor_histograms(int set = 8, vector<int> years = {2016}, TString path = "../histograms/nanoaods_dev/", TString hist = "lepm") {
  TString type = "event";

  DataInfo muonInfo(set, "muon");
  DataInfo electronInfo(set, "electron");
  muonInfo.ReadData();
  electronInfo.ReadData();

  ///////////////////
  // Get Muon Hist //
  ///////////////////
  
  TH1D* h_muon = 0;
  TString muon_out_name = Form("histograms/mumu_%s_%i", hist.Data(), set+ZTauTauHistMaker::kMuMu);
  for(int year : years) {
    //get mu+mu file
    TString muon_name = path + "ztautau_mumu_clfv_";
    muon_name += year;
    muon_name += "_SingleMu.hist";
    TFile* f_muon = TFile::Open(muon_name.Data(), "READ");
    if(!f_muon) {
      cout << "No muon file found for year " << year << endl;
      return 1;
    }
    //get di-lepton mass
    TH1D* h = (TH1D*) f_muon->Get(Form("Data/%s_%i/%s", type.Data(), set+ZTauTauHistMaker::kMuMu, hist.Data()));
    if(!h) {
      cout << "No muon histogram found for year " << year << " and set " << set << endl;
      return 2;
    }
    if(!h_muon) h_muon = h;
    else h_muon->Add(h);
    muon_out_name += Form("_%i", year);
    muonInfo.datamap_[year] = h->Integral();
  }
  muon_out_name += ".hist";
  cout << "Writing muon file " << muon_out_name.Data() << "...\n";
  TFile* muon_out = new TFile(muon_out_name.Data(), "RECREATE");
  // h_muon->SetName(Form("muon_%s", hist.Data()));
  h_muon->Write();
  muon_out->Write();
  delete muon_out;

  ///////////////////////
  // Get Electron Hist //
  ///////////////////////

  TH1D* h_electron = 0;
  TString electron_out_name = Form("histograms/ee_%s_%i", hist.Data(), set+ZTauTauHistMaker::kEE);
  for(int year : years) {
    //get mu+mu file
    TString electron_name = path + "ztautau_ee_clfv_";
    electron_name += year;
    electron_name += "_SingleEle.hist";
    TFile* f_electron = TFile::Open(electron_name.Data(), "READ");
    if(!f_electron) {
      cout << "No electron file found for year " << year << endl;
      return 1;
    }
    //get di-lepton mass
    TH1D* h = (TH1D*) f_electron->Get(Form("Data/%s_%i/%s", type.Data(), set+ZTauTauHistMaker::kEE, hist.Data()));
    if(!h) {
      cout << "No electron histogram found for year " << year << " and set " << set << endl;
      return 2;
    }
    if(!h_electron) h_electron = h;
    else h_electron->Add(h);
    electron_out_name += Form("_%i", year);
    electronInfo.datamap_[year] = h->Integral();
  }
  electron_out_name += ".hist";
  cout << "Writing electron file " << electron_out_name.Data() << "...\n";
  TFile* electron_out = new TFile(electron_out_name.Data(), "RECREATE");
  // h_electron->SetName(Form("electron_%s", hist.Data()));
  h_electron->Write();
  electron_out->Write();
  delete electron_out;

  //write out data info
  muonInfo.WriteData();
  electronInfo.WriteData();
  
  return 0;
}
