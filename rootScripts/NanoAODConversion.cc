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


#include "NanoAODConversion.hh"
#include <TStyle.h>

void NanoAODConversion::Begin(TTree * /*tree*/)
{
  // The Begin() function is called at the start of the query.
  // When running with PROOF Begin() is only called on the client.
  // The tree argument is deprecated (on PROOF 0 is passed).

  printf("NanoAODConversion::Begin\n");
  timer->Start();
  particleCorrections = new ParticleCorrections(fMuonIso);
  fChain = 0;
  
}

void NanoAODConversion::SlaveBegin(TTree * /*tree*/)
{
  // The SlaveBegin() function is called after the Begin() function.
  // When running with PROOF SlaveBegin() is called on each slave server.
  // The tree argument is deprecated (on PROOF 0 is passed).

  printf("NanoAODConversion::SlaveBegin\n");

}

void NanoAODConversion::InitializeInBranchStructure(TTree* tree) {
  tree->SetBranchAddress("genWeight"              , &genWeight                      ) ;
  tree->SetBranchAddress("puWeight"               , &puWeight                       ) ;
  tree->SetBranchAddress("nMuon"                  , &nMuons                         ) ;
  tree->SetBranchAddress("Muon_pt"                , &muonPt                         ) ;
  tree->SetBranchAddress("Muon_eta"               , &muonEta                        ) ;
  tree->SetBranchAddress("Muon_phi"               , &muonPhi                        ) ;
  tree->SetBranchAddress("Muon_mass"              , &muonMass                       ) ;
  tree->SetBranchAddress("Muon_pfRelIso03_all"    , &muonRelIso                     ) ;
  tree->SetBranchAddress("nElectron"              , &nElectrons                     ) ;
  tree->SetBranchAddress("Electron_pt"            , &electronPt                     ) ;
  tree->SetBranchAddress("Electron_eta"           , &electronEta                    ) ;
  tree->SetBranchAddress("Electron_phi"           , &electronPhi                    ) ;
  tree->SetBranchAddress("Electron_mass"          , &electronMass                   ) ;
  tree->SetBranchAddress("Electron_deltaEtaSC"    , &electronDeltaEtaSC             ) ;
  tree->SetBranchAddress("Electron_mvaFall17V2Iso_WP80", &electronMVAFall17         ) ;
  tree->SetBranchAddress("nJet"                   , &nJet                           ) ;
  tree->SetBranchAddress("Jet_pt"                 , &jetPt                          ) ;
  tree->SetBranchAddress("Jet_eta"                , &jetEta                         ) ;
  tree->SetBranchAddress("Jet_phi"                , &jetPhi                         ) ;
  tree->SetBranchAddress("Jet_mass"               , &jetMass                        ) ;
  tree->SetBranchAddress("Jet_jetId"              , &jetID                          ) ;
  tree->SetBranchAddress("Jet_puId"               , &jetPUID                        ) ;
  tree->SetBranchAddress("Jet_btagDeepB"          , &jetBTagDeepB                   ) ;
  tree->SetBranchAddress("Jet_btagCMVA"           , &jetBTagCMVA                    ) ;
  tree->SetBranchAddress("nPhoton"                , &nPhotons                       ) ;
  tree->SetBranchAddress("Photon_pt"              , &photonPt                       ) ;
  tree->SetBranchAddress("Photon_eta"             , &photonEta                      ) ;
  tree->SetBranchAddress("Photon_phi"             , &photonPhi                      ) ;
  tree->SetBranchAddress("Photon_mass"            , &photonMass                     ) ;
  tree->SetBranchAddress("HLT_IsoMu24"            , &HLT_IsoMu24                    ) ;
  tree->SetBranchAddress("HLT_IsoMu27"            , &HLT_IsoMu27                    ) ;
  tree->SetBranchAddress("HLT_Mu50"               , &HLT_Mu50                       ) ;
  tree->SetBranchAddress("HLT_Ele27_WPTight_Gsf"  , &HLT_Ele27_WPTight_GsF          ) ;
  tree->SetBranchAddress("HLT_Ele32_WPTight_Gsf"  , &HLT_Ele32_WPTight_GsF          ) ;
  tree->SetBranchAddress("HLT_Ele32_WPTight_Gsf_L1DoubleEG", &HLT_Ele32_WPTight_GsF_L1DoubleEG) ;
  tree->SetBranchAddress("PuppiMET_pt"            , &puppMET                        ) ;
  tree->SetBranchAddress("PuppiMET_phi"           , &puppMETphi                     ) ;
  tree->SetBranchAddress("PV_npvsGood"            , &nGoodPV                        ) ;

}

void NanoAODConversion::InitializeOutBranchStructure(TTree* tree) {
  if(!leptonOneP4)   leptonOneP4   = new TLorentzVector();
  if(!leptonTwoP4)   leptonTwoP4   = new TLorentzVector();
  if(!leptonOneSVP4) leptonOneSVP4 = new TLorentzVector();
  if(!leptonTwoSVP4) leptonTwoSVP4 = new TLorentzVector();
  if(!jetP4)         jetP4         = new TLorentzVector();
  if(!photonP4)      photonP4      = new TLorentzVector();
  
  tree->Branch("runNumber"           , &runNumber            );
  tree->Branch("evtNumber"           , &eventNumber          );
  tree->Branch("lumiSection"         , &lumiSection          );
  tree->Branch("nPV"                 , &nPV                  );
  tree->Branch("nPU"                 , &nPU                  );
  tree->Branch("nPartons"            , &nPartons             );
  tree->Branch("mcEra"               , &mcEra                );
  tree->Branch("triggerLeptonStatus" , &triggerLeptonStatus  );
  tree->Branch("eventWeight"         , &eventWeight          );
  tree->Branch("genWeight"           , &genWeight            );
  tree->Branch("puWeight"            , &puWeight             );
  tree->Branch("lepOneWeight"        , &lepOneWeight         );
  tree->Branch("lepTwoWeight"        , &lepTwoWeight         );
  tree->Branch("topPtWeight"         , &topPtWeight          );
  tree->Branch("zPtWeight"           , &zPtWeight            );
  tree->Branch("genTauFlavorWeight"  , &genTauFlavorWeight   );
  tree->Branch("tauDecayMode"        , &tauDecayMode         );
  tree->Branch("tauMVA"              , &tauMVA               );
  tree->Branch("tauGenFlavor"        , &tauGenFlavor         );
  tree->Branch("tauGenFlavorHad"     , &tauGenFlavorHad      );
  tree->Branch("tauVetoedJetPt"      , &tauVetoedJetPt       );
  tree->Branch("tauVetoedJetPtUnc"   , &tauVetoedJetPtUnc    );
  tree->Branch("leptonOneP4"         , &leptonOneP4          );
  tree->Branch("leptonTwoP4"         , &leptonTwoP4          );
  tree->Branch("leptonOneFlavor"     , &leptonOneFlavor      );
  tree->Branch("leptonTwoFlavor"     , &leptonTwoFlavor      );
  tree->Branch("leptonOneD0"         , &leptonOneD0          );
  tree->Branch("leptonTwoD0"         , &leptonTwoD0          );
  tree->Branch("leptonOneIso"        , &leptonOneIso         );
  tree->Branch("leptonTwoIso"        , &leptonTwoIso         );
  tree->Branch("genLeptonOneP4"      , &genLeptonOneP4       );
  tree->Branch("genLeptonTwoP4"      , &genLeptonTwoP4       );
  tree->Branch("photonP4"            , &photonP4             );
  tree->Branch("jetP4"               , &jetP4                );
  tree->Branch("tauP4"               , &tauP4                );
  tree->Branch("nMuons"              , &nMuons               );
  tree->Branch("nElectrons"          , &nElectrons           );
  tree->Branch("nTaus"               , &nTaus                );
  tree->Branch("nPhotons"            , &nPhotons             );
  tree->Branch("nJets"               , &nJets                );
  tree->Branch("nJets25"             , &nJets25              );
  tree->Branch("nJets20"             , &nJets20              );
  tree->Branch("nFwdJets"            , &nFwdJets             );
  tree->Branch("nBJetsDeepM"         , &nBJetsDeepM          );
  tree->Branch("nBJets"              , &nBJets               );
  tree->Branch("nBJetsM"             , &nBJetsM              );
  tree->Branch("nBJetsL"             , &nBJetsL              );
  tree->Branch("nBJets25"            , &nBJets25             );
  tree->Branch("nBJets25M"           , &nBJets25M            );
  tree->Branch("nBJets25L"           , &nBJets25L            );
  tree->Branch("nBJets20"            , &nBJets20             );
  tree->Branch("nBJets20M"           , &nBJets20M            );
  tree->Branch("nBJets20L"           , &nBJets20L            );
  tree->Branch("nGenTausHad"         , &nGenTausHad          );
  tree->Branch("nGenTausLep"         , &nGenTausLep          );
  tree->Branch("nGenElectrons"       , &nGenElectrons        );
  tree->Branch("nGenMuons"           , &nGenMuons            );
  tree->Branch("nGenPromptTaus"      , &nGenHardTaus         );
  tree->Branch("nGenPromptElectrons" , &nGenHardElectrons    );
  tree->Branch("nGenPromptMuons"     , &nGenHardMuons        );
  tree->Branch("htSum"               , &htSum                );
  tree->Branch("ht"                  , &ht                   );
  tree->Branch("htPhi"               , &htPhi                );
  // tree->Branch("pfMET"               , &pfMET                );
  // tree->Branch("pfMETphi"            , &pfMETphi             );
  // tree->Branch("pfMETCov00"          , &pfMETCov00           );
  // tree->Branch("pfMETCov01"          , &pfMETCov01           );
  // tree->Branch("pfMETCov11"          , &pfMETCov11           );
  tree->Branch("pfMETC"              , &pfMETC               );
  tree->Branch("pfMETCphi"           , &pfMETCphi            );
  tree->Branch("pfMETCCov00"         , &pfMETCCov00          );
  tree->Branch("pfMETCCov01"         , &pfMETCCov01          );
  tree->Branch("pfMETCCov11"         , &pfMETCCov11          );
  tree->Branch("puppMET"             , &puppMET              );
  tree->Branch("puppMETphi"          , &puppMETphi           );
  tree->Branch("puppMETCov00"        , &puppMETCov00         );
  tree->Branch("puppMETCov01"        , &puppMETCov01         );
  tree->Branch("puppMETCov11"        , &puppMETCov11         );
  tree->Branch("puppMETC"            , &puppMETC             );
  tree->Branch("puppMETCphi"         , &puppMETCphi          );
  // tree->Branch("puppMETCCov00"       , &puppMETCCov00        );
  // tree->Branch("puppMETCCov01"       , &puppMETCCov01        );
  // tree->Branch("puppMETCCov11"       , &puppMETCCov11        );
  // tree->Branch("alpacaMET"           , &alpacaMET            );
  // tree->Branch("alpacaMETphi"        , &alpacaMETphi         );
  // tree->Branch("pcpMET"              , &pcpMET               );
  // tree->Branch("pcpMETphi"           , &pcpMETphi            );
  tree->Branch("trkMET"              , &trkMET               );
  tree->Branch("trkMETphi"           , &trkMETphi            );
  tree->Branch("met"                 , &met                  );
  tree->Branch("metPhi"              , &metPhi               );
  tree->Branch("metCorr"             , &metCorr              );
  tree->Branch("metCorrPhi"          , &metCorrPhi           );
  tree->Branch("covMet00"            , &covMet00             );
  tree->Branch("covMet01"            , &covMet01             );
  tree->Branch("covMet11"            , &covMet11             );
  tree->Branch("massSVFit"           , &massSVFit            );
  tree->Branch("massErrSVFit"        , &massErrSVFit         );
  tree->Branch("svFitStatus"         , &svFitStatus          );
  tree->Branch("leptonOneSVP4"       , &leptonOneSVP4        );
  tree->Branch("leptonTwoSVP4"       , &leptonTwoSVP4        );

}

void NanoAODConversion::InitializeTreeVariables(Int_t selection) {
  genWeight = (genWeight == 0.) ? 0. : genWeight/abs(genWeight);

  //use PUPPI MET by default
  met = puppMET;
  metPhi = puppMETphi;
  //set corrected PUPPI MET to be nominal
  puppMETC    = puppMET   ;
  puppMETCphi = puppMETphi;
  //store muon and electron trigger (1 = electron, 2 = muon, 3 = both)
  //muon trigger is Mu50 for all years and IsoMu24 for 2016, 2018 and IsoMu27 for 2017
  bool lowMuonTriggered  = ((fYear == ParticleCorrections::k2016 && HLT_IsoMu24) || 
			    (fYear == ParticleCorrections::k2017 && HLT_IsoMu27) || 
			    (fYear == ParticleCorrections::k2018 && HLT_IsoMu24));
  bool highMuonTriggered = HLT_Mu50;
  bool electronTriggered = ((fYear == ParticleCorrections::k2016 && HLT_Ele27_WPTight_GsF) ||
			    (fYear == ParticleCorrections::k2017 && HLT_Ele32_WPTight_GsF_L1DoubleEG) || 
			    (fYear == ParticleCorrections::k2018 && HLT_Ele32_WPTight_GsF));
  triggerLeptonStatus = electronTriggered + 2*(lowMuonTriggered || highMuonTriggered);

  int trigger = 0; //muon weights use the trigger
  if(selection == kMuTau || selection == kEMu || selection == kMuMu)
    trigger = ((HLT_IsoMu24 && fYear != ParticleCorrections::k2017) || //default to low trigger if it passed it
	       (HLT_IsoMu27 && fYear == ParticleCorrections::k2017)) ? ParticleCorrections::kLowTrigger : ParticleCorrections::kHighTrigger;

	       //store lepton information
  if(selection == kMuTau || selection == kMuMu) {
    leptonOneP4->SetPtEtaPhiM(muonPt[0], muonEta[0], muonPhi[0], muonMass[0]);
    leptonOneFlavor = 13;
    if(!fIsData) lepOneWeight = particleCorrections->MuonWeight(muonPt[0], muonEta[0], trigger, fYear);
  } else if(selection == kETau || selection == kEMu || selection == kEE) {
    leptonOneP4->SetPtEtaPhiM(electronPt[0], electronEta[0], electronPhi[0], electronMass[0]);
    leptonOneFlavor = 11;
    if(!fIsData) lepOneWeight = particleCorrections->ElectronWeight(electronPt[0], electronEta[0], fYear);
  }    
  if(selection == kMuTau || selection == kETau) {
    leptonTwoP4->SetPtEtaPhiM(tauPt[0], tauEta[0], tauPhi[0], tauMass[0]);
    leptonTwoFlavor = 15;
    lepTwoWeight = 1.; //FIXME: Tau weights missing
  } else if(selection == kEMu) {
    leptonTwoP4->SetPtEtaPhiM(muonPt[0], muonEta[0], muonPhi[0], muonMass[0]);
    leptonTwoFlavor = 13;
    if(!fIsData) lepTwoWeight = particleCorrections->MuonWeight(muonPt[0], muonEta[0], trigger, fYear);
  } else if(selection == kMuMu) {
    leptonTwoP4->SetPtEtaPhiM(muonPt[1], muonEta[1], muonPhi[1], muonMass[1]);
    leptonTwoFlavor = 13;
    if(!fIsData) lepTwoWeight = particleCorrections->MuonWeight(muonPt[1], muonEta[1], trigger, fYear);
  } else if(selection == kEE) {
    leptonTwoP4->SetPtEtaPhiM(electronPt[1], electronEta[1], electronPhi[1], electronMass[1]);
    leptonTwoFlavor = 11;
    if(!fIsData) lepTwoWeight = particleCorrections->ElectronWeight(electronPt[1], electronEta[1], fYear);
  }
  if(!fIsData)
    eventWeight = lepOneWeight * lepTwoWeight * puWeight;
  else {
    eventWeight = 1.;
    genWeight = 1.;
    puWeight = 1.;
    lepOneWeight = 1.;
    lepTwoWeight = 1.;
  }
  
  //all are opposite signed, so just assign lepton two as opposite for now
  leptonTwoFlavor *= -1;
  if(nPhotons > 0)
    photonP4->SetPtEtaPhiM(photonPt[0], photonEta[0], photonPhi[0], photonMass[0]);
  else
    photonP4->SetPtEtaPhiM(0., 0., 0., 0.);
    
  //Jet loop
  unsigned njets = nJet;
  //reset counters
  nJets = 0; nJets25 = 0; nJets20 = 0; //BLT nominally uses jet pt > 30, so save 20-25 and 25-30 separately
  nBJets = 0; nBJetsL = 0; nBJetsM = 0;
  nBJets25 = 0; nBJets25L = 0; nBJets25M = 0;
  nBJets20 = 0; nBJets20L = 0; nBJets20M = 0;
  nBJetsDeepM = 0;
  
  jetP4->SetPtEtaPhiM(0., 0., 0., 0.);
  float jetptmax = -1.;
  int jetIDFlag = 7; //2016 values
  int jetPUIDFlag = 6;
  if(fYear == ParticleCorrections::k2017) {
    jetIDFlag = 4; jetPUIDFlag = 6;
  } else if(fYear == ParticleCorrections::k2018) {
    jetIDFlag = 4; jetPUIDFlag = 6;
  }

  TLorentzVector* jetLoop = new TLorentzVector(); //for checking delta R
  for(unsigned index = 0; index < njets; ++index) {
    float jetpt = jetPt[index];
    if(jetpt < 20.) continue; //too low of jet pt
    if((jetpt < 50. && jetPUID[index] < jetPUIDFlag) || jetID[index] < jetIDFlag) //bad jet
      continue;
    //check that the jet doesn't overlap the leptons
    jetLoop->SetPtEtaPhiM(jetPt[index], jetEta[index], jetPhi[index], jetMass[index]);
    if(jetLoop->DeltaR(*leptonOneP4) < 0.3 || jetLoop->DeltaR(*leptonTwoP4) < 0.3)
      continue;
    //store the hardest jet
    if(jetptmax < jetpt) {
      jetptmax = jetpt;
      jetP4->SetPtEtaPhiM(jetPt[index], jetEta[index], jetPhi[index], jetMass[index]);
    }
    if(jetpt > 30.) ++nJets;
    else if(jetpt > 25) ++nJets25;
    else if(jetpt > 20) ++nJets20;

    //Deep neural net based ID
    if(jetpt > 25. && jetBTagDeepB[index] > 0.4184) //only store medium ID for now
      ++nBJetsDeepM;
    
    //MVA based ID
    if(jetBTagCMVA[index] > -0.5884) {
      if(jetpt > 30.) ++nBJetsL;
      else if(jetpt > 25.) ++nBJets25L;
      else if(jetpt > 20.) ++nBJets20L;
      if(jetBTagCMVA[index] > 0.4432) {
	if(jetpt > 30.) ++nBJetsM;
	else if(jetpt > 25.) ++nBJets25M;
	else if(jetpt > 20.) ++nBJets20M;
	if(jetBTagCMVA[index] > 0.8484) {
	  if(jetpt > 30.) ++nBJets;
	  else if(jetpt > 25.) ++nBJets25;
	  else if(jetpt > 20.) ++nBJets20;
	}
      }
    }
  }
  delete jetLoop;
  //save npv as ngoodpv for now
  nPV = nGoodPV;
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


Bool_t NanoAODConversion::Process(Long64_t entry)
{
  fentry = entry;
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

  //selections (all exclusive)
  bool mutau = nTaus == 1  && nMuons == 1 && nElectrons == 0;
  bool etau  = nTaus == 1  && nMuons == 0 && nElectrons == 1;
  bool emu   = /*nTaus == 0  &&*/ nMuons == 1 && nElectrons == 1;
  bool mumu  = nMuons == 2 && nElectrons == 0; //no tau requirement
  bool ee    = nMuons == 0 && nElectrons == 2; //no tau requirement
  if(!mumu && !emu && !etau && !mutau && !ee) {
    std::cout << "Warning! Event " << entry << " passes no selection!" << std::endl
	      << "nMuons = " << nMuons << " nElectrons = " << nElectrons
	      << " nTaus = " << nTaus << std::endl;
    return kTRUE;
  }
  int selection = mutau*kMuTau + etau*kETau + emu*kEMu + mumu*kMuMu + ee*kEE;
  InitializeTreeVariables(selection);
  // if(entry%50000 == 0)
  //   std::cout << "Event selections: mutau = " << mutau
  // 	      << " etau = " << etau << " emu = " << emu
  // 	      << " mumu = " << mumu << " ee = " << ee
  // 	      << std::endl;
    
  fDirs[selection]->cd();  
  fOutTrees[selection]->Fill();
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


}
