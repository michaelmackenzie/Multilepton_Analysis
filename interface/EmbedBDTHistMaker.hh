//TSelector to process TTrees output by Z_LFV_Analysis.git ntupling in NanoAOD format
//Designed to make histograms for same flavor embedding studies

#ifndef EMBEDBDTHISTMAKER_HH
#define EMBEDBDTHISTMAKER_HH

#include "interface/HistMaker.hh"

namespace CLFV {

  class EmbedBDTHistMaker : public HistMaker {
  public :

    EmbedBDTHistMaker(int seed = 90, TTree * /*tree*/ = 0);
    ~EmbedBDTHistMaker();


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

    void    InitializeMVAs();
    TString GetOutputName() {
      return Form("emb%s_%i_%s%s%s.hist",
                  (fSelection == "") ? "" : ("_"+fSelection).Data(),fYear, fDataset.Data(),
                  (fDYType >  0) ? Form("-%i",fDYType) : "",
                  (fWNJets >= 0) ? Form("-%i",fWNJets) : ""
                  );
    }

    ClassDef(EmbedBDTHistMaker,0);

  };
}
#endif
