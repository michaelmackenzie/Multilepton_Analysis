//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Sep 10 09:06:07 2019 by ROOT version 6.06/01
// from TTree bltTree_ttbar_inclusive/bltTree_ttbar_inclusive
// found on file: /eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190909_153240/output_ttbar_inclusive.root
//////////////////////////////////////////////////////////

#ifndef ZTauTauHistMaker_hh
#define ZTauTauHistMaker_hh

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>

// Headers needed by this particular selector
#include "TLorentzVector.h"
#include "TStopwatch.h"
#include "TH1.h"
#include "TH2.h"

#include <iostream>


class ZTauTauHistMaker : public TSelector {
public :
   TTreeReader     fReader;  //!the tree reader
   TTree          *fChain = 0;   //!pointer to the analyzed TTree or TChain

   // Readers to access the data (delete the ones you do not need).
   TTreeReaderValue<UInt_t> runNumber = {fReader, "runNumber"};
   TTreeReaderValue<ULong64_t> eventNumber = {fReader, "evtNumber"};
   TTreeReaderValue<UInt_t> lumiSection = {fReader, "lumiSection"};
   TTreeReaderValue<UInt_t> nPV = {fReader, "nPV"};
   TTreeReaderValue<Float_t> nPU = {fReader, "nPU"};
   TTreeReaderValue<UInt_t> mcEra = {fReader, "mcEra"};
   TTreeReaderValue<UInt_t> triggerLeptonStatus = {fReader, "triggerLeptonStatus"};
   TTreeReaderValue<Float_t> eventWeight = {fReader, "eventWeight"};
   TTreeReaderValue<Float_t> genWeight = {fReader, "genWeight"};
   TTreeReaderValue<Float_t> puWeight = {fReader, "puWeight"};
   TTreeReaderValue<Float_t> topPtWeight = {fReader, "topPtWeight"};
   TTreeReaderValue<Int_t> tauDecayMode = {fReader, "tauDecayMode"};
   TTreeReaderValue<Float_t> tauMVA = {fReader, "tauMVA"};
   TTreeReaderValue<Int_t> tauGenFlavor = {fReader, "tauGenFlavor"};
   TTreeReaderValue<Int_t> tauGenFlavorHad = {fReader, "tauGenFlavorHad"};
   TTreeReaderValue<Float_t> tauVetoedJetPt = {fReader, "tauVetoedJetPt"};
   TTreeReaderValue<Float_t> tauVetoedJetPtUnc = {fReader, "tauVetoedJetPtUnc"};
   TTreeReaderValue<TLorentzVector> leptonOneP4 = {fReader, "leptonOneP4"};
   TTreeReaderValue<TLorentzVector> leptonTwoP4 = {fReader, "leptonTwoP4"};
   TTreeReaderValue<Int_t> leptonOneFlavor = {fReader, "leptonOneFlavor"};
   TTreeReaderValue<Int_t> leptonTwoFlavor = {fReader, "leptonTwoFlavor"};
   TTreeReaderValue<TLorentzVector> photonP4 = {fReader, "photonP4"};
   TTreeReaderValue<UInt_t> nMuons = {fReader, "nMuons"};
   TTreeReaderValue<UInt_t> nElectrons = {fReader, "nElectrons"};
   TTreeReaderValue<UInt_t> nTaus = {fReader, "nTaus"};
   TTreeReaderValue<UInt_t> nPhotons = {fReader, "nPhotons"};
   TTreeReaderValue<UInt_t> nJets = {fReader, "nJets"};
   TTreeReaderValue<UInt_t> nBJets = {fReader, "nBJets"};
   TTreeReaderValue<UInt_t> nGenTausHad = {fReader, "nGenTausHad"};
   TTreeReaderValue<UInt_t> nGenTausLep = {fReader, "nGenTausLep"};
   TTreeReaderValue<Float_t> htSum = {fReader, "htSum"};
   TTreeReaderValue<Float_t> ht = {fReader, "ht"};
   TTreeReaderValue<Float_t> htPhi = {fReader, "htPhi"};
   TTreeReaderValue<Float_t> met = {fReader, "met"};
   TTreeReaderValue<Float_t> metPhi = {fReader, "metPhi"};


   ZTauTauHistMaker(TTree * /*tree*/ =0) { }
   virtual ~ZTauTauHistMaker() { }
   virtual Int_t   Version() const { return 2; }
   virtual void    Begin(TTree *tree);
   virtual void    SlaveBegin(TTree *tree);
   virtual void    Init(TTree *tree);
   virtual Bool_t  Notify();
   virtual Bool_t  Process(Long64_t entry);
   virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
   virtual void    SetOption(const char *option) { fOption = option; }
   virtual void    SetObject(TObject *obj) { fObject = obj; }
   virtual void    SetInputList(TList *input) { fInput = input; }
   virtual TList  *GetOutputList() const { return fOutput; }
   virtual void    SlaveTerminate();
   virtual void    Terminate();

   ClassDef(ZTauTauHistMaker,0);

};

#endif

#ifdef ZTauTauHistMaker_cxx
void ZTauTauHistMaker::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the reader is initialized.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   fReader.SetTree(tree);
}

Bool_t ZTauTauHistMaker::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}


#endif // #ifdef ZTauTauHistMaker_cxx
