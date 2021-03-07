#ifndef __SYSTEMATICSHIFTS__
#define __SYSTEMATICSHIFTS__
//Class to decide if certain bins of systematics are shifted up or down
//c++ includes
#include <map>
#include <iostream>
//ROOT includes
#include "TRandom3.h"

class SystematicShifts {
public:
  SystematicShifts(int seed = 90) {
    rnd_ = new TRandom3(seed);

    for(int year = 0; year < 3; ++year) {
      //initialize Muon and Electron ID maps
      for(int ptbins = 0; ptbins < 20; ++ptbins) {
        for(int etabins = 0; etabins < 20; ++etabins) {
          for(int run_section = 0; run_section < 2; ++run_section) {
            muonID_        [year*kYears + kRunSection*run_section + 100*etabins + ptbins] = 0.5 < rnd_->Uniform();
            muonIsoID_     [year*kYears + kRunSection*run_section + 100*etabins + ptbins] = 0.5 < rnd_->Uniform();
            electronID_    [year*kYears + kRunSection*run_section + 100*etabins + ptbins] = 0.5 < rnd_->Uniform();
            electronRecoID_[year*kYears + kRunSection*run_section + 100*etabins + ptbins] = 0.5 < rnd_->Uniform();
          }
        }
      }
      //initialize Tau ID maps
      for(int genID = 0; genID < 6; ++genID) {
        for(int etabins = 0; etabins < kGenID; ++etabins) {
          tauID_[year*kYears + genID*kGenID + etabins] = 0.5 < rnd_->Uniform();
        }
      }
    } //end year loop
  }

  bool MuonID(int year, int ibin) {
    return muonID_[(year - 2016)*kYears + ibin];
  }
  bool MuonIsoID(int year, int ibin) {
    return muonIsoID_[(year - 2016)*kYears + ibin];
  }
  bool ElectronID(int year, int ibin) {
    return electronID_[(year - 2016)*kYears + ibin];
  }
  bool ElectronRecoID(int year, int ibin) {
    return electronRecoID_[(year - 2016)*kYears + ibin];
  }
  bool TauID(int year, int genID, int ibin) {
    return tauID_[(year - 2016)*kYears + kGenID*genID + ibin];
  }

public:
  enum {kYears = 100000, kRunSection = 10000, kGenID = 100};
  TRandom3 *rnd_;
  std::map<int, bool> muonID_;
  std::map<int, bool> muonIsoID_;
  std::map<int, bool> electronID_;
  std::map<int, bool> electronRecoID_;
  std::map<int, bool> tauID_;
};
#endif
