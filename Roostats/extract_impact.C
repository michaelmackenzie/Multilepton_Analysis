//extract the uncertainty impact of a systematic
struct fiteffect_t {
  double r_nom_;
  double up_nom_;
  double down_nom_;
  double r_group_;
  double up_group_;
  double down_group_;
  double impact_up_;
  double impact_down_;
};

int extract_impact(const char* fnominal, const char* fgroup, fiteffect_t* effect = nullptr) {
  TFile* file_nominal = TFile::Open(fnominal, "READ");
  TFile* file_group   = TFile::Open(fgroup  , "READ");

  if(effect) {
    effect->r_nom_       = 0.;
    effect->up_nom_      = 0.;
    effect->down_nom_    = 0.;
    effect->r_group_     = 0.;
    effect->up_group_    = 0.;
    effect->down_group_  = 0.;
    effect->impact_up_   = 0.;
    effect->impact_down_ = 0.;
  }

  if(!file_nominal || !file_group) return 1;

  TTree* t_nominal = (TTree*) file_nominal->Get("tree_fit_sb");
  TTree* t_group   = (TTree*) file_group  ->Get("tree_fit_sb");

  if(!t_nominal || !t_group) {
    cout << "Trees not found!\n";
    return 2;
  }

  Double_t r_n, rLoErr_n, rHiErr_n;
  Double_t r_g, rLoErr_g, rHiErr_g;
  t_nominal->SetBranchAddress("r"     , &r_n     );
  t_nominal->SetBranchAddress("rLoErr", &rLoErr_n);
  t_nominal->SetBranchAddress("rHiErr", &rHiErr_n);
  t_group  ->SetBranchAddress("r"     , &r_g     );
  t_group  ->SetBranchAddress("rLoErr", &rLoErr_g);
  t_group  ->SetBranchAddress("rHiErr", &rHiErr_g);

  t_nominal->GetEntry(0); t_group->GetEntry(0);

  const double impact_down(sqrt(max(0., (rLoErr_n*rLoErr_n - rLoErr_g*rLoErr_g))));
  const double impact_up  (sqrt(max(0., (rHiErr_n*rHiErr_n - rHiErr_g*rHiErr_g))));

  printf("Param : Nominal Group\n");
  printf("r     : %.3f %.3f\n", r_n, r_g);
  printf("rLoErr: %.3f %.3f\n", rLoErr_n, rLoErr_g);
  printf("rHiErr: %.3f %.3f\n", rHiErr_n, rHiErr_g);
  printf("Impact: r-shift  -unc   +unc\n");
  printf("      : %.4f -%.4f +%.4f\n", r_g-r_n, impact_down, impact_up);

  file_nominal->Close();
  file_group->Close();

  if(effect) {
    effect->r_nom_       = r_n;
    effect->up_nom_      = rHiErr_n;
    effect->down_nom_    = rLoErr_n;
    effect->r_group_     = r_g;
    effect->up_group_    = rHiErr_g;
    effect->down_group_  = rLoErr_g;
    effect->impact_up_   = impact_up;
    effect->impact_down_ = impact_down;
  }

  return 0;
}
