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
    fEventSets[i]  = 0;
    fSysSets[i] = 0;
  }
  for(int i = 0; i < fn; ++i) {
    fTreeSets[i]  = 0;
  }

  //Event Sets
  const bool mutau = fSelection == "" || fSelection == "mutau";
  const bool etau  = fSelection == "" || fSelection == "etau" ;
  const bool emu   = fSelection == "" || fSelection == "emu"  ;
  const bool mumu  = fSelection == "" || fSelection == "mumu" ;
  const bool ee    = fSelection == "" || fSelection == "ee"   ;

  if(mutau) {
    fEventSets [kMuTau + 8] = 1; // preselection
  }
  if(etau) {
    fEventSets [kETau + 8] = 1; // preselection
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

      Utilities::BookH1D(fEventHist[i]->hNMuons                  , "nmuons"                  , Form("%s: NMuons"                      ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNElectrons              , "nelectrons"              , Form("%s: NElectrons"                  ,dirname),  10,    0,  10, folder);
      Utilities::BookH1D(fEventHist[i]->hNTaus                   , "ntaus"                   , Form("%s: NTaus"                       ,dirname),  10,    0,  10, folder);

      Utilities::BookH1D(fEventHist[i]->hMcEra                   , "mcera"                   , Form("%s: MCEra + 2*(year-2016)"       ,dirname),   8,    0,   8, folder);

      Utilities::BookH1D(fEventHist[i]->hNJets20[0]              , "njets20"                 , Form("%s: NJets20"                     ,dirname),  10,    0,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hJetPt[0]           , "jetpt"               , Form("%s: JetPt"               ,dirname), 100,    0, 200, folder);

      Utilities::BookH1F(fEventHist[i]->hMet                 , "met"                 , Form("%s: Met"                     ,dirname)  , 100,  0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hMTOne               , "mtone"               , Form("%s: MTOne"                   ,dirname)  , 100, 0.,   150., folder);
      Utilities::BookH1F(fEventHist[i]->hMTTwo               , "mttwo"               , Form("%s: MTTwo"                   ,dirname)  , 100, 0.,   150., folder);
      Utilities::BookH1F(fEventHist[i]->hMTLep               , "mtlep"               , Form("%s: MTLep"                   ,dirname)  , 100, 0.,   150., folder);

      Utilities::BookH1F(fEventHist[i]->hLepPt[0], "leppt"         , Form("%s: Lepton Pt"      ,dirname)  , 100,   0, 200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepM[0] , "lepm"          , Form("%s: Lepton M"       ,dirname)  , 280,  40, 180, folder);
      Utilities::BookH1F(fEventHist[i]->hLepMEstimate   , "lepmestimate"   , Form("%s: Estimate di-lepton mass"  ,dirname)  ,100,0.,  200, folder);
      Utilities::BookH1F(fEventHist[i]->hLepMEstimateTwo, "lepmestimatetwo", Form("%s: Estimate di-lepton mass"  ,dirname)  ,100,0.,  200, folder);

      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[0]     , "deltaalpha0"      , Form("%s: Delta Alpha (Z) 0"   ,dirname),  80,  -5,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[1]     , "deltaalpha1"      , Form("%s: Delta Alpha (Z) 1"   ,dirname),  80,  -5,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[2]     , "deltaalpha2"      , Form("%s: Delta Alpha (H) 0"   ,dirname),  80,  -5,  10, folder);
      Utilities::BookH1F(fEventHist[i]->hDeltaAlpha[3]     , "deltaalpha3"      , Form("%s: Delta Alpha (H) 1"   ,dirname),  80,  -5,  10, folder);

      for(int ji = 0; ji < JetToTauComposition::kLast; ++ji) {
        Utilities::BookH1F(fEventHist[i]->hJetToTauComps[ji]     , Form("jettotaucomps_%i", ji), Form("%s: JetToTauComps %i"      ,dirname, ji),  50,    0,   2, folder);
        Utilities::BookH1F(fEventHist[i]->hJetToTauWts  [ji]     , Form("jettotauwts_%i"  , ji), Form("%s: JetToTauWts %i"        ,dirname, ji),  50,    0,   2, folder);
      }

      //j->tau measurement histograms
      if(fSelection == "" || fSelection == "mutau" || fSelection == "etau") {
        //variable width bins for eta vs pT
        const double tetabins[] = { 0., 1.5, 2.3};
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
        const double jt_lepm_bins[] = {50., 60., 70., 80., 100., 120., 140., 200.};
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


      //for correcting jet -> tau scale factors
      const double pts[] = {0.  , 20. , 25. , 30. , 35. ,
                            40. , 45. , 50. , 55. , 65. ,
                            80. , 100. , 120.,
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

      const double pts_qcd[] = {0.  , 20. , 25. , 30. , 35. ,
                                40. , 50. , 60. ,
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
  Hist->hNGenTaus            ->Fill(nGenTaus             , genWeight*eventWeight)      ;
  Hist->hNGenElectrons       ->Fill(nGenElectrons        , genWeight*eventWeight)      ;
  Hist->hNGenMuons           ->Fill(nGenMuons            , genWeight*eventWeight)      ;
  Hist->hNJets20[0]          ->Fill(nJets20            , genWeight*eventWeight)      ;
  Hist->hNFwdJets            ->Fill(nFwdJets           , genWeight*eventWeight)      ;
  Hist->hNBJets20[0]         ->Fill(nBJets20           , genWeight*eventWeight)      ;
  Hist->hNBJets20L[0]        ->Fill(nBJets20L          , genWeight*eventWeight)      ;


  if(jetOneP4 && jetOneP4->Pt() > 0.) { //if 0 then no jet stored
    Hist->hJetPt[0]        ->Fill(jetOneP4->Pt()             , genWeight*eventWeight)   ;
  }
  Hist->hHt                ->Fill(ht                 , genWeight*eventWeight)   ;

  Hist->hMet               ->Fill(met                , genWeight*eventWeight)      ;
  Hist->hMTOne             ->Fill(fTreeVars.mtone    , eventWeight*genWeight);
  Hist->hMTTwo             ->Fill(fTreeVars.mttwo    , eventWeight*genWeight);
  Hist->hMTLep             ->Fill(fTreeVars.mtlep    , eventWeight*genWeight);

  TLorentzVector lepSys = (*leptonOneP4) + (*leptonTwoP4);
  TLorentzVector sys    = (photonP4) ? (*photonP4) + lepSys : lepSys;

  Hist->hLepPt[0]     ->Fill(lepSys.Pt()            ,eventWeight*genWeight);
  Hist->hLepM[0]      ->Fill(lepSys.M()             ,eventWeight*genWeight);
  Hist->hLepMt        ->Fill(lepSys.Mt()            ,eventWeight*genWeight);
  Hist->hLepMEstimate   ->Fill(fTreeVars.mestimate   , eventWeight*genWeight);
  Hist->hLepMEstimateTwo->Fill(fTreeVars.mestimatetwo, eventWeight*genWeight);

  Hist->hDeltaAlpha[0]->Fill(fTreeVars.deltaalphaz1, eventWeight*genWeight);
  Hist->hDeltaAlpha[1]->Fill(fTreeVars.deltaalphaz2, eventWeight*genWeight);
  Hist->hDeltaAlpha[2]->Fill(fTreeVars.deltaalphah1, eventWeight*genWeight);
  Hist->hDeltaAlpha[3]->Fill(fTreeVars.deltaalphah2, eventWeight*genWeight);

  //MVA outputs
  for(unsigned i = 0; i < fMVAConfig.names_.size(); ++i) {
    Hist->hMVA[i][0]->Fill(fMvaOutputs[i], fTreeVars.eventweightMVA); //remove training samples
    Hist->hMVA[i][1]->Fill(fMvaOutputs[i], fTreeVars.eventweightMVA); //remove training samples
  }
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
  Hist->hOneEta       ->Fill(leptonOneP4->Eta()           ,eventWeight*genWeight);
  Hist->hOneDXY       ->Fill(leptonOneDXY                 ,eventWeight*genWeight);
  Hist->hOneDZ        ->Fill(leptonOneDZ                  ,eventWeight*genWeight);
  Hist->hOneRelIso    ->Fill(fTreeVars.leponereliso       ,eventWeight*genWeight);
  Hist->hOneTrigger   ->Fill(leptonOneTrigger               ,eventWeight*genWeight);
  double oneMetDelPhi  = std::fabs(leptonOneP4->Phi() - metPhi);
  if(oneMetDelPhi > M_PI) oneMetDelPhi = std::fabs(2.*M_PI - oneMetDelPhi);
  Hist->hOneMetDeltaPhi   ->Fill(oneMetDelPhi   ,eventWeight*genWeight);

  /////////////
  //  Lep 2  //
  /////////////

  Hist->hTwoPt[0]     ->Fill(fTreeVars.leptwopt            ,eventWeight*genWeight);
  Hist->hTwoEta       ->Fill(leptonTwoP4->Eta()           ,eventWeight*genWeight);
  Hist->hTwoDXY       ->Fill(leptonTwoDXY                 ,eventWeight*genWeight);
  Hist->hTwoDZ        ->Fill(leptonTwoDZ                  ,eventWeight*genWeight);
  Hist->hTwoRelIso    ->Fill(fTreeVars.leptworeliso       ,eventWeight*genWeight);
  Hist->hTwoTrigger   ->Fill(leptonTwoTrigger               ,eventWeight*genWeight);
  double twoMetDelPhi  = std::fabs(leptonTwoP4->Phi() - metPhi);
  if(twoMetDelPhi > M_PI) twoMetDelPhi = std::fabs(2.*M_PI - twoMetDelPhi);
  Hist->hTwoMetDeltaPhi   ->Fill(twoMetDelPhi   ,eventWeight*genWeight);

  ////////////////////////////////////////////////
  // Lepton comparisons/2D distributions

  Hist->hPtDiff      ->Fill(fTreeVars.leponept-fTreeVars.leptwopt ,eventWeight*genWeight);
}


//--------------------------------------------------------------------------------------------------------------
// Main function, process each event in the chain
Bool_t JTTHistMaker::Process(Long64_t entry)
{
  if(InitializeEvent(entry)) return kTRUE;

  ////////////////////////////////////////////////////////////
  // Set 1 + selection offset: base selection
  ////////////////////////////////////////////////////////////
  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;
  fTimes[3] = std::chrono::steady_clock::now(); //timer for filling all histograms
  FillAllHistograms(set_offset + 1);
  IncrementTimer(3, true);

  fCutFlow->Fill(icutflow); ++icutflow; //5

  //object pT thresholds
  float muon_pt(10.), electron_pt(15.), tau_pt(20.);

  if(std::abs(leptonOneFlavor) == 11 && leptonOneP4->Pt() <= electron_pt) return kTRUE;
  if(std::abs(leptonTwoFlavor) == 11 && leptonTwoP4->Pt() <= electron_pt) return kTRUE;
  if(std::abs(leptonOneFlavor) == 13 && leptonOneP4->Pt() <= muon_pt    ) return kTRUE;
  if(std::abs(leptonTwoFlavor) == 13 && leptonTwoP4->Pt() <= muon_pt    ) return kTRUE;
  if(std::abs(leptonOneFlavor) == 15 && leptonOneP4->Pt() <= tau_pt     ) return kTRUE;
  if(std::abs(leptonTwoFlavor) == 15 && leptonTwoP4->Pt() <= tau_pt     ) return kTRUE;

  fCutFlow->Fill(icutflow); ++icutflow; //6

  //eta region cuts
  const double electron_eta_max = (fUseEmbedCuts) ? 2.2 : 2.5;
  const double muon_eta_max     = (fUseEmbedCuts) ? 2.2 : 2.4;
  const double tau_eta_max      = (fUseEmbedCuts) ? 2.2 : 2.3;
  if(std::abs(leptonOneFlavor) == 11 && std::fabs(leptonOneP4->Eta()) >= electron_eta_max) return kTRUE;
  if(std::abs(leptonTwoFlavor) == 11 && std::fabs(leptonTwoP4->Eta()) >= electron_eta_max) return kTRUE;
  if(std::abs(leptonOneFlavor) == 13 && std::fabs(leptonOneP4->Eta()) >= muon_eta_max    ) return kTRUE;
  if(std::abs(leptonTwoFlavor) == 13 && std::fabs(leptonTwoP4->Eta()) >= muon_eta_max    ) return kTRUE;
  if(std::abs(leptonOneFlavor) == 15 && std::fabs(leptonOneP4->Eta()) >= tau_eta_max     ) return kTRUE;
  if(std::abs(leptonTwoFlavor) == 15 && std::fabs(leptonTwoP4->Eta()) >= tau_eta_max     ) return kTRUE;

  if(std::fabs(leptonOneP4->DeltaR(*leptonTwoP4)) < 0.3) return kTRUE;

  fCutFlow->Fill(icutflow); ++icutflow; //7

  const double mll = (*leptonOneP4+*leptonTwoP4).M();
  if(mll <= 51. || mll >= 170.) return kTRUE;

  fCutFlow->Fill(icutflow); ++icutflow; //8

  if(!(leptonOneFired || leptonTwoFired)) return kTRUE;

  ee   &= !isLooseElectron && nElectrons == 2;
  mumu &= !isLooseMuon && nMuons == 2;
  mutau &= !isLooseMuon;
  etau  &= !isLooseElectron;
  emu &= !isLooseElectron; //QCD loose ID is only loose muon region

  //remove MC jet -> light lepton contribution
  if(!fUseMCEstimatedFakeLep && !fIsData) {
    emu   &= !isFakeMuon;
    emu   &= !isFakeElectron;
    mumu  &= !isFakeMuon;
    ee    &= !isFakeElectron;
    mutau &= !isFakeMuon;
    etau  &= !isFakeElectron;
  }

  //remove MC estimated jet --> tau component
  mutau &= fIsData > 0 || std::abs(tauGenFlavor) != 26;
  etau  &= fIsData > 0 || std::abs(tauGenFlavor) != 26;


  //reject electrons in the barrel/endcap gap region
  const float elec_gap_low(1.4442), elec_gap_high(1.566);
  etau &= elec_gap_low > std::fabs(leptonOneSCEta) || std::fabs(leptonOneSCEta) > elec_gap_high;
  emu  &= elec_gap_low > std::fabs(leptonOneSCEta) || std::fabs(leptonOneSCEta) > elec_gap_high;
  ee   &= elec_gap_low > std::fabs(leptonOneSCEta) || std::fabs(leptonOneSCEta) > elec_gap_high;
  ee   &= elec_gap_low > std::fabs(leptonTwoSCEta) || std::fabs(leptonTwoSCEta) > elec_gap_high;

  fCutFlow->Fill(icutflow); ++icutflow; //9

  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;

  //Tau ID selections
  mutau &= leptonTwoID2 >= 2;
  etau  &= tauDeepAntiEle >= 50;
  etau  &= leptonTwoID2 >= 2;

  mutau &= tauDecayMode != 5 && tauDecayMode != 6;
  etau  &= tauDecayMode != 5 && tauDecayMode != 6;

  fCutFlow->Fill(icutflow); ++icutflow; //10

  if(!(mutau || etau || emu || mumu || ee)) return kTRUE;

  fCutFlow->Fill(icutflow); ++icutflow; //11

  //b-jet veto
  if(emu && nBJets20L > 0) return kTRUE;
  if(nBJets20 > 0) return kTRUE;


  //MET cuts
  const double met_cut       = 60.;
  const double mtlep_cut     = 70.;

  fCutFlow->Fill(icutflow); ++icutflow; //12

  if(met >= met_cut) return kTRUE;
  if(fTreeVars.mtlep >= mtlep_cut) return kTRUE;

  ////////////////////////////////////////////////////////////
  // Set 8 + selection offset: preselection
  ////////////////////////////////////////////////////////////
  FillAllHistograms(set_offset + 8);

  fCutFlow->Fill(icutflow); ++icutflow; //13

  return kTRUE;
}
