#ifndef __CLFV_BDTWRAPPER__
#define __CLFV_BDTWRAPPER__

//c++ includes
#include <iostream>
#include <vector>

//xgboost
#include <xgboost/c_api.h>

//ROOT includes
#include "TString.h"

//local includes

namespace CLFV {

  class BDTWrapper {
  public:
    BDTWrapper(std::string model = "bdt.xml", int nthreads = 1, int verbose = 0) {
      verbose_ = verbose;
      status_ = 0;
      nthreads_ = std::max(1, nthreads);
      if(model != "")
        status_ = InitializeModel(model);
    }

    ~BDTWrapper() {
    }

    int InitializeModel(std::string model) {
      if(verbose_ > -1) printf("BDTWrapper::%s: Initializing BDT with weight file %s\n", __func__, model.c_str());
      XGBoosterCreate(nullptr, 0, &bdt_);
      const int status = XGBoosterLoadModel(bdt_, model.c_str());
      if(status && verbose_ > -2) printf("BDTWrapper:%s: Error! BDT failed to load, status = %i\n", __func__, status);
      if(!status) {
        if(XGBoosterSetParam(bdt_, "nthreads", Form("%i", nthreads_)) < 0 && verbose_ > -2) //limit the number of threads used in BDT evaluation to 1
          printf("BDTWrapper::%s: Failed to set N(threads) to %i\n", __func__, nthreads_);
      }
      return status;
    }

    int GetStatus() { return status_; }

    void InitializeVariables(std::vector<float*> vars) {
      vars_ = vars;
    }

    float EvaluateScore() {
      if(!bdt_) return -999.f;
      if(verbose_ > 3) printf("BDTWrapper::%s:\n", __func__);
      if(vars_.size() == 0) {
        if(verbose_ > -2) printf("BDTWrapper::%s: No variables defined!\n", __func__);
        return -999.f;
      }
      DMatrixHandle dvalues;
      float data[vars_.size()];
      for(unsigned index = 0; index < vars_.size(); ++index) data[index] = vars_[index][0];
      if(verbose_ > 5) {
        printf(" BDT input variables:\n");
        for(unsigned index = 0; index < vars_.size(); ++index) {
          printf("  %i: %f\n", (int) index, *(vars_[index]));
        }
      }
      if(verbose_ > 3) std::cout << " --> Creating matrix\n";
      int status = XGDMatrixCreateFromMat(data, 1, vars_.size(), 0.f, &dvalues);
      if(status) {
        if(verbose_ > -2) printf("BDTWrapper::%s: Error creating data matrix! Status = %i\n", __func__, status);
        return - 999.f;
      }

      if(verbose_ > 3) std::cout << " --> Evaluating prediction\n";
      bst_ulong out_len=0;
      const float* score;

      auto ret = XGBoosterPredict(bdt_, dvalues, 0, 0, 0, &out_len, &score);
      if(verbose_ > 3) std::cout << " --> Evaluated prediction\n";

      XGDMatrixFree(dvalues);

      std::vector<float> results;
      if(ret==0) {
        for(unsigned int ic=0; ic<out_len; ++ic)
          results.push_back(score[ic]);
      }
      if(verbose_ > 3) std::cout << " --> BDT score = " << results[0] << std::endl;
      return results[0];
    }

  public:
    BoosterHandle bdt_;
  private:
    int verbose_;
    std::vector<float*> vars_;
    int status_;
    int nthreads_;
  };
}




#endif
