#ifndef __BTAGWEIGHT__
#define __BTAGWEIGHT__
//Class to handle pileup weight of MC processes
//c++ includes
#include <map>
#include <iostream>
//ROOT includes
#include "TFile.h"
#include "TH2.h"
#include "TRandom3.h"

class BTagWeight {
public:
  BTagWeight(int seed = 90) {
    TFile* f = 0;
    std::vector<int> working_points = {kLooseBTag, kMediumBTag, kTightBTag};
    std::vector<int> years = {2016, 2017, 2018};
    rnd_ = new TRandom3(seed);
    for(int wp : working_points) {
      for(int year : years) {
        f = TFile::Open(Form("../scale_factors/btag_eff_wp_%i_mumu_%i.root", wp, year), "READ");
        if(!f)
          f = TFile::Open(Form("scale_factors/btag_eff_wp_%i_mumu_%i.root", wp, year), "READ");
        if(f) {
          histsL_[year][wp] = (TH2D*) f->Get("hLRatio")->Clone(Form("hL_%i_%i", year, wp));
          if(!histsL_[year][wp]) std::cout << "BTagWeight::BTagWeight: Warning! No light quark histogram found for wp = "
                                           << wp << " year = " << year << std::endl;
          histsC_[year][wp] = (TH2D*) f->Get("hCRatio")->Clone(Form("hC_%i_%i", year, wp));
          if(!histsL_[year][wp]) std::cout << "BTagWeight::BTagWeight: Warning! No light quark histogram found for wp = "
                                           << wp << " year = " << year << std::endl;
          histsB_[year][wp] = (TH2D*) f->Get("hBRatio")->Clone(Form("hB_%i_%i", year, wp));
          if(!histsL_[year][wp]) std::cout << "BTagWeight::BTagWeight: Warning! No light quark histogram found for wp = "
                                           << wp << " year = " << year << std::endl;
          files_.push_back(f); //to close later
        }
      }
    }
  }
  ~BTagWeight() { for(unsigned i = 0; i < files_.size(); ++i) delete files_[i]; }

  float GetMCEff(int WP, int year, float jetpt, float jeteta, int jetflavor) {
    TH2D* h = 0;
    if(jetpt > 999.) jetpt = 999.;
    else if(jetpt < 20.) jetpt = 20.;
    if(fabs(jeteta) > 2.4) jeteta = 2.39*jeteta/fabs(jeteta);
    if(abs(jetflavor) == 4) //c-quark
      h = histsC_[year][WP];
    else if(abs(jetflavor) == 5) //b-quark
      h = histsB_[year][WP];
    else //light quark
      h = histsL_[year][WP];
    if(!h) {
      std::cout << "BTagWeight::" << __func__ << " Undefined histogram for wp = "
                << WP << " year = " << year << std::endl;
      return 1.;
    }
    int binx = h->GetXaxis()->FindBin(jeteta);
    int biny = h->GetYaxis()->FindBin(jetpt);
    float eff = h->GetBinContent(binx, biny);
    if(eff < 0.) {
      std::cout << "BTagWeight::" << __func__ << ": Warning! MC Eff < 0 = " << eff
                << " jetpt = " << jetpt << " jeteta = " << jeteta
                << " jetflavor = " << jetflavor << std::endl;
      eff = 0.000001;
    }
    return eff;
  }

  void GetSystematic(int WP, int year, float jetpt, int jetFlavor,
                     float& up, float& down, float& sys) {
    float x(jetpt);
    jetFlavor = abs(jetFlavor);
    if(x > 999.) x = 999.; //maximum pT
    else if(x < 20.) x = 20.;
    up = 1.; down = 1.; sys = 1.;

    int flavor = 2; //u,d,s,g
    if(jetFlavor == 5) flavor = 0; //true b-jet
    else if(jetFlavor == 4) flavor = 1; //true c-jet

    if(year == 2018) {
      //FIXME: Change to else if and evaluate base formula at beginning, add systematic offset after
      if(WP == kLooseBTag  && flavor == 1 && x > 20  && x < 30  ) down = 0.873139+((0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18))))))-0.11263331770896912)   ;
      if(WP == kLooseBTag  && flavor == 1 && x > 30  && x < 50  ) down = 0.873139+((0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18))))))-0.035774830728769302)  ;
      if(WP == kLooseBTag  && flavor == 1 && x > 50  && x < 70  ) down = 0.873139+((0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18))))))-0.048977665603160858)  ;
      if(WP == kLooseBTag  && flavor == 1 && x > 70  && x < 100 ) down = 0.873139+((0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18))))))-0.029415970668196678)  ;
      if(WP == kLooseBTag  && flavor == 1 && x > 100 && x < 140 ) down = 0.873139+((0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18))))))-0.02761555090546608)   ;
      if(WP == kLooseBTag  && flavor == 1 && x > 140 && x < 200 ) down = 0.873139+((0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18))))))-0.027591491118073463)  ;
      if(WP == kLooseBTag  && flavor == 1 && x > 200 && x < 300 ) down = 0.873139+((0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18))))))-0.030103476718068123)  ;
      if(WP == kLooseBTag  && flavor == 1 && x > 300 && x < 600 ) down = 0.873139+((0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18))))))-0.10650250315666199)   ;
      if(WP == kLooseBTag  && flavor == 1 && x > 600 && x < 1000) down = 0.873139+((0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18))))))-0.22497090697288513)   ;
      if(WP == kLooseBTag  && flavor == 0 && x > 20  && x < 30  ) down = 0.873139+((0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18))))))-0.045053325593471527)  ;
      if(WP == kLooseBTag  && flavor == 0 && x > 30  && x < 50  ) down = 0.873139+((0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18))))))-0.014309932477772236)  ;
      if(WP == kLooseBTag  && flavor == 0 && x > 50  && x < 70  ) down = 0.873139+((0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18))))))-0.019591066986322403)  ;
      if(WP == kLooseBTag  && flavor == 0 && x > 70  && x < 100 ) down = 0.873139+((0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18))))))-0.011766388081014156)  ;
      if(WP == kLooseBTag  && flavor == 0 && x > 100 && x < 140 ) down = 0.873139+((0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18))))))-0.011046220548450947)  ;
      if(WP == kLooseBTag  && flavor == 0 && x > 140 && x < 200 ) down = 0.873139+((0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18))))))-0.01103659626096487)   ;
      if(WP == kLooseBTag  && flavor == 0 && x > 200 && x < 300 ) down = 0.873139+((0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18))))))-0.012041390873491764)  ;
      if(WP == kLooseBTag  && flavor == 0 && x > 300 && x < 600 ) down = 0.873139+((0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18))))))-0.042601000517606735)  ;
      if(WP == kLooseBTag  && flavor == 0 && x > 600 && x < 1000) down = 0.873139+((0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18))))))-0.089988365769386292)  ;
      if(WP == kLooseBTag  && flavor == 1 && x > 20  && x < 30  ) up   = (0.873139+(0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18)))))))+0.11263331770896912   ;
      if(WP == kLooseBTag  && flavor == 1 && x > 30  && x < 50  ) up   = (0.873139+(0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18)))))))+0.035774830728769302  ;
      if(WP == kLooseBTag  && flavor == 1 && x > 50  && x < 70  ) up   = (0.873139+(0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18)))))))+0.048977665603160858  ;
      if(WP == kLooseBTag  && flavor == 1 && x > 70  && x < 100 ) up   = (0.873139+(0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18)))))))+0.029415970668196678  ;
      if(WP == kLooseBTag  && flavor == 1 && x > 100 && x < 140 ) up   = (0.873139+(0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18)))))))+0.02761555090546608   ;
      if(WP == kLooseBTag  && flavor == 1 && x > 140 && x < 200 ) up   = (0.873139+(0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18)))))))+0.027591491118073463  ;
      if(WP == kLooseBTag  && flavor == 1 && x > 200 && x < 300 ) up   = (0.873139+(0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18)))))))+0.030103476718068123  ;
      if(WP == kLooseBTag  && flavor == 1 && x > 300 && x < 600 ) up   = (0.873139+(0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18)))))))+0.10650250315666199   ;
      if(WP == kLooseBTag  && flavor == 1 && x > 600 && x < 1000) up   = (0.873139+(0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18)))))))+0.22497090697288513   ;
      if(WP == kLooseBTag  && flavor == 0 && x > 20  && x < 30  ) up   = (0.873139+(0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18)))))))+0.045053325593471527  ;
      if(WP == kLooseBTag  && flavor == 0 && x > 30  && x < 50  ) up   = (0.873139+(0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18)))))))+0.014309932477772236  ;
      if(WP == kLooseBTag  && flavor == 0 && x > 50  && x < 70  ) up   = (0.873139+(0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18)))))))+0.019591066986322403  ;
      if(WP == kLooseBTag  && flavor == 0 && x > 70  && x < 100 ) up   = (0.873139+(0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18)))))))+0.011766388081014156  ;
      if(WP == kLooseBTag  && flavor == 0 && x > 100 && x < 140 ) up   = (0.873139+(0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18)))))))+0.011046220548450947  ;
      if(WP == kLooseBTag  && flavor == 0 && x > 140 && x < 200 ) up   = (0.873139+(0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18)))))))+0.01103659626096487   ;
      if(WP == kLooseBTag  && flavor == 0 && x > 200 && x < 300 ) up   = (0.873139+(0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18)))))))+0.012041390873491764  ;
      if(WP == kLooseBTag  && flavor == 0 && x > 300 && x < 600 ) up   = (0.873139+(0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18)))))))+0.042601000517606735  ;
      if(WP == kLooseBTag  && flavor == 0 && x > 600 && x < 1000) up   = (0.873139+(0.00420739*(log(x+19)*(log(x+18)*(3-(0.380932*log(x+18)))))))+0.089988365769386292  ;

      if(WP == kMediumBTag && flavor == 1 && x > 20  && x < 30  ) down = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))-0.13967660069465637   ;
      if(WP == kMediumBTag && flavor == 1 && x > 30  && x < 50  ) down = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))-0.049122259020805359  ;
      if(WP == kMediumBTag && flavor == 1 && x > 50  && x < 70  ) down = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))-0.043598789721727371  ;
      if(WP == kMediumBTag && flavor == 1 && x > 70  && x < 100 ) down = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))-0.038782715797424316  ;
      if(WP == kMediumBTag && flavor == 1 && x > 100 && x < 140 ) down = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))-0.036949444562196732  ;
      if(WP == kMediumBTag && flavor == 1 && x > 140 && x < 200 ) down = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))-0.043523617088794708  ;
      if(WP == kMediumBTag && flavor == 1 && x > 200 && x < 300 ) down = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))-0.04994809627532959   ;
      if(WP == kMediumBTag && flavor == 1 && x > 300 && x < 600 ) down = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))-0.090836621820926666  ;
      if(WP == kMediumBTag && flavor == 1 && x > 600 && x < 1000) down = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))-0.16102308034896851   ;
      if(WP == kMediumBTag && flavor == 0 && x > 20  && x < 30  ) down = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))-0.046558864414691925  ;
      if(WP == kMediumBTag && flavor == 0 && x > 30  && x < 50  ) down = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))-0.016374086961150169  ;
      if(WP == kMediumBTag && flavor == 0 && x > 50  && x < 70  ) down = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))-0.014532930217683315  ;
      if(WP == kMediumBTag && flavor == 0 && x > 70  && x < 100 ) down = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))-0.012927571311593056  ;
      if(WP == kMediumBTag && flavor == 0 && x > 100 && x < 140 ) down = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))-0.012316481210291386  ;
      if(WP == kMediumBTag && flavor == 0 && x > 140 && x < 200 ) down = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))-0.014507872052490711  ;
      if(WP == kMediumBTag && flavor == 0 && x > 200 && x < 300 ) down = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))-0.016649365425109863  ;
      if(WP == kMediumBTag && flavor == 0 && x > 300 && x < 600 ) down = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))-0.030278874561190605  ;
      if(WP == kMediumBTag && flavor == 0 && x > 600 && x < 1000) down = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))-0.053674362599849701  ;
      if(WP == kMediumBTag && flavor == 1 && x > 20  && x < 30  ) up   = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))+0.13967660069465637   ;
      if(WP == kMediumBTag && flavor == 1 && x > 30  && x < 50  ) up   = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))+0.049122259020805359  ;
      if(WP == kMediumBTag && flavor == 1 && x > 50  && x < 70  ) up   = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))+0.043598789721727371  ;
      if(WP == kMediumBTag && flavor == 1 && x > 70  && x < 100 ) up   = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))+0.038782715797424316  ;
      if(WP == kMediumBTag && flavor == 1 && x > 100 && x < 140 ) up   = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))+0.036949444562196732  ;
      if(WP == kMediumBTag && flavor == 1 && x > 140 && x < 200 ) up   = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))+0.043523617088794708  ;
      if(WP == kMediumBTag && flavor == 1 && x > 200 && x < 300 ) up   = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))+0.04994809627532959   ;
      if(WP == kMediumBTag && flavor == 1 && x > 300 && x < 600 ) up   = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))+0.090836621820926666  ;
      if(WP == kMediumBTag && flavor == 1 && x > 600 && x < 1000) up   = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))+0.16102308034896851   ;
      if(WP == kMediumBTag && flavor == 0 && x > 20  && x < 30  ) up   = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))+0.046558864414691925  ;
      if(WP == kMediumBTag && flavor == 0 && x > 30  && x < 50  ) up   = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))+0.016374086961150169  ;
      if(WP == kMediumBTag && flavor == 0 && x > 50  && x < 70  ) up   = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))+0.014532930217683315  ;
      if(WP == kMediumBTag && flavor == 0 && x > 70  && x < 100 ) up   = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))+0.012927571311593056  ;
      if(WP == kMediumBTag && flavor == 0 && x > 100 && x < 140 ) up   = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))+0.012316481210291386  ;
      if(WP == kMediumBTag && flavor == 0 && x > 140 && x < 200 ) up   = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))+0.014507872052490711  ;
      if(WP == kMediumBTag && flavor == 0 && x > 200 && x < 300 ) up   = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))+0.016649365425109863  ;
      if(WP == kMediumBTag && flavor == 0 && x > 300 && x < 600 ) up   = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))+0.030278874561190605  ;
      if(WP == kMediumBTag && flavor == 0 && x > 600 && x < 1000) up   = (0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x))))+0.053674362599849701  ;

      if(WP == kTightBTag  && flavor == 1 && x > 20  && x < 30  ) down = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))-0.15082257986068726    ;
      if(WP == kTightBTag  && flavor == 1 && x > 30  && x < 50  ) down = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))-0.054858319461345673   ;
      if(WP == kTightBTag  && flavor == 1 && x > 50  && x < 70  ) down = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))-0.053491078317165375   ;
      if(WP == kTightBTag  && flavor == 1 && x > 70  && x < 100 ) down = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))-0.048742711544036865   ;
      if(WP == kTightBTag  && flavor == 1 && x > 100 && x < 140 ) down = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))-0.04667256772518158    ;
      if(WP == kTightBTag  && flavor == 1 && x > 140 && x < 200 ) down = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))-0.047940321266651154   ;
      if(WP == kTightBTag  && flavor == 1 && x > 200 && x < 300 ) down = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))-0.064067237079143524   ;
      if(WP == kTightBTag  && flavor == 1 && x > 300 && x < 600 ) down = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))-0.10955213010311127    ;
      if(WP == kTightBTag  && flavor == 1 && x > 600 && x < 1000) down = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))-0.16617907583713531    ;
      if(WP == kTightBTag  && flavor == 0 && x > 20  && x < 30  ) down = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))-0.043092165142297745   ;
      if(WP == kTightBTag  && flavor == 0 && x > 30  && x < 50  ) down = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))-0.01567380502820015    ;
      if(WP == kTightBTag  && flavor == 0 && x > 50  && x < 70  ) down = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))-0.015283165499567986   ;
      if(WP == kTightBTag  && flavor == 0 && x > 70  && x < 100 ) down = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))-0.013926489278674126   ;
      if(WP == kTightBTag  && flavor == 0 && x > 100 && x < 140 ) down = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))-0.01333501935005188    ;
      if(WP == kTightBTag  && flavor == 0 && x > 140 && x < 200 ) down = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))-0.013697234913706779   ;
      if(WP == kTightBTag  && flavor == 0 && x > 200 && x < 300 ) down = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))-0.018304925411939621   ;
      if(WP == kTightBTag  && flavor == 0 && x > 300 && x < 600 ) down = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))-0.031300608068704605   ;
      if(WP == kTightBTag  && flavor == 0 && x > 600 && x < 1000) down = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))-0.047479737550020218   ;
      if(WP == kTightBTag  && flavor == 1 && x > 20  && x < 30  ) up   = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))+0.15082257986068726    ;
      if(WP == kTightBTag  && flavor == 1 && x > 30  && x < 50  ) up   = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))+0.054858319461345673   ;
      if(WP == kTightBTag  && flavor == 1 && x > 50  && x < 70  ) up   = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))+0.053491078317165375   ;
      if(WP == kTightBTag  && flavor == 1 && x > 70  && x < 100 ) up   = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))+0.048742711544036865   ;
      if(WP == kTightBTag  && flavor == 1 && x > 100 && x < 140 ) up   = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))+0.04667256772518158    ;
      if(WP == kTightBTag  && flavor == 1 && x > 140 && x < 200 ) up   = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))+0.047940321266651154   ;
      if(WP == kTightBTag  && flavor == 1 && x > 200 && x < 300 ) up   = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))+0.064067237079143524   ;
      if(WP == kTightBTag  && flavor == 1 && x > 300 && x < 600 ) up   = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))+0.10955213010311127    ;
      if(WP == kTightBTag  && flavor == 1 && x > 600 && x < 1000) up   = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))+0.16617907583713531    ;
      if(WP == kTightBTag  && flavor == 0 && x > 20  && x < 30  ) up   = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))+0.043092165142297745   ;
      if(WP == kTightBTag  && flavor == 0 && x > 30  && x < 50  ) up   = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))+0.01567380502820015    ;
      if(WP == kTightBTag  && flavor == 0 && x > 50  && x < 70  ) up   = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))+0.015283165499567986   ;
      if(WP == kTightBTag  && flavor == 0 && x > 70  && x < 100 ) up   = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))+0.013926489278674126   ;
      if(WP == kTightBTag  && flavor == 0 && x > 100 && x < 140 ) up   = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))+0.01333501935005188    ;
      if(WP == kTightBTag  && flavor == 0 && x > 140 && x < 200 ) up   = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))+0.013697234913706779   ;
      if(WP == kTightBTag  && flavor == 0 && x > 200 && x < 300 ) up   = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))+0.018304925411939621   ;
      if(WP == kTightBTag  && flavor == 0 && x > 300 && x < 600 ) up   = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))+0.031300608068704605   ;
      if(WP == kTightBTag  && flavor == 0 && x > 600 && x < 1000) up   = (0.573021*((1.+(0.472221*x))/(1.+(0.27584*x))))+0.047479737550020218   ;

      if(WP == kLooseBTag  && flavor == 0) down = (1.03726+0.000218815*x+-6.70561e-11*x*x+2.15167/x)*(1-(0.049251+8.04227e-05*x+-1.1138e-07*x*x));
      if(WP == kLooseBTag  && flavor == 0) up   = (1.03726+0.000218815*x+-6.70561e-11*x*x+2.15167/x)*(1+(0.049251+8.04227e-05*x+-1.1138e-07*x*x));
      if(WP == kMediumBTag && flavor == 0) down = (1.09149+3.31851e-05*x+2.34826e-07*x*x+-0.888846/x)*(1-(0.127379+0.000199537*x+-2.43111e-07*x*x));
      if(WP == kMediumBTag && flavor == 0) up   = (1.09149+3.31851e-05*x+2.34826e-07*x*x+-0.888846/x)*(1+(0.127379+0.000199537*x+-2.43111e-07*x*x));
      if(WP == kTightBTag  && flavor == 0) down = (1.09741+-0.000871879*x+2.20312e-06*x*x+-1.53037e-09*x*x*x)*(1-(0.230419+0.000263021*x+-2.85751e-07*x*x));
      if(WP == kTightBTag  && flavor == 0) up   = (1.09741+-0.000871879*x+2.20312e-06*x*x+-1.53037e-09*x*x*x)*(1+(0.230419+0.000263021*x+-2.85751e-07*x*x));
    }

  }


  float GetScaleFactor(int WP, int year, float jetpt, int jetFlavor/*, float& up, float& down, float& sys*/) {
    float scale_factor(1.), x(jetpt); //use x here, seems like it should be pT?
    if(x > 999.) x = 999.; //maximum pT
    else if(x < 20.) x = 20.;

    //In CSV file: type 0 is b, type 1 is c, and type 2 is light
    if(jetFlavor == 5 || jetFlavor == 4) { //true b-jets and c-jets (since corrections the same)
      if(year == 2016) {
        if(WP == kLooseBTag)       scale_factor = 0.971065*((1.+(0.0100459*x))/(1.+(0.00975219*x)));
        else if(WP == kMediumBTag) scale_factor = 0.922748*((1.+(0.0241884*x))/(1.+(0.0223119*x)));
        else if(WP == kTightBTag)  scale_factor = 0.573021*((1.+(0.472221*x))/(1.+(0.27584*x)));
      } else if(year == 2017) {
        if(WP == kLooseBTag)       scale_factor = 1.04891*((1.+(0.0145976*x))/(1.+(0.0165274*x)));
        else if(WP == kMediumBTag) scale_factor = 0.991757*((1.+(0.0209615*x))/(1.+(0.0234962*x)));
        else if(WP == kTightBTag)  scale_factor = 0.908648*((1.+(0.00516407*x))/(1.+(0.00564675*x)));
      } else if(year == 2018) {
        if(WP == kLooseBTag)       scale_factor = 0.873139+(0.00420739*(log(x+19.)*(log(x+18.)*(3-(0.380932*log(x+18.))))));
        else if(WP == kMediumBTag) scale_factor = 1.0097+(-(2.89663e-06*(log(x+19.)*(log(x+18.)*(3-(-(110.381*log(x+18.))))))));
        else if(WP == kTightBTag)  scale_factor = 0.818896+(0.00682971*(log(x+19.)*(log(x+18.)*(3-(0.440998*log(x+18.))))));
      }
    } else { //other jets
      if(year == 2016) {
        if(WP == kLooseBTag)       scale_factor = 1.03726+0.000218815*x+-6.70561e-11*x*x+2.15167/x;
        else if(WP == kMediumBTag) scale_factor = 1.09149+3.31851e-05*x+2.34826e-07*x*x+-0.888846/x;
        else if(WP == kTightBTag)  scale_factor = 1.09741+-0.000871879*x+2.20312e-06*x*x+-1.53037e-09*x*x*x;
      } else if(year == 2017) {
        if(WP == kLooseBTag)       scale_factor = 1.43763+-0.000337048*x+2.22072e-07*x*x+-4.85489/x;
        else if(WP == kMediumBTag) scale_factor = 1.40779+-0.00094558*x+8.74982e-07*x*x+-4.67814/x;
        else if(WP == kTightBTag)  scale_factor = 0.952956+0.000569069*x+-1.88872e-06*x*x+1.25729e-09*x*x*x;
      } else if(year == 2018) {
        if(WP == kLooseBTag)       scale_factor = 1.61341+-0.000566321*x+1.99464e-07*x*x+-5.09199/x;
        else if(WP == kMediumBTag) scale_factor = 1.59373+-0.00113028*x+8.66631e-07*x*x+-1.10505/x;
        else if(WP == kTightBTag)  scale_factor = 1.77088+-0.00371551*x+5.86489e-06*x*x+-3.01178e-09*x*x*x;
      }
    }
    if(scale_factor < 0.01) scale_factor = 1.; //taken from btagSFProducer.py
    return scale_factor;
  }

  float GetWeight(int wp, int year, int njets, float* jetspt,
                  float* jetseta, int* jetsflavor, int* jetsbtag/*,
                  float& up, float& down, float& sys*/) {
    float weight(1.), prob_data(1.), prob_mc(1.);
    // up = 1.; down = 1.; sys = 1.;
    for(int jet = 0; jet < njets; ++jet) {
      if(fabs(jetseta[jet]) > 2.4) continue; //not in accepted volume
      if(jetspt[jet] < 20.) continue; //below pT threshold
      float p_mc = GetMCEff(wp, year, jetspt[jet], jetseta[jet], jetsflavor[jet]);
      float scale_factor = GetScaleFactor(wp, year, jetspt[jet], jetsflavor[jet]);
      float p_data = std::max(0., std::min(1., (double) p_mc*scale_factor)); //0 <= probability <= 1
      if(jetsbtag[jet] > wp) { //passes this working point
        prob_data *= p_data;
        prob_mc   *= p_mc;
      } else { //fails this working point
        prob_data *= 1. - p_data;
        prob_mc   *= 1. - p_mc;
      }
    }
    if(prob_mc > 0.)
      weight = prob_data / prob_mc;
    else {
      std::cout << "BTagWeight::" << __func__ << ": weight undefined! Returning 1...\n";
    }
    return weight;
  }

  enum {kLooseBTag, kMediumBTag, kTightBTag};
// private:

  std::map<int, std::map<int, TH2D*>> histsL_; //light quarks
  std::map<int, std::map<int, TH2D*>> histsC_; //c quarks
  std::map<int, std::map<int, TH2D*>> histsB_; //b quarks
  std::vector<TFile*> files_;
  TRandom3* rnd_; //for generating systematic shifted parameters
  //       year          eta           pt
  std::map<int, std::map<int, std::map<int, bool>>> isShiftedUp_; //whether the systematic is shifted up or down
};
#endif
