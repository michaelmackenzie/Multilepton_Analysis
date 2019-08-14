#define MultileptonNTupleMaker_cxx
// The class definition in MultileptonNTupleMaker.h has been generated automatically
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
// root> T->Process("MultileptonNTupleMaker.C")
// root> T->Process("MultileptonNTupleMaker.C","some options")
// root> T->Process("MultileptonNTupleMaker.C+")
//


#include "MultileptonNTupleMaker.h"
#include <TH2.h>
#include <TStyle.h>

void MultileptonNTupleMaker::Begin(TTree * /*tree*/)
{
  // The Begin() function is called at the start of the query.
  // When running with PROOF Begin() is only called on the client.
  // The tree argument is deprecated (on PROOF 0 is passed).
  TString option = GetOption();
  printf("MultileptonNTupleMaker::Begin\n");
  timer->Start();
  fChain = 0;
}

void MultileptonNTupleMaker::SlaveBegin(TTree * /*tree*/)
{
  // The SlaveBegin() function is called after the Begin() function.
  // When running with PROOF SlaveBegin() is called on each slave server.
  // The tree argument is deprecated (on PROOF 0 is passed).
  // Readers to access the data (delete the ones you do not need).

  printf("MultileptonNTupleMaker::SlaveBegin\n");
  TString option = GetOption();

}

//Taken from bin/ParticleSelector.cc
bool MultileptonNTupleMaker::BTagModifier(int i) 
{
    bool  isBTagged = false;
    float jetPt     = (i == 1) ? jet1P4->Pt() : jet2P4->Pt(); 
    float bTag      = (i == 1) ? jet1Tag : jet2Tag;      
    int   jetFlavor = (i == 1) ? jet1Flavor : jet2Flavor;   
    if(i == 1 && abs(jet1P4->Eta()) > 2.4) return false; //if it's a forward jet, can't tag
    else if(i == 2 && abs(jet2P4->Eta()) > 2.4) return false; //if it's a forward jet, can't tag
    TString tagName = "MVAT";

    // Get b tag efficiency and mistag scale factor
    float btagSF = 1.;
    float mistagSF = 1.;
    if (tagName == "CSVT") {
        // These SF are provided by the b tag POG 
        if (bTag > 0.935) isBTagged = true;
        btagSF   = 0.857294 + 3.75846e-05*jetPt; 
        mistagSF = 0.688619 + 260.84/(jetPt*jetPt); 
    } else if (tagName == "MVAT") {
        // These SF are provided by the b tag POG 
        if (bTag > 0.9432) isBTagged = true;
        btagSF   = 0.52032*((1.+(0.370141*jetPt))/(1.+(0.196566*jetPt))); 
        mistagSF = 0.985864+122.8/(jetPt*jetPt)+0.000416939*jetPt;
    } else if (tagName == "MVAM") {
        // These SF are provided by the b tag POG 
        if (bTag > 0.4432) isBTagged = true;
        btagSF   = 0.885562 - 5.67668e-05*jetPt; 
        mistagSF = 1.07108+-0.0015866*jetPt+3.06322e-06*jetPt*jetPt+-1.7438e-09*jetPt*jetPt*jetPt;
    }


    // Upgrade or downgrade jet
    float rNumber = _rng->Uniform(1.);
    if (abs(jetFlavor) == 5 || abs(jetFlavor) == 4) {
        float mcEff = 1.;
        if (abs(jetFlavor) == 4) 
            mcEff = 0.3;//_ctagEff->Eval(jetPt);
        else if (abs(jetFlavor) == 5) 
            mcEff = 0.6;//_btagEff->Eval(jetPt);

        if(btagSF >= 1){  // use this if SF>1
            if (!isBTagged) { //upgrade to tagged
                float mistagRate = (1. - btagSF) / (1. - 1./mcEff);
                if (rNumber < mistagRate) isBTagged = true;
            }
        } else if (btagSF < 1) { //downgrade tagged to untagged
            if(isBTagged && rNumber > btagSF) isBTagged = false;
        }

    } else {
        float mcEff = 0.003; //_misTagEff->Eval(jetPt);
        if(mistagSF > 1){  // use this if SF>1
            if (!isBTagged) { //upgrade to tagged
                float mistagPercent = (1. - mistagSF) / (1. - (1./mcEff));
                if (rNumber < mistagPercent) isBTagged = true;
            }
        } else if (mistagSF < 1) { //downgrade tagged to untagged
            if (isBTagged && rNumber > mistagSF) isBTagged = false;
        }
    }

    return isBTagged;
}

void MultileptonNTupleMaker::BookTrees() {
  BookEventTrees();
}

void MultileptonNTupleMaker::BookEventTrees() {
  for(int i = 0; i < fn; i++) {
    if(fEventSets[i] != 0) {
      char* dirname        = new char[20];
      sprintf(dirname,"event_%i",i);
      fDirectories[0*fn + i] = fTopDir->mkdir(dirname);
      fDirectories[0*fn + i]->cd();
      fEventNT[i] = new EventNT_t;
      fEventNT[i]->index = i;
      fEventTree[i] = new TTree(Form("event_tree_%i",i),Form("An Event Tree For Set %i",i));
      
      fEventTree[i]->Branch("lumiSection"  , &fEventNT[i]->lumiSection      ,"lumiSection/i"   );
      fEventTree[i]->Branch("triggerStatus", &fEventNT[i]->triggerStatus    ,"triggerStatus/O" );
      fEventTree[i]->Branch("eventWeight"  , &fEventNT[i]->eventWeight      ,"eventWeight/F"   );
      fEventTree[i]->Branch("genWeight"    , &fEventNT[i]->genWeight	    ,"genWeight/F"     );   
      fEventTree[i]->Branch("nPV"          , &fEventNT[i]->nPV	       	    ,"nPV/i"           );
      fEventTree[i]->Branch("nPU"          , &fEventNT[i]->nPU	       	    ,"nPU/i"           );
      fEventTree[i]->Branch("nPartons"     , &fEventNT[i]->nPartons	    ,"nPartons/i"      );   
      fEventTree[i]->Branch("met"          , &fEventNT[i]->met	       	    ,"met/F"           );
      fEventTree[i]->Branch("metPhi"       , &fEventNT[i]->metPhi	    ,"metPhi/F"        );   
      fEventTree[i]->Branch("nMuons"       , &fEventNT[i]->nMuons	    ,"nMuons/i"        );   
      fEventTree[i]->Branch("nElectrons"   , &fEventNT[i]->nElectrons       ,"nElectrons/i"    );
      fEventTree[i]->Branch("nJets"        , &fEventNT[i]->nJets	    ,"nJets/i"         );   
      fEventTree[i]->Branch("nFwdJets"     , &fEventNT[i]->nFwdJets	    ,"nFwdJets/i"      );   
      fEventTree[i]->Branch("nBJets"       , &fEventNT[i]->nBJets	    ,"nBJets/i"        );   
      fEventTree[i]->Branch("jetsM"        , &fEventNT[i]->jetsM	    ,"jetsM/D"         );   
      fEventTree[i]->Branch("jetsPt"       , &fEventNT[i]->jetsPt	    ,"jetsPt/D"        );   
      fEventTree[i]->Branch("jetsEta"      , &fEventNT[i]->jetsEta	    ,"jetsEta/D"       );   
      fEventTree[i]->Branch("jetsDelPhi"   , &fEventNT[i]->jetsDelPhi       ,"jetsDelPhi/D"    );
      fEventTree[i]->Branch("jetsDelEta"   , &fEventNT[i]->jetsDelEta       ,"jetsDelEta/D"    );
      fEventTree[i]->Branch("jetsDelR"     , &fEventNT[i]->jetsDelR	    ,"jetsDelR/D"      );   
      fEventTree[i]->Branch("lepM"         , &fEventNT[i]->lepM	       	    ,"lepM/D"          );
      fEventTree[i]->Branch("lepPt "       , &fEventNT[i]->lepPt	    ,"lepPt/D"         );   
      fEventTree[i]->Branch("lepEta"       , &fEventNT[i]->lepEta	    ,"lepEta/D"        );   
      fEventTree[i]->Branch("lepPhi"       , &fEventNT[i]->lepPhi	    ,"lepPhi/D"        );   
      fEventTree[i]->Branch("lepDelPhi"    , &fEventNT[i]->lepDelPhi	    ,"lepDelPhi/D"     );   
      fEventTree[i]->Branch("lepDelEta"    , &fEventNT[i]->lepDelEta	    ,"lepDelEta/D"     );   
      fEventTree[i]->Branch("lepDelR"      , &fEventNT[i]->lepDelR	    ,"lepDelR/D"       );   
      fEventTree[i]->Branch("lepPtOverM"   , &fEventNT[i]->lepPtOverM       ,"lepPtOverM/D"    );
      fEventTree[i]->Branch("sysM"         , &fEventNT[i]->sysM	       	    ,"sysM/D"          );
      fEventTree[i]->Branch("sysPt"        , &fEventNT[i]->sysPt	    ,"sysPt/D"         );   
      fEventTree[i]->Branch("sysEta"       , &fEventNT[i]->sysEta	    ,"sysEta/D"        );   
      fEventTree[i]->Branch("jetsLepDelPhi", &fEventNT[i]->jetsLepDelPhi    ,"jetsLepDelPhi/D" );
      fEventTree[i]->Branch("jetsLepDelEta", &fEventNT[i]->jetsLepDelEta    ,"jetsLepDelEta/D" );
      fEventTree[i]->Branch("jetsLepDelR"  , &fEventNT[i]->jetsLepDelR      ,"jetsLepDelR/D"   );
      fEventTree[i]->Branch("jetLepM"      , &fEventNT[i]->jetLepM          ,"jetLepM/D"       );
      fEventTree[i]->Branch("jetLepPt"     , &fEventNT[i]->jetLepPt         ,"jetLepPt/D"      );
      fEventTree[i]->Branch("jetLepDelPhi" , &fEventNT[i]->jetLepDelPhi     ,"jetLepDelPhi/D"  );
      fEventTree[i]->Branch("jetLepDelEta" , &fEventNT[i]->jetLepDelEta     ,"jetLepDelEta/D"  );
      fEventTree[i]->Branch("jetLepDelR"   , &fEventNT[i]->jetLepDelR       ,"jetLepDelR/D"    );
      fEventTree[i]->Branch("bjetLepM"     , &fEventNT[i]->bjetLepM         ,"bjetLepM/D"      );
      fEventTree[i]->Branch("bjetLepPt"    , &fEventNT[i]->bjetLepPt        ,"bjetLepPt/D"     );
      fEventTree[i]->Branch("bjetLepDelPhi", &fEventNT[i]->bjetLepDelPhi    ,"bjetLepDelPhi/D" );
      fEventTree[i]->Branch("bjetLepDelEta", &fEventNT[i]->bjetLepDelEta    ,"bjetLepDelEta/D" );
      fEventTree[i]->Branch("bjetLepDelR " , &fEventNT[i]->bjetLepDelR      ,"bjetLepDelR/D"   );  
      fEventTree[i]->Branch("jetbLepDelPhi", &fEventNT[i]->jetbLepDelPhi    ,"jetbLepDelPhi/D" );
      fEventTree[i]->Branch("jetbLepDelEta", &fEventNT[i]->jetbLepDelEta    ,"jetbLepDelEta/D" );
      fEventTree[i]->Branch("jetbLepDelR"  , &fEventNT[i]->jetbLepDelR      ,"jetbLepDelR/D"   );

      fEventTree[i]->Branch("jet1px"       , &fEventNT[i]->jet1px           ,"jet1px/D"        );
      fEventTree[i]->Branch("jet1py"       , &fEventNT[i]->jet1py           ,"jet1py/D"        );
      fEventTree[i]->Branch("jet1pz"       , &fEventNT[i]->jet1pz           ,"jet1pz/D"        );
      fEventTree[i]->Branch("jet1pt"       , &fEventNT[i]->jet1pt           ,"jet1pt/D"        );
      fEventTree[i]->Branch("jet1p"        , &fEventNT[i]->jet1p            ,"jet1p/D"         );
      fEventTree[i]->Branch("jet1e"        , &fEventNT[i]->jet1e            ,"jet1e/D"         );
      fEventTree[i]->Branch("jet1m"        , &fEventNT[i]->jet1m            ,"jet1m/D"         );
      fEventTree[i]->Branch("jet1eta"      , &fEventNT[i]->jet1eta          ,"jet1eta/D"       );
      fEventTree[i]->Branch("jet1phi"      , &fEventNT[i]->jet1phi          ,"jet1phi/D"       );
      fEventTree[i]->Branch("jet1flavor"   , &fEventNT[i]->jet1flavor       ,"jet1flavor/I"    );
      fEventTree[i]->Branch("jet1d0"       , &fEventNT[i]->jet1d0           ,"jet1d0/F"        );
      fEventTree[i]->Branch("jet1tag"      , &fEventNT[i]->jet1tag          ,"jet1tag/F"       );
      fEventTree[i]->Branch("jet1puid"     , &fEventNT[i]->jet1puid         ,"jet1puid/F"      );
      fEventTree[i]->Branch("jet1tagged"   , &fEventNT[i]->jet1tagged       ,"jet1tagged/O"    );

      fEventTree[i]->Branch("jet2px"       , &fEventNT[i]->jet2px           ,"jet2px/D"        );
      fEventTree[i]->Branch("jet2py"       , &fEventNT[i]->jet2py           ,"jet2py/D"        );
      fEventTree[i]->Branch("jet2pz"       , &fEventNT[i]->jet2pz           ,"jet2pz/D"        );
      fEventTree[i]->Branch("jet2pt"       , &fEventNT[i]->jet2pt           ,"jet2pt/D"        );
      fEventTree[i]->Branch("jet2p"        , &fEventNT[i]->jet2p            ,"jet2p/D"         );
      fEventTree[i]->Branch("jet2e"        , &fEventNT[i]->jet2e            ,"jet2e/D"         );
      fEventTree[i]->Branch("jet2m"        , &fEventNT[i]->jet2m            ,"jet2m/D"         );
      fEventTree[i]->Branch("jet2eta"      , &fEventNT[i]->jet2eta          ,"jet2eta/D"       );
      fEventTree[i]->Branch("jet2phi"      , &fEventNT[i]->jet2phi          ,"jet2phi/D"       );
      fEventTree[i]->Branch("jet2flavor"   , &fEventNT[i]->jet2flavor       ,"jet2flavor/I"    );
      fEventTree[i]->Branch("jet2d0"       , &fEventNT[i]->jet2d0           ,"jet2d0/F"        );
      fEventTree[i]->Branch("jet2tag"      , &fEventNT[i]->jet2tag          ,"jet2tag/F"       );
      fEventTree[i]->Branch("jet2puid"     , &fEventNT[i]->jet2puid         ,"jet2puid/F"      );
      fEventTree[i]->Branch("jet2tagged"   , &fEventNT[i]->jet2tagged       ,"jet2tagged/O"    );

      fEventTree[i]->Branch("lep1px"       , &fEventNT[i]->lep1px           ,"lep1px/D"        );
      fEventTree[i]->Branch("lep1py"       , &fEventNT[i]->lep1py           ,"lep1py/D"        );
      fEventTree[i]->Branch("lep1pz"       , &fEventNT[i]->lep1pz           ,"lep1pz/D"        );
      fEventTree[i]->Branch("lep1pt"       , &fEventNT[i]->lep1pt           ,"lep1pt/D"        );
      fEventTree[i]->Branch("lep1p"        , &fEventNT[i]->lep1p            ,"lep1p/D"         );
      fEventTree[i]->Branch("lep1e"        , &fEventNT[i]->lep1e            ,"lep1e/D"         );
      fEventTree[i]->Branch("lep1m"        , &fEventNT[i]->lep1m            ,"lep1m/D"         );
      fEventTree[i]->Branch("lep1eta"      , &fEventNT[i]->lep1eta          ,"lep1eta/D"       );
      fEventTree[i]->Branch("lep1phi"      , &fEventNT[i]->lep1phi          ,"lep1phi/D"       );
      fEventTree[i]->Branch("lep1flavor"   , &fEventNT[i]->lep1flavor       ,"lep1flavor/I"    );
      fEventTree[i]->Branch("lep1iso"      , &fEventNT[i]->lep1iso          ,"lep1iso/F"       );
      fEventTree[i]->Branch("lep1reliso"   , &fEventNT[i]->lep1reliso       ,"lep1reliso/D"    ); 
      fEventTree[i]->Branch("lep1q"        , &fEventNT[i]->lep1q            ,"lep1q/I"         );
      fEventTree[i]->Branch("lep1trigger"  , &fEventNT[i]->lep1trigger      ,"lep1trigger/O"   );

      fEventTree[i]->Branch("lep2px"       , &fEventNT[i]->lep2px           ,"lep2px/D"        );
      fEventTree[i]->Branch("lep2py"       , &fEventNT[i]->lep2py           ,"lep2py/D"        );
      fEventTree[i]->Branch("lep2pz"       , &fEventNT[i]->lep2pz           ,"lep2pz/D"        );
      fEventTree[i]->Branch("lep2pt"       , &fEventNT[i]->lep2pt           ,"lep2pt/D"        );
      fEventTree[i]->Branch("lep2p"        , &fEventNT[i]->lep2p            ,"lep2p/D"         );
      fEventTree[i]->Branch("lep2e"        , &fEventNT[i]->lep2e            ,"lep2e/D"         );
      fEventTree[i]->Branch("lep2m"        , &fEventNT[i]->lep2m            ,"lep2m/D"         );
      fEventTree[i]->Branch("lep2eta"      , &fEventNT[i]->lep2eta          ,"lep2eta/D"       );
      fEventTree[i]->Branch("lep2phi"      , &fEventNT[i]->lep2phi          ,"lep2phi/D"       );
      fEventTree[i]->Branch("lep2flavor"   , &fEventNT[i]->lep2flavor       ,"lep2flavor/I"    );
      fEventTree[i]->Branch("lep2iso"      , &fEventNT[i]->lep2iso          ,"lep2iso/F"       );
      fEventTree[i]->Branch("lep2reliso"   , &fEventNT[i]->lep2reliso       ,"lep2reliso/D"    ); 
      fEventTree[i]->Branch("lep2q"        , &fEventNT[i]->lep2q            ,"lep2q/I"         );
      fEventTree[i]->Branch("lep2trigger"  , &fEventNT[i]->lep2trigger      ,"lep2trigger/O"   );
    }
  }
}



void MultileptonNTupleMaker::FillEventTree(EventNT_t* tree) {
  tree->lumiSection   = lumiSection  ;
  tree->triggerStatus = triggerStatus;
  tree->eventWeight   = eventWeight  ;
  tree->genWeight     = genWeight    ;
  tree->nPV           = nPV          ;
  tree->nPU           = nPU          ;
  tree->nPartons      = nPartons     ;
  tree->met           = met          ;
  tree->metPhi        = metPhi       ;
  tree->nMuons        = nMuons       ;
  tree->nElectrons    = nElectrons   ;
  tree->nJets         = nJets        ;
  tree->nFwdJets      = nFwdJets     ;
  tree->nBJets        = nBJets       ;

  TLorentzVector jetSys = (*jet1P4) + (*jet2P4);
  TLorentzVector lepSys = (*leptonOneP4) + (*leptonTwoP4);
  TLorentzVector sys    = jetSys + lepSys;
  
  double jbDelR   = jet1P4->DeltaR(*jet2P4);
  double jbDelPhi = abs(jet1P4->DeltaPhi(*jet2P4));
  double jbDelEta = abs(jet1P4->Eta() - jet2P4->Eta());

  double lepDelR   = leptonOneP4->DeltaR(*leptonTwoP4);
  double lepDelPhi = abs(leptonOneP4->DeltaPhi(*leptonTwoP4));
  double lepDelEta = abs(leptonOneP4->Eta() - leptonTwoP4->Eta());

  tree->jetsM        = jetSys.M()            ;
  tree->jetsPt       = jetSys.Pt()           ;
  tree->jetsEta      = jetSys.Eta()          ;
  tree->jetsDelPhi   = jbDelPhi              ;
  tree->jetsDelEta   = jbDelEta              ;
  tree->jetsDelR     = jbDelR                ;
							       
  tree->lepPt        = lepSys.Pt()           ;
  tree->lepM         = lepSys.M()            ;
  tree->lepEta       = lepSys.Eta()          ;
  tree->lepPhi       = lepSys.Phi()          ;
						    		
  tree->lepDelPhi    = lepDelPhi             ;
  tree->lepDelEta    = lepDelEta             ;
  tree->lepDelR      = lepDelR               ;
  tree->lepPtOverM   = lepSys.Pt()/lepSys.M();




  double jslDelR   = jetSys.DeltaR(lepSys);
  double jslDelPhi = abs(jetSys.DeltaPhi(lepSys));
  double jslDelEta = abs(jetSys.Eta() - lepSys.Eta());

  
  int bjetnum = -1;
  //if both or neither are b's, pick the closest in delta R
  tree->jet1tagged = jet1tagged;
  tree->jet2tagged = jet2tagged;
  
  if((jet1tagged && jet2tagged) || (!jet1tagged && !jet2tagged)) {
    if(lepSys.DeltaR(*jet1P4) < lepSys.DeltaR(*jet2P4)) {
      bjetnum = 1;
    } else {
      bjetnum = 2;
    }
  } else if(jet1tagged) {
    bjetnum = 1; 
  } else { //jet 2 is tagged
    bjetnum = 2;
  }

  TLorentzVector blepSys = lepSys + ((bjetnum == 1) ? *jet1P4 : *jet2P4);
  double blM  = blepSys.M();
  double blPt = blepSys.Pt();
  
  double blDelR   = lepSys.DeltaR((bjetnum == 1) ? *jet1P4 : *jet2P4);
  double blDelPhi = abs(lepSys.DeltaPhi((bjetnum == 1) ? *jet1P4 : *jet2P4));
  double blDelEta = abs(lepSys.Eta() -
			((bjetnum == 1) ? jet1P4->Eta() : jet2P4->Eta()));

  TLorentzVector jlepSys = lepSys + ((bjetnum == 2) ? *jet1P4 : *jet2P4);
  double jlM  = jlepSys.M();
  double jlPt = jlepSys.Pt();
  double jlDelR   = lepSys.DeltaR((bjetnum == 2) ? *jet1P4 : *jet2P4);
  double jlDelPhi = abs(lepSys.DeltaPhi((bjetnum == 2) ? *jet1P4 : *jet2P4));
  double jlDelEta = abs(lepSys.Eta() -
			((bjetnum == 2) ? jet1P4->Eta() : jet2P4->Eta()));

  double jblDelR   = blepSys.DeltaR((bjetnum == 2) ? *jet1P4 : *jet2P4);
  double jblDelPhi = abs(blepSys.DeltaPhi((bjetnum == 2) ? *jet1P4 : *jet2P4));
  double jblDelEta = abs(blepSys.Eta() -
			((bjetnum == 2) ? jet1P4->Eta() : jet2P4->Eta()));

  
  tree->sysM          = sys.M()  ;
  tree->sysPt         = sys.Pt() ;
  tree->sysEta        = sys.Eta();
  tree->jetsLepDelPhi = jslDelPhi;
  tree->jetsLepDelEta = jslDelEta;
  tree->jetsLepDelR   = jslDelR  ;
  tree->jetLepM       = jlM      ;
  tree->jetLepPt      = jlPt     ;
  tree->jetLepDelPhi  = jlDelPhi ;
  tree->jetLepDelEta  = jlDelEta ;
  tree->jetLepDelR    = jlDelR   ;
  tree->bjetLepM      = blM      ;
  tree->bjetLepPt     = blPt     ;
  tree->bjetLepDelPhi = blDelPhi ;
  tree->bjetLepDelEta = blDelEta ;
  tree->bjetLepDelR   = blDelR   ;
  tree->jetbLepDelPhi = jblDelPhi;
  tree->jetbLepDelEta = jblDelEta;
  tree->jetbLepDelR   = jblDelR  ;


  tree->jet1px        = jet1P4->Px(); 
  tree->jet1py        = jet1P4->Py(); 
  tree->jet1pz        = jet1P4->Pz(); 
  tree->jet1pt        = jet1P4->Pt(); 
  tree->jet1p         = jet1P4->P();  
  tree->jet1e         = jet1P4->E();  
  tree->jet1m         = jet1P4->M();  
  tree->jet1eta       = jet1P4->Eta();
  tree->jet1phi       = jet1P4->Phi();
  tree->jet1flavor    = jet1Flavor;   
  tree->jet1d0        = jet1D0;	     
  tree->jet1tag       = jet1Tag;	     
  tree->jet1puid      = jet1PUID;     

  tree->jet2px        = jet2P4->Px(); 
  tree->jet2py        = jet2P4->Py(); 
  tree->jet2pz        = jet2P4->Pz(); 
  tree->jet2pt        = jet2P4->Pt(); 
  tree->jet2p         = jet2P4->P();  
  tree->jet2e         = jet2P4->E();  
  tree->jet2m         = jet2P4->M();  
  tree->jet2eta       = jet2P4->Eta();
  tree->jet2phi       = jet2P4->Phi();
  tree->jet2flavor    = jet2Flavor;   
  tree->jet2d0        = jet2D0;	     
  tree->jet2tag       = jet2Tag;	     
  tree->jet2puid      = jet2PUID;     
                     
  tree->lep1px        = leptonOneP4->Px() ;
  tree->lep1py        = leptonOneP4->Py() ;
  tree->lep1pz        = leptonOneP4->Pz() ;
  tree->lep1pt        = leptonOneP4->Pt() ;
  tree->lep1p         = leptonOneP4->P()  ;
  tree->lep1e         = leptonOneP4->E()  ;
  tree->lep1m         = leptonOneP4->M()  ;
  tree->lep1eta       = leptonOneP4->Eta();
  tree->lep1phi       = leptonOneP4->Phi();
  tree->lep1flavor    = leptonOneFlavor	  ;
  tree->lep1iso       = leptonOneIso      ;
  Double_t relIso1 = leptonOneIso/leptonOneP4->Pt();
  tree->lep1reliso    = relIso1           ;
  tree->lep1q         = leptonOneQ        ;
  tree->lep1trigger   = leptonOneTrigger  ;

  tree->lep2px        = leptonTwoP4->Px() ;
  tree->lep2py        = leptonTwoP4->Py() ;
  tree->lep2pz        = leptonTwoP4->Pz() ;
  tree->lep2pt        = leptonTwoP4->Pt() ;
  tree->lep2p         = leptonTwoP4->P()  ;
  tree->lep2e         = leptonTwoP4->E()  ;
  tree->lep2m         = leptonTwoP4->M()  ;
  tree->lep2eta       = leptonTwoP4->Eta();
  tree->lep2phi       = leptonTwoP4->Phi();
  tree->lep2flavor    = leptonTwoFlavor	  ;
  tree->lep2iso       = leptonTwoIso      ;
  Double_t relIso2 = leptonTwoIso/leptonTwoP4->Pt();
  tree->lep2reliso    = relIso2           ;
  tree->lep2q         = leptonTwoQ        ;
  tree->lep2trigger   = leptonTwoTrigger  ;

  
  fEventTree[tree->index]->Fill();
}


Bool_t MultileptonNTupleMaker::Process(Long64_t entry)
{
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
  
  if(entry%50000 == 0) printf("Processing event: %12lld\n", entry);
  
  fChain->GetEntry(entry);
  // cout << "Before Set Entry" << endl;
  // TTreeReader::EEntryStatus ees = fReader.SetEntry(entry);
  // if(ees == TTreeReader::kEntryValid) cout <<"valid entry\n";
  // if(ees == TTreeReader::kEntryNotLoaded) cout <<"not loaded entry\n";
  // if(ees == TTreeReader::kEntryNoTree) cout <<"no tree entry\n";
  // if(ees == TTreeReader::kEntryNotFound) cout <<"not found entry\n";
  // if(ees == TTreeReader::kEntryChainSetupError) cout <<"Chain Setup Error\n";
  // if(ees == TTreeReader::kEntryChainFileError) cout <<"Chain File Error\n";
  // if(ees == TTreeReader::kEntryDictionaryError) cout <<"Dictionary Problem entry\n";
  // else cout << ees << " EES Not Found \n"<< endl;

  //Decide whether or not each jet is a b-Jet
  //if both or neither are b's, pick the closest in delta R
  jet1tagged = BTagModifier(1);
  jet2tagged = BTagModifier(2);
  
  if(fEventSets[0]) FillEventTree(fEventNT[0]);

  TLorentzVector lepSys = (*leptonOneP4) + (*leptonTwoP4);
  Double_t lepM = lepSys.M();
  bool chargeTest = leptonOneQ*leptonTwoQ < 0; //opposite signed
  bool massTest = (lepM < 70.);
  bool b1f = (massTest && nFwdJets > 0 && (nJets+nBJets) == 1 && chargeTest);
  b1f = b1f && ((jet1P4->Pt() > 30. && abs(jet1P4->Eta()) > 2.4) ||
		(jet2P4->Pt() > 30. && abs(jet2P4->Eta()) > 2.4));
  bool b1c = massTest && nFwdJets == 0 && nJets > 1 && chargeTest;
  b1c = b1c && jet1P4->Pt() > 30. && abs(jet1P4->Eta()) < 2.4;
  b1c = b1c && jet2P4->Pt() > 30. && abs(jet2P4->Eta()) < 2.4;
  if(fEventSets[1] && massTest)     FillEventTree(fEventNT[1]);
  if(fEventSets[2] && b1c)          FillEventTree(fEventNT[2]);
  if(fEventSets[3] && b1f)          FillEventTree(fEventNT[3]);
  if(fEventSets[4] && (b1c || b1f)) FillEventTree(fEventNT[4]);


  //adding in analysis cuts 1 step at a time
  b1c = b1c && (jet1tagged || jet2tagged); // at least one jet is b-tagged
  b1f = b1f && (jet1tagged || jet2tagged); // at least one jet is b-tagged

  if(fEventSets[5] && b1c)          FillEventTree(fEventNT[5]);
  if(fEventSets[6] && b1f)          FillEventTree(fEventNT[6]);

  //exactly 1 is b-tagged
  if(fEventSets[11] && b1c && (!jet1tagged || !jet2tagged)) FillEventTree(fEventNT[11]);

  b1c = b1c && (nBJets + nJets) == 2; //2 jets, including bjet
  if(fEventSets[7] && b1c)          FillEventTree(fEventNT[7]);

  b1c = b1c && met < 40.;
  if(fEventSets[8] && b1c)          FillEventTree(fEventNT[8]);

  TLorentzVector jetSys = *jet1P4 + *jet2P4;
  double jslDelPhi = abs(jetSys.DeltaPhi(lepSys));
  b1c = b1c && jslDelPhi > 2.5;
  if(fEventSets[9] && b1c)          FillEventTree(fEventNT[9]);

  b1c = b1c && (lepSys.Pt()/lepSys.M()) > 2.;
  if(fEventSets[10] && b1c)          FillEventTree(fEventNT[10]);

  return kTRUE;
}

void MultileptonNTupleMaker::SlaveTerminate()
{
  // The SlaveTerminate() function is called after all entries or objects
  // have been processed. When running with PROOF SlaveTerminate() is called
  // on each slave server.

}

void MultileptonNTupleMaker::Terminate()
{
  // The Terminate() function is the last function to be called during
  // a query. It always runs on the client, it can be used to present
  // the results graphically or save the results to file.
  printf("Writing output file into %s%s.tree\n",fChain->GetName(),fEnd.Data());
  fOut->Write();
  delete fOut;
  // timer->Stop();
  Double_t cpuTime = timer->CpuTime();
  Double_t realTime = timer->RealTime();
  printf("Processing time: %7.2fs CPU time %7.2fs Wall time\n",cpuTime,realTime);
  if(realTime > 600. ) printf("Processing time: %7.2fmin CPU time %7.2fmin Wall time\n",cpuTime/60.,realTime/60.);

}
