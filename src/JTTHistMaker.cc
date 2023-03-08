#define JTTHISTMAKER_CXX

#include "interface/JTTHistMaker.hh"
#include <TStyle.h>
using namespace CLFV;


//--------------------------------------------------------------------------------------------------------------
JTTHistMaker::JTTHistMaker(int seed, TTree * /*tree*/) : HistMaker(seed) {
  fDoSystematics = 0;
  fWriteTrees = 0;
}

//--------------------------------------------------------------------------------------------------------------
void JTTHistMaker::Begin(TTree * /*tree*/)
{
  HistMaker::Begin(nullptr);
}


//--------------------------------------------------------------------------------------------------------------
void JTTHistMaker::InitHistogramFlags() {
  for(int i = 0; i < fn; ++i) {
    fEventSets[i] = 0;
    fSysSets  [i] = 0;
    fTreeSets [i] = 0;
    fSetFills [i] = 0;
  }

  //Event Sets
  const bool mutau = fSelection == "" || fSelection == "mutau";
  const bool etau  = fSelection == "" || fSelection == "etau" ;
  const bool emu   = fSelection == "" || fSelection == "emu"  ;
  const bool mumu  = fSelection == "" || fSelection == "mumu" ;
  const bool ee    = fSelection == "" || fSelection == "ee"   ;

  if(mutau) {
    fEventSets [kMuTau + 8] = 1; // preselection
    // jet --> tau DRs
    fEventSets [kMuTau + 30] = 1; //QCD
    fEventSets [kMuTau + 31] = 1; //W+Jets
    fEventSets [kMuTau + 32] = 1; //Top
    fEventSets [kMuTau + 34] = 1; //signal region but W+Jets j-->tau weights (no composition)
    fEventSets [kMuTau + 89] = 1; //W+Jets MC weights

    // jet --> tau DRs with MC taus
    fEventSets [kMuTau + 33] = 1; //Nominal selection without j-->tau weights, loose ID only
    fEventSets [kMuTau + 35] = 1; //Nominal selection
    fEventSets [kMuTau + 36] = 1; //QCD
    fEventSets [kMuTau + 37] = 1; //W+Jets
    fEventSets [kMuTau + 38] = 1; //Top
    fEventSets [kMuTau + 88] = 1; //W+Jets MC weights
    fEventSets [kMuTau + 92] = 1; //Loose nominal selection
    fEventSets [kMuTau + 94] = 1; //High isolation selection

    // jet --> tau MC taus in nominal selection, different process weights
    fEventSets [kMuTau + 81] = 1; //W+Jets
    fEventSets [kMuTau + 82] = 1; //Top
    fEventSets [kMuTau + 86] = 1; //W+Jets, no fake MC
    fEventSets [kMuTau + 87] = 1; //Top, no fake MC
    fEventSets [kMuTau + 93] = 1; //Loose nominal selection, no fake MC
    fEventSets [kMuTau + 95] = 1; //High isolation selection, no fake MC

    fEventSets [kMuTau + 42] = 1; //fake MC taus in the nominal selection

  }
  if(etau) {
    fEventSets [kETau + 8] = 1; // preselection

    // jet --> tau DRs
    fEventSets [kETau + 30] = 1; //QCD
    fEventSets [kETau + 31] = 1; //W+Jets
    fEventSets [kETau + 32] = 1; //Top
    fEventSets [kETau + 34] = 1; //signal region but W+Jets j-->tau weights (no composition)
    fEventSets [kETau + 89] = 1; //W+Jets MC weights

    // jet --> tau DRs with MC taus
    fEventSets [kETau + 33] = 1; //Nominal selection without j-->tau weights, loose ID only
    fEventSets [kETau + 35] = 1; //Nominal selection
    fEventSets [kETau + 36] = 1; //QCD
    fEventSets [kETau + 37] = 1; //W+Jets
    fEventSets [kETau + 38] = 1; //Top
    fEventSets [kETau + 88] = 1; //W+Jets MC weights
    fEventSets [kETau + 92] = 1; //Loose nominal selection
    fEventSets [kETau + 94] = 1; //Loose nominal selection

    // jet --> tau MC taus in nominal selection, different process weights
    fEventSets [kETau + 81] = 1; //W+Jets
    fEventSets [kETau + 82] = 1; //Top
    fEventSets [kETau + 86] = 1; //W+Jets, no fake MC
    fEventSets [kETau + 87] = 1; //Top, no fake MC
    fEventSets [kETau + 93] = 1; //Loose nominal selection, no fake MC
    fEventSets [kETau + 95] = 1; //Loose nominal selection, no fake MC

    fEventSets [kETau + 42] = 1; //fake MC taus in the nominal selection
  }
  if(emu) {
    fEventSets [kEMu  + 8] = 1; // preselection
  }
  if(mumu) {
    fEventSets [kMuMu + 8] = 1; // preselection
  }
  if(ee) {
    fEventSets [kEE   + 8] = 1; // preselection
  }
}

//--------------------------------------------------------------------------------------------------------------
void JTTHistMaker::BookHistograms() {
  BookEventHistograms();
  BookLepHistograms();
}

//--------------------------------------------------------------------------------------------------------------
void JTTHistMaker::BookEventHistograms() {
  for(int i = 0; i < fQcdOffset; ++i) {
    if(fEventSets[i]) { //turn on all offset histogram sets
      fEventSets[i+fQcdOffset] = 1;
      fEventSets[i+fMisIDOffset] = 1;
      fEventSets[i+fQcdOffset+fMisIDOffset] = 1;
    }
  }
  for(int i = 0; i < fn; i++) {
    if(fEventSets[i] != 0) {
      char* dirname        = new char[20];
      sprintf(dirname,"event_%i",i);
      TDirectory* folder = fTopDir->mkdir(dirname);
      fDirectories[0*fn + i] = folder;
      folder->cd();
      fEventHist[i] = new EventHist_t;

      Utilities::BookH1F(fEventHist[i]->hEventWeight             , "eventweight"             , Form("%s: EventWeight"                 ,dirname), 100,   -1,   3, folder);
      Utilities::BookH1F(fEventHist[i]->hLogEventWeight          , "logeventweight"          , Form("%s: LogEventWeight"              ,dirname),  50,  -10,   1, folder);
      Utilities::BookH1D(fEventHist[i]->hGenWeight               , "genweight"               , Form("%s: GenWeight"                   ,dirname),   5, -2.5, 2.5, folder);

      Utilities::BookH1D(fEventHist[i]->hNMuons                  , "nmuons"                  , Form("%s: NMuons"                      ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNElectrons              , "nelectrons"              , Form("%s: NElectrons"                  ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNTaus                   , "ntaus"                   , Form("%s: NTaus"                       ,dirname),  10,    0,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hNPV[0]                  , "npv"                     , Form("%s: NPV"                         ,dirname),  50,    0, 100, folder);
      Utilities::BookH1D(fEventHist[i]->hMcEra                   , "mcera"                   , Form("%s: MCEra + 2*(year-2016)"       ,dirname),   8,    0,   8, folder);
      Utilities::BookH1D(fEventHist[i]->hNJets20[0]              , "njets20"                 , Form("%s: NJets20"                     ,dirname),  10,    0,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hJetPt[0]           , "jetpt"               , Form("%s: JetPt"               ,dirname), 100,    0, 200, folder);
      Utilities::BookH1D(fEventHist[i]->hNBJets20[0]             , "nbjets20"                , Form("%s: NBJets20"                    ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNBJets20L[0]            , "nbjets20l"               , Form("%s: NBJets20L"                   ,dirname),  10,    0,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hHt                 , "ht"                  , Form("%s: Ht"                  ,dirname), 100,    0, 200, folder);

      Utilities::BookH1D(fEventHist[i]->hNGenTaus                , "ngentaus"                , Form("%s: NGenTaus"                    ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNGenElectrons           , "ngenelectrons"           , Form("%s: NGenElectrons"               ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNGenMuons               , "ngenmuons"               , Form("%s: NGenMuons"                   ,dirname),  10,    0,  10, folder);

      Utilities::BookH1F(fEventHist[i]->hMet                 , "met"                 , Form("%s: Met"                     ,dirname)  , 100,  0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hMTOne               , "mtone"               , Form("%s: MTOne"                   ,dirname)  , 100, 0.,   150., folder);
      Utilities::BookH1F(fEventHist[i]->hMTTwo               , "mttwo"               , Form("%s: MTTwo"                   ,dirname)  , 100, 0.,   150., folder);
      Utilities::BookH1F(fEventHist[i]->hMTLep               , "mtlep"               , Form("%s: MTLep"                   ,dirname)  , 100, 0.,   150., folder);

      Utilities::BookH1F(fEventHist[i]->hLepPt[0], "leppt"         , Form("%s: Lepton Pt"      ,dirname)  , 100,   0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepM[0] , "lepm"          , Form("%s: Lepton M"       ,dirname)  , 280,  40, 180, folder);
      Utilities::BookH1F(fEventHist[i]->hLepMt   , "lepmt"         , Form("%s: Lepton Mt"      ,dirname)  , 100,   0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepEta  , "lepeta"        , Form("%s: Lepton Eta"     ,dirname)  ,  50,  -5,   5, folder);
      Utilities::BookH1F(fEventHist[i]->hLepMEstimate   , "lepmestimate"   , Form("%s: Estimate di-lepton mass"  ,dirname)  ,100,0.,  200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepMEstimateTwo, "lepmestimatetwo", Form("%s: Estimate di-lepton mass"  ,dirname)  ,100,0.,  200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepDeltaPhi[0], "lepdeltaphi"   , Form("%s: Lepton DeltaPhi",dirname), 50,   0,   5, folder);
      Utilities::BookH1F(fEventHist[i]->hLepDeltaEta   , "lepdeltaeta"   , Form("%s: Lepton DeltaEta",dirname), 50,   0,   5, folder);
      Utilities::BookH1F(fEventHist[i]->hLepDeltaR[0]  , "lepdeltar"     , Form("%s: Lepton DeltaR"  ,dirname), 50,   0,   5, folder);

      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[0]     , "deltaalpha0"      , Form("%s: Delta Alpha (Z) 0"   ,dirname),  80,  -5,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[1]     , "deltaalpha1"      , Form("%s: Delta Alpha (Z) 1"   ,dirname),  80,  -5,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[2]     , "deltaalpha2"      , Form("%s: Delta Alpha (H) 0"   ,dirname),  80,  -5,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[3]     , "deltaalpha3"      , Form("%s: Delta Alpha (H) 1"   ,dirname),  80,  -5,  10, folder);

      for(unsigned j = 0; j < fMVAConfig->names_.size(); ++j)  {
        Utilities::BookH1D(fEventHist[i]->hMVA[j][0], Form("mva%i",j)   , Form("%s: %s MVA" ,dirname, fMVAConfig->names_[j].Data()) ,
                           fMVAConfig->NBins(j), fMVAConfig->Bins(j).data(), folder);
        //high mva score binning to improve cdf making
        Utilities::BookH1F(fEventHist[i]->hMVA[j][1]  , Form("mva%i_1",j)   , Form("%s: %s MVA"         ,dirname, fMVAConfig->names_[j].Data()), 3000, -1.,  2., folder);
      }

      /////////////////////////////////////
      // j-->tau specific histograms

      Utilities::BookH1F(fEventHist[i]->hJetToTauWeight          , "jettotauweight"          , Form("%s: JetToTauWeight"              ,dirname),  40,    0,   2, folder);
      Utilities::BookH1F(fEventHist[i]->hJetToTauWeightCorr      , "jettotauweightcorr"      , Form("%s: JetToTauWeightCorr"          ,dirname),  50,    0,   5, folder);
      Utilities::BookH1F(fEventHist[i]->hJetToTauWeightBias      , "jettotauweightbias"      , Form("%s: JetToTauWeightBias"          ,dirname),  50,    0,   5, folder);

      Utilities::BookH1F(fEventHist[i]->hTauPt              , "taupt"               , Form("%s: TauPt"               ,dirname),  50,    0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hTauEta             , "taueta"              , Form("%s: TauEta"              ,dirname),  30, -2.5, 2.5, folder);
      Utilities::BookH1D(fEventHist[i]->hTauDecayMode[0]    , "taudecaymode"        , Form("%s: TauDecayMode"        ,dirname),  15,    0,  15, folder);
      Utilities::BookH1D(fEventHist[i]->hTauGenFlavor       , "taugenflavor"        , Form("%s: TauGenFlavor"        ,dirname),  30,    0,  30, folder);
      Utilities::BookH1D(fEventHist[i]->hTauDeepAntiEle     , "taudeepantiele"      , Form("%s: TauDeepAntiEle"      ,dirname),  30,    0,  30, folder);
      Utilities::BookH1D(fEventHist[i]->hTauDeepAntiMu      , "taudeepantimu"       , Form("%s: TauDeepAntiMu"       ,dirname),  30,    0,  30, folder);
      Utilities::BookH1D(fEventHist[i]->hTauDeepAntiJet     , "taudeepantijet"      , Form("%s: TauDeepAntiJet"      ,dirname),  30,    0,  30, folder);

      for(int ji = 0; ji < JetToTauComposition::kLast; ++ji) {
        Utilities::BookH1F(fEventHist[i]->hJetToTauComps[ji]     , Form("jettotaucomps_%i", ji), Form("%s: JetToTauComps %i"      ,dirname, ji),  50,    0,   2, folder);
        Utilities::BookH1F(fEventHist[i]->hJetToTauWts  [ji]     , Form("jettotauwts_%i"  , ji), Form("%s: JetToTauWts %i"        ,dirname, ji),  50,    0,   2, folder);
      }

      //j->tau measurement histograms
      if(fSelection == "" || fSelection == "mutau" || fSelection == "etau") {
        //variable width bins for eta vs pT
        const double tetabins[] = {0., 2.3}; //{ 0., 1.5, 2.3};
        const int ntetabins = sizeof(tetabins) / sizeof(*tetabins) - 1;
        const double tpbins[] = { 20., 22., 24., 26., 29.,
                                  33., 37., 42., 50., 60.,
                                  100.,
                                  200.};
        const int ntpbins = sizeof(tpbins) / sizeof(*tpbins) - 1;

        const int ndms = 4; //decay mode categories
        const int njetsmax = 2; //N(jets) categories: 0, 1, and >= 2 jets
        const int ncats = 3; //tau isolation categories: 0 = inclusive; 1 = anti-isolated; 2 = isolated
        for(int cat = 0; cat < ncats; ++cat) {
          for(int dm = 0; dm < ndms; ++dm) {
            for(int njets = 0; njets <= njetsmax; ++njets) {
              const char* hname = Form("faketau%ijet%idm_%i", njets, dm, cat);
              Utilities::BookH2F(fEventHist[i]->hFakeTauNJetDMPtEta[cat][njets][dm], hname, Form("%s: Fake tau Eta vs Pt" ,dirname),
                                 ntpbins, tpbins, ntetabins, tetabins, folder);
              const char* hmcname = Form("faketaumc%ijet%idm_%i", njets, dm, cat);
              Utilities::BookH2F(fEventHist[i]->hFakeTauMCNJetDMPtEta[cat][njets][dm], hmcname, Form("%s: Fake tau MC Eta vs Pt" ,dirname),
                                 ntpbins, tpbins, ntetabins, tetabins, folder);
            }
          }
        }

        Utilities::BookH1F(fEventHist[i]->hTausPt       , "tauspt"       , Form("%s: TausPt     "  ,dirname), 100,  0., 200., folder);
        Utilities::BookH1F(fEventHist[i]->hTausEta      , "tauseta"      , Form("%s: TausEta    "  ,dirname),  50, -2.5, 2.5, folder);
        Utilities::BookH1D(fEventHist[i]->hTausDM       , "tausdm"       , Form("%s: TausDM     "  ,dirname),  15,  0.,  15., folder);
        Utilities::BookH1D(fEventHist[i]->hTausAntiJet  , "tausantijet"  , Form("%s: TausAntiJet"  ,dirname),  30,  0.,  30., folder);
        Utilities::BookH1D(fEventHist[i]->hTausAntiEle  , "tausantiele"  , Form("%s: TausAntiEle"  ,dirname),  30,  0.,  30., folder);
        Utilities::BookH1D(fEventHist[i]->hTausAntiMu   , "tausantimu"   , Form("%s: TausAntiMu"   ,dirname),  30,  0.,  30., folder);
        Utilities::BookH1D(fEventHist[i]->hTausMVAAntiMu, "tausmvaantimu", Form("%s: TausMVAAntiMu",dirname),  30,  0.,  30., folder);
        Utilities::BookH1D(fEventHist[i]->hTausGenFlavor, "tausgenflavor", Form("%s: TausGenFlavor",dirname),  40,  0.,  40., folder);
        Utilities::BookH1F(fEventHist[i]->hTausDeltaR   , "tausdeltar"   , Form("%s: TausDeltaR"   ,dirname),  50,  0.,   5., folder);
        Utilities::BookH1F(fEventHist[i]->hFakeTausPt   , "faketauspt"   , Form("%s: TausPt     "  ,dirname), 100,  0., 200., folder);
        Utilities::BookH1F(fEventHist[i]->hFakeTausEta  , "faketauseta"  , Form("%s: TausEta    "  ,dirname),  50, -2.5, 2.5, folder);
        Utilities::BookH1D(fEventHist[i]->hFakeTausDM   , "faketausdm"   , Form("%s: TausDM     "  ,dirname),  15,  0.,  15., folder);

        // jet --> tau non-closure histograms
        const double jt_lepm_bins[] = {50., 70., 85., 120., 170.};
        const int    jt_nlepm_bins  = sizeof(jt_lepm_bins) / sizeof(*jt_lepm_bins) - 1;
        const double jt_dr_bins[]   = {0., 2., 2.5, 3., 3.5, 4., 5.};
        const int    jt_ndr_bins    = sizeof(jt_dr_bins) / sizeof(*jt_dr_bins) - 1;
        const double jt_mt_bins[]   = {0., 20., 35., 45., 55., 65., 75., 85., 150.};
        const int    jt_nmt_bins    = sizeof(jt_mt_bins) / sizeof(*jt_mt_bins) - 1;
        Utilities::BookH1F(fEventHist[i]->hJetTauDeltaR[0], "jettaudeltar0", Form("%s: JetTauDeltaR",dirname),    jt_ndr_bins,   jt_dr_bins, folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauDeltaR[1], "jettaudeltar1", Form("%s: JetTauDeltaR",dirname),    jt_ndr_bins,   jt_dr_bins, folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauDeltaR[2], "jettaudeltar2", Form("%s: JetTauDeltaR",dirname),    jt_ndr_bins,   jt_dr_bins, folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauLepM  [0], "jettaulepm0"  , Form("%s: JetTauLepM"  ,dirname),  jt_nlepm_bins, jt_lepm_bins, folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauLepM  [1], "jettaulepm1"  , Form("%s: JetTauLepM"  ,dirname),  jt_nlepm_bins, jt_lepm_bins, folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauLepM  [2], "jettaulepm2"  , Form("%s: JetTauLepM"  ,dirname),  jt_nlepm_bins, jt_lepm_bins, folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauMTLep [0], "jettaumtlep0" , Form("%s: JetTauMTLep" ,dirname),  10, 0.,150., folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauMTLep [1], "jettaumtlep1" , Form("%s: JetTauMTLep" ,dirname),  10, 0.,150., folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauMTLep [2], "jettaumtlep2" , Form("%s: JetTauMTLep" ,dirname),  10, 0.,150., folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauMTOne [0], "jettaumtone0" , Form("%s: JetTauMTOne" ,dirname),  jt_nmt_bins, jt_mt_bins, folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauMTOne [1], "jettaumtone1" , Form("%s: JetTauMTOne" ,dirname),  jt_nmt_bins, jt_mt_bins, folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauMTOne [2], "jettaumtone2" , Form("%s: JetTauMTOne" ,dirname),  jt_nmt_bins, jt_mt_bins, folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauMTTwo [0], "jettaumttwo0" , Form("%s: JetTauMTTwo" ,dirname),  jt_nmt_bins, jt_mt_bins, folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauMTTwo [1], "jettaumttwo1" , Form("%s: JetTauMTTwo" ,dirname),  jt_nmt_bins, jt_mt_bins, folder);
        Utilities::BookH1F(fEventHist[i]->hJetTauMTTwo [2], "jettaumttwo2" , Form("%s: JetTauMTTwo" ,dirname),  jt_nmt_bins, jt_mt_bins, folder);
      }

      delete dirname;
    }
  }
}

//--------------------------------------------------------------------------------------------------------------
void JTTHistMaker::BookLepHistograms() {
  for(int i = 0; i < fn; i++) {
    if(fEventSets[i] > 0) {
      char* dirname        = new char[20];
      sprintf(dirname,"lep_%i",i);
      auto folder = fTopDir->mkdir(dirname);
      fDirectories[2*fn + i] = folder;
      fDirectories[2*fn + i]->cd();
      fLepHist[i] = new LepHist_t;

      Utilities::BookH1F(fLepHist[i]->hOnePt[0]       , "onept"            , Form("%s: Pt"           ,dirname)  , 150,   0, 150, folder);
      Utilities::BookH1F(fLepHist[i]->hOneEta         , "oneeta"           , Form("%s: Eta"          ,dirname),  50, -2.5,  2.5, folder);
      Utilities::BookH1F(fLepHist[i]->hOneDXY         , "onedxy"           , Form("%s: DXY"          ,dirname),  50,    0,  0.1, folder);
      Utilities::BookH1F(fLepHist[i]->hOneDZ          , "onedz"            , Form("%s: DZ"           ,dirname),  50,    0,  0.1, folder);
      Utilities::BookH1F(fLepHist[i]->hOneRelIso      , "onereliso"        , Form("%s: Iso / Pt"     ,dirname),  50,    0,  0.5, folder);
      Utilities::BookH1D(fLepHist[i]->hOneTrigger     , "onetrigger"       , Form("%s: Trigger"      ,dirname),  10,    0,   10, folder);
      Utilities::BookH1F(fLepHist[i]->hOneMetDeltaPhi , "onemetdeltaphi"   , Form("%s: Met Delta Phi",dirname),  50,    0,    5, folder);

      Utilities::BookH1F(fLepHist[i]->hTwoPt[0]       , "twopt"            , Form("%s: Pt"          ,dirname)  , 150,   0, 150, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoEta         , "twoeta"           , Form("%s: Eta"         ,dirname),  50,-2.5,  2.5, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoDXY         , "twodxy"           , Form("%s: DXY"         ,dirname),  50,   0,  0.1, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoDZ          , "twodz"            , Form("%s: DZ"          ,dirname),  50,   0,  0.1, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoRelIso      , "tworeliso"        , Form("%s: Iso / Pt"    ,dirname),  50,   0,  0.5, folder);
      Utilities::BookH1D(fLepHist[i]->hTwoTrigger     , "twotrigger"       , Form("%s: Trigger"     ,dirname),  10,   0,   10, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoMetDeltaPhi , "twometdeltaphi"   , Form("%s: Met Delta Phi",dirname), 50,   0,    5, folder);

      Utilities::BookH1F(fLepHist[i]->hPtDiff         , "ptdiff"           , Form("%s: 1 pT - 2 pT"  ,dirname), 100,  -80,   80, folder);


      //j-->tau weight variations
      Utilities::BookH1F(fLepHist[i]->hOnePt[10], "onept10", Form("%s: Pt"      ,dirname)  , 150,   0, 150, folder);
      Utilities::BookH1F(fLepHist[i]->hOnePt[11], "onept11", Form("%s: Pt"      ,dirname)  , 150,   0, 150, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoPt[10], "twopt10", Form("%s: Pt"      ,dirname)  , 150,   0, 150, folder);
      Utilities::BookH1F(fLepHist[i]->hTwoPt[11], "twopt11", Form("%s: Pt"      ,dirname)  , 150,   0, 150, folder);

      //for correcting jet -> tau scale factors
      const double pts[] = {0.  , 20. , 25. , 33. , 40. ,
                            50. , 60. , 80. ,
                            200.};
      const int nbins_pt = sizeof(pts)/sizeof(*pts) - 1;

      const double rbins[] = {0.  , 1.  , 1.5 , 2.  , 2.5,
                              3.  , 3.5 ,
                              5.};
      const int nrbins = sizeof(rbins)/sizeof(*rbins) - 1;
      for(int dmregion = 0; dmregion < 5; ++dmregion) {
        TString name_r = "jettauoneptvsr";
        if(dmregion > 0) {name_r += "_"; name_r += dmregion;}
        Utilities::BookH2F(fLepHist[i]->hJetTauOnePtVsR[dmregion], name_r.Data() , Form("%s: Delta R Vs One Pt"   ,dirname)  , nbins_pt, pts, nrbins, rbins, folder);
        TString name = "jettauonept";
        if(dmregion > 0) {name += "_"; name += dmregion;}
        Utilities::BookH1F(fLepHist[i]->hJetTauOnePt[dmregion], name.Data() , Form("%s: One Pt"   ,dirname)  , nbins_pt, pts, folder);
      }
      Utilities::BookH1F(fLepHist[i]->hJetTauOneR  , "jettauoner" , Form("%s: Delta R" ,dirname)  , nrbins, rbins, folder);

      const double pts_qcd[] = {0.  , 20. , 25. , 32. , 40. ,
                                55. ,
                                200.};
      const int nbins_pt_qcd = sizeof(pts_qcd)/sizeof(*pts_qcd) - 1;
      Utilities::BookH1F(fLepHist[i]->hJetTauOnePtQCD[0], "jettauoneptqcd_0" , Form("%s: One Pt"   ,dirname)  , nbins_pt_qcd, pts_qcd, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauOnePtQCD[1], "jettauoneptqcd_1" , Form("%s: One Pt"   ,dirname)  , nbins_pt_qcd, pts_qcd, folder);

      Utilities::BookH1F(fLepHist[i]->hJetTauOneEta, "jettauoneeta", Form("%s: |Eta|"  ,dirname)  , 20,    0, 2.5, folder);
      const double metbins[] = {0., 0.5, 0.9, 1.2, 1.4, 1.6, 1.8, 2., 2.2, 2.4, 2.6, 2.8, 3.0, 3.2, 4., 5.};
      const int nmetbins = sizeof(metbins)/sizeof(*metbins) - 1;
      for(int idm = -1; idm < 4; ++idm) {
        TString nm = (idm > -1) ? Form("jettauonemetdeltaphi_%i", idm) : "jettauonemetdeltaphi";
        Utilities::BookH1F(fLepHist[i]->hJetTauOneMetDeltaPhi[idm+1], nm.Data(), Form("%s: OneMetDeltaPhi"  ,dirname), nmetbins, metbins, folder);
      }
      const double metqcdbins[] = {0., 0.3, 0.6, 0.9, 1.3, 1.8, 2.5, 3.2, 4.};
      const int nmetqcdbins = sizeof(metqcdbins)/sizeof(*metqcdbins) - 1;
      Utilities::BookH1F(fLepHist[i]->hJetTauOneMetDPhiQCD[0], "jettauonemetdphiqcd0", Form("%s: OneMetDeltaPhi"  ,dirname), nmetqcdbins, metqcdbins, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauOneMetDPhiQCD[1], "jettauonemetdphiqcd1", Form("%s: OneMetDeltaPhi"  ,dirname), nmetqcdbins, metqcdbins, folder);
      const double isobins[] = {0., 0.02, 0.05, 0.10, 0.15, 0.20, 0.25, 0.30, 0.40, 0.50};
      const int nisobins = sizeof(isobins)/sizeof(*isobins) - 1;
      Utilities::BookH1F(fLepHist[i]->hJetTauOneRelIso[0], "jettauonereliso_0", Form("%s: OneRelIso"  ,dirname), nisobins, isobins, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauOneRelIso[1], "jettauonereliso_1", Form("%s: OneRelIso"  ,dirname), nisobins, isobins, folder);

      //j-->tau compositions
      const double mtbins[] = {0., 10., 20., 30., 40., 50., 65., 90., 150.};
      const int    nmtbins  = sizeof(mtbins) / sizeof(*mtbins) - 1;
      Utilities::BookH1F(fLepHist[i]->hJetTauOneMetDPhiComp, "jettauonemetdphicomp", Form("%s: OneMetDeltaPhi"  ,dirname), nmetbins    , metbins, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauTwoMetDPhiComp, "jettautwometdphicomp", Form("%s: TwoMetDeltaPhi"  ,dirname), nmetbins    , metbins, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauOnePtComp     , "jettauoneptcomp"     , Form("%s: OnePt"           ,dirname), nbins_pt_qcd, pts_qcd, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauTwoPtComp     , "jettautwoptcomp"     , Form("%s: TwoPt"           ,dirname), nbins_pt_qcd, pts_qcd, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauMTOneComp     , "jettaumtonecomp"     , Form("%s: MTOne"           ,dirname), nmtbins     , mtbins , folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauMTTwoComp     , "jettaumttwocomp"     , Form("%s: MTTwo"           ,dirname), nmtbins     , mtbins , folder);

      for(int dmregion = 0; dmregion < 5; ++dmregion) {
        TString name_r = "jettautwoptvsr";
        if(dmregion > 0) {name_r += "_"; name_r += dmregion;}
        Utilities::BookH2F(fLepHist[i]->hJetTauTwoPtVsR[dmregion], name_r.Data() , Form("%s: Delta R Vs Two Pt"   ,dirname)  , nbins_pt, pts, nrbins, rbins, folder);
        TString name = "jettautwopt";
        if(dmregion > 0) {name += "_"; name += dmregion;}
        Utilities::BookH1F(fLepHist[i]->hJetTauTwoPt[dmregion], name.Data() , Form("%s: Two Pt"   ,dirname)  , nbins_pt, pts, folder);
      }

      const double etabins[] = {0., 0.75, 1.5, 1.9, 2.3};
      const int    netabins  = sizeof(etabins) / sizeof(*etabins) - 1;
      Utilities::BookH1F(fLepHist[i]->hJetTauTwoR[0]     , "jettautwor0"     , Form("%s: Delta R" ,dirname)  , nrbins, rbins, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauTwoR[1]     , "jettautwor"      , Form("%s: Delta R" ,dirname)  , nrbins, rbins, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauTwoEta[0]   , "jettautwoeta0"   , Form("%s: |Eta|"  ,dirname)  , netabins, etabins, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauTwoEta[1]   , "jettautwoeta"    , Form("%s: |Eta|"  ,dirname)  , netabins, etabins, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauTwoEtaQCD[0], "jettautwoetaqcd0", Form("%s: |Eta|"  ,dirname)  , netabins, etabins, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauTwoEtaQCD[1], "jettautwoetaqcd1", Form("%s: |Eta|"  ,dirname)  , netabins, etabins, folder);

      Utilities::BookH1F(fLepHist[i]->hJetTauTwoMetDeltaPhi[0], "jettautwometdeltaphi0", Form("%s: TwoMetDeltaPhi"  ,dirname), nmetbins, metbins, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauTwoMetDeltaPhi[1], "jettautwometdeltaphi" , Form("%s: TwoMetDeltaPhi"  ,dirname), nmetbins, metbins, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauTwoRelIso[0], "jettautworeliso_0", Form("%s: TwoRelIso"  ,dirname), nisobins, isobins, folder);
      Utilities::BookH1F(fLepHist[i]->hJetTauTwoRelIso[1], "jettautworeliso_1", Form("%s: TwoRelIso"  ,dirname), nisobins, isobins, folder);


      //2D distributions
      const double tau_pts[] = {0., 25., 30., 35., 40.,
                                50.,
                                150.};
      const int nbins_tau_pt = sizeof(tau_pts) / sizeof(*tau_pts) - 1;
      Utilities::BookH2F(fLepHist[i]->hJetTauTwoPtVsOnePt, "jettautwoptvsonept", Form("%s: Two pT vs One pT", dirname), nbins_pt, pts, nbins_tau_pt, tau_pts, folder);

      delete dirname;
    }
  }
}


//--------------------------------------------------------------------------------------------------------------
void JTTHistMaker::FillEventHistogram(EventHist_t* Hist) {
  if(!Hist) {
    printf("JTTHistMaker::%s: Attempting to fill histograms for an uninitialized book\n", __func__);
    throw 2;
  }
  Hist->hEventWeight              ->Fill(eventWeight);
  Hist->hLogEventWeight           ->Fill((eventWeight > 1.e-10) ? std::log10(eventWeight) : -999.);
  Hist->hGenWeight                ->Fill(genWeight                   , eventWeight          );

  Hist->hNMuons              ->Fill(nMuons             , genWeight*eventWeight)      ;
  Hist->hNElectrons          ->Fill(nElectrons         , genWeight*eventWeight)      ;
  Hist->hNTaus               ->Fill(nTaus              , genWeight*eventWeight)      ;
  Hist->hNPV[0]              ->Fill(nPV                , genWeight*eventWeight)      ;
  Hist->hMcEra               ->Fill(mcEra + 2*(fYear - 2016), genWeight*eventWeight)   ;
  Hist->hNJets20[0]          ->Fill(nJets20            , genWeight*eventWeight)      ;
  if(jetOne.p4 && jetOne.p4->Pt() > 0.) { //if 0 then no jet stored
    Hist->hJetPt[0]        ->Fill(jetOne.p4->Pt()             , genWeight*eventWeight)   ;
  }
  Hist->hNBJets20[0]         ->Fill(nBJets20           , genWeight*eventWeight)      ;
  Hist->hNBJets20L[0]        ->Fill(nBJets20L          , genWeight*eventWeight)      ;
  Hist->hHt                ->Fill(ht                 , genWeight*eventWeight)   ;

  if(tauP4 && tauP4->Pt() > 0.) { //if 0 then no additional tau stored
    Hist->hTauPt           ->Fill(tauP4->Pt()        , genWeight*eventWeight)   ;
    Hist->hTauEta          ->Fill(tauP4->Eta()       , genWeight*eventWeight)   ;
  }
  Hist->hTauGenFlavor      ->Fill(tauGenFlavor               , genWeight*eventWeight)   ;
  Hist->hTauDecayMode[0]   ->Fill(tauDecayMode               , genWeight*eventWeight)   ;
  Hist->hTauDeepAntiEle    ->Fill(std::log2(tauDeepAntiEle+1), genWeight*eventWeight)   ;
  Hist->hTauDeepAntiMu     ->Fill(tauDeepAntiMu              , genWeight*eventWeight)   ;
  Hist->hTauDeepAntiJet    ->Fill(std::log2(tauDeepAntiJet+1), genWeight*eventWeight)   ;

  Hist->hNGenTaus            ->Fill(nGenTaus             , genWeight*eventWeight)      ;
  Hist->hNGenElectrons       ->Fill(nGenElectrons        , genWeight*eventWeight)      ;
  Hist->hNGenMuons           ->Fill(nGenMuons            , genWeight*eventWeight)      ;

  Hist->hMet               ->Fill(met                , genWeight*eventWeight)      ;
  Hist->hMTOne             ->Fill(fTreeVars.mtone    , eventWeight*genWeight);
  Hist->hMTTwo             ->Fill(fTreeVars.mttwo    , eventWeight*genWeight);
  Hist->hMTLep             ->Fill(fTreeVars.mtlep    , eventWeight*genWeight);

  PtEtaPhiMVector lepSys = (*leptonOne.p4) + (*leptonTwo.p4);
  // PtEtaPhiMVector sys    = (photonP4) ? (*photonP4) + lepSys : lepSys;
  const double lepDelR   = std::fabs(Utilities::DeltaR(*leptonOne.p4, *leptonTwo.p4));
  const double lepDelPhi = std::fabs(Utilities::DeltaPhi(leptonOne.p4->Phi(), leptonTwo.p4->Phi()));
  const double lepDelEta = std::fabs(leptonOne.p4->Eta() - leptonTwo.p4->Eta());

  Hist->hLepPt[0]     ->Fill(lepSys.Pt()            ,eventWeight*genWeight);
  Hist->hLepM[0]      ->Fill(lepSys.M()             ,eventWeight*genWeight);
  Hist->hLepMt        ->Fill(lepSys.Mt()            ,eventWeight*genWeight);
  Hist->hLepEta       ->Fill(lepSys.Eta()           ,eventWeight*genWeight);
  Hist->hLepMEstimate   ->Fill(fTreeVars.mestimate   , eventWeight*genWeight);
  Hist->hLepMEstimateTwo->Fill(fTreeVars.mestimatetwo, eventWeight*genWeight);
  Hist->hLepDeltaPhi[0]->Fill(lepDelPhi             ,eventWeight*genWeight);
  Hist->hLepDeltaEta  ->Fill(lepDelEta              ,eventWeight*genWeight);
  Hist->hLepDeltaR[0] ->Fill(lepDelR                ,eventWeight*genWeight);

  Hist->hDeltaAlpha[0]->Fill(fTreeVars.deltaalphaz1, eventWeight*genWeight);
  Hist->hDeltaAlpha[1]->Fill(fTreeVars.deltaalphaz2, eventWeight*genWeight);
  Hist->hDeltaAlpha[2]->Fill(fTreeVars.deltaalphah1, eventWeight*genWeight);
  Hist->hDeltaAlpha[3]->Fill(fTreeVars.deltaalphah2, eventWeight*genWeight);

  //MVA outputs
  for(unsigned i = 0; i < fMVAConfig->names_.size(); ++i) {
    Hist->hMVA[i][0]->Fill(fMvaOutputs[i], fTreeVars.eventweightMVA); //remove training samples
    Hist->hMVA[i][1]->Fill(fMvaOutputs[i], fTreeVars.eventweightMVA); //remove training samples
  }

  /////////////////////////////////////
  // j-->tau specific histograms

  if(nTaus == 1 && (leptonTwo.isTau     ()) ) {
    for(UInt_t itau = 0; itau < nTaus; ++itau) {
      int dm = -1;
      const int njets = std::min(2, (int) nJets); //0, 1, or >= 2 jets
      bool fakeMC = fIsData == 0 && std::abs(TauFlavorFromID(Tau_genPartFlav[itau])) == 26;
      if(std::fabs(Tau_eta[itau]) > 2.3) continue;
      if(Tau_idAntiMu[itau] < 2) continue; //ignore ones that fail the old MVA anti-mu tight ID
      float tau_wt = 1.; //FIXME: Add an array of tau specific weights
      if(fIsData || tau_wt <= 0.) tau_wt = 1.;
      //remove j->tau weights if set
      if(jetToTauWeightBias > 0.) tau_wt *= 1./jetToTauWeightBias;
      if(!fakeMC) {
        Hist->hTausPt->Fill(Tau_pt[itau], tau_wt*eventWeight*genWeight);
        Hist->hTausEta->Fill(Tau_eta[itau], tau_wt*eventWeight*genWeight);
        Hist->hTausDM->Fill(Tau_decayMode[itau], tau_wt*eventWeight*genWeight);
        Hist->hTausAntiJet->Fill(std::log2(Tau_idDeepTau2017v2p1VSjet[itau]+1), tau_wt*eventWeight*genWeight);
        Hist->hTausAntiEle->Fill(std::log2(Tau_idDeepTau2017v2p1VSe[itau]+1), tau_wt*eventWeight*genWeight);
        Hist->hTausAntiMu ->Fill(Tau_idDeepTau2017v2p1VSmu[itau]              , tau_wt*eventWeight*genWeight);
        Hist->hTausMVAAntiMu->Fill(Tau_idAntiMu[itau]         , tau_wt*eventWeight*genWeight);
        Hist->hTausGenFlavor->Fill(TauFlavorFromID(Tau_genPartFlav[itau]), tau_wt*eventWeight*genWeight);
        PtEtaPhiMVector tausLV;
        tausLV.SetCoordinates(Tau_pt[itau], Tau_eta[itau], Tau_phi[itau], Tau_mass[itau]);
        Hist->hTausDeltaR->Fill(std::fabs(Utilities::DeltaR(tausLV, *leptonOne.p4)), tau_wt*eventWeight*genWeight);
      } else {
        Hist->hFakeTausPt->Fill(Tau_pt[itau], tau_wt*eventWeight*genWeight);
        Hist->hFakeTausEta->Fill(Tau_eta[itau], tau_wt*eventWeight*genWeight);
        Hist->hFakeTausDM->Fill(Tau_decayMode[itau], tau_wt*eventWeight*genWeight);
      }

      if     (Tau_decayMode[itau] == 0 ) dm = 0;
      else if(Tau_decayMode[itau] == 1 ) dm = 1;
      else if(Tau_decayMode[itau] == 10) dm = 2;
      else if(Tau_decayMode[itau] == 11) dm = 3;
      else if(Tau_decayMode[itau] < 0  ) dm = 0; //only happens in tree version with bug --> default to progress until fixed
      else continue; //non-accepted decay mode

      if(fakeMC) { //MC fake tau
        Hist->hFakeTauMCNJetDMPtEta[0][njets][dm]->Fill(Tau_pt[itau], std::fabs(Tau_eta[itau]), tau_wt*eventWeight*genWeight); //all taus
      } else { //genuine MC tau or data tau
        Hist->hFakeTauNJetDMPtEta[0][njets][dm]  ->Fill(Tau_pt[itau], std::fabs(Tau_eta[itau]), tau_wt*eventWeight*genWeight); //all taus
      }
      if(Tau_idDeepTau2017v2p1VSjet[itau] > fFakeTauIsoCut) { //isolated tau
        if(fakeMC) { //MC fake tau
          Hist->hFakeTauMCNJetDMPtEta[1][njets][dm]->Fill(Tau_pt[itau], std::fabs(Tau_eta[itau]), tau_wt*eventWeight*genWeight); //tight Iso
        } else {
          Hist->hFakeTauNJetDMPtEta[1][njets][dm]  ->Fill(Tau_pt[itau], std::fabs(Tau_eta[itau]), tau_wt*eventWeight*genWeight); //tight Iso
        }
      } else { //anti-isolated tau
        if(fakeMC) { //MC fake tau
          Hist->hFakeTauMCNJetDMPtEta[2][njets][dm]->Fill(Tau_pt[itau], std::fabs(Tau_eta[itau]), tau_wt*eventWeight*genWeight); //anti-Iso
        } else { //genuine MC tau or data tau
          Hist->hFakeTauNJetDMPtEta[2][njets][dm]  ->Fill(Tau_pt[itau], std::fabs(Tau_eta[itau]), tau_wt*eventWeight*genWeight); //anti-Iso
        }
      }
      // jet --> tau non-closure histograms
      Float_t wt_no_nonclosure(eventWeight*genWeight), wt_no_biascorr(eventWeight*genWeight);
      if(jetToTauWeightCorr > 0.) wt_no_nonclosure *= jetToTauWeight / jetToTauWeightBias; //remove non-closure and bias corrections
      if(jetToTauWeightBias > 0.) wt_no_biascorr   *= jetToTauWeightCorr / jetToTauWeightBias; //remove the bias correction

      Hist->hJetTauDeltaR[0]->Fill(fTreeVars.lepdeltar, eventWeight*genWeight);
      Hist->hJetTauDeltaR[1]->Fill(fTreeVars.lepdeltar, wt_no_nonclosure     );
      Hist->hJetTauDeltaR[2]->Fill(fTreeVars.lepdeltar, wt_no_biascorr       );
      Hist->hJetTauLepM  [0]->Fill(fTreeVars.lepm     , eventWeight*genWeight);
      Hist->hJetTauLepM  [1]->Fill(fTreeVars.lepm     , wt_no_nonclosure     );
      Hist->hJetTauLepM  [2]->Fill(fTreeVars.lepm     , wt_no_biascorr       );
      Hist->hJetTauMTLep [0]->Fill(fTreeVars.mtlep    , eventWeight*genWeight);
      Hist->hJetTauMTLep [1]->Fill(fTreeVars.mtlep    , wt_no_nonclosure     );
      Hist->hJetTauMTLep [2]->Fill(fTreeVars.mtlep    , wt_no_biascorr       );
      Hist->hJetTauMTOne [0]->Fill(fTreeVars.mtone    , eventWeight*genWeight);
      Hist->hJetTauMTOne [1]->Fill(fTreeVars.mtone    , wt_no_nonclosure     );
      Hist->hJetTauMTOne [2]->Fill(fTreeVars.mtone    , wt_no_biascorr       );
      Hist->hJetTauMTTwo [0]->Fill(fTreeVars.mttwo    , eventWeight*genWeight);
      Hist->hJetTauMTTwo [1]->Fill(fTreeVars.mttwo    , wt_no_nonclosure     );
      Hist->hJetTauMTTwo [2]->Fill(fTreeVars.mttwo    , wt_no_biascorr       );
    }
  } //end j-->tau scale factor section
}

//--------------------------------------------------------------------------------------------------------------
void JTTHistMaker::FillLepHistogram(LepHist_t* Hist) {
  if(!Hist) {
    printf("JTTHistMaker::%s: Attempting to fill histograms for an uninitialized book\n", __func__);
    throw 2;
  }

  /////////////
  //  Lep 1  //
  /////////////
  Hist->hOnePt[0]     ->Fill(fTreeVars.leponept            ,eventWeight*genWeight);
  Hist->hOneEta       ->Fill(leptonOne.p4->Eta()           ,eventWeight*genWeight);
  Hist->hOneDXY       ->Fill(leptonOne.dxy                 ,eventWeight*genWeight);
  Hist->hOneDZ        ->Fill(leptonOne.dz                  ,eventWeight*genWeight);
  Hist->hOneRelIso    ->Fill(fTreeVars.leponereliso       ,eventWeight*genWeight);
  Hist->hOneTrigger   ->Fill(leptonOne.trigger               ,eventWeight*genWeight);
  double oneMetDelPhi  = std::fabs(leptonOne.p4->Phi() - metPhi);
  if(oneMetDelPhi > M_PI) oneMetDelPhi = std::fabs(2.*M_PI - oneMetDelPhi);
  Hist->hOneMetDeltaPhi   ->Fill(oneMetDelPhi   ,eventWeight*genWeight);

  /////////////
  //  Lep 2  //
  /////////////

  Hist->hTwoPt[0]     ->Fill(fTreeVars.leptwopt            ,eventWeight*genWeight);
  Hist->hTwoEta       ->Fill(leptonTwo.p4->Eta()           ,eventWeight*genWeight);
  Hist->hTwoDXY       ->Fill(leptonTwo.dxy                 ,eventWeight*genWeight);
  Hist->hTwoDZ        ->Fill(leptonTwo.dz                  ,eventWeight*genWeight);
  Hist->hTwoRelIso    ->Fill(fTreeVars.leptworeliso       ,eventWeight*genWeight);
  Hist->hTwoTrigger   ->Fill(leptonTwo.trigger               ,eventWeight*genWeight);
  double twoMetDelPhi  = std::fabs(leptonTwo.p4->Phi() - metPhi);
  if(twoMetDelPhi > M_PI) twoMetDelPhi = std::fabs(2.*M_PI - twoMetDelPhi);
  Hist->hTwoMetDeltaPhi   ->Fill(twoMetDelPhi   ,eventWeight*genWeight);

  ////////////////////////////////////////////////
  // Lepton comparisons/2D distributions

  Hist->hPtDiff      ->Fill(fTreeVars.leponept-fTreeVars.leptwopt ,eventWeight*genWeight);

  ////////////////////////////////////////////////
  // j-->tau histograms

  float wt(1.f);
  //remove j->tau weight
  wt = (jetToTauWeightBias > 0.) ? jetToTauWeightBias : 1.;
  Hist->hOnePt[10]->Fill(fTreeVars.leponept, eventWeight*genWeight/wt);
  Hist->hTwoPt[10]->Fill(fTreeVars.leptwopt, eventWeight*genWeight/wt);
  //remove j->tau weight correction + bias correction
  wt = (jetToTauWeight > 0. && jetToTauWeightBias > 0.) ? jetToTauWeightBias/jetToTauWeight : 1.;
  Hist->hOnePt[11]->Fill(fTreeVars.leponept, eventWeight*genWeight/wt);
  Hist->hTwoPt[11]->Fill(fTreeVars.leptwopt, eventWeight*genWeight/wt);

  if(nTaus == 1 && ((leptonTwo.isTau     () && tauDecayMode%10 < 2) || (Tau_decayMode[0]%10 < 2))) {
    int dmr = (leptonTwo.isTau     ()) ? tauDecayMode : Tau_decayMode[0];
    if(dmr > 9) dmr -= (10 - 2); //10,11 --> 2,3
    dmr += 1; //dmr = DM ID + 1, so 0 can be inclusive
    dmr = std::min(4, dmr); //ensure it's not out of bounds
    PtEtaPhiMVector taulv;
    if(leptonTwo.isTau     ()) taulv = *leptonTwo.p4;
    else taulv.SetCoordinates(Tau_pt[0], Tau_eta[0], Tau_phi[0], Tau_mass[0]);

    //FIXME: Remove tau pT region histograms
    float wt_nojt(eventWeight*genWeight), wt_nocorr(eventWeight*genWeight), wt_nobias(eventWeight*genWeight);
    if(jetToTauWeightBias > 0.) wt_nojt   *= 1.                 / jetToTauWeightBias; // remove the entire weight
    if(jetToTauWeightBias > 0.) wt_nocorr *= jetToTauWeight     / jetToTauWeightBias; // remove the non-closure and bias corrections
    if(jetToTauWeightBias > 0.) wt_nobias *= jetToTauWeightCorr / jetToTauWeightBias; // remove the bias corrections
    double dr = std::fabs(Utilities::DeltaR(*leptonOne.p4, taulv));
    Hist->hJetTauOnePt[0]       ->Fill(fTreeVars.leponept, wt_nocorr);
    Hist->hJetTauOnePt[dmr]     ->Fill(fTreeVars.leponept, wt_nocorr);
    Hist->hJetTauOnePtQCD[0]    ->Fill(fTreeVars.leponept, eventWeight*genWeight);
    Hist->hJetTauOnePtQCD[1]    ->Fill(fTreeVars.leponept, wt_nocorr);
    Hist->hJetTauOnePtVsR[0]    ->Fill(fTreeVars.leponept, dr, wt_nocorr);
    Hist->hJetTauOnePtVsR[dmr]  ->Fill(fTreeVars.leponept, dr, wt_nocorr);
    Hist->hJetTauOneR->Fill(dr, wt_nocorr);
    Hist->hJetTauOneEta->Fill(std::fabs(leptonOne.p4->Eta()), wt_nocorr);
    Hist->hJetTauOneMetDeltaPhi[0]->Fill(fTreeVars.onemetdeltaphi, wt_nocorr); //inclusive
    Hist->hJetTauOneMetDeltaPhi[dmr]->Fill(fTreeVars.onemetdeltaphi, wt_nocorr);
    Hist->hJetTauOneMetDPhiQCD[0]->Fill(fTreeVars.onemetdeltaphi, eventWeight*genWeight); //inclusive
    Hist->hJetTauOneMetDPhiQCD[1]->Fill(fTreeVars.onemetdeltaphi, wt_nocorr); //inclusive
    Hist->hJetTauOneRelIso[0]->Fill(fTreeVars.leponereliso, eventWeight*genWeight);
    Hist->hJetTauOneRelIso[1]->Fill(fTreeVars.leponereliso, wt_nobias);

    Hist->hJetTauOneMetDPhiComp->Fill(fTreeVars.onemetdeltaphi, wt_nojt); //compositions
    Hist->hJetTauTwoMetDPhiComp->Fill(fTreeVars.twometdeltaphi, wt_nojt);
    Hist->hJetTauOnePtComp     ->Fill(fTreeVars.leponept       , wt_nojt);
    Hist->hJetTauTwoPtComp     ->Fill(fTreeVars.leptwopt       , wt_nojt);
    Hist->hJetTauMTOneComp     ->Fill(fTreeVars.mtone         , wt_nojt);
    Hist->hJetTauMTTwoComp     ->Fill(fTreeVars.mttwo         , wt_nojt);

    dr = std::fabs(Utilities::DeltaR(*leptonTwo.p4, taulv));
    Hist->hJetTauTwoPt[0]       ->Fill(fTreeVars.leptwopt, wt_nocorr);
    Hist->hJetTauTwoPt[dmr]     ->Fill(fTreeVars.leptwopt, wt_nocorr);
    Hist->hJetTauTwoPtVsR[0]    ->Fill(fTreeVars.leptwopt, dr, wt_nocorr);
    Hist->hJetTauTwoPtVsR[dmr]  ->Fill(fTreeVars.leptwopt, dr, wt_nocorr);
    Hist->hJetTauTwoR[0]->Fill(dr, eventWeight*genWeight);
    Hist->hJetTauTwoR[1]->Fill(dr, wt_nocorr);
    Hist->hJetTauTwoEta[0]->Fill(std::fabs(leptonTwo.p4->Eta()), eventWeight*genWeight);
    Hist->hJetTauTwoEta[1]->Fill(std::fabs(leptonTwo.p4->Eta()), wt_nocorr);
    Hist->hJetTauTwoEtaQCD[0]->Fill(std::fabs(leptonTwo.p4->Eta()), eventWeight*genWeight);
    Hist->hJetTauTwoEtaQCD[1]->Fill(std::fabs(leptonTwo.p4->Eta()), wt_nocorr);
    Hist->hJetTauTwoPtVsOnePt->Fill(fTreeVars.leponept, taulv.Pt(), wt_nocorr);
    Hist->hJetTauTwoMetDeltaPhi[0]->Fill(fTreeVars.twometdeltaphi, eventWeight*genWeight);
    Hist->hJetTauTwoMetDeltaPhi[1]->Fill(fTreeVars.twometdeltaphi, wt_nocorr);
    Hist->hJetTauTwoRelIso[0]->Fill(fTreeVars.leptworeliso, eventWeight*genWeight);
    Hist->hJetTauTwoRelIso[1]->Fill(fTreeVars.leptworeliso, wt_nobias);
  }
}


//--------------------------------------------------------------------------------------------------------------
// Main function, process each event in the chain
Bool_t JTTHistMaker::Process(Long64_t entry)
{
  if(InitializeEvent(entry)) return kTRUE;

  //object pT thresholds
  const float muon_pt(10.), electron_pt(15.), tau_pt(20.);

  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;

  //ignore ee/mumu/emu for now
  if(!(mutau || etau)) return kTRUE;

  fCutFlow->Fill(icutflow); ++icutflow; //5

  if(leptonOne.isElectron() && leptonOne.p4->Pt() <= electron_pt) return kTRUE;
  if(leptonTwo.isElectron() && leptonTwo.p4->Pt() <= electron_pt) return kTRUE;
  if(leptonOne.isMuon    () && leptonOne.p4->Pt() <= muon_pt    ) return kTRUE;
  if(leptonTwo.isMuon    () && leptonTwo.p4->Pt() <= muon_pt    ) return kTRUE;
  if(leptonOne.isTau     () && leptonOne.p4->Pt() <= tau_pt     ) return kTRUE;
  if(leptonTwo.isTau     () && leptonTwo.p4->Pt() <= tau_pt     ) return kTRUE;

  //leptons must satisfy the pt requirements and fire a trigger
  mutau = mutau && leptonOne.p4->Pt() > muon_trig_pt_     && leptonTwo.p4->Pt() > tau_pt && leptonOne.fired;
  etau  = etau  && leptonOne.p4->Pt() > electron_trig_pt_ && leptonTwo.p4->Pt() > tau_pt && leptonOne.fired;

  if(!(mutau || etau)) return kTRUE;
  fCutFlow->Fill(icutflow); ++icutflow; //6

  //eta region cuts
  const double electron_eta_max = (fUseEmbedCuts) ? 2.2 : 2.5;
  const double muon_eta_max     = (fUseEmbedCuts) ? 2.2 : 2.4;
  const double tau_eta_max      = (fUseEmbedCuts) ? 2.3 : 2.3;
  if(leptonOne.isElectron() && std::fabs(leptonOne.p4->Eta()) >= electron_eta_max) return kTRUE;
  if(leptonTwo.isElectron() && std::fabs(leptonTwo.p4->Eta()) >= electron_eta_max) return kTRUE;
  if(leptonOne.isMuon    () && std::fabs(leptonOne.p4->Eta()) >= muon_eta_max    ) return kTRUE;
  if(leptonTwo.isMuon    () && std::fabs(leptonTwo.p4->Eta()) >= muon_eta_max    ) return kTRUE;
  if(leptonOne.isTau     () && std::fabs(leptonOne.p4->Eta()) >= tau_eta_max     ) return kTRUE;
  if(leptonTwo.isTau     () && std::fabs(leptonTwo.p4->Eta()) >= tau_eta_max     ) return kTRUE;

  //reject electrons in the barrel/endcap gap region
  const float elec_gap_low(1.4442), elec_gap_high(1.566);
  etau &= std::fabs(leptonOne.scEta) < elec_gap_low || std::fabs(leptonOne.scEta) > elec_gap_high;

  //enforce the leptons are separated
  if(std::fabs(Utilities::DeltaR(*leptonOne.p4, *leptonTwo.p4)) < 0.3) return kTRUE;

  //apply reasonable lepton isolation cuts
  if(leptonOne.isElectron() && fTreeVars.leponereliso >= 0.5) return kTRUE;
  if(leptonTwo.isElectron() && fTreeVars.leptworeliso >= 0.5) return kTRUE;
  if(leptonOne.isMuon    () && fTreeVars.leponereliso >= 0.5) return kTRUE;
  if(leptonTwo.isMuon    () && fTreeVars.leptworeliso >= 0.5) return kTRUE;

  fCutFlow->Fill(icutflow); ++icutflow; //7

  const double mll = (*leptonOne.p4+*leptonTwo.p4).M();
  if(mll <= 50. || mll >= 170.) return kTRUE;

  fCutFlow->Fill(icutflow); ++icutflow; //8

  if(!(leptonOne.fired || leptonTwo.fired)) return kTRUE;

  fCutFlow->Fill(icutflow); ++icutflow; //9

  //ensure reasonable dxy/dz cuts
  const float max_dz(0.5), max_dxy(0.2);
  mutau &= std::fabs(leptonOne.dxy) < max_dxy;
  mutau &= std::fabs(leptonOne.dz ) < max_dz ;
  etau  &= std::fabs(leptonOne.dxy) < max_dxy;
  etau  &= std::fabs(leptonOne.dz ) < max_dz ;

  fCutFlow->Fill(icutflow); ++icutflow; //9
  if(!(mutau || etau)) return kTRUE;

  //remove additional leptons
  mutau &= nElectrons == 0;
  etau  &= nMuons     == 0;

  fCutFlow->Fill(icutflow); ++icutflow; //10
  if(!(mutau || etau)) return kTRUE;

  ////////////////////////////////////////////////////////////
  // Add tau IDs
  ////////////////////////////////////////////////////////////

  mutau &= isLooseTau || tauDeepAntiJet >= 50; //63 = tight
  mutau &= tauDeepAntiMu  >= 10; //15 = tight
  mutau &= tauDeepAntiEle >= 10; //15 = loose
  mutau &= leptonTwo.id2  >=  2; //1 = loose, 3 = tight tau MVA anti-muon ID

  etau  &= isLooseTau || tauDeepAntiJet >= 50; //
  etau  &= tauDeepAntiMu  >= 10; //15 = tight
  etau  &= tauDeepAntiEle >= 100; //63 = tight, 127 = vtight
  etau  &= leptonTwo.id2  >=  2; //1 = loose, 3 = tight tau MVA anti-muon ID

  //remove tau decay modes not interested in
  mutau &= tauDecayMode != 5 && tauDecayMode != 6;
  etau  &= tauDecayMode != 5 && tauDecayMode != 6;

  fCutFlow->Fill(icutflow); ++icutflow; //11
  if(!(mutau || etau)) return kTRUE;

  //cut-flow for not loose lepton/QCD
  if(!looseQCDSelection)               {fCutFlow->Fill(icutflow);} //12
  ++icutflow;
  if(!looseQCDSelection && chargeTest) {fCutFlow->Fill(icutflow);} //13
  ++icutflow;

  ////////////////////////////////////////////////////////////
  // jet --> tau cuts and region definitions
  ////////////////////////////////////////////////////////////

  const double met_cut         = -1.;
  const double mtlep_cut       = 70.;
  const double qcd_mtlep_cut   = mtlep_cut; //(etau) ? 45. : mtlep_cut;
  const bool looseQCDRegion    = nBJetsUse == 0 && fTreeVars.mtlep < mtlep_cut && (met_cut < 0. || met < met_cut); // no isolation cut (0 - 0.5 allowed)
  const bool qcdSelection      = looseQCDRegion && fTreeVars.mtlep < qcd_mtlep_cut && (fTreeVars.leponereliso > 0.05) && !(isLooseMuon || isLooseElectron);
  const bool wjetsSelection    = fTreeVars.mtlep > mtlep_cut && nBJetsUse == 0 && !(isLooseMuon || isLooseElectron) && chargeTest;
  const bool topSelection      = nBJetsUse >= 1 && !(isLooseMuon || isLooseElectron) && chargeTest;
  const bool nominalSelection  = nBJetsUse == 0 && (met_cut < 0. || met < met_cut) && fTreeVars.mtlep < mtlep_cut && !(isLooseMuon || isLooseElectron);

  /**
     SR = Signal Region, AR = Application Region, DR = Determination Region, BR = Bias Region
     Nominal Selection = No b-jets, MET/MT(ll,MET) cuts, isolated muon/electron, isolated tau, opposite sign (OS)
     Each DR has two sub-regions: SR-like and AR-like regions defined by isolated or anti-isolated taus

     SR = Nominal Selection
     AR = Nominal Selection except anti-isolated tau

     W+Jets:
     DR = Nominal Selection except: no MET cut and inverted MT(ll, MET) cut
     BR = Measure MC scale factors in DR then test in MC in the AR/SR

     QCD:
     DR = Nominal Selection except: same sign (SS), muon/electron iso > 0.05
     BR_iso = DR except: isolation 0 - 0.5 is allowed, bias defined in terms of isolation
     BR_SS = Use iso in 0.15-0.5, measure in SS then test bias in OS region

     ttbar:
     DR = AR/SR in MC

   **/

  ////////////////////////////////////////////////////////////
  // Set 92 (93) + selection offset: QCD selection without any isolation cut with (without) MC taus
  ////////////////////////////////////////////////////////////

  if((etau || mutau) && looseQCDRegion) {
    int qcd_set_offset = set_offset;
    if(looseQCDSelection && !isLooseTau && (isLooseMuon || isLooseElectron)) qcd_set_offset -= fMisIDOffset; //remove offset if the loose lepton isn't the tau
    const Float_t prev_evt_wt = eventWeight;
    const Float_t prev_jtt_wt = jetToTauWeight;
    const Float_t prev_jtt_cr = jetToTauWeightCorr;
    const Float_t prev_jtt_bs = jetToTauWeightBias;
    const Float_t evt_wt_bare = (prev_jtt_bs > 0.) ? prev_evt_wt / prev_jtt_bs : 0.; //no j->tau weight
    const bool add_wt = fUseJetToTauComposition && isLooseTau;
    const bool isfake = !fIsData && (std::abs(tauGenFlavor) == 26 || isFakeElectron || isFakeMuon);
    //QCD weights from DR
    if(add_wt) {
      jetToTauWeight     = fJetToTauWts                      [JetToTauComposition::kQCD];
      jetToTauWeightCorr = jetToTauWeight*fJetToTauCorrs     [JetToTauComposition::kQCD];
      jetToTauWeightBias = jetToTauWeightCorr;//*fJetToTauBiases[JetToTauComposition::kQCD];
      eventWeight        = jetToTauWeightBias*evt_wt_bare;
    }
    //full isolation region for isolation bias measurement, no bias corrections
    if(!chargeTest) {
      FillAllHistograms(qcd_set_offset + 92);
      if(!isfake) FillAllHistograms(qcd_set_offset + 93);
    }

    if(fTreeVars.mtlep < qcd_mtlep_cut && fTreeVars.leponereliso > 0.15) { //high isolation region for SS --> OS bias measurement
      //QCD weights from high iso region, no bias corrections (stored in MC weights, but high iso weights)
      if(add_wt) {
        jetToTauWeight     = fJetToTauMCWts                      [JetToTauComposition::kQCD];
        jetToTauWeightCorr = jetToTauWeight*fJetToTauMCCorrs     [JetToTauComposition::kQCD];
        jetToTauWeightBias = jetToTauWeightCorr;//*fJetToTauMCBiases[JetToTauComposition::kQCD];
        eventWeight        = jetToTauWeightBias*evt_wt_bare;
      }
      FillAllHistograms(qcd_set_offset + 94);
      if(!isfake) FillAllHistograms(qcd_set_offset + 95);
    }
    //Restore the previous weights
    eventWeight        = prev_evt_wt;
    jetToTauWeight     = prev_jtt_wt;
    jetToTauWeightCorr = prev_jtt_cr;
    jetToTauWeightBias = prev_jtt_bs;
  }

  //////////////////////////
  // Handle loose leptons //
  //////////////////////////

  mutau &= !isLooseMuon;
  etau  &= !isLooseElectron;

  if(!(mutau || etau)) return kTRUE;
  if(!looseQCDSelection && chargeTest) {fCutFlow->Fill(icutflow);} //14
  ++icutflow;

  ////////////////////////////////////////////////////////////
  // Set 35 + selection offset: last set with MC estimated taus and leptons
  ////////////////////////////////////////////////////////////

  if((mutau || etau) && nominalSelection) {
    const Float_t prev_evt_wt = eventWeight;
    const Float_t prev_jtt_wt = jetToTauWeight;
    const Float_t prev_jtt_cr = jetToTauWeightCorr;
    const Float_t prev_jtt_bs = jetToTauWeightBias;
    const Float_t evt_wt_bare = (prev_jtt_bs > 0.) ? prev_evt_wt / prev_jtt_bs : 0.; //no j->tau weight
    const bool add_wt = fUseJetToTauComposition && isLooseTau;
    const bool isfake = !fIsData && (std::abs(tauGenFlavor) == 26 || isFakeElectron || isFakeMuon);

    //Nominal mixture
    FillAllHistograms(set_offset + 35);
    //fake tau only contribution
    if(isfake) FillAllHistograms(set_offset + 42);

    //Nominal mixture, without j-->tau weights
    if(add_wt) {
      jetToTauWeight     = 1.f;
      jetToTauWeightCorr = 1.f;
      jetToTauWeightBias = 1.f;
      eventWeight        = evt_wt_bare;
    }
    if(isLooseTau) {
      FillAllHistograms(set_offset + 33); //only fill for loose ID taus, since non-loose already have no j-->tau weights
    }

    //W+Jets data based weights from DR to test W+Jets only vs composition
    if(add_wt) {
      jetToTauWeight     = fJetToTauWts                      [JetToTauComposition::kWJets];
      jetToTauWeightCorr = jetToTauWeight*fJetToTauCorrs     [JetToTauComposition::kWJets];
      jetToTauWeightBias = jetToTauWeightCorr*fJetToTauBiases[JetToTauComposition::kWJets];
      eventWeight        = jetToTauWeightBias*evt_wt_bare;
    }
    if(chargeTest) {
      if(!isfake) { //no fake MC events
        FillAllHistograms(set_offset + 34);
      }
    }

    //W+Jets MC based weights from DR for bias tests
    if(add_wt) {
      jetToTauWeight     = fJetToTauMCWts                      [JetToTauComposition::kWJets];
      jetToTauWeightCorr = jetToTauWeight*fJetToTauMCCorrs     [JetToTauComposition::kWJets];
      jetToTauWeightBias = jetToTauWeightCorr*fJetToTauMCBiases[JetToTauComposition::kWJets];
      eventWeight        = jetToTauWeightBias*evt_wt_bare;
    }
    if(chargeTest) {
      FillAllHistograms(set_offset + 81);
      if(!isfake) { //no fake MC events
        FillAllHistograms(set_offset + 86);
      }
    }

    //Top MC based weights from DR (nominal weights are MC in this region anyway)
    if(add_wt) {
      jetToTauWeight     = fJetToTauMCWts                      [JetToTauComposition::kTop];
      jetToTauWeightCorr = jetToTauWeight*fJetToTauMCCorrs     [JetToTauComposition::kTop];
      jetToTauWeightBias = jetToTauWeightCorr*fJetToTauMCBiases[JetToTauComposition::kTop];
      eventWeight        = jetToTauWeightBias*evt_wt_bare;
    }
    if(chargeTest) {
      FillAllHistograms(set_offset + 82);
      if(!isfake) { //no fake MC events
        FillAllHistograms(set_offset + 87);
      }
    }

    //Restore the previous weights
    eventWeight        = prev_evt_wt;
    jetToTauWeight     = prev_jtt_wt;
    jetToTauWeightCorr = prev_jtt_cr;
    jetToTauWeightBias = prev_jtt_bs;
  }

  ////////////////////////////////////////////////////////////////////////////
  // Set 36 + selection offset: QCD selection with MC estimate taus and leptons
  ////////////////////////////////////////////////////////////////////////////
  if((mutau || etau) && qcdSelection && !chargeTest) {
    const Float_t prev_evt_wt = eventWeight;
    const Float_t prev_jtt_wt = jetToTauWeight;
    const Float_t prev_jtt_cr = jetToTauWeightCorr;
    const Float_t prev_jtt_bs = jetToTauWeightBias;
    const Float_t evt_wt_bare = (prev_jtt_bs > 0.) ? prev_evt_wt / prev_jtt_bs : 0.; //no j->tau weight
    if(fUseJetToTauComposition && isLooseTau) {
      jetToTauWeight     = fJetToTauWts                      [JetToTauComposition::kQCD];
      jetToTauWeightCorr = jetToTauWeight*fJetToTauCorrs     [JetToTauComposition::kQCD];
      jetToTauWeightBias = jetToTauWeightCorr; //no bias in DR
      eventWeight        = jetToTauWeightBias*evt_wt_bare;
    }
    FillAllHistograms(set_offset + 36);
    eventWeight        = prev_evt_wt;
    jetToTauWeight     = prev_jtt_wt;
    jetToTauWeightCorr = prev_jtt_cr;
    jetToTauWeightBias = prev_jtt_bs;
  }


  ////////////////////////////////////////////////////////////////////////////
  // Set 37 + selection offset: W+Jets selection with MC estimate taus and leptons
  ////////////////////////////////////////////////////////////////////////////
  if((mutau || etau) && wjetsSelection) {
    const Float_t prev_evt_wt = eventWeight;
    const Float_t prev_jtt_wt = jetToTauWeight;
    const Float_t prev_jtt_cr = jetToTauWeightCorr;
    const Float_t prev_jtt_bs = jetToTauWeightBias;
    const Float_t evt_wt_bare = (prev_jtt_bs > 0.) ? prev_evt_wt / prev_jtt_bs : 0.; //no j->tau weight
    const bool    add_wt      = fUseJetToTauComposition && isLooseTau;
    const bool    isfake      = !fIsData && (std::abs(tauGenFlavor) == 26 || isFakeElectron || isFakeMuon);
    //Data measured weights
    if(add_wt) {
      jetToTauWeight     = fJetToTauWts                      [JetToTauComposition::kWJets];
      jetToTauWeightCorr = jetToTauWeight*fJetToTauCorrs     [JetToTauComposition::kWJets];
      jetToTauWeightBias = jetToTauWeightCorr; //no bias in DR
      eventWeight        = jetToTauWeightBias*evt_wt_bare;
    }
    FillAllHistograms(set_offset + 37);
    //MC measured weights
    if(add_wt) {
      jetToTauWeight     = fJetToTauMCWts                 [JetToTauComposition::kWJets];
      jetToTauWeightCorr = jetToTauWeight*fJetToTauMCCorrs[JetToTauComposition::kWJets];
      jetToTauWeightBias = jetToTauWeightCorr; //no bias in DR
      eventWeight        = jetToTauWeightBias*evt_wt_bare;
    }
    FillAllHistograms(set_offset + 88);
    if(!isfake) { //no fake MC events
      FillAllHistograms(set_offset + 89);
    }
    eventWeight        = prev_evt_wt;
    jetToTauWeight     = prev_jtt_wt;
    jetToTauWeightCorr = prev_jtt_cr;
    jetToTauWeightBias = prev_jtt_bs;
  }

  ////////////////////////////////////////////////////////////////////////////
  // Set 38 + selection offset: ttbar selection with MC estimate taus and leptons
  ////////////////////////////////////////////////////////////////////////////
  if((mutau || etau) && topSelection) {
    const Float_t prev_evt_wt = eventWeight;
    const Float_t prev_jtt_wt = jetToTauWeight;
    const Float_t prev_jtt_cr = jetToTauWeightCorr;
    const Float_t prev_jtt_bs = jetToTauWeightBias;
    const Float_t evt_wt_bare = (prev_jtt_bs > 0.) ? prev_evt_wt / prev_jtt_bs : 0.; //no j->tau weight
    if(fUseJetToTauComposition && isLooseTau) {
      jetToTauWeight     = fJetToTauWts                      [JetToTauComposition::kTop];
      jetToTauWeightCorr = jetToTauWeight*fJetToTauCorrs     [JetToTauComposition::kTop];
      jetToTauWeightBias = jetToTauWeightCorr; //no bias in DR
      eventWeight        = jetToTauWeightBias*evt_wt_bare;
    }
    FillAllHistograms(set_offset + 38);
    eventWeight        = prev_evt_wt;
    jetToTauWeight     = prev_jtt_wt;
    jetToTauWeightCorr = prev_jtt_cr;
    jetToTauWeightBias = prev_jtt_bs;
  }

  //remove MC jet -> light lepton contribution
  if(!fUseMCEstimatedFakeLep && !fIsData) {
    mutau &= !isFakeMuon;
    etau  &= !isFakeElectron;
  }

  //remove MC estimated jet --> tau component
  mutau &= fIsData > 0 || std::abs(tauGenFlavor) != 26;
  etau  &= fIsData > 0 || std::abs(tauGenFlavor) != 26;

  if(!(mutau || etau)) return kTRUE;

  if(!looseQCDSelection && chargeTest) {fCutFlow->Fill(icutflow);} //15
  ++icutflow;

  ////////////////////////////////////////////////////////////////////////////
  // Set 30 + selection offset: QCD selection
  ////////////////////////////////////////////////////////////////////////////
  if((mutau || etau) && qcdSelection && !chargeTest) {
    const Float_t prev_evt_wt = eventWeight;
    const Float_t prev_jtt_wt = jetToTauWeight;
    const Float_t prev_jtt_cr = jetToTauWeightCorr;
    const Float_t prev_jtt_bs = jetToTauWeightBias;
    const Float_t evt_wt_bare = (prev_jtt_bs > 0.) ? prev_evt_wt / prev_jtt_bs : 0.; //no j->tau weight
    if(fUseJetToTauComposition && isLooseTau) {
      jetToTauWeight     = fJetToTauWts                      [JetToTauComposition::kQCD];
      jetToTauWeightCorr = jetToTauWeight*fJetToTauCorrs     [JetToTauComposition::kQCD];
      jetToTauWeightBias = jetToTauWeightCorr; //no bias in DR
      eventWeight        = jetToTauWeightBias*evt_wt_bare;
    }
    FillAllHistograms(set_offset + 30);
    eventWeight        = prev_evt_wt;
    jetToTauWeight     = prev_jtt_wt;
    jetToTauWeightCorr = prev_jtt_cr;
    jetToTauWeightBias = prev_jtt_bs;
  }

  ////////////////////////////////////////////////////////////////////////////
  // Set 31 + selection offset: W+Jets selection
  ////////////////////////////////////////////////////////////////////////////
  if((mutau || etau) && wjetsSelection) {
    const Float_t prev_evt_wt = eventWeight;
    const Float_t prev_jtt_wt = jetToTauWeight;
    const Float_t prev_jtt_cr = jetToTauWeightCorr;
    const Float_t prev_jtt_bs = jetToTauWeightBias;
    const Float_t evt_wt_bare = (prev_jtt_bs > 0.) ? prev_evt_wt / prev_jtt_bs : 0.; //no j->tau weight
    if(fUseJetToTauComposition && isLooseTau) {
      jetToTauWeight     = fJetToTauWts                      [JetToTauComposition::kWJets];
      jetToTauWeightCorr = jetToTauWeight*fJetToTauCorrs     [JetToTauComposition::kWJets];
      jetToTauWeightBias = jetToTauWeightCorr; //no bias in DR
      eventWeight        = jetToTauWeightBias*evt_wt_bare;
    }
    FillAllHistograms(set_offset + 31);
    eventWeight        = prev_evt_wt;
    jetToTauWeight     = prev_jtt_wt;
    jetToTauWeightCorr = prev_jtt_cr;
    jetToTauWeightBias = prev_jtt_bs;
  }

  ////////////////////////////////////////////////////////////////////////////
  // Set 32 + selection offset: Top selection
  ////////////////////////////////////////////////////////////////////////////
  if((mutau || etau) && topSelection) {
    const Float_t prev_evt_wt = eventWeight;
    const Float_t prev_jtt_wt = jetToTauWeight;
    const Float_t prev_jtt_cr = jetToTauWeightCorr;
    const Float_t prev_jtt_bs = jetToTauWeightBias;
    const Float_t evt_wt_bare = (prev_jtt_bs > 0.) ? prev_evt_wt / prev_jtt_bs : 0.; //no j->tau weight
    if(fUseJetToTauComposition && isLooseTau) {
      jetToTauWeight     = fJetToTauWts                      [JetToTauComposition::kTop];
      jetToTauWeightCorr = jetToTauWeight*fJetToTauCorrs     [JetToTauComposition::kTop];
      jetToTauWeightBias = jetToTauWeightCorr; //no bias in DR
      eventWeight        = jetToTauWeightBias*evt_wt_bare;
    }
    FillAllHistograms(set_offset + 32);
    eventWeight        = prev_evt_wt;
    jetToTauWeight     = prev_jtt_wt;
    jetToTauWeightCorr = prev_jtt_cr;
    jetToTauWeightBias = prev_jtt_bs;
  }

  //////////////////////////
  //    Add MET cuts      //
  //////////////////////////

  mutau &= met_cut < 0. || met < met_cut;
  etau  &= met_cut < 0. || met < met_cut;

  //Add W+Jets selection orthogonality condition
  mutau &= fTreeVars.mtlep < mtlep_cut;
  etau  &= fTreeVars.mtlep < mtlep_cut;

  if(!(mutau || etau)) return kTRUE;

  if(!looseQCDSelection && chargeTest) {fCutFlow->Fill(icutflow);} //16
  ++icutflow;

  //////////////////////////
  //    Reject b-jets     //
  //////////////////////////

  mutau &= nBJetsUse == 0;
  etau  &= nBJetsUse == 0;

  if(!(mutau || etau)) return kTRUE;
  if(!looseQCDSelection && chargeTest) {fCutFlow->Fill(icutflow);} //17
  ++icutflow;

  ////////////////////////////////////////////////////////////
  // Set 8 + selection offset: preselection
  ////////////////////////////////////////////////////////////
  fTimes[GetTimerNumber("SingleFill")] = std::chrono::steady_clock::now(); //timer for filling all histograms
  FillAllHistograms(set_offset + 8);
  IncrementTimer("SingleFill", true);

  return kTRUE;
}
