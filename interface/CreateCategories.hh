#ifndef __CREATECATEGORIES__HH
#define __CREATECATEGORIES__HH
//A tool to create a given number of categories

//c++ includes
#include <map>
#include <vector>
#include <iostream>
//ROOT includes
#include "TString.h"
#include "TH1.h"
class CreateCategories {
public:
  CreateCategories(double minBkg = 10., int verbose = 0) : minBkg_(minBkg), verbose_(verbose) {}

  void FindCategories(TH1* hsig, TH1* hbkg, unsigned ncat, std::vector<Double_t>& cuts, std::vector<Double_t>& sigs) {
    if(!hsig || !hbkg) return;
    int nbins = hsig->GetNbinsX();
    IndividualCategory(hsig, hbkg, nbins, ncat, cuts, sigs);
    if(verbose_ > -1) {
      printf("CreateCategories: Final categories as category: (cut, significance)\n");
      double val = 0.;
      for(unsigned index = 0; index < cuts.size(); ++index) {
        printf(" cat %3i: (%6.3f, %8.2f)\n", index, cuts[index], sigs[index]);
        val += sigs[index];
      }
      printf("Total significance = %8.2f (%8.2f)\n", val, sqrt(val));
    }
  }
private:
  double IndividualCategory(TH1* hsig, TH1* hbkg, Int_t bin_max, unsigned ncat, std::vector<Double_t>& cuts,
                          std::vector<Double_t>& sigs) {
    double val_max = -1.;
    for(int ibin = bin_max; ibin > 0; --ibin) {
      std::vector<Double_t> cuts_cur, sigs_cur;
      double xmax = hsig->GetBinLowEdge(ibin);
      double sig  = hsig->Integral(ibin, bin_max);
      double bkg  = hbkg->Integral(ibin, bin_max);
      if(bkg <= minBkg_) continue; //need background to be defined
      double val = sig/sqrt(bkg);
      if(ncat > 1)
        val = val*val + IndividualCategory(hsig, hbkg, ibin - 1, ncat - 1, cuts_cur, sigs_cur);
      else val *= val;
      if(verbose_ > 2) std::cout << "CreateCategories: Category level " << ncat
                                 << " for " << xmax << " < x < " << hsig->GetBinLowEdge(bin_max) << " has value " << val << std::endl;
      if(val_max < val) {
        val_max = val;
        cuts = cuts_cur;
        sigs = sigs_cur;
        cuts.push_back(hsig->GetBinLowEdge(ibin));
        sigs.push_back(sig*sig/bkg);
      }
    }
    if(verbose_ > 1) std::cout << "CreateCategories: Category level " << ncat << " has total significance " << val_max << std::endl;
    return val_max;
  }
  double minBkg_;
  int verbose_;
};
#endif
