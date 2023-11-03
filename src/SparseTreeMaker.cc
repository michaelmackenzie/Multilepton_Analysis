#define SPARSETREEMAKER_CXX

#include "interface/SparseTreeMaker.hh"
using namespace CLFV;


//--------------------------------------------------------------------------------------------------------------
SparseTreeMaker::SparseTreeMaker(int seed, TTree * /*tree*/) : HistMaker(seed) {
  fDoSystematics = 0;
  fWriteTrees = 0;
}

//--------------------------------------------------------------------------------------------------------------
void SparseTreeMaker::Begin(TTree * /*tree*/)
{
  HistMaker::Begin(nullptr);
  BookOutputTree();
}


//--------------------------------------------------------------------------------------------------------------
void SparseTreeMaker::InitHistogramFlags() {
  for(int i = 0; i < fn; ++i) {
    fEventSets[i]  = 0;
    fSysSets[i] = 0;
    fTreeSets[i]  = 0;
    fSetFills [i] = 0;
  }
}

//--------------------------------------------------------------------------------------------------------------
void SparseTreeMaker::InitializeInputTree(TTree* tree) {
  HistMaker::InitializeInputTree(tree);
}

//--------------------------------------------------------------------------------------------------------------
void SparseTreeMaker::BookOutputTree() {
  fOutputTree = new TTree(fSelection, "Sparse event tree");
  fOutputTree->Branch("eventweight"   , &eventWeight             );
  fOutputTree->Branch("genweight"     , &genWeight               );
  fOutputTree->Branch("lepm"          , &fTreeVars.lepm          );
  fOutputTree->Branch("leppt"         , &fTreeVars.leppt         );
  fOutputTree->Branch("lepeta"        , &fTreeVars.lepeta        );
  fOutputTree->Branch("lepphi"        , &fTreeVars.lepphi        );
  fOutputTree->Branch("lepdeltaphi"   , &fTreeVars.lepdeltaphi   );
  fOutputTree->Branch("lepdeltaeta"   , &fTreeVars.lepdeltaeta   );
  fOutputTree->Branch("met"           , &met                     );
  fOutputTree->Branch("metphi"        , &metPhi                  );
  fOutputTree->Branch("mtone"         , &fTreeVars.mtone         );
  fOutputTree->Branch("mttwo"         , &fTreeVars.mttwo         );
  fOutputTree->Branch("mtlep"         , &fTreeVars.mtlep         );
  fOutputTree->Branch("onemetdeltaphi", &fTreeVars.onemetdeltaphi);
  fOutputTree->Branch("twometdeltaphi", &fTreeVars.twometdeltaphi);
  fOutputTree->Branch("lepmetdeltaphi", &fTreeVars.metdeltaphi   );
  fOutputTree->Branch("lepestimateone", &fTreeVars.mestimate     ); //lep 1 = prompt
  fOutputTree->Branch("lepestimatetwo", &fTreeVars.mestimatetwo  ); //lep 2 = prompt
  fOutputTree->Branch("deltaalphaz1"  , &fTreeVars.deltaalphaz1  ); //lep 2 = prompt
  fOutputTree->Branch("deltaalphaz2"  , &fTreeVars.deltaalphaz2  ); //lep 1 = prompt
  fOutputTree->Branch("beta1"         , &fTreeVars.beta1         );
  fOutputTree->Branch("beta2"         , &fTreeVars.beta2         );
  fOutputTree->Branch("ht"            , &fTreeVars.ht            );
  fOutputTree->Branch("htsum"         , &fTreeVars.htsum         );
  fOutputTree->Branch("jetpt"         , &jetOne.pt               );
  fOutputTree->Branch("njets"         , &nJets20                 );
  fOutputTree->Branch("nbjets"        , &nBJetsUse               );
  fOutputTree->Branch("nelectrons"    , &nElectrons              );
  fOutputTree->Branch("nmuons"        , &nElectrons              );
  fOutputTree->Branch("ntaus"         , &nTaus                   );
  fOutputTree->Branch("ngenele"       , &nGenElectrons           );
  fOutputTree->Branch("ngenmu"        , &nGenMuons               );
  fOutputTree->Branch("ngentau"       , &nGenTaus                );
  fOutputTree->Branch("ngenhardele"   , &nGenHardElectrons       );
  fOutputTree->Branch("ngenhardmu"    , &nGenHardMuons           );
  fOutputTree->Branch("ngenhardtau"   , &nGenHardTaus            );
  fOutputTree->Branch("zpt"           , &zPt                     );
  fOutputTree->Branch("zmass"         , &zMass                   );
  fOutputTree->Branch("zeta"          , &zEta                    );
  fOutputTree->Branch("zleponept"     , &zLepOnePt               );
  fOutputTree->Branch("zleponeid"     , &zLepOneID               );
  fOutputTree->Branch("zleptwopt"     , &zLepTwoPt               );
  fOutputTree->Branch("zleptwoid"     , &zLepTwoID               );

  fOutputTree->Branch("onept"         , &leptonOne.pt            );
  fOutputTree->Branch("oneeta"        , &leptonOne.eta           );
  fOutputTree->Branch("onephi"        , &leptonOne.phi           );
  fOutputTree->Branch("onemass"       , &leptonOne.mass          );
  fOutputTree->Branch("onesceta"      , &leptonOne.scEta         );
  fOutputTree->Branch("oneflavor"     , &leptonOne.flavor        );
  fOutputTree->Branch("onereliso"     , &leptonOne.relIso        );
  fOutputTree->Branch("onedxy"        , &leptonOne.dxy           );
  fOutputTree->Branch("onedz"         , &leptonOne.dz            );
  fOutputTree->Branch("onedxysig"     , &leptonOne.dxySig        );
  fOutputTree->Branch("onedzsig"      , &leptonOne.dzSig         );
  fOutputTree->Branch("oneid1"        , &leptonOne.id1           );
  fOutputTree->Branch("oneid2"        , &leptonOne.id2           );
  fOutputTree->Branch("oneid3"        , &leptonOne.id3           );
  fOutputTree->Branch("oneisloose"    , &leptonOne.isLoose       );
  fOutputTree->Branch("onematched"    , &leptonOne.matched       );
  fOutputTree->Branch("onefired"      , &leptonOne.fired         );
  fOutputTree->Branch("onegenflavor"  , &leptonOne.genFlavor     );
  fOutputTree->Branch("onegenid"      , &leptonOne.genID         );

  fOutputTree->Branch("twopt"         , &leptonTwo.pt            );
  fOutputTree->Branch("twoeta"        , &leptonTwo.eta           );
  fOutputTree->Branch("twophi"        , &leptonTwo.phi           );
  fOutputTree->Branch("twomass"       , &leptonTwo.mass          );
  fOutputTree->Branch("twosceta"      , &leptonTwo.scEta         );
  fOutputTree->Branch("twoflavor"     , &leptonTwo.flavor        );
  fOutputTree->Branch("tworeliso"     , &leptonTwo.relIso        );
  fOutputTree->Branch("twodxy"        , &leptonTwo.dxy           );
  fOutputTree->Branch("twodz"         , &leptonTwo.dz            );
  fOutputTree->Branch("twodxysig"     , &leptonTwo.dxySig        );
  fOutputTree->Branch("twodzsig"      , &leptonTwo.dzSig         );
  fOutputTree->Branch("twoid1"        , &leptonTwo.id1           );
  fOutputTree->Branch("twoid2"        , &leptonTwo.id2           );
  fOutputTree->Branch("twoid3"        , &leptonTwo.id3           );
  fOutputTree->Branch("twoisloose"    , &leptonTwo.isLoose       );
  fOutputTree->Branch("twomatched"    , &leptonTwo.matched       );
  fOutputTree->Branch("twofired"      , &leptonTwo.fired         );
  fOutputTree->Branch("twogenflavor"  , &leptonTwo.genFlavor     );
  fOutputTree->Branch("twogenid"      , &leptonTwo.genID         );

  fOutputTree->Branch("taudeepantijet", &tauDeepAntiJet          );
  fOutputTree->Branch("taudeepantiele", &tauDeepAntiEle          );
  fOutputTree->Branch("taudeepantimu" , &tauDeepAntiMu           );
  fOutputTree->Branch("taudecaymode"  , &tauDecayMode            );

  for(unsigned imva = 0; imva < fMVAConfig->names_.size(); ++imva) {
    fOutputTree->Branch(Form("mva%i"    , imva), &fMvaOutputs[imva]);
    fOutputTree->Branch(Form("mvafofp%i", imva), &fMvaFofP   [imva]);
  }
}

//--------------------------------------------------------------------------------------------------------------
void SparseTreeMaker::FillOutputTree() {
  fOutputTree->Fill();
}

//--------------------------------------------------------------------------------------------------------------
void SparseTreeMaker::BookHistograms() {
}


//--------------------------------------------------------------------------------------------------------------
void SparseTreeMaker::FillAllHistograms(Int_t) {
}



//--------------------------------------------------------------------------------------------------------------
// Main function, process each event in the chain
Bool_t SparseTreeMaker::Process(Long64_t entry)
{
  InitializeEvent(entry);
  FillOutputTree();
  return kTRUE;
}
