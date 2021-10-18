#ifndef __SIGNIFICANCES__HH
#define __SIGNIFICANCES__HH
#include "Math/ProbFuncMathCore.h"
#include <cmath>
#include <iostream>

namespace CLFV {

  class Significances {
  public:
    Significances(double p = 0.05, double tolerance = 0.001,
                  bool useCLs = true, int maxAttempts = 10) : p_(p),
                                                              tolerance_(tolerance),
                                                              useCLs_(useCLs),
                                                              maxAttempts_(maxAttempts),
                                                              verbose_(0) {}

    //Probability of signal + background assuming only background
    double Probability(double signal, double background) {
      double val = ROOT::Math::poisson_cdf((int) (background), background + signal); //probability of <= background expectation from mu = signal + background
      if(useCLs_) val /= ROOT::Math::poisson_cdf((int) background, background); //CLs = P(sig + bkg) / P(bkg)
      return val;
    }

    //Probability of signal + background assuming only background for multiple categories
    double Probability(std::vector<double> signals, std::vector<double> backgrounds) {
      double val = 1.;
      unsigned categories = signals.size();
      if(verbose_ > 1) {
        std::cout << "Getting probability for signals = {";
        for(unsigned index = 0; index < categories; ++index) {
          std::cout << signals[index];
          if(index < categories - 1) std::cout << ", ";
        }
        std::cout << "}  backgrounds = {";
        for(unsigned index = 0; index < categories; ++index) {
          std::cout << backgrounds[index];
          if(index < categories - 1) std::cout << ", ";
        }
        std::cout << "}\n";
      }
      if(categories != backgrounds.size()) {
        std::cout << "Significances::" << __func__
                  << ": Error! Signal and background lists are different sizes!\n";
        return val;
      }
      //get probability for each category
      for(unsigned index = 0; index < categories; ++index) {
        val *= Probability(signals[index], backgrounds[index]);
        if(verbose_ > 2) std::cout << "Index " << index << " val = " << val << std::endl;
      }
      if(verbose_ > 1) std::cout << "Probability = " << val << std::endl;
      return val;
    }

    //find scale factor applied to signal to achieve p_ within tolerance_
    double LimitGain(double signal, double background, double &val) {
      static const double clsig = 1.96; // 95% CLs value
      double significance = signal/sqrt(background)/clsig; //not really significance but ratio of signal to ~Gaussian 95% CL background fluctuation
      double scale = 1./significance; //scale signal until achieve tolerance
      val = -1.; //running probability value
      unsigned attempts = 0;
      while(fabs(val - p_) > tolerance_ && attempts < maxAttempts_) { //guess scale factors until close to limit goal
        ++attempts;
        val = Probability(signal*scale, background);
        if(fabs(val-p_) > tolerance_) //only update if still not succeeding
          scale *= (val/p_ < 4.) ? (1.-p_)/(1.-val) : sqrt(background)*clsig/signal; //if far, start with the approximation scale
      }
      significance = 1./scale;
      if(significance > 10. || fabs(val - p_) > 5.*tolerance_) significance = -1.;
      return significance;
    }

    //find scale factor applied to signal to achieve p_ within tolerance_ for given categories
    double LimitGain(std::vector<double> signals, std::vector<double> backgrounds, double &val) {
      static const double clsig = 1.96; // 95% CL value
      unsigned categories = signals.size();
      if(categories != backgrounds.size()) {
        std::cout << "Significances::" << __func__
                  << ": Error! Signal and background lists are different sizes!\n";
        return -1.;
      }
      double significance = 0.;
      //loop through categories, set to be approximate limit for most sensitive category
      for(unsigned index = 0; index < categories; ++index)
        significance = std::max(significance, signals[index]/sqrt(backgrounds[index])/clsig);
      double scale = (significance > 0.) ? 1./significance : 1.; //scale signal until achieve tolerance
      double orig_scale = scale;
      val = -1.; //running probability value
      unsigned attempts = 0;
      while(fabs(val - p_) > tolerance_ && attempts < categories*maxAttempts_) { //guess scale factors until close to limit goal
        if(val > -0.5) //only update if not first attempt
          scale *= (val/p_ < 4.) ? (1.-p_)/(1.-val) : orig_scale; //if far, start with the approximation scale
        ++attempts;
        std::vector<double> tmp_signals = signals; //to apply scale factor to
        for(unsigned index = 0; index < categories; ++index) tmp_signals[index] = tmp_signals[index]*scale;
        val = Probability(tmp_signals, backgrounds);
        if(verbose_ > 1) std::cout << "Attempt " << attempts << " val = " << val << " with scale " << scale << std::endl;
      }
      significance = 1./scale;
      if(verbose_ > 0) std::cout << "Scale = " << scale << " --> limit gain = " << significance << std::endl;
      if(significance > 20. || fabs(val - p_) > 5.*tolerance_) significance = -1.;
      return significance;
    }
  public:
    double p_; //1 - confidence limit
    double tolerance_; //how far from p_ is acceptable
    bool useCLs_;
    unsigned maxAttempts_;
    int verbose_;
  };
}
#endif
