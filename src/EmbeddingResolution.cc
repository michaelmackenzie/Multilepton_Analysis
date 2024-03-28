#include "interface/EmbeddingResolution.hh"

using namespace CLFV;

//-------------------------------------------------------------------------------------------------------------------------
EmbeddingResolution::EmbeddingResolution(const int in_year, const int verbose) : verbose_(verbose) {
  const std::vector<int> years = {2016, 2017, 2018};
  const TString cmssw = gSystem->Getenv("CMSSW_BASE");
  const TString path = (cmssw == "") ? "../scale_factors" : cmssw + "/src/CLFVAnalysis/scale_factors";
  TFile* f(nullptr);
  for(int year : years) {
    if(in_year >= 2016 && in_year <= 2018 && year != in_year) continue;

    //////////////////////////////
    // Electron corrections

    //energy scale corrections
    f = TFile::Open(Form("%s/embed_scale_correction_electron_%i.root", path.Data(), year), "READ");
    elec_scale_[year] = {};
    if(f) {
      //Get the correction graph for each |eta| region
      for(int ieta = 0; ; ++ieta) {
        auto g = (TGraphAsymmErrors*) f->Get(Form("lepptvsres_eta_%i_mean_diff", ieta));
        if(!g) break;
        g = (TGraphAsymmErrors*) g->Clone(Form("electron_scale_correction_%i_%i", ieta, year));
        if(verbose_ > 1) printf(" --> Adding graph %s for year %i |eta| region %i\n", g->GetName(), year, ieta);
        elec_scale_[year][ieta] = g;
      }
      if(verbose_) printf("EmbeddingResolution: Found %lu electron scale correction graphs for %i\n", elec_scale_[year].size(), year);
      f->Close();
      delete f;
    }

    //energy resolution corrections
    //FIXME: Decide if these should be before/after scale corrections (scale vs. resolution correction file)
    f = TFile::Open(Form("%s/embed_resolution_correction_electron_%i.root", path.Data(), year), "READ");
    elec_resolution_[year] = {};
    if(f) {
      //Get the correction graph for each |eta| region
      for(int ieta = 0; ; ++ieta) {
        auto g = (TGraphAsymmErrors*) f->Get(Form("lepptvsres_eta_%i_width_ratio", ieta));
        if(!g) break;
        g = (TGraphAsymmErrors*) g->Clone(Form("electron_resolution_correction_%i_%i", ieta, year));
        if(verbose_ > 1) printf(" --> Adding graph %s for year %i |eta| region %i\n", g->GetName(), year, ieta);
        elec_resolution_[year][ieta] = g;
      }
      if(verbose_) printf("EmbeddingResolution: Found %lu electron resolution correction graphs for %i\n", elec_resolution_[year].size(), year);
      f->Close();
      delete f;
    }

    //////////////////////////////
    // Muon corrections

    //energy scale corrections
    f = TFile::Open(Form("%s/embed_scale_correction_muon_%i.root", path.Data(), year), "READ");
    muon_scale_[year] = {};
    if(f) {
      //Get the correction graph for each |eta| region
      for(int ieta = 0; ; ++ieta) {
        auto g = (TGraphAsymmErrors*) f->Get(Form("lepptvsres_eta_%i_mean_diff", ieta));
        if(!g) break;
        g = (TGraphAsymmErrors*) g->Clone(Form("muon_scale_correction_%i_%i", ieta, year));
        if(verbose_ > 1) printf(" --> Adding graph %s for year %i |eta| region %i\n", g->GetName(), year, ieta);
        muon_scale_[year][ieta] = g;
      }
      if(verbose_) printf("EmbeddingResolution: Found %lu muon scale correction graphs for %i\n", muon_scale_[year].size(), year);
      f->Close();
      delete f;
    }

    //energy resolution corrections
    //FIXME: Decide if these should be before/after scale corrections (scale vs. resolution correction file)
    f = TFile::Open(Form("%s/embed_scale_correction_muon_%i.root", path.Data(), year), "READ");
    muon_resolution_[year] = {};
    if(f) {
      //Get the correction graph for each |eta| region
      for(int ieta = 0; ; ++ieta) {
        auto g = (TGraphAsymmErrors*) f->Get(Form("lepptvsres_eta_%i_width_ratio", ieta));
        if(!g) break;
        g = (TGraphAsymmErrors*) g->Clone(Form("muon_resolution_correction_%i_%i", ieta, year));
        if(verbose_ > 1) printf(" --> Adding graph %s for year %i |eta| region %i\n", g->GetName(), year, ieta);
        muon_resolution_[year][ieta] = g;
      }
      if(verbose_) printf("EmbeddingResolution: Found %lu muon resolution correction graphs for %i\n", muon_resolution_[year].size(), year);
      f->Close();
      delete f;
    }
  }
}

//-------------------------------------------------------------------------------------------------------------------------
// Cleanup upon destructor call
EmbeddingResolution::~EmbeddingResolution() {
  for(auto it = elec_scale_.begin(); it != elec_scale_.end(); it++) {
    auto val = it->second;
    for(auto it_2 = val.begin(); it_2 != val.end(); it_2++) {
      auto val2 = it_2->second;
      if(val2) delete val2;
    }
  }
  for(auto it = elec_resolution_.begin(); it != elec_resolution_.end(); it++) {
    auto val = it->second;
    for(auto it_2 = val.begin(); it_2 != val.end(); it_2++) {
      auto val2 = it_2->second;
      if(val2) delete val2;
    }
  }
  for(auto it = muon_scale_.begin(); it != muon_scale_.end(); it++) {
    auto val = it->second;
    for(auto it_2 = val.begin(); it_2 != val.end(); it_2++) {
      auto val2 = it_2->second;
      if(val2) delete val2;
    }
  }
  for(auto it = muon_resolution_.begin(); it != muon_resolution_.end(); it++) {
    auto val = it->second;
    for(auto it_2 = val.begin(); it_2 != val.end(); it_2++) {
      auto val2 = it_2->second;
      if(val2) delete val2;
    }
  }
}

//-------------------------------------------------------------------------------------------------------------------------
// Correct the electron energy scale
float EmbeddingResolution::ElectronScale(float eta, float gen_pt, int year, float& up, float& down) {
  up = 1.f; down = 1.f;

  //ensure reasonable gen-level info
  if(gen_pt <= 0.f) {
    if(verbose_) printf("EmbeddingResolution::%s: Undefined gen-level pT = %.3f\n", __func__, gen_pt);
    return 1.f;
  }

  //List of |eta| dependent corrections
  auto g_list = elec_scale_[year];
  unsigned ngraphs = g_list.size();

  if(ngraphs == 0) { //if none found, return no correction
    if(verbose_) printf("EmbeddingResolution::%s: No correction graphs found\n", __func__);
    return 1.f;
  }

  //determine the graph |eta| region
  unsigned index = 0;
  const float abs_eta = std::fabs(eta);
  if(ngraphs == 1) index = 0; //inclusive correction
  if(ngraphs == 2) { //barrel vs. endcap
    index = (abs_eta >= 1.479);
  } else if(ngraphs == 3) { //split barrel into two regions
    index = (abs_eta >= 0.5) + (abs_eta >= 1.479);
  } else if(ngraphs == 4) { //split barrel and endcap into 2 regions
    index = (abs_eta >= 0.5) + (abs_eta >= 1.479) + (abs_eta >= 2.1);
  } else {
    printf("EmbeddingResolution::%s: Unknown number of graph regions (%i)\n", __func__, ngraphs);
    throw 20;
  }

  //Get the graph
  auto g = g_list[index];
  if(!g) { //no correction found
    printf("EmbeddingResolution::%s: Null correction graph (index %i)\n", __func__, index);
    throw 20;
  }

  //find the bin that contains the gen-level pT
  int bin(0);
  for(int ibin = 0; ibin < g->GetN(); ++ibin) {
    bin = ibin;
    if(gen_pt < g->GetX()[ibin] + g->GetEXhigh()[ibin]) break; //check if below the upper edge of the bin
  }
  const float mean_difference = g->GetY()[bin];

  //define the correction to the reco pT
  float scale = (mean_difference + 1.f);

  //uncertainty on the lepton energy scale
  const float unc((abs_eta <= 1.479f) ? 0.005f : 0.0125f); //0.5% in the barrel, 1.25% in the endcap
  up   = scale*(1.f + unc);
  down = scale*(1.f - unc);

  //ensure reasonable correction factor sizes
  const float min_scale(0.5f), max_scale(2.f); //can't change by > a factor of 2
  scale = std::max(min_scale, std::min(max_scale, scale));
  up    = std::max(min_scale, std::min(max_scale, up   ));
  down  = std::max(min_scale, std::min(max_scale, down ));

  if(verbose_ > 1) {
    printf("EmbeddingResolution::%s: For eta = %.2f, gen-pT = %.2f, scale = %.3f (%.3f/%.3f)\n",
           __func__, eta, gen_pt, scale, up, down);
  }

  return scale;
}

//-------------------------------------------------------------------------------------------------------------------------
// Correct the electron energy resolution
float EmbeddingResolution::ElectronResolution(float pt, float eta, float gen_pt, int year, float& up, float& down) {
  up = 1.f; down = 1.f;

  //ensure reasonable gen-level info
  if(gen_pt <= 0.f) {
    if(verbose_) printf("EmbeddingResolution::%s: Undefined gen-level pT = %.3f\n", __func__, gen_pt);
    return 1.f;
  }

  //List of |eta| dependent corrections
  auto g_list = elec_resolution_[year];
  unsigned ngraphs = g_list.size();

  if(ngraphs == 0) { //if none found, return no correction
    if(verbose_) printf("EmbeddingResolution::%s: No correction graphs found\n", __func__);
    return 1.f;
  }

  //determine the graph |eta| region
  unsigned index = 0;
  const float abs_eta = std::fabs(eta);
  if(ngraphs == 1) index = 0; //inclusive correction
  if(ngraphs == 2) { //barrel vs. endcap
    index = (abs_eta >= 1.479);
  } else if(ngraphs == 3) { //split barrel into two regions
    index = (abs_eta >= 0.5) + (abs_eta >= 1.479);
  } else if(ngraphs == 4) { //split barrel and endcap into 2 regions
    index = (abs_eta >= 0.5) + (abs_eta >= 1.479) + (abs_eta >= 2.1);
  } else {
    printf("EmbeddingResolution::%s: Unknown number of graph regions (%i)\n", __func__, ngraphs);
    throw 20;
  }

  //Get the graph
  auto g = g_list[index];
  if(!g) { //no correction found
    printf("EmbeddingResolution::%s: Null correction graph (index %i)\n", __func__, index);
    throw 20;
  }

  //find the bin that contains the gen-level pT
  int bin(0);
  for(int ibin = 0; ibin < g->GetN(); ++ibin) {
    bin = ibin;
    if(gen_pt < g->GetX()[ibin] + g->GetEXhigh()[ibin]) break; //check if below the upper edge of the bin
  }
  const float width_ratio = g->GetY()[bin];

  //define the correction to the reco pT
  const float pt_err = (pt - gen_pt);
  float scale = 1.f + (width_ratio - 1.f)*(pt_err)/(gen_pt);

  //uncertainty on the lepton energy resolution width
  const float unc(0.15f); //flat 15% uncertainty on the width
  up   = scale + unc*pt_err/gen_pt;
  down = scale - unc*pt_err/gen_pt;

  //ensure reasonable correction factor sizes
  const float min_scale(0.5f), max_scale(2.f); //can't change by > a factor of 2
  scale = std::max(min_scale, std::min(max_scale, scale));
  up    = std::max(min_scale, std::min(max_scale, up   ));
  down  = std::max(min_scale, std::min(max_scale, down ));

  if(verbose_ > 1) {
    printf("EmbeddingResolution::%s: For pT = %.2f, gen-pT = %.2f (diff = %.2e), width ratio = %.2f, scale = %.3f (%.3f/%.3f)\n",
           __func__, pt, gen_pt, pt_err, width_ratio, scale, up, down);
  }

  return scale;
}

//-------------------------------------------------------------------------------------------------------------------------
// Correct the muon energy scale
float EmbeddingResolution::MuonScale(float eta, float gen_pt, int year, float& up, float& down) {
  up = 1.f; down = 1.f;

  //ensure reasonable gen-level info
  if(gen_pt <= 0.f) {
    if(verbose_) printf("EmbeddingResolution::%s: Undefined gen-level pT = %.3f\n", __func__, gen_pt);
    return 1.f;
  }

  //List of |eta| dependent corrections
  auto g_list = muon_scale_[year];
  unsigned ngraphs = g_list.size();

  if(ngraphs == 0) { //if none found, return no correction
    if(verbose_) printf("EmbeddingResolution::%s: No correction graphs found\n", __func__);
    return 1.f;
  }

  //determine the graph |eta| region
  unsigned index = 0;
  const float abs_eta = std::fabs(eta);
  if(ngraphs == 1) index = 0; //inclusive correction
  if(ngraphs == 2) { //barrel vs. endcap
    index = (abs_eta >= 1.5);
  } else if(ngraphs == 3) { //split following Embedding uncertainties
    index = (abs_eta >= 1.5) + (abs_eta >= 2.1);
  } else if(ngraphs == 4) { //split barrel and endcap into 2 regions
    index = (abs_eta >= 0.5) + (abs_eta >= 1.5) + (abs_eta >= 2.1);
  } else {
    printf("EmbeddingResolution::%s: Unknown number of graph regions (%i)\n", __func__, ngraphs);
    throw 20;
  }

  //Get the graph
  auto g = g_list[index];
  if(!g) { //no correction found
    printf("EmbeddingResolution::%s: Null correction graph (index %i)\n", __func__, index);
    throw 20;
  }

  //find the bin that contains the gen-level pT
  int bin(0);
  for(int ibin = 0; ibin < g->GetN(); ++ibin) {
    bin = ibin;
    if(gen_pt < g->GetX()[ibin] + g->GetEXhigh()[ibin]) break; //check if below the upper edge of the bin
  }
  const float mean_difference = g->GetY()[bin];

  //define the correction to the reco pT
  float scale = (mean_difference + 1.f);

  //uncertainty on the lepton energy scale
  const float unc(0.001); //flat 0.1% uncertainty
  up   = scale*(1.f + unc);
  down = scale*(1.f - unc);

  //ensure reasonable correction factor sizes
  const float min_scale(0.5f), max_scale(2.f); //can't change by > a factor of 2
  scale = std::max(min_scale, std::min(max_scale, scale));
  up    = std::max(min_scale, std::min(max_scale, up   ));
  down  = std::max(min_scale, std::min(max_scale, down ));

  if(verbose_ > 1) {
    printf("EmbeddingResolution::%s: For eta = %.2f, gen-pT = %.2f, scale = %.3f (%.3f/%.3f)\n",
           __func__, eta, gen_pt, scale, up, down);
  }
  return scale;
}

//-------------------------------------------------------------------------------------------------------------------------
// Correct the muon energy resolution
float EmbeddingResolution::MuonResolution(float pt, float eta, float gen_pt, int year, float& up, float& down) {
  up = 1.f; down = 1.f;

  //ensure reasonable gen-level info
  if(gen_pt <= 0.f) {
    if(verbose_) printf("EmbeddingResolution::%s: Undefined gen-level pT = %.3f\n", __func__, gen_pt);
    return 1.f;
  }

  const bool apply_muon_res = false; //don't apply muon resolution corrections, only an uncertainty
  float scale(1.f);
  if(apply_muon_res) {
    //List of |eta| dependent corrections
    auto g_list = muon_resolution_[year];
    unsigned ngraphs = g_list.size();

    if(ngraphs == 0) { //if none found, return no correction
      if(verbose_) printf("EmbeddingResolution::%s: No correction graphs found\n", __func__);
      return 1.f;
    }

    //determine the graph |eta| region
    unsigned index = 0;
    const float abs_eta = std::fabs(eta);
    if(ngraphs == 1) index = 0; //inclusive correction
    if(ngraphs == 2) { //barrel vs. endcap
      index = (abs_eta >= 1.5);
    } else if(ngraphs == 3) { //split following Embedding uncertainties
      index = (abs_eta >= 1.5) + (abs_eta >= 2.1);
    } else if(ngraphs == 4) { //split barrel and endcap into 2 regions
      index = (abs_eta >= 0.5) + (abs_eta >= 1.5) + (abs_eta >= 2.1);
    } else {
      printf("EmbeddingResolution::%s: Unknown number of graph regions (%i)\n", __func__, ngraphs);
      throw 20;
    }

    //Get the graph
    auto g = g_list[index];
    if(!g) { //no correction found
      printf("EmbeddingResolution::%s: Null correction graph (index %i)\n", __func__, index);
      throw 20;
    }

    //find the bin that contains the gen-level pT
    int bin(0);
    for(int ibin = 0; ibin < g->GetN(); ++ibin) {
      bin = ibin;
      if(gen_pt < g->GetX()[ibin] + g->GetEXhigh()[ibin]) break; //check if below the upper edge of the bin
    }
    const float width_ratio = g->GetY()[bin];

    //define the correction to the reco pT
    const float pt_err = (pt - gen_pt);
    scale = 1.f + (width_ratio - 1.f)*(pt_err)/gen_pt;
  } else {
    scale = 1.f;
  }

  //uncertainty on the lepton energy resolution width
  const float min_ratio(0.05f), max_ratio(0.20f);
  const float min_pt(10.f), max_pt(100.f);
  const float pt_err = pt - gen_pt;

  //add a linear width ratio offset term with higher powers of pT error to alter the tails of the resolution function
  float pt_err_scale = std::max(min_ratio, std::min(max_ratio, min_ratio + (max_ratio - min_ratio)*(gen_pt - min_pt)/(max_pt - min_pt)));
  pt_err_scale += 0.04f*std::pow(std::fabs(pt_err/gen_pt)/0.01f,2) - 0.08f*pt_err/gen_pt/0.01f;

  up   = 1.f + pt_err_scale*pt_err/gen_pt;
  down = 1.f; //no correction applied in down

  //ensure reasonable correction factor sizes
  const float min_scale(0.5f), max_scale(2.f); //can't change by > a factor of 2
  scale = std::max(min_scale, std::min(max_scale, scale));
  up    = std::max(min_scale, std::min(max_scale, up   ));
  down  = std::max(min_scale, std::min(max_scale, down ));

  if(verbose_ > 1) {
    printf("EmbeddingResolution::%s: For pT = %.2f, gen-pT = %.2f (diff = %.2e), scale = %.3f (%.3f/%.3f)\n",
           __func__, pt, gen_pt, pt_err, scale, up, down);
  }

  return scale;
}

//-------------------------------------------------------------------------------------------------------------------------
// Return the one-sided muon resolution uncertainty
float EmbeddingResolution::MuonResolutionUnc(float pt, float eta, float gen_pt, int year) {
  float up(1.f), down(1.f);
  MuonResolution(pt, eta, gen_pt, year, up, down);
  return up;
}
