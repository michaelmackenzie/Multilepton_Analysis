//make plots of the base gen-level selection, to compare DY MC and Embedding

int ndebug_ = -1;

TLorentzVector get_had_tau_vector(int index, const int npart, const int* mothers, const int* pdgs,
                                  const float* pts, const float* etas, const float* phis, const float* masses) {
  vector<int> daughters;
  do {
    daughters = {};
    for(int ipart = 0; ipart < npart; ++ipart) {
      if(mothers[ipart] == index) {
        //catch tau -> tau + gamma
        if(abs(pdgs[ipart]) == 15) {
          index = ipart;
          daughters = {};
          break;
        }
        daughters.push_back(ipart);
      }
    }
  } while(daughters.size() == 0);

  if(daughters.size() == 0) {
    cout << "Error! No tau daughters found!\n";
    return TLorentzVector();
  }
  TLorentzVector visible, loop;
  visible.SetPtEtaPhiM(0., 0., 0., 0.);
  for(int ipart : daughters) {
    const int pdg = abs(pdgs[ipart]);
    if(pdg == 12 || pdg == 14 || pdg == 16) continue;
    loop.SetPtEtaPhiM(pts[ipart], etas[ipart], phis[ipart], masses[ipart]);
    visible += loop;
  }
  return visible;
}

int make_hists(const TString selec = "emu", const bool isEmbed = true, const int year = 2018, const bool tight = false) {
  TString final_state;
  if(selec == "emu") final_state = "EMu";
  else if(selec == "etau") final_state = "ETau";
  else if(selec == "mutau") final_state = "MuTau";
  else {
    cout << "Unknown selection " << selec.Data() << endl;
    return -1;
  }

  //List of runs to process if embedding
  vector<TString> runs = {""};
  if(isEmbed) {
    if(year == 2016) runs = {"B", "C", "D", "E", "F", "G", "H"};
    if(year == 2017) runs = {"B", "C", "D", "E", "F"};
    if(year == 2018) runs = {"A", "B", "C", "D"};
  }

  //unfolding weight
  CLFV::EmbeddingWeight embed_wt;
  //MC DY Z spectrum correction
  CLFV::ZPtWeight zptweight("MuMu", 1, 90);
  //Cross section information (including missing embedding events)
  CLFV::CrossSections xsecs;

  const float lum = xsecs.GetLuminosity(year); //for DY normalization

  //Data fields
  const int n = 500;
  UInt_t nGenPart;
  float GenPart_pt[n], GenPart_eta[n], GenPart_phi[n], GenPart_mass[n];
  int GenPart_pdgId[n], GenPart_status[n], GenPart_statusFlags[n], GenPart_genPartIdxMother[n];
  float z_pt, z_mass, z_eta;
  int lep_one_id, lep_two_id, lep_one_idx, lep_two_idx, tau_one_idx, tau_two_idx;
  float genWeight(1.f);

  //Histograms to compare
  TH1* hOnePt     = new TH1D("honept"    , "One p_{T}"      , 50,  0, 100);
  TH1* hTwoPt     = new TH1D("htwopt"    , "Two p_{T}"      , 50,  0, 100);
  TH1* hElectronPt= new TH1D("helectronpt"   , "Electron p_{T}"     , 50,  0, 100);
  TH1* hMuonPt    = new TH1D("hmuonpt"   , "Muon p_{T}"     , 50,  0, 100);
  TH1* hTauPt     = new TH1D("htaupt"    , "Tau p_{T}"      , 50,  0, 100);
  TH1* hTauOnePt  = new TH1D("htauonept" , "Tau One p_{T}"  , 50,  0, 100);
  TH1* hTauTwoPt  = new TH1D("htautwopt" , "Tau Two p_{T}"  , 50,  0, 100);
  TH1* hOneEta    = new TH1D("honeeta"   , "One #eta"       , 60, -3., 3.);
  TH1* hTwoEta    = new TH1D("htwoeta"   , "Two #eta"       , 60, -3., 3.);
  TH1* hElectronEta   = new TH1D("helectroneta"  , "Electron #eta"      , 60, -3., 3.);
  TH1* hMuonEta   = new TH1D("hmuoneta"  , "Muon #eta"      , 60, -3., 3.);
  TH1* hTauEta    = new TH1D("htaueta"   , "Tau #eta"       , 60, -3., 3.);
  TH1* hTauOneEta = new TH1D("htauoneeta", "Tau One #eta"   , 60, -3., 3.);
  TH1* hTauTwoEta = new TH1D("htautwoeta", "Tau Two #eta"   , 60, -3., 3.);
  TH1* hLeadPt    = new TH1D("hleadpt"   , "Lead p_{T}"     , 50,  0, 100);
  TH1* hTrailPt   = new TH1D("htrailpt"  , "Trail p_{T}"    , 50,  0, 100);
  TH1* hLeadEta   = new TH1D("hleadeta"  , "Lead #eta"      , 60, -3., 3.);
  TH1* hTrailEta  = new TH1D("htraileta" , "Trail #eta"     , 60, -3., 3.);
  TH1* hZMass     = new TH1D("hzmass"    , "Z Mass"         , 100, 40, 200);
  TH1* hZPt       = new TH1D("hzpt"      , "Z p_{T}"        , 50,  0, 100);
  TH1* hZEta      = new TH1D("hzeta"     , "Z #eta"         , 100,-10,  10);
  TH1* hDiLepMass = new TH1D("hdilepmass", "Di-lepton Mass" , 100,  0, 200);
  TH1* hDiLepPt   = new TH1D("hdileppt"  , "Di-lepton p_{T}", 50,  0, 100);
  TH1* hDiLepEta  = new TH1D("hdilepeta" , "Di-lepton #eta" , 100,-10,  10);

  const double pt_bins[]  = {0., 3., 6., 10., 20., 30., 50., 100.};
  const double eta_bins[] = {0., 1., 2., 3., 4., 5., 6., 8., 10.};
  const int n_pt_bins  = sizeof(pt_bins)  / sizeof(*pt_bins) - 1;
  const int n_eta_bins = sizeof(eta_bins) / sizeof(*eta_bins) - 1;
  TH2* hZEtaVsPt  = new TH2D("hzetavspt" , "Z |#eta| vs p_{T}",  n_eta_bins, eta_bins, n_pt_bins, pt_bins);

  TH1* hWeight    = new TH1D("hweight"   , "Total weight"   , 100,0.,  2);
  TH1* hUnfold    = new TH1D("hunfold"   , "Unfolding weight", 50,0.,  2);

  //Normalization info to output
  Long64_t nseen(0), nnorm(0);

  //Gen-level cuts
  const float min_lep_pt        = (selec == "emu") ? 9. : 18.;

  //Reco-level cuts
  const float min_electron_pt   = 15.;
  const float min_muon_pt       = 10.;
  const float min_tau_pt        = 20.;
  const float min_electron_trig = (year == 2016) ? 29. : 35.;
  const float min_muon_trig     = (year == 2017) ? 28. : 25.;
  const float max_eta           = 2.2;
  const float min_lepm          = 50.;

  //File loop
  for(TString run : runs) {
    TFile* f = TFile::Open(Form("root://cmseos.fnal.gov//store/user/mmackenz/%s_study/files/%sStudy_%s_%i.root",
                                selec.Data(), final_state.Data(),
                                (isEmbed) ? ("Embed-"+final_state+"-"+run).Data() : "DY50",
                                year), "READ");
    if(!f) return 1;
    if(ndebug_ > 0 && nseen > ndebug_) continue;
    TTree* Events = (TTree*) f->Get("Events");
    TTree* Norm   = (TTree*) f->Get("Norm"  );
    if(!Events || !Norm) {
      cout << "Input ntuples not found!\n";
      return 2;
    }

    //Get N(processed) normalization info
    Long64_t nevt;
    Norm->SetBranchAddress("NEvents", &nevt);
    for(int i = 0; i < Norm->GetEntries(); ++i) {
      Norm->GetEntry(i);
      nnorm += nevt;
    }
    const float xs = ((isEmbed) ? xsecs.GetCrossSection(Form("Embed-%s-%s", final_state.Data(), run.Data()), year) :
                      xsecs.GetCrossSection("DY50", year)*lum
                      /(nnorm*(1. - 2.*xsecs.GetNegativeFraction(Form("DY50-%s", (year == 2017) ? "ext" : "amc"), year))));

    //Events in the current tree
    const Long64_t nentries = Events->GetEntries();

    //Turn off unused branches
    Events->SetBranchStatus("*", 0);
    Events->SetBranchStatus("nGenPart"  , 1);
    Events->SetBranchStatus("GenPart*"  , 1);
    Events->SetBranchStatus("GenZll*"   , 1);
    Events->SetBranchStatus("genWeight" , 1);

    Events->SetBranchAddress("nGenPart"                , &nGenPart                );
    Events->SetBranchAddress("GenPart_pt"              , &GenPart_pt              );
    Events->SetBranchAddress("GenPart_eta"             , &GenPart_eta             );
    Events->SetBranchAddress("GenPart_phi"             , &GenPart_phi             );
    Events->SetBranchAddress("GenPart_mass"            , &GenPart_mass            );
    Events->SetBranchAddress("GenPart_pdgId"           , &GenPart_pdgId           );
    Events->SetBranchAddress("GenPart_status"          , &GenPart_status          );
    Events->SetBranchAddress("GenPart_statusFlags"     , &GenPart_statusFlags     );
    Events->SetBranchAddress("GenPart_genPartIdxMother", &GenPart_genPartIdxMother);

    Events->SetBranchAddress("GenZll_mass"             , &z_mass                  );
    Events->SetBranchAddress("GenZll_pt"               , &z_pt                    );
    Events->SetBranchAddress("GenZll_eta"              , &z_eta                   );
    Events->SetBranchAddress("GenZll_LepOne_DecayIdx"  , &lep_one_idx             );
    Events->SetBranchAddress("GenZll_LepTwo_DecayIdx"  , &lep_two_idx             );
    Events->SetBranchAddress("GenZll_LepOne_Idx"       , &tau_one_idx             );
    Events->SetBranchAddress("GenZll_LepTwo_Idx"       , &tau_two_idx             );

    Events->SetBranchAddress("genWeight"               , &genWeight               );

    cout << "Beginning processing " << nentries << " events";
    if(isEmbed) cout << " for Run " << run.Data();
    cout << "; xs = " << xs << endl;

    int ntest = 0;
    TLorentzVector lv1, lv2, nu1, nu2;
    for(Long64_t entry = 0; entry < nentries; ++entry) {
      if(ndebug_ > 0 && nseen > ndebug_) break;
      if(entry % 50000 == 0) printf("Processing event %12lld (%6.2f%%)...\n", entry, entry*100./nentries);

      Events->GetEntry(entry);

      if(lep_one_idx < 0) lv1 = get_had_tau_vector(tau_one_idx, nGenPart, GenPart_genPartIdxMother, GenPart_pdgId,
                                                   GenPart_pt, GenPart_eta, GenPart_phi, GenPart_mass);
      else                lv1.SetPtEtaPhiM(GenPart_pt[lep_one_idx], GenPart_eta[lep_one_idx],
                                           GenPart_phi[lep_one_idx], GenPart_mass[lep_one_idx]);
      if(lep_two_idx < 0) lv2 = get_had_tau_vector(tau_two_idx, nGenPart, GenPart_genPartIdxMother, GenPart_pdgId,
                                                   GenPart_pt, GenPart_eta, GenPart_phi, GenPart_mass);
      else                lv2.SetPtEtaPhiM(GenPart_pt[lep_two_idx], GenPart_eta[lep_two_idx],
                                           GenPart_phi[lep_two_idx], GenPart_mass[lep_two_idx]);

      //FIXME: Using original tau index in the case of hadronic tau decays
      if(lep_one_idx < 0) lep_one_idx = tau_one_idx;
      if(lep_two_idx < 0) lep_two_idx = tau_two_idx;

      if(lv1.Pt() < min_lep_pt) continue;
      if(lv2.Pt() < min_lep_pt) continue;
      //eta cuts
      if(std::fabs(lv1.Eta()) >= max_eta) continue;
      if(std::fabs(lv2.Eta()) >= max_eta) continue;

      //reconstructable events
      if(tight) {
        bool one_trig(false), two_trig(false);
        //electron cuts
        if(std::abs(GenPart_pdgId[lep_one_idx]) == 11 && lv1.Pt() < min_electron_pt) continue;
        if(std::abs(GenPart_pdgId[lep_two_idx]) == 11 && lv2.Pt() < min_electron_pt) continue;
        one_trig |= std::abs(GenPart_pdgId[lep_one_idx]) == 11 && lv1.Pt() > min_electron_trig;
        two_trig |= std::abs(GenPart_pdgId[lep_two_idx]) == 11 && lv2.Pt() > min_electron_trig;
        //muon cuts
        if(std::abs(GenPart_pdgId[lep_one_idx]) == 13 && lv1.Pt() < min_muon_pt) continue;
        if(std::abs(GenPart_pdgId[lep_two_idx]) == 13 && lv2.Pt() < min_muon_pt) continue;
        one_trig |= std::abs(GenPart_pdgId[lep_one_idx]) == 13 && lv1.Pt() > min_muon_trig;
        two_trig |= std::abs(GenPart_pdgId[lep_two_idx]) == 13 && lv2.Pt() > min_muon_trig;
        //tau cuts
        if(std::abs(GenPart_pdgId[lep_one_idx]) == 15 && lv1.Pt() < min_tau_pt) continue;
        if(std::abs(GenPart_pdgId[lep_two_idx]) == 15 && lv2.Pt() < min_tau_pt) continue;
        //trigger cuts
        if(!one_trig && !two_trig) continue;
        //di-lepton cuts
        if((lv1+lv2).M() <= min_lepm) continue;
      }

      ///////////////////////////////////////
      // Apply event weights

      if(isEmbed && genWeight > 1.f) continue;
      float wt = 1.f;
      if(isEmbed) wt *= (genWeight < 1.f)*genWeight;
      else        wt *= (genWeight < 0.f) ? -1.f : 1.f; //only apply the sign of the weight

      //get the embedding unfolding weight
      float unfold = (isEmbed) ? embed_wt.UnfoldingWeight(GenPart_pt[tau_one_idx], GenPart_eta[tau_one_idx],
                                                          GenPart_pt[tau_two_idx], GenPart_eta[tau_two_idx],
                                                          z_eta,
                                                          z_pt,
                                                          year) : 1.f;
      wt *= unfold;

      //apply the data-driven Z pT weights
      if(!isEmbed) {
        wt *= zptweight.GetWeight(year, z_pt, z_mass, false);
      }

      //apply the sample cross section
      wt *= xs;

      if(wt == 0. || !std::isfinite(wt)) {
        cout << "Warning! Weight = " << wt << ", tau 1: pt = " << GenPart_pt[tau_one_idx]
             << " eta = " << GenPart_eta[tau_one_idx] << "; tau 2: pt = " << GenPart_pt[tau_two_idx]
             << " eta = " << GenPart_eta[tau_two_idx]
             << "; genWeight = " << genWeight << endl;
        continue;
      }
      ++nseen;

      //////////////////////////////////////
      // Fill the histograms

      hOnePt ->Fill(lv1.Pt() , wt);
      hTwoPt ->Fill(lv2.Pt() , wt);
      hOneEta->Fill(lv1.Eta(), wt);
      hTwoEta->Fill(lv2.Eta(), wt);

      if(std::abs(GenPart_pdgId[lep_one_idx]) == 11) {
        hElectronPt ->Fill(lv1.Pt() , wt);
        hElectronEta->Fill(lv1.Eta(), wt);
      } else if(std::abs(GenPart_pdgId[lep_one_idx]) == 13) {
        hMuonPt ->Fill(lv1.Pt() , wt);
        hMuonEta->Fill(lv1.Eta(), wt);
      } else if(std::abs(GenPart_pdgId[lep_one_idx]) == 15) {
        hTauPt ->Fill(lv1.Pt() , wt);
        hTauEta->Fill(lv1.Eta(), wt);
      }

      if(std::abs(GenPart_pdgId[lep_two_idx]) == 11) {
        hElectronPt ->Fill(lv2.Pt() , wt);
        hElectronEta->Fill(lv2.Eta(), wt);
      } else if(std::abs(GenPart_pdgId[lep_two_idx]) == 13) {
        hMuonPt ->Fill(lv2.Pt() , wt);
        hMuonEta->Fill(lv2.Eta(), wt);
      } else if(std::abs(GenPart_pdgId[lep_two_idx]) == 15) {
        hTauPt ->Fill(lv2.Pt() , wt);
        hTauEta->Fill(lv2.Eta(), wt);
      }

      hTauOnePt ->Fill(GenPart_pt [tau_one_idx], wt);
      hTauTwoPt ->Fill(GenPart_pt [tau_two_idx], wt);
      hTauOneEta->Fill(GenPart_eta[tau_one_idx], wt);
      hTauTwoEta->Fill(GenPart_eta[tau_two_idx], wt);

      TLorentzVector lead_lv  = (lv1.Pt() > lv2.Pt()) ? lv1 : lv2;
      TLorentzVector trail_lv = (lv1.Pt() > lv2.Pt()) ? lv2 : lv1;
      hLeadPt  ->Fill(lead_lv .Pt() , wt);
      hTrailPt ->Fill(trail_lv.Pt() , wt);
      hLeadEta ->Fill(lead_lv .Eta(), wt);
      hTrailEta->Fill(trail_lv.Eta(), wt);

      hZMass   ->Fill(z_mass, wt);
      hZPt     ->Fill(z_pt, wt);
      hZEta    ->Fill(z_eta, wt);
      hZEtaVsPt->Fill(fabs(z_eta), z_pt, wt);

      hDiLepMass->Fill((lv1+lv2).M()  , wt);
      hDiLepPt  ->Fill((lv1+lv2).Pt() , wt);
      hDiLepEta ->Fill((lv1+lv2).Eta(), wt);

      hUnfold->Fill(unfold);
      hWeight->Fill(wt);
    }

    cout << "N(test) events = " << ntest << endl;
    f->Close();
  }

  cout << "Saw " << nseen << " events, dataset events = " << nnorm << endl;
  TH1* hnorm = new TH1D("hevents", "Event count", 1, -10, 10);
  hnorm->Fill(1,nnorm);

  const TString basename = Form("figures/%s_%s%s_%i", selec.Data(), (isEmbed) ? "embed" : "dy50", (tight) ? "_tight" : "", year);
  gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s", basename.Data(), basename.Data()));
  gSystem->Exec("[ ! -d histograms ] && mkdir histograms");

  cout << "Histograms have " << hOnePt->GetEntries() << " entries, integral = " << hOnePt->Integral() << endl;
  TString outname = basename;
  outname.ReplaceAll("figures/", "histograms/");
  TFile* fout = new TFile(Form("%s.hist", outname.Data()), "RECREATE");
  hnorm       ->Write();
  hOnePt      ->Write();
  hTwoPt      ->Write();
  hOneEta     ->Write();
  hTwoEta     ->Write();
  hElectronPt ->Write();
  hMuonPt     ->Write();
  hTauPt      ->Write();
  hElectronEta->Write();
  hMuonEta    ->Write();
  hTauEta     ->Write();
  hTauOnePt   ->Write();
  hTauTwoPt   ->Write();
  hTauOneEta  ->Write();
  hTauTwoEta  ->Write();
  hLeadPt     ->Write();
  hTrailPt    ->Write();
  hLeadEta    ->Write();
  hTrailEta   ->Write();
  hZMass      ->Write();
  hZPt        ->Write();
  hZEta       ->Write();
  hDiLepMass  ->Write();
  hDiLepPt    ->Write();
  hDiLepEta   ->Write();
  hZEtaVsPt   ->Write();
  fout->Close();

  TCanvas* c = new TCanvas();
  hOnePt->Draw("hist e1");
  c->Print(Form("%s/onept.png", basename.Data()));
  hTwoPt->Draw("hist e1");
  c->Print(Form("%s/twopt.png", basename.Data()));

  hOneEta->Draw("hist e1");
  c->Print(Form("%s/oneeta.png", basename.Data()));
  hTwoEta->Draw("hist e1");
  c->Print(Form("%s/twoeta.png", basename.Data()));

  hElectronPt->Draw("hist e1");
  c->Print(Form("%s/electronpt.png", basename.Data()));
  hMuonPt->Draw("hist e1");
  c->Print(Form("%s/muonpt.png", basename.Data()));
  hTauPt->Draw("hist e1");
  c->Print(Form("%s/taupt.png", basename.Data()));

  hElectronEta->Draw("hist e1");
  c->Print(Form("%s/electroneta.png", basename.Data()));
  hMuonEta->Draw("hist e1");
  c->Print(Form("%s/muoneta.png", basename.Data()));
  hTauEta->Draw("hist e1");
  c->Print(Form("%s/taueta.png", basename.Data()));

  hTauOnePt->Draw("hist e1");
  c->Print(Form("%s/tauonept.png", basename.Data()));
  hTauTwoPt->Draw("hist e1");
  c->Print(Form("%s/tautwopt.png", basename.Data()));

  hTauOneEta->Draw("hist e1");
  c->Print(Form("%s/tauoneeta.png", basename.Data()));
  hTauTwoEta->Draw("hist e1");
  c->Print(Form("%s/tautwoeta.png", basename.Data()));

  hLeadPt->Draw("hist e1");
  c->Print(Form("%s/leadpt.png", basename.Data()));
  hTrailPt->Draw("hist e1");
  c->Print(Form("%s/trailpt.png", basename.Data()));

  hLeadEta->Draw("hist e1");
  c->Print(Form("%s/leadeta.png", basename.Data()));
  hTrailEta->Draw("hist e1");
  c->Print(Form("%s/traileta.png", basename.Data()));

  hZMass->Draw("hist e1");
  c->Print(Form("%s/zmass.png", basename.Data()));
  hZPt->Draw("hist e1");
  c->Print(Form("%s/zpt.png", basename.Data()));
  hZEta->Draw("hist e1");
  c->Print(Form("%s/zeta.png", basename.Data()));

  hDiLepMass->Draw("hist e1");
  c->Print(Form("%s/dilepmass.png", basename.Data()));
  hDiLepPt->Draw("hist e1");
  c->Print(Form("%s/dileppt.png", basename.Data()));
  hDiLepEta->Draw("hist e1");
  c->Print(Form("%s/dilepeta.png", basename.Data()));

  hUnfold->Draw("hist e1");
  c->Print(Form("%s/unfold.png", basename.Data()));
  hWeight->Draw("hist e1");
  c->Print(Form("%s/weight.png", basename.Data()));

  gStyle->SetOptStat(0);
  hZEtaVsPt->Draw("colz");
  c->SetLogy(true);
  c->Print(Form("%s/zetavspt.png", basename.Data()));
  c->SetLogy(false);

  return 0;
}
