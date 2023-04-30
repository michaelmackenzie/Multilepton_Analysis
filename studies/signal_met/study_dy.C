//Study the MET and true neutrino distributions for the Drell-Yan tau-tau background
int verbose_ = 0; //10;

void study_dy(const bool hadronic = true, const int max_entries = -1) {

  if(verbose_ > 9 && max_entries < 0) {
    cout << "Too high of verbosity!\n";
    return;
  }
  TString file_name = "LFVAnalysis_DY50-amc_2016.root";

  TChain chain((hadronic) ? "mutau" : "emu");
  chain.Add(Form("root://cmseos.fnal.gov//store/user/mmackenz/lfvanalysis_rootfiles/%s", file_name.Data()));
  if(chain.GetNtrees() == 0) {
    cout << "No datasets found!\n";
    return;
  }


  TH1* hmet        = new TH1D("hmet"  , "Reco MET"       , 50, 0., 100.);
  TH1* hnupt       = new TH1D("hnupt" , "Gen #nu p_{T}"  , 50, 0., 100.);
  TH1* htaupt      = new TH1D("htaupt", "Gen #tau p_{T}" , 50, 0., 100.);
  TH1* hgenptratio = new TH1D("hgenptratio", "Gen lepton p_{T} ratio" , 50, 0., 3.);
  TH1* hvisptratio = new TH1D("hvisptratio", "Visible lepton p_{T} ratio" , 50, 0., 3.);
  TH1* hcolpt      = new TH1D("hcolpt", "Collinear p_{T}", 50, 0., 100.);
  TH1* hbalpt      = new TH1D("hbalpt", "Balanced p_{T}" , 50, 0., 100.);
  TH1* hmetnudiff  = new TH1D("hmetnudiff", "MET - #nu p_{T}", 50, -50., 50.);
  TH1* hcolnudiff  = new TH1D("hcolnudiff", "Collinear - #nu p_{T}", 50, -50., 50.);
  TH1* hbalnudiff  = new TH1D("hbalnudiff", "Balanced - #nu p_{T}", 50, -50., 50.);
  TH1* hnudphi     = new TH1D("hnudphi" , "|#Delta#phi(#nu,#tau_{vis})|", 25, 0., 3.2);
  TH1* hnudeta     = new TH1D("hnudeta" , "|#Delta#eta(#nu,#tau_{vis})|", 25, 0., 3.2);
  TH2* hnudphiveta = new TH2D("hnudphiveta" , "|#Delta#phi| vs |#Delta#eta| (#nu,#tau_{vis})", 20, 0., 0.8, 20, 0., 0.8);

  TH1* hgenalpha      = new TH1D("hgenalpha" , "Gen #alpha", 25, 0., 2.);
  TH1* hgenbeta       = new TH1D("hgenbeta"  , "Gen #beta" , 25, 0., 2.);
  TH1* hgenbetar      = new TH1D("hgenbetar" , "Gen #alpha/#beta", 25, 0., 3.);
  TH2* hgenalphavbeta = new TH2D("hgenalphavbeta", "Gen #alpha_{1} vs #alpha_{2}", 20, 0, 1.2, 20, 0, 1.2);
  TH1* hrecalpha      = new TH1D("hrecalpha" , "Rec #alpha", 25, 0., 2.);
  TH1* hrecbeta       = new TH1D("hrecbeta"  , "Rec #beta" , 25, 0., 2.);
  TH1* hrecbetar      = new TH1D("hrecbetar" , "Rec #alpha/#beta", 25, 0., 3.);
  TH2* hrecalphavbeta = new TH2D("hrecalphavbeta", "Rec #alpha_{1} vs #alpha_{2}", 20, 0, 1.2, 20, 0, 1.2);
  TH1* hlepdot        = new TH1D("hlepdot" , "#sqrt{l_{1}#cdotl_{2}", 50, 30., 90.);

  TH1* hmass       = new TH1D("hmass"   , "Gen M(l,#tau)", 100., 50., 120.);
  TH1* hvismass    = new TH1D("hvismass", "M(l,#tau_{vis})", 100., 50., 120.);
  TH1* hcolmass    = new TH1D("hcolmass", "M(l,#tau_{col})", 100., 50., 120.);
  TH1* hmetmass    = new TH1D("hmetmass", "M(l,#tau_{met})", 100., 50., 120.);
  TH1* hbalmass    = new TH1D("hbalmass", "M(l,#tau_{bal})", 100., 50., 120.);
  TH1* hcolmdiff   = new TH1D("hcolmdiff", "M(l,#tau_{col}) - M(l,#tau)", 100., -50., 50.);
  TH1* hmetmdiff   = new TH1D("hmetmdiff", "M(l,#tau_{met}) - M(l,#tau)", 100., -50., 50.);
  TH1* hbalmdiff   = new TH1D("hbalmdiff", "M(l,#tau_{bal}) - M(l,#tau)", 100., -50., 50.);

  TH1* hcoltaum    = new TH1D("hcoltaum"  , "M(#tau_{col})", 100., 0., 10.);
  TH2* hcolmdiffvphi = new TH2D("hcolmdiffvphi" , "#DeltaM(coll) vs |#Delta#phi(MET)| ", 30, -30., 30., 20, 0., M_PI);

  hmet       ->SetLineWidth(2);
  hnupt      ->SetLineWidth(2);
  htaupt     ->SetLineWidth(2);
  hgenptratio->SetLineWidth(2);
  hvisptratio->SetLineWidth(2);

  hgenalpha  ->SetLineWidth(2);
  hgenbeta   ->SetLineWidth(2);
  hgenbetar  ->SetLineWidth(2);
  hrecalpha  ->SetLineWidth(2);
  hrecbeta   ->SetLineWidth(2);
  hrecbetar  ->SetLineWidth(2);
  hlepdot    ->SetLineWidth(2);

  hcolpt     ->SetLineWidth(2);
  hbalpt     ->SetLineWidth(2);
  hmetnudiff ->SetLineWidth(2);
  hcolnudiff ->SetLineWidth(2);
  hbalnudiff ->SetLineWidth(2);
  hnudphi    ->SetLineWidth(2);
  hnudeta    ->SetLineWidth(2);
  hmass      ->SetLineWidth(2);
  hvismass   ->SetLineWidth(2);
  hcolmass   ->SetLineWidth(2);
  hmetmass   ->SetLineWidth(2);
  hbalmass   ->SetLineWidth(2);
  hcolmdiff  ->SetLineWidth(2);
  hmetmdiff  ->SetLineWidth(2);
  hbalmdiff  ->SetLineWidth(2);
  hcoltaum   ->SetLineWidth(2);

  const int nmax = 500;
  UInt_t nGenPart;
  Float_t GenPart_pt[nmax], GenPart_eta[nmax], GenPart_phi[nmax], GenPart_mass[nmax];
  Int_t GenPart_pdgId[nmax], GenPart_statusFlags[nmax], GenPart_mother[nmax];
  Float_t MET, METPhi;

  chain.SetBranchAddress("nGenPart"                , &nGenPart          );
  chain.SetBranchAddress("GenPart_pt"              , GenPart_pt         );
  chain.SetBranchAddress("GenPart_eta"             , GenPart_eta        );
  chain.SetBranchAddress("GenPart_phi"             , GenPart_phi        );
  chain.SetBranchAddress("GenPart_mass"            , GenPart_mass       );
  chain.SetBranchAddress("GenPart_pdgId"           , GenPart_pdgId      );
  chain.SetBranchAddress("GenPart_statusFlags"     , GenPart_statusFlags);
  chain.SetBranchAddress("GenPart_genPartIdxMother", GenPart_mother     );
  chain.SetBranchAddress("PuppiMET_pt"             , &MET               );
  chain.SetBranchAddress("PuppiMET_phi"            , &METPhi            );

  const Long64_t nentries = (max_entries < 0 || chain.GetEntries() < max_entries) ? chain.GetEntries() : max_entries;
  cout << "Processing " << nentries << " entries...\n";

  TLorentzVector ptau_1, ptau_2, pvis_1, pvis_2, pnu_1, pnu_2, pmet, pcol, pbal;
  int idx_z, idx_tau_1, idx_tau_2;
  for(Long64_t entry = 0; entry < nentries; ++entry) {
    if(entry % 10000 == 0 || verbose_ > 9) printf("Processing entry %6lld...\n", entry);
    chain.GetEntry(entry);
    if(verbose_ > 9) printf("ID :   pdg    pt      eta     phi    M   mother Last First Prompt Hard\n");
    if(verbose_ > 9) printf("----------------------------------------------------------------------\n");
    idx_z = -1; idx_tau_1 = -1; idx_tau_2 = -1;
    pnu_1.SetPxPyPzE(0., 0., 0., 0.);
    pnu_2.SetPxPyPzE(0., 0., 0., 0.);
    pvis_1.SetPxPyPzE(0., 0., 0., 0.);
    pvis_2.SetPxPyPzE(0., 0., 0., 0.);
    bool tau_1_lep(false), tau_2_lep(false);
    for(UInt_t idx = 0; idx < nGenPart; ++idx) {
      const bool isfirst    = (GenPart_statusFlags[idx] & 1<<12) != 0;
      const bool islast     = (GenPart_statusFlags[idx] & 1<<13) != 0;
      const bool prompt     = (GenPart_statusFlags[idx] & 1<< 0) != 0;
      const bool hard       = (GenPart_statusFlags[idx] & 1<< 7) != 0;
      const int  idx_mother = GenPart_mother[idx];
      const int  id_mother  = (idx_mother < 0) ? 0 : std::abs(GenPart_pdgId[idx_mother]);
      const int  pdg        = std::abs(GenPart_pdgId[idx]);
      if(verbose_ > 9) {
        printf("%3i: %5i %5.1f %9.2f %6.2f  %5.2f  %3i     %o    %o     %o      %o\n", idx, GenPart_pdgId[idx], GenPart_pt[idx],
               GenPart_eta[idx], GenPart_phi[idx], GenPart_mass[idx], idx_mother,
               islast, isfirst, prompt, hard);
      }
      if(GenPart_pdgId[idx] == 23 && islast) { //Z boson, last instance
        idx_z = idx;
      } else if(pdg == 15 && islast && prompt && idx_tau_1 < 0) { //Z->tau tau lepton
        idx_tau_1 = idx;
      } else if(pdg == 15 && islast && prompt) { //Z->tau tau other lepton
        idx_tau_2 = idx;
      } else if((pdg == 12 || pdg == 14 || pdg == 16) && idx_mother >= 0 && idx_mother == idx_tau_1) { //prompt tau(1) decay product
        TLorentzVector lv; lv.SetPtEtaPhiM(GenPart_pt[idx], GenPart_eta[idx], GenPart_phi[idx], 0.);
        pnu_1 += lv;
      } else if((pdg == 12 || pdg == 14 || pdg == 16) && idx_mother >= 0 && idx_mother == idx_tau_2) { //prompt tau(2) decay product
        TLorentzVector lv; lv.SetPtEtaPhiM(GenPart_pt[idx], GenPart_eta[idx], GenPart_phi[idx], 0.);
        pnu_2 += lv;
      } else if(idx_mother >= 0 && idx_mother == idx_tau_1) { //non-neutrino prompt tau(1) decay product
        TLorentzVector lv; lv.SetPtEtaPhiM(GenPart_pt[idx], GenPart_eta[idx], GenPart_phi[idx], GenPart_mass[idx]);
        pvis_1 += lv;
        tau_1_lep |= pdg == 11 || pdg == 13; //record if leptonic tau
      } else if(idx_mother >= 0 && idx_mother == idx_tau_2) { //non-neutrino prompt tau(2) decay product
        TLorentzVector lv; lv.SetPtEtaPhiM(GenPart_pt[idx], GenPart_eta[idx], GenPart_phi[idx], GenPart_mass[idx]);
        pvis_2 += lv;
        tau_2_lep |= pdg == 11 || pdg == 13; //record if leptonic tau
      }
    }
    if(idx_tau_1 < 0 || idx_tau_2 < 0) {
      // printf("Entry %lld: Z decay products not found!\n", entry);
      continue;
    }
    if(hadronic && !((tau_1_lep && !tau_2_lep) || (!tau_1_lep && tau_2_lep))) continue; // not a tau_h tau_l
    else if(!hadronic && (!tau_1_lep  || !tau_2_lep)) continue; //not a tau_l tau_l

    ptau_1.SetPtEtaPhiM(GenPart_pt[idx_tau_1], GenPart_eta[idx_tau_1], GenPart_phi[idx_tau_1], GenPart_mass[idx_tau_1]);
    ptau_2.SetPtEtaPhiM(GenPart_pt[idx_tau_2], GenPart_eta[idx_tau_2], GenPart_phi[idx_tau_2], GenPart_mass[idx_tau_2]);

    //FIXME: decide for leptonic case
    TLorentzVector& pvis     = (hadronic && tau_2_lep) ? pvis_1 : pvis_2;
    TLorentzVector& plep     = (hadronic && tau_2_lep) ? pvis_2 : pvis_1;
    TLorentzVector& ptau_vis = (hadronic && tau_2_lep) ? ptau_1 : ptau_2;
    TLorentzVector& ptau_lep = (hadronic && tau_2_lep) ? ptau_2 : ptau_1;

    pmet.SetPtEtaPhiM(MET, pvis.Eta(), METPhi, 0.);
    TVector3 ptau_dir =  pvis.Vect();
    ptau_dir.SetZ(0.); ptau_dir.SetMag(1.);
    TVector3 met = pmet.Vect(); met.SetZ(0.);
    const float pnu_col = met*ptau_dir;
    pcol.SetPtEtaPhiM(std::fabs(pnu_col), pvis.Eta()*((pnu_col < 0.) ? -1. : 1.), (pnu_col < 0.) ? M_PI + pvis.Phi() : pvis.Phi(), 0.);
    //neutrino momentum assuming collimated with tau and pT(Z) = 0
    pbal.SetPtEtaPhiM(std::fabs(plep.Pt() - pvis.Pt()), pvis.Eta()*((pvis.Pt() > plep.Pt()) ? -1. : 1.), (pvis.Pt() > plep.Pt()) ? -pvis.Phi() : pvis.Phi(), 0.);

    TLorentzVector pnu = pnu_1 + pnu_2;

    if(verbose_ > 9) {
      printf("\nObj:    pt   eta   phi    M\n");
      printf(  "------------------------------\n");
      printf("Tau: %5.1f %5.2f %5.2f %6.2f\n", ptau_1.Pt(), ptau_1.Eta(), ptau_1.Phi(), ptau_1.M());
      printf("Tau: %5.1f %5.2f %5.2f %6.2f\n", ptau_2.Pt(), ptau_2.Eta(), ptau_2.Phi(), ptau_2.M());
      printf("Lep: %5.1f %5.2f %5.2f %6.2f\n", plep.Pt(), plep.Eta(), plep.Phi(), plep.M());
      printf("Vis: %5.1f %5.2f %5.2f %6.2f\n", pvis.Pt(), pvis.Eta(), pvis.Phi(), pvis.M());
      printf("Nu1: %5.1f %5.2f %5.2f %6.2f\n", pnu_1.Pt(), pnu_1.Eta(), pnu_1.Phi(), pnu_1.M());
      printf("Nu2: %5.1f %5.2f %5.2f %6.2f\n", pnu_2.Pt(), pnu_2.Eta(), pnu_2.Phi(), pnu_2.M());
      printf("Nu : %5.1f %5.2f %5.2f %6.2f\n", pnu .Pt(), pnu .Eta(), pnu .Phi(), pnu .M());
      printf("MET: %5.1f %5.2f %5.2f %6.2f\n", MET, 0., METPhi, 0.);
      printf("Col: %5.1f %5.2f %5.2f %6.2f\n", pcol.Pt(), pcol.Eta(), pcol.Phi(), pcol.M());
      printf("Bal: %5.1f %5.2f %5.2f %6.2f\n", pbal.Pt(), pbal.Eta(), pbal.Phi(), pbal.M());
    }
    const float mcoll = (pvis+plep+pcol).M();
    if(!std::isfinite(mcoll) || mcoll < 0.) printf("Entry %lld: Undefined mcoll = %f\n", entry, mcoll);

    const float gen_alpha = pvis.Pt() / ptau_vis.Pt();
    const float gen_beta  = plep.Pt() / ptau_lep.Pt();
    const float lep_dot   = sqrt(2.*pvis*plep);
    const float rec_alpha = 1./((91.2/lep_dot)/sqrt(pvis.Pt()/plep.Pt()));
    const float rec_beta  = 1./((91.2/lep_dot)/sqrt(plep.Pt()/pvis.Pt()));
    hgenalpha->Fill(gen_alpha);
    hgenbeta->Fill(gen_beta);
    hgenbetar->Fill(gen_alpha/gen_beta);
    hgenalphavbeta->Fill(gen_alpha, gen_beta);
    hrecalpha->Fill(rec_alpha);
    hrecbeta->Fill(rec_beta);
    hrecbetar->Fill(rec_alpha/rec_beta);
    hrecalphavbeta->Fill(rec_alpha, rec_beta);
    hlepdot->Fill(lep_dot);

    hmet->Fill(MET);
    hnupt->Fill(pnu.Pt());
    htaupt->Fill(ptau_1.Pt());
    hgenptratio->Fill(ptau_1.Pt()/ptau_2.Pt());
    hvisptratio->Fill(pvis.Pt()/plep.Pt());
    hcolpt->Fill(pcol.Pt());
    hbalpt->Fill(pbal.Pt());
    hmetnudiff->Fill(MET - pnu.Pt());
    hcolnudiff->Fill(pcol.Pt() - pnu.Pt());
    hbalnudiff->Fill(pbal.Pt() - pnu.Pt());
    hnudphi->Fill(std::fabs(pnu.DeltaPhi(pvis)));
    hnudeta->Fill(std::fabs(pnu.Eta() - pvis.Eta()));
    hnudphiveta->Fill(std::fabs(pnu.DeltaPhi(pvis)), std::fabs(pnu.Eta() - pvis.Eta()));
    hcoltaum->Fill((pvis+pcol).M());
    const float mass = (ptau_1+ptau_2).M();
    hmass->Fill(mass);
    hvismass->Fill((pvis+plep).M());
    hcolmass->Fill((pvis+plep+pcol).M());
    hbalmass->Fill((pvis+plep+pbal).M());
    hmetmass->Fill((pvis+plep+pmet).M());
    hcolmdiff->Fill((pvis+plep+pcol).M() - mass);
    hcolmdiffvphi->Fill((pvis+plep+pcol).M() - mass, std::fabs(pvis.DeltaPhi(pmet)));
    hmetmdiff->Fill((pvis+plep+pmet).M() - mass);
    hbalmdiff->Fill((pvis+plep+pbal).M() - mass);
  }

  ///////////////////////////////////////////////
  //Plot the results

  gSystem->Exec("[ ! -d figures/dy ] && mkdir figures/dy");

  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas();

  hmet       ->Draw("hist"); c->SaveAs("figures/dy/met.png");
  hnupt      ->Draw("hist"); c->SaveAs("figures/dy/nupt.png");
  htaupt     ->Draw("hist"); c->SaveAs("figures/dy/taupt.png");
  hgenptratio->Draw("hist"); c->SaveAs("figures/dy/gen_pt_ratio.png");
  hvisptratio->Draw("hist"); c->SaveAs("figures/dy/vis_pt_ratio.png");

  hgenalpha     ->Draw("hist"); c->SaveAs("figures/dy/alpha_gen.png");
  hgenbeta      ->Draw("hist"); c->SaveAs("figures/dy/beta_gen.png");
  hgenbetar     ->Draw("hist"); c->SaveAs("figures/dy/alpha_over_beta_gen.png");
  hgenalphavbeta->Draw("colz"); c->SaveAs("figures/dy/alpha_v_beta_gen.png");
  hrecalpha     ->Draw("hist"); c->SaveAs("figures/dy/alpha_rec.png");
  hrecbeta      ->Draw("hist"); c->SaveAs("figures/dy/beta_rec.png");
  hrecbetar     ->Draw("hist"); c->SaveAs("figures/dy/alpha_over_beta_rec.png");
  hrecalphavbeta->Draw("colz"); c->SaveAs("figures/dy/alpha_v_beta_rec.png");
  hlepdot       ->Draw("hist"); c->SaveAs("figures/dy/lep_dot.png");

  hcolpt->Draw("hist");
  c->SaveAs("figures/dy/colpt.png");
  hbalpt->Draw("hist");
  c->SaveAs("figures/dy/bal_pt.png");
  hmetnudiff->Draw("hist");
  c->SaveAs("figures/dy/met_nu_diff.png");
  hcolnudiff->Draw("hist");
  c->SaveAs("figures/dy/col_nu_diff.png");
  hbalnudiff->Draw("hist");
  c->SaveAs("figures/dy/bal_nu_diff.png");
  hnudphi->Draw("hist");
  c->SaveAs("figures/dy/nudphi.png");
  hnudeta->Draw("hist");
  c->SaveAs("figures/dy/nudeta.png");
  hnudphiveta->Draw("colz");
  c->SaveAs("figures/dy/nu_dphi_vs_deta.png");
  hcoltaum->Draw("hist");
  c->SaveAs("figures/dy/col_tau_mass.png");
  hcolmdiffvphi->Draw("colz");
  c->SaveAs("figures/dy/mass_col_diff_v_dphi.png");
  hmass->Draw("hist");
  c->SaveAs("figures/dy/mass.png");
  hvismass->Draw("hist");
  c->SaveAs("figures/dy/mass_vis.png");
  hcolmass->Draw("hist");
  c->SaveAs("figures/dy/mass_col.png");
  hbalmass->Draw("hist");
  c->SaveAs("figures/dy/mass_bal.png");
  hmetmass->Draw("hist");
  c->SaveAs("figures/dy/mass_met.png");
  hcolmdiff->Draw("hist");
  c->SaveAs("figures/dy/mass_col_diff.png");
  hbalmdiff->Draw("hist");
  c->SaveAs("figures/dy/mass_bal_diff.png");
  hmetmdiff->Draw("hist");
  c->SaveAs("figures/dy/mass_met_diff.png");

  gStyle->SetOptStat(0);
  hmass->Draw("hist");
  hvismass->SetLineColor(kViolet+6);
  hcolmass->SetLineColor(kRed);
  hmetmass->SetLineColor(kGreen-7);
  hbalmass->SetLineColor(kYellow-3);
  hvismass->Draw("same hist");
  hcolmass->Draw("same hist");
  hmetmass->Draw("same hist");
  hbalmass->Draw("same hist");
  hmass->SetTitle("Mass estimates");
  TLegend* leg = new TLegend();
  leg->AddEntry(hmass, "True mass");
  leg->AddEntry(hvismass, "Visible mass");
  leg->AddEntry(hcolmass, "Projected mass");
  leg->AddEntry(hmetmass, "MET mass");
  leg->AddEntry(hbalmass, "Balanced mass");
  leg->Draw();
  c->SaveAs("figures/dy/mass_comparison.png");
}
