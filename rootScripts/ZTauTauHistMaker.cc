#define ZTauTauHistMaker_cxx
// The class definition in ZTauTauHistMaker.h has been generated automatically
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
// root> T->Process("ZTauTauHistMaker.C")
// root> T->Process("ZTauTauHistMaker.C","some options")
// root> T->Process("ZTauTauHistMaker.C+")
//


#include "ZTauTauHistMaker.hh"
#include <TStyle.h>

void ZTauTauHistMaker::Begin(TTree * /*tree*/)
{
  // The Begin() function is called at the start of the query.
  // When running with PROOF Begin() is only called on the client.
  // The tree argument is deprecated (on PROOF 0 is passed).

  TString option = GetOption();
  printf("ZTauTauHistMaker::Begin\n");
  timer->Start();
  fChain = 0;
}

void ZTauTauHistMaker::SlaveBegin(TTree * /*tree*/)
{
  // The SlaveBegin() function is called after the Begin() function.
  // When running with PROOF SlaveBegin() is called on each slave server.
  // The tree argument is deprecated (on PROOF 0 is passed).

  printf("ZTauTauHistMaker::SlaveBegin\n");
  TString option = GetOption();

}

void ZTauTauHistMaker::FillAllHistograms(Int_t index) {
  if(fEventSets [index]) {
    FillEventHistogram( fEventHist [index]);
    FillPhotonHistogram(fPhotonHist[index]);
    FillLepHistogram(   fLepHist   [index]);
  } else
    printf("WARNING! Attempted to fill un-initialized histogram set %i!\n", index);
  if(fWriteTrees && fTreeSets[index])
    fTrees[index]->Fill();
}

void ZTauTauHistMaker::BookHistograms() {
  BookEventHistograms();
  BookPhotonHistograms();
  BookLepHistograms();
  if(fWriteTrees) BookTrees();
}

void ZTauTauHistMaker::BookEventHistograms() {
  for(int i = 0; i < fn; i++) {
    if(fEventSets[i] != 0) {
      char* dirname        = new char[20];
      sprintf(dirname,"event_%i",i);
      fDirectories[0*fn + i] = fTopDir->mkdir(dirname);
      fDirectories[0*fn + i]->cd();
      fEventHist[i] = new EventHist_t;
      fEventHist[i]->hLumiSection            = new TH1F("lumiSection"         , Form("%s: LumiSection"         ,dirname)  , 200,    0, 4e6);
      fEventHist[i]->hTriggerStatus          = new TH1F("triggerStatus"       , Form("%s: TriggerStatus"       ,dirname)  ,   3, -1.5, 1.5);
      fEventHist[i]->hEventWeight            = new TH1F("eventweight"         , Form("%s: EventWeight"         ,dirname)  , 100,   -5,   5);     
      fEventHist[i]->hGenWeight              = new TH1F("genweight"           , Form("%s: GenWeight"           ,dirname)  ,   5, -2.5, 2.5);     
      fEventHist[i]->hGenTauFlavorWeight     = new TH1F("gentauflavorweight"  , Form("%s: GenTauFlavorWeight"  ,dirname)  ,  40,    0,   2);     
      fEventHist[i]->hNPV                    = new TH1F("npv"                 , Form("%s: NPV"                 ,dirname)  , 200,  0, 200); 
      fEventHist[i]->hNPU                    = new TH1F("npu"                 , Form("%s: NPU"                 ,dirname)  , 100,  0, 100); 
      fEventHist[i]->hNPartons               = new TH1F("npartons"            , Form("%s: NPartons"            ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNMuons                 = new TH1F("nmuons"              , Form("%s: NMuons"              ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNElectrons             = new TH1F("nelectrons"          , Form("%s: NElectrons"          ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNTaus                  = new TH1F("ntaus"               , Form("%s: NTaus"               ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNPhotons               = new TH1F("nphotons"            , Form("%s: NPhotons"            ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNGenTausHad            = new TH1F("ngentaushad"         , Form("%s: NGenTausHad"         ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNGenTausLep            = new TH1F("ngentauslep"         , Form("%s: NGenTausLep"         ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNGenTaus               = new TH1F("ngentaus"            , Form("%s: NGenTaus"            ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNGenElectrons          = new TH1F("ngenelectrons"       , Form("%s: NGenElectrons"       ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNGenMuons              = new TH1F("ngenmuons"           , Form("%s: NGenMuons"           ,dirname)  ,  10,  0,  10); 
      fEventHist[i]->hNJets                  = new TH1F("njets"               , Form("%s: NJets"               ,dirname)  ,  50,  0,  50); 
      fEventHist[i]->hNFwdJets               = new TH1F("nfwdjets"            , Form("%s: NFwdJets"            ,dirname)  ,  50,  0,  50); 
      fEventHist[i]->hNBJets                 = new TH1F("nbjets"              , Form("%s: NBJets"              ,dirname)  ,  50,  0,  50);
      fEventHist[i]->hMcEra                  = new TH1F("mcera"               , Form("%s: McEra"               ,dirname) ,   5,   0,  5);
      fEventHist[i]->hTriggerLeptonStatus    = new TH1F("triggerleptonstatus" , Form("%s: TriggerLeptonStatus" ,dirname) ,   5,   0,  5);
      fEventHist[i]->hPuWeight		     = new TH1F("puweight"	      , Form("%s: PuWeight"	       ,dirname) , 200,   0,  2);
      fEventHist[i]->hTopPtWeight	     = new TH1F("topptweight"	      , Form("%s: TopPtWeight"	       ,dirname) , 200,   0,  2);
      fEventHist[i]->hTauDecayMode	     = new TH1F("taudecaymode"	      , Form("%s: TauDecayMode"	       ,dirname) ,  11,   0, 11);
      fEventHist[i]->hTauMVA		     = new TH1F("taumva"              , Form("%s: TauMVA"              ,dirname) , 100,   0,  1);
      fEventHist[i]->hTauGenFlavor	     = new TH1F("taugenflavor"	      , Form("%s: TauGenFlavor"	       ,dirname) ,  50,   0, 50);
      fEventHist[i]->hTauGenFlavorHad	     = new TH1F("taugenflavorhad"     , Form("%s: TauGenFlavorHad"     ,dirname) ,  50,   0, 50);
      fEventHist[i]->hTauVetoedJetPt	     = new TH1F("tauvetoedjetpt"      , Form("%s: TauVetoedJetPt"      ,dirname) , 210, -10,200);
      fEventHist[i]->hTauVetoedJetPtUnc	     = new TH1F("tauvetoedjetptunc"   , Form("%s: TauVetoedJetPtUnc"   ,dirname) , 110,  -1, 10);
      fEventHist[i]->hHtSum		     = new TH1F("htsum"		      , Form("%s: HtSum"	       ,dirname) , 200,   0,800);
      fEventHist[i]->hHt		     = new TH1F("ht"		      , Form("%s: Ht"		       ,dirname) , 200,   0,800);
      fEventHist[i]->hHtPhi                  = new TH1F("htphi"               , Form("%s: HtPhi"               ,dirname) , 100,  -4,  4);
      fEventHist[i]->hPFMet                  = new TH1F("pfmet"               , Form("%s: PF Met"              ,dirname)  , 200,  0, 400); 
      fEventHist[i]->hPFMetPhi               = new TH1F("pfmetphi"            , Form("%s: PF MetPhi"           ,dirname)  ,  80, -4,   4); 
      fEventHist[i]->hPFCovMet00             = new TH1F("pfcovmet00"          , Form("%s: PF CovMet00"         ,dirname) , 1000,    0.,1000.);
      fEventHist[i]->hPFCovMet01	     = new TH1F("pfcovmet01"	      , Form("%s: PF CovMet01"         ,dirname) , 1000,-1000.,1000.);
      fEventHist[i]->hPFCovMet11	     = new TH1F("pfcovmet11"	      , Form("%s: PF CovMet11"         ,dirname) , 1000,    0.,1000.);
      fEventHist[i]->hPuppMet                = new TH1F("puppmet"             , Form("%s: PUPPI Met"           ,dirname)  , 200,  0, 400); 
      fEventHist[i]->hPuppMetPhi             = new TH1F("puppmetphi"          , Form("%s: PUPPI MetPhi"        ,dirname)  ,  80, -4,   4); 
      fEventHist[i]->hPuppCovMet00           = new TH1F("puppcovmet00"        , Form("%s: PUPPI CovMet00"      ,dirname) , 1000,    0.,1000.);
      fEventHist[i]->hPuppCovMet01	     = new TH1F("puppcovmet01"	      , Form("%s: PUPPI CovMet01"      ,dirname) , 1000,-1000.,1000.);
      fEventHist[i]->hPuppCovMet11	     = new TH1F("puppcovmet11"	      , Form("%s: PUPPI CovMet11"      ,dirname) , 1000,    0.,1000.);
      fEventHist[i]->hTrkMet                 = new TH1F("trkmet"              , Form("%s: Trk Met"             ,dirname)  , 200,  0, 400); 
      fEventHist[i]->hTrkMetPhi              = new TH1F("trkmetphi"           , Form("%s: Trk MetPhi"          ,dirname)  ,  80, -4,   4); 
      fEventHist[i]->hMet                    = new TH1F("met"                 , Form("%s: Met"                 ,dirname)  , 200,  0, 400); 
      fEventHist[i]->hMetPhi                 = new TH1F("metphi"              , Form("%s: MetPhi"              ,dirname)  ,  80, -4,   4); 
      fEventHist[i]->hMetCorr                = new TH1F("metcorr"             , Form("%s: Met Correction"      ,dirname)  , 200,  0, 40); 
      fEventHist[i]->hMetCorrPhi             = new TH1F("metcorrphi"          , Form("%s: MetPhi Correction"   ,dirname)  ,  80, -4,   4); 
      fEventHist[i]->hCovMet00               = new TH1F("covmet00"            , Form("%s: CovMet00"            ,dirname) , 1000,    0.,1000.);
      fEventHist[i]->hCovMet01		     = new TH1F("covmet01"	      , Form("%s: CovMet01"            ,dirname) , 1000,-1000.,1000.);
      fEventHist[i]->hCovMet11		     = new TH1F("covmet11"	      , Form("%s: CovMet11"            ,dirname) , 1000,    0.,1000.);
      fEventHist[i]->hMetVsPt                = new TH2F("metvspt"             , Form("%s: MetVsPt"             ,dirname) ,200,0,400, 200,0,400); 
      fEventHist[i]->hMetVsM                 = new TH2F("metvsm"              , Form("%s: MetVsM"              ,dirname) ,200,0,400, 200,0,400); 
      fEventHist[i]->hMassSVFit		     = new TH1F("masssvfit"	      , Form("%s: MassSVFit"           ,dirname) , 1000,    0., 200.);
      fEventHist[i]->hMassErrSVFit	     = new TH1F("masserrsvfit"        , Form("%s: MassErrSVFit"        ,dirname) , 1000,    0., 100.);
      fEventHist[i]->hSVFitStatus            = new TH1F("svfitstatus"         , Form("%s: SVFitStatus"         ,dirname) ,   10,    0.,  10.);

							       
      fEventHist[i]->hLepPt         = new TH1F("leppt"         , Form("%s: Lepton Pt"      ,dirname)  , 200,   0, 400);
      fEventHist[i]->hLepP          = new TH1F("lepp"          , Form("%s: Lepton P"       ,dirname)  , 200,   0, 400);
      fEventHist[i]->hLepE          = new TH1F("lepe"          , Form("%s: Lepton E"       ,dirname)  , 200,   0, 400);
      fEventHist[i]->hLepM          = new TH1F("lepm"          , Form("%s: Lepton M"       ,dirname)  , 400,   0, 400);
      fEventHist[i]->hLepEta        = new TH1F("lepeta"        , Form("%s: Lepton Eta"     ,dirname)  , 200, -10,  10);
      fEventHist[i]->hLepPhi        = new TH1F("lepphi"        , Form("%s: Lepton Phi"     ,dirname)  ,  80,  -4,   4);
      fEventHist[i]->hLepDeltaPhi   = new TH1F("lepdeltaphi"   , Form("%s: Lepton DeltaPhi",dirname)  ,  40,   0,   4);
      fEventHist[i]->hLepDeltaEta   = new TH1F("lepdeltaeta"   , Form("%s: Lepton DeltaEta",dirname)  , 100,   0,   5);
      fEventHist[i]->hLepDeltaR     = new TH1F("lepdeltar"     , Form("%s: Lepton DeltaR"  ,dirname)  , 100,   0,   5);
      fEventHist[i]->hLepDelRVsPhi  = new TH2F("lepdelrvsphi"  , Form("%s: LepDelRVsPhi"   ,dirname)  ,  40,  0,   4, 100,  0,   5);     
      fEventHist[i]->hLepPtOverM    = new TH1F("lepptoverm"    , Form("%s: Lepton Pt / M"  ,dirname)  , 100,   0,  10);
      fEventHist[i]->hHtDeltaPhi    = new TH1F("htdeltaphi"    , Form("%s: Ht Lep Delta Phi",dirname) ,  40,   0,   4);
      fEventHist[i]->hMetDeltaPhi   = new TH1F("metdeltaphi"   , Form("%s: Met Lep Delta Phi",dirname),  40,   0,   4);
      fEventHist[i]->hLepOneDeltaPhi   = new TH1F("leponedeltaphi"   , Form("%s: Lep One vs Sys Delta Phi",dirname),  40,   0,   4);
      fEventHist[i]->hLepTwoDeltaPhi   = new TH1F("leptwodeltaphi"   , Form("%s: Lep Two vs Sys Delta Phi",dirname),  40,   0,   4);

      fEventHist[i]->hLepSVPt       = new TH1F("lepsvpt"       , Form("%s: Lepton SVFit Pt"      ,dirname)  , 200,   0, 400);
      fEventHist[i]->hLepSVP        = new TH1F("lepsvp"        , Form("%s: Lepton SVFit P"       ,dirname)  , 200,   0, 400);
      fEventHist[i]->hLepSVE        = new TH1F("lepsve"        , Form("%s: Lepton SVFit E"       ,dirname)  , 200,   0, 400);
      fEventHist[i]->hLepSVM        = new TH1F("lepsvm"        , Form("%s: Lepton SVFit M"       ,dirname)  , 400,   0, 400);
      fEventHist[i]->hLepSVEta      = new TH1F("lepsveta"      , Form("%s: Lepton SVFit Eta"     ,dirname)  , 200, -10,  10);
      fEventHist[i]->hLepSVPhi      = new TH1F("lepsvphi"      , Form("%s: Lepton SVFit Phi"     ,dirname)  ,  80,  -4,   4);
      fEventHist[i]->hLepSVDeltaPhi = new TH1F("lepsvdeltaphi" , Form("%s: Lepton SVFit DeltaPhi",dirname)  ,  40,   0,   4);
      fEventHist[i]->hLepSVDeltaEta = new TH1F("lepsvdeltaeta" , Form("%s: Lepton SVFit DeltaEta",dirname)  , 100,   0,   5);
      fEventHist[i]->hLepSVDeltaR   = new TH1F("lepsvdeltar"   , Form("%s: Lepton SVFit DeltaR"  ,dirname)  , 100,   0,   5);
      fEventHist[i]->hLepSVDeltaM   = new TH1F("lepsvdeltam"   , Form("%s: Lepton SVFit DeltaM"  ,dirname)  , 200, -10,  90);
      fEventHist[i]->hLepSVDeltaPt  = new TH1F("lepsvdeltapt"  , Form("%s: Lepton SVFit DeltaPt" ,dirname)  , 200, -10,  90);
      fEventHist[i]->hLepSVDelRVsPhi= new TH2F("lepsvdelrvsphi", Form("%s: LepSVDelRVsPhi"       ,dirname)  ,  40,  0,   4, 100,  0,   5);     
      fEventHist[i]->hLepSVPtOverM  = new TH1F("lepsvptoverm"  , Form("%s: Lepton SVFit Pt / M"  ,dirname)  , 100,   0,  10);
      
      fEventHist[i]->hSysM          = new TH1F("sysm"          , Form("%s: SysM"          ,dirname)  ,1000,  0, 1e3);     
      fEventHist[i]->hSysPt         = new TH1F("syspt"         , Form("%s: SysPt"         ,dirname)  , 200,  0, 400);     
      fEventHist[i]->hSysEta        = new TH1F("syseta"        , Form("%s: SysEta"        ,dirname)  , 100, -5,   5);     

      fEventHist[i]->hSysSVM        = new TH1F("syssvm"        , Form("%s: SVFit SysM"            ,dirname)  ,1000,  0, 1e3);     
      fEventHist[i]->hSysSVPt       = new TH1F("syssvpt"       , Form("%s: SVFit SysPt"           ,dirname)  , 200,  0, 400);     
      fEventHist[i]->hSysSVEta      = new TH1F("syssveta"      , Form("%s: SVFit SysEta"          ,dirname)  , 100, -5,   5);     
      
      fEventHist[i]->hMTMu          = new TH1F("mtmu"          , Form("%s: MT_Mu"         ,dirname)  , 200, 0.,   200.);     
      fEventHist[i]->hMTE           = new TH1F("mte"           , Form("%s: MT_E"          ,dirname)  , 200, 0.,   200.);     
      fEventHist[i]->hMTTau         = new TH1F("mttau"         , Form("%s: MT_Tau"        ,dirname)  , 200, 0.,   200.);     

      fEventHist[i]->hMDiff         = new TH1F("mdiff"         , Form("%s: 0.85M_gll - M_ll" ,dirname)  , 100, -50.,   50.);     

      fEventHist[i]->hPXiVis[0]        = new TH1F("pxivis0"        , Form("%s: PXiVis     "     ,dirname)  ,1000,  -100.,  900.);      
      fEventHist[i]->hPXiInv[0]        = new TH1F("pxiinv0"        , Form("%s: PXiInv     "     ,dirname)  ,1000,-500.,   500.);     
      fEventHist[i]->hPXiVisOverInv[0] = new TH1F("pxivisoverinv0" , Form("%s: PXiVisOverInv"   ,dirname)  ,1000, -10.,    10.);     
      fEventHist[i]->hPXiInvVsVis[0]   = new TH2F("pxiinvvsvis0"   , Form("%s: PXiInv vs PXiVis",dirname)  , 800, -100., 700., 1000,-500.,   500.);     
      fEventHist[i]->hPXiDiff[0]       = new TH1F("pxidiff0"       , Form("%s: PXiVis - PXiInv" ,dirname)  ,2000,-500.,  1500.);     
      fEventHist[i]->hPXiDiff2[0]      = new TH1F("pxidiff20"      , Form("%s: a*PXiVis + b - PXiInv" ,dirname)  ,2000,-500.,  1500.);     
      fEventHist[i]->hPXiVis[1]        = new TH1F("pxivis1"        , Form("%s: PXiVis     "     ,dirname)  ,1000,  -100.,  900.);      
      fEventHist[i]->hPXiInv[1]        = new TH1F("pxiinv1"        , Form("%s: PXiInv     "     ,dirname)  ,1000,-500.,   500.);     
      fEventHist[i]->hPXiVisOverInv[1] = new TH1F("pxivisoverinv1" , Form("%s: PXiVisOverInv"   ,dirname)  ,1000, -10.,    10.);     
      fEventHist[i]->hPXiInvVsVis[1]   = new TH2F("pxiinvvsvis1"   , Form("%s: PXiInv vs PXiVis",dirname)  , 800, -100., 700., 1000,-500.,   500.);     
      fEventHist[i]->hPXiDiff[1]       = new TH1F("pxidiff1"       , Form("%s: PXiVis - PXiInv" ,dirname)  ,2000,-500.,  1500.);     
      fEventHist[i]->hPXiDiff2[1]      = new TH1F("pxidiff21"      , Form("%s: a*PXiVis + b - PXiInv" ,dirname)  ,2000,-500.,  1500.);     
      fEventHist[i]->hPXiVis[2]        = new TH1F("pxivis2"        , Form("%s: PXiVis     "     ,dirname)  ,1000,  -100.,  900.);      
      fEventHist[i]->hPXiInv[2]        = new TH1F("pxiinv2"        , Form("%s: PXiInv     "     ,dirname)  ,1000,-500.,   500.);     
      fEventHist[i]->hPXiVisOverInv[2] = new TH1F("pxivisoverinv2" , Form("%s: PXiVisOverInv"   ,dirname)  ,1000, -10.,    10.);     
      fEventHist[i]->hPXiInvVsVis[2]   = new TH2F("pxiinvvsvis2"   , Form("%s: PXiInv vs PXiVis",dirname)  , 800, -100., 700., 1000,-500.,   500.);     
      fEventHist[i]->hPXiDiff[2]       = new TH1F("pxidiff2"       , Form("%s: PXiVis - PXiInv" ,dirname)  ,2000,-500.,  1500.);     
      fEventHist[i]->hPXiDiff2[2]      = new TH1F("pxidiff22"      , Form("%s: a*PXiVis + b - PXiInv" ,dirname)  ,2000,-500.,  1500.);     

      fEventHist[i]->hPTauVisFrac      = new TH1F("ptauvisfrac"    , Form("%s: visible tau pT fraction " ,dirname)  ,300,0.,  1.5);     
      fEventHist[i]->hLepMEstimate     = new TH1F("lepmestimate"   , Form("%s: Estimate di-lepton mass"  ,dirname)  ,200,0.,  800.);     

      fEventHist[i]->hPtSum[0]         = new TH1F("ptsum0"         , Form("%s: Scalar Pt sum"                    ,dirname)    ,1000,  0.,  1000.);     
      fEventHist[i]->hPtSum[1]         = new TH1F("ptsum1"         , Form("%s: Scalar Pt sum"                    ,dirname)    ,1000,  0.,  1000.);     
      fEventHist[i]->hPt1Sum[0]        = new TH1F("pt1sum0"        , Form("%s: Scalar Pt sum Lepton 1 + MET"     ,dirname)    ,1000,  0.,  1000.);     
      fEventHist[i]->hPt1Sum[1]        = new TH1F("pt1sum1"        , Form("%s: Scalar Pt sum Lepton 2 + MET"     ,dirname)    ,1000,  0.,  1000.);     
      fEventHist[i]->hPt1Sum[2]        = new TH1F("pt1sum2"        , Form("%s: Scalar Pt sum Lepton 1 + 2"       ,dirname)    ,1000,  0.,  1000.);     
      fEventHist[i]->hPt1Sum[3]        = new TH1F("pt1sum3"        , Form("%s: Scalar Pt sum Lepton 1 + 2 - MET" ,dirname)    ,1000,  0.,  1000.);     
      for(unsigned j = 0; j < fMvaNames.size(); ++j)  {
	//high mva score binning to improve cdf making
	fEventHist[i]->hMVA[j]        = new TH1F(Form("mva%i",j)   , Form("%s: %s MVA" ,dirname, fMvaNames[j].Data()) ,10000, -1.,  2.); //beyond 1 for space for legend
	fEventHist[i]->hProb[j]       = new TH1F(Form("prob%i",j)   , Form("%s: %s MVA" ,dirname, fMvaNames[j].Data()) ,300, -1.,  2.); //beyond 1 for space for legend
	fEventHist[i]->hRarity[j]     = new TH1F(Form("rarity%i",j)   , Form("%s: %s MVA" ,dirname, fMvaNames[j].Data()) ,300, -1.,  2.); //beyond 1 for space for legend
	fEventHist[i]->hCdf[j]        = new TH1F(Form("cdf%i",j)    , Form("%s: %s MVA" ,dirname, fMvaNames[j].Data()) ,300, -1.,  2.); //beyond 1 for space for legend
      }      
    }
  }
}

void ZTauTauHistMaker::BookPhotonHistograms() {
  for(int i = 0; i < fn; i++) {
    if(fEventSets[i] > 0) {
      char* dirname        = new char[20];
      sprintf(dirname,"photon_%i",i);
      fDirectories[1*fn + i] = fTopDir->mkdir(dirname);
      fDirectories[1*fn + i]->cd();
      fPhotonHist[i] = new PhotonHist_t;
      fPhotonHist[i]->hPx        = new TH1F("px"      , Form("%s: Px"      ,dirname)  , 200,-1e3, 1e3);
      fPhotonHist[i]->hPy        = new TH1F("py"      , Form("%s: Py"      ,dirname)  , 200,-1e3, 1e3);
      fPhotonHist[i]->hPz        = new TH1F("pz"      , Form("%s: Pz"      ,dirname)  , 200,-1e3, 1e3);
      fPhotonHist[i]->hPt        = new TH1F("pt"      , Form("%s: Pt"      ,dirname)  , 500,   0, 1e3);
      fPhotonHist[i]->hP         = new TH1F("p"       , Form("%s: P"       ,dirname)  , 500,   0, 1e3);
      fPhotonHist[i]->hE         = new TH1F("e"       , Form("%s: E"       ,dirname)  , 200,   0, 1e3);
      fPhotonHist[i]->hEta       = new TH1F("eta"     , Form("%s: Eta"     ,dirname)  , 200, -10,  10);
      fPhotonHist[i]->hPhi       = new TH1F("phi"     , Form("%s: Phi"     ,dirname)  ,  80,  -4,   4);
      fPhotonHist[i]->hIso       = new TH1F("iso"     , Form("%s: Iso"     ,dirname)  , 200,   0,  10);
      fPhotonHist[i]->hRelIso    = new TH1F("reliso"  , Form("%s: Iso / Pt",dirname)  , 200,   0,   1);
      fPhotonHist[i]->hTrigger   = new TH1F("trigger" , Form("%s: Trigger" ,dirname)  ,  10,   0,  10);
    }
  }
}

void ZTauTauHistMaker::BookLepHistograms() {
  for(int i = 0; i < fn; i++) {
    if(fEventSets[i] > 0) {
      char* dirname        = new char[20];
      sprintf(dirname,"lep_%i",i);
      fDirectories[2*fn + i] = fTopDir->mkdir(dirname);
      fDirectories[2*fn + i]->cd();
      fLepHist[i] = new LepHist_t;
      fLepHist[i]->hOnePx        = new TH1F("onepx"      , Form("%s: Px"      ,dirname)  , 200,-1e3, 1e3);
      fLepHist[i]->hOnePy        = new TH1F("onepy"      , Form("%s: Py"      ,dirname)  , 200,-1e3, 1e3);
      fLepHist[i]->hOnePz        = new TH1F("onepz"      , Form("%s: Pz"      ,dirname)  , 200,-1e3, 1e3);
      fLepHist[i]->hOnePt        = new TH1F("onept"      , Form("%s: Pt"      ,dirname)  , 200,   0, 200);
      fLepHist[i]->hOneP         = new TH1F("onep"       , Form("%s: P"       ,dirname)  , 200,   0, 1e3);
      fLepHist[i]->hOneE         = new TH1F("onee"       , Form("%s: E"       ,dirname)  , 200,   0, 1e3);
      fLepHist[i]->hOneM         = new TH1F("onem"       , Form("%s: M"       ,dirname)  , 200,   0, 1e1);
      fLepHist[i]->hOneEta       = new TH1F("oneeta"     , Form("%s: Eta"     ,dirname)  , 200, -10,  10);
      fLepHist[i]->hOnePhi       = new TH1F("onephi"     , Form("%s: Phi"     ,dirname)  ,  80,  -4,   4);
      fLepHist[i]->hOneD0        = new TH1F("oned0"      , Form("%s: D0"      ,dirname)  , 200,-0.2, 0.2);
      fLepHist[i]->hOneIso       = new TH1F("oneiso"     , Form("%s: Iso"     ,dirname)  , 200,   0,  10);
      fLepHist[i]->hOneRelIso    = new TH1F("onereliso"  , Form("%s: Iso / Pt",dirname)  , 200,   0,   1);
      fLepHist[i]->hOneFlavor    = new TH1F("oneflavor"  , Form("%s: Flavor"  ,dirname)  ,  20,   0,  20);
      fLepHist[i]->hOneQ         = new TH1F("oneq"       , Form("%s: Q"       ,dirname)  ,   5,  -2,   2);
      fLepHist[i]->hOneTrigger   = new TH1F("onetrigger" , Form("%s: Trigger" ,dirname)  ,  10,   0,  10);
      //Gen Info
      fLepHist[i]->hOneGenPt     = new TH1F("onegenpt"   , Form("%s: Gen Pt"   ,dirname)  , 200,   0, 200);
      fLepHist[i]->hOneGenE      = new TH1F("onegene"    , Form("%s: Gen E"    ,dirname)  , 200,   0, 1e3);
      fLepHist[i]->hOneGenEta    = new TH1F("onegeneta"  , Form("%s: Gen Eta"  ,dirname)  , 200, -10,  10);
      fLepHist[i]->hOneDeltaPt   = new TH1F("onedeltapt" , Form("%s: Gen Delta Pt"   ,dirname)  , 200,-100, 100);
      fLepHist[i]->hOneDeltaE    = new TH1F("onedeltae"  , Form("%s: Gen Delta E"    ,dirname)  , 200,-500, 500);
      fLepHist[i]->hOneDeltaEta  = new TH1F("onedeltaeta", Form("%s: Gen Delta Eta"  ,dirname)  , 200, -10., 10.);
      fLepHist[i]->hOneMetDeltaPhi  = new TH1F("onemetdeltaphi"   , Form("%s: Met Delta Phi",dirname),  40,   0,   4);
      //SVFit Info
      fLepHist[i]->hOneSVPt      = new TH1F("onesvpt"    , Form("%s: SV Pt"   ,dirname)  , 200,   0, 200);
      fLepHist[i]->hOneSVP       = new TH1F("onesvp"     , Form("%s: SV P"    ,dirname)  , 200,   0, 1e3);
      fLepHist[i]->hOneSVE       = new TH1F("onesve"     , Form("%s: SV E"    ,dirname)  , 200,   0, 1e3);
      fLepHist[i]->hOneSVM       = new TH1F("onesvm"     , Form("%s: SV M"    ,dirname)  , 200,   0, 1e1);
      fLepHist[i]->hOneSVEta     = new TH1F("onesveta"   , Form("%s: SV Eta"  ,dirname)  , 200, -10., 10.);
      fLepHist[i]->hOneSVDeltaPt = new TH1F("onesvdeltapt"    , Form("%s: SV Delta Pt"   ,dirname)  , 200,-100, 100);
      fLepHist[i]->hOneSVDeltaP  = new TH1F("onesvdeltap"     , Form("%s: SV Delta P"    ,dirname)  , 200,-500, 500);
      fLepHist[i]->hOneSVDeltaE  = new TH1F("onesvdeltae"     , Form("%s: SV Delta E"    ,dirname)  , 200,-500, 500);
      fLepHist[i]->hOneSVDeltaEta= new TH1F("onesvdeltaeta"   , Form("%s: SV Delta Eta"  ,dirname)  , 200, -10., 10.);
      fLepHist[i]->hTwoPx        = new TH1F("twopx"      , Form("%s: Px"      ,dirname)  , 200,-1e3, 1e3);
      fLepHist[i]->hTwoPy        = new TH1F("twopy"      , Form("%s: Py"      ,dirname)  , 200,-1e3, 1e3);
      fLepHist[i]->hTwoPz        = new TH1F("twopz"      , Form("%s: Pz"      ,dirname)  , 200,-1e3, 1e3);
      fLepHist[i]->hTwoPt        = new TH1F("twopt"      , Form("%s: Pt"      ,dirname)  , 200,   0, 200);
      fLepHist[i]->hTwoP         = new TH1F("twop"       , Form("%s: P"       ,dirname)  , 200,   0, 1e3);
      fLepHist[i]->hTwoE         = new TH1F("twoe"       , Form("%s: E"       ,dirname)  , 200,   0, 1e3);
      fLepHist[i]->hTwoM         = new TH1F("twom"       , Form("%s: M"       ,dirname)  , 200,   0, 1e1);
      fLepHist[i]->hTwoEta       = new TH1F("twoeta"     , Form("%s: Eta"     ,dirname)  , 200, -10,  10);
      fLepHist[i]->hTwoPhi       = new TH1F("twophi"     , Form("%s: Phi"     ,dirname)  ,  80,  -4,   4);
      fLepHist[i]->hTwoD0        = new TH1F("twod0"      , Form("%s: D0"      ,dirname)  , 200,-0.2, 0.2);
      fLepHist[i]->hTwoIso       = new TH1F("twoiso"     , Form("%s: Iso"     ,dirname)  , 200,   0,  20);
      fLepHist[i]->hTwoRelIso    = new TH1F("tworeliso"  , Form("%s: Iso / Pt",dirname)  , 200,   0,   1);
      fLepHist[i]->hTwoFlavor    = new TH1F("twoflavor"  , Form("%s: Flavor"  ,dirname)  ,  20,   0,  20);
      fLepHist[i]->hTwoQ         = new TH1F("twoq"       , Form("%s: Q"       ,dirname)  ,   5,  -2,   2);
      fLepHist[i]->hTwoTrigger   = new TH1F("twotrigger" , Form("%s: Trigger" ,dirname)  ,  10,   0,  10);
      //Gen Info
      fLepHist[i]->hTwoGenPt     = new TH1F("twogenpt"   , Form("%s: Gen Pt"   ,dirname)  , 200,   0, 200);
      fLepHist[i]->hTwoGenE      = new TH1F("twogene"    , Form("%s: Gen E"    ,dirname)  , 200,   0, 1e3);
      fLepHist[i]->hTwoGenEta    = new TH1F("twogeneta"  , Form("%s: Gen Eta"  ,dirname)  , 200, -10,  10);
      fLepHist[i]->hTwoDeltaPt   = new TH1F("twodeltapt" , Form("%s: Gen Delta Pt"   ,dirname)  , 200,-100, 100);
      fLepHist[i]->hTwoDeltaE    = new TH1F("twodeltae"  , Form("%s: Gen Delta E"    ,dirname)  , 200,-500, 500);
      fLepHist[i]->hTwoDeltaEta  = new TH1F("twodeltaeta", Form("%s: Gen Delta Eta"  ,dirname)  , 200, -10., 10.);
      fLepHist[i]->hTwoMetDeltaPhi  = new TH1F("twometdeltaphi"   , Form("%s: Met Delta Phi",dirname),  40,   0,   4);
      //SVFit Info
      fLepHist[i]->hTwoSVPt      = new TH1F("twosvpt"    , Form("%s: SV Pt"   ,dirname)  , 200,   0, 200);
      fLepHist[i]->hTwoSVP       = new TH1F("twosvp"     , Form("%s: SV P"    ,dirname)  , 200,   0, 1e3);
      fLepHist[i]->hTwoSVE       = new TH1F("twosve"     , Form("%s: SV E"    ,dirname)  , 200,   0, 1e3);
      fLepHist[i]->hTwoSVM       = new TH1F("twosvm"     , Form("%s: SV M"    ,dirname)  , 200,   0, 1e1);
      fLepHist[i]->hTwoSVEta     = new TH1F("twosveta"   , Form("%s: SV Eta"  ,dirname)  , 200, -10., 10.);
      fLepHist[i]->hTwoSVDeltaPt      = new TH1F("twosvdeltapt"    , Form("%s: SV Delta Pt"   ,dirname)  , 200,-100, 100);
      fLepHist[i]->hTwoSVDeltaP       = new TH1F("twosvdeltap"     , Form("%s: SV Delta P"    ,dirname)  , 200,-500, 500);
      fLepHist[i]->hTwoSVDeltaE       = new TH1F("twosvdeltae"     , Form("%s: SV Delta E"    ,dirname)  , 200,-500, 500);
      fLepHist[i]->hTwoSVDeltaEta     = new TH1F("twosvdeltaeta"   , Form("%s: SV Delta Eta"  ,dirname)  , 200, -10., 10.);

      //2D distributions
      fLepHist[i]->hTwoPtVsOnePt      = new TH2F("twoptvsonept", Form("%s: Two pT vs One pT", dirname), 200, 0, 200, 200, 0, 200);
    }
  }
}

void ZTauTauHistMaker::BookTrees() {
  for(int i = 0; i < fn; ++i) {
    if(fTreeSets[i] != 0) {
      char* dirname        = new char[20];
      sprintf(dirname,"tree_%i",i);
      fDirectories[3*fn + i] = fTopDir->mkdir(dirname);
      fDirectories[3*fn + i]->cd();
      fTrees[i] = new TTree(Form("tree_%i",i),Form("ZTauTauHistMaker TTree %i",i));
      fTrees[i]->Branch("leponept",        &fTreeVars.leponept       );   
      fTrees[i]->Branch("leponem",         &fTreeVars.leponem	     );   
      fTrees[i]->Branch("leponeeta",       &fTreeVars.leponeeta      );  
      fTrees[i]->Branch("leponed0",        &fTreeVars.leponed0       );  
      fTrees[i]->Branch("leponeiso",       &fTreeVars.leponeiso      );  
      fTrees[i]->Branch("leptwopt",        &fTreeVars.leptwopt       );  
      fTrees[i]->Branch("leptwom",         &fTreeVars.leptwom	     );   
      fTrees[i]->Branch("leptwoeta",       &fTreeVars.leptwoeta      );  
      fTrees[i]->Branch("leptwod0",        &fTreeVars.leptwod0       );  
      fTrees[i]->Branch("leptwoiso",       &fTreeVars.leptwoiso      );  
      fTrees[i]->Branch("lepp",            &fTreeVars.lepp	     );   
      fTrees[i]->Branch("leppt",           &fTreeVars.leppt	     );   
      fTrees[i]->Branch("lepm",            &fTreeVars.lepm	     );   
      fTrees[i]->Branch("lepeta",          &fTreeVars.lepeta	     );   
      fTrees[i]->Branch("lepdeltaeta",     &fTreeVars.lepdeltaeta    );  
      fTrees[i]->Branch("lepdeltar",       &fTreeVars.lepdeltar      );  
      fTrees[i]->Branch("lepdeltaphi",     &fTreeVars.lepdeltaphi    );  
      fTrees[i]->Branch("htdeltaphi",      &fTreeVars.htdeltaphi     );  
      fTrees[i]->Branch("metdeltaphi",     &fTreeVars.metdeltaphi    );  
      fTrees[i]->Branch("leponedeltaphi",  &fTreeVars.leponedeltaphi );  
      fTrees[i]->Branch("leptwodeltaphi",  &fTreeVars.leptwodeltaphi );  
      fTrees[i]->Branch("onemetdeltaphi",  &fTreeVars.onemetdeltaphi );  
      fTrees[i]->Branch("twometdeltaphi",  &fTreeVars.twometdeltaphi );  
      fTrees[i]->Branch("lepmestimate",    &fTreeVars.mestimate	     );   
      fTrees[i]->Branch("met",             &fTreeVars.met            );
      fTrees[i]->Branch("mtone",           &fTreeVars.mtone	     );   
      fTrees[i]->Branch("mttwo",           &fTreeVars.mttwo	     );   
      fTrees[i]->Branch("pxivis",          &fTreeVars.pxivis	     );   
      fTrees[i]->Branch("pxiinv",          &fTreeVars.pxiinv	     );   
      fTrees[i]->Branch("ht",              &fTreeVars.ht	     );   
      fTrees[i]->Branch("htsum",           &fTreeVars.htsum	     );   
      fTrees[i]->Branch("njets",           &fTreeVars.njets	     );   
      fTrees[i]->Branch("nbjets",          &fTreeVars.nbjets	     );   
      fTrees[i]->Branch("nphotons",        &fTreeVars.nphotons       );  
      fTrees[i]->Branch("eventweight",     &fTreeVars.eventweight    );  
      fTrees[i]->Branch("fulleventweight", &fTreeVars.fulleventweight);
      fTrees[i]->Branch("eventcategory",   &fTreeVars.eventcategory  );
    }
  }
}

//selections: 1 = mutau, 2 = etau, 5 = emu
void ZTauTauHistMaker::InitializeTreeVariables(Int_t selection) {

  fTreeVars.leponept  = leptonOneP4->Pt();
  fTreeVars.leponem   = leptonOneP4->M();
  fTreeVars.leponeeta = leptonOneP4->Eta();
  fTreeVars.leponed0  = leptonOneD0;
  fTreeVars.leponeiso = leptonOneIso;
  fTreeVars.leptwopt  = leptonTwoP4->Pt();
  fTreeVars.leptwom   = leptonTwoP4->M();
  fTreeVars.leptwoeta = leptonTwoP4->Eta();
  fTreeVars.leptwod0  = leptonTwoD0;
  fTreeVars.leptwoiso = leptonTwoIso;
  TLorentzVector lep = *leptonOneP4 + *leptonTwoP4;
  fTreeVars.lepp   = lep.P();
  fTreeVars.leppt  = lep.Pt();
  fTreeVars.lepm   = lep.M();
  fTreeVars.lepeta = lep.Eta();
  fTreeVars.lepdeltar   = leptonOneP4->DeltaR(*leptonTwoP4);
  fTreeVars.lepdeltaphi = abs(leptonOneP4->DeltaPhi(*leptonTwoP4));
  fTreeVars.lepdeltaeta = abs(leptonOneP4->Eta() - leptonTwoP4->Eta());

  //phi differences
  fTreeVars.htdeltaphi = abs(lep.Phi() - htPhi);
  if(fTreeVars.htdeltaphi > M_PI)
    fTreeVars.htdeltaphi = abs(2.*M_PI - fTreeVars.htdeltaphi);
  fTreeVars.metdeltaphi = abs(lep.Phi() - metPhi);
  if(fTreeVars.metdeltaphi > M_PI)
    fTreeVars.metdeltaphi = abs(2.*M_PI - fTreeVars.metdeltaphi);
  fTreeVars.leponedeltaphi = abs(leptonOneP4->Phi() - lep.Phi());
  if(fTreeVars.leponedeltaphi > M_PI)
    fTreeVars.leponedeltaphi = abs(2.*M_PI - fTreeVars.leponedeltaphi);
  fTreeVars.leptwodeltaphi = abs(leptonTwoP4->Phi() - lep.Phi());
  if(fTreeVars.leptwodeltaphi > M_PI)
    fTreeVars.leptwodeltaphi = abs(2.*M_PI - fTreeVars.leptwodeltaphi);
  fTreeVars.onemetdeltaphi = abs(leptonOneP4->Phi() - metPhi);
  if(fTreeVars.onemetdeltaphi > M_PI)
    fTreeVars.onemetdeltaphi = abs(2.*M_PI - fTreeVars.onemetdeltaphi);
  fTreeVars.twometdeltaphi = abs(leptonTwoP4->Phi() - metPhi);
  if(fTreeVars.twometdeltaphi > M_PI)
    fTreeVars.twometdeltaphi = abs(2.*M_PI - fTreeVars.twometdeltaphi);

  //MET variables
  fTreeVars.met = met;
  fTreeVars.mtone = sqrt(2.*met*leptonOneP4->Pt()*(1.-cos(leptonOneP4->Phi() - metPhi)));
  fTreeVars.mttwo = sqrt(2.*met*leptonTwoP4->Pt()*(1.-cos(leptonTwoP4->Phi() - metPhi)));

  //momentum projections onto bisector
  TVector3 lp1 = leptonOneP4->Vect();
  TVector3 lp2 = leptonTwoP4->Vect();
  TVector3 missing(met*cos(metPhi), met*sin(metPhi), 0.);
  lp1.SetZ(0.);
  lp2.SetZ(0.);
  TVector3 bisector = (lp1.Mag()*lp2 + lp2.Mag()*lp1); //divides leptons
  if(bisector.Mag() > 0.) bisector.SetMag(1.);
  
  //project onto the bisectors
  fTreeVars.pxivis = (lp1+lp2)*bisector;
  fTreeVars.pxiinv = missing*bisector;
  double pnuest = max(0.,lp2*missing/lp2.Mag()); //inv pT along tau = tau pt unit vector dot missing
  fTreeVars.ptauvisfrac = lp2.Mag() / (lp2.Mag() + pnuest);
  fTreeVars.mestimate = fTreeVars.lepm/sqrt(fTreeVars.ptauvisfrac);
  
  //event variables
  fTreeVars.ht       = ht;
  fTreeVars.htsum    = htSum;
  fTreeVars.njets    = nJets;
  fTreeVars.nbjets   = nBJets;
  fTreeVars.nphotons = nPhotons;
  fTreeVars.eventweight = genWeight*eventWeight;
  fTreeVars.fulleventweight = genWeight*eventWeight*fXsec;
  if(fUseTauFakeSF) fTreeVars.fulleventweight *= genTauFlavorWeight;
  
  fTreeVars.eventcategory = fEventCategory;
  TString selecName = "";
  if(selection == 1)      selecName = "mutau";
  else if(selection == 2) selecName = "etau";
  else if(selection == 5) selecName = "emu";
  else                    selecName = "unknown";
  for(unsigned i = 0; i < fMvaNames.size(); ++i) {
    fMvaOutputs[i] = (fMvaNames[i].Contains(selecName.Data())) ? mva->EvaluateMVA(fMvaNames[i].Data()) : -2.;
    if(fMvaOutputs[i] < -100.)
      cout << "Error value returned for MVA " << fMvaNames[i].Data()
	   << " evaluation, Entry = " << fentry << endl;
    fMvaProb[i] = -1; //(fMvaNames[i].Contains(selecName.Data())) ? mva->GetProba(fMvaNames[i].Data()) : -1.;
    // probability ~e^(m*x) from 0-1 --> CDF = ~1/m*(e^(m*x)-1)
    // N/m*(e^(m)-1) = 1 --> N = m/(e^m-1)
    fMvaCdf[i] = (fMvaProb[i] > -1.) ? (exp(fMvaProb[i]*fMvaProbSlope[i])-1.)/(exp(fMvaProbSlope[i])-1.) : -1.;
    fMvaRarity[i] = -1.; //mva->GetRarity(fMvaNames[i].Data());
  }

}

float ZTauTauHistMaker::GetTauFakeSF(int genFlavor) {
  float weight = 1.;
  switch(abs(genFlavor)) {
  case 15 : weight = 0.95; break;
  case 11 : weight = genTauFlavorWeight; break;
  case 13 : weight = genTauFlavorWeight; break;
  }
  //jets are given weight 1
  return weight;
}

float ZTauTauHistMaker::ReweightMET(int selection, double met) {
  float weight = 1.;
  
  switch(abs(selection)) {
  case 0 : weight = (1.1 - 0.2/150.*met); break; // mu+tau
  case 1 : weight = 1.; break; // e+tau
  case 2 : weight = 1.; break; // e+mu
  }
  return weight;
}

void ZTauTauHistMaker::FillEventHistogram(EventHist_t* Hist) {
  Hist->hLumiSection         ->Fill(lumiSection        , genWeight*eventWeight)      ;
  // Hist->hTriggerStatus       ->Fill(triggerStatus      , genWeight*eventWeight)      ;
  Hist->hEventWeight         ->Fill(eventWeight        );
  Hist->hGenWeight           ->Fill(genWeight          );
  Hist->hGenTauFlavorWeight  ->Fill(genTauFlavorWeight );
  Hist->hNPV                 ->Fill(nPV                , genWeight*eventWeight)      ;
  Hist->hNPU                 ->Fill(nPU                , genWeight*eventWeight)      ;
  // Hist->hNPartons            ->Fill(nPartons           , genWeight*eventWeight)      ;
  Hist->hNMuons              ->Fill(nMuons             , genWeight*eventWeight)      ;
  Hist->hNElectrons          ->Fill(nElectrons         , genWeight*eventWeight)      ;
  Hist->hNTaus               ->Fill(nTaus              , genWeight*eventWeight)      ;
  Hist->hNPhotons            ->Fill(nPhotons           , genWeight*eventWeight)      ;
  Hist->hNGenTausHad         ->Fill(nGenTausHad        , genWeight*eventWeight)      ;
  Hist->hNGenTausLep         ->Fill(nGenTausLep        , genWeight*eventWeight)      ;
  Hist->hNGenTaus            ->Fill(nGenTausLep+nGenTausHad, genWeight*eventWeight)      ;
  Hist->hNGenElectrons       ->Fill(nGenElectrons      , genWeight*eventWeight)      ;
  Hist->hNGenMuons           ->Fill(nGenMuons          , genWeight*eventWeight)      ;
  Hist->hNJets               ->Fill(nJets              , genWeight*eventWeight)      ;
  Hist->hNFwdJets            ->Fill(nFwdJets           , genWeight*eventWeight)      ;
  Hist->hNBJets              ->Fill(nBJets             , genWeight*eventWeight)      ;
  Hist->hMcEra               ->Fill(mcEra              , genWeight*eventWeight)   ;
  Hist->hTriggerLeptonStatus ->Fill(triggerLeptonStatus, genWeight*eventWeight)   ;
  Hist->hPuWeight	     ->Fill(puWeight	       , genWeight*eventWeight)   ;
  Hist->hTopPtWeight	     ->Fill(topPtWeight	       , genWeight*eventWeight)   ;
  Hist->hTauDecayMode	     ->Fill(tauDecayMode       , genWeight*eventWeight)   ;
  Hist->hTauMVA		     ->Fill(tauMVA	       , genWeight*eventWeight)   ;
  Hist->hTauGenFlavor	     ->Fill(tauGenFlavor       , genWeight*eventWeight)   ;
  Hist->hTauGenFlavorHad     ->Fill(tauGenFlavorHad    , genWeight*eventWeight)   ;
  Hist->hTauVetoedJetPt	     ->Fill(tauVetoedJetPt     , genWeight*eventWeight)   ;
  Hist->hTauVetoedJetPtUnc   ->Fill(tauVetoedJetPtUnc  , genWeight*eventWeight)   ;
  Hist->hHtSum		     ->Fill(htSum	       , genWeight*eventWeight)   ;
  Hist->hHt		     ->Fill(ht		       , genWeight*eventWeight)   ;
  Hist->hHtPhi               ->Fill(htPhi              , genWeight*eventWeight)   ;

  Hist->hPFMet               ->Fill(pfMETC                , genWeight*eventWeight)      ;
  Hist->hPFMetPhi            ->Fill(pfMETCphi             , genWeight*eventWeight)      ;
  Hist->hPFCovMet00          ->Fill(pfMETCCov00           , genWeight*eventWeight)   ;
  Hist->hPFCovMet01	     ->Fill(pfMETCCov01           , genWeight*eventWeight)   ; 
  Hist->hPFCovMet11	     ->Fill(pfMETCov11           , genWeight*eventWeight)   ; 
  Hist->hPuppMet             ->Fill(puppMETC                , genWeight*eventWeight)      ;
  Hist->hPuppMetPhi          ->Fill(puppMETCphi             , genWeight*eventWeight)      ;
  Hist->hPuppCovMet00        ->Fill(puppMETCov00           , genWeight*eventWeight)   ;
  Hist->hPuppCovMet01	     ->Fill(puppMETCov01           , genWeight*eventWeight)   ; 
  Hist->hPuppCovMet11	     ->Fill(puppMETCov11           , genWeight*eventWeight)   ; 
  Hist->hTrkMet              ->Fill(trkMET                , genWeight*eventWeight)      ;
  Hist->hTrkMetPhi           ->Fill(trkMETphi             , genWeight*eventWeight)      ;
  Hist->hMet                 ->Fill(met                , genWeight*eventWeight)      ;
  Hist->hMetPhi              ->Fill(metPhi             , genWeight*eventWeight)      ;
  Hist->hMetCorr             ->Fill(metCorr            , genWeight*eventWeight)      ;
  Hist->hMetCorrPhi          ->Fill(metCorrPhi         , genWeight*eventWeight)      ;
  Hist->hCovMet00            ->Fill(covMet00           , genWeight*eventWeight)   ;
  Hist->hCovMet01	     ->Fill(covMet01           , genWeight*eventWeight)   ; 
  Hist->hCovMet11	     ->Fill(covMet11           , genWeight*eventWeight)   ; 
  TLorentzVector lepSys = (*leptonOneP4) + (*leptonTwoP4);
  Hist->hMetVsPt             ->Fill(lepSys.Pt(), met   , genWeight*eventWeight)   ; 
  Hist->hMetVsM              ->Fill(lepSys.M(), met    , genWeight*eventWeight)   ; 
  Hist->hMassSVFit	     ->Fill(massSVFit          , genWeight*eventWeight)   ; 
  Hist->hMassErrSVFit        ->Fill(massErrSVFit       , genWeight*eventWeight)   ;
  Hist->hSVFitStatus         ->Fill(svFitStatus        , genWeight*eventWeight)   ;


  TLorentzVector sys    = (*photonP4) + lepSys;
  TLorentzVector svLepSys = (leptonOneSVP4 && leptonTwoSVP4) ? (*leptonOneSVP4) + (*leptonTwoSVP4) : TLorentzVector(0.,0.,0.,0.);
  TLorentzVector svSys    = (leptonOneSVP4 && leptonTwoSVP4) ? (*photonP4) + svLepSys              : TLorentzVector(0.,0.,0.,0.);
  
  float lepDelR   = leptonOneP4->DeltaR(*leptonTwoP4);
  float lepDelPhi = abs(leptonOneP4->DeltaPhi(*leptonTwoP4));
  float lepDelEta = abs(leptonOneP4->Eta() - leptonTwoP4->Eta());
  float htDelPhi  = abs(lepSys.Phi() - htPhi);
  if(htDelPhi > M_PI)
    htDelPhi = abs(2.*M_PI - htDelPhi);
  float metDelPhi  = abs(lepSys.Phi() - metPhi);
  if(metDelPhi > M_PI)
    metDelPhi = abs(2.*M_PI - metDelPhi);
  float lepOneDelPhi  = abs(lepSys.Phi() - leptonOneP4->Phi());
  if(lepOneDelPhi > M_PI)
    lepOneDelPhi = abs(2.*M_PI - lepOneDelPhi);
  float lepTwoDelPhi  = abs(lepSys.Phi() - leptonTwoP4->Phi());
  if(lepTwoDelPhi > M_PI)
    lepTwoDelPhi = abs(2.*M_PI - lepTwoDelPhi);
  
  float lepSVDelR   = -1.;
  float lepSVDelPhi = -1.;
  float lepSVDelEta = -1.;
  float lepSVDelM   = -20.;
  float lepSVDelPt  = -20.;
  if(leptonOneSVP4 && leptonTwoSVP4
     && leptonOneSVP4->Pt() > 1.e-5 && leptonTwoSVP4->Pt() > 1.e-5) {
    lepSVDelR   = leptonOneSVP4->DeltaR(*leptonTwoSVP4)           ;
    lepSVDelPhi = abs(leptonOneSVP4->DeltaPhi(*leptonTwoSVP4))    ;
    lepSVDelEta = abs(leptonOneSVP4->Eta() - leptonTwoSVP4->Eta());
    lepSVDelM = svLepSys.M() - lepSys.M();
    lepSVDelPt = svLepSys.Pt() - lepSys.Pt();
  }
  Hist->hLepPt        ->Fill(lepSys.Pt()            ,eventWeight*genWeight);
  Hist->hLepP         ->Fill(lepSys.P()             ,eventWeight*genWeight);
  Hist->hLepE         ->Fill(lepSys.E()             ,eventWeight*genWeight);
  Hist->hLepM         ->Fill(lepSys.M()             ,eventWeight*genWeight);
  Hist->hLepEta       ->Fill(lepSys.Eta()           ,eventWeight*genWeight);
  Hist->hLepPhi       ->Fill(lepSys.Phi()           ,eventWeight*genWeight);
						    		
  Hist->hLepDeltaPhi  ->Fill(lepDelPhi              ,eventWeight*genWeight);
  Hist->hLepDeltaEta  ->Fill(lepDelEta              ,eventWeight*genWeight);
  Hist->hLepDeltaR    ->Fill(lepDelR                ,eventWeight*genWeight);
  Hist->hLepDelRVsPhi ->Fill(lepDelR , lepDelPhi    ,eventWeight*genWeight);
  Hist->hLepPtOverM   ->Fill(lepSys.Pt()/lepSys.M() ,eventWeight*genWeight);
  Hist->hHtDeltaPhi   ->Fill(htDelPhi               ,eventWeight*genWeight);
  Hist->hMetDeltaPhi  ->Fill(metDelPhi               ,eventWeight*genWeight);
  Hist->hLepOneDeltaPhi->Fill(lepOneDelPhi               ,eventWeight*genWeight);
  Hist->hLepTwoDeltaPhi->Fill(lepTwoDelPhi               ,eventWeight*genWeight);

  Hist->hLepSVPt      ->Fill(svLepSys.Pt()            ,eventWeight*genWeight);
  Hist->hLepSVP       ->Fill(svLepSys.P()             ,eventWeight*genWeight);
  Hist->hLepSVE       ->Fill(svLepSys.E()             ,eventWeight*genWeight);
  Hist->hLepSVM       ->Fill(svLepSys.M()             ,eventWeight*genWeight);
  Hist->hLepSVEta     ->Fill(((svLepSys.Pt() > 1.e-5) ? svLepSys.Eta()  : -1e6),eventWeight*genWeight);
  Hist->hLepSVPhi     ->Fill(svLepSys.Phi()           ,eventWeight*genWeight);

  Hist->hLepSVDeltaPhi ->Fill(lepSVDelPhi              ,eventWeight*genWeight);
  Hist->hLepSVDeltaEta ->Fill(lepSVDelEta              ,eventWeight*genWeight);
  Hist->hLepSVDeltaR   ->Fill(lepSVDelR                ,eventWeight*genWeight);
  Hist->hLepSVDeltaM   ->Fill(lepSVDelM                ,eventWeight*genWeight);
  Hist->hLepSVDeltaPt  ->Fill(lepSVDelPt               ,eventWeight*genWeight);
  Hist->hLepSVDelRVsPhi->Fill(lepSVDelR , lepSVDelPhi  ,eventWeight*genWeight);
  Hist->hLepSVPtOverM  ->Fill(svLepSys.Pt()/svLepSys.M() ,eventWeight*genWeight);
  
  		                
  Hist->hSysM          ->Fill(sys.M()      ,eventWeight*genWeight);
  Hist->hSysPt         ->Fill(sys.Pt()     ,eventWeight*genWeight);
  Hist->hSysEta        ->Fill(sys.Eta()    ,eventWeight*genWeight);

  Hist->hSysSVM          ->Fill(svSys.M()      ,eventWeight*genWeight);
  Hist->hSysSVPt         ->Fill(svSys.Pt()     ,eventWeight*genWeight);
  Hist->hSysSVEta        ->Fill(((svSys.Pt() > 1.e-5) ? svSys.Eta() : -1.e6),eventWeight*genWeight);
  
  TLorentzVector* tau = 0;
  TLorentzVector* muon = 0;
  TLorentzVector* electron = 0;
  if(abs(leptonOneFlavor) == 15)      tau = leptonOneP4;
  else if(abs(leptonTwoFlavor) == 15) tau = leptonTwoP4;
  if(abs(leptonOneFlavor) == 13)      muon = leptonOneP4;
  else if(abs(leptonTwoFlavor) == 13) muon = leptonTwoP4;
  if(abs(leptonOneFlavor) == 11)      electron = leptonOneP4;
  else if(abs(leptonTwoFlavor) == 11) electron = leptonTwoP4;

  double mTMu = (muon != 0) ? 2.*met*muon->Pt() : 0.;
  mTMu = (muon != 0) ? sqrt(mTMu*(1.-cos(muon->Phi() - metPhi))) : 0.;
  double mTE = (electron != 0) ? 2.*met*electron->Pt() : 0.;
  mTE  = (electron != 0) ? sqrt(mTE*(1.-cos(electron->Phi() - metPhi))) : 0.;
  double mTTau = (tau != 0) ? 2.*met*tau->Pt() : 0.;
  mTTau  = (tau != 0) ? sqrt(mTTau*(1.-cos(tau->Phi() - metPhi))) : 0.;

  Hist->hMTMu          ->Fill(mTMu       ,eventWeight*genWeight);
  Hist->hMTE           ->Fill(mTE        ,eventWeight*genWeight);
  Hist->hMTTau         ->Fill(mTTau      ,eventWeight*genWeight);

  Hist->hMDiff         ->Fill(0.85*sys.M() - lepSys.M(),eventWeight*genWeight);

  TVector3 lp1 = leptonOneP4->Vect();
  TVector3 lp2 = leptonTwoP4->Vect();
  TVector3 pp  = photonP4->Vect();
  TVector3 missing(met*cos(metPhi), met*sin(metPhi), 0.);
  lp1.SetZ(0.);
  lp2.SetZ(0.);
  pp.SetZ(0.);
  TVector3 bisector0 = (lp1.Mag()*lp2 + lp2.Mag()*lp1); //divides leptons
  if(bisector0.Mag() > 0.) bisector0.SetMag(1.);

  //combine photon with nearest lepton
  TVector3 lpp = (leptonOneP4->DeltaR(*photonP4) > leptonTwoP4->DeltaR(*photonP4)) ? lp2 : lp1; 
  TVector3 bisector1 = (lpp+pp).Mag()*(lp1+lp2-lpp) + (lpp+pp)*((lp1+lp2-lpp).Mag()); //divides lepton + photon and lepton
  if(bisector1.Mag() > 0.) bisector1.SetMag(1.);

  //combine leptons together
  TVector3 bisector2 = (pp.Mag())*(lp1+lp2) + (lp1+lp2).Mag()*pp; //divides leptons and photon
  if(bisector2.Mag() > 0.) bisector2.SetMag(1.);

  //project onto the bisectors
  //ignore photon
  double pxi_vis0 = (lp1+lp2)*bisector0;
  double pxi_inv0 = missing*bisector0;
  //add photon in
  double pxi_vis1 = (lp1+lp2+pp)*bisector1;
  double pxi_inv1 = missing*bisector1;
  double pxi_vis2 = (lp1+lp2+pp)*bisector2;
  double pxi_inv2 = missing*bisector2;

  Hist->hPXiVis[0]     ->Fill(pxi_vis0         ,eventWeight*genWeight);
  Hist->hPXiInv[0]     ->Fill(pxi_inv0         ,eventWeight*genWeight);
  Hist->hPXiVisOverInv[0]->Fill((pxi_inv0 != 0.) ? pxi_vis0/pxi_inv0 : 1.e6 ,eventWeight*genWeight);
  Hist->hPXiInvVsVis[0]->Fill(pxi_vis0, pxi_inv0,eventWeight*genWeight);
  Hist->hPXiDiff[0]    ->Fill(pxi_vis0-pxi_inv0 ,eventWeight*genWeight);

  Hist->hPTauVisFrac   ->Fill(fTreeVars.ptauvisfrac, eventWeight*genWeight);
  Hist->hLepMEstimate  ->Fill(fTreeVars.mestimate  , eventWeight*genWeight);

  if(nPhotons > 0) {
    Hist->hPXiVis[1]     ->Fill(pxi_vis1         ,eventWeight*genWeight);
    Hist->hPXiInv[1]     ->Fill(pxi_inv1         ,eventWeight*genWeight);
    Hist->hPXiVisOverInv[1]->Fill((pxi_inv1 != 0.) ? pxi_vis1/pxi_inv1 : 1.e6 ,eventWeight*genWeight);
    Hist->hPXiInvVsVis[1]->Fill(pxi_vis1, pxi_inv1,eventWeight*genWeight);
    Hist->hPXiDiff[1]    ->Fill(pxi_vis1-pxi_inv1 ,eventWeight*genWeight);
    Hist->hPXiVis[2]     ->Fill(pxi_vis2         ,eventWeight*genWeight);
    Hist->hPXiInv[2]     ->Fill(pxi_inv2         ,eventWeight*genWeight);
    Hist->hPXiVisOverInv[2]->Fill((pxi_inv2 != 0.) ? pxi_vis2/pxi_inv2 : 1.e6 ,eventWeight*genWeight);
    Hist->hPXiInvVsVis[2]->Fill(pxi_vis2, pxi_inv2,eventWeight*genWeight);
    Hist->hPXiDiff[2]    ->Fill(pxi_vis2-pxi_inv2 ,eventWeight*genWeight);
  }
  
  double coeff = 0.6;
  double offset = -40.;
  Hist->hPXiDiff2[0]   ->Fill(pxi_inv0 - (coeff*pxi_vis0+offset) ,eventWeight*genWeight);
  Hist->hPXiDiff2[1]   ->Fill(pxi_inv1 - (coeff*pxi_vis1+offset) ,eventWeight*genWeight);
  Hist->hPXiDiff2[2]   ->Fill(pxi_inv2 - (coeff*pxi_vis2+offset) ,eventWeight*genWeight);

  Hist->hPtSum[0]      ->Fill(leptonOneP4->Pt()+leptonTwoP4->Pt()+met ,eventWeight*genWeight);
  Hist->hPtSum[1]      ->Fill(leptonOneP4->Pt()+leptonTwoP4->Pt()+photonP4->Pt()+met ,eventWeight*genWeight);
  Hist->hPt1Sum[0]     ->Fill(leptonOneP4->Pt()+met ,eventWeight*genWeight);
  Hist->hPt1Sum[1]     ->Fill(leptonTwoP4->Pt()+met ,eventWeight*genWeight);
  Hist->hPt1Sum[2]     ->Fill(leptonTwoP4->Pt()+leptonTwoP4->Pt() ,eventWeight*genWeight);
  Hist->hPt1Sum[3]     ->Fill(leptonTwoP4->Pt()+leptonTwoP4->Pt()-met ,eventWeight*genWeight);

  //MVA outputs
  for(unsigned i = 0; i < fMvaNames.size(); ++i) {
    Hist->hMVA[i]   ->Fill(fMvaOutputs[i], eventWeight*genWeight);
    Hist->hProb[i]  ->Fill(fMvaProb[i], eventWeight*genWeight);
    Hist->hRarity[i]->Fill(fMvaRarity[i], eventWeight*genWeight);
    Hist->hCdf[i]   ->Fill(fMvaCdf[i], eventWeight*genWeight);
  }
}

void ZTauTauHistMaker::FillPhotonHistogram(PhotonHist_t* Hist) {
  Hist->hPx  ->Fill(photonP4->Px() , eventWeight*genWeight );
  Hist->hPy  ->Fill(photonP4->Py() , eventWeight*genWeight );
  Hist->hPz  ->Fill(photonP4->Pz() , eventWeight*genWeight );
  Hist->hPt  ->Fill(photonP4->Pt() , eventWeight*genWeight );
  Hist->hP   ->Fill(photonP4->P()  , eventWeight*genWeight );
  Hist->hE   ->Fill(photonP4->E()  , eventWeight*genWeight );
  Hist->hEta ->Fill((photonP4->Pt() > 0.) ? photonP4->Eta() : -1.e6, eventWeight*genWeight );
  Hist->hPhi ->Fill(photonP4->Phi(), eventWeight*genWeight );
  // Hist->hIso       ;
  // Hist->hRelIso    ;
  // Hist->hTrigger   ;
}

void ZTauTauHistMaker::FillLepHistogram(LepHist_t* Hist) {
  Hist->hOnePx        ->Fill(leptonOneP4->Px()      ,eventWeight*genWeight);
  Hist->hOnePy        ->Fill(leptonOneP4->Py()      ,eventWeight*genWeight);
  Hist->hOnePz        ->Fill(leptonOneP4->Pz()      ,eventWeight*genWeight);
  Hist->hOnePt        ->Fill(leptonOneP4->Pt()      ,eventWeight*genWeight);
  Hist->hOneP         ->Fill(leptonOneP4->P()       ,eventWeight*genWeight);
  Hist->hOneE         ->Fill(leptonOneP4->E()       ,eventWeight*genWeight);
  Hist->hOneM         ->Fill(leptonOneP4->M()       ,eventWeight*genWeight);
  Hist->hOneEta       ->Fill(leptonOneP4->Eta()     ,eventWeight*genWeight);
  Hist->hOnePhi       ->Fill(leptonOneP4->Phi()     ,eventWeight*genWeight);
  Hist->hOneD0        ->Fill(leptonOneD0            ,eventWeight*genWeight);
  Hist->hOneIso       ->Fill(leptonOneIso           ,eventWeight*genWeight);
  float relIso1 = leptonOneIso/ leptonOneP4->Pt();
  Hist->hOneRelIso    ->Fill(relIso1                ,eventWeight*genWeight);
  Hist->hOneFlavor    ->Fill(fabs(leptonOneFlavor)        ,eventWeight*genWeight);
  Hist->hOneQ         ->Fill(leptonOneFlavor < 0 ? -1 : 1 ,eventWeight*genWeight);
  // Hist->hOneTrigger   ->Fill(leptonOneTrigger       ,eventWeight*genWeight);

  if(genLeptonOneP4) {
    Hist->hOneGenPt       ->Fill(genLeptonOneP4->Pt()    ,eventWeight*genWeight);
    Hist->hOneGenE        ->Fill(genLeptonOneP4->E()     ,eventWeight*genWeight);
    Hist->hOneGenEta      ->Fill(genLeptonOneP4->Eta()   ,eventWeight*genWeight);
    Hist->hOneDeltaPt     ->Fill(leptonOneP4->Pt()  - genLeptonOneP4->Pt()    ,eventWeight*genWeight);
    Hist->hOneDeltaE      ->Fill(leptonOneP4->E()   - genLeptonOneP4->E()     ,eventWeight*genWeight);
    Hist->hOneDeltaEta    ->Fill(leptonOneP4->Eta() - genLeptonOneP4->Eta()   ,eventWeight*genWeight);
  }
  float oneMetDelPhi  = abs(leptonOneP4->Phi() - metPhi);
  if(oneMetDelPhi > M_PI)
    oneMetDelPhi = abs(2.*M_PI - oneMetDelPhi);
  Hist->hOneMetDeltaPhi   ->Fill(oneMetDelPhi   ,eventWeight*genWeight);

  if(leptonOneSVP4) {
    Hist->hOneSVPt       ->Fill(leptonOneSVP4->Pt()    ,eventWeight*genWeight);
    Hist->hOneSVP        ->Fill(leptonOneSVP4->P()     ,eventWeight*genWeight);
    Hist->hOneSVE        ->Fill(leptonOneSVP4->E()     ,eventWeight*genWeight);
    Hist->hOneSVM        ->Fill(leptonOneSVP4->M()     ,eventWeight*genWeight);
    if(leptonOneSVP4->Pt() > 1.e-5)
      Hist->hOneSVEta      ->Fill(leptonOneSVP4->Eta()   ,eventWeight*genWeight);
    else
      Hist->hOneSVEta      ->Fill(-1.e6   ,eventWeight*genWeight);
    Hist->hOneSVDeltaPt  ->Fill(leptonOneSVP4->Pt()  -  leptonOneP4->Pt()  ,eventWeight*genWeight);
    Hist->hOneSVDeltaP   ->Fill(leptonOneSVP4->P()   -  leptonOneP4->P()   ,eventWeight*genWeight);
    Hist->hOneSVDeltaE   ->Fill(leptonOneSVP4->E()   -  leptonOneP4->E()   ,eventWeight*genWeight);
    if(leptonOneSVP4->Pt() > 1.e-5)
      Hist->hOneSVDeltaEta ->Fill(leptonOneSVP4->Eta() -  leptonOneP4->Eta() ,eventWeight*genWeight);
    else
      Hist->hOneSVDeltaEta ->Fill(1.e6 ,eventWeight*genWeight);
  }  
  Hist->hTwoPx        ->Fill(leptonTwoP4->Px()      ,eventWeight*genWeight);
  Hist->hTwoPy        ->Fill(leptonTwoP4->Py()      ,eventWeight*genWeight);
  Hist->hTwoPz        ->Fill(leptonTwoP4->Pz()      ,eventWeight*genWeight);
  Hist->hTwoPt        ->Fill(leptonTwoP4->Pt()      ,eventWeight*genWeight);
  Hist->hTwoP         ->Fill(leptonTwoP4->P()       ,eventWeight*genWeight);
  Hist->hTwoE         ->Fill(leptonTwoP4->E()       ,eventWeight*genWeight);
  Hist->hTwoM         ->Fill(leptonTwoP4->M()       ,eventWeight*genWeight);
  Hist->hTwoEta       ->Fill(leptonTwoP4->Eta()     ,eventWeight*genWeight);
  Hist->hTwoPhi       ->Fill(leptonTwoP4->Phi()     ,eventWeight*genWeight);
  Hist->hTwoD0        ->Fill(leptonTwoD0            ,eventWeight*genWeight);
  Hist->hTwoIso       ->Fill(leptonTwoIso           ,eventWeight*genWeight);
  float relIso2 = leptonTwoIso/ leptonTwoP4->Pt();
  Hist->hTwoRelIso    ->Fill(relIso2                ,eventWeight*genWeight);
  Hist->hTwoFlavor    ->Fill(fabs(leptonTwoFlavor)        ,eventWeight*genWeight);
  Hist->hTwoQ         ->Fill(leptonTwoFlavor < 0 ? -1 : 1 ,eventWeight*genWeight);
  // Hist->hTwoTrigger   ->Fill(leptonTwoTrigger       ,eventWeight*genWeight);

  if(genLeptonTwoP4) {
    Hist->hTwoGenPt       ->Fill(genLeptonTwoP4->Pt()    ,eventWeight*genWeight);
    Hist->hTwoGenE        ->Fill(genLeptonTwoP4->E()     ,eventWeight*genWeight);
    Hist->hTwoGenEta      ->Fill(genLeptonTwoP4->Eta()   ,eventWeight*genWeight);
    Hist->hTwoDeltaPt     ->Fill(leptonTwoP4->Pt()  - genLeptonTwoP4->Pt()    ,eventWeight*genWeight);
    Hist->hTwoDeltaE      ->Fill(leptonTwoP4->E()   - genLeptonTwoP4->E()     ,eventWeight*genWeight);
    Hist->hTwoDeltaEta    ->Fill(leptonTwoP4->Eta() - genLeptonTwoP4->Eta()   ,eventWeight*genWeight);
  }

  float twoMetDelPhi  = abs(leptonTwoP4->Phi() - metPhi);
  if(twoMetDelPhi > M_PI)
    twoMetDelPhi = abs(2.*M_PI - twoMetDelPhi);
  Hist->hTwoMetDeltaPhi   ->Fill(twoMetDelPhi   ,eventWeight*genWeight);

  if(leptonTwoSVP4) {
    Hist->hTwoSVPt      ->Fill(leptonTwoSVP4->Pt()    ,eventWeight*genWeight);
    Hist->hTwoSVP       ->Fill(leptonTwoSVP4->P()     ,eventWeight*genWeight);
    Hist->hTwoSVE       ->Fill(leptonTwoSVP4->E()     ,eventWeight*genWeight);
    Hist->hTwoSVM       ->Fill(leptonTwoSVP4->M()     ,eventWeight*genWeight);
    if(leptonTwoSVP4->Pt() > 1.e-5)
      Hist->hTwoSVEta      ->Fill(leptonTwoSVP4->Eta()   ,eventWeight*genWeight);
    else
      Hist->hTwoSVEta      ->Fill(-1.e6   ,eventWeight*genWeight);
    Hist->hTwoSVDeltaPt  ->Fill(leptonTwoSVP4->Pt()  -  leptonTwoP4->Pt()  ,eventWeight*genWeight);
    Hist->hTwoSVDeltaP   ->Fill(leptonTwoSVP4->P()   -  leptonTwoP4->P()   ,eventWeight*genWeight);
    Hist->hTwoSVDeltaE   ->Fill(leptonTwoSVP4->E()   -  leptonTwoP4->E()   ,eventWeight*genWeight);
    if(leptonTwoSVP4->Pt() > 1.e-5)    
      Hist->hTwoSVDeltaEta ->Fill(leptonTwoSVP4->Eta() -  leptonTwoP4->Eta() ,eventWeight*genWeight);
    else
      Hist->hTwoSVDeltaEta ->Fill(-1.e6 ,eventWeight*genWeight);

  }
  Hist->hTwoPtVsOnePt->Fill(leptonOneP4->Pt(), leptonTwoP4->Pt(), eventWeight*genWeight);
}


Bool_t ZTauTauHistMaker::Process(Long64_t entry)
{
  fentry = entry;
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

  fChain->GetEntry(entry);
  if(entry%50000 == 0) printf("Processing event: %12lld\n", entry);
  //DY Splitting
  if(fDYType > 0) {
    if(fDYType > 1 && nGenTausHad+nGenTausLep >= 2) return kTRUE; // gen counting currently odd, so ask at least 2
    else if(fDYType == 1 && nGenTausHad+nGenTausLep < 2) return kTRUE; 
  }

  //skip if it's data and lepton status doesn't match data set ( 1 = electron 2 = muon) unless allowing overlap and it passes both
  if(fIsData>0 && triggerLeptonStatus != ((UInt_t) fIsData) &&
     (fSkipDoubleTrigger || triggerLeptonStatus != 3)) return kTRUE; //avoid double counting data events and events triggered for the other dataset
  
  //add the met correction to the met
  TVector3 missing(met*cos(metPhi), met*sin(metPhi), 0.);
  TVector3 missingCorr(metCorr*cos(metCorrPhi), metCorr*sin(metCorrPhi), 0.);
  missing = missing + missingCorr;
  met = missing.Mag();
  metPhi = missing.Phi();

  //selections
  bool mutau = nTaus == 1  && nMuons == 1 && nElectrons == 0;
  bool etau  = nTaus == 1  && nMuons == 0 && nElectrons == 1;
  bool emu   = nTaus == 0  && nMuons == 1 && nElectrons == 1;


  InitializeTreeVariables(mutau+2*etau+5*emu);
  //FIXME temporary fix to hadronic tau ID if is not data
  if((mutau || etau)&&fIsData==0) {
    eventWeight *= 1./0.95; //remove overall tau ID weight that didn't include if gen tau or not in the total event weight
    if(abs(tauGenFlavor) == 15 && fUseTauFakeSF == 1) genTauFlavorWeight *= 0.95; //apply to real taus to original weights
  }
  
  //use locally computed weight 
  if(fUseTauFakeSF > 1 && fIsData == 0) genTauFlavorWeight = GetTauFakeSF(tauGenFlavor);
  
  //apply fake tau SF
  if(fUseTauFakeSF && fIsData == 0) eventWeight *= genTauFlavorWeight;

  bool chargeTest = leptonOneFlavor*leptonTwoFlavor < 0;
  // FillAllHistograms(0);

  TVector3 lp1 = leptonOneP4->Vect();
  TVector3 lp2 = leptonTwoP4->Vect();
  lp1.SetZ(0.);
  lp2.SetZ(0.);
  TVector3 bisector = (lp1.Mag()*lp2 + lp2.Mag()*lp1);
  bisector.SetMag(1.);
  double pxi_vis = (lp1+lp2)*bisector;
  double pxi_inv = missing*bisector;
  
  // if(chargeTest) FillAllHistograms(1);
  // else           FillAllHistograms(1 + fQcdOffset);

  // if(nPhotons > 0  && chargeTest) FillAllHistograms(2);
  // else if(nPhotons > 0)           FillAllHistograms(2 + fQcdOffset);
  // if(nPhotons == 1 && chargeTest) FillAllHistograms(3);
  // else if(nPhotons == 1)          FillAllHistograms(3 + fQcdOffset);


  //////////////////////////////////////////////////////////////
  //
  // Object cuts + selection section
  //
  //////////////////////////////////////////////////////////////


  ////////////////////////////////////////////////////////////
  // Set 5 + selection offset: object number selection
  ////////////////////////////////////////////////////////////
  if(mutau && chargeTest) FillAllHistograms(5);
  else if(mutau)          FillAllHistograms(5 + fQcdOffset);
  if(etau  && chargeTest) FillAllHistograms(25);
  else if(etau)           FillAllHistograms(25 + fQcdOffset);
  if(emu   && chargeTest) FillAllHistograms(45);
  else if(emu)            FillAllHistograms(45 + fQcdOffset);
  
  TLorentzVector* tau = 0;
  TLorentzVector* muon = 0;
  TLorentzVector* electron = 0;
  if(abs(leptonOneFlavor) == 15)      tau = leptonOneP4;
  else if(abs(leptonTwoFlavor) == 15) tau = leptonTwoP4;
  if(abs(leptonOneFlavor) == 13)      muon = leptonOneP4;
  else if(abs(leptonTwoFlavor) == 13) muon = leptonTwoP4;
  if(abs(leptonOneFlavor) == 11)      electron = leptonOneP4;
  else if(abs(leptonTwoFlavor) == 11) electron = leptonTwoP4;

  mutau = mutau && (tau != 0) && (muon != 0);
  etau  = etau  && (tau != 0) && (electron != 0);
  emu   = emu   && (muon != 0) && (electron != 0);

  mutau = mutau && muon->Pt() > 25. && tau->Pt() > 20.;
  etau  = etau  && electron->Pt() > 30. && tau->Pt() > 20.;
  emu   = emu   && ((electron->Pt() > 30. && muon->Pt() > 10.) ||
		    (electron->Pt() > 15. && muon->Pt() > 25.));

  ////////////////////////////////////////////////////////////
  // Set 6 + selection offset: object pT cuts
  ////////////////////////////////////////////////////////////
  if(mutau && chargeTest) FillAllHistograms(6);
  else if(mutau)          FillAllHistograms(6 + fQcdOffset);
  if(etau  && chargeTest) FillAllHistograms(26);
  else if(etau)           FillAllHistograms(26 + fQcdOffset);
  if(emu  && chargeTest)  FillAllHistograms(46);
  else if(emu)            FillAllHistograms(46 + fQcdOffset);
  
  mutau = mutau && abs(muon->Eta()) < 2.4;
  mutau = mutau && abs(tau->Eta()) < 2.3;
  mutau = mutau && abs(tau->DeltaR(*muon)) > 0.3;

  etau  = etau  && abs(electron->Eta()) < 2.5;
  etau  = etau  && abs(tau->Eta()) < 2.3;
  etau  = etau  && abs(tau->DeltaR(*electron)) > 0.3;

  emu  = emu  && abs(electron->Eta()) < 2.5;
  emu  = emu  && abs(muon->Eta()) < 2.4;
  emu  = emu  && abs(muon->DeltaR(*electron)) > 0.3;

  ////////////////////////////////////////////////////////////
  // Set 7 + selection offset: object eta cuts
  ////////////////////////////////////////////////////////////
  if(mutau && chargeTest) FillAllHistograms(7);
  else if(mutau)          FillAllHistograms(7 + fQcdOffset);
  if(etau  && chargeTest) FillAllHistograms(27);
  else if(etau)           FillAllHistograms(27 + fQcdOffset);
  if(emu  && chargeTest) FillAllHistograms(47);
  else if(emu)           FillAllHistograms(47 + fQcdOffset);

  //////////////////////////////////////////////////////////////
  //
  // Analysis cut section
  //
  //////////////////////////////////////////////////////////////


  mutau &= nBJets == 0;
  etau  &= nBJets == 0;
  emu   &= nBJets == 0;

  ////////////////////////////////////////////////////////////////////////////
  // Set 8 + selection offset: nBJets = 0
  ////////////////////////////////////////////////////////////////////////////
  if(mutau && chargeTest) FillAllHistograms(8);
  else if(mutau)          FillAllHistograms(8 + fQcdOffset);
  if(etau && chargeTest)  FillAllHistograms(28);
  else if(etau)           FillAllHistograms(28 + fQcdOffset);
  if(emu && chargeTest)   FillAllHistograms(48);
  else if(emu)            FillAllHistograms(48 + fQcdOffset);

  
  ////////////////////////////////////////////////////////////////////////////
  // Set 9-12 : BDT Cut
  ////////////////////////////////////////////////////////////////////////////
  //Total background Z0 MVAs
  if(mutau && chargeTest && fMvaOutputs[1] > fMvaCuts[1]) FillAllHistograms(9);
  else if(mutau && fMvaOutputs[1] > fMvaCuts[1])          FillAllHistograms(9 + fQcdOffset);
  if(etau && chargeTest && fMvaOutputs[3] > fMvaCuts[3])  FillAllHistograms(29);
  else if(etau && fMvaOutputs[3] > fMvaCuts[3])           FillAllHistograms(29 + fQcdOffset);
  if(emu && chargeTest && fMvaOutputs[5] > fMvaCuts[5])   FillAllHistograms(49);
  else if(emu && fMvaOutputs[5] > fMvaCuts[5])            FillAllHistograms(49 + fQcdOffset);
  //Total background higgs MVAs
  if(mutau && chargeTest && fMvaOutputs[0] > fMvaCuts[0]) FillAllHistograms(10);
  else if(mutau && fMvaOutputs[0] > fMvaCuts[0])          FillAllHistograms(10 + fQcdOffset);
  if(etau && chargeTest && fMvaOutputs[2] > fMvaCuts[2])  FillAllHistograms(30);
  else if(etau && fMvaOutputs[2] > fMvaCuts[2])           FillAllHistograms(30 + fQcdOffset);
  if(emu && chargeTest && fMvaOutputs[4] > fMvaCuts[4])   FillAllHistograms(50);
  else if(emu && fMvaOutputs[4] > fMvaCuts[4])            FillAllHistograms(50 + fQcdOffset);
  // //Specific background MVAs
  // if(mutau && chargeTest && fMvaOutputs[0] > fMvaCuts[0]) FillAllHistograms(10);
  // else if(mutau && fMvaOutputs[0] > fMvaCuts[0])          FillAllHistograms(10 + fQcdOffset);
  // if(etau && chargeTest && fMvaOutputs[2] > fMvaCuts[2])  FillAllHistograms(30);
  // else if(etau && fMvaOutputs[2] > fMvaCuts[2])           FillAllHistograms(30 + fQcdOffset);

  // Mass window sets, before cuts
  double mll = (*leptonOneP4+*leptonTwoP4).M();
  double mgll = (*photonP4 + (*leptonOneP4+*leptonTwoP4)).M();
  //Z0 window
  if(mutau && chargeTest && mll < 105. && mll > 45.) FillAllHistograms(11);
  else if(mutau && mll < 105. && mll > 45.)          FillAllHistograms(11 + fQcdOffset);
  if(etau && chargeTest && mll < 105. && mll > 45.)  FillAllHistograms(31);
  else if(etau && mll < 105. && mll > 45.)           FillAllHistograms(31 + fQcdOffset);
  if(emu && chargeTest && mll < 105. && mll > 75.)   FillAllHistograms(51);
  else if(emu && mll < 105. && mll > 75.)            FillAllHistograms(51 + fQcdOffset);
  //higgs window
  if(mutau && chargeTest && mll < 130. && mll > 60.) FillAllHistograms(12);
  else if(mutau && mll < 130. && mll > 60.)          FillAllHistograms(12 + fQcdOffset);
  if(etau && chargeTest && mll < 130. && mll > 60.)  FillAllHistograms(32);
  else if(etau && mll < 130. && mll > 60.)           FillAllHistograms(32 + fQcdOffset);
  if(emu && chargeTest && mll < 130. && mll > 115.)  FillAllHistograms(52);
  else if(emu && mll < 130. && mll > 115.)           FillAllHistograms(52 + fQcdOffset);

  // if(mutau && chargeTest && fMvaOutputs[2] > fMvaCuts[2] && fMvaOutputs[3] > fMvaCuts[3]) FillAllHistograms(12);
  // else if(mutau && fMvaOutputs[2] > fMvaCuts[2] && fMvaOutputs[3] > fMvaCuts[3])          FillAllHistograms(12 + fQcdOffset);
  // if(etau && chargeTest && fMvaOutputs[2] > fMvaCuts[2] && fMvaOutputs[3] > fMvaCuts[3])  FillAllHistograms(32);
  // else if(etau && fMvaOutputs[2] > fMvaCuts[2] && fMvaOutputs[3] > fMvaCuts[3])           FillAllHistograms(32 + fQcdOffset);
  
  //adding visible/invisible pT cuts
  double offset = -50.;
  double coeff = 0.5;
  double visShift = coeff*pxi_vis + offset;

  mutau &= visShift < pxi_inv;
  etau  &= visShift < pxi_inv;
  // emu   &= visShift < pxi_inv;
  emu   &= met < 70.;

  ///////////////////////////////////////////////////////////////////////
  // Set 13 + selection offset: visible*0.5 - 50 GeV < invisible (or MET)
  ///////////////////////////////////////////////////////////////////////
  if(mutau && chargeTest) FillAllHistograms(13);
  else if(mutau)          FillAllHistograms(13 + fQcdOffset);
  if(etau && chargeTest)  FillAllHistograms(33);
  else if(etau)           FillAllHistograms(33 + fQcdOffset);
  if(emu && chargeTest)   FillAllHistograms(53);
  else if(emu)            FillAllHistograms(53 + fQcdOffset);
  
  mutau = mutau && mll < 105. && mll > 45.;
  etau  = etau  && mll < 105. && mll > 45.;

  ////////////////////////////////////////////////////////////////////////////
  // Set 14 + selection offset: 45 GeV < mll < 105 GeV
  ////////////////////////////////////////////////////////////////////////////
  if(mutau && chargeTest) FillAllHistograms(14);
  else if(mutau)          FillAllHistograms(14 + fQcdOffset);
  if(etau && chargeTest)  FillAllHistograms(34);
  else if(etau)           FillAllHistograms(34 + fQcdOffset);
  
  double mTTau = (tau != 0) ? 2.*met*tau->Pt() : 0.;
  mTTau = (tau != 0) ? sqrt(mTTau*(1.-cos(tau->Phi() - metPhi))) : 0.;
  double mTMu = (muon != 0) ? 2.*met*muon->Pt() : 0.;
  mTMu = (muon != 0) ? sqrt(mTMu*(1.-cos(muon->Phi() - metPhi))) : 0.;
  double mTE = (electron != 0) ? 2.*met*electron->Pt() : 0.;
  mTE  = (electron != 0) ? sqrt(mTE*(1.-cos(electron->Phi() - metPhi))) : 0.;

  mutau &= mTMu  < 100.;
  etau  &= mTE   < 100.;
  emu   &= mTE   < 100.;
  emu   &= mTMu  < 100.;

  ////////////////////////////////////////////////////////////////////////////
  // Set 15 + selection offset: mT_lep < 100 GeV
  ////////////////////////////////////////////////////////////////////////////
  if(mutau && chargeTest) FillAllHistograms(15);
  else if(mutau)          FillAllHistograms(15 + fQcdOffset);
  if(etau && chargeTest)  FillAllHistograms(35);
  else if(etau)           FillAllHistograms(35 + fQcdOffset);
  if(emu && chargeTest)   FillAllHistograms(55);
  else if(emu)            FillAllHistograms(55 + fQcdOffset);

  ////////////////////////////////////////////////////////////////////////////
  // Set 15 + selection offset: E+Mu mass window
  ////////////////////////////////////////////////////////////////////////////
  emu   &= mll < 105.;
  emu   &= mll > 75.;
  if(emu && chargeTest)   FillAllHistograms(56);
  else if(emu)            FillAllHistograms(56 + fQcdOffset);


  ////////////////////////////////////////////////////////////////////////////
  // Set 18-20 + selection offset: Jet categories 0, 1, >1
  ////////////////////////////////////////////////////////////////////////////
  if(mutau && nJets == 0 && chargeTest) FillAllHistograms(18);
  else if(mutau && nJets == 0)          FillAllHistograms(18 + fQcdOffset);
  if(etau && nJets == 0 && chargeTest)  FillAllHistograms(38);
  else if(emu && nJets == 0)            FillAllHistograms(58 + fQcdOffset);
  if(emu && nJets == 0 && chargeTest)   FillAllHistograms(58);
  else if(etau && nJets == 0)           FillAllHistograms(38 + fQcdOffset);
  if(mutau && nJets == 1 && chargeTest) FillAllHistograms(19);
  else if(mutau && nJets == 1)          FillAllHistograms(19 + fQcdOffset);
  if(etau && nJets == 1 && chargeTest)  FillAllHistograms(39);
  else if(etau && nJets == 1)           FillAllHistograms(39 + fQcdOffset);
  if(emu && nJets == 1 && chargeTest)   FillAllHistograms(59);
  else if(emu && nJets == 1)            FillAllHistograms(59 + fQcdOffset);
  if(mutau && nJets > 1  && chargeTest) FillAllHistograms(20);
  else if(mutau && nJets > 1 )          FillAllHistograms(20 + fQcdOffset);
  if(etau && nJets > 1  && chargeTest)  FillAllHistograms(40);
  else if(etau && nJets > 1 )           FillAllHistograms(40 + fQcdOffset);
  if(emu && nJets > 1  && chargeTest)   FillAllHistograms(60);
  else if(emu && nJets > 1 )            FillAllHistograms(60 + fQcdOffset);


  //add required photon
  ////////////////////////////////////////////////////////////////////////////
  // Set 21-23 + selection offset: Photon categories 0, 1, > 1
  ////////////////////////////////////////////////////////////////////////////
  if(mutau && nPhotons == 0 && chargeTest) FillAllHistograms(21);
  else if(mutau && nPhotons == 0)          FillAllHistograms(21 + fQcdOffset);
  if(etau && nPhotons == 0 && chargeTest)  FillAllHistograms(41);
  else if(etau && nPhotons == 0)           FillAllHistograms(41 + fQcdOffset);
  if(mutau && nPhotons == 1 && chargeTest) FillAllHistograms(22);
  else if(mutau && nPhotons == 1)          FillAllHistograms(22 + fQcdOffset);
  if(etau && nPhotons == 1 && chargeTest)  FillAllHistograms(42);
  else if(etau && nPhotons == 1)           FillAllHistograms(42 + fQcdOffset);
  if(mutau && nPhotons > 1  && chargeTest) FillAllHistograms(23);
  else if(mutau && nPhotons > 1 )          FillAllHistograms(23 + fQcdOffset);
  if(etau && nPhotons > 1  && chargeTest)  FillAllHistograms(43);
  else if(etau && nPhotons > 1 )           FillAllHistograms(43 + fQcdOffset);

  ////////////////////////////////////////////////////////////////////////////
  // Set 24 : BDT Cut
  ////////////////////////////////////////////////////////////////////////////
  if(mutau && chargeTest && fMvaOutputs[1] > fMvaCuts[1]) FillAllHistograms(24);
  else if(mutau && fMvaOutputs[1] > fMvaCuts[1])          FillAllHistograms(24 + fQcdOffset);
  if(etau && chargeTest && fMvaOutputs[3] > fMvaCuts[3])  FillAllHistograms(44);
  else if(etau && fMvaOutputs[3] > fMvaCuts[3])           FillAllHistograms(44 + fQcdOffset);

  // mutau = mutau && nPhotons > 0;
  // mutau = mutau && (abs(photonP4->Eta()) < 1.4442 || (abs(photonP4->Eta()) > 1.566 && abs(photonP4->Eta()) < 2.5));
  // mutau = mutau && photonP4->Pt() > 10.;
  // mutau = mutau && photonP4->DeltaR(*muon) > 0.5;
  // mutau = mutau && photonP4->DeltaR(*tau) > 0.5;
  // etau = etau && nPhotons > 0;
  // etau = etau && (abs(photonP4->Eta()) < 1.4442 || (abs(photonP4->Eta()) > 1.566 && abs(photonP4->Eta()) < 2.5));
  // etau = etau && photonP4->Pt() > 10.;
  // etau = etau && photonP4->DeltaR(*electron) > 0.5;
  // etau = etau && photonP4->DeltaR(*tau) > 0.5;
  // if(mutau && chargeTest) FillAllHistograms(21);
  // else if(mutau)          FillAllHistograms(21 + fQcdOffset);
  // if(etau && chargeTest)  FillAllHistograms(41);
  // else if(etau)           FillAllHistograms(41 + fQcdOffset);

  
  // //extra cuts added
  // mutau = mutau && 10. < (0.85*mgll - mll) && 25. > (0.85*mgll - mll);
  // mutau = mutau && photonP4->Pt() > 18.;
  // if(mutau && chargeTest) FillAllHistograms(10);
  // else if(mutau)          FillAllHistograms(10 + fQcdOffset);

  // double zmass = 91.1876;
  // double mge = (electron) ? (*photonP4 + *electron).M() : 0.;
  // etau = etau && 10. < (0.85*mgll - mll) && 25. > (0.85*mgll - mll);
  // etau = etau && photonP4->Pt() > 18.;
  // etau = etau && abs(mge-zmass) > 10.;
  // if(etau && chargeTest) FillAllHistograms(30);
  // else if(etau)          FillAllHistograms(30 + fQcdOffset);

  return kTRUE;
}

void ZTauTauHistMaker::SlaveTerminate()
{
  // The SlaveTerminate() function is called after all entries or objects
  // have been processed. When running with PROOF SlaveTerminate() is called
  // on each slave server.

}

void ZTauTauHistMaker::Terminate()
{
  // The Terminate() function is the last function to be called during
  // a query. It always runs on the client, it can be used to present
  // the results graphically or save the results to file.
  printf("Writing output file into %s\n",fOut->GetName());
  fOut->Write();//Form("%s_OutFile.root",fChain->GetName()));
  delete fOut;
  // timer->Stop();
  Double_t cpuTime = timer->CpuTime();
  Double_t realTime = timer->RealTime();
  printf("Processing time: %7.2fs CPU time %7.2fs Wall time\n",cpuTime,realTime);
  if(realTime > 600. ) printf("Processing time: %7.2fmin CPU time %7.2fmin Wall time\n",cpuTime/60.,realTime/60.);


}
