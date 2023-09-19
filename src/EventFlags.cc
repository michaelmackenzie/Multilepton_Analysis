#include "interface/EventFlags.hh"
using namespace CLFV;

//----------------------------------------------------------------------------------------
void EventFlags::InitBranches(TTree* tree) {
  Utilities::SetBranchAddress(tree, "Flag_METFilters"                        , &METFilters                        );
  Utilities::SetBranchAddress(tree, "Flag_BadChargedCandidateFilter"         , &BadChargedCandidateFilter         );
  Utilities::SetBranchAddress(tree, "Flag_goodVertices"                      , &goodVertices                      );
  Utilities::SetBranchAddress(tree, "Flag_HBHENoiseFilter"                   , &HBHENoiseFilter                   );
  Utilities::SetBranchAddress(tree, "Flag_HBHENoiseIsoFilter"                , &HBHENoiseIsoFilter                );
  Utilities::SetBranchAddress(tree, "Flag_eeBadScFilter"                     , &eeBadSCFilter                     );
  Utilities::SetBranchAddress(tree, "Flag_muonBadTrackFilter"                , &muonBadTrackFilter                );
  Utilities::SetBranchAddress(tree, "Flag_EcalDeadCellTriggerPrimitiveFilter", &EcalDeadCellTriggerPrimitiveFilter);
  Utilities::SetBranchAddress(tree, "Flag_globalTightHalo2016Filter"         , &globalTightHalo2016Filter         );
  Utilities::SetBranchAddress(tree, "Flag_BadPFMuonFilter"                   , &BadPFMuonFilter                   );
}
