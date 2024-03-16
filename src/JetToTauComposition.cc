#include "interface/JetToTauComposition.hh"

using namespace CLFV;

//-------------------------------------------------------------------------------------------------------------------------
// Mode = 100*(use DM binned factors) + 10*(use fits) + (variable mode)
JetToTauComposition::JetToTauComposition(const TString selection, const int set, const int Mode, const int verbose) : Mode_(Mode), verbose_(verbose) {
  TFile* f = 0;
  std::vector<int> years = {2016, 2017, 2018};
  useTwoPt_      = (Mode %   10) /   1 == 0; //composition in tau pT
  useOnePt_      = (Mode %   10) /   1 == 1; //composition in light lepton pT
  useOneDPhi_    = (Mode %   10) /   1 == 2; //composition in delta phi (light lep, MET)
  useMTOne_      = (Mode %   10) /   1 == 3; //composition in MT(light lep, MET)
  useTwoDPhi_    = (Mode %   10) /   1 == 4; //composition in delta phi (tau, MET)
  useMTTwo_      = (Mode %   10) /   1 == 5; //composition in MT(tau, MET)
  use2DMvsMTOne_ = (Mode %   10) /   1 == 6; //composition in 2D: (lepm, mtone)
  useDM_         = (Mode % 1000) / 100 == 1; //decay mode dependent compositions
  useRun2_       = (Mode % 10000) / 1000 == 1; //Run 2 composition estimate instead of by year

  if(use2DMvsMTOne_ && useDM_) {
    printf("JetToTauComposition::%s: DM composition is not implemented for 2D M vs MTOne composition factors\n", __func__);
    useDM_ = 0;
  }

  TString name                 = "jettautwoptcomp";
  if(useOnePt_)           name = "jettauoneptcomp";
  else if(useOneDPhi_)    name = "jettauonemetdphicomp";
  else if(useTwoDPhi_)    name = "jettautwometdphicomp";
  else if(useMTOne_)      name = "jettaumtonecomp";
  else if(useMTTwo_)      name = "jettaumttwocomp";
  else if(use2DMvsMTOne_) name = "jettaulepmvsmtonecomp";

  if(verbose_ > 0) {
    std::cout << __func__ << ":\n Mode = " << Mode
              << " useOnePt = " << useOnePt_
              << " useOneDPhi = " << useOneDPhi_
              << " useMTOne = " << useMTOne_
              << " useTwoPt = " << useTwoPt_
              << " useTwoDPhi = " << useTwoDPhi_
              << " useMTTwo = " << useMTTwo_
              << " use2DMvsMTOne = " << use2DMvsMTOne_
              << " useRun2 = " << useRun2_
              << std::endl
              << " scale factor selection = " << selection.Data()
              << " set = " << set << std::endl;
  }

  if(useRun2_) years = {2016};

  //Processes
  std::map<int, std::string> names;
  names[kWJets] = "WJets";
  names[kZJets] = "ZJets";
  names[kTop  ] = "Top"  ;
  names[kQCD  ] = "QCD"  ;

  const TString cmssw = gSystem->Getenv("CMSSW_BASE");
  const TString path = (cmssw == "") ? "../scale_factors" : cmssw + "/src/CLFVAnalysis/scale_factors";
  for(int year : years) {
    const char* fname = Form("%s/jet_to_tau_comp_%s_%i_%i_%s.root", path.Data(), selection.Data(), set+7, set, (useRun2_) ? "2016_2017_2018" : Form("%i", year));
    if(verbose_ > 1) printf("%s: Initializing %i scale factors with file %s\n", __func__, year, fname);
    //get the jet --> tau compositions
    f = TFile::Open(fname, "READ");
    if(f) {
      if(verbose_ > 1) printf(" --> Opened file, retrieving compositions\n");
      for(int proc = 0; proc < kLast; ++proc) {
        for(int idm = -1; idm < (4*useDM_); ++idm) {
          TString nm = (idm > -1) ? Form("hComposition_%s_%i_%s", name.Data(), idm, names[proc].c_str()) : Form("hComposition_%s_%s", name.Data(), names[proc].c_str());
          if(verbose_ > 1) printf(" Retrieving composition %s\n", nm.Data());
          //Get Data histogram
          TH1* h = (TH1*) f->Get(nm.Data());
          if(!h) {
            std::cout << __func__ << ": Warning! No " <<  nm.Data() << " Data histogram found for year = " << year
                      << " idm = " << idm
                      << " Mode = " << Mode << " selection = " << selection.Data() << std::endl;
          } else {
            if(verbose_ > 1) printf(" --> Retrieved the composition\n");
            h->SetName(Form("%s_%s_%i_%i", h->GetName(), selection.Data(), idm+1, year));
            h->SetDirectory(0);
            if(use2DMvsMTOne_) {
              hists2DData_[year][idm+1][proc] = (TH2*) h;
            } else {
              histsData_[year][idm+1][proc] = h;
            }
          }
        }
      }
      f->Close();
      delete f;
    }
  }
}

//-------------------------------------------------------------------------------------------------------------------------
JetToTauComposition::~JetToTauComposition() {
  //std::map<int, std::map<int, std::map<int, TH1D*>>> histsData_;
  for(std::pair<int, std::map<int, std::map<int, TH1*>>> val_1 : histsData_) {
    for(std::pair<int, std::map<int, TH1*>> val_2 : val_1.second) {
      for(std::pair<int, TH1*> val_3 : val_2.second) {if(val_3.second) delete val_3.second;}
    }
  }
  for(std::pair<int, std::map<int, std::map<int, TH2*>>> val_1 : hists2DData_) {
    for(std::pair<int, std::map<int, TH2*>> val_2 : val_1.second) {
      for(std::pair<int, TH2*> val_3 : val_2.second) {if(val_3.second) delete val_3.second;}
    }
  }
}

//-------------------------------------------------------------------------------------------------------------------------
//Get fraction of the each background process contributing to the j->tau background
void JetToTauComposition::GetComposition(float pt, float dphi, float mt, const int DM,
                                         float pt_lead, float lead_dphi, float lead_mt, float mass,
                                         int year, float*& compositions, float*& comp_up, float*& comp_down) {

  //Run 2 inclusive fractions use 2016 slot
  if(useRun2_) year = 2016;

  //enforce allowed ranges
  if(pt < 20.f ) pt = 20.f;
  if(pt > 199.f) pt = 199.f;
  if(pt_lead < 20.f ) pt_lead = 20.f;
  if(pt_lead > 199.f) pt_lead = 199.f;
  mass = std::min(169.f, std::max(41.f, mass));
  lead_dphi = std::fabs(lead_dphi);
  if(lead_dphi < 0.) lead_dphi = std::fabs(lead_dphi);
  if(lead_dphi > M_PI) lead_dphi = std::fabs(2.*M_PI - lead_dphi);
  int idm = (!useDM_) ? 0 : 1 + 2*(DM/10) + DM%10; //DM inclusive->0, DM 0->1, DM 1->2, DM 10->3, DM 11->4
  if(idm > 4 || idm < 0) {
    std::cout << "!!! Error! DM mapped to undefined index in JetToTauComposition::" << __func__ << ": idm = " << idm << " DM = " << DM;
    idm = std::min(4, std::max(0, idm));
    std::cout << " --> Defaulting to DM index " << idm << std::endl;
  }
  float tot = 0.f;
  float var = pt;
  if(useOnePt_)        var = pt_lead;
  else if(useOneDPhi_) var = lead_dphi;
  else if(useMTOne_)   var = lead_mt;
  else if(useTwoDPhi_) var = dphi;
  else if(useMTTwo_)   var = mt;
  if(verbose_ > 1)
    std::cout << __func__ << ": Composition fractions for"
              << " tau pt = " << pt << " tau met dphi = " << dphi << " tau MT = " << mt
              << " lead pt = " << pt_lead << " lead met dPhi = " << lead_dphi << " lead MT = " << lead_mt
              << " mass = " << mass
              << " year = " << year << " DM = " << DM
              << std::endl;

  float tot_neg  = 0.f; //negative composition
  float tot_up   = 0.f;
  float tot_down = 0.f;
  const float mc_yield_sys = 0.10; //yield uncertainty on MC j-->tau FIXME: Decide on a reasonable value
  for(int proc = 0; proc < kLast; ++proc) {
    float comp(0.f), up(0.f), down(0.f);
    if(use2DMvsMTOne_) { // 2D composition in (mass, mtone)
      auto h = hists2DData_[year][idm][proc];
      if(!h) {
        std::cout << "!!! " << __func__ << ": No histogram found for year = " << year << " DM = " << DM << " and process = " << proc << std::endl;
      } else {
        const int xbin = std::max(1, std::min(h->GetNbinsX(), h->GetXaxis()->FindBin(mass)));
        const int ybin = std::max(1, std::min(h->GetNbinsY(), h->GetYaxis()->FindBin(lead_mt)));
        comp = h->GetBinContent(xbin,ybin);
      }
    } else { //1D compositions
      auto h = histsData_[year][idm][proc];
      if(!h) {
        std::cout << "!!! " << __func__ << ": No histogram found for year = " << year << " DM = " << DM << " and process = " << proc << std::endl;
      } else {
        const int bin = std::max(1, std::min(h->GetNbinsX(), h->FindBin(var)));
        comp = h->GetBinContent(bin);
      }
    }
    //pQCD = (Data - MC) / Data = 1 - pMC, pMC = MC / Data
    //pMC+ = (1+sys)*pMC = x*pMC
    //pQCD+ = 1 - pMC+ = 1 - x*pMC = 1 - x*(1 - pQCD) = x*pQCD + (1-x) = (1+sys)*pQCD - sys
    up   = std::max(0.f, (proc == kQCD) ?( 1.f + mc_yield_sys)*comp - mc_yield_sys : (1.f + mc_yield_sys)*comp); //std::max(0., std::min(1., comp + h->GetBinError(bin)));
    down = std::max(0.f, (proc == kQCD) ? (1.f - mc_yield_sys)*comp + mc_yield_sys : (1.f - mc_yield_sys)*comp); //std::max(0., std::min(1., comp - h->GetBinError(bin)));
    tot_up += up;
    tot_down += down;
    if(verbose_ > 1) std::cout << __func__ << ": Composition fraction for process " << proc << " = " << comp << std::endl;
    if(comp < 0.f) tot_neg += std::fabs(comp);
    if(/*comp > 1.f ||*/ comp < 0.f) {
      std::cout << "!!! " << __func__ << ": Composition fraction out of bounds, comp = " << comp
                << " tau pt = " << pt << " tau met dphi = " << dphi << " tau MT = " << mt
                << " lead pt = " << pt_lead << " lead met dPhi = " << lead_dphi << " lead MT = " << lead_mt
                << " year = " << year << " DM = " << DM
                << std::endl;
      comp = std::max(0.f, std::min(comp, 1.f));
    }
    compositions[proc] = comp;
    comp_up     [proc] = up;
    comp_down   [proc] = down;
    tot += comp;
    if(verbose_ > 1) std::cout << __func__ << ": Composition total so far = " << tot << std::endl;
  }
  //if there was a negative contribution, re-scale the others to be in 0-1
  if(tot_neg > 0.f) {
    const float scale = 1.f / (1.f + tot_neg); //set the sum of the non-negative fractions to 1
    if(verbose_ > 1) std::cout << __func__ << ": Applying overall scale " << scale << " to negative negative compositions of " << tot_neg << std::endl;
    for(int proc = 0; proc < kLast; ++proc) {
      compositions[proc] *= scale;
      comp_up     [proc] *= scale;
      comp_down   [proc] *= scale;
    }
    tot                  *= scale;
  }
  //ensure up/down add up to 1
  if(tot_up > 0.f && tot_down > 0.f) {
    for(int proc = 0; proc < kLast; ++proc) {
      comp_up  [proc] /= tot_up;
      comp_down[proc] /= tot_down;
    }
  } else {
    std::cout << "!!! " << __func__ << ": Total composition up/down out of tolerance, up = " << tot_up << " down = " << tot_down
              << "; tau pt = " << pt << " tau met dphi = " << dphi << " tau MT = " << mt
              << " lead pt = " << pt_lead << " lead met dPhi = " << lead_dphi << " lead MT = " << lead_mt
              << std::endl;
  }

  if(tot == 0.f) { //no composition values found, assume W+Jets
    compositions[kWJets] = 1.f;
    comp_up     [kWJets] = 1.f;
    comp_down   [kWJets] = 1.f;
    tot = 1.f;
  }
  if(std::fabs(tot - 1.f) > 1.e-4) {
    std::cout << "!!! " << __func__ << ": Total composition out of tolerance, total = " << tot
              << " tau pt = " << pt << " tau met dphi = " << dphi << " tau MT = " << mt
              << " lead pt = " << pt_lead << " lead met dPhi = " << lead_dphi << " lead MT = " << lead_mt
              << std::endl;
  }
}
