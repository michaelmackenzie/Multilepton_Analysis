#ifndef __UTILITIES__HH
#define __UTILITIES__HH

//c++ includes
#include <cmath>
#include <iostream>

//ROOT includes
#include "TH1.h"
#include "TH2.h"
#include "TGraph.h"
#include "TAxis.h"
#include "TDirectory.h"
#include "TFolder.h"
#include "TTree.h"
#include "TBranch.h"
#include "TEventList.h"
#include "TVectorD.h"
#include "TMatrixD.h"
#include "TDecompSVD.h"
#include "TRandom3.h"


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
    // Get bin content for a given x-value
    static double GetBinContent(TH1* h, const double xval, const bool allow_out_range = true) {
      if(!h) {
        printf("Utilities::%s: Undefined histogram!\n", __func__);
        return -1.;
      }
      if(allow_out_range) return h->GetBinContent(h->GetXaxis()->FindBin(xval));
      return h->GetBinContent(std::max(1, std::min(h->GetNbinsX(), h->GetXaxis()->FindBin(xval))));
    }

    //------------------------------------------------------------------------------------------------------
    // Get bin content for a given (x-value, y-value
    static double GetBinContent(TH2* h, const double xval, const double yval, const bool allow_out_range = true) {
      if(!h) {
        printf("Utilities::%s: Undefined histogram!\n", __func__);
        return -1.;
      }
      if(allow_out_range) return h->GetBinContent(h->GetXaxis()->FindBin(xval), h->GetYaxis()->FindBin(yval));
      return h->GetBinContent(std::max(1, std::min(h->GetNbinsX(), h->GetXaxis()->FindBin(xval))),
                              std::max(1, std::min(h->GetNbinsY(), h->GetYaxis()->FindBin(yval)))
                              );
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
    // histogram integral in a given range, including the under/overflow if requested
    static double H1Integral(TH1* h, const double xmin = 1., const double xmax = -1., const bool under_over = false, const char* option = "") {
      if(!h) {
        printf("Utilities::%s: Undefined histogram!\n", __func__);
        return -1.;
      }
      const int bin_lo = (xmin < xmax) ? std::max((under_over) ? 0 : 1, h->FindBin(xmin)) : (under_over) ? 0 : 1;
      const int bin_hi = (xmin < xmax) ? std::min(h->GetNbinsX() + ((under_over) ? 1 : 0), h->FindBin(xmax)) : h->GetNbinsX() + ((under_over) ? 1 : 0);
      return h->Integral(bin_lo, bin_hi, option);
    }

    //------------------------------------------------------------------------------------------------------
    // minimum in a histogram above a certain value
    static double H1MinAbove(TH1* h, const double ymin) {
      if(!h) {
        printf("Utilities::%s: Undefined histogram!\n", __func__);
        return -1.;
      }
      double min_val = h->GetMaximum();
      for(int ibin = 1; ibin <= h->GetNbinsX(); ++ibin) {
        const double val = h->GetBinContent(ibin);
        if(val >= ymin)
          min_val = std::min(min_val, val);
      }
      return std::max(ymin, min_val);
    }

    //------------------------------------------------------------------------------------------------------
    // minimum in a histogram above a certain value
    static double H2MinAbove(TH2* h, const double zmin) {
      if(!h) {
        printf("Utilities::%s: Undefined histogram!\n", __func__);
        return -1.;
      }
      double min_val = h->GetMaximum();
      for(int xbin = 1; xbin <= h->GetNbinsX(); ++xbin) {
        for(int ybin = 1; ybin <= h->GetNbinsY(); ++ybin) {
          const double val = h->GetBinContent(xbin, ybin);
          if(val >= zmin) min_val = std::min(min_val, val);
        }
      }
      return std::max(zmin, min_val);
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
    // Error function inverse
    static double Erf_Inv(const double x) {
      if(x <= -1. || x >= 1.) {
        throw std::runtime_error(Form("Utilities::%s: Argument out of bounds (%f)\n", __func__, x));
      }
      double val = 0.;
      const int sign = (x < 0.) ? -1 : 1;
      const double ax = std::fabs(x);
      const double cx = 1. - ax; //complement to |x|

      //Use different approximations in different regions of |x|, taken following boost::math::erf_inv
      if(ax <= 0.5) { //for |x| < 0.5, approximate using val = ax*(ax+10)*(b + f(ax)/g(ax))
        const float b = 0.0891314744949340820313f;
        const double N[] = {
          -0.000508781949658280665617,
          -0.00836874819741736770379,
          0.0334806625409744615033,
          -0.0126926147662974029034,
          -0.0365637971411762664006,
          0.0219878681111168899165,
          0.00822687874676915743155,
          -0.00538772965071242932965
        };
        const double D[] = {
          1.0,
          -0.970005043303290640362,
          -1.56574558234175846809,
          1.56221558398423026363,
          0.662328840472002992063,
          -0.71228902341542847553,
          -0.0527396382340099713954,
          0.0795283687341571680018,
          -0.00233393759374190016776,
          0.000886216390456424707504
        };
        double val_n(0.), val_d(0.);
        for(int i = 0; i <  8; ++i) val_n += std::pow(ax, i)*N[i];
        for(int i = 0; i < 10; ++i) val_d += std::pow(ax, i)*D[i];
        val = ax*(10.+ax)*(b + val_n / val_d);
        // printf("%s: Using first approx: x = %f, inv(x) = %f\n", __func__, x, val);
      } else if(cx >= 0.25) { //for 0.5 < |x| <= 0.75, use sqrt(-2*log(cx)) / (b + f(cx - 0.25)/g(cx - 0.25))
        const float b = 2.249481201171875f;
        const double N[] = {
          -0.202433508355938759655,
          0.105264680699391713268,
          8.37050328343119927838,
          17.6447298408374015486,
          -18.8510648058714251895,
          -44.6382324441786960818,
          17.445385985570866523,
          21.1294655448340526258,
          -3.67192254707729348546
        };
        const double D[] = {
          1.0,
          6.24264124854247537712,
          3.9713437953343869095,
          -28.6608180499800029974,
          -20.1432634680485188801,
          48.5609213108739935468,
          10.8268667355460159008,
          -22.6436933413139721736,
          1.72114765761200282724
        };
        double val_n(0.), val_d(0.);
        for(int i = 0; i <  9; ++i) val_n += std::pow(cx-0.25, i)*N[i];
        for(int i = 0; i <  9; ++i) val_d += std::pow(cx-0.25, i)*D[i];
        val = std::sqrt(-2.*std::log(cx))/(b + val_n / val_d);
        // printf("%s: Using second approx: x = %f, inv(x) = %f\n", __func__, x, val);
      } else { //|x| > 0.75, use approximations as sqrt(-log(1-|x|)) gets large
        const double log_val = std::sqrt(-std::log(cx));
        if(log_val < 3.) {
          const float b = 0.807220458984375f;
          const double N[] = {
            -0.131102781679951906451,
            -0.163794047193317060787,
            0.117030156341995252019,
            0.387079738972604337464,
            0.337785538912035898924,
            0.142869534408157156766,
            0.0290157910005329060432,
            0.00214558995388805277169,
            -0.679465575181126350155e-6,
            0.285225331782217055858e-7,
            -0.681149956853776992068e-9
          };
          const double D[] = {
            1.0,
            3.46625407242567245975,
            5.38168345707006855425,
            4.77846592945843778382,
            2.59301921623620271374,
            0.848854343457902036425,
            0.152264338295331783612,
            0.01105924229346489121
          };
          double val_n(0.), val_d(0.);
          for(int i = 0; i < 11; ++i) val_n += std::pow(log_val - 1.125f, i)*N[i];
          for(int i = 0; i <  8; ++i) val_d += std::pow(log_val - 1.125f, i)*D[i];
          val = log_val*(b + val_n/val_d);
          // printf("%s: Using third approx: x = %f, sqrt(-log(1-|x|)) = %f, inv(x) = %f\n", __func__, x, log_val, val);
        } else { //only for log_val < 6, but this should be fine up to 1 - |x| > 1e-15

          const float b = 0.93995571136474609375f;
          const double N[] = {
             -0.0350353787183177984712,
             -0.00222426529213447927281,
             0.0185573306514231072324,
             0.00950804701325919603619,
             0.00187123492819559223345,
             0.000157544617424960554631,
             0.460469890584317994083e-5,
             -0.230404776911882601748e-9,
             0.266339227425782031962e-11
          };
          const double D[] = {
            1.0,
            1.3653349817554063097,
            0.762059164553623404043,
            0.220091105764131249824,
            0.0341589143670947727934,
            0.00263861676657015992959,
            0.764675292302794483503e-4
          };
          double val_n(0.), val_d(0.);
          for(int i = 0; i <  9; ++i) val_n += std::pow(log_val - 3.f, i)*N[i];
          for(int i = 0; i <  7; ++i) val_d += std::pow(log_val - 3.f, i)*D[i];
          val = log_val*(b + val_n/val_d);
          // printf("%s: Using fourth approx: x = %f, sqrt(-log(1-|x|)) = %f, inv(x) = %f\n", __func__, x, log_val, val);
        }
      }
      if(!std::isfinite(val)) {
        throw std::runtime_error(Form("Utilities::%s: Undefined error function inverse (%f) for |x| = %f, 1 - |x| = %f\n", __func__, val, ax, cx));
      }
      return sign*val;
    }

    //------------------------------------------------------------------------------------------------------
    // Double-sided Crystal Ball function, as implemented in RooDoubleCrystalBall with a symmetric code
    static double DSCB(const double x, const double mu, const double sigma, const double alpha_1, const double n_1, const double alpha_2, const double n_2) {
      if(sigma <= 0.) return 0.;
      const double u = (x-mu)/sigma;
      const double A1  = std::pow(n_1/std::fabs(alpha_1), n_1)*std::exp(-alpha_1*alpha_1/2.);
      const double A2  = std::pow(n_2/std::fabs(alpha_2), n_2)*std::exp(-alpha_2*alpha_2/2.);
      const double B1  = n_1/std::fabs(alpha_1) - std::fabs(alpha_1);
      const double B2  = n_2/std::fabs(alpha_2) - std::fabs(alpha_2);

      double val(1.);
      if     (u<-alpha_1) val = A1*std::pow(B1-u,-n_1); //left power-law tail
      else if(u< alpha_2) val = std::exp(-u*u/2.);      //central Gaussian
      else                val = A2*std::pow(B2+u,-n_2); //right power-law tail
      return val;
    }

    //------------------------------------------------------------------------------------------------------
    // Convert a histogram to an efficiency distribution
    static void HistToEff(TH1* h, const bool cut_low = true, const bool rejection = false) {
      const int nbins = h->GetNbinsX();
      const double integral = h->Integral(0, nbins+1);
      if(integral <= 0.) return; //cannot convert to efficiency
      TH1* htmp = (TH1*) h->Clone("hTMP_HistToEff"); //temporary histogram with the values preserved
      for(int ibin = 0; ibin <= nbins+1; ++ibin) {
        const double cut_integral = (cut_low) ? htmp->Integral(ibin, nbins+1) : htmp->Integral(0, ibin);
        if(rejection) { //store the rejection factor instead of efficiency
          h->SetBinContent(ibin, 1. - cut_integral / integral);
        } else { //store the cut efficiency
          h->SetBinContent(ibin, cut_integral / integral);
        }
      }
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
    // Linear interpolation between points in a TGraph
    static double Interpolate(const TGraph* g, const double x) {
      //if no graph, return 0
      if(!g) return 0.;

      //if no points, return 0
      if(g->GetN() <= 0) return 0.;

      if(!std::isfinite(x)) return 0.;
      //check if beyond the range of interpolation
      const int n = g->GetN();
      if(n == 1)              return g->GetY()[  0]; //only point --> no interpolation possible
      if(x <= g->GetX()[  0]) return g->GetY()[  0];
      if(x >= g->GetX()[n-1]) return g->GetY()[n-1];

      //find the two points x is between
      int bin_low(0), bin_high(1); //initialize to between points 0 and 1
      for(int ipoint = 1; ipoint < n - 1; ++ipoint) {
        if(x < g->GetX()[ipoint]) break; //if below this point, it's between this point and the previous
        bin_low  = ipoint; //x is still above this value --> continue looking
        bin_high = ipoint+1;
      }

      //Use point-slope interpolation form
      const double x1 = g->GetX()[bin_low ];
      const double x2 = g->GetX()[bin_high];
      const double y1 = g->GetY()[bin_low ];
      const double y2 = g->GetY()[bin_high];

      //y = y0 + (delta y / delta x)*(x - x0)
      const double val = y1 + (y2 - y1)/(x2 - x1)*(x - x1);

      return val;
    }

    //------------------------------------------------------------------------------------------------------
    // Randomly sample a tree to create a bootstrapped tree (including input weights to have unweighted output)
    static TTree* BootStrap(TTree* tree, const Long64_t nsample, TRandom3& rnd, const char* weight_name = nullptr, const TString cut = "", const int verbose = 0) {
      if(!tree) return nullptr;
      if(weight_name != nullptr && !tree->GetBranch(weight_name)) {
        printf("Utilities::%s: No weight branch named %s\n", __func__, weight_name);
        return nullptr;
      }
      const Long64_t nentries = tree->GetEntries();
      if(verbose > 0) printf("Utilities::%s:\n N(entries) = %lld\n", __func__, nentries);
      tree->SetEventList(nullptr);
      TEventList* elist = nullptr;
      if(cut != "") {
        tree->Draw(">>elist", cut.Data());
        elist = (TEventList*) gDirectory->Get("elist");
        if(!elist) {
          printf("Utilities::%s: No event list created using selection %s\n", __func__, cut.Data());
          return nullptr;
        }
        if(verbose > 0) printf(" Processed the event list\n");
        if(elist->GetN() == 0) {
          printf("Utilities::%s: No events pass the pre-selection cut %s\n", __func__, cut.Data());
          return nullptr;
        }
        // tree->SetEventList(elist);
      }
      const Long64_t nlist = (elist) ? elist->GetN() : nentries; //N(entries) in the event list

      Float_t weight(1.f), max_weight(0.f); //default to 0 maximum weight so if no weights, all events pass
      if(weight_name) {
        tree->SetBranchAddress(weight_name, &weight);

        //determine the maximum weight
        tree->Draw(Form("%s>>TMP_UTILITIES_%s", weight_name, __func__), cut, "0");
        auto hist = (TH1*) gDirectory->Get(Form("TMP_UTILITIES_%s", __func__));
        if(!hist) {
          printf("Utilities::%s: No unable to retrieve the event weight histogram (branch = %s)\n", __func__, weight_name);
          return nullptr;
        }
        //find the highest weight bin
        const int bin = hist->FindLastBinAbove(0.);
        max_weight = hist->GetBinLowEdge(bin) + hist->GetBinWidth(bin);
        delete hist; //clean up the memory
        if(verbose > 0) printf(" Retrieved the maximum weight of %.4f\n", max_weight);
        if(max_weight <= 0.) {
          printf("Utilities::%s: Maximum weight of %.4f is not useful! Ignoring wieghts\n", __func__, max_weight);
          max_weight = -1.f; //ensure it's ignored
        }
      }
      TTree* out_tree = tree->CloneTree(0);

      Long64_t nadded(0), nattempt(0);
      while(nadded < nsample) {
        if(verbose > 0 && (nattempt % 100000) == 0) printf(" Attempt %10lld\n", nattempt);
        ++nattempt;
        const Long64_t entry = (elist) ? elist->GetEntry(rnd.Integer(nlist)) : rnd.Integer(nentries);
        if(elist && !elist->Contains(entry)) continue;
        tree->GetEntry(entry);
        //use accept/reject to sample the events using event weights
        const double r = rnd.Uniform();
        if(max_weight > 0. && weight <= r*max_weight) continue;
        out_tree->Fill(); //accept the event
        ++nadded;
        if(verbose > 0 && (nadded*10) % nsample == 0) printf(" Added %7lld events (%.1f%%)\n", nadded, nadded*100./nsample);
      }
      return out_tree;
    }

    //------------------------------------------------------------------------------------------------------
    // Principal component shifted parameters, <up,down>
    static std::pair<TMatrixD, TMatrixD> PCAShifts(TMatrixDSym& cov, TVectorD& params, double nsigma = 1.) {
      if(nsigma < 0.) {
        printf("Utilities::%s: Given n(sigma) = %.3f, defaulting to 1 sigma\n", __func__, nsigma);
        nsigma = 1.;
      }

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
        for(int jrow = 0; jrow < nparams; ++jrow) shift[jrow] = (irow == jrow) ? unc*nsigma : 0.; //shift it by N(sigma) * 1 sigma uncertainty
        //shift the rotated parameter and find the shifted nominal parameters
        TVectorD params_up   = eig*(params_rot + shift);
        TVectorD params_down = eig*(params_rot - shift);
        up  [irow] = params_up;
        down[irow] = params_down;
      }
      return shifts;
    }

    //------------------------------------------------------------------------------------------------------
    // Get total up/down weights from a list of alternate up/down weights
    static std::pair<float, float> CombinedUncertainty(const float weight, std::vector<float> ups, std::vector<float> downs) {
      //consistency checks, default to no change
      if(!std::isfinite(weight))     return std::pair<float,float>(weight,weight);

      //add the deviations in quadrature (separately, allowing for more ups or downs)
      float up_err(0.f);
      for(unsigned index = 0; index < ups.size(); ++index) {
        up_err   += std::pow(weight - ups  [index], 2);
      }
      float down_err(0.f);
      for(unsigned index = 0; index < downs.size(); ++index) {
        down_err += std::pow(weight - downs[index], 2);
      }
      up_err   = (up_err   > 0.f) ? std::sqrt(up_err  ) : 0.f;
      down_err = (down_err > 0.f) ? std::sqrt(down_err) : 0.f;
      const float up   = weight + up_err  *((weight < 0.) ? -1. : 1.);
      const float down = weight - down_err*((weight < 0.) ? -1. : 1.);
      return std::pair<float,float>(up,down);
    }


    //------------------------------------------------------------------------------------------------------
    enum {kXAxis, kYAxis};
  };
}
#endif
