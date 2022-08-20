#include "interface/EmbeddingWeight.hh"

using namespace CLFV;

//-------------------------------------------------------------------------------------------------------------------------
EmbeddingWeight::EmbeddingWeight(int Mode, int seed, int verbose) : verbose_(verbose) {
  TFile* f = 0;
  rnd_ = new TRandom3(seed);
  useFF_ = (Mode % 10) / 1; //0: none; 1: eta unfolding; 2: (eta, pT) unfolding

  const TString cmssw = gSystem->Getenv("CMSSW_BASE");
  const TString path = (cmssw == "") ? "../scale_factors" : cmssw + "/src/CLFVAnalysis/scale_factors";
  for(int year = k2016; year <= k2018; ++year) {
    //Get variables
    f = TFile::Open(Form("%s/htt_scalefactors_legacy_%i.root", path.Data(), year + 2016), "READ");
    if(!f) {
      std::cout << "Embedding corrections file for " << year + 2016 << " not found!\n";
    } else {
      RooWorkspace* w = (RooWorkspace*) f->Get("w");
      if(!w) {
        std::cout << "Embedding workspace for " << year + 2016 << " not found!\n";
      } else {
        ws             [year]    = w;
        trigUnfold     [year]    = (RooFormulaVar*) w->obj(Form("m_sel_trg_%sratio", (year == k2016) ? "kit_" : ""));
        idUnfold       [year]    = (RooFormulaVar*) w->obj("m_sel_idEmb_ratio");
        genTauPt       [year][0] = (RooRealVar*) w->var("gt1_pt");
        genTauPt       [year][1] = (RooRealVar*) w->var("gt2_pt");
        genTauPt       [year][2] = (RooRealVar*) w->var("gt_pt");
        genTauEta      [year][0] = (RooRealVar*) w->var("gt1_eta");
        genTauEta      [year][1] = (RooRealVar*) w->var("gt2_eta");
        genTauEta      [year][2] = (RooRealVar*) w->var("gt_eta");
        muonTrig       [year][0] = (RooFormulaVar*) w->obj((year == k2017) ? "m_trg27_kit_data"  : "m_trg_data");
        muonTrig       [year][1] = (RooFormulaVar*) w->obj((year == k2017) ? "m_trg27_kit_embed" : "m_trg_mc");
        muonIso        [year]    = (RooFormulaVar*) w->obj((year == k2017) ? "m_iso_binned_embed_kit_ratio" : "m_iso_ratio");
        muonID         [year]    = (RooFormulaVar*) w->obj((year == k2017) ? "m_id_embed_kit_ratio" : "m_id_ratio");
        muonPt         [year]    = (RooRealVar*) w->var("m_pt");
        muonEta        [year]    = (RooRealVar*) w->var("m_eta");
        electronTrig   [year][0] = (RooFormulaVar*) w->obj((year == k2017) ? "e_trg32_kit_data"  : "e_trg_data");
        electronTrig   [year][1] = (RooFormulaVar*) w->obj((year == k2017) ? "e_trg32_kit_embed" : "e_trg_mc");
        electronID     [year]    = (RooFormulaVar*) w->obj((year == k2017) ? "e_id80_kit_ratio" : "e_id_ratio");
        electronIso    [year]    = (RooFormulaVar*) w->obj((year == k2017) ? "e_iso_binned_embed_kit_ratio" : "e_iso_ratio"); //the ID is without iso, so apply this as well
        electronPt     [year]    = (RooRealVar*) w->var("e_pt");
        electronEta    [year]    = (RooRealVar*) w->var("e_eta");
      }
      files_.push_back(f);
    }
    //Get FF
    f = TFile::Open(Form("%s/embedding_unfolding_%i.root", path.Data(), year + 2016), "READ");
    if(f) {
      zetaFF[year] = (TH1*) f->Get("ZEtaUnfolding");
      zetavptFF[year] = (TH2*) f->Get("ZEtaVsPtUnfolding");
      files_.push_back(f);
    } else if(useFF_) {
      printf("%s: Z eta unfolding factors not found for year %i\n", __func__, year+2016);
    }
  }
}

//-------------------------------------------------------------------------------------------------------------------------
EmbeddingWeight::~EmbeddingWeight() { for(unsigned i = 0; i < files_.size(); ++i) files_[i]->Close(); }


//-------------------------------------------------------------------------------------------------------------------------
double EmbeddingWeight::UnfoldingWeight(double pt_1, double eta_1, double pt_2, double eta_2, double zeta, double zpt, int year) {
  if(year > 2000) year -= 2016;
  if(year != k2016 && year != k2017 && year != k2018) {
    std::cout << "Warning! Undefined year in EmbeddingWeight::" << __func__ << ", returning 1" << std::endl;
    return 1.;
  }
  if(!genTauPt[year][0] || !genTauPt[year][1] || !genTauPt[year][2] || !genTauEta[year][0] || !genTauEta[year][1] || !genTauEta[year][2]) {
    std::cout << "Error! EmbeddingWeight::" << __func__ << ": Gen tau variables not found!\n";
    return 1.;
  }
  if(!trigUnfold[year] || !idUnfold[year]) {
    std::cout << "Error! EmbeddingWeight::" << __func__ << ": Unfolding functions not found!\n";
    return 1.;
  }

  ///////////////////////////
  // Apply trigger unfolding
  ///////////////////////////

  genTauPt [year][0]->setVal(pt_1);
  genTauPt [year][1]->setVal(pt_2);
  genTauEta[year][0]->setVal(eta_1);
  genTauEta[year][1]->setVal(eta_2);
  const double trigger(trigUnfold[year]->evaluate());

  ///////////////////////////
  // Apply each ID unfolding
  ///////////////////////////

  genTauPt [year][2]->setVal(pt_1);
  genTauEta[year][2]->setVal(eta_1);
  const double id_1 = idUnfold[year]->evaluate();
  genTauPt [year][2]->setVal(pt_2);
  genTauEta[year][2]->setVal(eta_2);
  const double id_2 = idUnfold[year]->evaluate();

  //unfolding corrections should be greater than 1 as they account for inefficiencies
  if(trigger < 1. || id_1 < 1. || id_2 < 1.) {
    printf("EmbeddingWeight::%s: Warning! Some unfolding corrections are below 1! trig = %.3f, id(1) = %.3f, id(2) = %.3f",
           __func__, trigger, id_1, id_2);
    printf("; pt(1) = %.1f, eta(1) = %.2f, pt(2) = %.1f, eta(2) = %.2f\n", pt_1, eta_1, pt_2, eta_2);
  }
  double scale_factor = std::max(1., trigger)*std::max(1., id_1)*std::max(1., id_2);

  ///////////////////////////
  // Apply Z eta unfolding
  ///////////////////////////

  if(useFF_ == 1) {
    TH1* hzeta = zetaFF[year];
    if(!hzeta) {
      printf("EmbeddingWeight::%s: Z eta unfolding correction not found for year %i\n",
             __func__, year + 2016);
    } else {
      scale_factor *= std::min(2., hzeta->GetBinContent(std::max(1, std::min(hzeta->GetNbinsX(), hzeta->FindBin(zeta)))));
    }
  } else if(useFF_ == 2) {
    TH2* hzetavspt = zetavptFF[year];
    if(!hzetavspt) {
      printf("EmbeddingWeight::%s: Z eta vs pT unfolding correction not found for year %i\n",
             __func__, year + 2016);
    } else {
      double ff = std::min(2., hzetavspt->GetBinContent(hzetavspt->FindBin(std::fabs(zeta), zpt)));
      if(ff <= 0.) ff = 1.;
      scale_factor *= ff;
    }
  }

  if(scale_factor <= 0.) {
    std::cout << "Warning! Scale factor <= 0 in EmbeddingWeight::" << __func__ << ", returning 1" << std::endl;
    return 1.;
  }
  return scale_factor;
}

//-------------------------------------------------------------------------------------------------------------------------
double EmbeddingWeight::MuonIDWeight(double pt, double eta, int year) {
  year -= 2016;
  if(year != k2016 && year != k2017 && year != k2018) {
    std::cout << "Warning! Undefined year in EmbeddingWeight::" << __func__ << ", returning 1" << std::endl;
    return 1.;
  }
  if(!muonPt[year] || !muonEta[year]) {
    std::cout << "Error! EmbeddingWeight::" << __func__ << ": Muon variables not found!\n";
    return 1.;
  }
  if(!muonIso[year] || !muonID[year]) {
    std::cout << "Error! EmbeddingWeight::" << __func__ << ": Weight functions not found!\n";
    return 1.;
  }

  ///////////////////////////
  // Apply Iso weight
  ///////////////////////////

  muonPt [year]->setVal(pt);
  muonEta[year]->setVal(eta);
  double scale_factor(muonIso[year]->evaluate());

  ///////////////////////////
  // Apply ID weight
  ///////////////////////////

  scale_factor *= muonID[year]->evaluate();

  if(scale_factor <= 0.) {
    std::cout << "Warning! Scale factor <= 0 in EmbeddingWeight::" << __func__ << ", returning 1" << std::endl;
    return 1.;
  }
  return scale_factor;
}

//-------------------------------------------------------------------------------------------------------------------------
double EmbeddingWeight::MuonTriggerWeight(double pt, double eta, int year, float& data_eff, float& mc_eff) {
  year -= 2016;
  mc_eff = 0.5; data_eff = 0.5;
  if(year != k2016 && year != k2017 && year != k2018) {
    std::cout << "Warning! Undefined year in EmbeddingWeight::" << __func__ << ", returning 1" << std::endl;
    return 1.;
  }
  if(!muonPt[year] || !muonEta[year]) {
    std::cout << "Error! EmbeddingWeight::" << __func__ << ": Muon variables not found!\n";
    return 1.;
  }
  if(!muonTrig[year][0] || !muonTrig[year][1]) {
    std::cout << "Error! EmbeddingWeight::" << __func__ << ": Trigger functions not found!\n";
    return 1.;
  }

  ///////////////////////////
  // Assert allowed ranges
  ///////////////////////////

  //not able to trigger
  if((pt < 25. && year != k2017) || (pt < 28. && year == k2017)) {
    return 1.;
  }
  eta = std::max(-2.39, std::min(2.39, eta));

  ///////////////////////////
  // Get efficiencies
  ///////////////////////////

  muonPt [year]->setVal(pt);
  muonEta[year]->setVal(eta);
  data_eff = muonTrig[year][0]->evaluate();
  mc_eff   = muonTrig[year][1]->evaluate();
  double scale_factor((mc_eff > 0.) ? data_eff / mc_eff : 1.);

  if(scale_factor <= 0. || data_eff <= 0. || mc_eff <= 0.) {
    std::cout << "Warning! Scale factor <= 0 in EmbeddingWeight::" << __func__
              << ": data_eff = " << data_eff << " mc_eff = " << mc_eff
              << "; pt = " << pt << " eta = " << eta
              << "; returning 1" << std::endl;
    //0.5 is safest for efficiencies, so no 0/0 in eff/eff or (1-eff)/(1-eff)
    mc_eff = 0.5;
    data_eff = 0.5;
    return 1.;
  }
  return scale_factor;
}

//-------------------------------------------------------------------------------------------------------------------------
double EmbeddingWeight::ElectronIDWeight(double pt, double eta, int year) {
  year -= 2016;
  if(year != k2016 && year != k2017 && year != k2018) {
    std::cout << "Warning! Undefined year in EmbeddingWeight::" << __func__ << ", returning 1" << std::endl;
    return 1.;
  }
  if(!electronPt[year] || !electronEta[year]) {
    std::cout << "Error! EmbeddingWeight::" << __func__ << ": Electron variables not found!\n";
    return 1.;
  }
  if(!electronID[year] || !electronIso[year]) {
    std::cout << "Error! EmbeddingWeight::" << __func__ << ": Weight functions not found!\n";
    return 1.;
  }

  ///////////////////////////
  // Apply ID weight
  ///////////////////////////

  electronPt [year]->setVal(pt);
  electronEta[year]->setVal(eta);
  double scale_factor(electronID[year]->evaluate() * electronIso[year]->evaluate());

  if(scale_factor <= 0.) {
    std::cout << "Warning! Scale factor <= 0 in EmbeddingWeight::" << __func__ << ", returning 1" << std::endl;
    return 1.;
  }
  return scale_factor;
}

//-------------------------------------------------------------------------------------------------------------------------
double EmbeddingWeight::ElectronTriggerWeight(double pt, double eta, int year, float& data_eff, float& mc_eff) {
  year -= 2016;
  mc_eff = 0.5; data_eff = 0.5;
  if(year != k2016 && year != k2017 && year != k2018) {
    std::cout << "Warning! Undefined year in EmbeddingWeight::" << __func__ << ", returning 1" << std::endl;
    return 1.;
  }
  if(!electronPt[year] || !electronEta[year]) {
    std::cout << "Error! EmbeddingWeight::" << __func__ << ": Electron variables not found!\n";
    return 1.;
  }
  if(!electronTrig[year][0] || !electronTrig[year][1]) {
    std::cout << "Error! EmbeddingWeight::" << __func__ << ": Trigger functions not found!\n";
    return 1.;
  }

  ///////////////////////////
  // Assert allowed ranges
  ///////////////////////////

  //not able to trigger
  if((pt < 33. && year != k2016) || (pt < 28. && year == k2016)) {
    return 1.;
  }
  eta = std::max(-2.49, std::min(2.49, eta));

  ///////////////////////////
  // Get efficiencies
  ///////////////////////////

  electronPt [year]->setVal(pt);
  electronEta[year]->setVal(eta);
  data_eff = electronTrig[year][0]->evaluate();
  mc_eff   = electronTrig[year][1]->evaluate();
  double scale_factor((mc_eff > 0.) ? data_eff / mc_eff : 1.);

  if(scale_factor <= 0. || data_eff <= 0. || mc_eff <= 0.) {
    std::cout << "Warning! Scale factor <= 0 or NaN in EmbeddingWeight::" << __func__
              << ": data_eff = " << data_eff << " mc_eff = " << mc_eff
              << "; pt = " << pt << " eta = " << eta
              << "; returning 1" << std::endl;
    //0.5 is safest for efficiencies, so no 0/0 in eff/eff or (1-eff)/(1-eff)
    mc_eff = 0.5;
    data_eff = 0.5;
    return 1.;
  }
  return scale_factor;
}
