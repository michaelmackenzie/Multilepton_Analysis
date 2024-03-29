#ifndef __SCALEUNCERTAINTY__
#define __SCALEUNCERTAINTY__

#include <vector>
#include "TString.h"

namespace CLFV {

  //Scale uncertainty object
  struct ScaleUncertainty_t {
    TString  name_   ; //name of the uncertainty
    Double_t scale_  ; //scale to apply overall
    TString  process_; //process to consider, "" to not apply
    std::vector<TString>  tags_ ; //process tag to consider, "" to not apply
    std::vector<TString>  vetos_; //process tag to reject, "" to not apply
    std::vector<Int_t>    years_   ; //year to consider, < 0 to not apply
    Bool_t   data_   ; //apply to data or not
    Bool_t   mc_     ; //apply to MC or not
    Bool_t   embed_  ; //apply to Embedding or not

    ScaleUncertainty_t(TString name = "", Double_t scale = 1.,
                       TString process = "", TString tag = "", TString veto = "",
                       Int_t year = -1,
                       Bool_t data = false, Bool_t mc = true, Bool_t embed = true): name_(name), scale_(scale), process_(process),
                                                                                    data_(data), mc_(mc), embed_(embed) {
      if(tag != "") tags_ = {tag};
      else tags_ = {};
      if(veto != "") vetos_ = {veto};
      else vetos_ = {};
      if(year > 2000) years_ = {year};
      else years_ = {};
    }

    ScaleUncertainty_t(TString name, Double_t scale,
                       TString process, std::vector<TString> tags, std::vector<TString> vetos,
                       Int_t year = -1,
                       Bool_t data = false, Bool_t mc = true, Bool_t embed = true): name_(name), scale_(scale), process_(process),
                                                                                    tags_(tags), vetos_(vetos),
                                                                                    data_(data), mc_(mc), embed_(embed) {
      if(year > 2000) years_ = {year};
      else years_ = {};
    }

    ScaleUncertainty_t(TString name, Double_t scale,
                       TString process, std::vector<TString> tags, std::vector<TString> vetos,
                       std::vector<Int_t> years,
                       Bool_t data = false, Bool_t mc = true, Bool_t embed = true): name_(name), scale_(scale), process_(process),
                                                                                    tags_(tags), vetos_(vetos), years_(years),
                                                                                    data_(data), mc_(mc), embed_(embed) {}

    void Print() {
      printf("Name = %s, scale = %.4f, process = %s, tags = {", name_.Data(), scale_, process_.Data());
      for(TString tag : tags_) printf("%s, ", tag.Data());
      printf("}, vetos = {");
      for(TString veto : vetos_) printf("%s, ", veto.Data());
      printf("}, years = {");
      for(Int_t year : years_) printf("%i, ", year);
      printf("}, data = %o, mc = %o, embed = %o\n", data_, mc_, embed_);
    }
  };
}
#endif
