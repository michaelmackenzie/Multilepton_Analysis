//TSelector to process TTrees output by Z_LFV_Analysis.git ntupling in NanoAOD format
//Designed to make histograms for j-->tau background measurements and studies

#ifndef JTTHISTMAKER_HH
#define JTTHISTMAKER_HH

#include "interface/HistMaker.hh"

namespace CLFV {

  class JTTHistMaker : public HistMaker {
  public :

    JTTHistMaker(int seed = 90, TTree * /*tree*/ = 0);


    void    Begin(TTree*);
    void    InitHistogramFlags();
    void    BookHistograms();
    void    BookEventHistograms();
    void    BookLepHistograms();

    void    FillEventHistogram(EventHist_t* Hist);
    void    FillLepHistogram(LepHist_t* Hist);

    Bool_t  Process(Long64_t entry);

    TString GetOutputName() {
      return Form("jtt%s_%i_%s%s%s.hist",
                  (fSelection == "") ? "" : ("_"+fSelection).Data(),fYear, fDataset.Data(),
                  (fDYType >  0) ? Form("-%i",fDYType) : "",
                  (fWNJets >= 0) ? Form("-%i",fWNJets) : ""
                  );
    }



    ClassDef(JTTHistMaker,0);

  };
}
#endif