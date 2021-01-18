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
#include "../utils/Significances.hh+g"
#include "../utils/CrossSections.hh+g"
#include "../utils/MVAConfig.hh+g"
#include "../utils/PUWeight.hh+g"
#include "../utils/BTagWeight.hh+g"
#include "../utils/JetToTauWeight.hh+g"
#include "./DataPlotter.cc+g"

int compile_objects() {
  TString version = gROOT->GetVersion();
  if(version.Contains("6.06")) {
    //Utilities
    gROOT->LoadMacro("../utils/TrkQualInit.hh+g");
    //Histogrammer
    gROOT->LoadMacro("ZTauTauHistMaker.cc+g");
  } else {
    //Conversion tools
    gROOT->LoadMacro("ParticleCorrections.cc+g");
    gROOT->LoadMacro("NanoAODConversion.cc+g");
  }

  std::cout << "Compilation complete!" << std::endl;
  return 0;
}
