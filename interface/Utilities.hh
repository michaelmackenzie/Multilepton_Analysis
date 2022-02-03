#ifndef __UTILITIES__HH
#define __UTILITIES__HH

//c++ includes
#include <cmath>
#include <iostream>

//ROOT includes
#include "TH1.h"
#include "TH2.h"
#include "TAxis.h"

namespace CLFV {

  class Utilities {
  public:
    Utilities() {}
    ~Utilities() {}

    static double Interpolate(const TH2* h, const double x, const double y, int along, int verbose = 0) {
      const int bin_x = std::max(1, std::min(h->GetXaxis()->GetNbins(), h->GetXaxis()->FindBin(x)));
      const int bin_y = std::max(1, std::min(h->GetYaxis()->GetNbins(), h->GetYaxis()->FindBin(y)));
      const double bin_con = h->GetBinContent(bin_x, bin_y);
      if(along != kXAxis && along != kYAxis) {
        if(verbose > 0) printf("Utilities::%s: Bin x = %i, Bin y = %i, Bin content = %.5e\n", __func__, bin_x, bin_y, bin_con);
        printf("Utilities::%s: Error! Undefined interpolation axis! Returning histogram bin content\n", __func__);
        return bin_con;
      }

      //Determine the axis and variable to interpolate with
      const TAxis* axis = (along == kXAxis) ? h->GetXaxis() : h->GetYaxis();
      const double arg  = (along == kXAxis) ? x             : y;
      const int    bin  = (along == kXAxis) ? bin_x         : bin_y;

      //Only interpolate in the given axis
      const double bin_cen = axis->GetBinCenter(bin);

      //Determine which bin to interpolate between
      const int bin_2 = (bin_cen < arg) ? std::min(axis->GetNbins(), bin + 1) : std::max(1, bin - 1);
      const double bin_cen_2 = axis->GetBinCenter(bin_2);
      const double bin_con_2 = (along == kXAxis) ? h->GetBinContent(bin_2, bin_y) : h->GetBinContent(bin_x, bin_2) ;

      //if the same bin, don't interpolate since at an edge
      if(bin == bin_2) {
        if(verbose > 0) printf("Utilities::%s: Bin x = %i, Bin y = %i, Bin content = %.5e, Bin center = %.5e, Bin 2 = %i = Bin 1\n",
                               __func__, bin_x, bin_y, bin_con, bin_cen, bin_2);
        return bin_con;
      }

      //Perform a linear interpolation
      //                   Y0    +  (X - X0)       *              ((Y0 - Y1) / (X0 - X1))
      const double val = bin_con + (arg - bin_cen) * ((bin_con - bin_con_2)  / (bin_cen - bin_cen_2));

      if(verbose > 0) printf("Utilities::%s: Bin x = %i, Bin y = %i, Bin content = %.5e, Bin center = %.5e, Bin 2 = %i, Bin 2 content = %.5e, Bin 2 center = %.5e, val = %.5e\n",
                             __func__, bin_x, bin_y, bin_con, bin_cen, bin_2, bin_con_2, bin_cen_2, val);

      return val;
    }

    enum {kXAxis, kYAxis};
  };
}
#endif
