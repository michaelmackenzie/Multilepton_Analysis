#ifndef __TRKQUALINIT__CC
#define __TRKQUALINIT__CC
// Class to initialize a TMVA factory

#if not defined(__CINT__) || defined(__MAKECINT__)
// needs to be included when makecint runs (ACLIC)
#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TTree.h"
#endif
#include "interface/Tree_t.hh"

class TrkQualInit {
public:
  TrkQualInit(int version = TrkQualInit::Default, int njets = 0) {
    version_ = version;
    njets_ = njets;
  }

  //information for a variable
  struct Var_t {
    TString var_;
    TString desc_;
    TString unit_;
    float* val_; //address
    bool use_; //use or just spectator
    char type_;
    Var_t(TString var, TString desc, TString unit, float* val, bool use) : var_(var), desc_(desc),
                                                                          unit_(unit), val_(val),
                                                                          use_(use), type_('F') {}
  };

  //get list of variables for training/evaluating MVAs
  std::vector<Var_t> GetVariables(TString selection, Tree_t& tree) {
    std::vector<Var_t> variables;
    //necessary event information
    variables.push_back(Var_t("fulleventweight"   ,"fullEventWeight"   ,"", &tree.fulleventweight   , false));
    variables.push_back(Var_t("fulleventweightlum","fullEventWeightLum","", &tree.fulleventweightlum, false));
    variables.push_back(Var_t("eventweight"       ,"eventWeight"       ,"", &tree.eventweight       , false));
    variables.push_back(Var_t("eventcategory"     ,"eventCategory"     ,"", &tree.eventcategory     , false));
    variables.push_back(Var_t("issignal"          ,"isSignal"          ,"", &tree.issignal          , false));


    if(!selection.Contains("emu")) {
      variables.push_back(Var_t("lepm" , "M_{ll}"    , "GeV", &tree.lepm , true));
      variables.push_back(Var_t("mtone", "MT(MET,l1)", ""   , &tree.mtone, true));
      variables.push_back(Var_t("mttwo", "MT(MET,l2)", ""   , &tree.mttwo, true));
    } else {
      variables.push_back(Var_t("lepm"      , "M_{ll}"           , "GeV", &tree.lepm      , false));
      variables.push_back(Var_t("mtoneoverm", "MT(MET,l1)/M_{ll}", ""   , &tree.mtoneoverm, true));
      variables.push_back(Var_t("mttwooverm", "MT(MET,l2)/M_{ll}", ""   , &tree.mttwooverm, true));
    }


    variables.push_back(Var_t("onemetdeltaphi","#Delta#phi_{MET,l1}","", &tree.onemetdeltaphi, true));
    variables.push_back(Var_t("twometdeltaphi","#Delta#phi_{MET,l2}","", &tree.twometdeltaphi, true));

    if(!selection.Contains("emu")) {
      variables.push_back(Var_t("leponept","pT_{l1}","GeV", &tree.leponept, true));
      variables.push_back(Var_t("leptwopt","pT_{l2}","GeV", &tree.leptwopt, true));
      variables.push_back(Var_t("leppt"   ,"pT_{ll}","GeV", &tree.leppt   , true));
    } else {
      variables.push_back(Var_t("leponeptoverm","pT_{l1}/M_{ll}","", &tree.leponeptoverm, true));
      variables.push_back(Var_t("leptwoptoverm","pT_{l2}/M_{ll}","", &tree.leptwoptoverm, true));
      variables.push_back(Var_t("lepptoverm"   ,"pT_{ll}/M_{ll}","", &tree.lepptoverm   , true));
    }
    variables.push_back(Var_t("lepdeltaphi","#Delta#phi_{ll}"    ,"", &tree.lepdeltaphi, true));
    variables.push_back(Var_t("lepdeltaeta","#Delta#eta_{ll}"    ,"", &tree.lepdeltaeta, false));
    variables.push_back(Var_t("metdeltaphi","#Delta#phi_{MET,ll}","", &tree.metdeltaphi, false));

    variables.push_back(Var_t("pxivis","p^{vis}_{#xi}","", &tree.pxivis, false));
    variables.push_back(Var_t("pxiinv","p^{inv}_{#xi}","", &tree.pxiinv, false));

    //tau specific
    if(selection.Contains("tau")) {
      //Delta alpha, difference between loss estimate using ~mass and pT ratio
      if(selection.Contains("z")) {
        if(selection.Contains("_e"))
          variables.push_back(Var_t("deltaalphaz1","#Delta#alpha","",&tree.deltaalphaz1, true));
        else
          variables.push_back(Var_t("deltaalphaz2","#Delta#alpha","",&tree.deltaalphaz2, true));
      } else {
        if(selection.Contains("_e"))
          variables.push_back(Var_t("deltaalphah1","#Delta#alpha","",&tree.deltaalphah1, true));
        else
          variables.push_back(Var_t("deltaalphah2","#Delta#alpha","",&tree.deltaalphah2, true));
      }
      if((!selection.Contains("mutau_e")))
        variables.push_back(Var_t("lepmestimate","M_{ll}^{Coll}","GeV", &tree.mestimate, true));
      else
        variables.push_back(Var_t("lepmestimatetwo","M_{ll}^{Coll}","GeV", &tree.mestimatetwo, true));

      // variables.push_back(Var_t("leptwoidone"  ,"#tau anti-electron ID","", &tree.leptwoidone  , false));
      // variables.push_back(Var_t("leptwoidtwo"  ,"#tau anti-muon ID"    ,"", &tree.leptwoidtwo  , false));
      // variables.push_back(Var_t("leptwoidthree","#tau anti-jet ID"     ,"", &tree.leptwoidthree, false));
    } else { //end tau specific
      variables.push_back(Var_t("lepmestimate","M_{ll}^{Coll}","GeV", &tree.mestimate, false));
      // variables.push_back(Var_t("onemetdeltaphi","#Delta#phi_{MET,l1}","", &tree.onemetdeltaphi, false)); //FIXME: Remove
      // variables.push_back(Var_t("twometdeltaphi","#Delta#phi_{MET,l2}","", &tree.onemetdeltaphi, false));
    }

    variables.push_back(Var_t("leponedeltaphi","#Delta#phi_{l1,ll}","", &tree.leponedeltaphi, false));
    variables.push_back(Var_t("leptwodeltaphi","#Delta#phi_{l2,ll}","", &tree.leptwodeltaphi, false));
    variables.push_back(Var_t("leponed0"      ,"D0_{l1}"           ,"", &tree.leponed0      , false));
    variables.push_back(Var_t("leptwod0"      ,"D0_{l2}"           ,"", &tree.leptwod0      , false));

    variables.push_back(Var_t("htdeltaphi","#Delta#phi_{hT,ll}"      ,"", &tree.htdeltaphi, false));
    variables.push_back(Var_t("ht"        ,"pT(#Sigma #vec{P}_{Jet})","", &tree.ht        , false));
    variables.push_back(Var_t("htsum"     ,"#Sigma pT_{Jet}"         ,"", &tree.htsum     , false));

    //FIXME: Remove jet pT from Z->X+tau since doesn't help
    variables.push_back(Var_t("jetpt","pT_{Jet}","", &tree.jetpt, true));

    // variables.push_back(Var_t("leponeiso","Iso_{l1}","", &tree.leponeiso, false));

    // variables.push_back(Var_t("met","MET","GeV", &tree.met, true));

    variables.push_back(Var_t("lepdeltar"         ,"#DeltaR_{ll}"      ,"", &tree.lepdeltar         , false));

    return variables;
  }

  int InitializeVariables(TMVA::DataLoader &loader, TString selection){
    int status = 0;
    Tree_t tree; //not used
    std::vector<Var_t> variables = GetVariables(selection, tree);
    for(unsigned index = 0; index < variables.size(); ++index) {
      Var_t& var = variables[index];
      if(var.use_) loader.AddVariable (var.var_.Data(), var.desc_.Data(), var.unit_.Data(), var.type_);
      else         loader.AddSpectator(var.var_.Data(), var.desc_.Data(), var.unit_.Data(), var.type_);
    }
    return status;
  }

  int InitializeVariables(TMVA::Reader &reader, TString selection, Tree_t& tree){
    int status = 0;
    std::vector<Var_t> variables = GetVariables(selection, tree);
    for(unsigned index = 0; index < variables.size(); ++index) {
      Var_t& var = variables[index];
      if(var.use_) reader.AddVariable (var.var_.Data(), var.val_);
      else         reader.AddSpectator(var.var_.Data(), var.val_);
    }
    return status;
  }

  int SetBranchAddresses(TTree* tree, TString selection, Tree_t& tree_t){
    int status = 0;
    std::vector<Var_t> variables = GetVariables(selection, tree_t);
    for(unsigned index = 0; index < variables.size(); ++index) {
      Var_t& var = variables[index];
      tree->SetBranchAddress(var.var_.Data(), var.val_);
    }
    return status;
  }

  //default version
  const static int Default = 7;
  //fields
  int version_;
  int njets_; //flag for jet binned categories

};
#endif
