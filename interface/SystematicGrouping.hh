#ifndef __SYSTEMATICGROUPING__
#define __SYSTEMATICGROUPING__
//Class to decide if certain bins of systematics are grouped together
//c++ includes
#include <map>
#include <iostream>
//ROOT includes
#include "TRandom3.h"

class SystematicGrouping {
public:
  SystematicGrouping() {
    for(int year = 0; year < 3; ++year) {
      //initialize Muon and Electron ID maps
      for(int ptbins = 0; ptbins < 20; ++ptbins) {
        for(int etabins = 0; etabins < 20; ++etabins) {
          for(int run_section = 0; run_section < 2; ++run_section) {
            InitMuonID(year, ptbins, etabins, run_section);
            InitMuonIsoID(year, ptbins, etabins, run_section);
            InitElectronID(year, ptbins, etabins, run_section);
            InitElectronRecoID(year, ptbins, etabins, run_section);
          }
        }
      }
      //initialize Tau ID maps
      for(int genID = 0; genID < 6; ++genID) {
        for(int etabins = 0; etabins < kGenID; ++etabins) {
          InitTauID(year, genID, etabins);
        }
      }
    } //end year loop
  }

  /////////////////////////////////////////////////
  // Initialize groupings
  /////////////////////////////////////////////////

  void InitMuonID(int year, int ptbin, int etabin, int run_section) {
    // bool endcap = etabin < 8 || etabin > 21; //2 per eta
    // bool highpt = ptbin >= 4; //2 per pT
    // int group = highpt + 2*endcap + 4*year; //3 per year
    int group = year;
    muonID_ [kYears*year + kRunSection*run_section + kEta*etabin + ptbin] = group;

  }

  void InitMuonIsoID(int year, int ptbin, int etabin, int run_section) {
    int group = year;
    muonIsoID_ [kYears*year + kRunSection*run_section + kEta*etabin + ptbin] = group;
  }

  void InitElectronID(int year, int ptbin, int etabin, int run_section) {
    int group = year;
    electronID_ [kYears*year + kRunSection*run_section + kEta*etabin + ptbin] = group;
  }

  void InitElectronRecoID(int year, int ptbin, int etabin, int run_section) {
    int group = year;
    electronRecoID_ [kYears*year + kRunSection*run_section + kEta*etabin + ptbin] = group;
  }

  void InitTauID(int year, int genID, int etabin) {
    int group = year;
    tauID_ [kYears*year + kGenID*genID + etabin] = group;
  }

  /////////////////////////////////////////////////
  // Retrieve groupings
  /////////////////////////////////////////////////

  int MuonID(int year, int ibin) {
    return muonID_[(year - 2016)*kYears + ibin] + kMuonID;
  }
  int MuonIsoID(int year, int ibin) {
    return muonIsoID_[(year - 2016)*kYears + ibin] + kMuonIsoID;
  }
  int ElectronID(int year, int ibin) {
    return electronID_[(year - 2016)*kYears + ibin] + kElectronID;
  }
  int ElectronRecoID(int year, int ibin) {
    return electronRecoID_[(year - 2016)*kYears + ibin] + kElectronRecoID;
  }
  int TauID(int year, int genID, int ibin) {
    return tauID_[(year - 2016)*kYears + kGenID*genID + ibin] + kTauID;
  }

public:
  enum {kYears = 100000, kRunSection = 10000, kGenID = 100, kEta = 100,
        kJetToTau = 50,
        kMuonID = 100, kMuonIsoID = 150,
        kElectronID = 200, kElectronRecoID = 250,
        kTauID = 300,
};
  std::map<int, int> muonID_;
  std::map<int, int> muonIsoID_;
  std::map<int, int> electronID_;
  std::map<int, int> electronRecoID_;
  std::map<int, int> tauID_;
};
#endif
