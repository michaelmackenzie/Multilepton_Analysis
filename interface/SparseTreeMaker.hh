//TSelector to process TTrees output by Z_LFV_Analysis.git ntupling in NanoAOD format

#ifndef SPARSETREEMAKER_HH
#define SPARSETREEMAKER_HH

#include "interface/HistMaker.hh"

namespace CLFV {

  class SparseTreeMaker : public HistMaker {
  public :

    SparseTreeMaker(int seed = 90, TTree * /*tree*/ = 0);


    void    Begin(TTree*);
    void    InitHistogramFlags();
    void    InitializeInputTree(TTree* tree);
    void    BookOutputTree();
    void    BookHistograms();

    void    FillAllHistograms(Int_t hist_set);
    void    FillOutputTree();

    Bool_t  Process(Long64_t entry);

    TString GetOutputName() {
      return Form("sparse%s_%i_%s%s%s.tree",
                  (fSelection == "") ? "" : ("_"+fSelection).Data(),fYear, fDataset.Data(),
                  (fDYType >  0) ? Form("-%i",fDYType) : "",
                  (fWNJets >= 0) ? Form("-%i",fWNJets) : ""
                  );
    }


    TTree* fOutputTree;
    ClassDef(SparseTreeMaker,0);

  };
}
#endif
