#define NanoAODConversion_cxx
// The class definition in NanoAODConversion.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.




#include "interface/NanoAODConversion.hh"
#include <TStyle.h>
using namespace CLFV;

//-----------------------------------------------------------------------------------------------------------------
void NanoAODConversion::Begin(TTree * /*tree*/)
{
  // The Begin() function is called at the start of the query.
  // When running with PROOF Begin() is only called on the client.
  // The tree argument is deprecated (on PROOF 0 is passed).

  printf("NanoAODConversion::%s\n", __func__);
  timer->Start();

  //Initialize correction objects
  particleCorrections = new ParticleCorrections(fIsEmbed, fMuonIso);
  if(fVerbose > 0) particleCorrections->fVerbose = fVerbose - 1; //1 lower level, 1 further in depth
  fChain = 0;
  TString dir = gSystem->Getenv("CMSSW_BASE");
  if(dir == "") dir = "../"; //if not in a CMSSW environment, assume in a sub-directory of the package
  else dir += "/src/CLFVAnalysis/";
  fRoccoR = new RoccoR(Form("%sscale_factors/RoccoR%i.txt", dir.Data(), fYear + 2016));
  fRnd = new TRandom3(fRndSeed);

  for(int i = kMuTau; i < kSelections; ++i) fQCDCounts[i] = 0;
  fSelecNames[kMuTau] = "MuTau";
  fSelecNames[kETau ] = "ETau" ;
  fSelecNames[kEMu  ] = "EMu"  ;
  fSelecNames[kMuMu ] = "MuMu" ;
  fSelecNames[kEE   ] = "EE"   ;

  //Object pT thresholds
  fElectronPtCount = 10.;
  fMuonPtCount     = 10.;
  fTauPtCount      = 20.;
  fPhotonPtCount   = 10.;

  //initialize selection parameters
  fTauUseDeep[kMuTau] = true;
  fTauUseDeep[kETau]  = true;
  fTauUseDeep[kEMu]   = true;
  fTauUseDeep[kEE]    = true;
  fTauUseDeep[kMuMu]  = true;

  //initialize object counting parameters for each selection
  //mutau
  fMuonIsoSelect   [kMuTau] = ParticleCorrections::kTightMuIso;
  fMuonIDSelect    [kMuTau] = 3;
  fTauAntiEleSelect[kMuTau] = 10;
  fTauAntiMuSelect [kMuTau] = 10;
  fTauAntiJetSelect[kMuTau] = 50;
  fTauIDDecaySelect[kMuTau] = true;
  //etau
  fElectronIDSelect[kETau]  = 2;
  fTauAntiEleSelect[kETau]  = 50;
  fTauAntiMuSelect [kETau]  = 10;
  fTauAntiJetSelect[kETau]  = 50;
  fTauIDDecaySelect[kETau]  = true;
  //emu
  fMuonIsoSelect   [kEMu]   = ParticleCorrections::kTightMuIso;
  fMuonIDSelect    [kEMu]   = 3;
  fElectronIDSelect[kEMu]   = 2;
  //mumu
  fMuonIsoSelect   [kMuMu]  = ParticleCorrections::kTightMuIso;
  fMuonIDSelect    [kMuMu]  = 3;
  //ee
  fElectronIDSelect[kEE]    = 2
;
  //initialize object counting parameters for each selection
  //mutau
  fCountMuons       [kMuTau] = true;
  fMuonIsoCount     [kMuTau] = ParticleCorrections::kVLooseMuIso;
  fMuonIDCount      [kMuTau] = 1;
  fCountElectrons   [kMuTau] = true;
  fElectronIDCount  [kMuTau] = 1;
  fCountTaus        [kMuTau] = true;
  fTauAntiEleCount  [kMuTau] = 10;
  fTauAntiMuCount   [kMuTau] = 10;
  fTauAntiOldMuCount[kMuTau] = 0;
  fTauAntiJetCount  [kMuTau] = 5;
  fTauDeltaRCount   [kMuTau] = 0.3;
  fTauIDDecayCount  [kMuTau] = true;
  fPhotonIDCount    [kMuTau] = 1; //WP80
  fPhotonDeltaRCount[kMuTau] = 0.3;
  //etau
  fCountMuons       [kETau]  = true;
  fMuonIsoCount     [kETau]  = ParticleCorrections::kVLooseMuIso;
  fMuonIDCount      [kETau]  = 1;
  fCountElectrons   [kETau]  = true;
  fElectronIDCount  [kETau]  = 1;
  fCountTaus        [kETau]  = true;
  fTauAntiEleCount  [kETau]  = 10;
  fTauAntiMuCount   [kETau]  = 10;
  fTauAntiOldMuCount[kETau]  = 0;
  fTauAntiJetCount  [kETau]  = 5;
  fTauDeltaRCount   [kETau]  = 0.3;
  fTauIDDecayCount  [kETau]  = true;
  fPhotonIDCount    [kETau] = 1; //WP80
  fPhotonDeltaRCount[kETau] = 0.3;
  //emu
  fCountMuons       [kEMu]   = true;
  fMuonIsoCount     [kEMu]   = ParticleCorrections::kVLooseMuIso;
  fMuonIDCount      [kEMu]   = 1;
  fCountElectrons   [kEMu]   = true;
  fElectronIDCount  [kEMu]   = 1;
  fCountTaus        [kEMu]   = true; //count taus just to have the information
  fTauAntiEleCount  [kEMu]   = 10;
  fTauAntiMuCount   [kEMu]   = 10;
  fTauAntiOldMuCount[kEMu]   = 0;
  fTauAntiJetCount  [kEMu]   = 5;
  fTauDeltaRCount   [kEMu]   = 0.3;
  fTauIDDecayCount  [kEMu]   = true;
  fPhotonIDCount    [kEMu]   = 1; //WP80
  fPhotonDeltaRCount[kEMu]   = 0.3;
  //mumu
  fCountMuons       [kMuMu]  = true;
  fMuonIsoCount     [kMuMu]  = ParticleCorrections::kVLooseMuIso;
  fMuonIDCount      [kMuMu]  = 1;
  fCountElectrons   [kMuMu]  = true;
  fElectronIDCount  [kMuMu]  = 1;
  fCountTaus        [kMuMu]  = true;
  fTauAntiEleCount  [kMuMu]  = 10;
  fTauAntiMuCount   [kMuMu]  = 10;
  fTauAntiOldMuCount[kMuMu]  = 2;
  fTauAntiJetCount  [kMuMu]  = 5;
  fTauDeltaRCount   [kMuMu]  = 0.3;
  fTauIDDecayCount  [kMuMu]  = true;
  fPhotonIDCount    [kMuMu]  = 1; //WP80
  fPhotonDeltaRCount[kMuMu]  = 0.3;
  //ee
  fCountMuons       [kEE]    = true;
  fMuonIsoCount     [kEE]    = ParticleCorrections::kVLooseMuIso;
  fMuonIDCount      [kEE]    = 1;
  fCountElectrons   [kEE]    = true;
  fElectronIDCount  [kEE]    = 1;
  fCountTaus        [kEE]    = true;
  fTauAntiEleCount  [kEE]    = 50;
  fTauAntiMuCount   [kEE]    = 10;
  fTauAntiOldMuCount[kEE]    = 2;
  fTauAntiJetCount  [kEE]    = 5;
  fTauDeltaRCount   [kEE]    = 0.3;
  fTauIDDecayCount  [kEE]    = true;
  fPhotonIDCount    [kEE]    = 1; //WP80
  fPhotonDeltaRCount[kEE]    = 0.3;

}

//-----------------------------------------------------------------------------------------------------------------
void NanoAODConversion::SlaveBegin(TTree * /*tree*/)
{
  // The SlaveBegin() function is called after the Begin() function.
  // When running with PROOF SlaveBegin() is called on each slave server.
  // The tree argument is deprecated (on PROOF 0 is passed).

  printf("NanoAODConversion::SlaveBegin\n");

}

//-----------------------------------------------------------------------------------------------------------------
void NanoAODConversion::InitializeInBranchStructure(TTree* tree) {
  if(fIsData == 0) {
    tree->SetBranchAddress("genWeight"                     , &genWeight                      ) ;
    tree->SetBranchAddress("puWeight"                      , &puWeight                       ) ;
  }
  //Lepton branches
  tree->SetBranchAddress("nMuon"                           , &nMuon                          ) ;
  tree->SetBranchAddress("nMuons"                          , &nMuonsSkim                     ) ;
  tree->SetBranchAddress("Muon_pt"                         , &muonPt                         ) ;
  tree->SetBranchAddress("Muon_eta"                        , &muonEta                        ) ;
  tree->SetBranchAddress("Muon_phi"                        , &muonPhi                        ) ;
  tree->SetBranchAddress("Muon_mass"                       , &muonMass                       ) ;
  tree->SetBranchAddress("Muon_charge"                     , &muonCharge                     ) ;
  tree->SetBranchAddress("Muon_pfRelIso04_all"             , &muonRelIso                     ) ;
  tree->SetBranchAddress("Muon_looseId"                    , &muonLooseId                    ) ;
  tree->SetBranchAddress("Muon_mediumId"                   , &muonMediumId                   ) ;
  tree->SetBranchAddress("Muon_tightId"                    , &muonTightId                    ) ;
  tree->SetBranchAddress("Muon_nTrackerLayers"             , &muonNTrkLayers                 ) ;
  tree->SetBranchAddress("Muon_genPartFlav"                , &muonGenFlavor                  ) ;
  tree->SetBranchAddress("Muon_dxy"                        , &muondxy                        ) ;
  tree->SetBranchAddress("Muon_dz"                         , &muondz                         ) ;
  tree->SetBranchAddress("nElectron"                       , &nElectron                      ) ;
  tree->SetBranchAddress("nElectrons"                      , &nElectronsSkim                 ) ;
  tree->SetBranchAddress("Electron_pt"                     , &electronPt                     ) ;
  tree->SetBranchAddress("Electron_eta"                    , &electronEta                    ) ;
  tree->SetBranchAddress("Electron_phi"                    , &electronPhi                    ) ;
  tree->SetBranchAddress("Electron_mass"                   , &electronMass                   ) ;
  tree->SetBranchAddress("Electron_charge"                 , &electronCharge                 ) ;
  tree->SetBranchAddress("Electron_deltaEtaSC"             , &electronDeltaEtaSC             ) ;
  tree->SetBranchAddress("Electron_mvaFall17V2Iso"         , &electronMVAFall17              ) ;
  tree->SetBranchAddress("Electron_mvaFall17V2Iso_WPL"     , &electronWPL                    ) ;
  tree->SetBranchAddress("Electron_mvaFall17V2Iso_WP80"    , &electronWP80                   ) ;
  tree->SetBranchAddress("Electron_mvaFall17V2Iso_WP90"    , &electronWP90                   ) ;
  tree->SetBranchAddress("Electron_pfRelIso03_all"         , &electronRelIso                 ) ;
  tree->SetBranchAddress("Electron_genPartFlav"            , &electronGenFlavor              ) ;
  tree->SetBranchAddress("Electron_dxy"                    , &electrondxy                    ) ;
  tree->SetBranchAddress("Electron_dz"                     , &electrondz                     ) ;
  tree->SetBranchAddress("Electron_eCorr"                  , &electronECorr                  ) ;
  tree->SetBranchAddress("nTau"                            , &nTau                           ) ;
  tree->SetBranchAddress("nTaus"                           , &nTausSkim                      ) ;
  tree->SetBranchAddress("nGenTaus"                        , &nGenTaus                       ) ;
  tree->SetBranchAddress("Tau_pt"                          , &tauPt                          ) ;
  tree->SetBranchAddress("Tau_eta"                         , &tauEta                         ) ;
  tree->SetBranchAddress("Tau_phi"                         , &tauPhi                         ) ;
  tree->SetBranchAddress("Tau_mass"                        , &tauMass                        ) ;
  tree->SetBranchAddress("Tau_charge"                      , &tauCharge                      ) ;
  tree->SetBranchAddress("Tau_dxy"                         , &taudxy                         ) ;
  tree->SetBranchAddress("Tau_dz"                          , &taudz                          ) ;
  tree->SetBranchAddress("Tau_idAntiEle"                   , &tauAntiEle                     ) ;
  tree->SetBranchAddress("Tau_idAntiEle2018"               , &tauAntiEle2018                 ) ;
  tree->SetBranchAddress("Tau_idAntiMu"                    , &tauAntiMu                      ) ;
  tree->SetBranchAddress("Tau_idMVAnewDM2017v2"            , &tauAntiJet                     ) ;
  tree->SetBranchAddress("Tau_decayMode"                   , &tauDecayMode                   ) ;
  tree->SetBranchAddress("Tau_idDecayMode"                 , &tauIDDecayMode                 ) ;
  tree->SetBranchAddress("Tau_idDecayModeNewDMs"           , &tauIDDecayModeNew              ) ;
  tree->SetBranchAddress("Tau_idDeepTau2017v2p1VSe"        , &tauDeep2017VsE                 ) ;
  tree->SetBranchAddress("Tau_idDeepTau2017v2p1VSmu"       , &tauDeep2017VsMu                ) ;
  tree->SetBranchAddress("Tau_idDeepTau2017v2p1VSjet"      , &tauDeep2017VsJet               ) ;
  if(fIsData == 0)
    tree->SetBranchAddress("Tau_genPartFlav"                 , &tauGenID                       ) ;

  //Jet branches
  tree->SetBranchAddress("nJet"                            , &nJet                           ) ;
  tree->SetBranchAddress("Jet_pt"                          , &jetPt                          ) ;
  tree->SetBranchAddress("Jet_eta"                         , &jetEta                         ) ;
  tree->SetBranchAddress("Jet_phi"                         , &jetPhi                         ) ;
  tree->SetBranchAddress("Jet_mass"                        , &jetMass                        ) ;
  tree->SetBranchAddress("Jet_jetId"                       , &jetID                          ) ;
  tree->SetBranchAddress("Jet_puId"                        , &jetPUID                        ) ;
  tree->SetBranchAddress("Jet_btagDeepB"                   , &jetBTagDeepB                   ) ;
  tree->SetBranchAddress("Jet_btagCMVA"                    , &jetBTagCMVA                    ) ;
  if(fIsData == 0)
    tree->SetBranchAddress("Jet_partonFlavour"               , &jetFlavor                      ) ;

  //Photon branches
  tree->SetBranchAddress("nPhoton"                         , &nPhoton                        ) ;
  tree->SetBranchAddress("Photon_pt"                       , &photonPt                       ) ;
  tree->SetBranchAddress("Photon_eta"                      , &photonEta                      ) ;
  tree->SetBranchAddress("Photon_phi"                      , &photonPhi                      ) ;
  tree->SetBranchAddress("Photon_mass"                     , &photonMass                     ) ;
  tree->SetBranchAddress("Photon_mvaID"                    , &photonMVAID                    ) ;
  tree->SetBranchAddress("Photon_mvaID_WP80"               , &photonWP80                     ) ;
  tree->SetBranchAddress("Photon_mvaID_WP90"               , &photonWP90                     ) ;

  //Triggers
  tree->SetBranchAddress("HLT_IsoMu24"                     , &HLT_IsoMu24                    ) ;
  tree->SetBranchAddress("HLT_IsoMu27"                     , &HLT_IsoMu27                    ) ;
  tree->SetBranchAddress("HLT_Mu50"                        , &HLT_Mu50                       ) ;
  tree->SetBranchAddress("HLT_Ele27_WPTight_Gsf"           , &HLT_Ele27_WPTight_GsF          ) ;
  tree->SetBranchAddress("HLT_Ele32_WPTight_Gsf"           , &HLT_Ele32_WPTight_GsF          ) ;
  tree->SetBranchAddress("HLT_Ele32_WPTight_Gsf_L1DoubleEG", &HLT_Ele32_WPTight_GsF_L1DoubleEG) ;
  tree->SetBranchAddress("nTrigObj"                        , &nTrigObjs                      ) ;
  tree->SetBranchAddress("TrigObj_filterBits"              , &trigObjFilterBits              ) ;
  tree->SetBranchAddress("TrigObj_pt"                      , &trigObjPt                      ) ;
  tree->SetBranchAddress("TrigObj_eta"                     , &trigObjEta                     ) ;
  tree->SetBranchAddress("TrigObj_phi"                     , &trigObjPhi                     ) ;
  tree->SetBranchAddress("TrigObj_id"                      , &trigObjID                      ) ;

  //Event info
  tree->SetBranchAddress("PuppiMET_pt"                     , &puppMET                        ) ;
  tree->SetBranchAddress("PuppiMET_phi"                    , &puppMETphi                     ) ;
  tree->SetBranchAddress("PV_npvsGood"                     , &nGoodPV                        ) ;
  if(fIsData == 0) {
    tree->SetBranchAddress("Pileup_nPU"                      , &nPUAdded                       ) ;
    tree->SetBranchAddress("Pileup_nTrueInt"                 , &nPU                            ) ;
  }
  tree->SetBranchAddress("LHE_Njets"                       , &LHE_Njets                      ) ;

  tree->SetBranchAddress("event"                           , &eventNumber                      ) ;
  tree->SetBranchAddress("run"                             , &runNumber                      ) ;
  tree->SetBranchAddress("luminosityBlock"                 , &lumiSection                      ) ;

  //Skimmer defined branches
  tree->SetBranchAddress("leptonOneIndex"                  , &leptonOneSkimIndex             ) ;
  tree->SetBranchAddress("leptonTwoIndex"                  , &leptonTwoSkimIndex             ) ;
  tree->SetBranchAddress("leptonOneFlavor"                 , &leptonOneSkimFlavor            ) ;
  tree->SetBranchAddress("leptonTwoFlavor"                 , &leptonTwoSkimFlavor            ) ;
  tree->SetBranchAddress("leptonOneGenPt"                  , &leptonOneGenPt                 ) ;
  tree->SetBranchAddress("leptonTwoGenPt"                  , &leptonTwoGenPt                 ) ;
  tree->SetBranchAddress("zPt"                             , &zPtIn                          ) ;
  tree->SetBranchAddress("zMass"                           , &zMassIn                        ) ;
  tree->SetBranchAddress("zLepOne"                         , &zLepOne                        ) ;
  tree->SetBranchAddress("zLepTwo"                         , &zLepTwo                        ) ;
  tree->SetBranchAddress("zLepOnePt"                       , &zLepOnePt                      ) ;
  tree->SetBranchAddress("zLepTwoPt"                       , &zLepTwoPt                      ) ;
  tree->SetBranchAddress("zLepOneEta"                      , &zLepOneEta                     ) ;
  tree->SetBranchAddress("zLepTwoEta"                      , &zLepTwoEta                     ) ;

}

//-----------------------------------------------------------------------------------------------------------------
// Initialize the outgoing TTree structure
void NanoAODConversion::InitializeOutBranchStructure(TTree* tree) {
  if(!leptonOneP4  ) leptonOneP4   = new TLorentzVector();
  if(!leptonTwoP4  ) leptonTwoP4   = new TLorentzVector();
  if(!leptonOneSVP4) leptonOneSVP4 = new TLorentzVector();
  if(!leptonTwoSVP4) leptonTwoSVP4 = new TLorentzVector();
  if(!jetP4        ) jetP4         = new TLorentzVector();
  if(!photonP4     ) photonP4      = new TLorentzVector();

  tree->Branch("runNumber"                     , &runNumber            );
  tree->Branch("evtNumber"                     , &eventNumber          );
  tree->Branch("lumiSection"                   , &lumiSection          );
  tree->Branch("nPV"                           , &nPV                  );
  tree->Branch("nPU"                           , &nPU                  );
  tree->Branch("nPUAdded"                      , &nPUAdded             );
  tree->Branch("nPartons"                      , &nPartons             );
  tree->Branch("mcEra"                         , &mcEra                );
  tree->Branch("triggerLeptonStatus"           , &triggerLeptonStatus  );
  tree->Branch("muonTriggerStatus"             , &muonTriggerStatus    );
  tree->Branch("eventWeight"                   , &eventWeight          );
  tree->Branch("genWeight"                     , &genWeight            );
  tree->Branch("puWeight"                      , &puWeight             );
  tree->Branch("btagWeight"                    , &btagWeight           );
  tree->Branch("lepOneWeight1"                 , &lepOneWeight1        );
  tree->Branch("lepOneWeight1_up"              , &lepOneWeight1_up     );
  tree->Branch("lepOneWeight1_down"            , &lepOneWeight1_down   );
  tree->Branch("lepOneWeight1_bin"             , &lepOneWeight1_bin    );
  tree->Branch("lepOneWeight2"                 , &lepOneWeight2        );
  tree->Branch("lepOneWeight2_up"              , &lepOneWeight2_up     );
  tree->Branch("lepOneWeight2_down"            , &lepOneWeight2_down   );
  tree->Branch("lepOneWeight2_bin"             , &lepOneWeight2_bin    );
  tree->Branch("lepTwoWeight1"                 , &lepTwoWeight1        );
  tree->Branch("lepTwoWeight1_up"              , &lepTwoWeight1_up     );
  tree->Branch("lepTwoWeight1_down"            , &lepTwoWeight1_down   );
  tree->Branch("lepTwoWeight1_bin"             , &lepTwoWeight1_bin    );
  tree->Branch("lepTwoWeight2"                 , &lepTwoWeight2        );
  tree->Branch("lepTwoWeight2_up"              , &lepTwoWeight2_up     );
  tree->Branch("lepTwoWeight2_down"            , &lepTwoWeight2_down   );
  tree->Branch("lepTwoWeight2_bin"             , &lepTwoWeight2_bin    );
  tree->Branch("lepOneTrigger"                 , &lepOneTrigger        );
  tree->Branch("lepTwoTrigger"                 , &lepTwoTrigger        );
  tree->Branch("lepOneTrigWeight"              , &lepOneTrigWeight     );
  tree->Branch("lepTwoTrigWeight"              , &lepTwoTrigWeight     );
  tree->Branch("lepOneFired"                   , &lepOneFired          );
  tree->Branch("lepTwoFired"                   , &lepTwoFired          );
  tree->Branch("nTrigModes"                    , &nTrigModes           );
  tree->Branch("triggerWeights"                , &triggerWeights, "triggerWeights[nTrigModes]/F");
  tree->Branch("topPtWeight"                   , &topPtWeight          );
  tree->Branch("zPtWeight"                     , &zPtWeight            );
  tree->Branch("zPt"                           , &zPtOut               );
  tree->Branch("zMass"                         , &zMassOut             );
  tree->Branch("zLepOnePt"                     , &zLepOnePt            );
  tree->Branch("zLepTwoPt"                     , &zLepTwoPt            );
  tree->Branch("zLepOneEta"                    , &zLepOneEta           );
  tree->Branch("zLepTwoEta"                    , &zLepTwoEta           );
  tree->Branch("embeddingWeight"               , &embeddingWeight      );
  tree->Branch("looseQCDSelection"             , &looseQCDSelection    );
  tree->Branch("genTauFlavorWeight"            , &genTauFlavorWeight   );
  tree->Branch("tauEnergyScale"                , &tauEnergyScale       );
  tree->Branch("tauES_up"                      , &tauES_up             );
  tree->Branch("tauES_down"                    , &tauES_down           );
  tree->Branch("tauES_bin"                     , &tauES_bin            );
  tree->Branch("tauDecayMode"                  , &tauDecayModeOut      );
  tree->Branch("tauMVA"                        , &tauMVA               );
  tree->Branch("tauGenID"                      , &tauGenIDOut          );
  tree->Branch("tauGenFlavor"                  , &tauGenFlavor         );
  tree->Branch("tauGenFlavorHad"               , &tauGenFlavorHad      );
  tree->Branch("tauVetoedJetPt"                , &tauVetoedJetPt       );
  tree->Branch("tauVetoedJetPtUnc"             , &tauVetoedJetPtUnc    );
  tree->Branch("leptonOneP4"                   , &leptonOneP4          );
  tree->Branch("leptonTwoP4"                   , &leptonTwoP4          );
  tree->Branch("leptonOneSCEta"                , &leptonOneSCEta       );
  tree->Branch("leptonTwoSCEta"                , &leptonTwoSCEta       );
  tree->Branch("leptonOneFlavor"               , &leptonOneFlavor      );
  tree->Branch("leptonTwoFlavor"               , &leptonTwoFlavor      );
  tree->Branch("leptonOneGenFlavor"            , &leptonOneGenFlavor   );
  tree->Branch("leptonTwoGenFlavor"            , &leptonTwoGenFlavor   );
  tree->Branch("leptonOneGenPt"                , &leptonOneGenPt       );
  tree->Branch("leptonTwoGenPt"                , &leptonTwoGenPt       );
  tree->Branch("leptonOneD0"                   , &leptonOneD0          );
  tree->Branch("leptonTwoD0"                   , &leptonTwoD0          );
  tree->Branch("leptonOneIso"                  , &leptonOneIso         );
  tree->Branch("leptonTwoIso"                  , &leptonTwoIso         );
  tree->Branch("leptonOneID1"                  , &leptonOneID1         );
  tree->Branch("leptonTwoID1"                  , &leptonTwoID1         );
  tree->Branch("leptonOneID2"                  , &leptonOneID2         );
  tree->Branch("leptonTwoID2"                  , &leptonTwoID2         );
  tree->Branch("leptonTwoID3"                  , &leptonTwoID3         );
  tree->Branch("leptonOneIndex"                , &leptonOneIndex       );
  tree->Branch("leptonTwoIndex"                , &leptonTwoIndex       );
  tree->Branch("leptonOnePtSF"                 , &leptonOnePtSF        );
  tree->Branch("leptonTwoPtSF"                 , &leptonTwoPtSF        );
  tree->Branch("leptonOneSkimIndex"            , &leptonOneSkimIndex   );
  tree->Branch("leptonTwoSkimIndex"            , &leptonTwoSkimIndex   );
  tree->Branch("leptonOneSkimFlavor"           , &leptonOneSkimFlavor  );
  tree->Branch("leptonTwoSkimFlavor"           , &leptonTwoSkimFlavor  );
  tree->Branch("genLeptonOneP4"                , &genLeptonOneP4       );
  tree->Branch("genLeptonTwoP4"                , &genLeptonTwoP4       );
  tree->Branch("photonP4"                      , &photonP4             );
  tree->Branch("photonIDWeight"                , &photonIDWeight       );
  tree->Branch("jetP4"                         , &jetP4                );
  tree->Branch("jetID"                         , &jetIDOut             );
  tree->Branch("jetPUID"                       , &jetPUIDOut           );
  tree->Branch("tauP4"                         , &tauP4                );
  tree->Branch("tauFlavor"                     , &tauFlavor            );
  tree->Branch("taudxy"                        , &taudxyOut            );
  tree->Branch("taudz"                         , &taudzOut             );
  tree->Branch("tauDeepAntiEle"                , &tauDeepAntiEle       );
  tree->Branch("tauDeepAntiMu"                 , &tauDeepAntiMu        );
  tree->Branch("tauDeepAntiJet"                , &tauDeepAntiJet       );
  tree->Branch("nMuons"                        , &nMuons               );
  tree->Branch("nMuonsNano"                    , &nMuon                );
  // tree->Branch("slimMuons"                     , &slimMuons);
  tree->Branch("nElectrons"                    , &nElectrons           );
  tree->Branch("nElectronsNano"                , &nElectron            );
  // tree->Branch("slimElectrons"                 , &slimElectrons);
  tree->Branch("nTaus"                         , &nTaus                );
  tree->Branch("nTausNano"                     , &nTau                 );
  // tree->Branch("slimTaus"                      , &slimTaus);
  tree->Branch("nPhotons"                      , &nPhotons             );
  tree->Branch("nPhotonsNano"                  , &nPhoton              );
  // tree->Branch("slimPhotons"                   , &slimPhotons          );
  tree->Branch("nJetsNano"                     , &nJet                 );
  // tree->Branch("slimJets"                      , &slimJets             );
  tree->Branch("nJets"                         , &nJets                );
  tree->Branch("nJets20"                       , &nJets20              );
  tree->Branch("nJets20Rej"                    , &nJets20Rej           );
  tree->Branch("nFwdJets"                      , &nFwdJets             );
  tree->Branch("nBJets"                        , &nBJets               );
  tree->Branch("nBJetsM"                       , &nBJetsM              );
  tree->Branch("nBJetsL"                       , &nBJetsL              );
  tree->Branch("nBJets20"                      , &nBJets20             );
  tree->Branch("nBJets20M"                     , &nBJets20M            );
  tree->Branch("nBJets20L"                     , &nBJets20L            );
  tree->Branch("nGenTaus"                      , &nGenTaus             );
  tree->Branch("nGenTausHad"                   , &nGenTausHad          );
  tree->Branch("nGenTausLep"                   , &nGenTausLep          );
  tree->Branch("nGenElectrons"                 , &nGenElectrons        );
  tree->Branch("nGenMuons"                     , &nGenMuons            );
  tree->Branch("nGenPromptTaus"                , &nGenHardTaus         );
  tree->Branch("nGenPromptElectrons"           , &nGenHardElectrons    );
  tree->Branch("nGenPromptMuons"               , &nGenHardMuons        );
  tree->Branch("htSum"                         , &htSum                );
  tree->Branch("ht"                            , &ht                   );
  tree->Branch("htPhi"                         , &htPhi                );
  // tree->Branch("pfMET"               , &pfMET                );
  // tree->Branch("pfMETphi"            , &pfMETphi             );
  // tree->Branch("pfMETCov00"          , &pfMETCov00           );
  // tree->Branch("pfMETCov01"          , &pfMETCov01           );
  // tree->Branch("pfMETCov11"          , &pfMETCov11           );
  tree->Branch("pfMETC"                        , &pfMETC               );
  tree->Branch("pfMETCphi"                     , &pfMETCphi            );
  tree->Branch("pfMETCCov00"                   , &pfMETCCov00          );
  tree->Branch("pfMETCCov01"                   , &pfMETCCov01          );
  tree->Branch("pfMETCCov11"                   , &pfMETCCov11          );
  tree->Branch("puppMET"                       , &puppMET              );
  tree->Branch("puppMETphi"                    , &puppMETphi           );
  tree->Branch("puppMETCov00"                  , &puppMETCov00         );
  tree->Branch("puppMETCov01"                  , &puppMETCov01         );
  tree->Branch("puppMETCov11"                  , &puppMETCov11         );
  tree->Branch("puppMETC"                      , &puppMETC             );
  tree->Branch("puppMETCphi"                   , &puppMETCphi          );
  // tree->Branch("puppMETCCov00"       , &puppMETCCov00        );
  // tree->Branch("puppMETCCov01"       , &puppMETCCov01        );
  // tree->Branch("puppMETCCov11"       , &puppMETCCov11        );
  // tree->Branch("alpacaMET"           , &alpacaMET            );
  // tree->Branch("alpacaMETphi"        , &alpacaMETphi         );
  // tree->Branch("pcpMET"              , &pcpMET               );
  // tree->Branch("pcpMETphi"           , &pcpMETphi            );
  tree->Branch("trkMET"                        , &trkMET               );
  tree->Branch("trkMETphi"                     , &trkMETphi            );
  tree->Branch("met"                           , &met                  );
  tree->Branch("metPhi"                        , &metPhi               );
  tree->Branch("metCorr"                       , &metCorr              );
  tree->Branch("metCorrPhi"                    , &metCorrPhi           );
  tree->Branch("covMet00"                      , &covMet00             );
  tree->Branch("covMet01"                      , &covMet01             );
  tree->Branch("covMet11"                      , &covMet11             );
  tree->Branch("massSVFit"                     , &massSVFit            );
  tree->Branch("massErrSVFit"                  , &massErrSVFit         );
  tree->Branch("svFitStatus"                   , &svFitStatus          );
  tree->Branch("leptonOneSVP4"                 , &leptonOneSVP4        );
  tree->Branch("leptonTwoSVP4"                 , &leptonTwoSVP4        );
  //information for b-tag efficiency studies
  tree->Branch("jetsPt"                        , jetsPt      , "jetsPt[nJets20]/F");
  tree->Branch("jetsEta"                       , jetsEta     , "jetsEta[nJets20]/F");
  tree->Branch("jetsFlavor"                    , jetsFlavor  , "jetsFlavor[nJets20]/I");
  tree->Branch("jetsBTag"                      , jetsBTag    , "jetsBTag[nJets20]/I");
  //information for PU ID efficiency studyes
  tree->Branch("jetsRejPt"                     , jetsPt      , "jetsPt[nJets20Rej]/F");
  tree->Branch("jetsRejEta"                    , jetsEta     , "jetsEta[nJets20Rej]/F");
  //information for fake tau estimation
  tree->Branch("tausPt"                        , tausPt        , "tausPt[nTaus]/F");
  tree->Branch("tausEta"                       , tausEta       , "tausEta[nTaus]/F");
  tree->Branch("tausIsPositive"                , tausIsPositive, "tausIsPositive[nTaus]/O");
  tree->Branch("tausDM"                        , tausDM        , "tausDM[nTaus]/I");
  tree->Branch("tausGenFlavor"                 , tausGenFlavor , "tausGenFlavor[nTaus]/I");
  tree->Branch("tausAntiJet"                   , tausAntiJet   , "tausAntiJet[nTaus]/b");
  tree->Branch("tausAntiMu"                    , tausAntiMu    , "tausAntiMu[nTaus]/b");
  tree->Branch("tausMVAAntiMu"                 , tausMVAAntiMu , "tausMVAAntiMu[nTaus]/b");
  tree->Branch("tausAntiEle"                   , tausAntiEle   , "tausAntiEle[nTaus]/b");
  tree->Branch("tausWeight"                    , tausWeight    , "tausWeight[nTaus]/F");
  //information for fake light lepton estimation
  tree->Branch("nExtraLep"                     , &nExtraLep);
  tree->Branch("leptonsPt"                     , leptonsPt        , "leptonsPt[nExtraLep]/F");
  tree->Branch("leptonsEta"                    , leptonsEta       , "leptonsEta[nExtraLep]/F");
  tree->Branch("leptonsIsPositive"             , leptonsIsPositive, "leptonsIsPositive[nExtraLep]/O");
  tree->Branch("leptonsIsMuon"                 , leptonsIsMuon    , "leptonsIsMuon[nExtraLep]/O");
  tree->Branch("leptonsID"                     , leptonsID        , "leptonsID[nExtraLep]/b");
  tree->Branch("leptonsIsoID"                  , leptonsIsoID     , "leptonsIsoID[nExtraLep]/b");
  tree->Branch("leptonsTriggered"              , leptonsTriggered , "leptonsTriggered[nExtraLep]/b");
  tree->Branch("leptonsGenFlavor"              , leptonsGenFlavor , "leptonsGenFlavor[nExtraLep]/b");
  //gen level info for splitting into jet-binned samples
  tree->Branch("LHE_Njets"                     , &LHE_Njets);
}

//-----------------------------------------------------------------------------------------------------------------
//calculate the weight for b-tagging corrections
float NanoAODConversion::BTagWeight(int WP) {
  //loop through each jet, getting the P(b-tag) in data and MC
  double p_data(1.), p_mc(1.);
  UInt_t nbjets = 0;
  if(fVerbose > 1) std::cout << "NanoAODConversion::" << __func__
                             << ": printing jet b-tag info for WP = "
                             << WP << "...\n";
  for(unsigned jet = 0; jet < nJets20; ++jet) {
    if(std::fabs(jetsEta[jet]) >= 2.4) continue;
    double pd = particleCorrections->BTagDataProb(jetsPt[jet], jetsEta[jet], jetsFlavor[jet], fYear, WP);
    double pm = particleCorrections->BTagMCProb  (jetsPt[jet], jetsEta[jet], jetsFlavor[jet], fYear, WP);
    if(fVerbose > 1) std::cout << "Jet " << jet << " has pt = " << jetsPt[jet] << " eta " << jetsEta[jet]
                               << " pmc = " << pm << " pdata = " << pd << " btagID = " << jetsBTag[jet]
                               << " with btagged = " << (jetsBTag[jet] > WP) << std::endl;
    if(jetsBTag[jet] > WP) { //is b-tagged
      ++nbjets;
      if(pd <= 0. || pm <= 0.) {
        pd = std::max(0.0001, pd);
        pm = std::max(0.0001, pm);
      }
      p_data *= pd;
      p_mc   *= pm;
    } else {
      if(pd >= 1. || pm >= 1.) {
        pd = std::min(0.9999, pd);
        pm = std::min(0.9999, pm);
      }
      p_data *= 1. - pd;
      p_mc   *= 1. - pm;
    }
    if(fVerbose > 1) std::cout << " --> p_mc = " << p_mc << " p_data = " << p_data << std::endl;
  }
  UInt_t nexpected = nBJets20;
  if     (WP == ParticleCorrections::kLooseBTag ) nexpected = nBJets20L;
  else if(WP == ParticleCorrections::kMediumBTag) nexpected = nBJets20M;
  if(nbjets != nexpected)
    std::cout << "Entry " << fentry << " !!! Warning! btag nbjets = " << nbjets
              << " doesn't equal expected " << nexpected << "!\n";

  if(fVerbose > 1 || p_data <= 0. || p_mc <= 0.) {
    if(p_data <= 0. || p_mc <= 0.) std::cout << "!!! Warning! <= 0 b-tag probabilities! ";
    std::cout << "NanoAODConversion::" << __func__ << " entry " << fentry << ": p_data = " << p_data
              << " p_mc = " << p_mc << " scale = " << p_data/p_mc
              << " njets = " << nJets20 << " nbjets = " << nbjets << std::endl;
  }
  if(p_mc <= 0. || p_data <= 0.) return 1.;
  float scale_factor = p_data / p_mc;
  return scale_factor;
}

//-----------------------------------------------------------------------------------------------------------------
// Initialize event variables for the given selection
void NanoAODConversion::InitializeEventVariables() {
  ////////////////////////////////////////////////////////////////////
  //   Generator/Embedding Weight
  ////////////////////////////////////////////////////////////////////
  //store sign of the generator weight, except in embedding weight where the value is meaningful --> move this to a new variable
  embeddingWeight = 1.;
  if(!fIsEmbed) {
    genWeight = (genWeight == 0.) ? 0. : genWeight/std::fabs(genWeight);
  } else {
    //if embedding weight is undefined, set to near-zero
    genWeight = std::fabs(genWeight);
    if(genWeight > 1.) {  //remove the undefined event by setting genWeight to 0
      genWeight = 0.;
    } else if(genWeight == 1.) {
      std::cout << "!!! Warning! Entry = " << fentry << ": Unit input embedding weight = " << genWeight
                << " in event = " << eventNumber << " lumi = " << lumiSection << " run = " << runNumber
                << std::endl;
      genWeight = 0.;
    } else { //move the weight value to the embeddingWeight variable
      embeddingWeight = genWeight;
      genWeight = 1.;
    }
    if(embeddingWeight >= 1. && genWeight > 0.5) {
      std::cout << "!!! Warning! Entry = " << fentry << ": Large embedding weight = " << embeddingWeight
                << " with non-zero genWeight = " << genWeight
                << " in event = " << eventNumber << " lumi = " << lumiSection << " run = " << runNumber
                << std::endl;
    }
  }

  ////////////////////////////////////////////////////////////////////
  //   Z info + re-weight
  ////////////////////////////////////////////////////////////////////
  //Z pT/mass info (DY and Z signals only)
  zPtOut = zPtIn;
  zMassOut = zMassIn;
  if(fIsDY) {
    //re-weight if pt/mass are defined
    if(zPtOut > 0. && zMassOut > 0.) zPtWeight = particleCorrections->ZWeight(zPtOut, zMassOut, fYear);
    //store generator level Z->ll lepton ids
    nGenHardElectrons = 0; nGenHardMuons = 0; nGenHardTaus = 0;
    //lepton one
    if(abs(zLepOne) == 11)      ++nGenHardElectrons;
    else if(abs(zLepOne) == 13) ++nGenHardMuons;
    else if(abs(zLepOne) == 15) ++nGenHardTaus;
    //lepton two
    if(abs(zLepTwo) == 11)      ++nGenHardElectrons;
    else if(abs(zLepTwo) == 13) ++nGenHardMuons;
    else if(abs(zLepTwo) == 15) ++nGenHardTaus;
  }

}

//-----------------------------------------------------------------------------------------------------------------
// Initialize event variables for the given selection
void NanoAODConversion::InitializeTreeVariables(Int_t selection) {
  eventWeight = 1.; //reset the overall event weight

  //reset to-be-defined weights
  lepOneWeight1      = 1.; lepTwoWeight1      = 1.;
  lepOneWeight1_up   = 1.; lepTwoWeight1_up   = 1.;
  lepOneWeight1_down = 1.; lepTwoWeight1_down = 1.;
  lepOneWeight1_bin  = 0 ; lepTwoWeight1_bin  = 0 ;

  lepOneWeight2      = 1.; lepTwoWeight2      = 1.;
  lepOneWeight2_up   = 1.; lepTwoWeight2_up   = 1.;
  lepOneWeight2_down = 1.; lepTwoWeight2_down = 1.;
  lepOneWeight2_bin  = 0 ; lepTwoWeight2_bin  = 0 ;

  lepOneTrigWeight   = 1.; lepTwoTrigWeight   = 1.;
  lepOneFired        = 0 ; lepTwoFired        = 0 ;

  leptonOnePtSF = 1.; leptonTwoPtSF = 1.;

  tauEnergyScale = 1.; tauES_up = 1.; tauES_down = 1.;
  eleEnergyScale = 1.; eleES_up = 1.; eleES_down = 1.;

  //////////////////////////////
  //           MET            //
  //////////////////////////////

  //use PUPPI MET by default, update by selection due to any MET corrections after object pT corrections
  met = puppMET;
  metPhi = puppMETphi;
  //set corrected PUPPI MET to be nominal
  puppMETC    = puppMET   ;
  puppMETCphi = puppMETphi;


  //////////////////////////////
  //      Lepton info         //
  //////////////////////////////
  nMuons     = fNMuons    [selection];
  nElectrons = fNElectrons[selection];
  nTaus      = fNTaus     [selection];
  nPhotons   = fNPhotons  [selection]; //also set N(photons) variable
  if(fVerbose > 1) std::cout << "nElectrons = " << nElectrons
                             << " nMuons = " << nMuons
                             << " nTaus = " << nTaus
                             << " nPhotons = "  << nPhotons
                             << std::endl;

  //////////////////////////////
  //        Trigger           //
  //////////////////////////////
  bool useMu50 = true; //whether or not to consider the Mu50 trigger as an OR with IsoMu

  //store muon and electron trigger (1 = electron, 2 = muon, 3 = both)
  //muon trigger is Mu50 for all years and IsoMu24 for 2016, 2018 and IsoMu27 for 2017
  bool lowMuonTriggered  = nMuons > 0 && ((fYear == ParticleCorrections::k2016 && HLT_IsoMu24) ||
                                          (fYear == ParticleCorrections::k2017 && HLT_IsoMu27) ||
                                          (fYear == ParticleCorrections::k2018 && HLT_IsoMu24));
  bool highMuonTriggered = useMu50 && nMuons > 0 && HLT_Mu50;

  double muon_lo_pt = (fYear == ParticleCorrections::k2017) ? 28. : 25.;
  double muon_hi_pt = 50.; //independent of year
  bool electronTriggered = nElectrons > 0 &&
    ((fYear == ParticleCorrections::k2016 && HLT_Ele27_WPTight_GsF) ||
     (fYear == ParticleCorrections::k2017 && HLT_Ele32_WPTight_GsF_L1DoubleEG) || //FIXME: add L1 Ele35 test as well
     (fYear == ParticleCorrections::k2018 && HLT_Ele32_WPTight_GsF));
  double elec_pt = (fYear == ParticleCorrections::k2016) ? 28. : 33.;

  triggerLeptonStatus = electronTriggered + 2*(lowMuonTriggered || highMuonTriggered); //bit 1: electron bit 2: muon
  muonTriggerStatus   = lowMuonTriggered + 2*highMuonTriggered; //store which triggers were fired

  bool useTrigObj = nTrigObjs > 0; //do actual trigger matching if trigger objects are available
  if(!useTrigObj) std::cout << "!!! NanoAODConversion::" << __func__
                            << ": Warning! No trigger objects available in event "
                            << fentry << std::endl;

  int trigMatchOne =  0; //trigger matching result
  int trigMatchTwo =  0; //trigger matching result
  int trigIndexOne = -1;
  int trigIndexTwo = -1;

  int trigger = -1; //muon weights need to know which trigger to use
  if(selection == kMuTau || selection == kEMu || selection == kMuMu) {
    //default to low trigger if it passed it, < 0 if no trigger
    trigger = (lowMuonTriggered) ? ParticleCorrections::kLowTrigger : (2*highMuonTriggered - 1)*ParticleCorrections::kHighTrigger;
  }
  if(!lowMuonTriggered && !highMuonTriggered && (selection == kMuMu || selection == kMuTau) && !fIgnoreNoTriggerMatch) {
    std::cout << "!!! NanoAODConversion::" << __func__
              << " Warning! Muon only selection but no identified muon trigger in event "
              << fentry << std::endl;
  } else if(!electronTriggered && (selection == kEE || selection == kETau) && !fIgnoreNoTriggerMatch) {
    std::cout << "!!! NanoAODConversion::" << __func__
              << " Warning! Electron only selection but no identified electron trigger in event "
              << fentry << std::endl;
  } else if(!electronTriggered && !lowMuonTriggered && !highMuonTriggered && !fIgnoreNoTriggerMatch) {
    std::cout << "!!! NanoAODConversion::" << __func__
              << " Warning! Passed a selection but no identified trigger in event "
              << fentry << std::endl;
  }

  //for multiple potential triggers fired, consider efficiencies instead of scale factors
  float data_eff[]  = {0.5, 0.5};
  float mc_eff[]    = {0.5, 0.5};
  bool trig_fired[] = {false, false};
  int trigger_index = 0;

  //reset lepton IDs
  leptonOneID1 =  0 ; leptonOneID2 =  0 ;
  leptonTwoID1 =  0 ; leptonTwoID2 =  0 ; leptonTwoID3 = 0;
  leptonOneIso = -1.; leptonTwoIso = -1.;
  leptonOneD0  =  0.; leptonTwoD0  =  0.;

  //////////////////////////////
  //      lep 1 = muon        //
  //////////////////////////////
  if(selection == kMuTau || selection == kMuMu) {
    unsigned index = (selection == kMuMu && nMuons > 2) ? leptonOneIndex : fMuonIndices[selection][0];
    leptonOneP4->SetPtEtaPhiM(muonPt[index], muonEta[index],
                              muonPhi[index], muonMass[index]);
    leptonOneSCEta = muonEta[index];
    leptonOneFlavor = -13*muonCharge[index];
    leptonOneID1 = muonIsoId[index];
    leptonOneID2 = 0;
    leptonOneIso = muonRelIso[index]*muonPt[index];
    leptonOneD0 = sqrt(muondxy[index]*muondxy[index] + muondz[index]*muondz[index]);
    leptonOnePtSF = muonRoccoSF[index];
    leptonOneGenFlavor = (!fIsData) ? MuonFlavorFromID(muonGenFlavor[index]) : 0;
    leptonOneIndex = index;
    trigMatchOne = GetTriggerMatch(index, true, trigIndexOne);
    if(trigMatchOne == 2 && !highMuonTriggered) trigMatchOne = 0; //if not using high muon trigger, ignore a match to it
    if(useTrigObj) {
      if(trigMatchOne == 0) trigger = -1;
      else                  trigger = (trigMatchOne == 1 || trigMatchOne == 3) ? ParticleCorrections::kLowTrigger : ParticleCorrections::kHighTrigger;
    }
    if(!fIsData) {
      particleCorrections->MuonWeight(muonPt[index], muonEta[index], trigger, fYear, lepOneTrigWeight,
                                      lepOneWeight1, lepOneWeight1_up, lepOneWeight1_down, lepOneWeight1_bin,
                                      lepOneWeight2, lepOneWeight2_up, lepOneWeight2_down, lepOneWeight2_bin
                                      );
      particleCorrections->MuonTriggerEff(muonPt[index], muonEta[index], (trigger < 0) ? ParticleCorrections::kLowTrigger : trigger,
                                          fYear, data_eff[trigger_index], mc_eff[trigger_index]);
      lepOneTrigWeight = (trigMatchOne > 0) ? data_eff[trigger_index] / mc_eff[trigger_index] : (1. - data_eff[trigger_index]) / (1. - mc_eff[trigger_index]);
      if(fVerbose > 2) std::cout << "Lepton 1 has trigger status " << trigger << " and trigger weight " << lepOneTrigWeight << std::endl;
    }
    if(lowMuonTriggered || highMuonTriggered)
      lepOneFired = (useTrigObj) ? trigMatchOne > 0 : (lowMuonTriggered && muonPt[index] > muon_lo_pt) || (highMuonTriggered && muonPt[index] > muon_hi_pt);
    trig_fired[trigger_index] = lepOneFired;
    ++trigger_index;
  //////////////////////////////
  //     lep 1 = electron     //
  //////////////////////////////
  } else if(selection == kETau || selection == kEMu || selection == kEE) {
    unsigned index = (selection == kEE && nElectrons > 2) ? leptonOneIndex : fElectronIndices[selection][0];
    leptonOneP4->SetPtEtaPhiM(electronPt[index], electronEta[index],
                              electronPhi[index], electronMass[index]);
    leptonOneSCEta = electronEta[index] + electronDeltaEtaSC[index];
    leptonOneFlavor = -11*electronCharge[index];
    leptonOneID1 = electronWPL[index] + 2*electronWP90[index] + 4*electronWP80[index];
    leptonOneID2 = 0;
    leptonOneIso = electronRelIso[index]*electronPt[index];
    leptonOneD0 = sqrt(electrondxy[index]*electrondxy[index] + electrondz[index]*electrondz[index]);
    leptonOneGenFlavor = (!fIsData) ? ElectronFlavorFromID(electronGenFlavor[index]) : 0;
    leptonOneIndex = index;
    trigMatchOne = GetTriggerMatch(index, false, trigIndexOne);
    if(!fIsData) {
      particleCorrections->ElectronWeight(electronPt[index], electronEta[index]+electronDeltaEtaSC[index], fYear, lepOneTrigWeight,
                                          lepOneWeight1, lepOneWeight1_up, lepOneWeight1_down, lepOneWeight1_bin,
                                          lepOneWeight2, lepOneWeight2_up, lepOneWeight2_down, lepOneWeight2_bin
                                          );
      particleCorrections->ElectronTriggerEff(electronPt[index], electronEta[index]+electronDeltaEtaSC[index], fYear,
                                              data_eff[trigger_index], mc_eff[trigger_index]);
      lepOneTrigWeight = (trigMatchOne) ? data_eff[trigger_index] / mc_eff[trigger_index] : (1. - data_eff[trigger_index]) / (1. - mc_eff[trigger_index]);
      eleEnergyScale = particleCorrections->ElectronEnergyScale(electronPt[index], electronEta[index]+electronDeltaEtaSC[index], fYear,
                                                                eleES_up, eleES_down);
      if(fIsEmbed) { //remove given energy correction
        const float eCorr = electronECorr[index];
        if(eCorr > 0.f) *leptonOneP4 *= 1./eCorr;
      }
      *leptonOneP4 *= eleEnergyScale;

      if(fVerbose > 2) std::cout << "Lepton 1 has trigger status " << trigger << " and trigger weight " << lepOneTrigWeight << std::endl;
    }
    if(electronTriggered)
      lepOneFired = (useTrigObj) ? trigMatchOne > 0 : electronPt[index] > elec_pt;
    trig_fired[trigger_index] = lepOneFired;
    ++trigger_index;
    if(fVerbose > 9) std::cout << "Lepton one found as an electron with index " << index
                               << " ID1 " << leptonOneID1 << " ID2 " << leptonOneID2
                               << " trigMatch " << trigMatchOne << std::endl;
  }
  //////////////////////////////
  //      lep 2 = tau         //
  //////////////////////////////
  if(selection == kMuTau || selection == kETau) {
    unsigned index = fTauIndices[selection][0];
    leptonTwoP4->SetPtEtaPhiM(tauPt[index], tauEta[index],
                              tauPhi[index],tauMass[index]);
    leptonTwoSCEta = tauEta[index];
    //FIXME: should apply energy scale correction to all taus
    tauEnergyScale = (fIsData == 0) ? particleCorrections->TauEnergyScale(leptonTwoP4->Pt(), leptonTwoP4->Eta(), tauDecayMode[index],
                                                                          tauGenID[index], tauDeep2017VsJet[index], fYear, tauES_up, tauES_down, tauES_bin) : 1.;
    if(tauEnergyScale <= 0.)
      std::cout << "WARNING! In event " << fentry
                << " tau energy scale <= 0 = " << tauEnergyScale
                << std::endl;
    *leptonTwoP4 *= tauEnergyScale;

    leptonTwoFlavor = -15*tauCharge[index];
    tauGenIDOut  = tauGenID[index];
    tauGenFlavor = TauFlavorFromID((int) tauGenIDOut);
    leptonTwoGenFlavor = tauGenFlavor;
    lepTwoWeight1 = (fIsData == 0) ? particleCorrections->TauWeight(leptonTwoP4->Pt(), leptonTwoP4->Eta(), tauGenID[index], tauDeep2017VsJet[index], fYear,
                                                                    lepTwoWeight1_up, lepTwoWeight1_down, lepTwoWeight1_bin) : 1.;
    leptonTwoID1 = tauAntiEle[index]; //MVA ID
    leptonTwoID2 = tauAntiMu [index]; //MVA ID
    leptonTwoID3 = tauAntiJet[index]; //MVA ID
    taudxyOut = taudxy[index];
    taudzOut  = taudz [index];
    leptonTwoD0 = sqrt(taudxy[index]*taudxy[index] + taudz[index]*taudz[index]);
    leptonTwoIndex = index;
    tauDecayModeOut = tauDecayMode[index];

    tauDeepAntiEle = tauDeep2017VsE  [index];
    tauDeepAntiMu  = tauDeep2017VsMu [index];
    tauDeepAntiJet = tauDeep2017VsJet[index];
  //////////////////////////////
  //      lep 2 = muon        //
  //////////////////////////////
  } else if(selection == kEMu) {
    unsigned index = fMuonIndices[selection][0];
    leptonTwoP4->SetPtEtaPhiM(muonPt[index], muonEta[index],
                              muonPhi[index],muonMass[index]);
    leptonTwoSCEta = muonEta[index];
    leptonTwoFlavor = -13*muonCharge[index];
    leptonTwoID1 = muonIsoId[index];
    leptonTwoID2 = 0;
    leptonTwoIso = muonRelIso[index]*muonPt[index];
    leptonTwoD0 = sqrt(muondxy[index]*muondxy[index] + muondz[index]*muondz[index]);
    leptonTwoPtSF = muonRoccoSF[index];
    leptonTwoGenFlavor = (!fIsData) ? MuonFlavorFromID(muonGenFlavor[index]) : 0;
    leptonTwoIndex = index;
    trigMatchTwo = GetTriggerMatch(index, true, trigIndexTwo);
    if(trigMatchTwo == 2 && !highMuonTriggered) trigMatchTwo = 0; //if not using high muon trigger, ignore a match to it
    if(useTrigObj) {
      if(trigMatchTwo == 0) trigger = -1;
      else                  trigger = (trigMatchTwo == 1 || trigMatchTwo == 3) ? ParticleCorrections::kLowTrigger : ParticleCorrections::kHighTrigger;
    }
    if(!fIsData) {
      particleCorrections->MuonWeight(muonPt[index], muonEta[index], trigger, fYear, lepTwoTrigWeight,
                                      lepTwoWeight1, lepTwoWeight1_up, lepTwoWeight1_down, lepTwoWeight1_bin,
                                      lepTwoWeight2, lepTwoWeight2_up, lepTwoWeight2_down, lepTwoWeight2_bin
                                      );
      particleCorrections->MuonTriggerEff(muonPt[index], muonEta[index], (trigger < 0) ? ParticleCorrections::kLowTrigger : trigger,
                                          fYear, data_eff[trigger_index], mc_eff[trigger_index]);
      lepTwoTrigWeight = (trigMatchTwo) ? data_eff[trigger_index] / mc_eff[trigger_index] : (1. - data_eff[trigger_index]) / (1. - mc_eff[trigger_index]);
      if(fVerbose > 2) std::cout << "Lepton 2 has trigger status " << trigger << " and trigger weight " << lepTwoTrigWeight << std::endl;
    }
    if(lowMuonTriggered || highMuonTriggered)
      lepTwoFired = (useTrigObj) ? trigMatchTwo > 0 : (lowMuonTriggered && muonPt[index] > muon_lo_pt) || (highMuonTriggered && muonPt[index] > muon_hi_pt);
    else if(trigMatchTwo > 0) {
      std::cout << "!!! Warning! Entry " << fentry << " has no muon trigger but muon matched to a trigger with bits = " << trigMatchTwo
                << " HLT_IsoMu24 = " << HLT_IsoMu24 << " HLT_IsoMu27 = " << HLT_IsoMu27 << " HLT_Mu50 = " << HLT_Mu50 << std::endl;
    }
    trig_fired[trigger_index] = lepTwoFired;
    ++trigger_index;
  //////////////////////////////
  //      lep 2 = muon(2)     //
  //////////////////////////////
  } else if(selection == kMuMu) {
    unsigned index = (nMuons > 2) ? leptonTwoIndex : fMuonIndices[selection][1];
    leptonTwoP4->SetPtEtaPhiM(muonPt[index], muonEta[index],
                              muonPhi[index], muonMass[index]);
    leptonTwoSCEta = muonEta[index];
    leptonTwoFlavor = -13*muonCharge[index];
    leptonTwoID1 = muonIsoId[index];
    leptonTwoID2 = 0;
    leptonTwoIso = muonRelIso[index]*muonPt[index];
    leptonTwoD0 = sqrt(muondxy[index]*muondxy[index] + muondz[index]*muondz[index]);
    leptonTwoPtSF = muonRoccoSF[index];
    leptonTwoGenFlavor = (!fIsData) ? MuonFlavorFromID(muonGenFlavor[index]) : 0;
    leptonTwoIndex = index;
    trigMatchTwo = GetTriggerMatch(index, true, trigIndexTwo);
    if(trigMatchTwo == 2 && !highMuonTriggered) trigMatchTwo = 0; //if not using high muon trigger, ignore a match to it
    if(useTrigObj) {
      if(trigMatchTwo == 0) trigger = -1;
      else                  trigger = (trigMatchTwo == 1 || trigMatchTwo == 3) ? ParticleCorrections::kLowTrigger : ParticleCorrections::kHighTrigger;
    }
    if(!fIsData) {
      particleCorrections->MuonWeight(muonPt[index], muonEta[index], trigger, fYear, lepTwoTrigWeight,
                                      lepTwoWeight1, lepTwoWeight1_up, lepTwoWeight1_down, lepTwoWeight1_bin,
                                      lepTwoWeight2, lepTwoWeight2_up, lepTwoWeight2_down, lepTwoWeight2_bin
                                      );
      particleCorrections->MuonTriggerEff(muonPt[index], muonEta[index], (trigger < 0) ? ParticleCorrections::kLowTrigger : trigger,
                                          fYear, data_eff[trigger_index], mc_eff[trigger_index]);
      lepTwoTrigWeight = (trigMatchTwo) ? data_eff[trigger_index] / mc_eff[trigger_index] : (1. - data_eff[trigger_index]) / (1. - mc_eff[trigger_index]);
      if(fVerbose > 2) std::cout << "Lepton 2 has trigger status " << trigger << " and trigger weight " << lepTwoTrigWeight << std::endl;
    }
    if(lowMuonTriggered || highMuonTriggered)
      lepTwoFired = (useTrigObj) ? trigMatchTwo > 0 : (lowMuonTriggered && muonPt[index] > muon_lo_pt) || (highMuonTriggered && muonPt[index] > muon_hi_pt);
    trig_fired[trigger_index] = lepTwoFired;
    ++trigger_index;
  //////////////////////////////
  //    lep 2 = electron(2)   //
  //////////////////////////////
  } else if(selection == kEE) {
    unsigned index = (nElectrons > 2) ? leptonTwoIndex : fElectronIndices[selection][1];
    leptonTwoP4->SetPtEtaPhiM(electronPt[index], electronEta[index],
                              electronPhi[index], electronMass[index]);
    leptonTwoSCEta = electronEta[index] + electronDeltaEtaSC[index];
    leptonTwoFlavor = -11*electronCharge[index];
    leptonTwoID1 = electronWPL[index] + 2*electronWP90[index] + 4*electronWP80[index];
    leptonTwoID2 = 0;
    leptonTwoIso = electronRelIso[index]*electronPt[index];
    leptonTwoD0 = sqrt(electrondxy[index]*electrondxy[index] + electrondz[index]*electrondz[index]);
    trigMatchTwo = GetTriggerMatch(index, false, trigIndexTwo);
    leptonTwoGenFlavor = (!fIsData) ? ElectronFlavorFromID(electronGenFlavor[index]) : 0;
    leptonTwoIndex = index;
    if(!fIsData) {
      particleCorrections->ElectronWeight(electronPt[index], electronEta[index]+electronDeltaEtaSC[index], fYear, lepTwoTrigWeight,
                                          lepTwoWeight1, lepTwoWeight1_up, lepTwoWeight1_down, lepTwoWeight1_bin,
                                          lepTwoWeight2, lepTwoWeight2_up, lepTwoWeight2_down, lepTwoWeight2_bin
                                          );
      particleCorrections->ElectronTriggerEff(electronPt[index], electronEta[index]+electronDeltaEtaSC[index], fYear,
                                              data_eff[trigger_index], mc_eff[trigger_index]);
      lepTwoTrigWeight = (trigMatchTwo) ? data_eff[trigger_index] / mc_eff[trigger_index] : (1. - data_eff[trigger_index]) / (1. - mc_eff[trigger_index]);
      eleEnergyScale = particleCorrections->ElectronEnergyScale(electronPt[index], electronEta[index]+electronDeltaEtaSC[index], fYear,
                                                                eleES_up, eleES_down);
      if(fIsEmbed) { //remove given energy correction
        const float eCorr = electronECorr[index];
        if(eCorr > 0.f) *leptonTwoP4 *= 1./eCorr;
      }
      *leptonTwoP4 *= eleEnergyScale;
      if(fVerbose > 2) std::cout << "Lepton 2 has trigger status " << trigger << " and trigger weight " << lepTwoTrigWeight << std::endl;
    }
    if(electronTriggered)
      lepTwoFired = (useTrigObj) ? trigMatchTwo > 0 : electronPt[index] > elec_pt;
    trig_fired[trigger_index] = lepTwoFired;
    ++trigger_index;
    if(fVerbose > 9) std::cout << "Lepton two found as an electron with index " << index
                               << " ID1 " << leptonTwoID1 << " ID2 " << leptonTwoID2
                               << " trigMatch " << trigMatchTwo << std::endl;
  }
  // //don't scale trigger leptons if they didn't fire a trigger...
  // if(!lepOneFired) lepOneTrigWeight = 1.;
  // if(!lepTwoFired) lepTwoTrigWeight = 1.;

  if((selection == kEE || selection == kMuMu) && leptonOneIndex == leptonTwoIndex)
    std::cout << "!!! Warning! Entry " << fentry << ": Check point 1: Same flavor channel has repeated indices: "
              << leptonOneIndex << " " << leptonTwoIndex << std::endl;

  if(trigIndexOne == trigIndexTwo && (lepOneFired || lepTwoFired)) {
    std::cout << "!!! Warning! Entry " << fentry << ": Both leptons are matched to the same trigger index " << trigIndexOne << " selection "
              << fSelecNames[selection].Data() << std::endl;
  }
  //////////////////////////////
  //        b-Tag SF          //
  //////////////////////////////

  CountJets(selection);
  if(!fIsData) {
    int WP = ParticleCorrections::kTightBTag;
    if(selection == kEMu || selection == kEE || selection == kMuMu)
      WP = ParticleCorrections::kLooseBTag;
    btagWeight = BTagWeight(WP);
  } else btagWeight = 1.;

  if((selection == kEE || selection == kMuMu) && leptonOneIndex == leptonTwoIndex)
    std::cout << "!!! Warning! Entry " << fentry << ": Check point 2: Same flavor channel has repeated indices: "
              << leptonOneIndex << " " << leptonTwoIndex << std::endl;

  //////////////////////////////
  //    Store other objects   //
  //////////////////////////////
  if(!(selection == kMuTau || selection == kETau)) {
    if(nTaus > 0) {
      const unsigned index = fTauIndices[selection][0];
      tauP4->SetPtEtaPhiM(tauPt[index], tauEta[index], tauPhi[index], tauMass[index]);
      taudxyOut = taudxy[index];
      taudzOut  = taudz[index];
      tauFlavor = -15*tauCharge[index];
    } else {
      tauP4->SetPtEtaPhiM(0., 0., 0., 0.);
      taudxyOut = 0.;
      taudzOut  = 0.;
      tauFlavor = 0;
    }
  }
  CountTaus(selection);
  CountLightLeptons(selection);

  if((selection == kEE || selection == kMuMu) && leptonOneIndex == leptonTwoIndex)
    std::cout << "!!! Warning! Entry " << fentry << ": Check point 3: Same flavor channel has repeated indices: "
              << leptonOneIndex << " " << leptonTwoIndex << std::endl;

  lepOneTrigger = trigMatchOne;
  lepTwoTrigger = trigMatchTwo;

  //correct the trigger status using the trigger matching information
  if(selection == kEE) {
    triggerLeptonStatus = (lepOneFired || lepTwoFired);
    muonTriggerStatus = 0;
  } else if(selection == kMuMu) {
    triggerLeptonStatus = 2*(lepOneFired || lepTwoFired);
    muonTriggerStatus = lepOneTrigger | lepTwoTrigger; //OR of bits 1 and 2
  } else if(selection == kEMu) {
    triggerLeptonStatus = lepOneFired + 2*lepTwoFired;
    muonTriggerStatus = lepTwoTrigger;
  } else if(selection == kMuTau) {
    triggerLeptonStatus = 2*lepOneFired;
    muonTriggerStatus = lepOneTrigger;
  } else if(selection == kETau) {
    triggerLeptonStatus = lepOneFired;
    muonTriggerStatus = 0;
  }

  //for ee/mumu, ensure lepton one is higher pT
  if((selection == kEE || selection == kMuMu) && leptonOneP4->Pt() < leptonTwoP4->Pt()) {
    //swap momenta
    TLorentzVector ltmp = *leptonOneP4;
    leptonOneP4->SetPtEtaPhiM(leptonTwoP4->Pt(), leptonTwoP4->Eta(), leptonTwoP4->Phi(), leptonTwoP4->M());
    leptonTwoP4->SetPtEtaPhiM(ltmp.Pt(), ltmp.Eta(), ltmp.Phi(), ltmp.M());
    //swap flavor
    Int_t ftmp = leptonOneFlavor;
    leptonOneFlavor = leptonTwoFlavor;
    leptonTwoFlavor = ftmp;
    //swap weights
    Float_t wtmp = lepOneWeight1;
    lepOneWeight1 = lepTwoWeight1;
    lepTwoWeight1 = wtmp;
    wtmp = lepOneTrigWeight;
    lepOneTrigWeight = lepTwoTrigWeight;
    lepTwoTrigWeight = wtmp;
    wtmp = lepOneWeight1_up;
    lepOneWeight1_up = lepTwoWeight1_up;
    lepTwoWeight1_up = wtmp;
    wtmp = lepOneWeight1_down;
    lepOneWeight1_down = lepTwoWeight1_down;
    lepTwoWeight1_down = wtmp;
    wtmp = leptonOneSCEta;
    leptonOneSCEta = leptonTwoSCEta;
    leptonTwoSCEta = wtmp;
    //swap trig bools
    Bool_t ttmp = lepOneFired;
    lepOneFired = lepTwoFired;
    lepTwoFired = ttmp;
    //swap indices
    Int_t itmp = leptonOneIndex;
    leptonOneIndex = leptonTwoIndex;
    leptonTwoIndex = itmp;
    //swap trigger match
    itmp = lepOneTrigger;
    lepOneTrigger = lepTwoTrigger;
    lepTwoTrigger = itmp;
  }

  if((selection == kEE || selection == kMuMu) && leptonOneIndex == leptonTwoIndex)
    std::cout << "!!! Warning! Entry " << fentry << ": Check point 4: Same flavor channel has repeated indices: "
              << leptonOneIndex << " " << leptonTwoIndex << std::endl;

  //A Mu50 event
  if(muonTriggerStatus == 2) {
    if(fVerbose > 2)
      std::cout << "Entry " << fentry << " has Mu50 event: " << (int) lepOneTrigger << " " << (int) lepTwoTrigger << " status " << muonTriggerStatus << std::endl;
    ++fNHighMuonTriggered;
  }

  if(nPhotons > 0) {
    const unsigned index = fPhotonIndices[selection][0];
    photonP4->SetPtEtaPhiM(photonPt [index], photonEta [index],
                           photonPhi[index], photonMass[index]);
    if(!fIsData) photonIDWeight = particleCorrections->PhotonWeight(photonP4->Pt(), photonP4->Eta(), fYear);
    else         photonIDWeight = 1.;
  } else {
    photonP4->SetPtEtaPhiM(0., 0., 0., 0.);
    photonIDWeight = 1.;
  }

  //////////////////////////////
  //  Add trigger/MC weights  //
  //////////////////////////////
  if(!fIsData) {
    //establish the overall trigger weight for all modes
    if(lepOneFired && lepTwoFired) {
      //use the muon trigger by default, picking the higher pT lepton if both are the same flavor
      if(abs(leptonOneFlavor) == 13 || abs(leptonTwoFlavor) != 13) {
        triggerWeights[0] = lepOneTrigWeight;
      } else { //second one is a muon and first is an electron --> use muon
        triggerWeights[0] = lepTwoTrigWeight;
      }
    } else if(lepOneFired) {
      triggerWeights[0] = lepOneTrigWeight;
    } else if(lepTwoFired) {
      triggerWeights[0] = lepTwoTrigWeight;
    } else {
      triggerWeights[0] = 1.f; //no trigger fired, will be removed anyway
    }
    //probability of at least one triggering
    triggerWeights[1] = (1. - (1.-data_eff[0])*(1.-data_eff[1]))/(1. - (1. - mc_eff[0])*(1. - mc_eff[1]));
    if(triggerWeights[1] > 10.) std::cout << "!!! Warning! Entry " << fentry << " selection " << fSelecNames[selection].Data()
                                          << " has large triggerWeights[1] = " << triggerWeights[1]
                                          << " data_eff = (" << data_eff[0] << ", " << data_eff[1]
                                          << ") mc_eff = (" << mc_eff[0] << ", " << mc_eff[1] << ")\n";
    //probability of this event
    triggerWeights[2] = 1.f;
    triggerWeights[2] *= (trig_fired[0]) ? data_eff[0]/mc_eff[0] : (1. - data_eff[0]) / (1. - mc_eff[0]);
    triggerWeights[2] *= (trig_fired[1]) ? data_eff[1]/mc_eff[1] : (1. - data_eff[1]) / (1. - mc_eff[1]);

    if((lepOneFired || lepTwoFired) && leptonTwoP4->Pt() < 20. && std::fabs(triggerWeights[2] - triggerWeights[0]) > 0.001) {
      std::cout << "!!! Warning! Entry " << fentry << " trigger weight methods disagree (0,1,2): ("
                << triggerWeights[0] << ", " << triggerWeights[1] << ", " << triggerWeights[2] << ") data_eff = ("
                << data_eff[0] << ", " << data_eff[1] << ") mc_eff = ("
                << mc_eff[0] << ", " << mc_eff[1] << ") fired = ("
                << trig_fired[0] << ", " << trig_fired[1] << ")\n";
    }

    if((lepOneFired && leptonOneP4->Pt() < muon_lo_pt) || (lepTwoFired && leptonTwoP4->Pt() < muon_lo_pt)) {
      std::cout << "!!! Warning! Entry " << fentry << " has leptons firing triggers below threshold, 1: "
                << leptonOneP4->Pt() << " 2: " << leptonTwoP4->Pt() << " selection " << fSelecNames[selection].Data()
                << std::endl;
    }

    const int triggerMode = 0; //fix a trigger weights mode
    if(triggerMode == 0) { //use only 1 of the triggers, defaulting to muon
      if(lepOneFired && lepTwoFired) {
        //use the muon trigger by default, picking the higher pT lepton if both are the same flavor
        if(abs(leptonOneFlavor) == 13 || abs(leptonTwoFlavor) != 13) {
          lepTwoTrigWeight = 1.;
        } else { //second one is a muon and first is an electron --> use muon
          lepOneTrigWeight = 1.;
        }
      } else { //use weight from the firing lepton, set other to 1
        if(!lepOneFired) lepOneTrigWeight = 1.;
        if(!lepTwoFired) lepTwoTrigWeight = 1.;
      }
    } else if(triggerMode == 1) { //ask the probability of a trigger firing in data / MC
      float trig_correction = particleCorrections->CombineEfficiencies(data_eff[0], mc_eff[0], data_eff[1], mc_eff[1]);
      lepOneTrigWeight = (trig_correction >= 0.) ? sqrt(trig_correction) : 1.; //split evenly between them, doesn't matter though
      lepTwoTrigWeight = (trig_correction >= 0.) ? sqrt(trig_correction) : 1.;
    } else if(triggerMode == 2) { //considerer inefficiency if a trigger didn't fire: Currently has normalization issues
      float trig_correction = (trig_fired[0]) ? data_eff[0] / mc_eff[0] : (1. - data_eff[0]) / (1. - mc_eff[0]);
      trig_correction *= (trig_fired[1]) ? data_eff[1] / mc_eff[1] : (1. - data_eff[1]) / (1. - mc_eff[1]);
      if(std::fabs(trig_correction - lepOneTrigWeight*lepTwoTrigWeight) > 1.e-5)
        std::cout << "!!! Warning! Entry " << fentry << " has disagreeing trigger weights! " << trig_correction << " vs "
                  << lepOneTrigWeight*lepTwoTrigWeight << " (" << lepOneTrigWeight << " * " << lepTwoTrigWeight << ")\n";
    }

    if(std::fabs(triggerWeights[triggerMode] - lepOneTrigWeight * lepTwoTrigWeight) > 0.001)
      std::cout << "!!! Warning! Entry " << fentry << ": Trigger weight disagreement! "
           << triggerWeights[triggerMode] << " vs " << lepOneTrigWeight*lepTwoTrigWeight << std::endl;

    if(trigger_index > 2)
      std::cout << "!!! Warning! Counting of trigger objects found "
                << trigger_index << " objects in entry "
                << fentry << std::endl;

    if(fIsEmbed) {
      puWeight = 1.; //PU taken from data
      zPtWeight = 1.; //Z spectrum taken from data
      photonIDWeight = 1.; //photons away from leptons from data, near becomes less clear
      btagWeight = 1.; //b-tagged jets taken from data
    }

    //use *= since embedding already starts with an inherent weight
    eventWeight = lepOneWeight1 * lepOneWeight2 * lepTwoWeight1 * lepTwoWeight2 * lepOneTrigWeight * lepTwoTrigWeight * puWeight * btagWeight * embeddingWeight;
    if(fUsePhotonWeight > 0) eventWeight *= photonIDWeight;
    else if(fUsePhotonWeight < 0) photonIDWeight = 1.; //completely ignore and ensure it is not used

    if(fIsDY) eventWeight *= zPtWeight;
  } else {
    //Remove any weights from data
    eventWeight = 1.;
    genWeight = 1.;
    puWeight = 1.;
    lepOneWeight1 = 1.;
    lepOneWeight2 = 1.;
    lepTwoWeight1 = 1.;
    lepTwoWeight2 = 1.;
    lepOneTrigWeight = 1.;
    lepTwoTrigWeight = 1.;
    zPtWeight = 1.;
    photonIDWeight = 1.;
    btagWeight = 1.;
    triggerWeights[0] = 1.;
    triggerWeights[1] = 1.;
    triggerWeights[2] = 1.;
  }
  if((trig_fired[0] + trig_fired[1]) != (lepOneFired + lepTwoFired)) {
    std::cout << "!!! Warning! Entry " << fentry << " Selection " << fSelecNames[selection].Data() << ": trigger firing disagreement! trig_fired = ("
              << trig_fired[0] << ", " << trig_fired[1] << ") lepOneFired = " << lepOneFired
              << " lepTwoFired = " << lepTwoFired << std::endl;
  }
  if((eventWeight <= 0. && embeddingWeight != 0.) || fVerbose > 1 /*|| (useTrigObj && trigMatchOne == 0 && trigMatchTwo == 0)*/ ) {
    if(eventWeight <= 0.)
      std::cout << "WARNING! Eventweight <= 0. in entry " << fentry << "!\n";
    if(useTrigObj && trigMatchOne == 0 && trigMatchTwo == 0)
      std::cout << "WARNING! No trigger match found in entry " << fentry << "!\n";
    std::cout << "eventWeight = " << eventWeight
              << " genWeight = " << genWeight
              << " puWeight = " << puWeight
              << " btagWeight = " << btagWeight
              << " embeddingWeight = " << embeddingWeight
              << " photonIDWeight = " << photonIDWeight << std::endl
              << " lepOneWeight1 = " << lepOneWeight1
              << " lepOneWeight2 = " << lepOneWeight2
              << " lepTwoWeight1 = " << lepTwoWeight1
              << " lepTwoWeight2 = " << lepTwoWeight2
              << " muonLowTriggered = " << lowMuonTriggered
              << " muonHighTriggered = " << highMuonTriggered
              << " electronTriggered = " << electronTriggered << std::endl
              << " lepOneTrigWeight = " << lepOneTrigWeight
              << " lepTwoTrigWeight = " << lepTwoTrigWeight
              << " triggerWeights[0] = " << triggerWeights[0]
              << " triggerWeights[1] = " << triggerWeights[1]
              << " triggerWeights[2] = " << triggerWeights[2]
              << " lepOneFired = " << lepOneFired
              << " lepTwoFired = " << lepTwoFired
              << " trigMatchOne = " << trigMatchOne
              << " trigMatchTwo = " << trigMatchTwo
              << " lepOneIndex = " << leptonOneIndex
              << " lepTwoIndex = " << leptonTwoIndex
              << std::endl;
  }

  //save npv as ngoodpv for now
  nPV = nGoodPV;
}

//-----------------------------------------------------------------------------------------------------------------
//count jets separately, due to delta R cut from selection leptons
void NanoAODConversion::CountJets(int selection) {
  //Jet loop
  unsigned njets = nJet;
  //reset counters
  nJets = 0; nJets20 = 0; nJets20Rej = 0; nFwdJets = 0;
  nBJets = 0; nBJetsL = 0; nBJetsM = 0;
  nBJets20 = 0; nBJets20L = 0; nBJets20M = 0;
  ht = 0.; htPhi = 0.; htSum = 0.;
  jetP4->SetPtEtaPhiM(0., 0., 0., 0.); jetIDOut = 0; jetPUIDOut = 0;
  float jetptmax = -1.;
  int jetIDFlag = 2; //tight (without lep veto) in all years is >= 2
  int jetPUIDFlag = 6; //medium ID in all years
  // if(fYear == ParticleCorrections::k2017) {
  //   jetIDFlag = 4; jetPUIDFlag = 6;
  // } else if(fYear == ParticleCorrections::k2018) {
  //   jetIDFlag = 4; jetPUIDFlag = 6;
  // }
  TLorentzVector htLV;
  TLorentzVector* jetLoop = new TLorentzVector(); //for checking delta R
  for(int index = 0; index < std::min((int)njets,(int)kMaxParticles); ++index) {
    slimJets[index].pt       = jetPt[index];
    slimJets[index].eta      = jetEta[index];
    slimJets[index].phi      = jetPhi[index];
    slimJets[index].mass     = jetMass[index];
    slimJets[index].ID       = jetID[index];
    slimJets[index].puID     = jetPUID[index];
    slimJets[index].bTagCMVA = jetBTagCMVA[index];
    slimJets[index].bTagDeep = jetBTagDeepB[index];

    float jetpt = jetPt[index];
    if(jetpt < 20.) continue; //too low of jet pt
    if(jetID[index] < jetIDFlag) //bad jet
      continue;
    //apply PU ID after overlap checks

    //////////////////////////////////////////////////////////////
    //check that the jet doesn't overlap the leptons or photons //
    //////////////////////////////////////////////////////////////

    jetLoop->SetPtEtaPhiM(jetPt[index], jetEta[index], jetPhi[index], jetMass[index]);
    bool passDeltaR = true;
    double deltaRMin = 0.3;
    TLorentzVector lv;

    /** check muons **/
    for(UInt_t imuon = 0; imuon < fNMuons[selection]; ++imuon) {
      lv.SetPtEtaPhiM(muonPt[fMuonIndices[selection][imuon]] , muonEta[fMuonIndices[selection][imuon]],
                      muonPhi[fMuonIndices[selection][imuon]], muonMass[fMuonIndices[selection][imuon]]);
      passDeltaR &= jetLoop->DeltaR(lv) > deltaRMin;
      if(!passDeltaR) break;
    }
    //if fails, move to next jet
    if(!passDeltaR) continue;

    /** check electrons **/
    for(UInt_t ielectron = 0; ielectron < fNElectrons[selection]; ++ielectron) {
      lv.SetPtEtaPhiM(electronPt[fElectronIndices[selection][ielectron]] , electronEta[fElectronIndices[selection][ielectron]],
                      electronPhi[fElectronIndices[selection][ielectron]], electronMass[fElectronIndices[selection][ielectron]]);
      passDeltaR &= jetLoop->DeltaR(lv) > deltaRMin;
      if(!passDeltaR) break;
    }
    //if fails, move to next jet
    if(!passDeltaR) continue;

    /** check taus **/
    for(UInt_t itau = 0; itau < fNTaus[selection]; ++itau) {
      lv.SetPtEtaPhiM(tauPt[fTauIndices[selection][itau]] , tauEta[fTauIndices[selection][itau]],
                      tauPhi[fTauIndices[selection][itau]], tauMass[fTauIndices[selection][itau]]);
      passDeltaR &= jetLoop->DeltaR(lv) > deltaRMin;
      if(!passDeltaR) break;
    }
    //if fails, move to next jet
    if(!passDeltaR) continue;

    // /** check photons **/
    // for(UInt_t iphoton = 0; iphoton < fNPhotons[selection]; ++iphoton) {
    //   lv.SetPtEtaPhiM(photonPt[ fPhotonIndices[selection][iphoton]], photonEta[ fPhotonIndices[selection][iphoton]],
    //                   photonPhi[fPhotonIndices[selection][iphoton]], photonMass[fPhotonIndices[selection][iphoton]]);
    //   passDeltaR &= jetLoop->DeltaR(lv) > deltaRMin;
    //   if(!passDeltaR) break;
    // }
    // //if fails, move to next jet
    // if(!passDeltaR) continue;

    //check if jet fails PU ID or not
    if(jetpt < 50. && jetPUID[index] < jetPUIDFlag) {
      jetsRejPt    [nJets20Rej] = jetPt[index];
      jetsRejEta   [nJets20Rej] = jetEta[index];
      ++nJets20Rej;
      continue;
    }

    //////////////////////////
    // Count jet, check eta //
    //////////////////////////

    if(std::fabs(jetEta[index]) < 3.) {
      //store the highest pT jet
      if(jetptmax < jetpt) {
        jetptmax = jetpt;
        jetP4->SetPtEtaPhiM(jetPt[index], jetEta[index], jetPhi[index], jetMass[index]);
        jetIDOut = jetID[index];
        jetPUIDOut = jetPUID[index];
      }
      //counters for different pT cuts on the jets
      if(jetpt > 25.) ++nJets;
      ++nJets20;

      htLV += *jetLoop;
      htSum += jetpt;
      int jetBTag = 0;
      //check if b-tagged, must have eta < 2.4
      if(std::fabs(jetEta[index]) < 2.4) {
        if(jetBTagDeepB[index] > particleCorrections->BTagCut(ParticleCorrections::kLooseBTag, fYear)) {
          ++jetBTag; ++nBJets20L; if(jetpt > 25.) ++nBJetsL;
          if(jetBTagDeepB[index] > particleCorrections->BTagCut(ParticleCorrections::kMediumBTag, fYear)) {
            ++jetBTag; ++nBJets20M; if(jetpt > 25.) ++nBJetsM;
            if(jetBTagDeepB[index] > particleCorrections->BTagCut(ParticleCorrections::kTightBTag, fYear)) {
              ++jetBTag; ++nBJets20; if(jetpt > 25.) ++nBJets;
            }
          }
        }
      }
      //save info for jet b-tag efficiency
      jetsPt    [nJets20-1] = jetpt;
      jetsEta   [nJets20-1] = jetEta[index];
      jetsBTag  [nJets20-1] = jetBTag;
      jetsFlavor[nJets20-1] = jetFlavor[index];
    } else {
      ++nFwdJets;
    }
  }
  ht = htLV.Pt();
  htPhi = htLV.Phi();
  delete jetLoop;
  if(fVerbose > 1) std::cout << "Jet counts: nJets20 = " << nJets20
                             << " nBJets20 = " << nBJets20
                             << " nBJets20L = " << nBJets20L
                             << std::endl;
}

//-----------------------------------------------------------------------------------------------------------------
//apply the Rochester corrections to the muon objects' pts
void NanoAODConversion::ApplyMuonCorrections() {
  const static int s(0), m(0); //error set and member for corrections
  for(UInt_t index = 0; index < nMuon; ++index) {
    const double u = fRnd->Uniform();
    double sf = 1.;
    if(fIsData) {
      sf = fRoccoR->kScaleDT(muonCharge[index], muonPt[index], muonEta[index], muonPhi[index], s, m);
    } else {
      //For muons in embedding events not identified as the main leptons, apply typical data corrections
      if(fIsEmbed && (abs(leptonOneFlavor) != 13 || (int) index != leptonOneIndex) && (abs(leptonTwoFlavor) != 13 || (int) index != leptonTwoIndex)) {
        sf = fRoccoR->kScaleDT(muonCharge[index], muonPt[index], muonEta[index], muonPhi[index], s, m);
      } else {
        //For simulated muons, both in embedding and standard MC, apply given MC corrections
        sf = fRoccoR->kSmearMC(muonCharge[index], muonPt[index], muonEta[index], muonPhi[index], muonNTrkLayers[index], u, s, m);
      }
    }
    muonPt[index] *= sf;
    muonRoccoSF[index] = sf;
  }
}

//-----------------------------------------------------------------------------------------------------------------
// Apply electron ID checks
bool NanoAODConversion::ElectronID(Int_t index, Int_t ID) {
  const double elec_eta = std::fabs(electronEta[index]);
  const double elec_sc_eta = std::fabs(electronEta[index] + electronDeltaEtaSC[index]);
  bool passed = (ID == 0 ||
                 (ID == 1 && electronWPL [index]) ||
                 (ID == 2 && electronWP80[index]) ||
                 (ID == 3 && electronWP90[index]) // FIXME: reorder IDs to be increasing in purity
                 );
  passed &=electronPt[index] > fElectronPtCount;
  passed &= elec_eta    < fElectronEtaCount;
  // passed &= elec_sc_eta < fElectronEtaCount;
  passed &= (elec_sc_eta <  1.442 || elec_sc_eta > 1.566);

  return passed;
}

//-----------------------------------------------------------------------------------------------------------------
// Apply muon ID checks
bool NanoAODConversion::MuonID(Int_t index, Int_t isoID, Int_t ID) {
  bool passed = muonIsoId[index] > isoID;
  passed &=  ( ID == 0                         ||
              (ID == 1 && muonLooseId [index]) ||
              (ID == 2 && muonMediumId[index]) ||
              (ID == 3 && muonTightId [index])
              );
  passed &= muonPt[index] > fMuonPtCount;
  passed &= std::fabs(muonEta[index]) < fMuonEtaCount;
  return passed;
}

//-----------------------------------------------------------------------------------------------------------------
// Apply tau ID checks
bool NanoAODConversion::TauID(Int_t index, Bool_t useDeep, Int_t antiEle, Int_t antiMu, Int_t antiJet, Bool_t DM, Int_t oldAntiMu) {
  bool passed = true;
  if(useDeep) { //use Deep NN based IDs
    passed &= tauDeep2017VsE  [index] >= antiEle;
    passed &= tauDeep2017VsMu [index] >= antiMu ;
    passed &= tauDeep2017VsJet[index] >= antiJet;
    passed &= (!DM || tauIDDecayModeNew[index]);
    passed &= tauAntiMu[index] >= oldAntiMu; //Old MVA ID due to 2016 QCD excess
  } else {
    passed &= tauAntiEle[index] >= antiEle;
    passed &= tauAntiMu [index] >= antiMu;
    passed &= (!DM || tauIDDecayMode[index]);
  }
  passed &= tauPt[index] > fTauPtCount;
  passed &= std::fabs(tauEta[index]) < fTauEtaCount;
  return passed;
}

//-----------------------------------------------------------------------------------------------------------------
//count and create maps for reconstructed objects
void NanoAODConversion::CountObjects() {
  ApplyMuonCorrections(); //apply momentum corrections before counting objects

  //reset counters
  for(Int_t selection = kMuTau; selection < kSelections; ++selection) {
    fNTaus[selection] = 0;
    fNElectrons[selection] = 0;
    fNMuons[selection] = 0;
    fNPhotons[selection] = 0;
  }

  /////////////////////////////////////////
  // Count the muons
  /////////////////////////////////////////
  for(Int_t index = 0; index < std::min(((int)nMuon),((int)kMaxParticles)); ++index) {
    //initialize the muon iso IDs
    muonIsoId[index] = 0; //initially fails all IDs
    for(int level = ParticleCorrections::kVLooseMuIso; level <= ParticleCorrections::kVVTightMuIso; ++level) {
      const double isoval = particleCorrections->muonIsoValues[level];
      if(muonRelIso[index] < isoval) { //increment the ID value for each cut it passes
        muonIsoId[index] += 1;
      } else {
        break;
      }
    }
    //Apply the selection cuts for each channel
    for(Int_t selection = kMuTau; selection < kSelections; ++selection) {
      if(fVerbose > 9) std::cout << "Checking muon " << index << " for selection " << fSelecNames[selection].Data() << " with pt = " << muonPt[index]
                                 << " eta = " << muonEta[index]
                                 << " iso = " << ((int) muonIsoId[index]) << " > isoId = " << ((int) fMuonIsoCount[selection])
                                 << " loose id = " << muonLooseId[index]
                                 << " medium id = " << muonMediumId[index] << " tight id = " << muonTightId[index]
                                 << std::endl;
      if(fCountMuons[selection] && MuonID(index, fMuonIsoCount[selection], fMuonIDCount[selection])) {
        fMuonIndices[selection][fNMuons[selection]] = index;
        ++fNMuons[selection];
        if(fVerbose > 2) std::cout << "Accepting muon " << index << " for selection " << fSelecNames[selection].Data() << " with pt = " << muonPt[index]
                                   << " eta = " << muonEta[index]
                                   << " iso = " << ((int) muonIsoId[index]) << " > isoId = " << ((int) fMuonIsoCount[selection])
                                   << " loose id = " << muonLooseId[index]
                                   << " medium id = " << muonMediumId[index] << " tight id = " << muonTightId[index]
                                   << std::endl;
      }
    } //end selection loop
    //store basic information for all muons
    slimMuons[index].pt       = muonPt[index];
    slimMuons[index].eta      = muonEta[index];
    slimMuons[index].phi      = muonPhi[index];
    slimMuons[index].mass     = muonMass[index];
    slimMuons[index].pfIsoId  = muonIsoId[index];
    slimMuons[index].looseId  = muonLooseId[index];
    slimMuons[index].mediumId = muonMediumId[index];
    slimMuons[index].tightId  = muonTightId[index];
    slimMuons[index].positive = muonCharge[index] > 0;
  }

  /////////////////////////////////////////
  // Count the electrons
  /////////////////////////////////////////
  for(Int_t index = 0; index < std::min(((int)kMaxParticles),((int)nElectron)); ++index) {
    for(Int_t selection = kMuTau; selection < kSelections; ++selection) {
      if(fVerbose > 9) std::cout << "Checking electron " << index << " for selection " << fSelecNames[selection].Data() << " with pt = " << electronPt[index]
                                 << " eta = " << electronEta[index]
                                 << " WPL id = " << electronWPL[index]
                                 << " WP80 id = " << electronWP80[index] << " WP90 id = " << electronWP90[index]
                                 << std::endl;
      if(fCountElectrons[selection] && ElectronID(index, fElectronIDCount[selection])) {
        fElectronIndices[selection][fNElectrons[selection]] = index;
        ++fNElectrons[selection];
        if(fVerbose > 2) std::cout << "Accepting electron " << index
                                   << " (" << fNElectrons[selection] << " --> " << fElectronIndices[selection][fNElectrons[selection] - 1] << ") for selection "
                                   << fSelecNames[selection].Data()
                                   << " with pt = " << electronPt[index]
                                   << " eta = " << electronEta[index]
                                   << " WPL id = " << electronWPL[index]
                                   << " WP80 id = " << electronWP80[index] << " WP90 id = " << electronWP90[index]
                                   << std::endl;
      }
    } //end selection loop
    //store basic information for all electrons
    slimElectrons[index].pt         = electronPt[index];
    slimElectrons[index].eta        = electronEta[index];
    slimElectrons[index].phi        = electronPhi[index];
    slimElectrons[index].mass       = electronMass[index];
    slimElectrons[index].WPL        = electronWPL [index];
    slimElectrons[index].WP80       = electronWP80[index];
    slimElectrons[index].WP90       = electronWP90[index];
    slimElectrons[index].scDeltaEta = electronDeltaEtaSC[index];
    slimElectrons[index].positive   = electronCharge[index] > 0;
  }

  /////////////////////////////////////////
  // Count the taus
  /////////////////////////////////////////
  for(Int_t index = 0; index < std::min(((int)kMaxParticles),((int)nTau)); ++index) {
    TLorentzVector lv1, lv2;
    lv2.SetPtEtaPhiM(tauPt[index], tauEta[index], tauPhi[index], tauMass[index]);
    if(tauPt[index] <= fTauPtCount || std::fabs(tauEta[index]) >= fTauEtaCount) continue; //continue to next tau
    for(Int_t selection = kMuTau; selection < kSelections; ++selection) {
      if(fVerbose > 9) std::cout << "Checking tau " << index << " for selection " << fSelecNames[selection].Data() << " with pt = " << tauPt[index]
                                 << " eta = " << tauEta[index]
                                 << " decay id = " << tauIDDecayModeNew[index]
                                 << " anti ele id = " << ((int) tauDeep2017VsE[index])
                                 << " anti mu id = " << ((int) tauDeep2017VsMu[index])
                                 << " anti jet id = " << ((int) tauDeep2017VsJet[index])
                                 << std::endl;
      //ensure no overlaps
      bool passDeltaR = true;
      /** check muons **/
      for(UInt_t imuon = 0; imuon < fNMuons[selection]; ++imuon) {
        lv1.SetPtEtaPhiM(muonPt[fMuonIndices[selection][imuon]] , muonEta[fMuonIndices[selection][imuon]],
                         muonPhi[fMuonIndices[selection][imuon]], muonMass[fMuonIndices[selection][imuon]]);
        passDeltaR = passDeltaR && lv1.DeltaR(lv2) > fTauDeltaRCount[selection];
        if(!passDeltaR) break;
      }
      //if fails, move to next selection
      if(!passDeltaR) continue;
      /** check electrons **/
      for(UInt_t ielectron = 0; ielectron < fNElectrons[selection]; ++ielectron) {
        lv1.SetPtEtaPhiM(electronPt[fElectronIndices[selection][ielectron]] , electronEta[fElectronIndices[selection][ielectron]],
                         electronPhi[fElectronIndices[selection][ielectron]], electronMass[fElectronIndices[selection][ielectron]]);
        passDeltaR = passDeltaR && lv1.DeltaR(lv2) > fTauDeltaRCount[selection];
        if(!passDeltaR) break;
      }
      //if fails, move to next selection
      if(!passDeltaR) continue;
      if(fCountTaus[selection] && TauID(index, fTauUseDeep[selection], fTauAntiEleCount[selection],
                                        fTauAntiMuCount[selection], fTauAntiJetCount[selection],
                                        fTauIDDecayCount[selection], fTauAntiOldMuCount[selection])) {
        fTauIndices[selection][fNTaus[selection]] = index;
        ++fNTaus[selection];
        if(fVerbose > 2) std::cout << "Accepting tau " << index << " for selection " << fSelecNames[selection].Data() << " with pt = " << tauPt[index]
                                   << " eta = " << tauEta[index]
                                   << " decay id = " << tauIDDecayModeNew[index]
                                   << " anti ele id = " << ((int) tauDeep2017VsE[index])
                                   << " anti mu id = " << ((int) tauDeep2017VsMu[index])
                                   << " anti jet id = " << ((int) tauDeep2017VsJet[index])
                                   << std::endl;
      }
    } //end selection loop
    //store basic information for all taus
    slimTaus[index].pt             = tauPt[index];
    slimTaus[index].eta            = tauEta[index];
    slimTaus[index].phi            = tauPhi[index];
    slimTaus[index].mass           = tauMass[index];
    slimTaus[index].decayMode      = tauDecayMode[index];
    slimTaus[index].idDecayMode    = tauIDDecayMode[index];
    slimTaus[index].idDecayModeNew = tauIDDecayModeNew[index];
    slimTaus[index].antiEle        = tauAntiEle[index];
    slimTaus[index].antiEle2018    = tauAntiEle2018[index];
    slimTaus[index].antiMu         = tauAntiMu[index];
    slimTaus[index].deepAntiEle    = tauDeep2017VsE[index];
    slimTaus[index].deepAntiMu     = tauDeep2017VsMu[index];
    slimTaus[index].deepAntiJet    = tauDeep2017VsJet[index];
    slimTaus[index].genID          = tauGenID[index];
    slimTaus[index].positive       = tauCharge[index] > 0;
  }

  /////////////////////////////////////////
  // Count the photons
  /////////////////////////////////////////
  for(Int_t index = 0; index < std::min((int)kMaxParticles,(int)nPhoton); ++index) {
    TLorentzVector lv1, lv2;
    lv2.SetPtEtaPhiM(photonPt[index], photonEta[index], photonPhi[index], photonMass[index]);
    for(Int_t selection = kMuTau; selection < kSelections; ++selection) {
      /** Selection cuts on photon **/
      if(std::fabs(photonEta[index]) > 2.5 /* FIXME: Add barrel endcap overlap region rejection! */) continue;
      if(photonPt[index] < fPhotonPtCount) continue; //pT cut
      if(!(fPhotonIDCount[selection] == 0 || //No ID cut
           (fPhotonIDCount[selection] == 1 && photonWP80[index]))) //WP80 cut FIXME: switch to WP90
        continue;
      //ensure no overlaps
      bool passDeltaR = true;
      /** check muons **/
      for(UInt_t imuon = 0; imuon < fNMuons[selection]; ++imuon) {
        lv1.SetPtEtaPhiM(muonPt[fMuonIndices[selection][imuon]] , muonEta[fMuonIndices[selection][imuon]],
                         muonPhi[fMuonIndices[selection][imuon]], muonMass[fMuonIndices[selection][imuon]]);
        passDeltaR = passDeltaR && lv1.DeltaR(lv2) > fPhotonDeltaRCount[selection];
        if(!passDeltaR) break;
      }
      //if fails, move to next photon
      if(!passDeltaR) continue;
      /** check electrons **/
      for(UInt_t ielectron = 0; ielectron < fNElectrons[selection]; ++ielectron) {
        lv1.SetPtEtaPhiM(electronPt[fElectronIndices[selection][ielectron]] , electronEta[fElectronIndices[selection][ielectron]],
                         electronPhi[fElectronIndices[selection][ielectron]], electronMass[fElectronIndices[selection][ielectron]]);
        passDeltaR = passDeltaR && lv1.DeltaR(lv2) > fPhotonDeltaRCount[selection];
        if(!passDeltaR) break;
      }
      //if fails, move to next photon
      if(!passDeltaR) continue;
      /** check taus **/
      for(UInt_t itau = 0; itau < fNTaus[selection]; ++itau) {
        lv1.SetPtEtaPhiM(tauPt[fTauIndices[selection][itau]] , tauEta[fTauIndices[selection][itau]],
                         tauPhi[fTauIndices[selection][itau]], tauMass[fTauIndices[selection][itau]]);
        passDeltaR = passDeltaR && lv1.DeltaR(lv2) > fPhotonDeltaRCount[selection];
        if(!passDeltaR) break;
      }
      //if fails, move to next photon
      if(!passDeltaR) continue;


      fPhotonIndices[selection][fNPhotons[selection]] = index;
      ++fNPhotons[selection];
      if(fVerbose > 2) std::cout << "Accepting photon " << index << " for selection " << fSelecNames[selection].Data() << " with pt = " << photonPt[index]
                                 << " eta = " << photonEta[index]
                                 << std::endl;
    } //end selection loop
    slimPhotons[index].pt    = photonPt[index];
    slimPhotons[index].eta   = photonEta[index];
    slimPhotons[index].phi   = photonPhi[index];
    slimPhotons[index].mass  = photonMass[index];
    slimPhotons[index].MVA   = photonMVAID[index];
    slimPhotons[index].MVA17 = photonMVAID17[index];
    slimPhotons[index].WP80  = photonWP80[index];
  }
}

//count taus used for fake tau studies
void NanoAODConversion::CountTaus(int selection) {
  for(unsigned ntau = 0; ntau < fNTaus[selection]; ++ntau) {
    unsigned index = fTauIndices[selection][ntau];
    tausPt        [ntau] = tauPt [index];
    tausEta       [ntau] = tauEta[index];
    tausPhi       [ntau] = tauPhi[index];
    tausIsPositive[ntau] = (tauCharge[index] > 0);
    tausAntiJet   [ntau] = tauDeep2017VsJet[index];
    tausAntiMu    [ntau] = tauDeep2017VsMu[index];
    tausMVAAntiMu [ntau] = tauAntiMu[index];
    tausAntiEle   [ntau] = tauDeep2017VsE[index];
    tausDM        [ntau] = tauDecayMode[index];
    tausGenFlavor [ntau] = TauFlavorFromID((int) tauGenID[index]);
    tausWeight    [ntau] = (fIsData == 0) ? particleCorrections->TauWeight(tauPt[index], tauEta[index], tauGenID[index], tauDeep2017VsJet[index], fYear) : 1.;
    float up, down;
    int bin;
    float tausEScale = particleCorrections->TauEnergyScale(tauPt[index], tauEta[index], tauDecayMode[index],
                                                           tauGenID[index], tauDeep2017VsJet[index], fYear, up, down, bin);
    if(tausEScale <= 0.)
      std::cout << "WARNING! In event " << fentry
                << __func__ << " tau energy scale <= 0 = " << tausEScale
                << " for index " << index
                << std::endl;
    else tausPt[ntau] *= tausEScale;

  }
}

//count light leptons used for fake lepton studies
void NanoAODConversion::CountLightLeptons(int selection) {
  nExtraLep = 0;
  if(selection == kEE) {
    for(unsigned nlep = 0; nlep < fNElectrons[selection]; ++nlep) {
      int index = fElectronIndices[selection][nlep];
      if(index == leptonOneIndex || index == leptonTwoIndex) continue;
      leptonsPt        [nExtraLep] = electronPt [index];
      leptonsEta       [nExtraLep] = electronEta[index];
      leptonsIsPositive[nExtraLep] = (electronCharge[index] > 0);
      leptonsIsMuon    [nExtraLep] = false;
      leptonsID        [nExtraLep] = (electronWPL[index] + 2*electronWP90[index] + 4*electronWP80[index]);
      leptonsIsoID     [nExtraLep] = 0;
      leptonsTriggered [nExtraLep] = GetTriggerMatch(index, false);
      leptonsGenFlavor [nExtraLep] = (fIsData == 0) ? electronGenFlavor[index] : 0;
      ++nExtraLep;
    }
  } else if(selection == kMuMu) {
    for(unsigned nlep = 0; nlep < fNMuons[selection]; ++nlep) {
      int index = fMuonIndices[selection][nlep];
      if(index == leptonOneIndex || index == leptonTwoIndex) continue;
      leptonsPt        [nExtraLep] = muonPt [index];
      leptonsEta       [nExtraLep] = muonEta[index];
      leptonsIsPositive[nExtraLep] = (muonCharge[index] > 0);
      leptonsIsMuon    [nExtraLep] = true;
      leptonsID        [nExtraLep] = (muonLooseId[index] + 2*muonMediumId[index] + 4*muonTightId[index]);
      leptonsIsoID     [nExtraLep] = muonIsoId[index];
      leptonsTriggered [nExtraLep] = GetTriggerMatch(index, true);
      leptonsGenFlavor [nExtraLep] = (fIsData == 0) ? muonGenFlavor[index] : 0;
      ++nExtraLep;
    }
  }
}

//check that the selected leptons pass their IDs
bool NanoAODConversion::SelectionID(Int_t selection) {
  bool passed = true;
  //check muon
  if(selection == kMuTau || selection == kMuMu || selection == kEMu) {
    int index = (selection == kMuMu) ? leptonOneSkimIndex : fMuonIndices[selection][0];
    passed &= MuonID(index, fMuonIsoSelect[selection], fMuonIDSelect[selection]);
    if(fVerbose > 2) std::cout << "SelectionID: After Muon 1 has status " << passed << std::endl;
  }
  if(!passed) return false;
  if(selection == kMuMu) {
    int index = (selection == kMuMu) ? leptonTwoSkimIndex : fMuonIndices[selection][1];
    passed &= MuonID(index, fMuonIsoSelect[selection], fMuonIDSelect[selection]);
    if(fVerbose > 2) std::cout << "SelectionID: After Muon 2 has status " << passed << std::endl;
  }
  if(!passed) return false;
  //check electron
  if(selection == kETau || selection == kEMu || selection == kEE) {
    int index = (selection == kEE) ? leptonOneSkimIndex : fElectronIndices[selection][0];
    passed &= ElectronID(index, fElectronIDSelect[selection]);

    if(fVerbose > 2) std::cout << "SelectionID: After Electron 1 has status " << passed << std::endl;
  }
  if(!passed) return false;
  if(selection == kEE) {
    int index = (selection == kEE) ? leptonTwoSkimIndex : fElectronIndices[selection][1];
    passed &= ElectronID(index, fElectronIDSelect[selection]);

    if(fVerbose > 2) std::cout << "SelectionID: After Electron 2 has status " << passed << std::endl;
  }
  if(!passed) return false;
  //check tau
  if(selection == kMuTau || selection == kETau) {
    const int index = fTauIndices[selection][0];
    passed &= TauID(index, fTauUseDeep[selection], fTauAntiEleSelect[selection],
                    fTauAntiMuSelect[selection], fTauAntiJetSelect[selection],
                    fTauIDDecaySelect[selection], fTauAntiOldMuCount[selection]); //use the counting level of the old anti-mu ID
    if(fVerbose > 2) std::cout << "SelectionID: After Tau 1 has status " << passed << std::endl;
  }
  return passed;
}

//-----------------------------------------------------------------------------------------------------------------
// check if the selected leptons pass most IDs but fail the QCD measurement region tight ID
bool NanoAODConversion::QCDSelection(Int_t selection) {
  bool passed         = true; //Each lepton must pass Loose ID and least one must fail the Tight ID
  bool lep1_not_tight = true; //Check if leptons fail the Tight ID after passing the Loose ID
  bool lep2_not_tight = true;

  //check muons
  if(selection == kMuTau || selection == kMuMu || selection == kEMu) {
    const int index = (selection == kMuMu) ? leptonOneSkimIndex : fMuonIndices[selection][0];
    passed          &=  MuonID(index, fMuonIsoCount [selection], fMuonIDSelect[selection]); //must pass loose QCD region ID
    if(selection == kEMu) { //lepton two is the muon
      lep2_not_tight = !MuonID(index, fMuonIsoSelect[selection], fMuonIDSelect[selection]); //fails  tight QCD region ID
    } else { //lepton one is the muon
      lep1_not_tight = !MuonID(index, fMuonIsoSelect[selection], fMuonIDSelect[selection]); //fails  tight QCD region ID
    }
    if(fVerbose > 2) std::cout << __func__ << ": After Muon 1 has status " << passed << std::endl;
  }
  if(!passed) return false;
  if(selection == kMuMu) {
    const int index = (selection == kMuMu) ? leptonTwoSkimIndex : fMuonIndices[selection][1];
    passed        &=  MuonID(index, fMuonIsoCount [selection], fMuonIDSelect[selection]); //must pass loose QCD region ID
    lep2_not_tight = !MuonID(index, fMuonIsoSelect[selection], fMuonIDSelect[selection]); //fails  tight QCD region ID
    if(fVerbose > 2) std::cout << __func__ << ": After Muon 2 has status " << passed << std::endl;
  }
  if(!passed) return false;

  //check electrons
  if(selection == kETau || selection == kEMu || selection == kEE) {
    const int index = (selection == kEE) ? leptonOneSkimIndex : fElectronIndices[selection][0];
    passed        &=  ElectronID(index, fElectronIDCount [selection]); //passes loose QCD region ID
    lep1_not_tight = !ElectronID(index, fElectronIDSelect[selection]); //fails  tight QCD region ID
    if(fVerbose > 2) std::cout << __func__ << ": After Electron 1 has status " << passed << std::endl;
  }
  if(!passed) return false;
  if(selection == kEE) {
    const int index = (selection == kEE) ? leptonTwoSkimIndex : fElectronIndices[selection][1];
    passed        &=  ElectronID(index, fElectronIDCount [selection]); //passes loose QCD region ID
    lep2_not_tight = !ElectronID(index, fElectronIDSelect[selection]); //fails  tight QCD region ID
    if(fVerbose > 2) std::cout << __func__ << ": After Electron 2 has status " << passed << std::endl;
  }
  if(!passed) return false;

  //check tau
  if(selection == kMuTau || selection == kETau) {
    const int index = fTauIndices[selection][0];
    passed        &=  TauID(index, fTauUseDeep[selection], fTauAntiEleSelect[selection], fTauAntiMuSelect[selection],
                            fTauAntiJetCount [selection],  //passes loose anti-jet ID
                            fTauIDDecaySelect[selection], fTauAntiOldMuCount[selection]);
    lep2_not_tight = !TauID(index, fTauUseDeep[selection], fTauAntiEleSelect[selection], fTauAntiMuSelect[selection],
                            fTauAntiJetSelect[selection],  //fails tight anti-jet ID
                            fTauIDDecaySelect[selection],  fTauAntiOldMuCount[selection]);
    if(fVerbose > 2) std::cout << __func__ << ": After Tau 1 has status " << passed << std::endl;
  }
  if(!passed) return false;

 //at least one lepton must fail the Tight ID check
  passed = lep1_not_tight || lep2_not_tight;
  if(fVerbose > 2) std::cout << __func__ << ": Lep 1 !Tight = " << lep1_not_tight << "; Lep 2 !Tight = " << lep2_not_tight
                             << "; status = " << passed << std::endl;
  return passed;
}

//-----------------------------------------------------------------------------------------------------------------
float NanoAODConversion::GetTauFakeSF(int genFlavor) {
  float weight = 1.;
  switch(abs(genFlavor)) {
  case 15 : weight = 0.95; break;
  case 11 : weight = genTauFlavorWeight; break;
  case 13 : weight = genTauFlavorWeight; break;
  }
  //jets are given weight 1
  return weight;
}

//-----------------------------------------------------------------------------------------------------------------
float NanoAODConversion::CorrectMET(int selection, float met) {
  if(selection < 0) return met;
  float corrected = met;

  // switch(abs(selection)) {
  // case 0 : weight = (1.1 - 0.2/150.*met); break; // mu+tau
  // case 1 : weight = 1.; break; // e+tau
  // case 2 : weight = 1.; break; // e+mu
  // }
  return corrected;
}

//-----------------------------------------------------------------------------------------------------------------
// re-weight the AMC@NLO DY Z pT
float NanoAODConversion::GetZPtWeight(float pt) {
  float weight = 1.;
  if(pt >= 140.)
    weight = 0.891188;
  else {
    weight = (0.876979 + pt*(4.11598e-3 - pt*2.35520e-5));
    weight *= 1.10211*(0.958512-0.131835*erf((pt-14.1972)/10.1525));
  }
  if(weight < 0.) {
    printf("!!! Warning! Z pT weight < 0: weight = %.3e, pt = %.2f\n", weight, pt);
  }
  return weight;
}

//-----------------------------------------------------------------------------------------------------------------
//Check if lepton matches to a trigger
int NanoAODConversion::GetTriggerMatch(UInt_t index, bool isMuon, Int_t& trigIndex) {
  float pt, eta, phi, min_pt_1, min_pt_2;
  int bit_1, bit_2, id; //trigger bits to use and pdgID
  bool flag_1(true), flag_2(true);
  if(isMuon) {
    pt  = muonPt[index];
    eta = muonEta[index];
    phi = muonPhi[index];
    bit_1 = 1; //IsoMu24/IsoMu27
    min_pt_1 = (fYear == ParticleCorrections::k2017) ? 28. : 25.;
    bit_2 = 10; //Mu50
    min_pt_2 = 50.;
    id = 13;
    flag_1 = (fYear == ParticleCorrections::k2017) ? HLT_IsoMu27 : HLT_IsoMu24;
    flag_2 = HLT_Mu50;
  } else { //electron
    pt  = electronPt[index];
    eta = electronEta[index];
    phi = electronPhi[index];
    id = 11;
    if(fYear == ParticleCorrections::k2017) {
      bit_1 = 10; //different trigger in 2017 to correct for Ele32 issues
      bit_2 = 10;
    } else {
      bit_1 = 1;
      bit_2 = 1;
    }
    min_pt_1 = (fYear == ParticleCorrections::k2016) ? 28. : 33.;
    min_pt_2 = (fYear == ParticleCorrections::k2016) ? 28. : 33.;
    flag_1 = ((fYear == ParticleCorrections::k2016 && HLT_Ele27_WPTight_GsF) ||
              (fYear == ParticleCorrections::k2017 && HLT_Ele32_WPTight_GsF_L1DoubleEG) ||
              (fYear == ParticleCorrections::k2018 && HLT_Ele32_WPTight_GsF));
    flag_2 = flag_1;
  }


  if(fVerbose > 2) std::cout << "Doing trigger match on index " << index << " with isMuon = " << isMuon
                             << " and bits " << (1<<bit_1) << " and " << (1<<bit_2) << std::endl
                             << "Given lepton pt, eta, phi = " << pt << ", " << eta << ", " << phi << std::endl;
  if(pt < min_pt_1 && pt < min_pt_2) {
    if(fVerbose > 2) std::cout << " lepton pT below trigger thresholds\n";
    return 0; //pT below threshold considered
  }

  float deltar_match = 0.2;
  float deltapt_match = 10; //fractional match, > ~10 means don't use
  bool passedBit1 = false;
  bool passedBit2 = false;
  trigIndex = -1;
  for(unsigned trig_i = 0; trig_i < nTrigObjs; ++trig_i) {
    if(trigObjID[trig_i] != id) continue;
    //check if passes correct bit and the considered pT threshold for that trigger
    bool passBit1 = ((trigObjFilterBits[trig_i] & (1<<bit_1)) != 0) && (!fDoTriggerMatchPt || (pt > min_pt_1 && trigObjPt[trig_i] > min_pt_1));
    bool passBit2 = ((trigObjFilterBits[trig_i] & (1<<bit_2)) != 0) && (!fDoTriggerMatchPt || (pt > min_pt_2 && trigObjPt[trig_i] > min_pt_2));
    if(fVerbose > 2) std::cout << " TrigObj " << trig_i << " has bits " << trigObjFilterBits[trig_i] << std::endl;
    if(!passBit1 && !passBit2) continue;
    if(fVerbose > 2) std::cout << " TrigObj " << trig_i << " passed bits check, has pt, eta, phi "
                               << trigObjPt[trig_i] <<  ", "
                               << trigObjEta[trig_i] <<  ", "
                               << trigObjPhi[trig_i] << std::endl;
    passBit1 &= flag_1;
    passBit2 &= flag_2;
    if(!passBit1 && !passBit2) continue;
    if(fVerbose > 2) std::cout << " TrigObj " << trig_i << " passed global flag test\n";
    if(std::fabs(pt - trigObjPt[trig_i]) < pt*deltapt_match) {
      float deltaeta = std::fabs(eta - trigObjEta[trig_i]);
      float deltaphi = std::fabs(phi - trigObjPhi[trig_i]);
      if(deltaphi > M_PI) deltaphi = std::fabs(2.*M_PI - deltaphi);
      if(fVerbose > 2) std::cout << "  TrigObj " << trig_i << " passed pt check\n";
      if(sqrt(deltaeta*deltaeta + deltaphi*deltaphi) < deltar_match) {
      if(fVerbose > 2) std::cout << "   TrigObj " << trig_i << " passed delta r xcheck\n";
      trigIndex = trig_i;
      if(fVerbose > 2)
        std::cout << "--- Found a matching trigger object! Continuing search for additional matches...\n";
      passedBit1 |= passBit1;
      passedBit2 |= passBit2;
      }
    }
  }
  return (passedBit1+2*passedBit2);//no matching trigger object found = 0
}

//-----------------------------------------------------------------------------------------------------------------
// Main processing loop
Bool_t NanoAODConversion::Process(Long64_t entry)
{
  fentry = entry;
  if(fVerbose > 1) std::cout << "\n***Event " << entry << std::endl;
  // The Process() function is called for each entry in the tree (or possibly
  // keyed object in the case of PROOF) to be processed. The entry argument
  // specifies which entry in the currently loaded tree is to be processed.
  // When processing keyed objects with PROOF, the object is already loaded
  // and is available via the fObject pointer.

  fChain->GetEntry(entry);
  if(entry%50000 == 0) printf("Processing event: %12lld (%5.1f%%)\n", entry, entry*100./fChain->GetEntriesFast());

  //Set the lepton indices initially to the indices from the skimmer
  leptonOneIndex = leptonOneSkimIndex; leptonTwoIndex = leptonTwoSkimIndex;

  if(fVerbose > 2) std::cout << "Event info: nMuon = " << nMuon << " nElectron = " << nElectron
                             << " nTau = " << nTau << " nJet = " << nJet << " nTrigObj = " << nTrigObjs
                             << " nPhoton = " << nPhoton
                             << std::endl
                             << "Skim event info: indices = " << leptonOneSkimIndex << " and " << leptonTwoSkimIndex
                             << " flavors = " << leptonOneSkimFlavor << " and " << leptonTwoSkimFlavor << std::endl;

  if(kMaxParticles < nMuon || kMaxParticles < nElectron || kMaxParticles < nJets ||
     kMaxParticles < nTau  || kMaxParticles < nPhotons  || kMaxTriggers < nTrigObjs)
    std::cout << "!!! WARNING! More particles than allowed in event " << entry
              << ":\n"  << "Event info: nMuon = " << nMuon << " nElectron = " << nElectron
              << " nTau = " << nTau << " nJet = " << nJet << " nTrigObj = " << nTrigObjs
              << " nPhoton = " << nPhoton
              << std::endl;

  //replace pileup weight if requested
  if(fIsData == 0 && fIsEmbed == 0 && fReplacePUWeights > 0) puWeight = fPUWeight.GetWeight(nPU, fYear+2016); //uses absolute year number

  //Count physics objects in the event (leptons, jets, etc.)
  CountObjects();

  //////////////////////////////////////////////////////
  // Check channel selections
  //////////////////////////////////////////////////////

  //Check the triggers fired
  bool muontriggered = HLT_Mu50 || ((fYear == ParticleCorrections::k2016 && HLT_IsoMu24) ||
                                    (fYear == ParticleCorrections::k2017 && HLT_IsoMu27) ||
                                    (fYear == ParticleCorrections::k2018 && HLT_IsoMu24));

  bool electrontriggered = ((fYear == ParticleCorrections::k2016 && HLT_Ele27_WPTight_GsF) ||
                          (fYear == ParticleCorrections::k2017 && HLT_Ele32_WPTight_GsF_L1DoubleEG) || //FIXME: add L1 Ele35 test as well
                          (fYear == ParticleCorrections::k2018 && HLT_Ele32_WPTight_GsF));

  bool triggered = muontriggered || electrontriggered;

  //Check object counts + trigger selections for channel filtering
  bool mutau = fNTaus[kMuTau]  == 1 && fNMuons[kMuTau]     == 1 && muontriggered     /* && SelectionID(kMuTau)*/;
  bool etau  = fNTaus[kETau]   == 1 && fNElectrons[kETau]  == 1 && electrontriggered /* && SelectionID(kETau) */;
  bool emu   = fNMuons[kEMu]   == 1 && fNElectrons[kEMu]   == 1 && triggered         /* && SelectionID(kEMu)  */;
  bool mumu  = fNMuons[kMuMu]  == 2 && fNElectrons[kMuMu]  == 0 && muontriggered     /* && SelectionID(kMuMu) */;
  bool ee    = fNMuons[kEE]    == 0 && fNElectrons[kEE]    == 2 && electrontriggered /* && SelectionID(kEE)   */;

  bool mutau_selec = mutau && SelectionID(kMuTau);
  bool etau_selec  = etau  && SelectionID(kETau );
  bool emu_selec   = emu   && SelectionID(kEMu  );
  bool mumu_selec  = mumu  && SelectionID(kMuMu );
  bool ee_selec    = ee    && SelectionID(kEE   );

  //if a good muon, electron, and tau, veto from tau categories (use actual selection IDs)
  if(mutau_selec && etau_selec) {mutau = false; mutau_selec = false; etau = false; etau_selec = false;}

  //Reject overlap of etau/mutau with emu
  if(emu_selec && (mutau_selec || etau_selec)) {mutau = false; mutau_selec = false; etau = false; etau_selec = false;}

   //if passes a signal region, don't use in ee/mumu categories
  if(ee + mumu > 0 && (emu_selec || etau_selec || mutau_selec)) {
    ee = false; ee_selec = false; mumu = false; mumu_selec = false;
  }

  //must pass at least one selection
  if(!mumu && !emu && !etau && !mutau && !ee) {
    ++fNFailed;
    if(!fIgnoreNoSelection && ((fVerbose > 0 && nMuonsSkim < 3 && nElectronsSkim < 3) || fVerbose > 1))
      std::cout << "!!! Warning! Event " << entry << " passes no selection! Skim flavors are: "
                << leptonOneSkimFlavor << " and " << leptonTwoSkimFlavor << std::endl;
    return kTRUE;
  }


  //remove single lepton data from category without corresponding lepton flavor
  ee          &= fIsData != 2;
  etau        &= fIsData != 2;
  mumu        &= fIsData != 1;
  mutau       &= fIsData != 1;
  mutau_selec &= mutau;
  etau_selec  &= etau ;
  emu_selec   &= emu  ;
  mumu_selec  &= mumu ;
  ee_selec    &= ee   ;

  //increment selection counts (for tight selection)
  if(emu_selec  )   ++fNEMu;
  if(etau_selec )  ++fNETau;
  if(mutau_selec) ++fNMuTau;
  if(mumu_selec )  ++fNMuMu;
  if(ee_selec   )    ++fNEE;
  if((int) (emu_selec+etau_selec+mutau_selec+mumu_selec+ee_selec) > 1) std::cout << "!!! Warning! Event " << entry << " passes multiple tight ID selections!\n"
                                                                                 << "mutau = " << mutau_selec << " etau = " << etau_selec << " emu = " << emu_selec
                                                                                 << " mumu = " << mumu_selec << " ee = " << ee_selec << std::endl;

  InitializeEventVariables(); //variables independent of selection

  /////////////////////////////////////
  // Selection Loop
  /////////////////////////////////////

  //loop through selections, in case an event passes multiple (shouldn't happen)
  for(int selection = kMuTau; selection < kSelections; ++selection) {
    //check if it passes the selection
    if(!((selection == kMuTau && mutau) || (selection == kETau && etau) ||
         (selection == kEMu && emu) || (selection == kMuMu && mumu) || (selection == kEE && ee)))
      continue;

    //Initialize all the relevant variables
    InitializeTreeVariables(selection);

    //Check is a same sign (SS) event if requested to skip them
    bool skipSS = fSkipMuMuEESS && ((selection == kMuMu && muonCharge    [leptonOneIndex] == muonCharge    [leptonTwoIndex]) ||
                                    (selection == kEE   && electronCharge[leptonOneIndex] == electronCharge[leptonTwoIndex]));

    //Skip the SS events if requested
    if(skipSS) {
      if(leptonOneSkimFlavor != leptonTwoSkimFlavor)
        std::cout << "!!! Warning! Event " << entry << " fails sign test, but skim flavors indicate different flavors!"
                  << " Skim flavors = " << leptonOneSkimFlavor << " and " << leptonTwoSkimFlavor
                  << ", indices = " << leptonOneIndex << " and " << leptonTwoIndex
                  << ", skim indices = " << leptonOneSkimIndex << " and " << leptonTwoSkimIndex
                  << " found selection = " << fSelecNames[selection].Data()
                  << std::endl;
      ++fNSkipped;
      continue;
    }

    //Check if passes tight ID requirements, if not mark for ABCD scale factor calculation
    bool isTight = false;
    switch(selection) {
    case kMuTau: isTight = mutau_selec; break;
    case kETau : isTight = etau_selec ; break;
    case kEMu  : isTight = emu_selec  ; break;
    case kMuMu : isTight = mumu_selec ; break;
    case kEE   : isTight = ee_selec   ; break;
    default    : isTight = false      ; break;
    }
    looseQCDSelection = QCDSelection(selection);
    if(!looseQCDSelection && !isTight) { //remove events passing neither the tight nor the ABCD measurement region selection
      continue;
    }

    if(looseQCDSelection) ++fQCDCounts[selection];


    if((selection == kMuMu || selection == kEE) && ((int) nElectrons != nElectronsSkim || (int) nMuons != nMuonsSkim))
      std::cout << fentry << ": Light lepton counts differ between skim and conversion! N(electrons) = " << nElectronsSkim
                << " --> " << nElectrons << ", N(muons) = " << nMuonsSkim << " --> " << nMuons
                << " (may be due to Rochester corrections changing muon pT)" << std::endl;

    //a trigger must be fired
    if(lepOneFired || lepTwoFired) {
      //skip electron data with muon trigger
      if(fIsData == 1 && ((abs(leptonOneFlavor) == 13 && lepOneFired) || (abs(leptonTwoFlavor) == 13 && lepTwoFired)))
          continue;
      //skip muon data without muon trigger
      if(fIsData == 2 && !(abs(leptonOneFlavor) == 13 && lepOneFired) && !(abs(leptonTwoFlavor) == 13 && lepTwoFired))
        continue;
      //fill the event tree
      fDirs[selection]->cd();
      fOutTrees[selection]->Fill();
    } else {
      ++fNFailedTrig;
    }
  } //end selection loop

  return kTRUE;
} //end process call

//-----------------------------------------------------------------------------------------------------------------
void NanoAODConversion::SlaveTerminate()
{
  // The SlaveTerminate() function is called after all entries or objects
  // have been processed. When running with PROOF SlaveTerminate() is called
  // on each slave server.

}

//-----------------------------------------------------------------------------------------------------------------
void NanoAODConversion::Terminate()
{
  // The Terminate() function is the last function to be called during
  // a query. It always runs on the client, it can be used to present
  // the results graphically or save the results to file.
  printf("Writing output file into %s\n",fOut->GetName());
  for(int selection = 0; selection < kSelections; ++selection) {
    fDirs[selection]->cd();
    fOutTrees[selection]->Write();
  }
  fOut->Write();//Form("%s_OutFile.root",fChain->GetName()));
  delete fOut;
  // timer->Stop();
  Double_t cpuTime = timer->CpuTime();
  Double_t realTime = timer->RealTime();
  printf("Processing time: %7.2fs CPU time %7.2fs Wall time\n",cpuTime,realTime);
  if(realTime > 600. ) printf("Processing time: %7.2fmin CPU time %7.2fmin Wall time\n",cpuTime/60.,realTime/60.);
  printf("Found:      %10i MuTau, %10i ETau, %10i EMu, %10i MuMu, %10i EE, %i failed trigger matching, %i failed, %i skipped, and %i high muon triggered events\n",
         fNMuTau, fNETau, fNEMu, fNMuMu, fNEE, fNFailedTrig, fNFailed, fNSkipped, fNHighMuonTriggered);

  printf("QCD counts: ");
  for(int i = kMuTau; i < kSelections; ++i) printf("%10i %s, ", fQCDCounts[i], fSelecNames[i].Data());
  printf("\n");
}
