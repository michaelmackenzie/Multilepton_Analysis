#ifndef __TRKQUALINIT__CC
#define __TRKQUALINIT__CC
// Class to initialize a TMVA factory and reader consistently

#if not defined(__CINT__) || defined(__MAKECINT__)
// needs to be included when makecint runs (ACLIC)
#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TTree.h"
#endif

//local includes
#include "interface/Tree_t.hh"

namespace CLFV {

  class TrkQualInit {
  public:

    /**
       Version information:
       x+tau:
       <7: No longer supported
       7 : lepm, mtone, mttwo, onemetdphi, twometdphi, onept, twopt, leppt, lepdeltaphi, deltaalpha, lepmestimate, jetpt
       8 : lepm, oneprimepz, oneprimee, twoprimepz, twoprimepx, twoprimee, metprimee   , deltaalpha, lepmestimate
       9 : development version
       e+mu:
       7 : mtoneoverm, mttwooverm, onemetdphi, twometdphi, oneptoverm, twoptoverm, lepptoverm, lepdeltaphi, jetpt
       8 : 7 but with updated spectators
       9 : development version
     **/
    TrkQualInit(int version = TrkQualInit::Default, int njets = 0) {
      version_ = version;
      njets_ = njets;
    }

    //information for a variable
    struct Var_t {
      TString var_; //name
      TString desc_; //description
      TString unit_; //units, if any
      float* val_; //address
      bool use_; //use or just spectator
      char type_;
      Var_t(TString var, TString desc, TString unit, float* val, bool use = false) : var_(var), desc_(desc),
                                                                                     unit_(unit), val_(val),
                                                                                     use_(use), type_('F') {}
    };

    //get list of variables for training/evaluating MVAs
    std::vector<Var_t> GetVariables(TString selection, Tree_t& tree) {
      std::vector<Var_t> variables;
      const bool emu      = selection.EndsWith("emu");
      const bool tau      = selection.Contains("tau");
      const bool emu_data = emu || selection.Contains("_");
      const bool tau_data = selection.EndsWith("tau");

      std::vector<TString> train_var;
      if(tau) {
        if(version_ ==  7) train_var = {"lepm", "lepmestimate", "deltaalpha", "mtone", "mttwo", "onemetdeltaphi", "twometdeltaphi",
                                        "leponept", "leptwopt", "lepdeltaphi", "leppt", "jetpt"};
        if(version_ ==  8) train_var = {"lepm", "lepmestimate", "deltaalpha", "leponeprimepz", "leponeprimee", "leptwoprimepz",
                                        "leptwoprimepx", "leptwoprimee", "metprimee"};
        if(version_ ==  9) train_var = {"lepm", "lepmestimate", "deltaalpha", "mtone", "mttwo", "mtlep", "dzeta",
                                        "ptdiff", "lepdeltaeta", "lepdeltaphi", "leppt", "jetpt"};
        if(version_ == 10) train_var = {"lepm", "lepmestimate", "deltaalpha", "mtone", "mttwo", "onemetdeltaphi", "twometdeltaphi",
                                        "leponept", "leptwopt", "lepdeltaphi", "leppt", "jetpt"};
        if(version_ == 11) train_var = {"lepm", "lepmestimate", "deltaalpha", "mtone", "mttwo", "mtlep", "dzeta",
                                        "ptdiff", "lepdeltaphi", "lepdeltaeta"};
        if(version_ == 12) train_var = {"lepm", "lepmestimate", "deltaalpha", "mtone", "mttwo", "mtlep",
                                        "leponept", "leptwopt", "lepdeltaphi", "leppt", "jetpt"};
        if(version_ == 13) train_var = {"lepm", "lepmestimate", "deltaalpha", "mtone", "mttwo", "mtlep", "onemetdeltaphi", "twometdeltaphi",
                                        "leponept", "leptwopt", "lepdeltaphi", "leppt", "jetpt"};
        if(version_ == 14) train_var = {"lepm", "lepmestimate", "deltaalpha", "leptrkdeltam", "mtone", "mttwo", "mtlep",
                                        "leponept", "leptwopt", "lepdeltaphi", "lepdeltaphi", "leppt", "jetpt"};
      } else {
        if(version_ ==  7) train_var = {"mtoneoverm", "mttwooverm", "onemetdeltaphi", "twometdeltaphi",
                                        "leponeptoverm", "leptwoptoverm", "lepptoverm", "lepdeltaphi", "jetpt"};
        if(version_ ==  8) train_var = {"mtoneoverm", "mttwooverm", "onemetdeltaphi", "twometdeltaphi",
                                        "leponeptoverm", "leptwoptoverm", "lepptoverm", "lepdeltaphi", "jetpt"};
        if(version_ ==  9) train_var = {"mtoneoverm", "mttwooverm", "mtlepoverm", "lepptoverm",
                                        "ptdiff", "lepdeltaphi", "lepdeltaeta", "dzeta"};
        if(version_ == 11) train_var = {"mtoneoverm", "mttwooverm", "lepptoverm", "leponeptoverm", "lepptwoptoverm",
                                        "ptdiff", "lepdeltaphi", "jetpt", "dzeta"};

      }

      if(train_var.size() == 0) {
        std::cout << "TrkQualInit::" << __func__ << ": No training variables defined for selection "
                  << selection.Data() << " and version " << version_ << std::endl;
        throw 20;
      }

      //necessary event information
      variables.push_back(Var_t("fulleventweight"   ,"fullEventWeight"   ,"", &tree.fulleventweight   ));
      variables.push_back(Var_t("fulleventweightlum","fullEventWeightLum","", &tree.fulleventweightlum));
      variables.push_back(Var_t("eventweight"       ,"eventWeight"       ,"", &tree.eventweight       ));
      variables.push_back(Var_t("eventcategory"     ,"eventCategory"     ,"", &tree.eventcategory     ));
      variables.push_back(Var_t("issignal"          ,"isSignal"          ,"", &tree.issignal          ));
      variables.push_back(Var_t("type"              ,"type"              ,"", &tree.type              ));
      if(version_ > 7) {
        if(tau_data)
          variables.push_back(Var_t("jettotaunonclosure","jet->tau NC"     ,"", &tree.jettotaunonclosure));
        variables.push_back(Var_t("zptup"             ,"Z pT weight up"    ,"", &tree.zptup             ));
        variables.push_back(Var_t("zptdown"           ,"Z pT weight down"  ,"", &tree.zptdown           ));
        variables.push_back(Var_t("jetantimu"         ,"#mu->#tau unc."    ,"", &tree.jetantimu         ));
        variables.push_back(Var_t("jetantiele"        ,"e->#tau unc."      ,"", &tree.jetantiele        ));
        variables.push_back(Var_t("btagsys"           ,"b-Tag systematic"  ,"", &tree.btagsys           ));
        variables.push_back(Var_t("qcdsys"            ,"QCD systematic"    ,"", &tree.qcdsys            ));
      }

      if(version_ == 8 && tau && !selection.Contains("mutau_e")) {
        variables.push_back(Var_t("leponeprimepz0", "l_{1} #tilde{pz}", "GeV", &(tree.leponeprimepz[0])));
        variables.push_back(Var_t("leponeprimee0" , "l_{1} #tilde{E}" , "GeV", &(tree.leponeprimee [0])));
        variables.push_back(Var_t("leptwoprimepz0", "l_{2} #tilde{pz}", "GeV", &(tree.leptwoprimepz[0])));
        variables.push_back(Var_t("leptwoprimepx0", "l_{2} #tilde{px}", "GeV", &(tree.leptwoprimepx[0])));
        variables.push_back(Var_t("leptwoprimee0" , "l_{2} #tilde{E}" , "GeV", &(tree.leptwoprimee [0])));
        variables.push_back(Var_t("metprimee0"    , "MET #tilde{E}"   , "GeV", &(tree.metprimee    [0])));
      } else if(version_ == 8 && tau) { //selections hmutau_e and zmutau_e
        variables.push_back(Var_t("leptwoprimepz1", "l_{2} #tilde{pz}", "GeV", &(tree.leponeprimepz[1])));
        variables.push_back(Var_t("leptwoprimee1" , "l_{2} #tilde{E}" , "GeV", &(tree.leponeprimee [1])));
        variables.push_back(Var_t("leponeprimepz1", "l_{1} #tilde{pz}", "GeV", &(tree.leptwoprimepz[1])));
        variables.push_back(Var_t("leponeprimepx1", "l_{1} #tilde{px}", "GeV", &(tree.leptwoprimepx[1])));
        variables.push_back(Var_t("leponeprimee1" , "l_{1} #tilde{E}" , "GeV", &(tree.leptwoprimee [1])));
        variables.push_back(Var_t("metprimee1"    , "MET #tilde{E}"   , "GeV", &(tree.metprimee    [1])));
      }

      if(tau) {
        variables.push_back(Var_t("lepm" , "M_{ll}"    , "GeV", &tree.lepm ));
        variables.push_back(Var_t("mtone", "MT(MET,l1)", ""   , &tree.mtone));
        variables.push_back(Var_t("mttwo", "MT(MET,l2)", ""   , &tree.mttwo));
        if(version_ >= 8) {
          variables.push_back(Var_t("mtlep", "MT(MET,ll)", ""   , &tree.mtlep));
          variables.push_back(Var_t("leptrkdeltam", "#DeltaM_{trk}", "", &tree.leptrkdeltam));
          variables.push_back(Var_t("leptrkdeltam", "pT_{trk}/pT", "", &tree.trkptoverpt));
        }
      } else { //B->e+mu
        variables.push_back(Var_t("lepm"      , "M_{ll}"           , "GeV", &tree.lepm      ));
        variables.push_back(Var_t("mtoneoverm", "MT(MET,l1)/M_{ll}", ""   , &tree.mtoneoverm));
        variables.push_back(Var_t("mttwooverm", "MT(MET,l2)/M_{ll}", ""   , &tree.mttwooverm));
        if(version_ >= 8) {
          variables.push_back(Var_t("mtlepoverm", "MT(MET,ll)/M_{ll}", ""   , &tree.mtlepoverm));
        }
      }


      variables.push_back(Var_t("onemetdeltaphi","#Delta#phi_{MET,l1}","", &tree.onemetdeltaphi));
      variables.push_back(Var_t("twometdeltaphi","#Delta#phi_{MET,l2}","", &tree.twometdeltaphi));

      if(!emu) {
        variables.push_back(Var_t("leponept","pT_{l1}","GeV", &tree.leponept));
        variables.push_back(Var_t("leptwopt","pT_{l2}","GeV", &tree.leptwopt));
        variables.push_back(Var_t("leppt"   ,"pT_{ll}","GeV", &tree.leppt   ));
      } else {
        variables.push_back(Var_t("leponeptoverm","pT_{l1}/M_{ll}","", &tree.leponeptoverm));
        variables.push_back(Var_t("leptwoptoverm","pT_{l2}/M_{ll}","", &tree.leptwoptoverm));
        variables.push_back(Var_t("lepptoverm"   ,"pT_{ll}/M_{ll}","", &tree.lepptoverm   ));
      }
      variables.push_back(Var_t("lepdeltaphi","#Delta#phi_{ll}"    ,"", &tree.lepdeltaphi));
      variables.push_back(Var_t("lepdeltaeta","#Delta#eta_{ll}"    ,"", &tree.lepdeltaeta));
      variables.push_back(Var_t("metdeltaphi","#Delta#phi_{MET,ll}","", &tree.metdeltaphi));

      // variables.push_back(Var_t("pxivis","p^{vis}_{#xi}","", &tree.pxivis));
      // variables.push_back(Var_t("pxiinv","p^{inv}_{#xi}","", &tree.pxiinv));

      //tau specific
      if(tau) {
        //Delta alpha, difference between loss estimate using ~mass and pT ratio
        if(selection.Contains("z")) {
          if(selection.Contains("_e"))
            variables.push_back(Var_t("deltaalphaz1","#Delta#alpha","",&tree.deltaalphaz1));
          else
            variables.push_back(Var_t("deltaalphaz2","#Delta#alpha","",&tree.deltaalphaz2));
        } else {
          if(selection.Contains("_e"))
            variables.push_back(Var_t("deltaalphah1","#Delta#alpha","",&tree.deltaalphah1));
          else
            variables.push_back(Var_t("deltaalphah2","#Delta#alpha","",&tree.deltaalphah2));
        }
        if((!selection.Contains("mutau_e")))
          variables.push_back(Var_t("lepmestimate","M_{ll}^{Coll}","GeV", &tree.mestimate));
        else
          variables.push_back(Var_t("lepmestimatetwo","M_{ll}^{Coll}","GeV", &tree.mestimatetwo));

      } else { //end tau specific
        variables.push_back(Var_t("lepmestimate","M_{ll}^{Coll}","GeV", &tree.mestimate));
      }

      variables.push_back(Var_t("leponedeltaphi","#Delta#phi_{l1,ll}","", &tree.leponedeltaphi));
      variables.push_back(Var_t("leptwodeltaphi","#Delta#phi_{l2,ll}","", &tree.leptwodeltaphi));
      if(version_ < 8 || !emu)
        variables.push_back(Var_t("leptwod0"    ,"D0_{l2}"           ,"", &tree.leptwod0      ));

      // variables.push_back(Var_t("htdeltaphi","#Delta#phi_{hT,ll}"      ,"", &tree.htdeltaphi));
      if(version_ >= 8) {
        variables.push_back(Var_t("ht"        ,"pT(#Sigma #vec{P}_{Jet})","", &tree.ht        ));
        variables.push_back(Var_t("htsum"     ,"#Sigma pT_{Jet}"         ,"", &tree.htsum     ));
        variables.push_back(Var_t("jetpt"     ,"pT_{Jet}"                ,"", &tree.jetpt     ));
        variables.push_back(Var_t("met"       , "MET"                    , "GeV", &tree.met   ));
      } else {
        variables.push_back(Var_t("jetpt","pT_{Jet}","", &tree.jetpt, true));
      }

      variables.push_back(Var_t("lepdeltar" ,"#DeltaR_{ll}"      ,"", &tree.lepdeltar        ));

      if(version_ >= 8) {
        variables.push_back(Var_t("dzeta","#Delta#zeta","", &tree.dzeta));
        variables.push_back(Var_t("pzetavis","#zeta vis","", &tree.pzetavis));
        variables.push_back(Var_t("pzetainv","#zeta inv","", &tree.pzetainv));
        variables.push_back(Var_t("ptdiff","pT_{1} - pT_{2}","", &tree.ptdiff));
        if(emu) variables.push_back(Var_t("ptdiffoverm","#DeltapT / M","", &tree.ptdiffoverm));
      }

      //Set flags for variables identified as training variables
      for(TString name : train_var) {
        bool found = false;
        if(name.Contains("trk") && emu_data) continue; //only relevant for hadronic taus
        for(Var_t& var : variables) {
          if(var.var_ == name) var.use_ = true;
          else if(name.Contains("lepmestimate") && var.var_.Contains(name)) var.use_ = true; //assume only correct version added
          else if(name.Contains("deltaalpha") && var.var_.Contains(name)) var.use_ = true; //assume only correct version added
          else if(name.Contains("prime") && var.var_.Contains(name)) var.use_ = true; //assume only correct version added
          else continue;
          found = true;
          break; //if found, continue to the next training variable name
        }
        if(!found) {
          std::cout << "TrkQualInit::" << __func__ << ": WARNING! Failed to find training variable named: "
                    << name.Data() << std::endl;
        }
      }

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

    void TestVariables(Long64_t entry, TTree* tree, TString selection, Tree_t& vars, bool setAddresses = true) {
      std::vector<Var_t> variables = GetVariables(selection, vars);
      if(setAddresses) SetBranchAddresses(tree, selection, vars);
      tree->GetEntry(entry);
      for(unsigned index = 0; index < variables.size(); ++index) {
        Var_t& var = variables[index];
        printf("%20s (%30s) = %12.5f", var.var_.Data(), var.desc_.Data(), *(var.val_));
        if(var.use_) printf(" (Variable)\n");
        else         printf(" (Spectator)\n");
        if(TMath::IsNaN(*(var.val_)) || !TMath::Finite(*(var.val_))) printf("!!! Variable %s is nan/not finite!\n", var.var_.Data());
      }
    }
    //default version
    const static int Default = 7;
    //fields
    int version_;
    int njets_; //flag for jet binned categories

  };
}
#endif
