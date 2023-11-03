//Study the MET and true neutrino distributions for the signal
int verbose_ = 0; //10;

void study(TString signal = "zmutau", const int max_entries = -1) {
  signal.ToLower();
  TString file_name = "LFVAnalysis_";
  if     (signal.BeginsWith("z"))   file_name += "Z";
  else if(signal.BeginsWith("h"))   file_name += "H";
  if     (signal.Contains("mutau")) file_name += "MuTau";
  else if(signal.Contains("etau" )) file_name += "ETau" ;
  else if(signal.Contains("emu"  )) file_name += "EMu"  ;
  else if(signal == "DY")           file_name += "DY50-amc_2016.root";
  else return;
  if(signal.BeginsWith("z")) file_name += "-v2_201*.root"; //use high statistics samples

  TString selection = (signal.EndsWith("emu") || signal.Contains("_")) ? "emu" : (signal.EndsWith("mutau")) ? "mutau" : "etau";
  TChain chain(selection.Data());
  chain.Add(Form("root://cmseos.fnal.gov//store/user/mmackenz/lfvanalysis_rootfiles/%s", file_name.Data()));
  if(chain.GetNtrees() == 0) {
    cout << "No datasets found!\n";
    return;
  }


  TH1* hmet        = new TH1D("hmet"  , "Reco MET"       , 50, -50., 100.);
  TH1* hgenmet     = new TH1D("hgenmet", "Gen MET"       , 50, -50., 100.);
  TH1* hnupt       = new TH1D("hnupt" , "Gen #nu p_{T}"  , 50, -50., 100.);
  TH1* htaupt      = new TH1D("htaupt", "Gen #tau p_{T}" , 50, 0., 100.);
  TH1* hgenptratio = new TH1D("hgenptratio", "Gen lepton p_{T} ratio" , 50, 0., 3.);
  TH1* hvisptratio = new TH1D("hvisptratio", "Visible lepton p_{T} ratio" , 50, 0., 3.);
  TH1* hcolpt      = new TH1D("hcolpt", "Collinear p_{T}", 50, -50., 100.);
  TH1* hbalpt      = new TH1D("hbalpt", "Balanced p_{T}" , 50, -50., 100.);
  TH1* hcombpt     = new TH1D("hcombpt", "Combined p_{T}", 50, -50., 100.);
  TH1* hmetnudiff  = new TH1D("hmetnudiff", "MET - #nu p_{T}", 50, -50., 50.);
  TH1* hgenmetnudiff = new TH1D("hgenmetnudiff", "Gen MET - #nu p_{T}", 50, -5., 5.);
  TH1* hgenmetdiff = new TH1D("hgenmetdiff", "MET - Gen MET", 50, -50., 50.);
  TH1* hcolnudiff  = new TH1D("hcolnudiff", "Collinear - #nu p_{T}", 50, -50., 50.);
  TH1* hbalnudiff  = new TH1D("hbalnudiff", "Balanced - #nu p_{T}", 50, -50., 50.);
  TH1* hcolnusize  = new TH1D("hcolnusize", "|#Delta#nu p|", 25, 0., 50.);
  TH1* hnudphi     = new TH1D("hnudphi" , "|#Delta#phi(#nu,#tau_{vis})|", 25, 0., 3.2);
  TH1* hnudeta     = new TH1D("hnudeta" , "|#Delta#eta(#nu,#tau_{vis})|", 25, 0., 3.2);
  TH2* hnudphiveta = new TH2D("hnudphiveta" , "|#Delta#phi| vs |#Delta#eta| (#nu,#tau_{vis})", 20, 0., 0.8, 20, 0., 0.8);

  TH1* hdphi       = new TH1D("hdphi", "Gen |#Delta#phi(l,l)|", 20, 0., 3.2);
  TH1* hptratiovdphi = new TH2D("hptratiovdphi", "Gen pT ratio vs. |#Delta#phi(l,l)|", 20., 0.5, 1.5, 20, 2, 3.2);

  TH1* hzpt        = new TH1D("hzpt", "Gen p_{T}^{ll}", 50, 0., 100.);
  TH1* hviszpt     = new TH1D("hviszpt", "Visible p_{T}^{ll}", 50, 0., 100.);
  TH1* hcolzpt     = new TH1D("hcolzpt", "Colinear p_{T}^{ll}", 50, 0., 100.);
  TH1* hbalzpt     = new TH1D("hbalzpt", "Balanced p_{T}^{ll}", 50, 0., 100.);
  TH1* hcombzpt    = new TH1D("hcombzpt", "Combined p_{T}^{ll}", 50, 0., 100.);

  TH1* hgenalpha      = new TH1D("hgenalpha" , "Gen #alpha", 25, 0., 2.);
  TH1* hgenbeta       = new TH1D("hgenbeta"  , "Gen #beta" , 25, 0., 2.);
  TH1* hgenbetar      = new TH1D("hgenbetar" , "Gen #alpha/#beta", 25, 0., 3.);
  TH2* hgenalphavbeta = new TH2D("hgenalphavbeta", "Gen #alpha_{1} vs #alpha_{2}", 20, 0, 1.2, 20, 0, 1.2);
  TH1* hrecalpha      = new TH1D("hrecalpha" , "Rec #alpha", 25, 0., 2.);
  TH1* hrecbeta       = new TH1D("hrecbeta"  , "Rec #beta" , 25, 0., 2.);
  TH1* hrecbetar      = new TH1D("hrecbetar" , "Rec #alpha/#beta", 25, 0., 3.);
  TH2* hrecalphavbeta = new TH2D("hrecalphavbeta", "Rec #alpha_{1} vs #alpha_{2}", 20, 0, 1.2, 20, 0, 1.2);
  TH1* hlepdot        = new TH1D("hlepdot" , "#sqrt{l_{1}#cdotl_{2}", 50, 50., 120.);

  TH1* hmass       = new TH1D("hmass"   , "Gen M(l,#tau)", 100., 50., 120.);
  TH1* hvismass    = new TH1D("hvismass", "M(l,#tau_{vis})", 100., 50., 120.);
  TH1* hcolmass    = new TH1D("hcolmass", "M(l,#tau_{col})", 100., 50., 120.);
  TH1* hmetmass    = new TH1D("hmetmass", "M(l,#tau_{met})", 100., 50., 120.);
  TH1* hbalmass    = new TH1D("hbalmass", "M(l,#tau_{bal})", 100., 50., 120.);
  TH1* hcombmass   = new TH1D("hcombmass", "M(l,#tau_{comb})", 100., 50., 120.);
  TH1* hcolmdiff   = new TH1D("hcolmdiff", "M(l,#tau_{col}) - M(l,#tau)", 100., -50., 50.);
  TH1* hmetmdiff   = new TH1D("hmetmdiff", "M(l,#tau_{met}) - M(l,#tau)", 100., -50., 50.);
  TH1* hbalmdiff   = new TH1D("hbalmdiff", "M(l,#tau_{bal}) - M(l,#tau)", 100., -50., 50.);

  TH1* hcoltaum    = new TH1D("hcoltaum"  , "M(#tau_{col})", 100., 0., 10.);
  TH2* hcolmdiffvphi = new TH2D("hcolmdiffvphi" , "#DeltaM(coll) vs |#Delta#phi(MET)| ", 30, -30., 30., 20, 0., M_PI);

  hmet       ->SetLineWidth(2);
  hgenmet    ->SetLineWidth(2);
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
  hcombpt    ->SetLineWidth(2);
  hmetnudiff ->SetLineWidth(2);
  hgenmetnudiff->SetLineWidth(2);
  hgenmetdiff->SetLineWidth(2);
  hcolnudiff ->SetLineWidth(2);
  hbalnudiff ->SetLineWidth(2);
  hcolnusize ->SetLineWidth(2);
  hnudphi    ->SetLineWidth(2);
  hnudeta    ->SetLineWidth(2);

  hdphi      ->SetLineWidth(2);

  hzpt       ->SetLineWidth(2);
  hviszpt     ->SetLineWidth(2);
  hcolzpt    ->SetLineWidth(2);
  hbalzpt    ->SetLineWidth(2);
  hcombzpt   ->SetLineWidth(2);

  hmass      ->SetLineWidth(2);
  hvismass   ->SetLineWidth(2);
  hcolmass   ->SetLineWidth(2);
  hmetmass   ->SetLineWidth(2);
  hbalmass   ->SetLineWidth(2);
  hbalmass   ->SetLineWidth(2);
  hcombmass  ->SetLineWidth(2);
  hmetmdiff  ->SetLineWidth(2);
  hbalmdiff  ->SetLineWidth(2);
  hcoltaum   ->SetLineWidth(2);

  hrecalphavbeta->GetXaxis()->SetTitle("#alpha_{1}");
  hrecalphavbeta->GetYaxis()->SetTitle("#alpha_{2}");
  hnudphiveta->GetXaxis()->SetTitle("#Delta#phi");
  hnudphiveta->GetYaxis()->SetTitle("#Delta#eta");

  const int nmax = 100;
  UInt_t nGenPart;
  Float_t GenPart_pt[nmax], GenPart_eta[nmax], GenPart_phi[nmax], GenPart_mass[nmax];
  Int_t GenPart_pdgId[nmax], GenPart_statusFlags[nmax], GenPart_mother[nmax];
  Float_t MET, METPhi;
  Float_t GenMET, GenMETPhi;

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
  chain.SetBranchAddress("GenMET_pt"               , &GenMET            );
  chain.SetBranchAddress("GenMET_phi"              , &GenMETPhi         );

  const Long64_t nentries = (max_entries < 0 || chain.GetEntries() < max_entries) ? chain.GetEntries() : max_entries;
  cout << "Processing " << nentries << " entries...\n";

  TLorentzVector pnu, pmet, pgenmet, pcol, pbal, pcomb, plep, ptau, pvis, pz;
  int idx_z, idx_lep, idx_tau;
  for(Long64_t entry = 0; entry < nentries; ++entry) {
    if(entry % 10000 == 0 || verbose_ > 9) printf("Processing entry %6lld...\n", entry);
    chain.GetEntry(entry);
    if(verbose_ > 9) printf("ID :   pdg    pt      eta     phi    M   mother Last First Prompt Hard\n");
    if(verbose_ > 9) printf("----------------------------------------------------------------------\n");
    idx_z = -1; idx_lep = -1; idx_tau = -1;
    pnu.SetPxPyPzE(0., 0., 0., 0.);
    pvis.SetPxPyPzE(0., 0., 0., 0.);
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
      } else if((pdg == 11 || pdg == 13) && islast && prompt) { //Z->l+tau lepton
        idx_lep = idx;
      } else if((pdg == 15) && islast && prompt) { //Z->l+tau tau
        idx_tau = idx;
      } else if((pdg == 12 || pdg == 14 || pdg == 16) && idx_mother >= 0 && idx_mother == idx_tau) { //prompt tau decay product
        TLorentzVector lv; lv.SetPtEtaPhiM(GenPart_pt[idx], GenPart_eta[idx], GenPart_phi[idx], 0.);
        pnu += lv;
      } else if(idx_mother >= 0 && idx_mother == idx_tau) { //non-neutrino prompt tau decay product
        TLorentzVector lv; lv.SetPtEtaPhiM(GenPart_pt[idx], GenPart_eta[idx], GenPart_phi[idx], GenPart_mass[idx]);
        pvis += lv;
      }
    }
    if(idx_lep < 0 || idx_tau < 0) {
      printf("Entry %lld: Z decay products not found!\n", entry);
      continue;
    }
    plep.SetPtEtaPhiM(GenPart_pt[idx_lep], GenPart_eta[idx_lep], GenPart_phi[idx_lep], GenPart_mass[idx_lep]);
    ptau.SetPtEtaPhiM(GenPart_pt[idx_tau], GenPart_eta[idx_tau], GenPart_phi[idx_tau], GenPart_mass[idx_tau]);
    pmet.SetPtEtaPhiM(MET, pvis.Eta(), METPhi, 0.);
    pgenmet.SetPtEtaPhiM(GenMET, pvis.Eta(), GenMETPhi, 0.);
    TVector3 ptau_dir = pvis.Vect(); ptau_dir.SetZ(0.); ptau_dir.SetMag(1.);
    TVector3 met = pmet.Vect(); met.SetZ(0.);
    TVector3 genmet = pgenmet.Vect(); genmet.SetZ(0.);
    const float pnu_col = met*ptau_dir;
    pcol.SetPtEtaPhiM(std::fabs(pnu_col), pvis.Eta()*((pnu_col < 0.) ? -1. : 1.), (pnu_col < 0.) ? M_PI + pvis.Phi() : pvis.Phi(), 0.);
    //neutrino momentum assuming collimated with tau and pT(Z) = 0
    pbal.SetPtEtaPhiM(std::fabs(plep.Pt() - pvis.Pt()), pvis.Eta()*((pvis.Pt() > plep.Pt()) ? -1. : 1.), (pvis.Pt() > plep.Pt()) ? -pvis.Phi() : pvis.Phi(), 0.);

    // if(pnu_col > 0. && pnu_col < pbal.Pt()) { //Take the MET as an upper limit on the neutrino pT
    //   pcomb.SetPtEtaPhiM(pcol.Pt(), pvis.Eta(), pcol.Phi(), 0.);
    // } else {
    //   pcomb.SetPtEtaPhiM(pbal.Pt(), pvis.Eta(), pbal.Phi(), 0.);
    // }
    TVector3 pcomb_v = 0.5*(pcol.Vect() + pbal.Vect());
    pcomb = TLorentzVector(pcomb_v, pcomb_v.Mag());

    if(verbose_ > 9) {
      printf("\nObj:    pt   eta   phi    M\n");
      printf(  "------------------------------\n");
      printf("Lep: %5.1f %5.2f %5.2f %6.2f\n", plep.Pt(), plep.Eta(), plep.Phi(), plep.M());
      printf("Tau: %5.1f %5.2f %5.2f %6.2f\n", ptau.Pt(), ptau.Eta(), ptau.Phi(), ptau.M());
      printf("Vis: %5.1f %5.2f %5.2f %6.2f\n", pvis.Pt(), pvis.Eta(), pvis.Phi(), pvis.M());
      printf("Nu : %5.1f %5.2f %5.2f %6.2f\n", pnu .Pt(), pnu .Eta(), pnu .Phi(), pnu .M());
      printf("MET: %5.1f %5.2f %5.2f %6.2f\n", MET, 0., METPhi, 0.);
      printf("GMT: %5.1f %5.2f %5.2f %6.2f\n", GenMET, 0., GenMETPhi, 0.);
      printf("Col: %5.1f %5.2f %5.2f %6.2f\n", pcol.Pt(), pcol.Eta(), pcol.Phi(), pcol.M());
      printf("Bal: %5.1f %5.2f %5.2f %6.2f\n", pbal.Pt(), pbal.Eta(), pbal.Phi(), pbal.M());
      printf("Cmb: %5.1f %5.2f %5.2f %6.2f\n", pcomb.Pt(), pcomb.Eta(), pcomb.Phi(), pcomb.M());
    }
    const float mcoll = (pvis+plep+pcol).M();
    if(!std::isfinite(mcoll) || mcoll < 0.) printf("Entry %lld: Undefined mcoll = %f\n", entry, mcoll);

    const float gen_alpha = pvis.Pt() / ptau.Pt();
    const float gen_beta  = plep.Pt() / plep.Pt();
    const float lep_dot   = sqrt(2.*pvis*plep);
    const float rec_alpha = ((lep_dot/91.2)*sqrt(pvis.Pt()/plep.Pt()));
    const float rec_beta  = ((lep_dot/91.2)*sqrt(plep.Pt()/pvis.Pt()));
    hgenalpha->Fill(gen_alpha);
    hgenbeta->Fill(gen_beta);
    hgenbetar->Fill(gen_alpha/gen_beta);
    hgenalphavbeta->Fill(gen_alpha, gen_beta);
    hrecalpha->Fill(rec_alpha);
    hrecbeta->Fill(rec_beta);
    hrecbetar->Fill(rec_alpha/rec_beta);
    hrecalphavbeta->Fill(rec_alpha, rec_beta);
    hlepdot->Fill(lep_dot);

    hmet->Fill(MET*((std::fabs(pmet.DeltaPhi(pnu)) > 1.5) ? -1 : 1 ));
    hgenmet->Fill(GenMET);
    hnupt->Fill(pnu.Pt()*((fabs(pnu.DeltaPhi(pvis)) > 1.) ? -1. : 1.));
    htaupt->Fill(ptau.Pt());
    hgenptratio->Fill(ptau.Pt()/plep.Pt());
    hvisptratio->Fill(pvis.Pt()/plep.Pt());
    hcolpt->Fill(pcol.Pt()*((fabs(pcol.DeltaPhi(pvis)) > 1.) ? -1. : 1.));
    hbalpt->Fill(pbal.Pt()*((fabs(pbal.DeltaPhi(pvis)) > 1.) ? -1. : 1.));
    hcombpt->Fill(pcomb.Pt()*((fabs(pcomb.DeltaPhi(pvis)) > 1.) ? -1. : 1.));
    hmetnudiff->Fill(MET - pnu.Pt());
    hgenmetnudiff->Fill(GenMET - pnu.Pt());
    hgenmetdiff->Fill(MET - GenMET);
    hcolnudiff->Fill(pcol.Pt() - pnu.Pt());
    hbalnudiff->Fill(pbal.Pt() - pnu.Pt());
    hnudphi->Fill(std::fabs(pnu.DeltaPhi(pvis)));
    hnudeta->Fill(std::fabs(pnu.Eta() - pvis.Eta()));
    hnudphiveta->Fill(std::fabs(pnu.DeltaPhi(pvis)), std::fabs(pnu.Eta() - pvis.Eta()));
    hcoltaum->Fill((pvis+pcol).M());

    TVector3 pcol_t = pcol.Vect(); pcol_t.SetZ(0.);
    TVector3 pnu_t  = pnu.Vect(); pnu_t.SetZ(0.);
    //FIXME: Eval size(p_t) vector diff size

    hdphi->Fill(std::fabs(plep.DeltaPhi(ptau)));
    hptratiovdphi->Fill(plep.Pt()/ptau.Pt(), std::fabs(plep.DeltaPhi(ptau)));

    hzpt->Fill((ptau+plep).Pt());
    hviszpt->Fill((pvis+plep).Pt());
    hcolzpt->Fill((pvis+plep+pcol).Pt());
    hbalzpt->Fill((pvis+plep+pbal).Pt());
    hcombzpt->Fill((pvis+plep+pcomb).Pt());

    hmass->Fill((ptau+plep).M());
    hvismass->Fill((pvis+plep).M());
    hcolmass->Fill((pvis+plep+pcol).M());
    hbalmass->Fill((pvis+plep+pbal).M());
    hcombmass->Fill((pvis+plep+pcomb).M());
    hmetmass->Fill((pvis+plep+pmet).M());
    hcolmdiff->Fill((pvis+plep+pcol).M() - (ptau+plep).M());
    hcolmdiffvphi->Fill((pvis+plep+pcol).M() - (ptau+plep).M(), std::fabs(pvis.DeltaPhi(pmet)));
    hmetmdiff->Fill((pvis+plep+pmet).M() - (ptau+plep).M());
    hbalmdiff->Fill((pvis+plep+pbal).M() - (ptau+plep).M());
  }

  ///////////////////////////////////////////////
  //Plot the results

  gSystem->Exec(Form("[ ! -d figures/%s/signal ] && mkdir -p figures/%s/signal", signal.Data(), signal.Data()));

  gStyle->SetOptStat(0);
  // gStyle->SetOptStat(1111111);
  TCanvas* c = new TCanvas();

  hmet       ->Draw("hist"); c->SaveAs(Form("figures/%s/signal/met.png"         , signal.Data()));
  hgenmet    ->Draw("hist"); c->SaveAs(Form("figures/%s/signal/genmet.png"      , signal.Data()));
  hnupt      ->Draw("hist"); c->SaveAs(Form("figures/%s/signal/nupt.png"        , signal.Data()));
  htaupt     ->Draw("hist"); c->SaveAs(Form("figures/%s/signal/taupt.png"       , signal.Data()));
  hgenptratio->Draw("hist"); c->SaveAs(Form("figures/%s/signal/gen_pt_ratio.png", signal.Data()));
  hvisptratio->Draw("hist"); c->SaveAs(Form("figures/%s/signal/vis_pt_ratio.png", signal.Data()));

  hzpt       ->Draw("hist"); c->SaveAs(Form("figures/%s/signal/z_pt.png"    , signal.Data()));
  hviszpt    ->Draw("hist"); c->SaveAs(Form("figures/%s/signal/vis_zpt.png" , signal.Data()));
  hcolzpt    ->Draw("hist"); c->SaveAs(Form("figures/%s/signal/col_zpt.png" , signal.Data()));
  hbalzpt    ->Draw("hist"); c->SaveAs(Form("figures/%s/signal/bal_zpt.png" , signal.Data()));
  hcombzpt   ->Draw("hist"); c->SaveAs(Form("figures/%s/signal/comb_zpt.png", signal.Data()));

  hgenalpha     ->Draw("hist"); c->SaveAs(Form("figures/%s/signal/alpha_gen.png"          , signal.Data()));
  hgenbeta      ->Draw("hist"); c->SaveAs(Form("figures/%s/signal/beta_gen.png"           , signal.Data()));
  hgenbetar     ->Draw("hist"); c->SaveAs(Form("figures/%s/signal/alpha_over_beta_gen.png", signal.Data()));
  hgenalphavbeta->Draw("colz"); c->SaveAs(Form("figures/%s/signal/alpha_v_beta_gen.png"   , signal.Data()));
  hrecalpha     ->Draw("hist"); c->SaveAs(Form("figures/%s/signal/alpha_rec.png"          , signal.Data()));
  hrecbeta      ->Draw("hist"); c->SaveAs(Form("figures/%s/signal/beta_rec.png"           , signal.Data()));
  hrecbetar     ->Draw("hist"); c->SaveAs(Form("figures/%s/signal/alpha_over_beta_rec.png", signal.Data()));
  hrecalphavbeta->Draw("colz"); c->SaveAs(Form("figures/%s/signal/alpha_v_beta_rec.png"   , signal.Data()));
  hlepdot       ->Draw("hist"); c->SaveAs(Form("figures/%s/signal/lep_dot.png"            , signal.Data()));

  hcolpt ->Draw("hist"); c->SaveAs(Form("figures/%s/signal/colpt.png"  , signal.Data()));
  hbalpt ->Draw("hist"); c->SaveAs(Form("figures/%s/signal/bal_pt.png" , signal.Data()));
  hcombpt->Draw("hist"); c->SaveAs(Form("figures/%s/signal/comb_pt.png", signal.Data()));

  hmetnudiff   ->Draw("hist"); c->SaveAs(Form("figures/%s/signal/met_nu_diff.png"    , signal.Data()));
  hgenmetnudiff->Draw("hist"); c->SaveAs(Form("figures/%s/signal/genmet_nu_diff.png" , signal.Data()));
  hgenmetdiff  ->Draw("hist"); c->SaveAs(Form("figures/%s/signal/genmet_diff.png"    , signal.Data()));
  hcolnudiff   ->Draw("hist"); c->SaveAs(Form("figures/%s/signal/col_nu_diff.png"    , signal.Data()));
  hbalnudiff   ->Draw("hist"); c->SaveAs(Form("figures/%s/signal/bal_nu_diff.png"    , signal.Data()));
  hnudphi      ->Draw("hist"); c->SaveAs(Form("figures/%s/signal/nudphi.png"         , signal.Data()));
  hnudeta      ->Draw("hist"); c->SaveAs(Form("figures/%s/signal/nudeta.png"         , signal.Data()));
  hnudphiveta  ->Draw("colz"); c->SaveAs(Form("figures/%s/signal/nu_dphi_vs_deta.png", signal.Data()));
  hdphi        ->Draw("hist"); c->SaveAs(Form("figures/%s/signal/dphi.png"           , signal.Data()));
  hptratiovdphi->Draw("colz"); c->SaveAs(Form("figures/%s/signal/pt_ratio_v_dphi.png", signal.Data()));

  hcoltaum     ->Draw("hist"); c->SaveAs(Form("figures/%s/signal/col_tau_mass.png"        , signal.Data()));
  hcolmdiffvphi->Draw("colz"); c->SaveAs(Form("figures/%s/signal/mass_col_diff_v_dphi.png", signal.Data()));
  hmass        ->Draw("hist"); c->SaveAs(Form("figures/%s/signal/mass.png"                , signal.Data()));
  hvismass     ->Draw("hist"); c->SaveAs(Form("figures/%s/signal/mass_vis.png"            , signal.Data()));
  hcolmass     ->Draw("hist"); c->SaveAs(Form("figures/%s/signal/mass_col.png"            , signal.Data()));
  hbalmass     ->Draw("hist"); c->SaveAs(Form("figures/%s/signal/mass_bal.png"            , signal.Data()));
  hcombmass    ->Draw("hist"); c->SaveAs(Form("figures/%s/signal/mass_comb.png"           , signal.Data()));
  hmetmass     ->Draw("hist"); c->SaveAs(Form("figures/%s/signal/mass_met.png"            , signal.Data()));
  hcolmdiff    ->Draw("hist"); c->SaveAs(Form("figures/%s/signal/mass_col_diff.png"       , signal.Data()));
  hbalmdiff    ->Draw("hist"); c->SaveAs(Form("figures/%s/signal/mass_bal_diff.png"       , signal.Data()));
  hmetmdiff    ->Draw("hist"); c->SaveAs(Form("figures/%s/signal/mass_met_diff.png"       , signal.Data()));

  gStyle->SetOptStat(0);
  hmass->Draw("hist");
  hvismass->SetLineColor(kViolet+6);
  hcolmass->SetLineColor(kRed);
  hmetmass->SetLineColor(kGreen-7);
  hbalmass->SetLineColor(kYellow-3);
  hcombmass->SetLineColor(kOrange);
  hvismass->Draw("same hist");
  hcolmass->Draw("same hist");
  hmetmass->Draw("same hist");
  hbalmass->Draw("same hist");
  hcombmass->Draw("same hist");
  hmass->SetTitle("Mass estimates");
  TLegend* leg = new TLegend();
  leg->AddEntry(hmass, "True mass");
  leg->AddEntry(hvismass, "Visible mass");
  leg->AddEntry(hcolmass, "Projected mass");
  leg->AddEntry(hmetmass, "MET mass");
  leg->AddEntry(hbalmass, "Balanced mass");
  leg->AddEntry(hcombmass, "Combined mass");
  leg->Draw();
  c->SaveAs(Form("figures/%s/signal/mass_comparison.png", signal.Data()));

  hzpt       ->Draw("hist");
  hviszpt->SetLineColor(kViolet+6);
  hcolzpt->SetLineColor(kRed);
  hbalzpt->SetLineColor(kYellow-3);
  hcombzpt->SetLineColor(kOrange);
  hviszpt    ->Draw("same hist");
  hcolzpt    ->Draw("same hist");
  hbalzpt    ->Draw("same hist");
  hcombzpt   ->Draw("same hist");
  hzpt->SetTitle("Z pT estimates");
  leg = new TLegend();
  leg->AddEntry(hzpt, "True pT");
  leg->AddEntry(hviszpt, "Visible pT");
  leg->AddEntry(hcolzpt, "Projected pT");
  leg->AddEntry(hbalzpt, "Balanced pT");
  leg->AddEntry(hcombzpt, "Combined pT");
  leg->Draw();
  c->SaveAs(Form("figures/%s/signal/z_pt_comparison.png", signal.Data()));

  hnupt       ->Draw("hist");// hnupt->SetFillColor(kBlue); hnupt->SetFillStyle(3003);
  hmet   ->SetLineColor(kGreen-7);
  hcolpt ->SetLineColor(kRed);// hcolpt ->SetFillColor(kRed); hcolpt ->SetFillStyle(3007);
  hbalpt ->SetLineColor(kCyan+1);// hbalpt ->SetFillColor(kCyan+1); hbalpt ->SetFillStyle(3004);
  hcombpt->SetLineColor(kOrange);// hcombpt->SetFillColor(kOrange); hcombpt->SetFillStyle(3005);
  hmet      ->Draw("same hist");
  hcolpt    ->Draw("same hist");
  hbalpt    ->Draw("same hist");
  hcombpt   ->Draw("same hist");
  hnupt->SetTitle("#nu pT estimates");
  leg = new TLegend(0.6,0.75,0.9,0.9);
  leg->AddEntry(hnupt, "True pT");
  leg->AddEntry(hmet, "MET pT");
  leg->AddEntry(hcolpt, "Projected pT");
  leg->AddEntry(hbalpt, "Balanced pT");
  leg->AddEntry(hcombpt, "Combined pT");
  leg->Draw();
  c->SaveAs(Form("figures/%s/signal/nu_pt_comparison.png", signal.Data()));

}
