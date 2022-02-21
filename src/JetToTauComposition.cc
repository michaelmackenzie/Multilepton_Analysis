#include "interface/JetToTauComposition.hh"

using namespace CLFV;

//-------------------------------------------------------------------------------------------------------------------------
// Mode = 100*(use DM binned factors) + 10*(use fits) + (do QCD vs W+Jets in tau pT)
JetToTauComposition::JetToTauComposition(const TString selection, const int set, const int Mode, const int verbose) : Mode_(Mode), verbose_(verbose) {
  TFile* f = 0;
  std::vector<int> years = {2016, 2017, 2018};
  useOnePt_   = (Mode %   10) /   1 == 1; //composition in light lepton pT
  useOneDPhi_ = (Mode %   10) /   1 == 2; //composition in delta phi (light lep, MET)
  useFits_    = (Mode %  100) /  10 == 1; //use fits of the composition (Not Implemented)
  useDM_      = (Mode % 1000) / 100 == 1; //decay mode dependent compositions

  TString name            = "jettautwopt";
  if(useOnePt_)      name = "jettauonept";
  else if(useOneDPhi_) name = "jettauonemetdeltaphi";

  if(verbose_ > 0) {
    std::cout << __func__ << ":\n Mode = " << Mode
              << " useOnePt = " << useOnePt_
              << " useOneDPhi = " << useOneDPhi_
              << " useFits = " << useFits_
              << std::endl
              << " scale factor selection = " << selection.Data()
              << " set = " << set << std::endl;
  }

  const TString cmssw = gSystem->Getenv("CMSSW_BASE");
  const TString path = (cmssw == "") ? "../scale_factors" : cmssw + "/src/CLFVAnalysis/scale_factors";
  for(int year : years) {
    if(verbose_ > 1) printf("%s: Initializing %i scale factors\n", __func__, year);
    //get the jet --> tau compositions measured
    f = TFile::Open(Form("%s/jet_to_tau_comp_%s_%i_%i.root", path.Data(), selection.Data(), set, year), "READ");
    if(f) {
      std::map<int, std::string> names;
      names[kWJets] = "WJets";
      names[kZJets] = "ZJets";
      names[kTop  ] = "Top"  ;
      names[kQCD  ] = "QCD"  ;
      for(int proc = 0; proc < kLast; ++proc) {
        for(int idm = -1; idm < 4; ++idm) {
          TString nm = (idm > -1) ? Form("hComposition_%s_%i_%s", name.Data(), idm, names[proc].c_str()) : Form("hComposition_%s_%s", name.Data(), names[proc].c_str());
          //Get Data histogram
          histsData_[year][idm+1][proc] = (TH1D*) f->Get(nm.Data());
          if(!histsData_[year][idm+1][proc]) {
            std::cout << __func__ << ": Warning! No " <<  nm.Data() << " Data histogram found for year = " << year
                      << " idm = " << idm
                      << " Mode = " << Mode << " selection = " << selection.Data() << std::endl;
          } else {
            histsData_[year][idm+1][proc]->SetName(Form("%s_%s_%i_%i", histsData_[year][idm+1][proc]->GetName(), selection.Data(), idm+1, year));
            histsData_[year][idm+1][proc]->SetDirectory(0);
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
  for(std::pair<int, std::map<int, std::map<int, TH1D*>>> val_1 : histsData_) {
    for(std::pair<int, std::map<int, TH1D*>> val_2 : val_1.second) {
      for(std::pair<int, TH1D*> val_3 : val_2.second) {if(val_3.second) delete val_3.second;}
    }
  }
}

//-------------------------------------------------------------------------------------------------------------------------
//Get fraction of the each background process contributing to the j->tau background
void JetToTauComposition::GetComposition(float pt, const int DM, float pt_lead, float lead_dphi, const int year, float*& compositions, float*& comp_up, float*& comp_down) {
  //enforce allowed ranges
  if(pt < 20. ) pt = 20.f;
  if(pt > 199.) pt = 199.f;
  if(pt_lead < 20. ) pt_lead = 20.f;
  if(pt_lead > 199.) pt_lead = 199.f;
  lead_dphi = std::fabs(lead_dphi);
  if(lead_dphi < 0.) lead_dphi = std::fabs(lead_dphi);
  if(lead_dphi > M_PI) lead_dphi = std::fabs(2.*M_PI - lead_dphi);
  int idm = (!useDM_) ? 0 : 1 + 2*(DM/10) + DM%10; //DM inclusive->0, DM 0->1, DM 1->2, DM 10->3, DM 11->4
  if(idm > 4 || idm < 0) {
    std::cout << "!!! Error! DM mapped to undefined index in JetToTauComposition::" << __func__ << ": idm = " << idm << " DM = " << DM;
    idm = std::min(3, std::max(0, idm));
    std::cout << " --> Defaulting to DM index " << idm << std::endl;
  }
  float tot = 0.f;
  float var = pt;
  if(useOnePt_) var = pt_lead;
  else if(useOneDPhi_) var = lead_dphi;
  if(verbose_ > 1)
    std::cout << __func__ << ": Composition fractions for"
              << " pt = " << pt << " lead pt = " << pt_lead << " lead met dPhi = " << lead_dphi
              << " year = " << year << " DM = " << DM
              << std::endl;
  for(int proc = 0; proc < kLast; ++proc) {
    float comp(0.f), up(0.f), down(0.f);
    auto h = histsData_[year][idm][proc];
    if(!h) {
      std::cout << "!!! " << __func__ << ": No histogram found for year = " << year << " DM = " << DM << " and process = " << proc << std::endl;
    } else {
      comp = h->GetBinContent(h->FindBin(var));
      up   = comp + h->GetBinError(h->FindBin(var));
      down = comp - h->GetBinError(h->FindBin(var));
    }
    if(verbose_ > 1) std::cout << __func__ << ": Composition fraction for process " << proc << " = " << comp << std::endl;
    if(comp > 1.f || comp < 0.f) {
      std::cout << "!!! " << __func__ << ": Composition fraction out of bounds, comp = " << comp
                << " pt = " << pt << " lead pt = " << pt_lead << " lead met dPhi = " << lead_dphi
                << " year = " << year << " DM = " << DM
                << std::endl;
      comp = std::max(0.f, std::min(comp, 1.f));
    }
    up   = std::max(0.f, std::min(up  , 1.f));
    down = std::max(0.f, std::min(down, 1.f));
    compositions[proc] = comp;
    comp_up  [proc] = up;
    comp_down[proc] = down;
    tot += comp;
  }
  if(std::fabs(tot - 1.f) > 1.e-4) {
    std::cout << "!!! " << __func__ << ": Total composition out of tolerance, total = " << tot
              << " pt = " << pt << " lead pt = " << pt_lead << " lead met dphi = " << lead_dphi
              << " year = " << year << " DM = " << DM
              << std::endl;
  }
}
