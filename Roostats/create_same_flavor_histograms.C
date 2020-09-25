//Script to create histograms of mu+mu and e+e data

Int_t create_same_flavor_histograms(int set = 8, TString path = "../histograms/nanoaods_dev/", int year = 2016) {
  //get mu+mu file
  TString muon_name = path + "ztautau_mumu_clfv_";
  muon_name += year;
  muon_name += "_SingleMu.hist";
  TFile* f_muon = TFile::Open(muon_name.Data(), "READ");
  if(!f_muon) return 1;
  //get di-lepton mass
  TH1F* h_muon = (TH1F*) f_muon->Get(Form("Data/event_%i/lepm", set+ZTauTauHistMaker::kMuMu));
  if(!h_muon) return 2;

  TFile* muon_out = new TFile(Form("mumu_lepm_%i_%i.hist", set+ZTauTauHistMaker::kMuMu, year), "RECREATE");
  h_muon->Write();
  muon_out->Write();
  delete muon_out;
  
  //get e+e file
  TString electron_name = path + "ztautau_ee_clfv_";
  electron_name += year;
  electron_name += "_SingleEle.hist";
  TFile* f_electron = TFile::Open(electron_name.Data(), "READ");
  if(!f_electron) return 3;
  //get di-lepton mass
  TH1F* h_electron = (TH1F*) f_electron->Get(Form("Data/event_%i/lepm", set+ZTauTauHistMaker::kEE));
  if(!h_electron) return 4;

  TFile* electron_out = new TFile(Form("ee_lepm_%i_%i.hist", set+ZTauTauHistMaker::kEE, year), "RECREATE");
  h_electron->Write();
  electron_out->Write();
  delete electron_out;
  
  return 0;
}
