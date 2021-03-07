#ifndef __DATACARD__
#define __DATACARD__

#include "TString.h"
//data information
struct DataCard_t {
  TString filename_;
  TString name_;
  TString label_;
  double xsec_;
  bool isdata_;
  bool issignal_;
  Int_t year_;
  Int_t color_;
  bool combine_; //useful for correcting cross-sections
    
  DataCard_t() : filename_(""), name_(""), label_(""), xsec_(1.), isdata_(false), issignal_(false), color_(-1), combine_(false) {}

  DataCard_t(TString filename, TString name, TString label, bool isdata, double xsec, bool issignal, int year, bool combine = false) : DataCard_t() {
    filename_ = filename;
    name_ = name;
    label_ = label;
    xsec_ = xsec;
    isdata_ = isdata;
    issignal_ = issignal;
    year_ = year;
    combine_ = combine;
  }
  DataCard_t(TString filename, TString name, TString label, bool isdata, double xsec, bool issignal, int year, int color, bool combine = false) :
    DataCard_t(filename,name,label,isdata,xsec,issignal,year,combine) {
    color_ = color;
  }
};

#endif
