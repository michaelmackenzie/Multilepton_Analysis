#include "interface/TauIDWeight.hh"

using namespace CLFV;

//-------------------------------------------------------------------------------------------------------------
TauIDWeight::TauIDWeight(bool isEmbed, int verbose) : isEmbed_(isEmbed), verbose_(verbose) {
  std::vector<TString> file_regions;

  typedef std::pair<TString,TString> fpair;

  TString tauScaleFactorPath = gSystem->Getenv("CMSSW_BASE");
  tauScaleFactorPath += (tauScaleFactorPath != "") ? "/src/TauPOG/TauIDSFs/data/" : "../scale_factors/";

  /////////////////////////////////
  // Tau anti-jet ID
  /////////////////////////////////

  std::map<int, TString> tauJetIDFileNames;
  tauJetIDFileNames[k2016]    = Form("TauID_SF_pt_DeepTau2017v2p1VSjet_2016Legacy%s.root", isEmbed ? "_EMB" : "");
  tauJetIDFileNames[k2017]    = Form("TauID_SF_pt_DeepTau2017v2p1VSjet_2017ReReco%s.root", isEmbed ? "_EMB" : "");
  tauJetIDFileNames[k2018]    = Form("TauID_SF_pt_DeepTau2017v2p1VSjet_2018ReReco%s.root", isEmbed ? "_EMB" : "");
  for(int period = k2016; period <= k2018; ++period) {
    if(tauJetIDFileNames[period] == "") continue;
    TFile* f = TFile::Open((tauScaleFactorPath + tauJetIDFileNames[period]).Data(),"READ");
    if(!f) continue;
    tauJetIDMap    [period][kTight ] = (TF1*) f->Get("Tight_cent" );
    tauJetUpIDMap  [period][kTight ] = (TF1*) f->Get("Tight_up"   );
    tauJetDownIDMap[period][kTight ] = (TF1*) f->Get("Tight_down" );
    tauJetIDMap    [period][kVLoose] = (TF1*) f->Get("VLoose_cent");
    tauJetUpIDMap  [period][kVLoose] = (TF1*) f->Get("VLoose_up"  );
    tauJetDownIDMap[period][kVLoose] = (TF1*) f->Get("VLoose_down");
    if(!tauJetIDMap[period][kTight] ) std::cout << "Error! Tight tauJetID TF1 not found for year "  << period + 2016 << std::endl;
    if(!tauJetIDMap[period][kVLoose]) std::cout << "Error! VLoose tauJetID TF1 not found for year " << period + 2016 << std::endl;
    files_.push_back(f);
  }

  /////////////////////////////////
  // Tau anti-ele ID
  /////////////////////////////////

  std::map<int, fpair> tauEleIDFileNames;
  tauEleIDFileNames[k2016]    = fpair("TauID_SF_eta_DeepTau2017v2p1VSe_2016Legacy.root","Tight"); //use etau selection ID
  tauEleIDFileNames[k2017]    = fpair("TauID_SF_eta_DeepTau2017v2p1VSe_2017ReReco.root","Tight");
  tauEleIDFileNames[k2018]    = fpair("TauID_SF_eta_DeepTau2017v2p1VSe_2018ReReco.root","Tight");
  for(int period = k2016; period <= k2018; ++period) {
    if(tauEleIDFileNames[period].first == "") continue;
    TFile* f = TFile::Open((tauScaleFactorPath + tauEleIDFileNames[period].first).Data(),"READ");
    if(!f) continue;
    tauEleIDMap[period] = (TH1*) f->Get((tauEleIDFileNames[period].second).Data())->Clone();
    files_.push_back(f);
  }
  /////////////////////////////////
  // Tau anti-mu ID
  /////////////////////////////////

  std::map<int, fpair> tauMuIDFileNames;
  tauMuIDFileNames[k2016]    = fpair("TauID_SF_eta_DeepTau2017v2p1VSmu_2016Legacy.root","Tight");
  tauMuIDFileNames[k2017]    = fpair("TauID_SF_eta_DeepTau2017v2p1VSmu_2017ReReco.root","Tight");
  tauMuIDFileNames[k2018]    = fpair("TauID_SF_eta_DeepTau2017v2p1VSmu_2018ReReco.root","Tight");
  for(int period = k2016; period <= k2018; ++period) {
    if(tauMuIDFileNames[period].first == "") continue;
    TFile* f = TFile::Open((tauScaleFactorPath + tauMuIDFileNames[period].first).Data(),"READ");
    if(!f) continue;
    tauMuIDMap[period] = (TH1*) f->Get((tauMuIDFileNames[period].second).Data())->Clone();
    files_.push_back(f);
  }

  /////////////////////////////////
  // Tau energy scale
  /////////////////////////////////

  std::map<int, fpair> tauESLowFileNames;
  tauESLowFileNames[k2016]    = fpair("TauES_dm_DeepTau2017v2p1VSjet_2016Legacy.root", "tes");
  tauESLowFileNames[k2017]    = fpair("TauES_dm_DeepTau2017v2p1VSjet_2017ReReco.root", "tes");
  tauESLowFileNames[k2018]    = fpair("TauES_dm_DeepTau2017v2p1VSjet_2018ReReco.root", "tes");
  for(int period = k2016; period <= k2018; ++period) {
    if(tauESLowFileNames[period].first == "") continue;
    TFile* f = TFile::Open((tauScaleFactorPath + tauESLowFileNames[period].first).Data(),"READ");
    if(!f) continue;
    tauESLowMap[period] = (TH1*) f->Get((tauESLowFileNames[period].second).Data())->Clone();
    files_.push_back(f);
  }

  /////////////////////////////////
  // Tau energy scale (high pt)
  /////////////////////////////////

  std::map<int, fpair> tauESHighFileNames;
  tauESHighFileNames[k2016]    = fpair("TauES_dm_DeepTau2017v2p1VSjet_2016Legacy_ptgt100.root", "tes");
  tauESHighFileNames[k2017]    = fpair("TauES_dm_DeepTau2017v2p1VSjet_2017ReReco_ptgt100.root", "tes");
  tauESHighFileNames[k2018]    = fpair("TauES_dm_DeepTau2017v2p1VSjet_2018ReReco_ptgt100.root", "tes");
  for(int period = k2016; period <= k2018; ++period) {
    if(tauESHighFileNames[period].first == "") continue;
    TFile* f = TFile::Open((tauScaleFactorPath + tauESHighFileNames[period].first).Data(),"READ");
    if(!f) continue;
    tauESHighMap[period] = (TH1*) f->Get((tauESHighFileNames[period].second).Data())->Clone();
    files_.push_back(f);
  }

  /////////////////////////////////
  // Tau fake energy scale
  /////////////////////////////////

  std::map<int, fpair> tauFakeESFileNames;
  tauFakeESFileNames[k2016]    = fpair("TauFES_eta-dm_DeepTau2017v2p1VSe_2016Legacy.root", "fes");
  tauFakeESFileNames[k2017]    = fpair("TauFES_eta-dm_DeepTau2017v2p1VSe_2017ReReco.root", "fes");
  tauFakeESFileNames[k2018]    = fpair("TauFES_eta-dm_DeepTau2017v2p1VSe_2018ReReco.root", "fes");
  for(int period = k2016; period <= k2018; ++period) {
    if(tauFakeESFileNames[period].first == "") continue;
    TFile* f = TFile::Open((tauScaleFactorPath + tauFakeESFileNames[period].first).Data(),"READ");
    if(!f) continue;
    tauFakeESMap[period] = (TGraphAsymmErrors*) f->Get((tauFakeESFileNames[period].second).Data())->Clone();
    files_.push_back(f);
  }
}

//-------------------------------------------------------------------------------------------------------------
TauIDWeight::~TauIDWeight() {
  for(unsigned i = 0; i < files_.size(); ++i) {files_[i]->Close(); delete files_[i];}
}


//-------------------------------------------------------------------------------------------------------------
//correction for the tau ID
double TauIDWeight::IDWeight(double pt, double eta, int genID, UChar_t antiJet, int year,
                             float& up, float& down
                             ) {
  if(year > 2000) year -= 2016;
  if(year != k2016 && year != k2017 && year != k2018) {
    std::cout << "Warning! Undefined year in TauIDWeight::" << __func__ << ", returning -1" << std::endl;
    return -1.;
  }
  pt = std::max(pt, 20.001); //has to be slightly greater than 20
  double scale_factor = 1.;
  up = 1.; down = 1.;
  const int antiJetBit = BitFromID(antiJet);
  const int antiJetIndex = (antiJetBit >= kTight) ? kTight : kVLoose; //tight or loose tau
  if(genID == 5) { //genuine tau
    scale_factor  = tauJetIDMap[year][antiJetIndex]->Eval(pt);
    up            = tauJetUpIDMap[year][antiJetIndex]->Eval(pt);
    down          = tauJetDownIDMap[year][antiJetIndex]->Eval(pt);
  } else if(genID == 1) { //genuine electron -> tau
    const int bin = std::max(1, std::min(tauEleIDMap[year]->GetNbinsX(), tauEleIDMap[year]->FindBin(std::fabs(eta))));
    scale_factor  = tauEleIDMap[year]->GetBinContent(bin);
    up            = scale_factor + tauEleIDMap[year]->GetBinError(bin);
    down          = scale_factor - tauEleIDMap[year]->GetBinError(bin);
  } else if(genID == 2) { //genuine muon -> tau
    const int bin = std::max(1, std::min(tauMuIDMap[year]->GetNbinsX(), tauMuIDMap[year]->FindBin(std::fabs(eta))));
    scale_factor  = tauMuIDMap[year]->GetBinContent(bin);
    up            = scale_factor + tauMuIDMap[year]->GetBinError(bin);
    down          = scale_factor - tauMuIDMap[year]->GetBinError(bin);
  }

  if(scale_factor <= 0. || verbose_ > 0) {
    if(scale_factor <= 0.) std::cout << "Warning! Scale factor <= 0! ";
    std::cout << "TauIDWeight::" << __func__
              << " year = " << year
              << " pt = " << pt
              << " eta = " << eta
              << " genID = " << genID
              << " antiJet = " << antiJet
              << " tight jet = " << (antiJetIndex == kTight)
              << " scale_factor = " << scale_factor
              << std::endl;
  }
  return scale_factor;
}

//-------------------------------------------------------------------------------------------------------------
//correction to the tau TLorentzVector
double TauIDWeight::EnergyScale(double pt, double eta, int dm, int genID, UChar_t antiJet, int year, float& up, float& down) {
  if(year > 2000) year -= 2016;
  double scale_factor = 1.;
  up = 1.; down = 1.;
  const double pt_low = 34.;
  const double pt_high = 170.;
  //only defined for certain decay modes
  if(!(dm == 0 || dm == 1 || dm == 10 || dm == 11)) return scale_factor;

  const int antiJetBit = BitFromID(antiJet);
  const int antiJetIndex = (antiJetBit >= kTight) ? kTight : kVLoose; //tight or loose tau

  //if embedding sample, use fixed corrections
  if(isEmbed_) {
    if(year == k2016) {
      if     (dm == 0) {scale_factor += -0.0020; up = scale_factor + 0.0046; down = scale_factor - 0.0046;}
      else if(dm == 1) {scale_factor += -0.0022; up = scale_factor + 0.0022; down = scale_factor - 0.0025;}
      else             {scale_factor += -0.0126; up = scale_factor + 0.0033; down = scale_factor - 0.0051;}
    }
    else if(year == k2017) {
      if     (dm == 0) {scale_factor += -0.0004; up = scale_factor + 0.0041; down = scale_factor - 0.0042;}
      else if(dm == 1) {scale_factor += -0.0120; up = scale_factor + 0.0052; down = scale_factor - 0.0021;}
      else             {scale_factor += -0.0075; up = scale_factor + 0.0044; down = scale_factor - 0.0046;}
    }
    else if(year == k2018) {
      if     (dm == 0) {scale_factor += -0.0033; up = scale_factor + 0.0039; down = scale_factor - 0.0039;}
      else if(dm == 1) {scale_factor += -0.0057; up = scale_factor + 0.0037; down = scale_factor - 0.0031;}
      else             {scale_factor += -0.0074; up = scale_factor + 0.0032; down = scale_factor - 0.0032;}
    }
    return scale_factor;
  }

  /////////////////////
  // Genuine taus
  /////////////////////
  if(genID == 5) {

    //Use the low energy scale factor, but uncertainty using high and low
    const int bin_low  = tauESLowMap [year]->FindBin(dm);
    const int bin_high = tauESHighMap[year]->FindBin(dm);
    scale_factor = tauESLowMap [year]->GetBinContent(bin_low);
    if(pt < pt_low) { //use only the low uncertainty
      up   = scale_factor + tauESLowMap[year]->GetBinError(bin_low);
      down = scale_factor - tauESLowMap[year]->GetBinError(bin_low);
    } else if(pt < pt_high) { //interpolate the uncertainties
      const double err_low  = tauESLowMap [year]->GetBinError(bin_low );
      const double err_high = tauESHighMap[year]->GetBinError(bin_high);
      const double err = err_low + (err_high - err_low)/(pt_high - pt_low)*(pt-pt_low);
      up   = scale_factor + err;
      down = scale_factor - err;
    } else { //use high pT map
      up   = scale_factor + tauESHighMap[year]->GetBinError(bin_high);
      down = scale_factor - tauESHighMap[year]->GetBinError(bin_high);
    }

    /////////////////////
    // Genuine electrons
    /////////////////////
  } else if(genID == 1 || genID == 3) { //genuine electron -> fake tau or tau -> e -> fake tau
    if(dm != 0 && dm != 1) return scale_factor;

    eta = fabs(eta);
    int point = 0;
    if(eta > 2.5)  return scale_factor;
    else if(eta < 1.5) point += 0; //barrel region
    else point += 2; //endcap region
    if(dm == 0) point += 0;
    else        point += 1;
    double x, y;
    tauFakeESMap[year]->GetPoint(point, x, y);
    scale_factor = y;
    up   = scale_factor + tauFakeESMap[year]->GetErrorYhigh(point);
    down = scale_factor - tauFakeESMap[year]->GetErrorYlow (point);

    /////////////////////
    // Genuine muons
    /////////////////////
  } else if(genID == 2 || genID == 4) { //genuine muon
    //no correction, +- 1% uncertainty
    scale_factor = 1.;
    up = 1.01;
    down = 0.99;
  }

  if(scale_factor <= 0. || verbose_ > 0) {
    if(scale_factor <= 0.) std::cout << "Warning! Scale factor <= 0! ";
    std::cout << "TauIDWeight::" << __func__
              << " year = " << year
              << " GenID = " << genID
              << " dm = " << dm
              << " pt = " << pt
              << " eta = " << eta
              << " antiJet = " << antiJet
              << " tight jet = " << (antiJetIndex == kTight)
              << " scale_factor = " << scale_factor
              << std::endl;
  }
  return scale_factor;
}