//TSelector to process TTrees output by Z_LFV_Analysis.git ntupling in NanoAOD format
//Designed to make histograms for Drell-Yan MC spectrum correction measurements and studies

#ifndef ZPTHISTMAKER_HH
#define ZPTHISTMAKER_HH

#include "interface/HistMaker.hh"

namespace CLFV {

  class ZPtHistMaker : public HistMaker {
  public :

    ZPtHistMaker(int seed = 90, TTree * /*tree*/ = 0);


    void    Begin(TTree*);
    void    Init(TTree* tree);
    void    InitHistogramFlags();
    void    BookHistograms();
    void    BookEventHistograms();
    void    BookLepHistograms();

    void    FillEventHistogram(EventHist_t* Hist);
    void    FillLepHistogram(LepHist_t* Hist);

    Bool_t  Process(Long64_t entry);

    TString GetOutputName() {
      return Form("zpt%s_%i_%s%s%s.hist",
                  (fSelection == "") ? "" : ("_"+fSelection).Data(),fYear, fDataset.Data(),
                  (fDYType >  0) ? Form("-%i",fDYType) : "",
                  (fWNJets >= 0) ? Form("-%i",fWNJets) : ""
                  );
    }



    ClassDef(ZPtHistMaker,0);

  };
}
#endif
