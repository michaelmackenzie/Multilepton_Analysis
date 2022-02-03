#ifndef __LEPHIST__HH
#define __LEPHIST__HH
#include "TH1.h"
#include "TH2.h"

namespace CLFV {
  struct LepHist_t {
    TH1D* hOnePz = 0;
    TH1D* hOnePt[12]; //0: nominal 1: no trigger weight 2-5: trigger weight options 6: no ID1 weight 7: no ID2 weight
    TH1D* hJetTauOnePt[5/*tau DM*/][4/*tau pT range*/];
    TH1D* hJetTauOneR = 0;
    TH1D* hJetTauOneEta = 0;
    TH2D* hJetTauOnePtVsR[5/*tau DM + inclusive*/];
    TH1D* hJetTauOneMetDeltaPhi[5/*tau DM + inclusive*/];
    TH1D* hJetTauCompOnePt = 0;
    TH1D* hOneP = 0;
    TH1D* hOneM = 0;
    TH1D* hOnePtOverM = 0;
    TH1D* hOneEta = 0;
    TH1D* hOnePhi = 0;
    TH1D* hOneD0 = 0;
    TH1D* hOneIso = 0;
    TH1D* hOneID1 = 0;
    TH1D* hOneID2 = 0;
    TH1D* hOneRelIso = 0;
    TH1D* hOneFlavor = 0;
    TH1D* hOneGenFlavor = 0;
    TH1D* hOneQ = 0;
    TH1D* hOneTrigger = 0;
    TH1D* hOneWeight = 0;
    TH1D* hOneWeight1Group = 0;
    TH1D* hOneWeight2Group = 0;
    TH1D* hOneTrigWeight = 0;
    //Gen Info
    TH1D* hOneGenPt = 0;
    TH1D* hOneGenE = 0;
    TH1D* hOneGenEta = 0;
    TH1D* hOneDeltaPt = 0;
    TH1D* hOneDeltaE = 0;
    TH1D* hOneDeltaEta = 0;
    TH1D* hOneMetDeltaPhi = 0;
    //SVFit Info
    TH1D* hOneSVPt = 0;
    TH1D* hOneSVM = 0;
    TH1D* hOneSVEta = 0;
    TH1D* hOneSVDeltaPt = 0;
    TH1D* hOneSVDeltaP = 0;
    TH1D* hOneSVDeltaE = 0;
    TH1D* hOneSVDeltaEta = 0;
    TH1D* hOneSlimEQ = 0;
    TH1D* hOneSlimMuQ = 0;
    TH1D* hOneSlimTauQ = 0;
    TH1D* hOneSlimEM = 0;
    TH1D* hOneSlimEMSS = 0;
    TH1D* hOneSlimEMOS = 0;
    TH1D* hOneSlimMuM = 0;
    TH1D* hOneSlimMuMSS = 0;
    TH1D* hOneSlimMuMOS = 0;
    TH1D* hOneSlimTauM = 0;
    TH1D* hOneSlimTauMSS = 0;
    TH1D* hOneSlimTauMOS = 0;

    TH1D* hTwoPz = 0;
    TH1D* hTwoPt[12]; //0: nominal 1: no trigger weight 2-5: trigger weight options 6: no ID1 weight 7: no ID2 weight
    TH1D* hJetTauTwoPt[5/*tau DM*/][4/*tau pT range*/];
    TH1D* hJetTauTwoR = 0;
    TH1D* hJetTauTwoEta = 0;
    TH2D* hJetTauTwoPtVsR[5/*tau DM*/];
    TH1D* hJetTauTwoMetDeltaPhi = 0;
    TH1D* hJetTauCompTwoPt = 0;
    TH1D* hTwoP = 0;
    TH1D* hTwoM = 0;
    TH1D* hTwoPtOverM = 0;
    TH1D* hTwoEta = 0;
    TH1D* hTwoPhi = 0;
    TH1D* hTwoD0 = 0;
    TH1D* hTwoIso = 0;
    TH1D* hTwoID1 = 0;
    TH1D* hTwoID2 = 0;
    TH1D* hTwoID3 = 0;
    TH1D* hTwoRelIso = 0;
    TH1D* hTwoFlavor = 0;
    TH1D* hTwoGenFlavor = 0;
    TH1D* hTwoQ = 0;
    TH1D* hTwoTrigger = 0;
    TH1D* hTwoWeight = 0;
    TH1D* hTwoWeight1Group = 0;
    TH1D* hTwoWeight2Group = 0;
    TH1D* hTwoTrigWeight = 0;
    //Gen Info
    TH1D* hTwoGenPt = 0;
    TH1D* hTwoGenE = 0;
    TH1D* hTwoGenEta = 0;
    TH1D* hTwoDeltaPt = 0;
    TH1D* hTwoDeltaE = 0;
    TH1D* hTwoDeltaEta = 0;
    TH1D* hTwoMetDeltaPhi = 0;

    //SVFit Info
    TH1D* hTwoSVPt = 0;
    TH1D* hTwoSVM = 0;
    TH1D* hTwoSVEta = 0;
    TH1D* hTwoSVDeltaPt = 0;
    TH1D* hTwoSVDeltaP = 0;
    TH1D* hTwoSVDeltaE = 0;
    TH1D* hTwoSVDeltaEta = 0;

    TH1D* hTwoSlimEQ = 0;
    TH1D* hTwoSlimMuQ = 0;
    TH1D* hTwoSlimTauQ = 0;
    TH1D* hTwoSlimEM = 0;
    TH1D* hTwoSlimEMSS = 0;
    TH1D* hTwoSlimEMOS = 0;
    TH1D* hTwoSlimMuM = 0;
    TH1D* hTwoSlimMuMSS = 0;
    TH1D* hTwoSlimMuMOS = 0;
    TH1D* hTwoSlimTauM = 0;
    TH1D* hTwoSlimTauMSS = 0;
    TH1D* hTwoSlimTauMOS = 0;


    TH1D* hPtDiff = 0;
    TH1D* hD0Diff = 0;

    //2D distribution
    TH2D* hTwoPtVsOnePt = 0;
    TH2D* hJetTauTwoPtVsOnePt = 0;

    // ~LepHist_t() {
    //   // if(hOnePz                          ) delete hOnePz                          ;
    //   if(hJetTauOneR                     ) delete hJetTauOneR                     ;
    //   if(hJetTauOneEta                   ) delete hJetTauOneEta                   ;
    //   if(hJetTauCompOnePt                ) delete hJetTauCompOnePt                ;
    //   if(hOneP                           ) delete hOneP                           ;
    //   if(hOneM                           ) delete hOneM                           ;
    //   if(hOnePtOverM                     ) delete hOnePtOverM                     ;
    //   if(hOneEta                         ) delete hOneEta                         ;
    //   if(hOnePhi                         ) delete hOnePhi                         ;
    //   if(hOneD0                          ) delete hOneD0                          ;
    //   if(hOneIso                         ) delete hOneIso                         ;
    //   if(hOneID1                         ) delete hOneID1                         ;
    //   if(hOneID2                         ) delete hOneID2                         ;
    //   if(hOneRelIso                      ) delete hOneRelIso                      ;
    //   if(hOneFlavor                      ) delete hOneFlavor                      ;
    //   if(hOneGenFlavor                   ) delete hOneGenFlavor                   ;
    //   if(hOneQ                           ) delete hOneQ                           ;
    //   if(hOneTrigger                     ) delete hOneTrigger                     ;
    //   if(hOneWeight                      ) delete hOneWeight                      ;
    //   if(hOneWeight1Group                ) delete hOneWeight1Group                ;
    //   if(hOneWeight2Group                ) delete hOneWeight2Group                ;
    //   if(hOneTrigWeight                  ) delete hOneTrigWeight                  ;
    //   if(hOneGenPt                       ) delete hOneGenPt                       ;
    //   if(hOneGenE                        ) delete hOneGenE                        ;
    //   if(hOneGenEta                      ) delete hOneGenEta                      ;
    //   if(hOneDeltaPt                     ) delete hOneDeltaPt                     ;
    //   if(hOneDeltaE                      ) delete hOneDeltaE                      ;
    //   if(hOneDeltaEta                    ) delete hOneDeltaEta                    ;
    //   if(hOneMetDeltaPhi                 ) delete hOneMetDeltaPhi                 ;
    //   if(hOneSVPt                        ) delete hOneSVPt                        ;
    //   if(hOneSVM                         ) delete hOneSVM                         ;
    //   if(hOneSVEta                       ) delete hOneSVEta                       ;
    //   if(hOneSVDeltaPt                   ) delete hOneSVDeltaPt                   ;
    //   if(hOneSVDeltaP                    ) delete hOneSVDeltaP                    ;
    //   if(hOneSVDeltaE                    ) delete hOneSVDeltaE                    ;
    //   if(hOneSVDeltaEta                  ) delete hOneSVDeltaEta                  ;
    //   if(hOneSlimEQ                      ) delete hOneSlimEQ                      ;
    //   if(hOneSlimMuQ                     ) delete hOneSlimMuQ                     ;
    //   if(hOneSlimTauQ                    ) delete hOneSlimTauQ                    ;
    //   if(hOneSlimEM                      ) delete hOneSlimEM                      ;
    //   if(hOneSlimEMSS                    ) delete hOneSlimEMSS                    ;
    //   if(hOneSlimEMOS                    ) delete hOneSlimEMOS                    ;
    //   if(hOneSlimMuM                     ) delete hOneSlimMuM                     ;
    //   if(hOneSlimMuMSS                   ) delete hOneSlimMuMSS                   ;
    //   if(hOneSlimMuMOS                   ) delete hOneSlimMuMOS                   ;
    //   if(hOneSlimTauM                    ) delete hOneSlimTauM                    ;
    //   if(hOneSlimTauMSS                  ) delete hOneSlimTauMSS                  ;
    //   if(hOneSlimTauMOS                  ) delete hOneSlimTauMOS                  ;
    //   if(hTwoPz                          ) delete hTwoPz                          ;
    //   if(hJetTauTwoR                     ) delete hJetTauTwoR                     ;
    //   if(hJetTauTwoEta                   ) delete hJetTauTwoEta                   ;
    //   if(hJetTauTwoMetDeltaPhi           ) delete hJetTauTwoMetDeltaPhi           ;
    //   if(hJetTauCompTwoPt                ) delete hJetTauCompTwoPt                ;
    //   if(hTwoP                           ) delete hTwoP                           ;
    //   if(hTwoM                           ) delete hTwoM                           ;
    //   if(hTwoPtOverM                     ) delete hTwoPtOverM                     ;
    //   if(hTwoEta                         ) delete hTwoEta                         ;
    //   if(hTwoPhi                         ) delete hTwoPhi                         ;
    //   if(hTwoD0                          ) delete hTwoD0                          ;
    //   if(hTwoIso                         ) delete hTwoIso                         ;
    //   if(hTwoID1                         ) delete hTwoID1                         ;
    //   if(hTwoID2                         ) delete hTwoID2                         ;
    //   if(hTwoID3                         ) delete hTwoID3                         ;
    //   if(hTwoRelIso                      ) delete hTwoRelIso                      ;
    //   if(hTwoFlavor                      ) delete hTwoFlavor                      ;
    //   if(hTwoGenFlavor                   ) delete hTwoGenFlavor                   ;
    //   if(hTwoQ                           ) delete hTwoQ                           ;
    //   if(hTwoTrigger                     ) delete hTwoTrigger                     ;
    //   if(hTwoWeight                      ) delete hTwoWeight                      ;
    //   if(hTwoWeight1Group                ) delete hTwoWeight1Group                ;
    //   if(hTwoWeight2Group                ) delete hTwoWeight2Group                ;
    //   if(hTwoTrigWeight                  ) delete hTwoTrigWeight                  ;
    //   if(hTwoGenPt                       ) delete hTwoGenPt                       ;
    //   if(hTwoGenE                        ) delete hTwoGenE                        ;
    //   if(hTwoGenEta                      ) delete hTwoGenEta                      ;
    //   if(hTwoDeltaPt                     ) delete hTwoDeltaPt                     ;
    //   if(hTwoDeltaE                      ) delete hTwoDeltaE                      ;
    //   if(hTwoDeltaEta                    ) delete hTwoDeltaEta                    ;
    //   if(hTwoMetDeltaPhi                 ) delete hTwoMetDeltaPhi                 ;
    //   if(hTwoSVPt                        ) delete hTwoSVPt                        ;
    //   if(hTwoSVM                         ) delete hTwoSVM                         ;
    //   if(hTwoSVEta                       ) delete hTwoSVEta                       ;
    //   if(hTwoSVDeltaPt                   ) delete hTwoSVDeltaPt                   ;
    //   if(hTwoSVDeltaP                    ) delete hTwoSVDeltaP                    ;
    //   if(hTwoSVDeltaE                    ) delete hTwoSVDeltaE                    ;
    //   if(hTwoSVDeltaEta                  ) delete hTwoSVDeltaEta                  ;
    //   if(hTwoSlimEQ                      ) delete hTwoSlimEQ                      ;
    //   if(hTwoSlimMuQ                     ) delete hTwoSlimMuQ                     ;
    //   if(hTwoSlimTauQ                    ) delete hTwoSlimTauQ                    ;
    //   if(hTwoSlimEM                      ) delete hTwoSlimEM                      ;
    //   if(hTwoSlimEMSS                    ) delete hTwoSlimEMSS                    ;
    //   if(hTwoSlimEMOS                    ) delete hTwoSlimEMOS                    ;
    //   if(hTwoSlimMuM                     ) delete hTwoSlimMuM                     ;
    //   if(hTwoSlimMuMSS                   ) delete hTwoSlimMuMSS                   ;
    //   if(hTwoSlimMuMOS                   ) delete hTwoSlimMuMOS                   ;
    //   if(hTwoSlimTauM                    ) delete hTwoSlimTauM                    ;
    //   if(hTwoSlimTauMSS                  ) delete hTwoSlimTauMSS                  ;
    //   if(hTwoSlimTauMOS                  ) delete hTwoSlimTauMOS                  ;
    //   if(hPtDiff                         ) delete hPtDiff                         ;
    //   if(hD0Diff                         ) delete hD0Diff                         ;
    //   if(hTwoPtVsOnePt                   ) delete hTwoPtVsOnePt                   ;
    //   if(hJetTauTwoPtVsOnePt             ) delete hJetTauTwoPtVsOnePt             ;

    //   for(int i = 0; i < 12; ++i) {
    //     if(hOnePt[i]                     ) delete hOnePt[i]                       ;
    //     if(hTwoPt[i]                     ) delete hTwoPt[i]                       ;
    //   }
    //   for(int i = 0; i < 5; ++i) {
    //     if(hJetTauOnePtVsR[i]            ) delete hJetTauOnePtVsR[i]              ;
    //     if(hJetTauTwoPtVsR[i]            ) delete hJetTauTwoPtVsR[i]              ;
    //     if(hJetTauOneMetDeltaPhi[i]      ) delete hJetTauOneMetDeltaPhi[i]        ;
    //     for(int j = 0; j < 4; ++j) {
    //       if(hJetTauOnePt[i][j]          ) delete hJetTauOnePt[i][j]              ;
    //       if(hJetTauTwoPt[i][j]          ) delete hJetTauTwoPt[i][j]              ;
    //     }
    //   }
    // }
  };
}
#endif
