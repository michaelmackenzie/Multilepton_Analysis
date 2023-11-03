#include "interface/EmbBDTUncertainty.hh"

using namespace CLFV;

//-------------------------------------------------------------------------------------------------------------------------
EmbBDTUncertainty::EmbBDTUncertainty() {
  TFile* f = 0;
  const std::vector<int> years = {2016, 2017, 2018};
  const std::vector<TString> selections = {"zmutau","zetau","zmutau_e","zetau_mu"};
  const TString cmssw = gSystem->Getenv("CMSSW_BASE");
  const TString path = (cmssw == "") ? "../scale_factors" : cmssw + "/src/CLFVAnalysis/scale_factors";
  for(int year : years) {
    for(TString selection : selections) {
      f = TFile::Open(Form("%s/emb_bdt_%s_%i.root", path.Data(), selection.Data(), year), "READ");
      if(f) {
        //Get ratio histogram
        TH1* h = (TH1*) f->Get("ratio");
        if(!h) std::cout << "EmbBDTUncertainty::" << __func__ << ": No BDT histogram found for selection = "
                         << selection.Data()
                         << " year = " << year << std::endl;
        else {
          h = (TH1*) h->Clone(Form("emb_bdt_%s_%i", selection.Data(), year));
          h->SetDirectory(0);
          scales_[selection][year] = h;
        }
        f->Close();
        delete f;
      }
    }
  }
}

//-------------------------------------------------------------------------------------------------------------------------
// Cleanup upon destructor call
EmbBDTUncertainty::~EmbBDTUncertainty() {
  for(std::map<TString, std::map<int, TH1*>>::iterator it = scales_.begin(); it != scales_.end(); it++) {
    std::map<int, TH1*> val = it->second;
    for(std::map<int, TH1*>::iterator it_2 = val.begin(); it_2 != val.end(); it_2++) {
      auto val2 = it_2->second;
      if(val2) delete val2;
    }
  }
}

//-------------------------------------------------------------------------------------------------------------------------
// Uncertainty as a function of BDT score
float EmbBDTUncertainty::Weight(float score, int year, TString selection) {
  TH1* h = scales_[selection][year];
  if(!h) {
    // printf("EmbBDTUncertainty::%s: Histogram for selection %s and year %i not defined\n", __func__, selection.Data(), year);
    return 1.f;
  }

  float scale = h->GetBinContent(std::max(1, std::min(h->GetNbinsX(), h->FindBin(score))));
  if(scale <= 0.f) return 1.f;
  if(scale > 2.f) return 2.f;
  return scale;
}
