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
  TrkQualInit(int version = TrkQualInit::Default, int njets = 0) {
    version_ = version;
    njets_ = njets;
  }

  int InitializeVariables(TMVA::Factory &factory, TString selection){
    int status = 0;

    if(!selection.Contains("emu"))
      factory.AddVariable("lepm" , "M_{ll}" , "GeV", 'F');   
    else
      factory.AddSpectator("lepm" , "M_{ll}" , "GeV", 'F');
    
    factory.AddVariable("mtone","MT(MET,l1)","",'F');
    factory.AddVariable("mttwo","MT(MET,l2)","",'F');
    factory.AddVariable("onemetdeltaphi","#Delta#phi_{MET,l1}","",'F');
    factory.AddVariable("twometdeltaphi","#Delta#phi_{MET,l2}","",'F');

    if(!selection.Contains("emu")) {
      factory.AddVariable("leponept","pT_{l1}","",'F');
      factory.AddVariable("leptwopt","pT_{l2}","",'F');
      factory.AddVariable("leppt", "pT_{ll}", "GeV", 'F');
    } else {
      factory.AddVariable("leponeptoverm","pT_{l1}/M_{ll}","",'F');
      factory.AddVariable("leptwoptoverm","pT_{l2}/M_{ll}","",'F');
      factory.AddVariable("lepptoverm","pT_{ll}/M_{ll}","",'F');
    }

    // if(njets_)
    //   factory.AddVariable("njets", "nJets", "", 'F');
    // else
    //   factory.AddSpectator("njets", "nJets", "", 'F');
        
    factory.AddVariable("lepdeltaphi","#Delta#phi_{ll}","",'F');
    factory.AddSpectator("lepdeltaeta","#Delta#eta_{ll}","",'F');
    factory.AddSpectator("metdeltaphi","#Delta#phi_{MET,ll}","",'F');

    factory.AddSpectator("pxivis","p^{vis}_{#xi}","",'F');
    factory.AddSpectator("pxiinv","p^{inv}_{#xi}","",'F');

    //tau specific
    if(selection.Contains("tau")) {
      //Delta alpha, difference between loss estimate using ~mass and pT ratio
      if(selection.Contains("z")) {
	if(selection.Contains("_e"))
	  factory.AddVariable("deltaalphaz1", "#Delta#alpha", "", 'F');
	else
	  factory.AddVariable("deltaalphaz2", "#Delta#alpha", "", 'F');
      } else {
	if(selection.Contains("_e"))
	  factory.AddVariable("deltaalphah1", "#Delta#alpha", "", 'F');
	else
	  factory.AddVariable("deltaalphah2", "#Delta#alpha", "", 'F');
      }
      if((!selection.Contains("mutau_e")))
	factory.AddVariable("lepmestimate" , "M_{ll}^{Coll}" , "GeV", 'F'); 
      else
	factory.AddVariable("lepmestimatetwo" , "M_{ll}^{Coll}" , "GeV", 'F'); 

      factory.AddSpectator("leptwoidone"  , "#tau anti-electron ID", "", 'F');
      factory.AddSpectator("leptwoidtwo"  , "#tau anti-muon ID"    , "", 'F');
      factory.AddSpectator("leptwoidthree", "#tau anti-jet ID"     , "", 'F');
    } else { //end tau specific
      factory.AddSpectator("lepmestimate" , "M_{ll}^{Coll}" , "GeV", 'F');   
      factory.AddSpectator("onemetdeltaphi","#Delta#phi_{MET,l1}","",'F');  //FIXME: Remove
      factory.AddSpectator("twometdeltaphi","#Delta#phi_{MET,l2}","",'F');  
    }
    
    factory.AddSpectator("leponedeltaphi","#Delta#phi_{l1,ll}","",'F');
    factory.AddSpectator("leptwodeltaphi","#Delta#phi_{l2,ll}","",'F');
    factory.AddSpectator("leponed0","D0_{l1}","",'F');
    factory.AddSpectator("leptwod0","D0_{l2}","",'F');

    factory.AddSpectator("htdeltaphi","#Delta#phi_{hT,ll}","",'F');
    factory.AddSpectator("ht","pT(#Sigma #vec{P}_{Jet})","",'F');
    factory.AddSpectator("htsum","#Sigma pT_{Jet}","",'F');
    
    factory.AddVariable("jetpt","pT_{Jet}","",'F');    

    factory.AddSpectator("leponeiso","Iso_{l1}","",'F');

    factory.AddVariable("met","MET","GeV",'F');
    
    factory.AddSpectator("lepdeltar","#DeltaR_{ll}","",'F');
    factory.AddSpectator("fulleventweight", "fullEventWeight", "", 'F'); 
    factory.AddSpectator("fulleventweightlum", "fullEventWeightLum", "", 'F'); 
    factory.AddSpectator("eventweight", "eventWeight", "", 'F'); 
    factory.AddSpectator("eventcategory", "eventCategory", "", 'F'); 

    return status;
  }

  int InitializeVariables(TMVA::Reader &reader, TString selection, Tree_t& tree){
    int status = 0;
    
    if(!selection.Contains("emu"))
      reader.AddVariable("lepm" , &tree.lepm);
    else
      reader.AddSpectator("lepm" , &tree.lepm);

    reader.AddVariable("mtone", &tree.mtone);
    reader.AddVariable("mttwo", &tree.mttwo);
    reader.AddVariable("onemetdeltaphi", &tree.onemetdeltaphi);
    reader.AddVariable("twometdeltaphi", &tree.twometdeltaphi);

    if(!selection.Contains("emu")) {
      reader.AddVariable("leponept", &tree.leponept);
      reader.AddVariable("leptwopt", &tree.leponept);
      reader.AddVariable("leppt", &tree.leppt); 
    } else {
      reader.AddVariable("leponeptoverm", &tree.leponeptoverm);
      reader.AddVariable("leptwoptoverm", &tree.leponeptoverm);
      reader.AddVariable("lepptoverm", &tree.lepptoverm); 
    }
    
    // if(njets_)
    //   reader.AddVariable("njets", &tree.njets); 
    // else
    //   reader.AddSpectator("njets", &tree.njets); 
    
    reader.AddVariable("lepdeltaphi", &tree.lepdeltaphi);
    reader.AddSpectator("lepdeltaeta", &tree.lepdeltaeta);
    reader.AddSpectator("metdeltaphi", &tree.metdeltaphi);

    reader.AddSpectator("pxivis", &tree.pxivis);
    reader.AddSpectator("pxiinv", &tree.pxiinv);

    //tau specific
    if(selection.Contains("tau")) {
      //Delta alpha, difference between loss estimate using ~mass and pT ratio
      if(selection.Contains("z")) {
	if(selection.Contains("_e"))
	  reader.AddVariable("deltaalphaz1", &tree.deltaalphaz2);
	else
	  reader.AddVariable("deltaalphaz2", &tree.deltaalphaz1);
      } else {
	if(selection.Contains("_e"))
	  reader.AddVariable("deltaalphah1", &tree.deltaalphah2);
	else
	  reader.AddVariable("deltaalphah2", &tree.deltaalphah1);
      }
      if(!selection.Contains("mutau_e"))
	reader.AddVariable("lepmestimate" , &tree.mestimate); 
      else 
	reader.AddVariable("lepmestimatetwo" , &tree.mestimatetwo); //project onto the electron
      
      reader.AddSpectator("leptwoidone"  , &tree.leptwoidone); 
      reader.AddSpectator("leptwoidtwo"  , &tree.leptwoidtwo); 
      reader.AddSpectator("leptwoidthree", &tree.leptwoidthree); 
    } else { //end tau specific
      reader.AddSpectator("lepmestimate" , &tree.mestimate); 
      reader.AddSpectator("onemetdeltaphi", &tree.onemetdeltaphi); //FIXME: Remove
      reader.AddSpectator("twometdeltaphi", &tree.twometdeltaphi);
    }
    
    reader.AddSpectator("leponedeltaphi", &tree.leponedeltaphi);
    reader.AddSpectator("leptwodeltaphi", &tree.leptwodeltaphi);
    reader.AddSpectator("leponed0", &tree.leponed0);
    reader.AddSpectator("leptwod0", &tree.leptwod0);

    reader.AddSpectator("htdeltaphi", &tree.htdeltaphi);
    reader.AddSpectator("ht", &tree.ht);
    reader.AddSpectator("htsum", &tree.htsum);

    reader.AddVariable("jetpt",&tree.jetpt);

    reader.AddSpectator("leponeiso", &tree.leponeiso);

    reader.AddVariable("met", &tree.met);
    
    reader.AddSpectator("lepdeltar", &tree.lepdeltar);
    reader.AddSpectator("fulleventweight", &tree.fulleventweight); 
    reader.AddSpectator("fulleventweightlum", &tree.fulleventweightlum); 
    reader.AddSpectator("eventweight", &tree.eventweight); 
    reader.AddSpectator("eventcategory", &tree.eventcategory);
    
    return status;
  }

  //default version
  const static int Default = 7;
  //fields
  int version_;
  int njets_; //flag for jet binned categories
  
};
#endif
