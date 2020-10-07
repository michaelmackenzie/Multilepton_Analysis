//Script to create histograms of mu+mu and e+e data

Int_t create_same_flavor_histograms(int set = 8, TString path = "../histograms/nanoaods_dev/", int year = 2016, TString hist = "lepm") {
  TString type = "event";
  //get mu+mu file
  TString muon_name = path + "ztautau_mumu_clfv_";
  muon_name += year;
  muon_name += "_SingleMu.hist";
  TFile* f_muon = TFile::Open(muon_name.Data(), "READ");
  if(!f_muon) return 1;
  //get di-lepton mass
  TH1F* h_muon = (TH1F*) f_muon->Get(Form("Data/%s_%i/%s", type.Data(), set+ZTauTauHistMaker::kMuMu, hist.Data()));
  if(!h_muon) return 2;

  TFile* muon_out = new TFile(Form("histograms/mumu_%s_%i_%i.hist", hist.Data(), set+ZTauTauHistMaker::kMuMu, year), "RECREATE");
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
  TH1F* h_electron = (TH1F*) f_electron->Get(Form("Data/%s_%i/%s", type.Data(), set+ZTauTauHistMaker::kEE, hist.Data()));
  if(!h_electron) return 4;

  TFile* electron_out = new TFile(Form("histograms/ee_%s_%i_%i.hist", hist.Data(), set+ZTauTauHistMaker::kEE, year), "RECREATE");
  h_electron->Write();
  electron_out->Write();
  delete electron_out;
  
  return 0;
}
