#ifndef __BTAGWEIGHT__
#define __BTAGWEIGHT__
//Class to handle pileup weight of MC processes
//c++ includes
#include <map>
#include <iostream>
//ROOT includes
#include "TFile.h"
#include "TH2.h"

class BTagWeight {
public:
  BTagWeight() {
    TFile* f = 0;
    vector<int> working_points = {kLooseBTag, kMediumBTag, kTightBTag};
    vector<int> years = {2016, 2017, 2018};
    for(int wp : working_points) {
      for(int year : years) {
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
  
  float GetScaleFactor(int WP, int year, float jetpt, int jetFlavor) {
    float scale_factor(1.), x(jetpt); //use x here, seems like it should be pT?
    if(x > 999.) x = 999.; //maximum pT
    else if(x < 20.) x = 20.;
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
		  float* jetseta, int* jetsflavor, int* jetsbtag) {
    float weight(1.), prob_data(1.), prob_mc(1.);
    for(int jet = 0; jet < njets; ++jet) {
      if(fabs(jetseta[jet]) > 2.4) continue; //in accepted volume
      if(jetspt[jet] < 20.) continue; //above pT threshold
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
};
#endif
