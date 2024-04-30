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
    static double H1Integral(TH1* h, const double xmin = 1., const double xmax = -1., const bool under_over = false) {
      if(!h) {
        printf("Utilities::%s: Undefined histogram!\n", __func__);
        return -1.;
      }
      const int bin_lo = (xmin < xmax) ? std::max((under_over) ? 0 : 1, h->FindBin(xmin)) : (under_over) ? 0 : 1;
      const int bin_hi = (xmin < xmax) ? std::min(h->GetNbinsX() + ((under_over) ? 1 : 0), h->FindBin(xmax)) : h->GetNbinsX() + ((under_over) ? 1 : 0);
      return h->Integral(bin_lo, bin_hi);
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
