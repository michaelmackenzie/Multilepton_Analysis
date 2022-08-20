//Plot muon efficiencies to get an idea of the unfolding

int unfold_estimate(int year = 2018) {
  TFile* fID = nullptr;
  if     (year == 2016) fID = TFile::Open("../../scale_factors/RunBCDEF_Eff_data_ID_muon_2016.root", "READ");
  else if(year == 2017) fID = TFile::Open("../../scale_factors/RunBCDEF_Eff_data_ID_muon_2017.root", "READ");
  else if(year == 2018) fID = TFile::Open("../../scale_factors/RunABCD_Eff_data_ID_muon_2018.root" , "READ");
  else return -1;
  if(!fID) return 1;

  TH2* hID = nullptr;
  if     (year == 2016) hID = (TH2*) fID->Get("NUM_LooseID_DEN_genTracks_eta_pt");
  else if(year == 2017) hID = (TH2*) fID->Get("NUM_LooseID_DEN_TrackerMuons_pt_abseta");
  else if(year == 2018) hID = (TH2*) fID->Get("NUM_LooseID_DEN_TrackerMuons_pt_abseta");

  if(!hID) {
    cout << "ID histogram not found!\n";
    return 2;
  }

  TString dir = Form("figures/unfold_%i", year);
  gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s", dir.Data(), dir.Data()));

  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  gStyle->SetPaintTextFormat(".2f");
  TCanvas* c = new TCanvas();

  hID->Draw("colz text");
  c->SaveAs(Form("%s/ID.png", dir.Data()));

  fID->Close();
  delete c;
  return 0;
}
