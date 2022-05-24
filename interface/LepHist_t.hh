#ifndef __LEPHIST__HH
#define __LEPHIST__HH
#include "TH1.h"
#include "TH2.h"

namespace CLFV {
  struct LepHist_t {
    TH1* hJetTauOnePt[5/*tau DM*/]; //0: inclusive 1: DM 0 2: DM 1 3: DM 10 4: DM 11
    TH1* hJetTauOnePtQCD[2]; //0: nominal 1: no non-closure
    TH1* hJetTauOneR;
    TH1* hJetTauOneEta;
    TH2* hJetTauOnePtVsR[5/*tau DM + inclusive*/];
    TH1* hJetTauOneMetDeltaPhi[5/*tau DM + inclusive*/];
    TH1* hJetTauOneMetDPhiQCD[2]; //0: nominal 1: no j-->tau correction
    TH1* hJetTauOneRelIso[2]; //0: nominal 1: no bias correction

    TH1* hJetTauOneMetDPhiComp; //no j-->tau weights for composition
    TH1* hJetTauTwoMetDPhiComp;
    TH1* hJetTauOnePtComp;
    TH1* hJetTauTwoPtComp;
    TH1* hJetTauMTOneComp;
    TH1* hJetTauMTTwoComp;

    TH1* hOnePz;
    TH1* hOnePt[12]; //0: nominal 1: no trigger weight 2-5: trigger weight options 6: no ID1 weight 7: no ID2 weight
    TH1* hOneP;
    TH1* hOneM;
    TH1* hOnePtOverM;
    TH1* hOneEta;
    TH1* hOnePhi;
    TH1* hOneD0;
    TH1* hOneDXY;
    TH1* hOneDZ;
    TH1* hOneIso;
    TH1* hOneID1;
    TH1* hOneID2;
    TH1* hOneRelIso;
    TH1* hOneFlavor;
    TH1* hOneGenFlavor;
    TH1* hOneQ;
    TH1* hOneTrigger;
    TH1* hOneWeight;
    TH1* hOneWeight1Group;
    TH1* hOneWeight2Group;
    TH1* hOneTrigWeight;
    //Gen Info
    TH1* hOneGenPt;
    TH1* hOneGenE;
    TH1* hOneGenEta;
    TH1* hOneDeltaPt;
    TH1* hOneDeltaE;
    TH1* hOneDeltaEta;
    TH1* hOneMetDeltaPhi;
    //SVFit Info
    TH1* hOneSVPt;
    TH1* hOneSVM;
    TH1* hOneSVEta;
    TH1* hOneSVDeltaPt;
    TH1* hOneSVDeltaP;
    TH1* hOneSVDeltaE;
    TH1* hOneSVDeltaEta;
    TH1* hOneSlimEQ;
    TH1* hOneSlimMuQ;
    TH1* hOneSlimTauQ;
    TH1* hOneSlimEM;
    TH1* hOneSlimEMSS;
    TH1* hOneSlimEMOS;
    TH1* hOneSlimMuM;
    TH1* hOneSlimMuMSS;
    TH1* hOneSlimMuMOS;
    TH1* hOneSlimTauM;
    TH1* hOneSlimTauMSS;
    TH1* hOneSlimTauMOS;

    TH1* hJetTauTwoPt[5/*tau DM*/];
    TH1* hJetTauTwoR[2]; //0: nominal 1: no j-->tau correction
    TH1* hJetTauTwoEta[2]; //0: nominal 1: no j-->tau correction
    TH1* hJetTauTwoEtaQCD[2]; //0: nominal 1: no j-->tau correction
    TH2* hJetTauTwoPtVsR[5/*tau DM*/];
    TH1* hJetTauTwoMetDeltaPhi[2]; //0: nominal 1: no j-->tau correction
    TH1* hJetTauTwoRelIso[2]; //0: nominal 1: no bias correction

    TH1* hTwoPz;
    TH1* hTwoPt[12]; //0: nominal 1: no trigger weight 2-5: trigger weight options 6: no ID1 weight 7: no ID2 weight
    TH1* hTwoP;
    TH1* hTwoM;
    TH1* hTwoPtOverM;
    TH1* hTwoEta;
    TH1* hTwoPhi;
    TH1* hTwoD0;
    TH1* hTwoDXY;
    TH1* hTwoDZ;
    TH1* hTwoIso;
    TH1* hTwoID1;
    TH1* hTwoID2;
    TH1* hTwoID3;
    TH1* hTwoRelIso;
    TH1* hTwoFlavor;
    TH1* hTwoGenFlavor;
    TH1* hTwoQ;
    TH1* hTwoTrigger;
    TH1* hTwoWeight;
    TH1* hTwoWeight1Group;
    TH1* hTwoWeight2Group;
    TH1* hTwoTrigWeight;
    //Gen Info
    TH1* hTwoGenPt;
    TH1* hTwoGenE;
    TH1* hTwoGenEta;
    TH1* hTwoDeltaPt;
    TH1* hTwoDeltaE;
    TH1* hTwoDeltaEta;
    TH1* hTwoMetDeltaPhi;

    //SVFit Info
    TH1* hTwoSVPt;
    TH1* hTwoSVM;
    TH1* hTwoSVEta;
    TH1* hTwoSVDeltaPt;
    TH1* hTwoSVDeltaP;
    TH1* hTwoSVDeltaE;
    TH1* hTwoSVDeltaEta;

    TH1* hTwoSlimEQ;
    TH1* hTwoSlimMuQ;
    TH1* hTwoSlimTauQ;
    TH1* hTwoSlimEM;
    TH1* hTwoSlimEMSS;
    TH1* hTwoSlimEMOS;
    TH1* hTwoSlimMuM;
    TH1* hTwoSlimMuMSS;
    TH1* hTwoSlimMuMOS;
    TH1* hTwoSlimTauM;
    TH1* hTwoSlimTauMSS;
    TH1* hTwoSlimTauMOS;


    TH1* hPtDiff;
    TH1* hD0Diff;
    TH1* hDXYDiff;
    TH1* hDZDiff;

    //2D distribution
    TH2* hTwoPtVsOnePt;
    TH2* hJetTauTwoPtVsOnePt;

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
