#ifndef __EVENTFLAGS__
#define __EVENTFLAGS__
//Class to handle event flags

//c++ includes
#include <iostream>

//ROOT includes
#include "TTree.h"

//local includes
#include "interface/Utilities.hh"

namespace CLFV {
  class EventFlags {

  public:
    EventFlags() {}
    ~EventFlags() {}

    void InitBranches(TTree* tree);

    //flags
    bool METFilters = true;
    bool BadChargedCandidateFilter = true;
    bool goodVertices = true;
    bool HBHENoiseFilter = true;
    bool HBHENoiseIsoFilter = true;
    bool eeBadSCFilter = true;
    bool muonBadTrackFilter = true;
    bool EcalDeadCellTriggerPrimitiveFilter = true;
    bool globalTightHalo2016Filter = true;
    bool BadPFMuonFilter = true;
  };
}
#endif
