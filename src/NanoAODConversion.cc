#define NanoAODConversion_cxx
// The class definition in NanoAODConversion.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.


// The following methods are defined in this file:
//    Begin():        called every time a loop on the tree starts,
//                    a convenient place to create your histograms.
//    SlaveBegin():   called after Begin(), when on PROOF called only on the
//                    slave servers.
//    Process():      called for each event, in this function you decide what
//                    to read and fill your histograms.
//    SlaveTerminate: called at the end of the loop on the tree, when on PROOF
//                    called only on the slave servers.
//    Terminate():    called at the end of the loop on the tree,
//                    a convenient place to draw/fit your histograms.
//
// To use this file, try the following session on your Tree T:
//
// root> T->Process("NanoAODConversion.C")
// root> T->Process("NanoAODConversion.C","some options")
// root> T->Process("NanoAODConversion.C+")
//


#include "interface/NanoAODConversion.hh"
#include <TStyle.h>

void NanoAODConversion::Begin(TTree * /*tree*/)
{
  // The Begin() function is called at the start of the query.
  // When running with PROOF Begin() is only called on the client.
  // The tree argument is deprecated (on PROOF 0 is passed).

  printf("NanoAODConversion::Begin\n");
  timer->Start();
  particleCorrections = new ParticleCorrections(fMuonIso);
  if(fVerbose > 0) particleCorrections->fVerbose = fVerbose - 1; //1 lower level, 1 further in depth
  fChain = 0;

  for(int i = kMuTau; i < kSelections; ++i) fQCDCounts[i] = 0;
  fSelecNames[kMuTau] = "MuTau";
  fSelecNames[kETau]  = "ETau";
  fSelecNames[kEMu]   = "EMu";
  fSelecNames[kMuMu]  = "MuMu";
  fSelecNames[kEE]    = "EE";
  //Object pT thresholds
  fElectronPtCount = 10.;
  fMuonPtCount = 10.;
  fTauPtCount = 20.;
  fPhotonPtCount = 10.;
  
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
  fTauAntiEleSelect[kMuTau] = 1;
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
  fElectronIDSelect[kEE]    = 2;

  //initialize object counting parameters for each selection
  //mutau
  fCountMuons       [kMuTau] = true;
  fMuonIsoCount     [kMuTau] = ParticleCorrections::kVLooseMuIso;
  fMuonIDCount      [kMuTau] = 1;
  fCountElectrons   [kMuTau] = true;
  fElectronIDCount  [kMuTau] = 1;
  fCountTaus        [kMuTau] = true;
  fTauAntiEleCount  [kMuTau] = 1;
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
  fTauAntiEleCount  [kETau]  = 50;
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
  fCountTaus        [kEMu]   = false;
  fPhotonIDCount    [kEMu]   = 1; //WP80
  fPhotonDeltaRCount[kEMu]   = 0.3;
  //mumu
  fCountMuons       [kMuMu]  = true;
  fMuonIsoCount     [kMuMu]  = ParticleCorrections::kVLooseMuIso;
  fMuonIDCount      [kMuMu]  = 1;
  fCountElectrons   [kMuMu]  = true;
  fElectronIDCount  [kMuMu]  = 1;
  fCountTaus        [kMuMu]  = true;
  fTauAntiEleCount  [kMuMu]  = 1;
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

void NanoAODConversion::SlaveBegin(TTree * /*tree*/)
{
  // The SlaveBegin() function is called after the Begin() function.
  // When running with PROOF SlaveBegin() is called on each slave server.
  // The tree argument is deprecated (on PROOF 0 is passed).

  printf("NanoAODConversion::SlaveBegin\n");

}

void NanoAODConversion::InitializeInBranchStructure(TTree* tree) {
  if(fIsData == 0) {
    tree->SetBranchAddress("genWeight"                       , &genWeight                      ) ;
    tree->SetBranchAddress("puWeight"                        , &puWeight                       ) ;
  }
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
  tree->SetBranchAddress("Muon_genPartFlav"                , &muonGenFlavor                  ) ;
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
  tree->SetBranchAddress("Electron_genPartFlav"            , &electronGenFlavor              ) ;
  tree->SetBranchAddress("nTau"                            , &nTau                           ) ;
  tree->SetBranchAddress("nTaus"                           , &nTausSkim                      ) ;
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
  tree->SetBranchAddress("nPhoton"                         , &nPhoton                        ) ;
  tree->SetBranchAddress("Photon_pt"                       , &photonPt                       ) ;
  tree->SetBranchAddress("Photon_eta"                      , &photonEta                      ) ;
  tree->SetBranchAddress("Photon_phi"                      , &photonPhi                      ) ;
  tree->SetBranchAddress("Photon_mass"                     , &photonMass                     ) ;
  tree->SetBranchAddress("Photon_mvaID"                    , &photonMVAID                    ) ;
  tree->SetBranchAddress("Photon_mvaID_WP80"               , &photonWP80                     ) ;
  tree->SetBranchAddress("Photon_mvaID_WP90"               , &photonWP90                     ) ;
  tree->SetBranchAddress("HLT_IsoMu24"                     , &HLT_IsoMu24                    ) ;
  tree->SetBranchAddress("HLT_IsoMu27"                     , &HLT_IsoMu27                    ) ;
  tree->SetBranchAddress("HLT_Mu50"                        , &HLT_Mu50                       ) ;
  tree->SetBranchAddress("HLT_Ele27_WPTight_Gsf"           , &HLT_Ele27_WPTight_GsF          ) ;
  tree->SetBranchAddress("HLT_Ele32_WPTight_Gsf"           , &HLT_Ele32_WPTight_GsF          ) ;
  tree->SetBranchAddress("HLT_Ele32_WPTight_Gsf_L1DoubleEG", &HLT_Ele32_WPTight_GsF_L1DoubleEG) ;
  tree->SetBranchAddress("PuppiMET_pt"                     , &puppMET                        ) ;
  tree->SetBranchAddress("PuppiMET_phi"                    , &puppMETphi                     ) ;
  tree->SetBranchAddress("PV_npvsGood"                     , &nGoodPV                        ) ;
  if(fIsData == 0) {
    tree->SetBranchAddress("Pileup_nPU"                      , &nPUAdded                       ) ;
    tree->SetBranchAddress("Pileup_nTrueInt"                 , &nPU                            ) ;
  }
  tree->SetBranchAddress("nTrigObj"                        , &nTrigObjs                      ) ;
  tree->SetBranchAddress("TrigObj_filterBits"              , &trigObjFilterBits              ) ;
  tree->SetBranchAddress("TrigObj_pt"                      , &trigObjPt                      ) ;
  tree->SetBranchAddress("TrigObj_eta"                     , &trigObjEta                     ) ;
  tree->SetBranchAddress("TrigObj_phi"                     , &trigObjPhi                     ) ;
  tree->SetBranchAddress("TrigObj_id"                      , &trigObjID                      ) ;

  tree->SetBranchAddress("leptonOneIndex"                  , &leptonOneSkimIndex             ) ;
  tree->SetBranchAddress("leptonTwoIndex"                  , &leptonTwoSkimIndex             ) ;
  tree->SetBranchAddress("leptonOneFlavor"                 , &leptonOneSkimFlavor            ) ;
  tree->SetBranchAddress("leptonTwoFlavor"                 , &leptonTwoSkimFlavor            ) ;
  tree->SetBranchAddress("zPt"                             , &zPtIn                          ) ;
  tree->SetBranchAddress("zMass"                           , &zMassIn                        ) ;
  
}

void NanoAODConversion::InitializeOutBranchStructure(TTree* tree) {
  if(!leptonOneP4)   leptonOneP4   = new TLorentzVector();
  if(!leptonTwoP4)   leptonTwoP4   = new TLorentzVector();
  if(!leptonOneSVP4) leptonOneSVP4 = new TLorentzVector();
  if(!leptonTwoSVP4) leptonTwoSVP4 = new TLorentzVector();
  if(!jetP4)         jetP4         = new TLorentzVector();
  if(!photonP4)      photonP4      = new TLorentzVector();
  
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
  tree->Branch("lepOneWeight"                  , &lepOneWeight         );
  tree->Branch("lepTwoWeight"                  , &lepTwoWeight         );
  tree->Branch("lepOneTrigWeight"              , &lepOneTrigWeight     );
  tree->Branch("lepTwoTrigWeight"              , &lepTwoTrigWeight     );
  tree->Branch("lepOneFired"                   , &lepOneFired          );
  tree->Branch("lepTwoFired"                   , &lepTwoFired          );
  tree->Branch("topPtWeight"                   , &topPtWeight          );
  tree->Branch("zPtWeight"                     , &zPtWeight            );
  tree->Branch("zPt"                           , &zPtOut               );
  tree->Branch("zMass"                         , &zMassOut             );
  tree->Branch("looseQCDSelection"             , &looseQCDSelection    );
  tree->Branch("genTauFlavorWeight"            , &genTauFlavorWeight   );
  tree->Branch("tauEnergyScale"                , &tauEnergyScale       );
  tree->Branch("tauDecayMode"                  , &tauDecayModeOut      );
  tree->Branch("tauMVA"                        , &tauMVA               );
  tree->Branch("tauGenID"                      , &tauGenIDOut          );
  tree->Branch("tauGenFlavor"                  , &tauGenFlavor         );
  tree->Branch("tauGenFlavorHad"               , &tauGenFlavorHad      );
  tree->Branch("tauVetoedJetPt"                , &tauVetoedJetPt       );
  tree->Branch("tauVetoedJetPtUnc"             , &tauVetoedJetPtUnc    );
  tree->Branch("leptonOneP4"                   , &leptonOneP4          );
  tree->Branch("leptonTwoP4"                   , &leptonTwoP4          );
  tree->Branch("leptonOneFlavor"               , &leptonOneFlavor      );
  tree->Branch("leptonTwoFlavor"               , &leptonTwoFlavor      );
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
  tree->Branch("leptonOneSkimIndex"            , &leptonOneSkimIndex   );
  tree->Branch("leptonTwoSkimIndex"            , &leptonTwoSkimIndex   );
  tree->Branch("leptonOneSkimFlavor"           , &leptonOneSkimFlavor  );
  tree->Branch("leptonTwoSkimFlavor"           , &leptonTwoSkimFlavor  );
  tree->Branch("genLeptonOneP4"                , &genLeptonOneP4       );
  tree->Branch("genLeptonTwoP4"                , &genLeptonTwoP4       );
  tree->Branch("photonP4"                      , &photonP4             );
  tree->Branch("photonIDWeight"                , &photonIDWeight       );
  tree->Branch("jetP4"                         , &jetP4                );
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
  tree->Branch("nFwdJets"                      , &nFwdJets             );
  tree->Branch("nBJets"                        , &nBJets               );
  tree->Branch("nBJetsM"                       , &nBJetsM              );
  tree->Branch("nBJetsL"                       , &nBJetsL              );
  tree->Branch("nBJets20"                      , &nBJets20             );
  tree->Branch("nBJets20M"                     , &nBJets20M            );
  tree->Branch("nBJets20L"                     , &nBJets20L            );
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

}

//calculate the weight for b-tagging corrections
float NanoAODConversion::BTagWeight(int WP) {
  //loop through each jet, getting the P(b-tag) in data and MC
  double p_data(1.), p_mc(1.);
  UInt_t nbjets = 0;
  if(fVerbose > 1) std::cout << "NanoAODConversion::" << __func__ 
			     << ": printing jet b-tag info for WP = "
			     << WP << "...\n";
  for(unsigned jet = 0; jet < nJets20; ++jet) {
    if(fabs(jetsEta[jet]) >= 2.4) continue;
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
    std::cout << "Entry " << fentry << " Warning! btag nbjets = " << nbjets 
	      << " doesn't equal expected " << nexpected << "!\n";

  if(fVerbose > 1 || p_data <= 0. || p_mc <= 0.) {
    if(p_data <= 0. || p_mc <= 0.) std::cout << "Warning! <= 0 b-tag probabilities! ";
    std::cout << "NanoAODConversion::" << __func__ << " entry " << fentry << ": p_data = " << p_data
	      << " p_mc = " << p_mc << " scale = " << p_data/p_mc
	      << " njets = " << nJets20 << " nbjets = " << nbjets << std::endl;    
  }
  if(p_mc <= 0. || p_data <= 0.) return 1.;
  float scale_factor = p_data / p_mc;
  return scale_factor;
}

void NanoAODConversion::InitializeTreeVariables(Int_t selection) {
  //store sign of generator weight
  genWeight = (genWeight == 0.) ? 0. : genWeight/abs(genWeight);

  //////////////////////////////
  //           MET            //
  //////////////////////////////

  //use PUPPI MET by default
  met = puppMET;
  metPhi = puppMETphi;
  //set corrected PUPPI MET to be nominal
  puppMETC    = puppMET   ;
  puppMETCphi = puppMETphi;

  
  //////////////////////////////
  //      Lepton info         //
  //////////////////////////////
  nMuons = fNMuons[selection];
  nElectrons = fNElectrons[selection];
  nTaus = fNTaus[selection];
  lepOneWeight     = 1.; lepTwoWeight     = 1.;
  lepOneTrigWeight = 1.; lepTwoTrigWeight = 1.;
  lepOneFired      = 0 ; lepTwoFired      = 0 ;
  if(fVerbose > 1) std::cout << "nElectrons = " << nElectrons
			     << " nMuons = " << nMuons
			     << " nTaus = " << nTaus
			     << " nPhotons = "  << nPhotons
			     << std::endl;

  //////////////////////////////
  //        Trigger           //
  //////////////////////////////

  //store muon and electron trigger (1 = electron, 2 = muon, 3 = both)
  //muon trigger is Mu50 for all years and IsoMu24 for 2016, 2018 and IsoMu27 for 2017
  bool lowMuonTriggered  = nMuons > 0 && ((fYear == ParticleCorrections::k2016 && HLT_IsoMu24) || 
					  (fYear == ParticleCorrections::k2017 && HLT_IsoMu27) || 
					  (fYear == ParticleCorrections::k2018 && HLT_IsoMu24));
  bool highMuonTriggered = nMuons > 0 && HLT_Mu50;
  double muon_lo_pt = (fYear == ParticleCorrections::k2017) ? 28. : 25.;
  double muon_hi_pt = 50.; //independent of year
  bool electronTriggered = nElectrons > 0 &&
    ((fYear == ParticleCorrections::k2016 && HLT_Ele27_WPTight_GsF) ||
     (fYear == ParticleCorrections::k2017 && HLT_Ele32_WPTight_GsF_L1DoubleEG) || //FIXME: add L1 Ele35 test as well
     (fYear == ParticleCorrections::k2018 && HLT_Ele32_WPTight_GsF));
  double elec_pt = (fYear == ParticleCorrections::k2016) ? 28. : 33.;
  
  triggerLeptonStatus = electronTriggered + 2*(lowMuonTriggered || highMuonTriggered); //bit 1: electron bit 2: muon
  muonTriggerStatus = lowMuonTriggered + 2*highMuonTriggered; //store which triggers were fired
  
  bool useTrigObj = nTrigObjs > 0; //do actual trigger matching if trigger objects are available
  if(!useTrigObj) std::cout << "NanoAODConversion::" << __func__
			    << ": Warning! No trigger objects available in event "
			    << fentry << std::endl;

  int trigMatchOne = 0; //trigger matching result
  int trigMatchTwo = 0; //trigger matching result
  
  int trigger = -1; //muon weights need to know which trigger to use
  if(selection == kMuTau || selection == kEMu || selection == kMuMu)
    //default to low trigger if it passed it, < 0 if no trigger
    trigger = (lowMuonTriggered) ? ParticleCorrections::kLowTrigger : (2*highMuonTriggered - 1)*ParticleCorrections::kHighTrigger;
  if(!lowMuonTriggered && !highMuonTriggered && (selection == kMuMu || selection == kMuTau)) 
    std::cout << "NanoAODConversion::" << __func__
	      << "Warning! Muon only selection but no identified muon trigger in event "
	      << fentry << std::endl;
  else if(!electronTriggered && (selection == kEE || selection == kETau)) 
    std::cout << "NanoAODConversion::" << __func__
	      << "Warning! Electron only selection but no identified electron trigger in event "
	      << fentry << std::endl;
  else if(!electronTriggered && !lowMuonTriggered && !highMuonTriggered)
    std::cout << "NanoAODConversion::" << __func__
	      << "Warning! Passed a selection but no identified trigger in event "
	      << fentry << std::endl;
  
  //////////////////////////////
  //   Z info + re-weight     //
  //////////////////////////////
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

  //for multiple potential triggers fired, consider efficiencies instead of scale factors
  float data_eff[2], mc_eff[2];
  int trigger_index = 0;

  //reset lepton IDs
  leptonOneID1 = 0; leptonOneID2 = 0;
  leptonTwoID1 = 0; leptonTwoID2 = 0; leptonTwoID3 = 0;
  
  //////////////////////////////
  //      lep 1 = muon        //
  //////////////////////////////
  if(selection == kMuTau || selection == kMuMu) {
    unsigned index = (selection == kMuMu) ? leptonOneIndex : fMuonIndices[selection][0];
    leptonOneP4->SetPtEtaPhiM(muonPt[index], muonEta[index],
			      muonPhi[index], muonMass[index]);
    leptonOneFlavor = -13*muonCharge[index];
    leptonOneID1 = muonIsoId[index];
    leptonOneID2 = 0;
    leptonOneIso = muonRelIso[index]*muonPt[index];
    leptonOneIndex = index;
    trigMatchOne = GetTriggerMatch(index, true);
    if(!fIsData) {
      if(useTrigObj) {
	if(trigMatchOne == 0) trigger = -1;
	else                  trigger = (trigMatchOne == 1 || trigMatchOne == 3) ? ParticleCorrections::kLowTrigger : ParticleCorrections::kHighTrigger;
      }
      lepOneWeight = particleCorrections->MuonWeight(muonPt[index], muonEta[index], trigger, fYear, lepOneTrigWeight);
      if(trigger > -1) {
	particleCorrections->MuonTriggerEff(muonPt[index], muonEta[index], trigger, fYear, data_eff[trigger_index], mc_eff[trigger_index]);
	++trigger_index;
      } else
	lepOneTrigWeight = 1.; //no weight if didn't trigger
    }
    if(lowMuonTriggered || highMuonTriggered)
      lepOneFired = (useTrigObj) ? trigMatchOne > 0 : (lowMuonTriggered && muonPt[index] > muon_lo_pt) || (highMuonTriggered && muonPt[index] > muon_hi_pt);
  //////////////////////////////
  //     lep 1 = electron     //
  //////////////////////////////
  } else if(selection == kETau || selection == kEMu || selection == kEE) {
    unsigned index = (selection == kEE) ? leptonOneIndex : fElectronIndices[selection][0];
    leptonOneP4->SetPtEtaPhiM(electronPt[index], electronEta[index],
			      electronPhi[index], electronMass[index]);
    leptonOneFlavor = -11*electronCharge[index];
    leptonOneID1 = electronWPL[index] + 2*electronWP90[index] + 4*electronWP80[index];
    leptonOneID2 = 0;
    leptonOneIndex = index;
    trigMatchOne = GetTriggerMatch(index, false);
    if(!fIsData) {
      lepOneWeight = particleCorrections->ElectronWeight(electronPt[index], electronEta[index]+electronDeltaEtaSC[index], fYear, lepOneTrigWeight);
      if((trigMatchOne > 0 && useTrigObj) || (!useTrigObj && electronTriggered)) {
	particleCorrections->ElectronTriggerEff(electronPt[index], electronEta[index]+electronDeltaEtaSC[index], fYear, data_eff[trigger_index], mc_eff[trigger_index]);
	++trigger_index;
      } else
	lepOneTrigWeight = 1.; //no weight if didn't trigger
    }
    if(electronTriggered) 
      lepOneFired = (useTrigObj) ? trigMatchOne > 0 : electronPt[index] > elec_pt;
  }    
  //////////////////////////////
  //      lep 2 = tau         //
  //////////////////////////////
  if(selection == kMuTau || selection == kETau) {
    unsigned index = fTauIndices[selection][0];
    leptonTwoP4->SetPtEtaPhiM(tauPt[index], tauEta[index],
			      tauPhi[index],tauMass[index]);
    leptonTwoFlavor = -15*tauCharge[index];
    tauGenIDOut  = tauGenID[index];
    tauGenFlavor = TauFlavorFromID((int) tauGenIDOut);
    lepTwoWeight = particleCorrections->TauWeight(leptonTwoP4->Pt(), leptonTwoP4->Eta(), tauGenID[index], fYear);
    leptonTwoID1 = tauAntiEle[index]; //MVA ID
    leptonTwoID2 = tauAntiMu[index];  //MVA ID
    leptonTwoID3 = tauAntiJet[index];  //MVA ID
    taudxyOut = taudxy[index];
    taudzOut  = taudz[index];
    leptonTwoIndex = index;
    tauDecayModeOut = tauDecayMode[index];
    
    //FIXME: should ID weight use updated tau energy scale?
    double up(1.), down(1.);    
    tauEnergyScale = particleCorrections->TauEnergyScale(leptonTwoP4->Pt(), leptonTwoP4->Eta(), tauDecayModeOut,
							 tauGenID[index], fYear, up, down); //FIXME: keep uncertainties
    if(tauEnergyScale <= 0.)
      std::cout << "WARNING! In event " << fentry
		<< " tau energy scale <= 0 = " << tauEnergyScale
		<< std::endl;
    *leptonTwoP4 *= tauEnergyScale;
    tauDeepAntiEle = tauDeep2017VsE[index];
    tauDeepAntiMu  = tauDeep2017VsMu[index];
    tauDeepAntiJet = tauDeep2017VsJet[index];
  //////////////////////////////
  //      lep 2 = muon        //
  //////////////////////////////
  } else if(selection == kEMu) {
    unsigned index = fMuonIndices[selection][0];
    leptonTwoP4->SetPtEtaPhiM(muonPt[index], muonEta[index],
			      muonPhi[index],muonMass[index]);
    leptonTwoFlavor = -13*muonCharge[index];
    leptonTwoID1 = muonIsoId[index];
    leptonTwoID2 = 0;
    leptonTwoIso = muonRelIso[index]*muonPt[index];
    leptonTwoIndex = index;
    trigMatchTwo = GetTriggerMatch(index, true);
    if(useTrigObj) {
      if(trigMatchOne == 0) trigger = -1;
      else                  trigger = (trigMatchTwo == 1 || trigMatchTwo == 3) ? ParticleCorrections::kLowTrigger : ParticleCorrections::kHighTrigger;
    }
    if(!fIsData) {
      lepTwoWeight = particleCorrections->MuonWeight(muonPt[index], muonEta[index], trigger, fYear, lepTwoTrigWeight);
      if(trigger > -1) {
	particleCorrections->MuonTriggerEff(muonPt[index], muonEta[index], trigger, fYear, data_eff[trigger_index], mc_eff[trigger_index]);
	++trigger_index;
      } else
	lepTwoTrigWeight = 1.; //no weight if didn't trigger
    }
    if(lowMuonTriggered || highMuonTriggered)
      lepTwoFired = (useTrigObj) ? trigMatchTwo > 0 : (lowMuonTriggered && muonPt[index] > muon_lo_pt) || (highMuonTriggered && muonPt[index] > muon_hi_pt);
  //////////////////////////////
  //      lep 2 = muon(2)     //
  //////////////////////////////
  } else if(selection == kMuMu) {
    unsigned index = leptonTwoIndex; //fMuonIndices[selection][1];
    leptonTwoP4->SetPtEtaPhiM(muonPt[index], muonEta[index],
			      muonPhi[index], muonMass[index]);
    leptonTwoFlavor = -13*muonCharge[index];
    leptonTwoID1 = muonIsoId[index];
    leptonTwoID2 = 0;
    leptonTwoIndex = index;
    trigMatchTwo = GetTriggerMatch(index, true);
    if(useTrigObj) {
      if(trigMatchTwo == 0) trigger = -1;
      else                  trigger = (trigMatchTwo == 1 || trigMatchTwo == 3) ? ParticleCorrections::kLowTrigger : ParticleCorrections::kHighTrigger;
    }
    if(!fIsData) {
      lepTwoWeight = particleCorrections->MuonWeight(muonPt[index], muonEta[index], trigger, fYear, lepTwoTrigWeight);
      if(trigger > -1) {
	particleCorrections->MuonTriggerEff(muonPt[index], muonEta[index], trigger, fYear, data_eff[trigger_index], mc_eff[trigger_index]);
	++trigger_index;
      } else
	lepTwoTrigWeight = 1.; //no weight if didn't trigger
    }
    if(lowMuonTriggered || highMuonTriggered)
      lepTwoFired = (useTrigObj) ? trigMatchTwo > 0 : (lowMuonTriggered && muonPt[index] > muon_lo_pt) || (highMuonTriggered && muonPt[index] > muon_hi_pt);
  //////////////////////////////
  //    lep 2 = electron(2)   //
  //////////////////////////////
  } else if(selection == kEE) {
    unsigned index = leptonTwoIndex; //fElectronIndices[selection][1];
    leptonTwoP4->SetPtEtaPhiM(electronPt[index], electronEta[index],
			      electronPhi[index], electronMass[index]);
    leptonTwoFlavor = -11*electronCharge[index];
    leptonTwoID1 = electronWPL[index] + 2*electronWP90[index] + 4*electronWP80[index];
    leptonTwoID2 = 0;
    leptonTwoIndex = index;
    trigMatchTwo = GetTriggerMatch(index, false);
    if(!fIsData) {
      lepTwoWeight = particleCorrections->ElectronWeight(electronPt[index], electronEta[index]+electronDeltaEtaSC[index], fYear, lepTwoTrigWeight);
      if((trigMatchTwo > 0 && useTrigObj) || (!useTrigObj && electronTriggered)) {
	particleCorrections->ElectronTriggerEff(electronPt[index], electronEta[index]+electronDeltaEtaSC[index], fYear, data_eff[trigger_index], mc_eff[trigger_index]);
	++trigger_index;
      } else
	lepTwoTrigWeight = 1.; //no weight if didn't trigger
    }
    if(electronTriggered)
      lepTwoFired = (useTrigObj) ? trigMatchTwo > 0 : electronPt[index] > elec_pt;
  }
  //don't scale trigger leptons if they didn't fire a trigger...
  if(!lepOneFired) lepOneTrigWeight = 1.;
  if(!lepTwoFired) lepTwoTrigWeight = 1.;

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

  //////////////////////////////
  //    Store other objects   //
  //////////////////////////////
  if(!(selection == kMuTau || selection == kETau)) {
    if(nTaus > 0) {
      unsigned index = fTauIndices[selection][0];
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
    CountTaus(selection);
  }
  CountLightLeptons(selection);
  
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
    Float_t wtmp = lepOneWeight;
    lepOneWeight = lepTwoWeight;
    lepTwoWeight = wtmp;
    wtmp = lepOneTrigWeight;
    lepOneTrigWeight = lepTwoTrigWeight;
    lepTwoTrigWeight = wtmp;
    //swap trig bools
    Bool_t ttmp = lepOneFired;
    lepOneFired = lepTwoFired;
    lepTwoFired = ttmp;
    //swap indices
    Int_t itmp = leptonOneIndex;
    leptonOneIndex = leptonTwoIndex;
    leptonTwoIndex = itmp;
  }
  nPhotons = fNPhotons[selection];
  if(fNPhotons[selection] > 0) {
    photonP4->SetPtEtaPhiM(photonPt[fPhotonIndices[selection][0]], photonEta[fPhotonIndices[selection][0]],
			   photonPhi[fPhotonIndices[selection][0]], photonMass[fPhotonIndices[selection][0]]); 
    if(!fIsData) photonIDWeight = particleCorrections->PhotonWeight(photonP4->Pt(), photonP4->Eta(), fYear);
  } else {
    photonP4->SetPtEtaPhiM(0., 0., 0., 0.);
    photonIDWeight = 1.;
  }
  
  if(!fIsData) {
    if(trigger_index > 1) { //more than one trigger fired, use efficiencies combined
      float trig_correction = particleCorrections->CombineEfficiencies(data_eff[0], mc_eff[0], data_eff[1], mc_eff[1]);
      lepOneTrigWeight = (trig_correction >= 0.) ? sqrt(trig_correction) : 1.; //split evenly between them, doesn't matter though
      lepTwoTrigWeight = (trig_correction >= 0.) ? sqrt(trig_correction) : 1.;
      if(trigger_index > 2)
	std::cout << "WARNING! Counting of trigger objects found "
		  << trigger_index << " objects in entry "
		  << fentry << std::endl;
    }
    eventWeight = lepOneWeight * lepTwoWeight * lepOneTrigWeight * lepTwoTrigWeight * puWeight * photonIDWeight * btagWeight;
    if(fIsDY) eventWeight *= zPtWeight;
  } else {
    eventWeight = 1.;
    genWeight = 1.;
    puWeight = 1.;
    lepOneWeight = 1.;
    lepTwoWeight = 1.;
    lepOneTrigWeight = 1.;
    lepTwoTrigWeight = 1.;
    zPtWeight = 1.;
    photonIDWeight = 1.;
    btagWeight = 1.;
  }
  if(eventWeight <= 0. || fVerbose > 1 /*|| (useTrigObj && trigMatchOne == 0 && trigMatchTwo == 0)*/ ) {
    if(eventWeight <= 0.)
      std::cout << "WARNING! Eventweight <= 0. in entry " << fentry << "!\n";
    if(useTrigObj && trigMatchOne == 0 && trigMatchTwo == 0)
      std::cout << "WARNING! No trigger match found in entry " << fentry << "!\n";
    std::cout << "eventWeight = " << eventWeight
	      << " genWeight = " << genWeight
	      << " puWeight = " << puWeight
	      << " btagWeight = " << btagWeight
	      << " photonIDWeight = " << photonIDWeight << std::endl
	      << " lepOneWeight = " << lepOneWeight
	      << " lepTwoWeight = " << lepTwoWeight 
	      << " muonLowTriggered = " << lowMuonTriggered
	      << " muonHighTriggered = " << highMuonTriggered
	      << " electronTriggered = " << electronTriggered << std::endl
	      << " lepOneTrigWeight = " << lepOneTrigWeight
	      << " lepTwoTrigWeight = " << lepTwoTrigWeight
	      << " lepOneFired = " << lepOneFired
	      << " lepTwoFired = " << lepTwoFired
	      << " trigMatchOne = " << trigMatchOne
	      << " trigMatchTwo = " << trigMatchTwo      
	      << std::endl;
  }
  
  //save npv as ngoodpv for now
  nPV = nGoodPV;
}

//count jets separately, due to delta R cut from selection leptons
void NanoAODConversion::CountJets(int selection) {
  //Jet loop
  unsigned njets = nJet;
  //reset counters
  nJets = 0; nJets20 = 0; nFwdJets = 0;
  nBJets = 0; nBJetsL = 0; nBJetsM = 0;
  nBJets20 = 0; nBJets20L = 0; nBJets20M = 0;
  ht = 0.; htPhi = 0.; htSum = 0.;
  jetP4->SetPtEtaPhiM(0., 0., 0., 0.);
  float jetptmax = -1.;
  int jetIDFlag = 1; 
  int jetPUIDFlag = 4;
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
    if((jetpt < 50. && jetPUID[index] < jetPUIDFlag) || jetID[index] < jetIDFlag) //bad jet
      continue;

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

    /** check photons **/
    for(UInt_t iphoton = 0; iphoton < fNPhotons[selection]; ++iphoton) {
      lv.SetPtEtaPhiM(photonPt[ fPhotonIndices[selection][iphoton]], photonEta[ fPhotonIndices[selection][iphoton]],
		      photonPhi[fPhotonIndices[selection][iphoton]], photonMass[fPhotonIndices[selection][iphoton]]);
      passDeltaR &= jetLoop->DeltaR(lv) > deltaRMin;
      if(!passDeltaR) break;
    }
    //if fails, move to next jet
    if(!passDeltaR) continue; 
    
    //////////////////////////
    // Count jet, check eta //
    //////////////////////////
    
    if(abs(jetEta[index]) < 2.4) {
      //store the hardest jet
      if(jetptmax < jetpt) {
	jetptmax = jetpt;
	jetP4->SetPtEtaPhiM(jetPt[index], jetEta[index], jetPhi[index], jetMass[index]);
      }
      if(jetpt > 25.) ++nJets;
      ++nJets20;

      htLV += *jetLoop;
      htSum += jetpt;
      int jetBTag = 0;
      //check if b-tagged, must have eta < 2.4
      if(fabs(jetEta[index]) < 2.4) {
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

//count and create maps for reconstructed objects
void NanoAODConversion::CountObjects() {
  //reset counters
  for(Int_t selection = kMuTau; selection < kSelections; ++selection) {
    fNTaus[selection] = 0;
    fNElectrons[selection] = 0;
    fNMuons[selection] = 0;
    fNPhotons[selection] = 0;
  }
  
  //count muons
  for(Int_t index = 0; index < std::min(((int)nMuon),((int)kMaxParticles)); ++index) {
    //initialize the muon iso IDs
    muonIsoId[index] = 0; //initially fails all IDs
    for(int level = ParticleCorrections::kVLooseMuIso; level <= ParticleCorrections::kVVTightMuIso; ++level) {
      // if(!particleCorrections) std::cout << "ERROR! Particle Corrections not defined!\n";
      // if(level >= sizeof(particleCorrections->muonIsoValues)/sizeof(double)) break;
      // std::cout << "level = " << level << ", address = " << &(particleCorrections->muonIsoValues[level]);
      double isoval = particleCorrections->muonIsoValues[level];
      if(isoval > muonRelIso[index])
	muonIsoId[index] += 1; 
      else
	break;
    }
    for(Int_t selection = kMuTau; selection < kSelections; ++selection) {
      if(fCountMuons[selection] &&
	 muonIsoId[index] > fMuonIsoCount[selection] &&
	 (fMuonIDCount[selection] == 0 ||
	  (fMuonIDCount[selection] == 1 && muonLooseId[index]) ||
	  (fMuonIDCount[selection] == 2 && muonMediumId[index]) ||
	  (fMuonIDCount[selection] == 3 && muonTightId[index])) &&
	 muonPt[index] > fMuonPtCount &&
	 fabs(muonEta[index]) < fMuonEtaCount) {
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
  //count electrons
  for(Int_t index = 0; index < std::min(((int)kMaxParticles),((int)nElectron)); ++index) {
    for(Int_t selection = kMuTau; selection < kSelections; ++selection) {
      double elec_eta = fabs(electronEta[index] + electronDeltaEtaSC[index]);
      if(fCountElectrons[selection] &&
	 (fElectronIDCount[selection] == 0 ||
	  (fElectronIDCount[selection] == 1 && electronWPL[index]) ||
	  (fElectronIDCount[selection] == 2 && electronWP80[index]) ||
	  (fElectronIDCount[selection] == 3 && electronWP90[index])) &&
	 electronPt[index] > fElectronPtCount && 
	  elec_eta < fElectronEtaCount &&
	 (elec_eta <  1.442 || elec_eta > 1.566)) {
	fElectronIndices[selection][fNElectrons[selection]] = index;
	++fNElectrons[selection];
	if(fVerbose > 2) std::cout << "Accepting electron " << index << " for selection " << fSelecNames[selection].Data() << " with pt = " << electronPt[index]
				   << " eta = " << electronEta[index]
				   << " loose id = " << electronWPL[index]
				   << " medium id = " << electronWP80[index] << " tight id = " << electronWP90[index]
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
  //count taus
  for(Int_t index = 0; index < std::min(((int)kMaxParticles),((int)nTau)); ++index) {
    TLorentzVector lv1, lv2;
    lv2.SetPtEtaPhiM(tauPt[index], tauEta[index], tauPhi[index], tauMass[index]);
    if(tauPt[index] < fTauPtCount || fabs(tauEta[index]) > fTauEtaCount) continue; //continue to next tau
    for(Int_t selection = kMuTau; selection < kSelections; ++selection) {
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
      if(fCountTaus[selection] &&
	 ((!fTauUseDeep[selection] &&
	   tauAntiEle[index] >= fTauAntiEleCount[selection] && //MVA based IDs
	   tauAntiMu[index] >= fTauAntiMuCount[selection] &&
	   (!fTauIDDecayCount[selection] || tauIDDecayMode[index])) ||
	  (fTauUseDeep[selection] &&
	   tauDeep2017VsE[index] >= fTauAntiEleCount[selection] && //Deep NN based IDs
	   tauDeep2017VsMu [index] >= fTauAntiMuCount [selection] &&
	   tauAntiMu[index] >= fTauAntiOldMuCount[selection] && //Old MVA ID due to 2016 QCD excess
	   tauDeep2017VsJet[index] >= fTauAntiJetCount[selection] &&
	   (!fTauIDDecayCount[selection] || tauIDDecayModeNew[index])))) {
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
  //count photons
  for(Int_t index = 0; index < std::min((int)kMaxParticles,(int)nPhoton); ++index) {
    TLorentzVector lv1, lv2;
    lv2.SetPtEtaPhiM(photonPt[index], photonEta[index], photonPhi[index], photonMass[index]);
    for(Int_t selection = kMuTau; selection < kSelections; ++selection) {
      /** Selection cuts on photon **/
      if(fabs(photonEta[index]) > 2.5 /* FIXME: Add barrel endcap overlap region rejection! */) continue;
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
    tausIsPositive[ntau] = (tauCharge[index] > 0);
    tausAntiJet   [ntau] = tauDeep2017VsJet[index];
    tausAntiMu    [ntau] = tauDeep2017VsMu[index];
    tausMVAAntiMu [ntau] = tauAntiMu[index];
    tausAntiEle   [ntau] = tauDeep2017VsE[index];
    tausDM        [ntau] = tauDecayMode[index];
    tausGenFlavor [ntau] = TauFlavorFromID((int) tauGenID[index]);
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
    passed &= muonIsoId[index] > fMuonIsoSelect[selection];
    passed &=  fMuonIDSelect[selection] == 0 ||
	      (fMuonIDSelect[selection] == 1 && muonLooseId [index]) ||
	      (fMuonIDSelect[selection] == 2 && muonMediumId[index]) ||
	      (fMuonIDSelect[selection] == 3 && muonTightId [index]);
    if(fVerbose > 2) std::cout << "SelectionID: After Muon 1 has status " << passed << std::endl;
  }
  if(!passed) return false;
  if(selection == kMuMu) {
    int index = (selection == kMuMu) ? leptonTwoSkimIndex : fMuonIndices[selection][1];
    passed &= muonIsoId[index] > fMuonIsoSelect[selection];
    passed &=  fMuonIDSelect[selection] == 0 ||
	      (fMuonIDSelect[selection] == 1 && muonLooseId [index]) ||
	      (fMuonIDSelect[selection] == 2 && muonMediumId[index]) ||
	      (fMuonIDSelect[selection] == 3 && muonTightId [index]);
    if(fVerbose > 2) std::cout << "SelectionID: After Muon 2 has status " << passed << std::endl;
  }
  if(!passed) return false;
  //check electron
  if(selection == kETau || selection == kEMu || selection == kEE) {
    int index = (selection == kEE) ? leptonOneSkimIndex : fElectronIndices[selection][0];
    passed &= fElectronIDSelect[selection] == 0 ||
             (fElectronIDSelect[selection] == 1 && electronWPL [index]) ||
             (fElectronIDSelect[selection] == 2 && electronWP80[index]) ||
             (fElectronIDSelect[selection] == 3 && electronWP90[index]);

    if(fVerbose > 2) std::cout << "SelectionID: After Electron 1 has status " << passed << std::endl;
  }
  if(!passed) return false;
  if(selection == kEE) {
    int index = (selection == kEE) ? leptonTwoSkimIndex : fElectronIndices[selection][1];
    passed &= fElectronIDSelect[selection] == 0 ||
             (fElectronIDSelect[selection] == 1 && electronWPL [index]) ||
             (fElectronIDSelect[selection] == 2 && electronWP80[index]) ||
             (fElectronIDSelect[selection] == 3 && electronWP90[index]);

    if(fVerbose > 2) std::cout << "SelectionID: After Electron 2 has status " << passed << std::endl;
  }
  if(!passed) return false;
  //check tau
  if(selection == kMuTau || selection == kETau) {
    if(!fTauUseDeep[selection])
      passed &= tauAntiEle[fTauIndices[selection][0]] >= fTauAntiEleSelect[selection] && //MVA based IDs
	tauAntiMu[fTauIndices[selection][0]] >= fTauAntiMuSelect[selection] &&
	(!fTauIDDecaySelect[selection] || tauIDDecayMode[fTauIndices[selection][0]]);
    else
      passed &= (tauDeep2017VsE  [fTauIndices[selection][0]] >= fTauAntiEleSelect[selection] && //Deep NN based IDs
		 tauDeep2017VsMu [fTauIndices[selection][0]] >= fTauAntiMuSelect [selection] &&
		 tauDeep2017VsJet[fTauIndices[selection][0]] >= fTauAntiJetSelect[selection] &&
		 (!fTauIDDecaySelect[selection] || tauIDDecayModeNew[fTauIndices[selection][0]]));

    if(fVerbose > 2) std::cout << "SelectionID: After Tau 1 has status " << passed << std::endl;
  }
  return passed;
}

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
    printf("Warning! Z pT weight < 0: weight = %.3e, pt = %.2f\n", weight, pt);
  }
  return weight;
}

//Check if lepton matches to a trigger
int NanoAODConversion::GetTriggerMatch(UInt_t index, bool isMuon) {
  float pt, eta, phi, min_pt_1, min_pt_2;
  int bit_1, bit_2, id; //trigger bits to use and pdgID
  if(isMuon) {
    pt  = muonPt[index];
    eta = muonEta[index];
    phi = muonPhi[index];
    bit_1 = 1; //IsoMu24/IsoMu27
    min_pt_1 = (fYear == ParticleCorrections::k2017) ? 28. : 25.;
    bit_2 = 10; //Mu50
    min_pt_2 = 50.;
    id = 13;
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
  for(unsigned trig_i = 0; trig_i < nTrigObjs; ++trig_i) {
    if(trigObjID[trig_i] != id) continue;
    //check if passes correct bit and the considered pT threshold for that trigger
    bool passBit1 = ((trigObjFilterBits[trig_i] & (1<<bit_1)) != 0) && (!fDoTriggerMatchPt || pt > min_pt_1);
    bool passBit2 = ((trigObjFilterBits[trig_i] & (1<<bit_2)) != 0) && (!fDoTriggerMatchPt || pt > min_pt_2);
    if(fVerbose > 2) std::cout << " TrigObj " << trig_i << " has bits " << trigObjFilterBits[trig_i] << std::endl;

    if(!passBit1 && !passBit2) continue;
    if(fVerbose > 2) std::cout << " TrigObj " << trig_i << " passed bits check, has pt, eta, phi "
			       << trigObjPt[trig_i] <<  ", " 
			       << trigObjEta[trig_i] <<  ", " 
			       << trigObjPhi[trig_i] << std::endl;
    if(fabs(pt - trigObjPt[trig_i]) < pt*deltapt_match) {
      float deltaeta = fabs(eta - trigObjEta[trig_i]);
      float deltaphi = fabs(phi - trigObjPhi[trig_i]);
      if(deltaphi > M_PI) deltaphi = fabs(2.*M_PI - deltaphi);
      if(fVerbose > 2) std::cout << "  TrigObj " << trig_i << " passed pt check\n";
      if(sqrt(deltaeta*deltaeta + deltaphi*deltaphi) < deltar_match) {
      if(fVerbose > 2) std::cout << "   TrigObj " << trig_i << " passed delta r xcheck\n";
	return (passBit1 + 2*passBit2);
      }
    }
  }

  return 0;//no matching trigger object found
}

Bool_t NanoAODConversion::Process(Long64_t entry)
{
  fentry = entry;
  if(fVerbose > 1) std::cout << "\n***Event " << entry << std::endl;
  // The Process() function is called for each entry in the tree (or possibly
  // keyed object in the case of PROOF) to be processed. The entry argument
  // specifies which entry in the currently loaded tree is to be processed.
  // When processing keyed objects with PROOF, the object is already loaded
  // and is available via the fObject pointer.
  //
  // This function should contain the \"body\" of the analysis. It can contain
  // simple or elaborate selection criteria, run algorithms on the data
  // of the event and typically fill histograms.
  //
  // The processing can be stopped by calling Abort().
  //
  // Use fStatus to set the return value of TTree::Process().
  //
  // The return value is currently not used.

  fChain->GetEntry(entry);
  if(entry%50000 == 0) printf("Processing event: %12lld (%5.1f%%)\n", entry, entry*100./fChain->GetEntriesFast());
  if(fVerbose > 2) std::cout << "Event info: nMuon = " << nMuon << " nElectron = " << nElectron
			     << " nTau = " << nTau << " nJet = " << nJet << " nTrigObj = " << nTrigObjs
			     << " nPhoton = " << nPhoton
			     << std::endl;
  if(kMaxParticles < nMuon || kMaxParticles < nElectron || kMaxParticles < nJets ||
     kMaxParticles < nTau  || kMaxParticles < nPhotons  || kMaxTriggers < nTrigObjs)
    std::cout << "WARNING! More particles than allowed in event " << entry
	      << ":\n"  << "Event info: nMuon = " << nMuon << " nElectron = " << nElectron
	      << " nTau = " << nTau << " nJet = " << nJet << " nTrigObj = " << nTrigObjs
	      << " nPhoton = " << nPhoton
	      << std::endl;

  //replace pileup weight if requested
  if(fIsData == 0 && fReplacePUWeights > 0) puWeight = fPUWeight.GetWeight(nPU, fYear+2016); //uses absolute year number

  CountObjects();

  //selections (all hopefully exclusive)
  bool muontriggered = HLT_Mu50 || ((fYear == ParticleCorrections::k2016 && HLT_IsoMu24) || 
				    (fYear == ParticleCorrections::k2017 && HLT_IsoMu27) || 
				    (fYear == ParticleCorrections::k2018 && HLT_IsoMu24));

  bool electrontriggered = ((fYear == ParticleCorrections::k2016 && HLT_Ele27_WPTight_GsF) ||
			  (fYear == ParticleCorrections::k2017 && HLT_Ele32_WPTight_GsF_L1DoubleEG) || //FIXME: add L1 Ele35 test as well
			  (fYear == ParticleCorrections::k2018 && HLT_Ele32_WPTight_GsF));
  
  bool triggered = muontriggered || electrontriggered;

  bool mutau = fNTaus[kMuTau] == 1 && fNMuons[kMuTau] == 1 && muontriggered /* && SelectionID(kMuTau)*/;
  bool etau  = fNTaus[kETau]  == 1 && fNElectrons[kETau]  == 1 && electrontriggered /* && SelectionID(kETau)*/;
  //if a good muon, electron, and tau, veto from tau categories (use actual selection IDs)
  if(mutau && SelectionID(kMuTau) && etau && SelectionID(kETau)) {mutau = false; etau = false;}
  bool emu   = fNMuons[kEMu]   == 1 && fNElectrons[kEMu]   == 1 && triggered /* && SelectionID(kEMu)*/;
  bool mumu  = fNMuons[kMuMu]  >= 2 && fNElectrons[kMuMu]  == 0 && muontriggered /* && SelectionID(kMuMu)*/;
  bool ee    = fNMuons[kEE]    == 0 && fNElectrons[kEE]    >= 2 && electrontriggered /* && SelectionID(kEE)*/;
  bool skipSS = false; //to track if no selection is in error or expected
  if(fSkipMuMuEESS && (ee || mumu)) { //require opposite sign if skipping same sign for ee/mumu
    mumu = mumu && muonCharge[fMuonIndices[kMuMu][0]]*muonCharge[fMuonIndices[kMuMu][1]] < 0;
    ee   = ee   && electronCharge[fElectronIndices[kEE][0]]*electronCharge[fElectronIndices[kEE][1]] < 0;
    skipSS = !(ee || mumu); //failed due to SS rejection
  }
  if(ee + mumu > 0 && ((emu && SelectionID(kEMu)) || (etau && SelectionID(kETau)) || (mutau && SelectionID(kMuTau)))) {
    ee = false; mumu = false;} //if passes a signal region, don't use in ee/mumu categories (shouldn't happen)
  //must pass at least one selection
  if(!mumu && !emu && !etau && !mutau && !ee) {
    if(!skipSS) {
      ++fNFailed;
      if(fVerbose > 0) std::cout << "Warning! Event " << entry << " passes no selection! Skim flavors are: " 
				 << leptonOneSkimFlavor << " and " << leptonTwoSkimFlavor << std::endl;
    } else {
      ++fNSkipped;
    }
    return kTRUE;
  }

  //Reject overlap of etau/mutau with emu
  if(emu && SelectionID(kEMu) && ((mutau && SelectionID(kMuTau)) || (etau && SelectionID(kETau)))) {mutau = false; etau = false;}
  
  //remove data overlap
  ee    &= fIsData != 2;
  etau  &= fIsData != 2;
  mumu  &= fIsData != 1;
  mutau &= fIsData != 1;

  //increment selection counts (for tight selection)
  if(emu   && SelectionID(kEMu))   ++fNEMu;
  if(etau  && SelectionID(kETau))  ++fNETau;
  if(mutau && SelectionID(kMuTau)) ++fNMuTau;
  if(mumu  && SelectionID(kMuMu))  ++fNMuMu;
  if(ee    && SelectionID(kEE))    ++fNEE;
  if((int) (emu&&SelectionID(kEMu)+etau&&SelectionID(kETau)
     +mutau&&SelectionID(kMuTau)+mumu&&SelectionID(kMuMu)
     +ee&&SelectionID(kEE)) > 1 && fVerbose > 0) std::cout << "Warning! Event " << entry << " passes multiple selections!\n"
							  << "mutau = " << mutau << " etau = " << etau << " emu = " << emu
							  << " mumu = " << mumu << " ee = " << ee << std::endl;
  
  //loop through selections, in case an event passes multiple (shouldn't happen)
  for(int selection = kMuTau; selection < kSelections; ++selection) {
    if(fSkipMuMuEE > 0 && (selection == kMuMu || selection == kEE)) { //check if skipping same flavor sets
      ++fNSkipped;
      continue;
    }
    //check if it passes the selection
    if(!((selection == kMuTau && mutau) || (selection == kETau && etau) ||
	 (selection == kEMu && emu) || (selection == kMuMu && mumu) || (selection == kEE && ee)))
      continue;
    //Check if passes tight ID requirements, if not mark for ABCD QCD scale factor calculation
    looseQCDSelection = !SelectionID(selection);
    if(looseQCDSelection) ++fQCDCounts[selection];
    InitializeTreeVariables(selection);
    if((selection == kMuMu || selection == kEE) && ((int) nElectrons != nElectronsSkim || (int) nMuons != nMuonsSkim))
      std::cout << fentry << ": Light lepton counts differ between skim and conversion! N(electrons) = " << nElectronsSkim
		<< " --> " << nElectrons << ", N(muons) = " << nMuonsSkim << " --> " << nMuons << std::endl;
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
  }
  return kTRUE;
}

void NanoAODConversion::SlaveTerminate()
{
  // The SlaveTerminate() function is called after all entries or objects
  // have been processed. When running with PROOF SlaveTerminate() is called
  // on each slave server.

}

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
  printf("Found:      %10i MuTau, %10i ETau, %10i EMu, %10i MuMu, %10i EE, %i failed trigger matching, %i failed, and %i skipped\n",
	 fNMuTau, fNETau, fNEMu, fNMuMu, fNEE, fNFailedTrig, fNFailed, fNSkipped);

  printf("QCD counts: ");
  for(int i = kMuTau; i < kSelections; ++i) printf("%10i %s, ", fQCDCounts[i], fSelecNames[i].Data());
  printf("\n");
}
