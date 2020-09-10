//script to compile relevant objects for the project
//run with `root -l compile_objects.C`

//Data formats
#include "../dataFormats/SlimObject_t.hh+g"
#include "../dataFormats/SlimElectron_t.hh+g"
#include "../dataFormats/SlimMuon_t.hh+g"
#include "../dataFormats/SlimTau_t.hh+g"
#include "../dataFormats/SlimJet_t.hh+g"
#include "../dataFormats/SlimPhoton_t.hh+g"
#include "../dataFormats/Tree_t.hh+g"

//Utilities
#include "../utils/TrkQualInit.cc+g"
#include "../utils/Significances.hh+g"

//Conversion tools
#include "ParticleCorrections.cc+g"
#include "NanoAODConversion.cc+g"

//Histogrammer
#include "ZTauTauHistMaker.cc+g"

//Plotting histograms
#include "DataPlotter.cc+g"

int compile_objects() {
  std::cout << "Compilation complete!" << std::endl;
  return 0;
}
