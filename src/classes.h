#include <vector>
#include <map>
#include <utility>
#include <string>
#include "interface/Tree_t.hh"
#include "interface/CrossSections.hh"
#include "interface/Significances.hh"
#include "interface/BTagWeight.hh"
#include "interface/PUWeight.hh"
#include "interface/JetToTauWeight.hh"
#include "interface/SlimObject_t.hh"
#include "interface/SlimElectron_t.hh"
#include "interface/SlimMuon_t.hh"
#include "interface/SlimTau_t.hh"
#include "interface/SlimJet_t.hh"
#include "interface/SlimPhoton_t.hh"
#include "interface/MVAConfig.hh"
#include "interface/TrkQualInit.hh"
#include "interface/PlottingCard_t.hh"
#include "interface/DataCard_t.hh"
#include "interface/ZTauTauHistMaker.hh"
#include "interface/DataPlotter.hh"
#include "interface/StandardDataConfig.hh"
#include "interface/ParticleCorrections.hh"
#include "interface/NanoAODConversion.hh"

namespace { struct dictionary {
  Tree_t dummy1;
  CrossSections dummy2;
  Significances dummy3;
  BTagWeight dummy4;
  PUWeight dummy5;
  JetToTauWeight dummy6;
  SlimObject_t dummy7;
  SlimElectron_t dummy8;
  SlimMuon_t dummy9;
  SlimTau_t dummy10;
  SlimJet_t dummy11;
  SlimPhoton_t dummy12;
  MVAConfig dummy13;
  TrkQualInit dummy14;
  ZTauTauHistMaker dummy15;
  DataPlotter dummy16;
  ParticleCorrections dummy17;
  NanoAODConversion dummy18;  
};
}

