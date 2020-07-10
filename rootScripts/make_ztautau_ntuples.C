/**
   Script to clone BLT ZTauTau Analyzer trees and add MVA scores to the trees
 **/


namespace {
  //Tree Variables
  struct Tree_t {
    //lepton variables
    float leponept;
    float leponem;
    float leponeeta;
    float leponed0;
    float leponeiso;
    float leptwopt;
    float leptwom;
    float leptwoeta;
    float leptwod0;
    float leptwoiso;
    //di-lepton variables
    float lepp;
    float leppt;
    float lepm;
    float lepeta;
    float lepdeltaeta;
    float lepdeltar;
    float lepdeltaphi;

    //extra angles
    float htdeltaphi;    
    float metdeltaphi;
    float leponedeltaphi;
    float leptwodeltaphi;
    float onemetdeltaphi;
    float twometdeltaphi;

    //MET variables
    float met;
    float metphi;
    float mtone;
    float mttwo;
    float pxivis;
    float pxiinv;
    float ptauvisfrac;
    float mestimate;
    float mestimatetwo;
    
    //Event variables
    float ht;
    float htsum;
    float njets;
    float nbjets; //pt > 30
    float nbjetsm;
    float nbjetsl;
    float nbjetstot25; //pt > 25
    float nbjetstot25m;
    float nbjetstot25l;
    float nbjetstot20; //pt > 20
    float nbjetstot20m;
    float nbjetstot20l;
    float nphotons;
    float eventweight;
    float fulleventweight; //includes cross-section and number gen
    float eventcategory; //for identifying the process in mva trainings

    //identify to use in training
    float train; //  < 0 --> testing, > 0 --> training sample
  };
  Tree_t fTreeVars;
  enum {kMaxMVAs = 80};
  //Define relevant fields
  TStopwatch* timer = new TStopwatch();
  TMVA::Reader* mva[kMaxMVAs]; //read and apply mva weight files
  vector<TString> fMvaNames = { //mva names for getting weights
    "mutau_BDT_8.higgs","mutau_BDT_8.Z0", //0 - 9: total mvas
    "etau_BDT_28.higgs","etau_BDT_28.Z0",
    "emu_BDT_47.higgs","emu_BDT_47.Z0",
    "mutau_e_BDT_48.higgs","mutau_e_BDT_48.Z0",
    "etau_mu_BDT_48.higgs","etau_mu_BDT_48.Z0", 
    "mutau_BDT_18.higgs","mutau_BDT_18.Z0", //10 - 19: 0 jets
    "etau_BDT_38.higgs","etau_BDT_38.Z0",
    "emu_BDT_58.higgs","emu_BDT_58.Z0",
    "mutau_e_BDT_58.higgs","mutau_e_BDT_58.Z0",
    "etau_mu_BDT_58.higgs","etau_mu_BDT_58.Z0", 
    "mutau_BDT_19.higgs","mutau_BDT_19.Z0", // 20 - 29: 1 jet
    "etau_BDT_39.higgs","etau_BDT_39.Z0",
    "emu_BDT_59.higgs","emu_BDT_59.Z0",
    "mutau_e_BDT_59.higgs","mutau_e_BDT_59.Z0",
    "etau_mu_BDT_59.higgs","etau_mu_BDT_59.Z0", 
    "mutau_BDT_20.higgs","mutau_BDT_20.Z0",  // 30 - 39: >1 jet
    "etau_BDT_40.higgs","etau_BDT_40.Z0",
    "emu_BDT_60.higgs","emu_BDT_60.Z0",
    "mutau_e_BDT_60.higgs","mutau_e_BDT_60.Z0",
    "etau_mu_BDT_60.higgs","etau_mu_BDT_60.Z0"
  };
  Int_t fIsJetBinnedMVAs[kMaxMVAs]; //storing number of jets for MVA, < 0 if not binned
  float fMvaOutputs[kMaxMVAs]; //store MVA scores
  TBranch* fMvaBranches[kMaxMVAs]; //mva branches in the new ttree
  float met; //standalone to not update with the correction in new tree
  float metPhi;
  float metCorr;
  float metCorrPhi;

  TLorentzVector* leptonOneP4;
  TLorentzVector* leptonTwoP4;
  UInt_t nElectrons;
  UInt_t nMuons;
  UInt_t nTaus;
  UInt_t njets;
  UInt_t nBJets                      ;
  UInt_t nBJetsM                     ;
  UInt_t nBJetsL                     ;
  UInt_t nBJets25                    ;
  UInt_t nBJets25M                   ;
  UInt_t nBJets25L                   ;
  UInt_t nBJets20                    ;
  UInt_t nBJets20M                   ;
  UInt_t nBJets20L                   ;
  UInt_t nBJets25Tot                 ;
  UInt_t nBJets25TotM                ;
  UInt_t nBJets25TotL                ;
  UInt_t nBJetsTot                   ;
  UInt_t nBJetsTotM                  ;
  UInt_t nBJetsTotL                  ;
  Long64_t fentry;
  Long64_t fMaxEntries = 0;
  float htPhi;
  
  bool debug = false;
}

Int_t book_mvas() {
  TMVA::Tools::Instance(); //load the library
  for(int i = 0; i < kMaxMVAs; ++i) mva[i] = 0; //initially 0s
    
  for(unsigned mva_i = 0; mva_i < fMvaNames.size(); ++mva_i) {
      
    mva[mva_i] = new TMVA::Reader("!Color:!Silent");
    TString selection = "";
    if(fMvaNames[mva_i].Contains("higgs")) selection += "h";
    else selection += "z";
    if(fMvaNames[mva_i].Contains("mutau")) selection += "mutau";
    else if(fMvaNames[mva_i].Contains("etau")) selection += "etau";
    else if(fMvaNames[mva_i].Contains("emu")) selection += "emu";
    else {
      printf ("Warning! Didn't determine mva weight file %s corresponding selection!\n",
	      fMvaNames[mva_i].Data());
      selection += "mutau"; //just to default to something
    }
    //add for leptonic tau channels FIXME: Put as part of original check
    if(fMvaNames[mva_i].Contains("mutau_e")) selection += "_e";
    else if(fMvaNames[mva_i].Contains("etau_mu")) selection += "_mu";

    Int_t isJetBinned = -1; // -1 is not binned, 0 = 0 jets, 1 = 1 jet, 2 = >1 jets
    if(fMvaNames[mva_i].Contains("_18") || //0 jet
       fMvaNames[mva_i].Contains("_38") ||
       fMvaNames[mva_i].Contains("_58"))
      isJetBinned = 0;
    else if(fMvaNames[mva_i].Contains("_19") || //1 jet
	    fMvaNames[mva_i].Contains("_39") ||
	    fMvaNames[mva_i].Contains("_59"))
      isJetBinned = 1;
    else if(fMvaNames[mva_i].Contains("_20") || //>1 jet
	    fMvaNames[mva_i].Contains("_40") ||
	    fMvaNames[mva_i].Contains("_60"))
      isJetBinned = 2;

    fIsJetBinnedMVAs[mva_i] = isJetBinned; //store for checking when filling
      
    printf("Using selection %s\n", selection.Data());
    //Order must match the mva training!
    mva[mva_i]->AddVariable("lepm"            ,&fTreeVars.lepm           ); 
    mva[mva_i]->AddVariable("mtone"           ,&fTreeVars.mtone          );
    mva[mva_i]->AddVariable("mttwo"           ,&fTreeVars.mttwo          );
    mva[mva_i]->AddVariable("leponept"        ,&fTreeVars.leponept       );
    mva[mva_i]->AddVariable("leptwopt"        ,&fTreeVars.leptwopt       );
    mva[mva_i]->AddVariable("leppt"           ,&fTreeVars.leppt          );
    mva[mva_i]->AddSpectator("pxivis"         ,&fTreeVars.pxivis         );
    mva[mva_i]->AddSpectator("pxiinv"         ,&fTreeVars.pxiinv         );
    if(isJetBinned < 0)
      mva[mva_i]->AddVariable("njets"         ,&fTreeVars.njets          );
    else
      mva[mva_i]->AddSpectator("njets"        ,&fTreeVars.njets          );

    if(fMvaNames[mva_i].Contains("_47")) //use btag information
      mva[mva_i]->AddVariable("nbjetstot20m", &fTreeVars.nbjetstot20m);

    mva[mva_i]->AddSpectator("lepdeltaeta"    ,&fTreeVars.lepdeltaeta    );
    mva[mva_i]->AddSpectator("metdeltaphi"    ,&fTreeVars.metdeltaphi    );

    //tau specific
    if(selection.Contains("tau")) {
      mva[mva_i]->AddVariable("lepmestimate"   ,&fTreeVars.mestimate     ); 
      mva[mva_i]->AddVariable("onemetdeltaphi" ,&fTreeVars.onemetdeltaphi);
      mva[mva_i]->AddVariable("twometdeltaphi" ,&fTreeVars.twometdeltaphi);
    } else {
      mva[mva_i]->AddSpectator("lepmestimate"  ,&fTreeVars.mestimate     ); 
      mva[mva_i]->AddSpectator("onemetdeltaphi",&fTreeVars.onemetdeltaphi);
      mva[mva_i]->AddSpectator("twometdeltaphi",&fTreeVars.twometdeltaphi); 
    }
      
    //Spectators from mva training also required!
    mva[mva_i]->AddSpectator("leponedeltaphi"  ,&fTreeVars.leponedeltaphi );
    mva[mva_i]->AddSpectator("leptwodeltaphi"  ,&fTreeVars.leptwodeltaphi );
    mva[mva_i]->AddSpectator("leponed0"       ,&fTreeVars.leponed0       );
    mva[mva_i]->AddSpectator("leptwod0"       ,&fTreeVars.leptwod0       );
    //boson specific
    if(isJetBinned != 0) {
      mva[mva_i]->AddVariable("htdeltaphi"     ,&fTreeVars.htdeltaphi     );
      mva[mva_i]->AddVariable("ht"            ,&fTreeVars.ht             ); 
    } else {
      mva[mva_i]->AddSpectator("htdeltaphi"     ,&fTreeVars.htdeltaphi     );
      mva[mva_i]->AddSpectator("ht"           ,&fTreeVars.ht             ); 
    }
    mva[mva_i]->AddVariable("lepdeltaphi"    ,&fTreeVars.lepdeltaphi    );
    mva[mva_i]->AddSpectator("htsum"          ,&fTreeVars.htsum          ); 
    mva[mva_i]->AddSpectator("leponeiso"      ,&fTreeVars.leponeiso      );
    mva[mva_i]->AddSpectator("met"            ,&fTreeVars.met            );
    mva[mva_i]->AddSpectator("lepdeltar"      ,&fTreeVars.lepdeltar      );
    mva[mva_i]->AddSpectator("fulleventweight",&fTreeVars.fulleventweight);
    mva[mva_i]->AddSpectator("eventweight"    ,&fTreeVars.eventweight    );
    mva[mva_i]->AddSpectator("eventcategory"  ,&fTreeVars.eventcategory  );

    //Initialize MVA weight file
    const char* f = Form("weights/%s.weights.xml",fMvaNames[mva_i].Data());
    mva[mva_i]->BookMVA(fMvaNames[mva_i].Data(),f);
    printf("Booked MVA %s with selection %s\n", fMvaNames[mva_i].Data(), selection.Data());
  }
  return 0;
}

//selections: 1 = mutau, 2 = etau, 5 = emu
Int_t initialize_tree_vars(int selection) {
  if(nBJets  > nBJetsM) nBJetsM = nBJets;
  if(nBJetsM > nBJetsL) nBJetsL = nBJetsM;
  if(nBJets25   > nBJets25M ) nBJets25M = nBJets25;
  if(nBJets25M  > nBJets25L ) nBJets25L = nBJets25M;
  if(nBJets20   > nBJets20M ) nBJets20M = nBJets20;
  if(nBJets20M  > nBJets20L ) nBJets20L = nBJets20M;

  //pt > 25
  nBJets25Tot = nBJets + nBJets25;
  nBJets25TotM = nBJetsM + nBJets25M;
  nBJets25TotL = nBJetsL + nBJets25L;

  //pt > 20
  nBJetsTot = nBJets + nBJets25 + nBJets20;
  nBJetsTotM = nBJetsM + nBJets25M + nBJets20M;
  nBJetsTotL = nBJetsL + nBJets25L + nBJets20L;

  //update MET with correction
  TVector3 missing(met*cos(metPhi), met*sin(metPhi), 0.);
  TVector3 missingCorr(metCorr*cos(metCorrPhi), metCorr*sin(metCorrPhi), 0.);
  missing = missing + missingCorr;
  //updated MET variables
  fTreeVars.met = missing.Mag();
  fTreeVars.metphi = missing.Phi();

  fTreeVars.leponept  = leptonOneP4->Pt();
  fTreeVars.leptwopt  = leptonTwoP4->Pt();
  TLorentzVector lep = *leptonOneP4 + *leptonTwoP4;
  fTreeVars.leppt  = lep.Pt();
  fTreeVars.lepm   = lep.M();
  // fTreeVars.lepeta = lep.Eta();
  // fTreeVars.lepdeltar   = leptonOneP4->DeltaR(*leptonTwoP4);
  fTreeVars.lepdeltaphi = abs(leptonOneP4->DeltaPhi(*leptonTwoP4));
  fTreeVars.lepdeltaeta = abs(leptonOneP4->Eta() - leptonTwoP4->Eta());

  //phi differences
  fTreeVars.htdeltaphi = abs(lep.Phi() - htPhi);
  if(fTreeVars.htdeltaphi > M_PI)
    fTreeVars.htdeltaphi = abs(2.*M_PI - fTreeVars.htdeltaphi);
  fTreeVars.metdeltaphi = abs(lep.Phi() - fTreeVars.metphi);
  if(fTreeVars.metdeltaphi > M_PI)
    fTreeVars.metdeltaphi = abs(2.*M_PI - fTreeVars.metdeltaphi);
  fTreeVars.leponedeltaphi = abs(leptonOneP4->DeltaPhi(lep));
  fTreeVars.leptwodeltaphi = abs(leptonTwoP4->DeltaPhi(lep));
  fTreeVars.onemetdeltaphi = abs(leptonOneP4->Phi() - fTreeVars.metphi);
  if(fTreeVars.onemetdeltaphi > M_PI)
    fTreeVars.onemetdeltaphi = abs(2.*M_PI - fTreeVars.onemetdeltaphi);
  fTreeVars.twometdeltaphi = abs(leptonTwoP4->Phi() - fTreeVars.metphi);
  if(fTreeVars.twometdeltaphi > M_PI)
    fTreeVars.twometdeltaphi = abs(2.*M_PI - fTreeVars.twometdeltaphi);

  
  fTreeVars.mtone = sqrt(2.*fTreeVars.met*fTreeVars.leponept*(1.-cos(leptonOneP4->Phi() - fTreeVars.metphi)));
  fTreeVars.mttwo = sqrt(2.*fTreeVars.met*fTreeVars.leptwopt*(1.-cos(leptonTwoP4->Phi() - fTreeVars.metphi)));

  //momentum projections onto bisector
  TVector3 lp1 = leptonOneP4->Vect();
  TVector3 lp2 = leptonTwoP4->Vect();
  lp1.SetZ(0.);
  lp2.SetZ(0.);
  TVector3 bisector = (lp1.Mag()*lp2 + lp2.Mag()*lp1); //divides leptons
  if(bisector.Mag() > 0.) bisector.SetMag(1.);
  
  //project onto the bisectors
  fTreeVars.pxivis = (lp1+lp2)*bisector;
  fTreeVars.pxiinv = missing*bisector;
  double pnuest    = max(0.,lp2*missing/lp2.Mag()); //inv pT along tau = tau pt unit vector dot missing
  double pnuesttwo = max(0.,lp1*missing/lp1.Mag()); //inv pT along other lepton (for emu case with tau decay)
  fTreeVars.ptauvisfrac  = lp2.Mag() / (lp2.Mag() + pnuest);
  fTreeVars.mestimate    = fTreeVars.lepm/sqrt(fTreeVars.ptauvisfrac);
  fTreeVars.mestimatetwo = fTreeVars.lepm/sqrt(lp1.Mag() / (lp1.Mag() + pnuesttwo));

  fTreeVars.njets = njets;
  fTreeVars.nbjets = nBJets;
  fTreeVars.nbjetsm = nBJetsM;
  fTreeVars.nbjetsl = nBJetsL;
  fTreeVars.nbjetstot25   = nBJets25Tot;
  fTreeVars.nbjetstot25m  = nBJets25TotM;
  fTreeVars.nbjetstot25l  = nBJets25TotL;
  fTreeVars.nbjetstot20   = nBJetsTot;
  fTreeVars.nbjetstot20m  = nBJetsTotM;
  fTreeVars.nbjetstot20l  = nBJetsTotL;
  //don't actually care about these, just for MVA spectators
  fTreeVars.eventweight = 1.;
  fTreeVars.fulleventweight = 1.;
  fTreeVars.eventcategory = 1;
  fTreeVars.train = 0;
  
  TString selecName = "";
  if(selection == 1)      selecName = "mutau";
  else if(selection == 2) selecName = "etau";
  else if(selection == 5) selecName = "emu";
  else                    selecName = "unknown";
  for(unsigned i = 0; i < fMvaNames.size(); ++i) {
    if(fMvaBranches[i] && (fMvaNames[i].Contains(selecName.Data()) || //is this selection and filling a branch for it
	(selecName == "emu" && (fMvaNames[i].Contains("_e") || fMvaNames[i].Contains("_mu")))) && //or leptonic tau category
       (fIsJetBinnedMVAs[i] < 0 || fIsJetBinnedMVAs[i] == min((int) fTreeVars.njets,2))) //and either not jet binned or right number of jets
      fMvaOutputs[i] = mva[i]->EvaluateMVA(fMvaNames[i].Data());
    else
      fMvaOutputs[i] = -2.;
    if(debug && fentry % 10000 == 0)
      cout << "entry " << fentry << ": MVA " << i << ": score = " << fMvaOutputs[i] << endl;
    if(fMvaOutputs[i] < -100.) 
      cout << "Error value returned for MVA " << fMvaNames[i].Data()
	   << " evaluation, Entry = " << fentry << endl;
  }
  return 0;
}

//read only information needed by MVAs, for selections,  or for debugging
Int_t set_addresses(TTree* fChain) {
  fChain->SetBranchStatus("*", 0); //turn off all branches
  fChain->SetBranchStatus("leptonOneP4"         , 1); fChain->SetBranchAddress("leptonOneP4"         , &leptonOneP4          );   
  fChain->SetBranchStatus("leptonTwoP4"         , 1); fChain->SetBranchAddress("leptonTwoP4"         , &leptonTwoP4          );   
  fChain->SetBranchStatus("nMuons"              , 1); fChain->SetBranchAddress("nMuons"              , &nMuons               );   
  fChain->SetBranchStatus("nElectrons"          , 1); fChain->SetBranchAddress("nElectrons"          , &nElectrons           );   
  fChain->SetBranchStatus("nTaus"               , 1); fChain->SetBranchAddress("nTaus"               , &nTaus                );   
  // fChain->SetBranchStatus("nPhotons"            , 1); fChain->SetBranchAddress("nPhotons"            , &fTreeVars.nphotons   );   
  fChain->SetBranchStatus("nJets"               , 1); fChain->SetBranchAddress("nJets"               , &njets                );   
  // fChain->SetBranchStatus("nFwdJets"            , 1); fChain->SetBranchAddress("nFwdJets"            , &nfwdjets             );   
  fChain->SetBranchStatus("nBJets"              , 1); fChain->SetBranchAddress("nBJets"              , &nBJets               );   
  fChain->SetBranchStatus("nBJetsM"             , 1); fChain->SetBranchAddress("nBJetsM"             , &nBJetsM              );   
  fChain->SetBranchStatus("nBJetsL"             , 1); fChain->SetBranchAddress("nBJetsL"             , &nBJetsL              );   
  fChain->SetBranchStatus("nBJets25"            , 1); fChain->SetBranchAddress("nBJets25"            , &nBJets25             );   
  fChain->SetBranchStatus("nBJets25M"           , 1); fChain->SetBranchAddress("nBJets25M"           , &nBJets25M            );   
  fChain->SetBranchStatus("nBJets25L"           , 1); fChain->SetBranchAddress("nBJets25L"           , &nBJets25L            );   
  fChain->SetBranchStatus("nBJets20"            , 1); fChain->SetBranchAddress("nBJets20"            , &nBJets20             );   
  fChain->SetBranchStatus("nBJets20M"           , 1); fChain->SetBranchAddress("nBJets20M"           , &nBJets20M            );   
  fChain->SetBranchStatus("nBJets20L"           , 1); fChain->SetBranchAddress("nBJets20L"           , &nBJets20L            );   
  fChain->SetBranchStatus("puppMETC"            , 1); fChain->SetBranchAddress("puppMETC"            , &met                  );   
  fChain->SetBranchStatus("puppMETCphi"         , 1); fChain->SetBranchAddress("puppMETCphi"         , &metPhi               );   
  fChain->SetBranchStatus("metCorr"             , 1); fChain->SetBranchAddress("metCorr"             , &metCorr              );   
  fChain->SetBranchStatus("metCorrPhi"          , 1); fChain->SetBranchAddress("metCorrPhi"          , &metCorrPhi           );   
  fChain->SetBranchStatus("ht"                  , 1); fChain->SetBranchAddress("ht"                  , &fTreeVars.ht         );   
  fChain->SetBranchStatus("htPhi"               , 1); fChain->SetBranchAddress("htPhi"               , &htPhi                );   

  //add new branches for MVA outputs
  for(unsigned mva_i = 0; mva_i < fMvaNames.size(); ++mva_i)  fMvaBranches[mva_i] = 0; //set to 0 initially
  int nfound = 0;
  for(unsigned mva_i = 0; mva_i < fMvaNames.size(); ++mva_i)  {
    // //add branch if doesn't already exist unless
    ++nfound;
    if(!fChain->GetBranch(Form("mva%i",mva_i))) {
      fMvaBranches[mva_i] = fChain->Branch(Form("mva%i",mva_i), &(fMvaOutputs[mva_i]));
    } else { //update branch if it exists
      fChain->SetBranchStatus(Form("mva%i", mva_i), 1);
      fChain->SetBranchAddress(Form("mva%i", mva_i), &(fMvaOutputs[mva_i]));
      fMvaBranches[mva_i] = fChain->GetBranch(Form("mva%i",mva_i));

    }
  }      
  return nfound;
}

Int_t make_new_tree(TString path, TString path_in_file, TString tree_name) {
  cout << "Given file path " << path.Data() << " with folder " << path_in_file.Data()
       << " and tree name " << tree_name.Data() << endl;
  
  // get the file and tree we're adding to
  TFile* file = TFile::Open(path.Data(), "UPDATE");
  if(!file) return 1;
  TDirectory* fdir = (TDirectory*) (path_in_file == "" ? file : file->Get(path_in_file.Data()));
  if(!fdir) return 2;
  fdir->cd();
  TTree* tree = (TTree*) fdir->Get(tree_name.Data());
  if(!tree) {
    cout << "Tree not found in given file!\n";
    return 3;
  }
  //set the relevant addresses and add mva branches
  int status = (set_addresses(tree) == 0) ? 4 : 0;
  if(status) return status;
  
  //run through tree and evaluate mvas for each
  Long64_t nentries = tree->GetEntriesFast();
  cout << "Processing " << tree->GetName() << " tree with " << nentries << " entries" << endl;
  for(Long64_t entry = 0; entry < nentries; ++entry) {
    if(entry % 50000 == 0)
      cout << "Processing entry " << entry << "..." << endl;
    if(fMaxEntries > 0 && entry >= fMaxEntries) {
      cout << "Hit maximum entries, " << fMaxEntries << ", exiting\n";
      break;
    }
    fentry = entry;
    Int_t tree_status = tree->GetEntry(entry, 0);
    if(debug && entry%10000 == 0)
      cout << "Status getting entry: " << tree_status << endl;
    int selection = (nElectrons == 0 && nMuons == 1 && nTaus == 1) +
      2*(nElectrons == 1 && nMuons == 0 && nTaus == 1) +
      5*(nElectrons == 1 && nMuons == 1 && nTaus == 0);
    initialize_tree_vars(selection);
    for(unsigned mva_i = 0; mva_i < fMvaNames.size(); ++mva_i) {
      if(fMvaBranches[mva_i]) {
	if(entry%10000 == 0 && debug)
	  cout << "Filling MVA branch " << mva_i << " with score " << fMvaOutputs[mva_i] << endl;
	fMvaBranches[mva_i]->Fill();
      }
    }
  }
  if(debug) tree->Draw("mva5");
  fdir->cd();
  tree->Write();
  fdir->Write();
  file->Write();
  delete file;
  return 0;
}

Int_t initialize() {
  Int_t status = book_mvas();
  return status;
}

Int_t process_standard_trees() {
  int status = 0;
  TString grid_path = "/eos/uscms/store/user/mmackenz/ztautau_trees/";
  vector<TString> files = {"ttbar_inclusive"             ,
			 "DYJetsToLL_M-50_amcatnlo"    ,
			 "DYJetsToLL_M-10to50_amcatnlo",
			 "T_tW-channel"                ,
			 "Tbar_tW-channel"             ,
			 "DYJetsToLL_M-50"             ,
			 "DYJetsToLL_M-10to50"         ,
			 "DY1JetsToLL_M-50"            ,
			 "DY1JetsToLL_M-10to50"        ,
			 "DY2JetsToLL_M-50"            ,
			 "DY2JetsToLL_M-10to50"        ,
			 "DY3JetsToLL_M-50"            ,
			 "DY3JetsToLL_M-10to50"        ,
			 "DY4JetsToLL_M-50"            ,
			 "DY4JetsToLL_M-10to50"        ,
			 "W1JetsToLNu"                 ,
			 "W2JetsToLNu"                 ,
			 "W3JetsToLNu"                 ,
			 "W4JetsToLNu"                 ,
			 "WJetsToLNu"                  ,
			 "WJetsToLNu_ext1"             ,
			 "WJetsToLNu_ext2"             ,
			 "WW"                          ,
			 "WZJetsTo2L2Q"                ,
			 "WZJetsTo3LNu"                ,
			 "ZZJetsTo2L2Nu"               ,
			 "ZZJetsTo2L2Q"                ,
			 "ZZJetsTo4L"                  ,
			 "hzg_gluglu"                  ,
			 "hzg_tth"                     ,
			 "hzg_vbf"                     ,
			 "hzg_wminus"                  ,
			 "hzg_wplus"                   ,
			 "hzg_zh"                      ,
			 "htautau_gluglu"              ,
			 "zetau"                       ,
			 "zmutau"                      ,
			 "zemu"                        ,
			 "hetau"                       ,
			 "hmutau"                      ,
			 "hemu"                        ,
			 "muon_2016B_v2"               , 
			 "muon_2016C"                  , 
			 "muon_2016D"                  , 
			 "muon_2016E"                  , 
			 "muon_2016F"                  , 
			 "muon_2016G"                  , 
			 "muon_2016H_v2"               ,
			 "electron_2016B_v2"               , 
			 "electron_2016C"                  , 
			 "electron_2016D"                  , 
			 "electron_2016E"                  , 
			 "electron_2016F"                  , 
			 "electron_2016G"                  , 
			 "electron_2016H_v2"
  };
  vector<TString> names = {"ttbar_inclusive"            ,  //"ttbar_inclusive"               
			   "zjets_m-50_amcatnlo"        ,  //"DYJetsToLL_M-50_amcatnlo"        
			   "zjets_m-10to50_amcatnlo"    ,  //"DYJetsToLL_M-10to50_amcatnlo"    
			   "t_tw"                       ,  //"T_tW-channel"                    
			   "tbar_tw"                    ,  //"Tbar_tW-channel"                 
			   "zjets_m-50"                 ,  //"DYJetsToLL_M-50"                 
			   "zjets_m-10to50"             ,  //"DYJetsToLL_M-10to50"             
			   "z1jets_m-50"                ,  //"DY1JetsToLL_M-50"                
			   "z1jets_m-10to50"            ,  //"DY1JetsToLL_M-10to50"            
			   "z2jets_m-50"                ,  //"DY2JetsToLL_M-50"                
			   "z2jets_m-10to50"            ,  //"DY2JetsToLL_M-10to50"            
			   "z3jets_m-50"                ,  //"DY3JetsToLL_M-50"                
			   "z3jets_m-10to50"            ,  //"DY3JetsToLL_M-10to50"            
			   "z4jets_m-50"                ,  //"DY4JetsToLL_M-50"                
			   "z4jets_m-10to50"            ,  //"DY4JetsToLL_M-10to50"            
			   "w1jets"                     ,  //"W1JetsToLNu"                     
			   "w2jets"                     ,  //"W2JetsToLNu"                     
			   "w3jets"                     ,  //"W3JetsToLNu"                     
			   "w4jets"                     ,  //"W4JetsToLNu"                     
			   "wjets"                      ,  //"WJetsToLNu"                      
			   "wjets_ext1"                 ,  //"WJetsToLNu_ext1"                 
			   "wjets_ext2"                 ,  //"WJetsToLNu_ext2"                 
			   "ww"                         ,  //"WW"                              
			   "wz_2l2q"                    ,  //"WZJetsTo2L2Q"                    
			   "wz_3lnu"                    ,  //"WZJetsTo3LNu"                    
			   "zz_2l2nu"                   ,  //"ZZJetsTo2L2Nu"                   
			   "zz_2l2q"                    ,  //"ZZJetsTo2L2Q"                    
			   "zz_4l"                      ,  //"ZZJetsTo4L"                      
			   "hzg_gluglu"                 ,  //"hzg_gluglu"                      
			   "hzg_tth"                    ,  //"hzg_tth"                         
			   "hzg_vbf"                    ,  //"hzg_vbf"                         
			   "hzg_wminus"                 ,  //"hzg_wminus"                      
			   "hzg_wplus"                  ,  //"hzg_wplus"                       
			   "hzg_zh"                     ,  //"hzg_zh"                          
			   "htautau_gluglu"             ,  //"htautau_gluglu"                  
			   "zetau"                      ,  //"zetau"                           
			   "zmutau"                     ,  //"zmutau"                          
			   "zemu"                       ,  //"zemu"                            
			   "hetau"                      ,  //"hetau"                           
			   "hmutau"                     ,  //"hmutau"                          
			   "hemu"                       ,  //"hemu"                            
			   "muon_2016B"	                ,  //"muon_2016B_v2"               
			   "muon_2016C"	                ,  //"muon_2016C"                  
			   "muon_2016D"	                ,  //"muon_2016D"                  
			   "muon_2016E"	                ,  //"muon_2016E"                  
			   "muon_2016F"	                ,  //"muon_2016F"                  
			   "muon_2016G"	                ,  //"muon_2016G"                  
			   "muon_2016H"	                ,  //"muon_2016H_v2"               
			   "electron_2016B"	        ,  //"electron_2016B_v2"           
			   "electron_2016C"	        ,  //"electron_2016C"              
			   "electron_2016D"	        ,  //"electron_2016D"              
			   "electron_2016E"	        ,  //"electron_2016E"              
			   "electron_2016F"	        ,  //"electron_2016F"              
			   "electron_2016G"	        ,  //"electron_2016G"              
			   "electron_2016H"                //"electron_2016H_v2"		   
  };
  vector<TString> folders = {"emu"}; //{"mutau", "etau", "emu"};
  status = initialize(); //initialize the MVAs
  if(status) return status;
  
  TStopwatch* timer = new TStopwatch();
  for(unsigned f_i = 0; f_i < files.size(); ++f_i) {
    auto file = files[f_i];
    auto name = names[f_i];
    for(auto folder : folders) {
      TString file_name = (grid_path + "output_") + (file + ".root");
      int stat = make_new_tree(file_name, folder, "bltTree_"+name);
      if(stat) cout << "file " << file.Data() << " folder " << folder.Data()
		    << " returned status " << stat << endl;
      status += stat;
    }
  }
  //report the time spent histogramming
  Double_t cpuTime = timer->CpuTime();
  Double_t realTime = timer->RealTime();
  printf("Processing time: %7.2fs CPU time %7.2fs Wall time\n",cpuTime,realTime);
  if(realTime > 600. ) printf("Processing time: %7.2fmin CPU time %7.2fmin Wall time\n",cpuTime/60.,realTime/60.);
  return status;
}
