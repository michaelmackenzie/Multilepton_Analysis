#include "interface/EmbeddingWeight.hh"

using namespace CLFV;

//-------------------------------------------------------------------------------------------------------------------------
EmbeddingWeight::EmbeddingWeight(int seed, int verbose) : verbose_(verbose) {
  TFile* f = 0;
  rnd_ = new TRandom3(seed);

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
        electronIso    [year]    = (RooFormulaVar*) w->obj((year == k2017) ? "e_iso_binned_embed_kit_ratio" : "e_iso_ratio"); //the KIT ID is without iso, so apply this as well
        electronPt     [year]    = (RooRealVar*) w->var("e_pt");
        electronEta    [year]    = (RooRealVar*) w->var("e_eta");
      }
      files_.push_back(f);
    }
  }
}

//-------------------------------------------------------------------------------------------------------------------------
EmbeddingWeight::~EmbeddingWeight() { for(unsigned i = 0; i < files_.size(); ++i) files_[i]->Close(); }


//-------------------------------------------------------------------------------------------------------------------------
double EmbeddingWeight::UnfoldingWeight(double pt_1, double eta_1, double pt_2, double eta_2, int year) {
  year -= 2016;
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
  double scale_factor(trigUnfold[year]->evaluate());

  ///////////////////////////
  // Apply each ID unfolding
  ///////////////////////////

  genTauPt [year][2]->setVal(pt_1);
  genTauEta[year][2]->setVal(eta_1);
  scale_factor *= idUnfold[year]->evaluate();
  genTauPt [year][2]->setVal(pt_2);
  genTauEta[year][2]->setVal(eta_2);
  scale_factor *= idUnfold[year]->evaluate();

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
