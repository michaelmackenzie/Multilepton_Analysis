//TSelector to process TTrees output by Z_LFV_Analysis.git ntupling in NanoAOD format

#ifndef SPARSEHISTMAKER_HH
#define SPARSEHISTMAKER_HH

#include "interface/HistMaker.hh"

namespace CLFV {

  class SparseHistMaker : public HistMaker {
  public :

    SparseHistMaker(int seed = 90, TTree * /*tree*/ = 0);


    void    Begin(TTree*);
    void    InitHistogramFlags();
    void    BookHistograms();
    void    BookEventHistograms();
    void    BookLepHistograms();

    void    FillEventHistogram(EventHist_t* Hist);
    void    FillLepHistogram(LepHist_t* Hist);

    Bool_t  Process(Long64_t entry);

    TString GetOutputName() {
      return Form("sparse%s_%i_%s%s%s.hist",
                  (fSelection == "") ? "" : ("_"+fSelection).Data(),fYear, fDataset.Data(),
                  (fDYType >  0) ? Form("-%i",fDYType) : "",
                  (fWNJets >= 0) ? Form("-%i",fWNJets) : ""
                  );
    }



    ClassDef(SparseHistMaker,0);

  };
}
#endif
