//TSelector to process TTrees output by Z_LFV_Analysis.git ntupling in NanoAOD format
//Designed to make histograms for QCD background measurements and studies

#ifndef QCDHISTMAKER_HH
#define QCDHISTMAKER_HH

#include "interface/HistMaker.hh"

namespace CLFV {

  class QCDHistMaker : public HistMaker {
  public :

    QCDHistMaker(int seed = 90, TTree * /*tree*/ = 0);


    void    Begin(TTree*);
    void    InitHistogramFlags();
    void    BookHistograms();
    void    BookEventHistograms();
    void    BookLepHistograms();

    void    FillEventHistogram(EventHist_t* Hist);
    void    FillLepHistogram(LepHist_t* Hist);

    Bool_t  Process(Long64_t entry);

    TString GetOutputName() {
      return Form("qcd%s_%i_%s%s%s.hist",
                  (fSelection == "") ? "" : ("_"+fSelection).Data(),fYear, fDataset.Data(),
                  (fDYType >  0) ? Form("-%i",fDYType) : "",
                  (fWNJets >= 0) ? Form("-%i",fWNJets) : ""
                  );
    }


    QCDWeight* fMuTauQCDWeight; //weights with the mutau_e or etau_mu (mass, bdt score) bias weights included
    QCDWeight* fETauQCDWeight ;

    ClassDef(QCDHistMaker,0);

  };
}
#endif
