#ifndef __AWKWARDTH2__
#define __AWKWARDTH2__
//Class to handle 2D histograms with binning in y non constant in x

//c++ includes
#include <map>
#include <vector>
#include <algorithm> //for std::sort
#include <iostream>

//ROOT includes
#include "TH2.h"
#include "TSystem.h"

namespace CLFV {
  class AwkwardTH2 {
  public:
    AwkwardTH2(const char* name, const char* title, std::map<double, std::vector<double>> binning, const char type = 'D');
    ~AwkwardTH2() {if(h_) delete h_; h_ = nullptr;}

    int    FindBin(double x, double y);
    int    Fill(double x, double y, double wt = 1.);
    double Integral();
    double Integral(int low_bin, int high_bin);
    double Integral(double xmin, double xmax, double ymin, double ymax);

    int    NBins     () { return h_->GetNbinsX() - 2*(bin_edges_.size() - 1); } //no under/overflow count
    int    NTotalBins() { return h_->GetNbinsX() + 2; } //with under/overflow count

    TH1* h_; //internal histogram
    std::map<double, std::vector<double>> binning_;
    std::vector<double> bin_edges_;
  };
}
#endif
