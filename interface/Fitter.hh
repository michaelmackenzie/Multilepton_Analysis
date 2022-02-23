#ifndef __FITTER__
#define __FITTER__

#include "TString.h"
#include "TFile.h"
#include "TH1.h"
#include "TF1.h"
#include "THStack.h"
#include "TCanvas.h"


namespace CLFV {

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

    ClassDef(Fitter, 0);
  };
}
#endif
