//extract the uncertainty impact of a systematic

double extract_impact(const char* fnominal, const char* fgroup) {
  TFile* file_nominal = TFile::Open(fnominal, "READ");
  TFile* file_group   = TFile::Open(fgroup  , "READ");

  if(!file_nominal || !file_group) return 0.;

  TTree* t_nominal = (TTree*) file_nominal->Get("tree_fit_sb");
  TTree* t_group   = (TTree*) file_group  ->Get("tree_fit_sb");

  if(!t_nominal || !t_group) {
    cout << "Trees not found!\n";
    return 0.;
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

  printf("Param : Nominal Group\n");
  printf("r     : %.3f %.3f\n", r_n, r_g);
  printf("rLoErr: %.3f %.3f\n", rLoErr_n, rLoErr_g);
  printf("rHiErr: %.3f %.3f\n", rHiErr_n, rHiErr_g);
  printf("Impact: r-shift  -unc   +unc\n");
  printf("      : %.4f -%.4f +%.4f\n", r_g-r_n,
         sqrt(max(0., (rLoErr_n*rLoErr_n - rLoErr_g*rLoErr_g))),
         sqrt(max(0., (rHiErr_n*rHiErr_n - rHiErr_g*rHiErr_g))));
  return r_g - r_n;
  return 0.;
}
