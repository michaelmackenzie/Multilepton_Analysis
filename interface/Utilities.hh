#ifndef __UTILITIES__HH
#define __UTILITIES__HH

//c++ includes
#include <cmath>
#include <iostream>

//ROOT includes
#include "TH1.h"
#include "TH2.h"
#include "TAxis.h"
#include "TDirectory.h"
#include "TFolder.h"
#include "TTree.h"
#include "TBranch.h"
#include "TVectorD.h"
#include "TMatrixD.h"
#include "TDecompSVD.h"


namespace CLFV {

  class Utilities {
  public:
    Utilities() {}
    ~Utilities() {}

  private:
    typedef TDirectory fFolder;

    //------------------------------------------------------------------------------------------------------
    static void AddHistogram(TH1* h, fFolder* Folder) {
      if(!Folder) {
        std::cout << __func__ << ": Folder not defined!\n";
      } else if(!h) {
        std::cout << __func__ << ": Histogram not defined!\n";
      }
      // else {
      //   Folder->Add(h);
      // }
    }

  public:
    //------------------------------------------------------------------------------------------------------
    static void BookH1F(TH1*& h,  const char* Name, const char* Title,
                        Int_t Nx, Double_t XMin, Double_t XMax,
                        fFolder* Folder) {
      h = new TH1F(Name, Title, Nx, XMin, XMax);
      AddHistogram(h, Folder);
    }

    //------------------------------------------------------------------------------------------------------
    static void BookH1F(TH1*& h,  const char* Name, const char* Title,
                        Int_t Nx, const Double_t* Bins,
                        fFolder* Folder) {
      h = new TH1F(Name, Title, Nx, Bins);
      AddHistogram(h, Folder);
    }

    //------------------------------------------------------------------------------------------------------
    static void BookH1D(TH1*& h,  const char* Name, const char* Title,
                        Int_t Nx, Double_t XMin, Double_t XMax,
                        fFolder* Folder) {
      h = new TH1D(Name, Title, Nx, XMin, XMax);
      AddHistogram(h, Folder);
    }

    //------------------------------------------------------------------------------------------------------
    static void BookH1D(TH1*& h,  const char* Name, const char* Title,
                        Int_t Nx, const Double_t* Bins,
                        fFolder* Folder) {
      h = new TH1D(Name, Title, Nx, Bins);
      AddHistogram(h, Folder);
    }

    //------------------------------------------------------------------------------------------------------
    static void BookH2F(TH2*& h,  const char* Name, const char* Title,
                        Int_t Nx, Double_t XMin, Double_t XMax,
                        Int_t Ny, Double_t YMin, Double_t YMax,
                        fFolder* Folder) {
      h = new TH2F(Name, Title, Nx, XMin, XMax, Ny, YMin, YMax);
      AddHistogram(h, Folder);
    }

    //------------------------------------------------------------------------------------------------------
    static void BookH2F(TH2*& h,  const char* Name, const char* Title,
                        Int_t Nx, const Double_t* XBins,
                        Int_t Ny, const Double_t* YBins,
                        fFolder* Folder) {
      h = new TH2F(Name, Title, Nx, XBins, Ny, YBins);
      AddHistogram(h, Folder);
    }

    //------------------------------------------------------------------------------------------------------
    static void BookH2F(TH2*& h,  const char* Name, const char* Title,
                        Int_t Nx, Double_t XMin, Double_t XMax,
                        Int_t Ny, const Double_t* YBins,
                        fFolder* Folder) {
      h = new TH2F(Name, Title, Nx, XMin, XMax, Ny, YBins);
      AddHistogram(h, Folder);
    }

    //------------------------------------------------------------------------------------------------------
    static void BookH2F(TH2*& h,  const char* Name, const char* Title,
                        Int_t Nx, const Double_t* XBins,
                        Int_t Ny, Double_t YMin, Double_t YMax,
                        fFolder* Folder) {
      h = new TH2F(Name, Title, Nx, XBins, Ny, YMin, YMax);
      AddHistogram(h, Folder);
    }

    //------------------------------------------------------------------------------------------------------
    static void BookH2D(TH2*& h,  const char* Name, const char* Title,
                        Int_t Nx, Double_t XMin, Double_t XMax,
                        Int_t Ny, Double_t YMin, Double_t YMax,
                        fFolder* Folder) {
      h = new TH2D(Name, Title, Nx, XMin, XMax, Ny, YMin, YMax);
      AddHistogram(h, Folder);
    }

    //------------------------------------------------------------------------------------------------------
    static void BookH2D(TH2*& h,  const char* Name, const char* Title,
                        Int_t Nx, const Double_t* XBins,
                        Int_t Ny, const Double_t* YBins,
                        fFolder* Folder) {
      h = new TH2D(Name, Title, Nx, XBins, Ny, YBins);
      AddHistogram(h, Folder);
    }

    //------------------------------------------------------------------------------------------------------
    static void BookH2D(TH2*& h,  const char* Name, const char* Title,
                        Int_t Nx, Double_t XMin, Double_t XMax,
                        Int_t Ny, const Double_t* YBins,
                        fFolder* Folder) {
      h = new TH2D(Name, Title, Nx, XMin, XMax, Ny, YBins);
      AddHistogram(h, Folder);
    }

    //------------------------------------------------------------------------------------------------------
    static void BookH2D(TH2*& h,  const char* Name, const char* Title,
                        Int_t Nx, const Double_t* XBins,
                        Int_t Ny, Double_t YMin, Double_t YMax,
                        fFolder* Folder) {
      h = new TH2D(Name, Title, Nx, XBins, Ny, YMin, YMax);
      AddHistogram(h, Folder);
    }

    //------------------------------------------------------------------------------------------------------
    // minimum in histogram for a given range
    static double H1Min(TH1* h, const double xmin = 1., const double xmax = -1.) {
      if(!h) {
        printf("Utilities::%s: Undefined histogram!\n", __func__);
        return -1.;
      }
      const int bin_lo = (xmin < xmax) ? std::max(1, h->FindBin(xmin)) : 1;
      const int bin_hi = (xmin < xmax) ? std::min(h->GetNbinsX(), h->FindBin(xmax)) : h->GetNbinsX();
      double min_val = h->GetBinContent(bin_lo);
      for(int ibin = bin_lo+1; ibin <= bin_hi; ++ibin) min_val = std::min(min_val, h->GetBinContent(ibin));
      return min_val;
    }

    //------------------------------------------------------------------------------------------------------
    // maximum in histogram for a given range
    static double H1Max(TH1* h, const double xmin = 1., const double xmax = -1.) {
      if(!h) {
        printf("Utilities::%s: Undefined histogram!\n", __func__);
        return -1.;
      }
      const int bin_lo = (xmin < xmax) ? std::max(1, h->FindBin(xmin)) : 1;
      const int bin_hi = (xmin < xmax) ? std::min(h->GetNbinsX(), h->FindBin(xmax)) : h->GetNbinsX();
      double max_val = h->GetBinContent(bin_lo);
      for(int ibin = bin_lo+1; ibin <= bin_hi; ++ibin) max_val = std::max(max_val, h->GetBinContent(ibin));
      return max_val;
    }

    //------------------------------------------------------------------------------------------------------
    static int SetBranchAddress(TTree*& tree,  const char* branch, void* val, TBranch** br = nullptr) {
      if(!tree->GetBranch(branch)) {
        std::cout << "Utilities::" << __func__ << ": Unknown branch " << branch << std::endl;
        return -1;
      }
      tree->SetBranchStatus(branch, 1);
      auto f = tree->GetCurrentFile();
      if(tree->GetReadCache(f)) {
        tree->AddBranchToCache(branch, 1);
      }
      if(br != nullptr) {
        return tree->SetBranchAddress(branch, val, br);
      }
      return tree->SetBranchAddress(branch, val);
    }

    //------------------------------------------------------------------------------------------------------
    // Get phi from (x,y) point
    static double PhiFromXY(const double x, const double y) {
      if(!std::isfinite(x) || !std::isfinite(y)) {
        std::cout << "Utilities::" << __func__ << ": Input (x,y) coordinates are not finite!\n";
        return 0.;
      }
      const double r = std::sqrt(x*x+y*y);
      if(r <= 0.) return 0.;
      const double phi = std::acos(std::max(-1., std::min(1., x/r)))*(y < 0. ? -1. : 1.);
      return phi;
    }

    //------------------------------------------------------------------------------------------------------
    // Delta Phi in [-pi, pi)
    static double DeltaPhi(const double phi_1, const double phi_2) {
      if(!std::isfinite(phi_1) || !std::isfinite(phi_2)) {
        std::cout << "Utilities::" << __func__ << ": Input phi values are not finite!\n";
        return 0.;
      }
      double val = phi_1 - phi_2;
      if(val >= M_PI) val -= 2.*M_PI;
      if(val < -M_PI) val += 2.*M_PI;
      return val;
    }

    //------------------------------------------------------------------------------------------------------
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

    //------------------------------------------------------------------------------------------------------
    // Principal component shifted parameters, <up,down>
    static std::pair<TMatrixD, TMatrixD> PCAShifts(TMatrixDSym& cov, TVectorD& params) {

      const int nparams = params.GetNoElements();
      std::pair<TMatrixD, TMatrixD> shifts = {TMatrixD(nparams, nparams), TMatrixD(nparams, nparams)};
      if(nparams != cov.GetNrows()) {
        std::cout << "Utilities::" << __func__ << ": N(params) doesn't match covariance matrix!\n";
        return shifts;
      }

      //Get the up/down shifted parameter matrices
      TMatrixD& up   = shifts.first;
      TMatrixD& down = shifts.second;

      //Perform the SVD
      TDecompSVD svd(cov);
      if(!svd.Decompose()) {
        std::cout << "Utilities::" << __func__ << ": SVD failed!\n";
        return shifts;
      }

      //Retrieve the eigen vector/values
      TVectorD sig  = svd.GetSig();
      TMatrixD eig  = svd.GetV();
      TMatrixD eigT = eig; eigT.Transpose(eigT);

      //Create a vector of rotated parameters
      TVectorD params_rot = eigT*params;

      //Create a vector for each shifted rotated parameter
      for(int irow = 0; irow < nparams; ++irow) {
        const double unc = sqrt((nparams-1)*sig[irow]); //uncertainty on s' parameter
        //create a vector of just the one parameter shifted
        TVectorD shift(nparams);
        for(int jrow = 0; jrow < nparams; ++jrow) shift[jrow] = (irow == jrow) ? unc : 0.;
        //shift the rotated parameter and find the shifted nominal parameters
        TVectorD params_up   = eig*(params_rot + shift);
        TVectorD params_down = eig*(params_rot - shift);
        up  [irow] = params_up;
        down[irow] = params_down;
      }
      return shifts;
    }

    //------------------------------------------------------------------------------------------------------
    enum {kXAxis, kYAxis};
  };
}
#endif
