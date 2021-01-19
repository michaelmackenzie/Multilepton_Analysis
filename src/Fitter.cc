#include "TString.h"
#include "TFile.h"
#include "TH1.h"
#include "TF1.h"
#include "THStack.h"
#include "TCanvas.h"


class Fitter : public TObject {

public:
  Fitter() : TObject() {}

  Int_t    Get_order(Int_t set);
  Double_t Get_set_norm(Int_t set, Int_t signal);
  void     Get_fit_values(Int_t set, Double_t* coeffs, Double_t* errors);
  TF1*     Get_background_function(Int_t order);
  TF1*     Get_signal_function();
  TF1*     Get_fit_background_function(Int_t set, Double_t sigmaRange);
  TF1*     Get_fit_signal_function(Int_t set);

  Double_t lum_;
  Int_t signal_mock_data_ = 0;
  Int_t set5Order_  = 3;
  Int_t set6Order_  = 3;
  Int_t set7Order_  = 3;
  Int_t set10Order_ = 2;
  Int_t set11Order_ = 2;
  Int_t set12Order_ = 2;
  Int_t set111Order_ = 3; //set 11 in tmva tree fits
  Int_t set102Order_ = 4; //set 2  in tmva tree fits
  Int_t set103Order_ = 4; //set 3  in tmva tree fits
  Int_t set106Order_ = 3; //set 6  in tmva tree fits
  Int_t set113Order_ = 5; //set 13 in tmva tree fits
  Double_t fitMin_ = 12.;
  Double_t fitMax_ = 70.;
};

Int_t Fitter::Get_order(Int_t set) {
  if(set == 5)  return set5Order_;
  if(set == 6)  return set6Order_;
  if(set == 7)  return set7Order_;
  if(set == 10) return set10Order_;
  if(set == 11) return set11Order_;
  if(set == 12) return set12Order_;
  if(set == 111) return set111Order_;
  if(set == 102) return set102Order_;
  if(set == 103) return set103Order_;
  if(set == 106) return set106Order_;
  if(set == 113) return set113Order_;
  return -1;
}

Double_t Fitter::Get_set_norm(Int_t set, Int_t signal) {
  Double_t lumNorm = 5.3e3; //luminosity fits done at
  Double_t entries = 0.;
  Double_t sigEntries = 0.;
  switch(set) {
  case 5 : entries = 202.; sigEntries = 252.; break;
  case 6 : entries = 129.; sigEntries = 153.; break;
  case 7 : entries = 72.6; sigEntries = 99.5; break;
  case 10: entries = 235.; sigEntries = 261.; break;
  case 11: entries = 44.9; sigEntries = 69.1; break;
  case 12: entries = 280.; sigEntries = 330.; break;
  default: return 0.;
  }
  Double_t norm = (signal > 0) ? entries/lumNorm : sigEntries/lumNorm;
  return norm;
}
void Fitter::Get_fit_values(Int_t set, Double_t* coeffs, Double_t* errors) {
  Int_t order = Get_order(set);
  if(order < 0) return;
  Double_t coeff5[] = {1.3826e1,-4.8423e-1,-6.8548e-2,1.6258e-1};
  Double_t coeff5Err[] = {1.e3, 1.26e-2,2.07e-2,2.09e-2};
  Double_t coeff6[] = {8.79050e0,-4.92225e-1,-5.33637e-02,1.56331e-01};
  Double_t coeff6Err[] = {1.e3, 1.58157e-02,2.64321e-02,2.61540e-02};
  Double_t coeff7[] = {4.90373e+00,-4.83293e-01,-8.13811e-02,1.79712e-01};
  Double_t coeff7Err[] = {1.e3, 2.06791e-02,3.31240e-02,3.40737e-02};
  
  Double_t coeff10[] = {1.59932e+01,9.97308e-02,5.38212e-03};
  Double_t coeff10Err[] = {1.e3,1.45954e-02,1.80583e-02};
  Double_t coeff11[] = {2.87433e+00,1.61898e-01,1.55672e-01};
  Double_t coeff11Err[] = {1.e3,4.01819e-02,5.14662e-02};
  Double_t coeff12[] = {1.9169e1,1.1108e-1,3.0578e-2};
  Double_t coeff12Err[] = {1.e3,1.38e-2,1.72e-2};

  //set 100 + set tmva tree fits
  Double_t coeff111[] = {5.61005e+02, 2.15280e-01,-7.71547e-02, -6.52856e-02};
  Double_t coeff111Err[] = {1.e3,8.98070e-03, 1.20303e-02, 1.43565e-02};
  Double_t coeff102[] = {7.44585e+03, -6.54640e-02, 1.56222e-01, -8.68367e-02, 7.07079e-02};
  Double_t coeff102Err[] = {1.e4, 7.07079e-02, 3.35549e-03, 4.07028e-03, 4.79137e-03};
  Double_t coeff103[] = {2.98185e+03, -1.04483e-01, 1.87648e-01, -8.84354e-02,  9.00702e-02};
  Double_t coeff103Err[] = {1.e4, 4.30835e-03, 5.39498e-03, 6.46380e-03, 7.60702e-03};
  Double_t coeff106[] = {2.10387e+02, 1.79814e-01, -1.22680e-02, 1.43777e-02};
  Double_t coeff106Err[] = {1.e4, 1.52679e-02, 2.03617e-02, 2.38027e-02};
  Double_t coeff113[] = {1.53702e+03, -5.90429e-01, 8.21394e-02, 1.44719e-01, -1.11216e-01, 4.22580e-02};
  Double_t coeff113Err[] = {1.e3, 4.51421e-03, 7.44619e-03, 8.97758e-03, 1.02478e-02, 9.37706e-03};

  for(int i = 0; i <= order; ++i) {
    if(set == 5) {
      coeffs[i] = coeff5[i];
      errors[i] = coeff5Err[i];
    }
    else if(set == 6) {
      coeffs[i] = coeff6[i];
      errors[i] = coeff6Err[i];
    }
    else if(set == 7) {
      coeffs[i] = coeff7[i];
      errors[i] = coeff7Err[i];
    }
    else if(set == 10) {
      coeffs[i] = coeff10[i];
      errors[i] = coeff10Err[i];
    }
    else if(set == 11) {
      coeffs[i] = coeff11[i];
      errors[i] = coeff11Err[i];
    }
    else if(set == 12) {
      coeffs[i] = coeff12[i];
      errors[i] = coeff12Err[i];
    }
    else if(set == 111) {
      coeffs[i] = coeff111[i];
      errors[i] = coeff111Err[i];
    }
    else if(set == 113) {
      coeffs[i] = coeff113[i];
      errors[i] = coeff113Err[i];
    }
    else if(set == 102) {
      coeffs[i] = coeff102[i];
      errors[i] = coeff102Err[i];
    }
    else if(set == 103) {
      coeffs[i] = coeff103[i];
      errors[i] = coeff103Err[i];
    }
    else if(set == 106) {
      coeffs[i] = coeff106[i];
      errors[i] = coeff106Err[i];
    }
  }
}

TF1* Fitter::Get_background_function(Int_t order = 2) {
  //Background is legendre polynomials?
  TString eq = "[0]*(0.5";
  Double_t coeff[] = {1. ,
		      3./2., -1./2.,
		      5./2., -3./2.,
		      35./8., -30./8., 3./8.,
		      63./8., -70./8., 15./8.,
		      231./16., -315./16., 105./16., -5./16.};
  Int_t index0[] = {0,1,3,5,8,11}; //too lazy to do math
  Int_t numImplemented = 6;
  TString var = Form("(2*(x-%.2f)/(%.2f-%.2f) - 1)",fitMin_,fitMax_,fitMin_);
  for(int i = 0; i < std::min(order,numImplemented); ++i) {
    eq = eq + Form(" + [%i]*(",i+1);
    eq = eq + Form("%.4f*%s^%i",coeff[index0[i]],var.Data(),i+1);
    for(int j = 1; j <= (i+1)/2; ++j) {
      int index = index0[i]+j;
      eq = eq + Form(" + %.4f*%s^%i",coeff[index],var.Data(),i+1-2*j);
    }
    eq = eq + ")";
  }
  eq = eq + ")";
  TF1* f = new TF1("fBackground",eq.Data());
  for(int i = 0; i <= order; ++i) {
    f->SetParameter(i,0.1);
    if(i > 0) f->SetParLimits(i,-1.,1.);
    f->SetParName(i,Form("a%i",i));
  }
  // printf("Equation: %s\n",eq.Data());
  return f;
}

TF1* Fitter::Get_signal_function() {
  //assume gaussian for now
  TF1* fBW = new TF1("fSignal","[Norm]/sqrt(2.*3.14159265)/[Sigma]*exp(-0.5*(x-[Mode])*(x-[Mode])/[Sigma]/[Sigma])");
  fBW->SetParameters(28.9,5.,1.8);
  fBW->SetParLimits(0,fitMin_+2.,fitMax_-2.); // Mode
  fBW->SetParLimits(1,0.,5e4); // Norm
  fBW->SetParLimits(2,.1,10.); // Sigma
  fBW->SetRange(fitMin_,fitMax_);
  return fBW;
}

TF1* Fitter::Get_fit_background_function(Int_t set, Double_t sigmaRange = 2.) {
  TF1* f;
  Int_t order = Get_order(set);
  if(order < 0) return NULL;
  f = Get_background_function(order);
  Double_t coeffs[order+1];
  Double_t errors[order+1];
  Get_fit_values(set,coeffs,errors);
  for(int i = 0; i <= order; ++i) {
    f->SetParameter(i,coeffs[i]);
    f->SetParLimits(i,coeffs[i]-sigmaRange*errors[i],coeffs[i]+sigmaRange*errors[i]);    
  }
  f->SetRange(fitMin_,fitMax_);
  return f;
}

TF1* Fitter::Get_fit_signal_function(Int_t set) {
  TF1* f = Get_signal_function();
  if(set == 5)
    f->SetParameters(2.85e1,84.,1.3);
  else if(set == 6)
    f->SetParameters(2.85e1,42.,1.3);
  else if(set == 7)
    f->SetParameters(2.85e1,42.,1.3);
  else if(set == 10)
    f->SetParameters(2.85e1,43.,1.3);
  else if(set == 11)
    f->SetParameters(2.85e1,34.,1.3);
  else if(set == 12)
    f->SetParameters(2.85e1,77.,1.3);
  else return NULL;
  
  f->SetRange(fitMin_,fitMax_);
  return f;
}
