#include "interface/JetPUWeight.hh"

using namespace CLFV;


//-------------------------------------------------------------------------------------------------------------------------
JetPUWeight::JetPUWeight(int seed) {
  const std::vector<int> years = {2016, 2017, 2018};
  rnd_ = new TRandom3(seed);
  const TString cmssw = gSystem->Getenv("CMSSW_BASE");
  const TString path = (cmssw == "") ? "../scale_factors" : cmssw + "/src/CLFVAnalysis/scale_factors";
  TFile* feff = TFile::Open(Form("%s/scalefactorsPUID_81Xtraining.root", path.Data()), "READ"); //given scales
  if(feff) {
    for(int year : years) {
      efficiencies_[year] = (TH2F*) feff->Get(Form("h2_eff_sf%i_M", year));
      if(!efficiencies_[year]) {
        printf("!!! %s: Jet PU scale factors for year %i not found!\n", __func__, year);
      } else {
        efficiencies_[year]->SetDirectory(0);
      }
      TFile* f = TFile::Open(Form("%s/jet_puid_mumu_%i.root", path.Data(), year), "READ"); //measured efficiencies
      if(f) {
        hists_[year] = (TH2D*) f->Get("hRatio");
        if(!hists_[year]) printf("!!! %s: Jet PU MC Efficiencies for year %i not found!\n", __func__, year);
        else {
          hists_[year]->SetName(Form("hRatio_%i", year));
          hists_[year]->SetDirectory(0);
        }
        f->Close();
        delete f;
      }
    }
  }
  feff->Close();
  delete feff;
}

//-------------------------------------------------------------------------------------------------------------------------
JetPUWeight::~JetPUWeight() {
  for(std::pair<int, TH2F*> val : efficiencies_) {if(val.second) delete val.second;}
  for(std::pair<int, TH2D*> val : hists_       ) {if(val.second) delete val.second;}
  if(rnd_) delete rnd_;
}

//-------------------------------------------------------------------------------------------------------------------------
float JetPUWeight::GetMCEff(int year, float jetpt, float jeteta) {
  if(jetpt > 49.99) jetpt = 49.99;
  else if(jetpt < 15.) jetpt = 15.;
  if(std::fabs(jeteta) > 4.99) jeteta = (jeteta < 0.) ? -4.99 : 4.99;
  const TH2D* h = hists_[year];
  if(!h) {
    std::cout << "!!! JetPUWeight::" << __func__ << " Undefined histogram for  year = " << year << std::endl;
    return 0.5; //safe value in 1/x or 1/(1-x)
  }
  const int binx = h->GetXaxis()->FindBin(jetpt);
  const int biny = h->GetYaxis()->FindBin(jeteta);
  float eff = h->GetBinContent(binx, biny);
  if(eff <= 0.) {
    std::cout << "!!! JetPUWeight::" << __func__ << ": Warning! MC Eff <= 0 = " << eff
              << " jetpt = " << jetpt << " jeteta = " << jeteta << std::endl;
    eff = 0.000001;
  }
  return eff;
}

//-------------------------------------------------------------------------------------------------------------------------
float JetPUWeight::GetScaleFactor(int year, float jetpt, float jeteta) {
  float scale_factor(1.);
  if(jetpt > 49.99) jetpt = 49.99; //maximum pT
  else if(jetpt < 15.) jetpt = 15.;
  if(std::fabs(jeteta) > 4.99) jeteta = (jeteta < 0.) ? -4.99 : 4.99;
  const TH2F* h = efficiencies_[year];
  const int binx = h->GetXaxis()->FindBin(jetpt);
  const int biny = h->GetYaxis()->FindBin(jeteta);
  scale_factor = h->GetBinContent(binx, biny);
  if(scale_factor <= 0.) {
    std::cout << "!!! JetPUWeight::" << __func__ << ": Warning! scale factor <= 0 = " << scale_factor
              << " jetpt = " << jetpt << " jeteta = " << jeteta << std::endl;
    scale_factor = 0.000001;
  }
  return scale_factor;
}

//-------------------------------------------------------------------------------------------------------------------------
float JetPUWeight::GetWeight(int year, int njets, float* jetspt, float* jetseta,
                int nrejjets, float* jetsrejpt, float* jetsrejeta) {
  float weight(1.), prob_data(1.), prob_mc(1.);
  //accepted jets
  for(int jet = 0; jet < njets; ++jet) {
    if(jetspt[jet] < 20.) continue; //below pT threshold
    if(jetspt[jet] >= 50.) continue; //above PU ID pT threshold
    const float p_mc = GetMCEff(year, jetspt[jet], jetseta[jet]);
    const float scale_factor = GetScaleFactor(year, jetspt[jet], jetseta[jet]);
    const float p_data = std::max(0.f, std::min(1.f, p_mc*scale_factor)); //0 <= probability <= 1
    prob_data *= p_data;
    prob_mc   *= p_mc;
  }
  //rejected jets
  for(int jet = 0; jet < nrejjets; ++jet) {
    if(jetsrejpt[jet] < 20.) continue; //below pT threshold
    if(jetsrejpt[jet] >= 50.) continue; //above PU ID pT threshold
    const float p_mc = GetMCEff(year, jetsrejpt[jet], jetsrejeta[jet]);
    const float scale_factor = GetScaleFactor(year, jetsrejpt[jet], jetsrejeta[jet]);
    const float p_data = std::max(0.f, std::min(1.f, p_mc*scale_factor)); //0 <= probability <= 1
    prob_data *= 1. - p_data;
    prob_mc   *= 1. - p_mc;
  }
  //get overall scale factor as ratio of probabilities
  if(prob_mc > 0. && prob_mc <= 1. && prob_data <= 1.)
    weight = prob_data / prob_mc;
  else {
    std::cout << "!!! JetPUWeight::" << __func__ << ": weight undefined! Returning 1...\n";
    return 1.;
  }
  if(weight <= 0.) {
    std::cout << "!!! JetPUWeight::" << __func__ << ": weight is <= 0 = " << weight << std::endl;
    weight = 1.;
  }
  return weight;
}
