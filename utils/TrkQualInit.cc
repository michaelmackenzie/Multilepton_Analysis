#ifndef __TRKQUALINIT__CC
#define __TRKQUALINIT__CC
// Class to initialize a TMVA factory

#if not defined(__CINT__) || defined(__MAKECINT__)
// needs to be included when makecint runs (ACLIC)
#include "TMVA/Factory.h"
// #include "TMVA/DataLoader.h"
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#endif
#include "../dataFormats/Tree_t.hh"

class TrkQualInit {
public:
  TrkQualInit(int version = 3, int njets = 0) {
    version_ = version;
    njets_ = njets;
  }

  int InitializeVariables(TMVA::Factory &factory, TString selection){
    int status = 0;
    
    factory.AddVariable("lepm" , "M_{ll}" , "GeV", 'F');   
    factory.AddVariable("mtone","MT(MET,l1)","",'F');
    factory.AddVariable("mttwo","MT(MET,l2)","",'F');
    factory.AddVariable("leponept","pT_{l1}","",'F');
    factory.AddVariable("leptwopt","pT_{l2}","",'F');
    factory.AddVariable("leppt", "Pt_{ll}", "GeV", 'F');
    factory.AddSpectator("pxivis","p^{vis}_{#xi}","",'F');
    factory.AddSpectator("pxiinv","p^{inv}_{#xi}","",'F');
    if(njets_)
      factory.AddVariable("njets", "nJets", "", 'F');
    else
      factory.AddSpectator("njets", "nJets", "", 'F');
    
    if((version_ < 3) && !(selection.Contains("emu") || selection.Contains("_")))
      factory.AddVariable("nbjetstot20l", "nBJets (loose ID pt > 20)" , "", 'F');
    
    factory.AddSpectator("lepdeltaeta","#Delta#eta_{ll}","",'F');
    factory.AddSpectator("metdeltaphi","#Delta#phi_{MET,ll}","",'F');
    //tau specific
    if(selection.Contains("tau")) {
      factory.AddVariable("lepmestimate" , "M_{ll}^{Coll}" , "GeV", 'F'); 
      factory.AddVariable("onemetdeltaphi","#Delta#phi_{MET,l1}","",'F');
      factory.AddVariable("twometdeltaphi","#Delta#phi_{MET,l2}","",'F');
      if(version_ == 1 && !selection.Contains("_")) {
	factory.AddVariable("leptwoidone"  , "#tau anti-electron ID", "", 'F');
	factory.AddVariable("leptwoidtwo"  , "#tau anti-muon ID"    , "", 'F');
	factory.AddVariable("leptwoidthree", "#tau anti-jet ID"     , "", 'F');
      } else if((version_ == 2 || version_ == 3) && !selection.Contains("_")) {
	factory.AddSpectator("leptwoidone"  , "#tau anti-electron ID", "", 'F');
	factory.AddSpectator("leptwoidtwo"  , "#tau anti-muon ID"    , "", 'F');
	factory.AddVariable("leptwoidthree", "#tau anti-jet ID"     , "", 'F');
      }
    } else {
      factory.AddSpectator("lepmestimate" , "M_{ll}^{Coll}" , "GeV", 'F');   
      factory.AddSpectator("onemetdeltaphi","#Delta#phi_{MET,l1}","",'F');  
      factory.AddSpectator("twometdeltaphi","#Delta#phi_{MET,l2}","",'F');  
    }
    factory.AddSpectator("leponedeltaphi","#Delta#phi_{l1,ll}","",'F');
    factory.AddSpectator("leptwodeltaphi","#Delta#phi_{l2,ll}","",'F');
    factory.AddSpectator("leponed0","D0_{l1}","",'F');
    factory.AddSpectator("leptwod0","D0_{l2}","",'F');

    if(version_ == 0) {
      factory.AddVariable("htdeltaphi","#Delta#phi_{hT,ll}","",'F');
      factory.AddVariable("ht","pT(#Sigma #vec{P}_{Jet})","",'F');
    } else {
      factory.AddSpectator("htdeltaphi","#Delta#phi_{hT,ll}","",'F');
      factory.AddSpectator("ht","pT(#Sigma #vec{P}_{Jet})","",'F');
    }
    //higgs specific
    if(selection.Contains("h")) {
      // factory.AddVariable("ptoverm := leppt/lepm","pT_{ll}/M_{ll}","",'F');
    } else {
      // factory.AddSpectator("ht","pT(#Sigma #vec{P}_{Jet})","",'F');
    }
    factory.AddVariable("lepdeltaphi","#Delta#phi_{ll}","",'F');
    factory.AddSpectator("htsum","#Sigma pT_{Jet}","",'F');
    factory.AddSpectator("leponeiso","Iso_{l1}","",'F');
    if(version_ == 1 || version_ == 2 || version_ == 3) {
      factory.AddVariable("met","MET","GeV",'F');
    } else {
      factory.AddSpectator("met","MET","GeV",'F');
    }
    
    factory.AddSpectator("lepdeltar","#DeltaR_{ll}","",'F');
    factory.AddSpectator("fulleventweight", "fullEventWeight", "", 'F'); 
    factory.AddSpectator("eventweight", "eventWeight", "", 'F'); 
    factory.AddSpectator("eventcategory", "eventCategory", "", 'F'); 
    return status;
  }

  int InitializeVariables(TMVA::Reader &reader, TString selection, Tree_t& tree){
    int status = 0;
    
    reader.AddVariable("lepm" , &tree.lepm);
    reader.AddVariable("mtone", &tree.mtone);
    reader.AddVariable("mttwo", &tree.mttwo);
    reader.AddVariable("leponept", &tree.leponept);
    reader.AddVariable("leptwopt", &tree.leptwopt);
    reader.AddVariable("leppt", &tree.leppt); 
    reader.AddSpectator("pxivis", &tree.pxivis);
    reader.AddSpectator("pxiinv", &tree.pxiinv);
    if(njets_)
      reader.AddVariable("njets", &tree.njets); 
    else
      reader.AddSpectator("njets", &tree.njets); 
    
    if(version_ < 3 && !(selection.Contains("emu") || selection.Contains("_")))
      reader.AddVariable("nbjetstot20l", &tree.nbjetstot20l); 
    
    reader.AddSpectator("lepdeltaeta", &tree.lepdeltaeta);
    reader.AddSpectator("metdeltaphi", &tree.metdeltaphi);
    //tau specific
    if(selection.Contains("tau")) {
      reader.AddVariable("lepmestimate" , &tree.mestimate); 
      reader.AddVariable("onemetdeltaphi", &tree.onemetdeltaphi);
      reader.AddVariable("twometdeltaphi", &tree.twometdeltaphi);
      if(version_ == 1 && !selection.Contains("_")) {
	reader.AddVariable("leptwoidone"  , &tree.leptwoidone); 
	reader.AddVariable("leptwoidtwo"  , &tree.leptwoidtwo); 
	reader.AddVariable("leptwoidthree", &tree.leptwoidthree); 
      } else if((version_ == 2 || version_ == 3) && !selection.Contains("_")) {
	reader.AddSpectator("leptwoidone"  , &tree.leptwoidone); 
	reader.AddSpectator("leptwoidtwo"  , &tree.leptwoidtwo); 
	reader.AddVariable("leptwoidthree", &tree.leptwoidthree); 
      }
    } else {
      reader.AddSpectator("lepmestimate" , &tree.mestimate); 
      reader.AddSpectator("onemetdeltaphi", &tree.onemetdeltaphi);
      reader.AddSpectator("twometdeltaphi", &tree.twometdeltaphi);
    }
    reader.AddSpectator("leponedeltaphi", &tree.leponedeltaphi);
    reader.AddSpectator("leptwodeltaphi", &tree.leptwodeltaphi);
    reader.AddSpectator("leponed0", &tree.leponed0);
    reader.AddSpectator("leptwod0", &tree.leptwod0);

    if(version_ == 0) {
      reader.AddVariable("htdeltaphi", &tree.htdeltaphi);
      reader.AddVariable("ht", &tree.ht);
    } else {
      reader.AddSpectator("htdeltaphi", &tree.htdeltaphi);
      reader.AddSpectator("ht", &tree.ht);
    }
    //higgs specific
    if(selection.Contains("h")) {
      // reader.AddVariable("ptoverm := leppt/lepm","pT_{ll}/M_{ll}","",'F');
    } else {
      // reader.AddSpectator("ht","pT(#Sigma #vec{P}_{Jet})","",'F');
    }
    reader.AddVariable("lepdeltaphi", &tree.lepdeltaphi);
    reader.AddSpectator("htsum", &tree.htsum);
    reader.AddSpectator("leponeiso", &tree.leponeiso);
    if(version_ == 1 || version_ == 2 || version_ == 3) {
      reader.AddVariable("met", &tree.met);
    } else {
      reader.AddSpectator("met", &tree.met);
    }
    
    reader.AddSpectator("lepdeltar", &tree.lepdeltar);
    reader.AddSpectator("fulleventweight", &tree.fulleventweight); 
    reader.AddSpectator("eventweight", &tree.eventweight); 
    reader.AddSpectator("eventcategory", &tree.eventcategory); 
    return status;
  }

  //fields
  int version_;
  int njets_; //flag for jet binned categories
  
};
#endif