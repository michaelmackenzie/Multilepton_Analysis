//TSelector to process TTrees output by Z_LFV_Analysis.git ntupling in NanoAOD format

#ifndef CLFVTMPHISTMAKER_HH
#define CLFVTMPHISTMAKER_HH

#include "interface/HistMaker.hh"

namespace CLFV {

  class CLFVTmpHistMaker : public HistMaker {
  public :

    CLFVTmpHistMaker(int seed = 90, TTree * /*tree*/ = 0);
    ~CLFVTmpHistMaker();


    void    Begin(TTree*);
    void    InitHistogramFlags();
    void    BookHistograms();
    void    BookEventHistograms();
    void    BookPhotonHistograms();
    void    BookLepHistograms();
    void    BookSystematicHistograms();
    void    BookTrees();
    void    DeleteHistograms();

    void    FillAllHistograms(Int_t index);
    void    FillEventHistogram(EventHist_t* Hist);
    void    FillPhotonHistogram(PhotonHist_t* Hist);
    void    FillLepHistogram(LepHist_t* Hist);
    void    FillSystematicHistogram(SystematicHist_t* Hist);

    Bool_t  Process(Long64_t entry);

    TString GetOutputName() {
      return Form("clfv%s_%i_%s%s%s.hist",
                  (fSelection == "") ? "" : ("_"+fSelection).Data(),fYear, fDataset.Data(),
                  (fDYType >  0) ? Form("-%i",fDYType) : "",
                  (fWNJets >= 0) ? Form("-%i",fWNJets) : ""
                  );
    }


    Bool_t          fDYTesting = false; //for speeding up histogramming to only do DY weight related aspects
    Bool_t          fDYFakeTauTesting = false; //for speeding up histogramming to only do DY jet --> fake tau scale factor aspects
    Bool_t          fWJFakeTauTesting = false; //for speeding up histogramming to only do W+jet --> fake tau scale factor aspects
    Bool_t          fTTFakeTauTesting = false; //for speeding up histogramming to only do ttbar jet --> fake tau scale factor aspects
    Bool_t          fQCDFakeTauTesting = false; //for speeding up histogramming to only do qcd jet --> fake tau scale factor aspects
    Bool_t          fCutFlowTesting = false; //for only testing basic cutflow sets
    Bool_t          fJetTauTesting = false; //for including MC j->tau sets in testing mode
    Bool_t          fTriggerTesting = false; //add emu histograms for electron/muon triggered events
    Bool_t          fDoMVASets = false; //for filling MVA cut sets even in DYTesting mode

    ClassDef(CLFVTmpHistMaker,0);

  };
}
#endif