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


int compile_objects() {
  TString version = gROOT->GetVersion();
  gROOT->LoadMacro("../utils/Significances.hh+g");
  //Plotting histograms
  gROOT->LoadMacro("DataPlotter.cc+g");
  if(version.Contains("6.06")) {
    //Utilities
    gROOT->LoadMacro("../utils/TrkQualInit.cc+g");
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
