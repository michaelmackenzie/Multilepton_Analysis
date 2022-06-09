#include <vector>
#include <map>
#include <utility>
#include <string>

#include "interface/GlobalConstants.h"
#include "interface/Tree_t.hh"
#include "interface/EventHist_t.hh"
#include "interface/LepHist_t.hh"
#include "interface/SystematicHist_t.hh"
#include "interface/CrossSections.hh"
#include "interface/Significances.hh"
#include "interface/Utilities.hh"
#include "interface/CreateCategories.hh"
#include "interface/BTagWeight.hh"
#include "interface/PUWeight.hh"
#include "interface/JetPUWeight.hh"
#include "interface/PrefireWeight.hh"
#include "interface/JetToLepWeight.hh"
#include "interface/JetToTauWeight.hh"
#include "interface/JetToTauComposition.hh"
#include "interface/QCDWeight.hh"
#include "interface/MuonIDWeight.hh"
#include "interface/ZPtWeight.hh"
#include "interface/EmbeddingWeight.hh"
#include "interface/EmbeddingTnPWeight.hh"
#include "interface/TauIDWeight.hh"
#include "interface/ElectronIDWeight.hh"
#include "interface/RoccoR.h"
#include "interface/MVAConfig.hh"
#include "interface/TrkQualInit.hh"
#include "interface/HistMaker.hh"
#include "interface/CLFVHistMaker.hh"
#include "interface/SparseHistMaker.hh"
#include "interface/CLFVTmpHistMaker.hh"
#include "interface/DataCard_t.hh"
#include "interface/PlottingCard_t.hh"
#include "interface/DataPlotter.hh"
#include "interface/Fitter.hh"

using namespace CLFV;
namespace CLFV {
  struct dictionary {
    Tree_t dummy1;
    EventHist_t dummy1b;
    LepHist_t dummy1c;
    SystematicHist_t dummy1d;
    CrossSections dummy2;
    Significances dummy3;
    Utilities dummy3a;
    CreateCategories dummy3b;
    BTagWeight dummy4;
    PUWeight dummy5;
    JetPUWeight dummy5b;
    PrefireWeight dummy5c;
    JetToTauWeight dummy6;
    JetToTauComposition dummy6b;
    JetToLepWeight dummy6b2;
    QCDWeight dummy6b3;
    ElectronIDWeight dummy6c2;
    MuonIDWeight dummy6c;
    TauIDWeight dummy6d;
    ZPtWeight dummy6e;
    EmbeddingWeight dummy6f;
    EmbeddingTnPWeight dummy6f2;
    RoccoR dummy12b;
    MVAConfig dummy13;
    TrkQualInit dummy14;
    HistMaker dummy15a;
    CLFVHistMaker dummy15b;
    SparseHistMaker dummy15b2;
    CLFVTmpHistMaker dummy15b3;
    DataCard_t dummy15c;
    PlottingCard_t dummy15d;
    DataPlotter dummy16;
    Fitter dummy19;
  };
}
