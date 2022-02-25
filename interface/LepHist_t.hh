#ifndef __LEPHIST__HH
#define __LEPHIST__HH
#include "TH1.h"
#include "TH2.h"

namespace CLFV {
  struct LepHist_t {
    TH1* hOnePz = 0;
    TH1* hOnePt[12]; //0: nominal 1: no trigger weight 2-5: trigger weight options 6: no ID1 weight 7: no ID2 weight
    TH1* hJetTauOnePt[5/*tau DM*/][4/*tau pT range*/];
    TH1* hJetTauOneR = 0;
    TH1* hJetTauOneEta = 0;
    TH2* hJetTauOnePtVsR[5/*tau DM + inclusive*/];
    TH1* hJetTauOneMetDeltaPhi[5/*tau DM + inclusive*/];
    TH1* hJetTauCompOnePt = 0;
    TH1* hOneP = 0;
    TH1* hOneM = 0;
    TH1* hOnePtOverM = 0;
    TH1* hOneEta = 0;
    TH1* hOnePhi = 0;
    TH1* hOneD0 = 0;
    TH1* hOneIso = 0;
    TH1* hOneID1 = 0;
    TH1* hOneID2 = 0;
    TH1* hOneRelIso = 0;
    TH1* hOneFlavor = 0;
    TH1* hOneGenFlavor = 0;
    TH1* hOneQ = 0;
    TH1* hOneTrigger = 0;
    TH1* hOneWeight = 0;
    TH1* hOneWeight1Group = 0;
    TH1* hOneWeight2Group = 0;
    TH1* hOneTrigWeight = 0;
    //Gen Info
    TH1* hOneGenPt = 0;
    TH1* hOneGenE = 0;
    TH1* hOneGenEta = 0;
    TH1* hOneDeltaPt = 0;
    TH1* hOneDeltaE = 0;
    TH1* hOneDeltaEta = 0;
    TH1* hOneMetDeltaPhi = 0;
    //SVFit Info
    TH1* hOneSVPt = 0;
    TH1* hOneSVM = 0;
    TH1* hOneSVEta = 0;
    TH1* hOneSVDeltaPt = 0;
    TH1* hOneSVDeltaP = 0;
    TH1* hOneSVDeltaE = 0;
    TH1* hOneSVDeltaEta = 0;
    TH1* hOneSlimEQ = 0;
    TH1* hOneSlimMuQ = 0;
    TH1* hOneSlimTauQ = 0;
    TH1* hOneSlimEM = 0;
    TH1* hOneSlimEMSS = 0;
    TH1* hOneSlimEMOS = 0;
    TH1* hOneSlimMuM = 0;
    TH1* hOneSlimMuMSS = 0;
    TH1* hOneSlimMuMOS = 0;
    TH1* hOneSlimTauM = 0;
    TH1* hOneSlimTauMSS = 0;
    TH1* hOneSlimTauMOS = 0;

    TH1* hTwoPz = 0;
    TH1* hTwoPt[12]; //0: nominal 1: no trigger weight 2-5: trigger weight options 6: no ID1 weight 7: no ID2 weight
    TH1* hJetTauTwoPt[5/*tau DM*/][4/*tau pT range*/];
    TH1* hJetTauTwoR = 0;
    TH1* hJetTauTwoEta = 0;
    TH2* hJetTauTwoPtVsR[5/*tau DM*/];
    TH1* hJetTauTwoMetDeltaPhi = 0;
    TH1* hJetTauCompTwoPt = 0;
    TH1* hTwoP = 0;
    TH1* hTwoM = 0;
    TH1* hTwoPtOverM = 0;
    TH1* hTwoEta = 0;
    TH1* hTwoPhi = 0;
    TH1* hTwoD0 = 0;
    TH1* hTwoIso = 0;
    TH1* hTwoID1 = 0;
    TH1* hTwoID2 = 0;
    TH1* hTwoID3 = 0;
    TH1* hTwoRelIso = 0;
    TH1* hTwoFlavor = 0;
    TH1* hTwoGenFlavor = 0;
    TH1* hTwoQ = 0;
    TH1* hTwoTrigger = 0;
    TH1* hTwoWeight = 0;
    TH1* hTwoWeight1Group = 0;
    TH1* hTwoWeight2Group = 0;
    TH1* hTwoTrigWeight = 0;
    //Gen Info
    TH1* hTwoGenPt = 0;
    TH1* hTwoGenE = 0;
    TH1* hTwoGenEta = 0;
    TH1* hTwoDeltaPt = 0;
    TH1* hTwoDeltaE = 0;
    TH1* hTwoDeltaEta = 0;
    TH1* hTwoMetDeltaPhi = 0;

    //SVFit Info
    TH1* hTwoSVPt = 0;
    TH1* hTwoSVM = 0;
    TH1* hTwoSVEta = 0;
    TH1* hTwoSVDeltaPt = 0;
    TH1* hTwoSVDeltaP = 0;
    TH1* hTwoSVDeltaE = 0;
    TH1* hTwoSVDeltaEta = 0;

    TH1* hTwoSlimEQ = 0;
    TH1* hTwoSlimMuQ = 0;
    TH1* hTwoSlimTauQ = 0;
    TH1* hTwoSlimEM = 0;
    TH1* hTwoSlimEMSS = 0;
    TH1* hTwoSlimEMOS = 0;
    TH1* hTwoSlimMuM = 0;
    TH1* hTwoSlimMuMSS = 0;
    TH1* hTwoSlimMuMOS = 0;
    TH1* hTwoSlimTauM = 0;
    TH1* hTwoSlimTauMSS = 0;
    TH1* hTwoSlimTauMOS = 0;


    TH1* hPtDiff = 0;
    TH1* hD0Diff = 0;

    //2D distribution
    TH2* hTwoPtVsOnePt = 0;
    TH2* hJetTauTwoPtVsOnePt = 0;

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
