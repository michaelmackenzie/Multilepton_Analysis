/**
   Script to clone BLT ZTauTau Analyzer trees and add MVA scores to the trees
 **/


namespace {
  //Tree Variables
  Tree_t fTreeVars;
  // enum {kMaxMVAs = 80};
  //Define relevant fields
  Int_t year_;
  TString folder_;
  TStopwatch* timer = new TStopwatch();
  Bool_t fDoAllYearsMVA = true; //use full Run-II trained MVA
  TMVA::Reader* mva[kMaxMVAs]; //read and apply mva weight files
  vector<TString> fMvaNames = { //mva names for getting weights
    "mutau_BDT_MM_8.higgs","mutau_BDT_MM_8.Z0", //0 - 9: total mvas
    "etau_BDT_MM_8.higgs","etau_BDT_MM_8.Z0",
    "emu_BDT_MM_8.higgs","emu_BDT_MM_8.Z0",
    "mutau_e_BDT_MM_8.higgs","mutau_e_BDT_MM_8.Z0",
    "etau_mu_BDT_MM_8.higgs","etau_mu_BDT_MM_8.Z0"/*,
    "mutau_TMlpANN_8.higgs","mutau_TMlpANN_8.Z0", //10 - 19: alternate mvas
    "etau_TMlpANN_8.higgs","etau_TMlpANN_8.Z0",
    "emu_BDT_68.higgs","emu_BDT_68.Z0",
    "mutau_e_TMlpANN_8.higgs","mutau_e_TMlpANN_8.Z0",
    "etau_mu_TMlpANN_8.higgs","etau_mu_TMlpANN_8.Z0"*/
  };
  Int_t   fIsJetBinnedMVAs[kMaxMVAs]; //storing number of jets for MVA, < 0 if not binned
  Float_t fMvaOutputs[kMaxMVAs];
  Int_t   fTrkQualVersion = TrkQualInit::Default; //for updating which variables are used
  TBranch* fMvaBranches[kMaxMVAs]; //mva branches in the new ttree
  float met; //standalone to not update with the correction in new tree
  float metPhi;
  float metCorr;
  float metCorrPhi;

  TLorentzVector* leptonOneP4;
  TLorentzVector* leptonTwoP4;
  TLorentzVector* jetP4;
  UInt_t nElectrons;
  UInt_t nMuons;
  UInt_t nTaus;
  UChar_t tauDeepAntiEle             ;
  UChar_t tauDeepAntiMu              ;
  UChar_t tauDeepAntiJet             ;
  Int_t   tauGenFlavor               ;
  UInt_t njets;
  UInt_t nBJets                      ;
  UInt_t nBJetsM                     ;
  UInt_t nBJetsL                     ;
  UInt_t nBJets20                    ;
  UInt_t nBJets20M                   ;
  UInt_t nBJets20L                   ;
  Long64_t fentry;
  Long64_t fMaxEntries = 0;
  float htPhi;

  bool debug = false;
  TH1F* hDebug = 0; //if debugging, make a histograms of scores instead

  struct datacard_t {
    bool process_;
    TString fname_;
    Long64_t iStart_;
    bool isData_;
    bool skipJetTaus_;
    datacard_t(bool process, TString fname, Long64_t iStart = 0) :
      process_(process), fname_(fname), iStart_(iStart), isData_(false), skipJetTaus_(false) {}
  };
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
    // if(fMvaNames[mva_i].Contains("_18") || //0 jet
    //    fMvaNames[mva_i].Contains("_48") ||
    //    fMvaNames[mva_i].Contains("_78"))
    //   isJetBinned = 0;
    // else if(fMvaNames[mva_i].Contains("_19") || //1 jet
    //      fMvaNames[mva_i].Contains("_49") ||
    //      fMvaNames[mva_i].Contains("_79"))
    //   isJetBinned = 1;
    // else if(fMvaNames[mva_i].Contains("_20") || //>1 jet
    //      fMvaNames[mva_i].Contains("_50") ||
    //      fMvaNames[mva_i].Contains("_80"))
    //   isJetBinned = 2;

    fIsJetBinnedMVAs[mva_i] = isJetBinned; //store for checking when filling

    printf("Using selection %s\n", selection.Data());
    //use a tool to initialize the MVA variables and spectators
    TrkQualInit trkQualInit(fTrkQualVersion, isJetBinned);
    trkQualInit.InitializeVariables(*(mva[mva_i]), selection, fTreeVars);

    TString year_string = "";
    if(fDoAllYearsMVA) year_string = "2016_2017_2018";
    else year_string += year_;

    //Initialize MVA weight file
    const char* f = Form("weights/%s.%s.weights.xml",fMvaNames[mva_i].Data(), year_string.Data());
    mva[mva_i]->BookMVA(fMvaNames[mva_i].Data(),f);
    printf("Booked MVA %s with selection %s\n", fMvaNames[mva_i].Data(), selection.Data());
  }
  return 0;
}

//selections: 1 = mutau, 2 = etau, 5 = emu, 9 = mumu, 18 = ee
Int_t initialize_tree_vars(int selection) {
  //update MET with correction
  TVector3 missing(met*cos(metPhi), met*sin(metPhi), 0.);
  TVector3 missingCorr(metCorr*cos(metCorrPhi), metCorr*sin(metCorrPhi), 0.);
  missing = missing + missingCorr;
  //updated MET variables
  fTreeVars.met = missing.Mag();
  metPhi = missing.Phi();

  // if(nBJets  > nBJetsM) nBJetsM = nBJets;
  // if(nBJetsM > nBJetsL) nBJetsL = nBJetsM;
  // if(nBJets20   > nBJets20M ) nBJets20M = nBJets20;
  // if(nBJets20M  > nBJets20L ) nBJets20L = nBJets20M;

  fTreeVars.leponept  = leptonOneP4->Pt();
  fTreeVars.leptwopt  = leptonTwoP4->Pt();
  // fTreeVars.leponeidone   = 0.;
  // fTreeVars.leponeidtwo   = 0.;
  // fTreeVars.leponeidthree = 0.;
  // if(selection < 4) {//tau selection
  //   fTreeVars.leptwoidone   = tauDeepAntiEle;
  //   fTreeVars.leptwoidtwo   = tauDeepAntiMu;
  //   fTreeVars.leptwoidthree = tauDeepAntiJet;
  // } else { //no other ids for now
  //   fTreeVars.leptwoidone   = 0.;
  //   fTreeVars.leptwoidtwo   = 0.;
  //   fTreeVars.leptwoidthree = 0.;
  // }

  TLorentzVector lep = *leptonOneP4 + *leptonTwoP4;
  fTreeVars.leppt  = lep.Pt();
  fTreeVars.lepm   = lep.M();
  fTreeVars.lepptoverm    = fTreeVars.leppt   /fTreeVars.lepm;
  fTreeVars.leponeptoverm = fTreeVars.leponept/fTreeVars.lepm;
  fTreeVars.leptwoptoverm = fTreeVars.leptwopt/fTreeVars.lepm;
  // fTreeVars.lepeta = lep.Eta();
  // fTreeVars.lepdeltar   = leptonOneP4->DeltaR(*leptonTwoP4);
  fTreeVars.lepdeltaphi = abs(leptonOneP4->DeltaPhi(*leptonTwoP4));
  // fTreeVars.lepdeltaeta = abs(leptonOneP4->Eta() - leptonTwoP4->Eta());

  //phi differences
  // fTreeVars.htdeltaphi = abs(lep.Phi() - htPhi);
  // if(fTreeVars.htdeltaphi > M_PI)
  //   fTreeVars.htdeltaphi = abs(2.*M_PI - fTreeVars.htdeltaphi);
  // fTreeVars.metdeltaphi = abs(lep.Phi() - metPhi);
  // if(fTreeVars.metdeltaphi > M_PI)
  //   fTreeVars.metdeltaphi = abs(2.*M_PI - fTreeVars.metdeltaphi);
  // fTreeVars.leponedeltaphi = abs(leptonOneP4->DeltaPhi(lep));
  // fTreeVars.leptwodeltaphi = abs(leptonTwoP4->DeltaPhi(lep));
  fTreeVars.onemetdeltaphi = abs(leptonOneP4->Phi() - metPhi);
  if(fTreeVars.onemetdeltaphi > M_PI)
    fTreeVars.onemetdeltaphi = abs(2.*M_PI - fTreeVars.onemetdeltaphi);
  fTreeVars.twometdeltaphi = abs(leptonTwoP4->Phi() - metPhi);
  if(fTreeVars.twometdeltaphi > M_PI)
    fTreeVars.twometdeltaphi = abs(2.*M_PI - fTreeVars.twometdeltaphi);


  fTreeVars.mtone = sqrt(2.*fTreeVars.met*fTreeVars.leponept*(1.-cos(leptonOneP4->Phi() - metPhi)));
  fTreeVars.mttwo = sqrt(2.*fTreeVars.met*fTreeVars.leptwopt*(1.-cos(leptonTwoP4->Phi() - metPhi)));
  fTreeVars.mtoneoverm = fTreeVars.mtone / fTreeVars.lepm;
  fTreeVars.mttwooverm = fTreeVars.mttwo / fTreeVars.lepm;

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
  double pnuest    = max(1.e-8,lp2*missing/lp2.Mag()); //inv pT along tau = tau pt unit vector dot missing
  double pnuesttwo = max(1.e-8,lp1*missing/lp1.Mag()); //inv pT along other lepton (for emu case with tau decay)
  fTreeVars.ptauvisfrac  = lp2.Mag() / (lp2.Mag() + pnuest);
  fTreeVars.mestimate    = fTreeVars.lepm/sqrt(fTreeVars.ptauvisfrac);
  fTreeVars.mestimatetwo = fTreeVars.lepm/sqrt(lp1.Mag() / (lp1.Mag() + pnuesttwo));

  double hmass(125.), zmass(91.2), tmass(1.78), lepdot(2.*((*leptonOneP4)*(*leptonTwoP4)));
  //delta alpha 1 = (m_boson^2 - m_tau^2) / (p(l1)\cdot p(l2))
  //delta alpha 2 = pT(l1) / pT(l2) (l1 = tau)
  //delta alpha 3 = pT(l2) / pT(l1) (l2 = tau)
  fTreeVars.alphaz1 = (zmass*zmass-tmass*tmass)/(lepdot);
  fTreeVars.alphah1 = (hmass*hmass-tmass*tmass)/(lepdot);
  fTreeVars.alpha2 = leptonTwoP4->Pt()/leptonOneP4->Pt(); //for lep 1 = tau, lep 2 = non-tau
  fTreeVars.alpha3 = leptonOneP4->Pt()/leptonTwoP4->Pt(); //for lep 2 = non-tau, lep 1 = tau
  fTreeVars.deltaalphaz1 = fTreeVars.alphaz1 - fTreeVars.alpha2;
  fTreeVars.deltaalphaz2 = fTreeVars.alphaz1 - fTreeVars.alpha3;
  fTreeVars.deltaalphah1 = fTreeVars.alphah1 - fTreeVars.alpha2;
  fTreeVars.deltaalphah2 = fTreeVars.alphah1 - fTreeVars.alpha3;
  //mass from delta alpha equation: m_boson = sqrt(m_tau^2 + pT(lep)/pT(tau) * p(l1) \cdot p(l2))
  fTreeVars.deltaalpham1 = std::sqrt(tmass*tmass + fTreeVars.alpha2 * lepdot); //lep 1 = tau
  fTreeVars.deltaalpham2 = std::sqrt(tmass*tmass + fTreeVars.alpha3 * lepdot); //lep 2 = tau

  fTreeVars.jetpt = jetP4->Pt();
  fTreeVars.njets = njets;
  // fTreeVars.nbjets = nBJets;
  // fTreeVars.nbjetsm = nBJetsM;
  // fTreeVars.nbjetsl = nBJetsL;
  //don't actually care about these, just for MVA spectators
  fTreeVars.eventweight = 1.;
  fTreeVars.fulleventweight = 1.;
  fTreeVars.fulleventweightlum = 1.;
  fTreeVars.eventcategory = 1;
  fTreeVars.train = 1;

  TString selecName = "";
  if(selection == 1)      selecName = "mutau";
  else if(selection == 2) selecName = "etau";
  else if(selection == 5) selecName = "emu";
  else if(selection == 9) selecName = "mumu";
  else if(selection == 18)selecName = "ee";
  else {                  selecName = "unknown"; cout << "---Warning! Entry " << fentry << " has unknown selection "
                                                      << selection << "!\n nMuons = " << nMuons << " nElectrons = "
                                                      << nElectrons << " nTaus = " << nTaus << endl;}

  for(unsigned i = 0; i < fMvaNames.size(); ++i) {
    if((fMvaBranches[i] || (debug&&mva[i])) && //branch exists or debug mode
       ((fMvaNames[i].Contains(selecName.Data()) && !fMvaNames[i].Contains("tau_e") && !fMvaNames[i].Contains("tau_mu")) || //is this selection, reject leptonic tau not in emu
        (selecName == "emu" && (fMvaNames[i].Contains("_e") || fMvaNames[i].Contains("_mu"))) || //or leptonic tau category
        ((selecName == "mumu" || selecName == "ee") && fMvaNames[i].Contains("emu"))) && //or is ee/mumu and compare to emu
       (fIsJetBinnedMVAs[i] < 0 || fIsJetBinnedMVAs[i] == min((int) fTreeVars.njets,2))) //and either not jet binned or right number of jets
      fMvaOutputs[i] = mva[i]->EvaluateMVA(fMvaNames[i].Data());
    else
      fMvaOutputs[i] = -2.;
    if(debug && fentry % 10000 == 0)
      cout << "entry " << fentry << ": MVA " << i << ": score = " << fMvaOutputs[i] << endl;
    if(fMvaOutputs[i] < -100.)
      cout << "Error value returned for MVA " << fMvaNames[i].Data()
           << " evaluation, Entry = " << fentry
           << " lepmestimate = " << fTreeVars.mestimate << " lepmestimatetwo = "
           << fTreeVars.mestimatetwo << endl;
    if(debug) hDebug->Fill(fMvaOutputs[i]);
  }
  return 0;
}

//read only information needed by MVAs, for selections,  or for debugging
Int_t set_addresses(TTree* fChain) {
  fChain->SetBranchStatus("*", 0); //turn off all branches
  fChain->SetBranchStatus("leptonOneP4"         , 1); fChain->SetBranchAddress("leptonOneP4"         , &leptonOneP4          );
  fChain->SetBranchStatus("leptonTwoP4"         , 1); fChain->SetBranchAddress("leptonTwoP4"         , &leptonTwoP4          );
  fChain->SetBranchStatus("jetP4"               , 1); fChain->SetBranchAddress("jetP4"               , &jetP4                );
  // fChain->SetBranchStatus("nMuons"              , 1); fChain->SetBranchAddress("nMuons"              , &nMuons               );
  // fChain->SetBranchStatus("nElectrons"          , 1); fChain->SetBranchAddress("nElectrons"          , &nElectrons           );
  // fChain->SetBranchStatus("nTaus"               , 1); fChain->SetBranchAddress("nTaus"               , &nTaus                );
  // fChain->SetBranchStatus("nPhotons"            , 1); fChain->SetBranchAddress("nPhotons"            , &fTreeVars.nphotons   );
  fChain->SetBranchStatus("nJets"               , 1); fChain->SetBranchAddress("nJets"               , &njets                );
  // fChain->SetBranchStatus("nFwdJets"            , 1); fChain->SetBranchAddress("nFwdJets"            , &nfwdjets             );
  // fChain->SetBranchStatus("nBJets"              , 1); fChain->SetBranchAddress("nBJets"              , &nBJets               );
  // fChain->SetBranchStatus("nBJetsM"             , 1); fChain->SetBranchAddress("nBJetsM"             , &nBJetsM              );
  // fChain->SetBranchStatus("nBJetsL"             , 1); fChain->SetBranchAddress("nBJetsL"             , &nBJetsL              );
  // fChain->SetBranchStatus("nBJets20"            , 1); fChain->SetBranchAddress("nBJets20"            , &nBJets20             );
  // fChain->SetBranchStatus("nBJets20M"           , 1); fChain->SetBranchAddress("nBJets20M"           , &nBJets20M            );
  // fChain->SetBranchStatus("nBJets20L"           , 1); fChain->SetBranchAddress("nBJets20L"           , &nBJets20L            );
  // fChain->SetBranchStatus("puppMETC"            , 1); fChain->SetBranchAddress("puppMETC"            , &met                  );
  // fChain->SetBranchStatus("puppMETCphi"         , 1); fChain->SetBranchAddress("puppMETCphi"         , &metPhi               );
  fChain->SetBranchStatus("met"                 , 1); fChain->SetBranchAddress("met"                 , &met                  );
  fChain->SetBranchStatus("metPhi"              , 1); fChain->SetBranchAddress("metPhi"              , &metPhi               );
  fChain->SetBranchStatus("metCorr"             , 1); fChain->SetBranchAddress("metCorr"             , &metCorr              );
  fChain->SetBranchStatus("metCorrPhi"          , 1); fChain->SetBranchAddress("metCorrPhi"          , &metCorrPhi           );
  // fChain->SetBranchStatus("ht"                  , 1); fChain->SetBranchAddress("ht"                  , &fTreeVars.ht         );
  // fChain->SetBranchStatus("htPhi"               , 1); fChain->SetBranchAddress("htPhi"               , &htPhi                );
  // fChain->SetBranchStatus("tauDeepAntiEle"      , 1); fChain->SetBranchAddress("tauDeepAntiEle"      , &tauDeepAntiEle       );
  // fChain->SetBranchStatus("tauDeepAntiMu"       , 1); fChain->SetBranchAddress("tauDeepAntiMu"       , &tauDeepAntiMu        );
  // fChain->SetBranchStatus("tauDeepAntiJet"      , 1); fChain->SetBranchAddress("tauDeepAntiJet"      , &tauDeepAntiJet       );
  fChain->SetBranchStatus("tauGenFlavor"        , 1); fChain->SetBranchAddress("tauGenFlavor"        , &tauGenFlavor         );


  //add new branches for MVA outputs
  for(unsigned mva_i = 0; mva_i < fMvaNames.size(); ++mva_i)  fMvaBranches[mva_i] = 0; //set to 0 initially
  int nfound = 0;
  for(unsigned mva_i = 0; mva_i < fMvaNames.size(); ++mva_i)  {
    // //add branch if doesn't already exist unless
    ++nfound;
    if(!fChain->GetBranch(Form("mva%i",mva_i))) {
      if(!debug) fMvaBranches[mva_i] = fChain->Branch(Form("mva%i",mva_i), &(fMvaOutputs[mva_i]));
    } else { //update branch if it exists
      if(!debug) {
        fChain->SetBranchStatus(Form("mva%i", mva_i), 1);
        fChain->SetBranchAddress(Form("mva%i", mva_i), &(fMvaOutputs[mva_i]));
        fMvaBranches[mva_i] = fChain->GetBranch(Form("mva%i",mva_i));
      }
    }
  }
  return nfound;
}

//if there are I/O errors, may be most useful to update the tree in parts
Int_t make_new_tree_inparts(TString path, TString path_in_file, TString tree_name, Long64_t iStart, Long64_t iStep = 1e6, bool isData = false, bool skipJetTaus = false) {
  cout << "Given file path " << path.Data() << " with folder " << path_in_file.Data()
       << " and tree name " << tree_name.Data() << endl
       << "Will process events " << iStart << " through " << iStart+iStep-1 << endl;

  // get the file and tree we're adding to
  TFile* file = TFile::Open(path.Data(), ((debug) ? "READ" : "UPDATE"));
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
  for(Long64_t entry = iStart; entry < min(nentries, iStart+iStep); ++entry) {
    if(entry % 50000 == 0)
      printf("Processing entry %10lld (%5.1f%%)...\n", entry, entry*100./nentries);
    if(fMaxEntries > 0 && entry-iStart >= fMaxEntries) {
      cout << "Hit maximum entries, " << fMaxEntries << ", exiting!\n";
      break;
    }
    fentry = entry;
    Int_t tree_status = tree->GetEntry(entry, 0);
    bool mutau = folder_ == "mutau";
    bool etau  = folder_ == "etau";
    bool emu   = folder_ == "emu";
    bool mumu  = folder_ == "mumu";
    bool ee    = folder_ == "ee";
    if((mutau || etau) && isData && skipJetTaus && tauGenFlavor > 15) {
      for(unsigned i = 0; i < fMvaNames.size(); ++i) {
        if(fMvaBranches[i]) {
          fMvaOutputs[i] = -2.;
          fMvaBranches[i]->Fill();
        }
      }
    } else {
      int selection = (mutau +
                       2*etau +
                       5*emu +
                       9*mumu +
                       18*ee);
      initialize_tree_vars(selection);
      for(unsigned mva_i = 0; mva_i < fMvaNames.size(); ++mva_i) {
        if(fMvaBranches[mva_i]) {
          fMvaBranches[mva_i]->Fill();
        }
      }
    }
  }
  cout << "Finished processing, writing back the updated tree!" << endl;
  fdir->cd();
  tree->Write();
  fdir->Write();
  file->Close();
  cout << "Finished writing back the updated tree!" << endl;
  delete file;
  if(nentries > iStart+iStep) return make_new_tree_inparts(path, path_in_file, tree_name, iStart+iStep, iStep);
  else return 0;
}

Int_t make_new_tree(TString path, TString path_in_file, TString tree_name, bool isData = false, bool skipJetTaus = false) {
  cout << "Given file path " << path.Data() << " with folder " << path_in_file.Data()
       << " and tree name " << tree_name.Data() << endl;

  // get the file and tree we're adding to
  TFile* file = TFile::Open(path.Data(), ((debug) ? "READ" : "UPDATE"));
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

  if(debug) {
    hDebug = new TH1F("hDebug", "hDebug", 500, -3., 2.);
  }

  //run through tree and evaluate mvas for each
  Long64_t nentries = tree->GetEntriesFast();
  cout << "Processing " << tree->GetName() << " tree with " << nentries << " entries" << endl;
  for(Long64_t entry = 0; entry < nentries; ++entry) {
    if(entry % 50000 == 0)
      printf("Processing entry %10lld (%5.1f%%)...\n", entry, entry*100./nentries);
    if(fMaxEntries > 0 && entry >= fMaxEntries) {
      cout << "Hit maximum entries, " << fMaxEntries << ", exiting!\n";
      break;
    }
    fentry = entry;
    Int_t tree_status = tree->GetEntry(entry, 0);
    if(debug && entry%10000 == 0)
      cout << "Status getting entry: " << tree_status << endl;
    bool mutau = folder_ == "mutau";
    bool etau  = folder_ == "etau";
    bool emu   = folder_ == "emu";
    bool mumu  = folder_ == "mumu";
    bool ee    = folder_ == "ee";
    if((mutau || etau) && !isData && skipJetTaus && tauGenFlavor > 15) {
      for(unsigned i = 0; i < fMvaNames.size(); ++i) {
        if(fMvaBranches[i]) {
          fMvaOutputs[i] = -2.;
          fMvaBranches[i]->Fill();
        }
      }
    } else {
      int selection = (mutau +
                       2*etau +
                       5*emu +
                       9*mumu +
                       18*ee);
      if(debug && entry%10000 == 0)
        cout << "Using selection " << selection << endl;
      initialize_tree_vars(selection);
      for(unsigned mva_i = 0; mva_i < fMvaNames.size(); ++mva_i) {
        if(!debug&&fMvaBranches[mva_i]) {
          fMvaBranches[mva_i]->Fill();
        }
      }
    }
  }
  if(debug) hDebug->Draw();
  else {
    cout << "Finished processing, writing back the updated tree!" << endl;
    fdir->cd();
    tree->Write();
    fdir->Write();
    file->Close();
    cout << "Finished writing back the updated tree!" << endl;
    delete file;
  }
  return 0;
}

Int_t initialize() {
  Int_t status = book_mvas();
  return status;
}


//Parameters: year, true/false to process small parts of the file at a time, true/false to copy the file locally to update,
//            and true/false to use the trees in the output directory instead of clean trees
Int_t process_standard_nano_trees(int year = 2016, bool doInParts = false, bool copyLocal = true, bool update = false) {
  int status = 0;
  year_ = year;
  TString grid_path = "root://cmseos.fnal.gov///store/user/mmackenz/ztautau_nanoaod_test_trees/";
  TString grid_out = "root://cmseos.fnal.gov///store/user/mmackenz/ztautau_nanoaod_test_trees/";
  // TString grid_out = "root://cmseos.fnal.gov///store/user/mmackenz/ztautau_nanoaod_trees/";
  if(copyLocal&&!update) grid_path = "root://cmseos.fnal.gov///store/user/mmackenz/ztautau_nanoaod_trees_nomva/";
  TString name = "clfv_";
  name += year;
  name += "_";
  TString ext  = ".tree";
  vector<datacard_t> cards;
  cards.push_back(datacard_t(true                 , name+"ttbarToSemiLeptonic"));
  cards.push_back(datacard_t(true                 , name+"ttbarToHadronic"));
  cards.push_back(datacard_t(true                 , name+"ttbarlnu"));
  cards.push_back(datacard_t(true                 , name+"DY50"));
  cards.push_back(datacard_t(true  && year != 2018, name+"DY50-ext"));
  cards.push_back(datacard_t(true                 , name+"SingleAntiToptW"));
  cards.push_back(datacard_t(true                 , name+"SingleToptW"));
  cards.push_back(datacard_t(true                 , name+"Wlnu"));
  cards.push_back(datacard_t(true  && year != 2018, name+"Wlnu-ext"));
  cards.push_back(datacard_t(true                 , name+"WW"));
  cards.push_back(datacard_t(true                 , name+"WZ"));
  cards.push_back(datacard_t(true                 , name+"ZETau"));
  cards.push_back(datacard_t(true                 , name+"ZMuTau"));
  cards.push_back(datacard_t(true                 , name+"ZEMu"));
  cards.push_back(datacard_t(true                 , name+"HETau"));
  cards.push_back(datacard_t(true                 , name+"HMuTau"));
  cards.push_back(datacard_t(true                 , name+"HEMu"));
  cards.push_back(datacard_t(true                 , name+"ZZ"));
  cards.push_back(datacard_t(true                 , name+"WWW"));
  cards.push_back(datacard_t(true                 , name+"QCDDoubleEMEnrich30to40"));
  cards.push_back(datacard_t(true                 , name+"QCDDoubleEMEnrich30toInf"));
  cards.push_back(datacard_t(true                 , name+"QCDDoubleEMEnrich40toInf"));
  cards.push_back(datacard_t(true                 , name+"SingleMu"));
  cards.push_back(datacard_t(true                 , name+"SingleEle"));

  vector<TString> folders = {"ee", "mumu", "mutau", "etau", "emu"};
  status = initialize(); //initialize the MVAs
  if(status) return status;

  TStopwatch* timer = new TStopwatch();
  for(datacard_t& card : cards) {
    if(!card.process_) continue;
    TString file_name = grid_path + card.fname_ + ext;
    if(copyLocal) {
      cout << "Copying " << file_name.Data() << " locally...\n";
      gSystem->Exec(Form("time xrdcp -f %s ./%s", file_name.Data(), (card.fname_+ext).Data()));
      file_name = card.fname_ + ext;
    }
    card.skipJetTaus_ = true;
    if(card.fname_.Contains("SingleMu") || card.fname_.Contains("SingleEle")) card.isData_ = true;
    int stat = 0; //status for this card
    for(auto folder : folders) {
      folder_ = folder;
      int stat_f = ((doInParts) ? make_new_tree_inparts(file_name, folder, card.fname_, card.iStart_, card.isData_, card.skipJetTaus_) :
                    make_new_tree(file_name, folder, card.fname_, card.isData_, card.skipJetTaus_));
      if(stat_f) cout << "file " << card.fname_.Data() << ": folder " << folder.Data()
                    << " returned status " << stat << endl;
      stat += stat_f;
    }
    status += stat;
    if(stat) { cout << "Card status = " << stat << ", continuing!\n"; continue;}
    if(copyLocal && !debug) {
      cout << "Copying " << file_name.Data() << " to " << (grid_out+card.fname_+ext).Data() << "...\n";
      gSystem->Exec(Form("time xrdcp -f ./%s %s; rm %s;", file_name.Data(), (grid_out+card.fname_+ext).Data(), file_name.Data()));
    } else if(copyLocal && debug) {
      cout << "Not copying or deleting local file  " << file_name.Data() << " due to debug mode...\n";
    }

  }
  //report the time spent histogramming
  Double_t cpuTime = timer->CpuTime();
  Double_t realTime = timer->RealTime();
  printf("Processing time: %7.2fs CPU time %7.2fs Wall time\n",cpuTime,realTime);
  if(realTime > 600. ) printf("Processing time: %7.2fmin CPU time %7.2fmin Wall time\n",cpuTime/60.,realTime/60.);
  return status;
}
