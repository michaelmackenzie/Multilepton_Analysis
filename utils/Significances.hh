#ifndef __SIGNIFICANCES__HH
#define __SIGNIFICANCES__HH
#include "Math/ProbFuncMathCore.h"
class Significances {
public:
  Significances(double p = 0.05, double tolerance = 0.001,
		bool useCLs = true, int maxAttempts = 10) : p_(p),
							 tolerance_(tolerance),
							 useCLs_(useCLs),
							 maxAttempts_(maxAttempts) {}

  //Probability of signal + background assuming only background
  double Probability(double signal, double background) {
    double val = ROOT::Math::poisson_cdf((int) (background), background + signal); //probability of <= background expectation from mu = signal + background
    if(useCLs_) val /= ROOT::Math::poisson_cdf((int) background, background); //CLs = P(sig + bkg) / P(bkg)
    return val;
  }

  //find scale factor applied to signal to achieve p_ within tolerance_
  double LimitGain(double signal, double background, double &val) {
    static const double clsig = 1.644853627; // 95% CL value
    double significance = signal/sqrt(background)/clsig; //not really significance but ratio of signal to ~Gaussian 95% CL background fluctuation
    double scale = 1./significance; //scale signal until achieve tolerance
    val = -1.; //running probability value
    int attempts = 0;
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
public:
  double p_; //1 - confidence limit
  double tolerance_; //how far from p_ is acceptable
  bool useCLs_;
  int maxAttempts_;
};
#endif
