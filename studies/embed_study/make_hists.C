//make plots of the base gen-level selection, to compare DY MC and Embedding

int ndebug_ = -1;
bool overRide_ = true; //override 2018 unfolding with 2016 or IC weights

//-------------------------------------------------------------------------------------------------------------------
void print_figure(TH1* h, TString path) {
  TCanvas* c = new TCanvas();
  h->SetLineWidth(2);
  h->SetMarkerStyle(20);
  h->SetMarkerSize(0.8);
  h->Draw("hist e1");
  c->Print(path.Data());
  delete c;
}

//-------------------------------------------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------------------------------------------
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
  CLFV::EmbeddingWeight embed_wt((overRide_) ? 10 : 0); //mode 4: override 2018 trigger efficiencies with 2016 values, mode 10: use IC weights for 2018
  //MC DY Z spectrum correction
  CLFV::ZPtWeight zptweight("MuMu", 1);
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
  UInt_t nTau = 0;
  float Tau_pt[n], Tau_eta[n], Tau_phi[n], Tau_mass[n];
  UChar_t Tau_idDeepTau2017v2p1VSjet[n];
  Int_t Tau_genPartIdx[n];

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
  TH1* hMuonEta   = new TH1D("hmuoneta"  , "Muon #eta"      ,  60, -3., 3.);
  TH1* hTauEta    = new TH1D("htaueta"   , "Tau #eta"       ,  60, -3., 3.);
  TH1* hTauOneEta = new TH1D("htauoneeta", "Tau One #eta"   ,  60, -3., 3.);
  TH1* hTauTwoEta = new TH1D("htautwoeta", "Tau Two #eta"   ,  60, -3., 3.);
  TH1* hLeadPt    = new TH1D("hleadpt"   , "Lead p_{T}"     ,  50,  0, 100);
  TH1* hTrailPt   = new TH1D("htrailpt"  , "Trail p_{T}"    ,  50,  0, 100);
  TH1* hLeadEta   = new TH1D("hleadeta"  , "Lead #eta"      ,  60, -3., 3.);
  TH1* hTrailEta  = new TH1D("htraileta" , "Trail #eta"     ,  60, -3., 3.);
  TH1* hZMass     = new TH1D("hzmass"    , "Z Mass"         , 100, 40, 200);
  TH1* hZPt       = new TH1D("hzpt"      , "Z p_{T}"        ,  50,  0, 150);
  TH1* hZEta      = new TH1D("hzeta"     , "Z #eta"         ,  50,-10,  10);
  TH1* hDiLepMass = new TH1D("hdilepmass", "Di-lepton Mass" , 100,  0, 200);
  TH1* hDiLepPt   = new TH1D("hdileppt"  , "Di-lepton p_{T}",  50,  0, 100);
  TH1* hDiLepEta  = new TH1D("hdilepeta" , "Di-lepton #eta" , 100,-10,  10);

  const double pt_bins[]  = {0., 3., 6., 10., 20., 30., 50., 100.};
  const double eta_bins[] = {0., 1., 2., 3., 4., 5., 6., 8., 10.};
  const int n_pt_bins  = sizeof(pt_bins)  / sizeof(*pt_bins) - 1;
  const int n_eta_bins = sizeof(eta_bins) / sizeof(*eta_bins) - 1;
  TH2* hZEtaVsPt  = new TH2D("hzetavspt" , "Z |#eta| vs p_{T}",  n_eta_bins, eta_bins, n_pt_bins, pt_bins);

  TH1* hWeight    = new TH1D("hweight"   , "Total weight"         , 100, -2., 2);
  TH1* hLogWeight = new TH1D("hlogweight", "Log10(|Total weight|)",  50,-7., 2);
  TH1* hUnfold    = new TH1D("hunfold"   , "Unfolding weight"     , 50, 0., 2);

  //Reco tau info
  TH1* hNRecoTau   = new TH1D("hnrecotau"  , "N(reco) taus", 10, 0,  10.);
  TH1* hRecoTauPt  = new TH1D("hrecotaupt" , "Reco tau p_{T}", 50, 0, 100.);
  TH1* hGenTauPt   = new TH1D("hgentaupt"  , "Gen tau p_{T} matched to Reco taus", 50, 0, 100.);
  TH1* hTauPtDiff  = new TH1D("htauptdiff" , "Reco tau p_{T} - gen p_{T}", 50, -25, 25.);
  TH1* hRecoTauEta = new TH1D("hrecotaueta", "Reco tau #eta", 60, -3., 3.);
  TH1* hGenTauEta  = new TH1D("hgentaueta" , "Gen tau #eta matched to Reco taus", 60, -3., 3.);

  const double tau_eff_pt_bins[]  = {20., 25., 30., 35., 40., 45., 50., 55., 65., 80., 100.};
  const double tau_eff_eta_bins[] = {0., 0.8, 1.5, 2.3};
  const int n_tau_eff_pt_bins  = sizeof(tau_eff_pt_bins)  / sizeof(double) - 1;
  const int n_tau_eff_eta_bins = sizeof(tau_eff_eta_bins) / sizeof(double) - 1;
  TH2* hGenTauEtaVsPt = new TH2D("hgentauetaptvseta" , "Gen tau |#eta| vs p_{T} matched to Reco taus", n_tau_eff_eta_bins, tau_eff_eta_bins, n_tau_eff_pt_bins, tau_eff_pt_bins);
  TH2* hTauEtaVsPt    = new TH2D("htauetaptvseta"    , "Gen tau |#eta| vs p_{T}"                     , n_tau_eff_eta_bins, tau_eff_eta_bins, n_tau_eff_pt_bins, tau_eff_pt_bins);

  //Normalization info to output
  Long64_t nseen(0), nnorm(0), nneg(0);

  //Gen-level cuts
  const float min_lep_pt        = (selec == "emu") ? 9. : 18.;

  //Reco-level cuts
  const float min_electron_pt   = 15.;
  const float min_muon_pt       = 10.;
  const float min_tau_pt        = 20.;
  const float min_electron_trig = 29.; //(year == 2016) ? 29. : 35.;
  const float min_muon_trig     = 25.; //(year == 2017) ? 28. : 25.;
  const float max_lep_eta       = (selec == "emu") ? 2.4 : 2.2;
  const float max_tau_eta       = 2.3;
  const float min_lepm          = 50.;

  //Reco tau cuts
  const int   min_tau_id        = 50; //reco tau anti-jet ID

  //File loop
  for(TString run : runs) {
    TFile* f = TFile::Open(Form("root://cmseos.fnal.gov//store/user/mmackenz/%s_study/files/%sStudy_%s_%i.root",
                                selec.Data(), final_state.Data(),
                                (isEmbed) ? ("Embed-"+final_state+"-"+run).Data() : "DY50",
                                year), "READ");
    if(!f) return 1;
    TTree* Events = (TTree*) f->Get("Events");
    TTree* Norm   = (TTree*) f->Get("Norm"  );
    if(!Events || !Norm) {
      cout << "Input ntuples not found!\n";
      return 2;
    }

    //Get N(processed) normalization info
    Long64_t nevt, nsample(0);
    Norm->SetBranchAddress("NEvents", &nevt);
    for(int i = 0; i < Norm->GetEntries(); ++i) {
      Norm->GetEntry(i);
      nnorm += nevt;
      nsample += nevt;
      if(nevt <= 0) {
        cout << "WARNING! Entry " << i << " in Norm tree has 0 events\n";
      }
    }
    const Long64_t ngen = (isEmbed) ? xsecs.GetGenNumber(Form("Embed-%s-%s", final_state.Data(), run.Data()), year) : 0;
    if(isEmbed && ngen != nsample) {
      cout << "WARNING! N(sample events) != N(gen events)! N(sample) = " << nsample << " and N(gen) = " << ngen << endl;
    }
    const float neg_frac = (isEmbed) ? 0.f : xsecs.GetNegativeFraction(Form("DY50-%s", (year == 2017) ? "ext" : "amc"), year);
    const float xs = ((isEmbed) ? xsecs.GetCrossSection(Form("Embed-%s-%s", final_state.Data(), run.Data()), year)*(ngen*1./nsample) :
                      xsecs.GetCrossSection("DY50", year)*lum
                      /(nnorm*(1. - 2.*neg_frac)));

    //Skip to the next event if finished with debug processes
    if(ndebug_ > 0 && nseen > ndebug_) continue;

    //Events in the current tree
    const Long64_t nentries = Events->GetEntries();

    //Turn off unused branches
    Events->SetBranchStatus("*", 0);
    // Events->SetBranchStatus("nGenPart"  , 1);
    // Events->SetBranchStatus("GenPart*"  , 1);
    // Events->SetBranchStatus("GenZll*"   , 1);
    // Events->SetBranchStatus("genWeight" , 1);
    // Events->SetBranchStatus("nTau"      , 1);

    CLFV::Utilities::SetBranchAddress(Events, "nGenPart"                , &nGenPart                );
    CLFV::Utilities::SetBranchAddress(Events, "GenPart_pt"              , &GenPart_pt              );
    CLFV::Utilities::SetBranchAddress(Events, "GenPart_eta"             , &GenPart_eta             );
    CLFV::Utilities::SetBranchAddress(Events, "GenPart_phi"             , &GenPart_phi             );
    CLFV::Utilities::SetBranchAddress(Events, "GenPart_mass"            , &GenPart_mass            );
    CLFV::Utilities::SetBranchAddress(Events, "GenPart_pdgId"           , &GenPart_pdgId           );
    CLFV::Utilities::SetBranchAddress(Events, "GenPart_status"          , &GenPart_status          );
    CLFV::Utilities::SetBranchAddress(Events, "GenPart_statusFlags"     , &GenPart_statusFlags     );
    CLFV::Utilities::SetBranchAddress(Events, "GenPart_genPartIdxMother", &GenPart_genPartIdxMother);

    CLFV::Utilities::SetBranchAddress(Events, "GenZll_mass"             , &z_mass                  );
    CLFV::Utilities::SetBranchAddress(Events, "GenZll_pt"               , &z_pt                    );
    CLFV::Utilities::SetBranchAddress(Events, "GenZll_eta"              , &z_eta                   );
    CLFV::Utilities::SetBranchAddress(Events, "GenZll_LepOne_DecayIdx"  , &lep_one_idx             );
    CLFV::Utilities::SetBranchAddress(Events, "GenZll_LepTwo_DecayIdx"  , &lep_two_idx             );
    CLFV::Utilities::SetBranchAddress(Events, "GenZll_LepOne_Idx"       , &tau_one_idx             );
    CLFV::Utilities::SetBranchAddress(Events, "GenZll_LepTwo_Idx"       , &tau_two_idx             );

    CLFV::Utilities::SetBranchAddress(Events, "genWeight"               , &genWeight               );

    CLFV::Utilities::SetBranchAddress(Events, "nTau"                      , &nTau                      );
    CLFV::Utilities::SetBranchAddress(Events, "Tau_pt"                    , &Tau_pt                    );
    CLFV::Utilities::SetBranchAddress(Events, "Tau_eta"                   , &Tau_eta                   );
    CLFV::Utilities::SetBranchAddress(Events, "Tau_phi"                   , &Tau_phi                   );
    CLFV::Utilities::SetBranchAddress(Events, "Tau_mass"                  , &Tau_mass                  );
    CLFV::Utilities::SetBranchAddress(Events, "Tau_idDeepTau2017v2p1VSjet", &Tau_idDeepTau2017v2p1VSjet);
    CLFV::Utilities::SetBranchAddress(Events, "Tau_genPartIdx"            , &Tau_genPartIdx            );

    cout << "Beginning processing " << nentries << " events";
    if(isEmbed) cout << " for Run " << run.Data();
    cout << "; xs = " << xs << endl;

    int ntest = 0;
    TLorentzVector lv1, lv2, nu1, nu2, tau1, tau2, recotau;
    int reco_tau_jet_id = -1;
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

      tau1.SetPtEtaPhiM(GenPart_pt [tau_one_idx], GenPart_eta [tau_one_idx],
                        GenPart_phi[tau_one_idx], GenPart_mass[tau_one_idx]);
      tau2.SetPtEtaPhiM(GenPart_pt [tau_two_idx], GenPart_eta [tau_two_idx],
                        GenPart_phi[tau_two_idx], GenPart_mass[tau_two_idx]);

      //FIXME: Add plots of neutrino variables
      nu1 = tau1 - lv1;
      nu2 = tau2 - lv2;

      if(z_mass < 0.f || z_pt < 0.f) { //if missing Z info, replace with gen di-tau info
        z_mass = (tau1 + tau2).M();
        z_pt   = (tau1 + tau2).Pt();
        z_eta  = (tau1 + tau2).Eta();
      }

      //set the hadronic tau Idx to original tau Idx to set pdg ID = 15
      if(lep_one_idx < 0) lep_one_idx = tau_one_idx;
      if(lep_two_idx < 0) lep_two_idx = tau_two_idx;

      const int pdg_l1 = std::abs(GenPart_pdgId[lep_one_idx]);
      const int pdg_l2 = std::abs(GenPart_pdgId[lep_two_idx]);

      //pt cuts
      if(lv1.Pt() < min_lep_pt) continue;
      if(lv2.Pt() < min_lep_pt) continue;

      //eta cuts
      if(pdg_l1 == 15 && std::fabs(lv1.Eta()) >= max_tau_eta) continue;
      if(pdg_l1 != 15 && std::fabs(lv1.Eta()) >= max_lep_eta) continue;
      if(pdg_l2 == 15 && std::fabs(lv2.Eta()) >= max_tau_eta) continue;
      if(pdg_l2 != 15 && std::fabs(lv2.Eta()) >= max_lep_eta) continue;

      //reconstructable events
      if(tight) {
        bool one_trig(false), two_trig(false);
        //electron cuts
        if(pdg_l1 == 11 && lv1.Pt() < min_electron_pt) continue;
        if(pdg_l2 == 11 && lv2.Pt() < min_electron_pt) continue;
        one_trig |= pdg_l1 == 11 && lv1.Pt() > min_electron_trig;
        two_trig |= pdg_l2 == 11 && lv2.Pt() > min_electron_trig;
        //muon cuts
        if(pdg_l1 == 13 && lv1.Pt() < min_muon_pt) continue;
        if(pdg_l2 == 13 && lv2.Pt() < min_muon_pt) continue;
        one_trig |= pdg_l1 == 13 && lv1.Pt() > min_muon_trig;
        two_trig |= pdg_l2 == 13 && lv2.Pt() > min_muon_trig;
        //tau cuts
        if(pdg_l1 == 15 && lv1.Pt() < min_tau_pt) continue;
        if(pdg_l2 == 15 && lv2.Pt() < min_tau_pt) continue;
        //trigger cuts
        if(!one_trig && !two_trig) continue;
        //di-lepton cuts
        if((lv1+lv2).M() <= min_lepm) continue;
      }

      ///////////////////////////////////////
      // Find reco tau, if available
      recotau.SetPtEtaPhiM(0., 0., 0., 0.);
      reco_tau_jet_id = -1;
      for(UInt_t iTau = 0; iTau < nTau; ++iTau) {
        const int idx = Tau_genPartIdx[iTau];
        TLorentzVector loop;
        loop.SetPtEtaPhiM(Tau_pt[iTau], Tau_eta[iTau], Tau_phi[iTau], Tau_mass[iTau]);
        if(pdg_l1 == 15 && loop.DeltaR(lv1) < 0.5) {
          recotau = loop;
          reco_tau_jet_id = Tau_idDeepTau2017v2p1VSjet[iTau];
          if(lep_one_idx == Tau_genPartIdx[iTau]) ++ntest;
          break;
        }
        if(pdg_l2 == 15 && loop.DeltaR(lv2) < 0.5) {
          recotau = loop;
          reco_tau_jet_id = Tau_idDeepTau2017v2p1VSjet[iTau];
          if(lep_two_idx == Tau_genPartIdx[iTau]) ++ntest;
          break;
        }
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
      if(genWeight < 0.f) ++nneg;

      //////////////////////////////////////
      // Fill the histograms

      hOnePt ->Fill(lv1.Pt() , wt);
      hTwoPt ->Fill(lv2.Pt() , wt);
      hOneEta->Fill(lv1.Eta(), wt);
      hTwoEta->Fill(lv2.Eta(), wt);

      if(pdg_l1 == 11) {
        hElectronPt ->Fill(lv1.Pt() , wt);
        hElectronEta->Fill(lv1.Eta(), wt);
      } else if(pdg_l1 == 13) {
        hMuonPt ->Fill(lv1.Pt() , wt);
        hMuonEta->Fill(lv1.Eta(), wt);
      } else if(pdg_l1 == 15) {
        hTauPt ->Fill(lv1.Pt() , wt);
        hTauEta->Fill(lv1.Eta(), wt);
        hTauEtaVsPt->Fill(std::fabs(lv1.Eta()), lv1.Pt(), wt);
      }

      if(pdg_l2 == 11) {
        hElectronPt ->Fill(lv2.Pt() , wt);
        hElectronEta->Fill(lv2.Eta(), wt);
      } else if(pdg_l2 == 13) {
        hMuonPt ->Fill(lv2.Pt() , wt);
        hMuonEta->Fill(lv2.Eta(), wt);
      } else if(pdg_l2 == 15) {
        hTauPt ->Fill(lv2.Pt() , wt);
        hTauEta->Fill(lv2.Eta(), wt);
        hTauEtaVsPt->Fill(std::fabs(lv2.Eta()), lv2.Pt(), wt);
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

      hNRecoTau->Fill(nTau, wt);
      if(recotau.Pt() > 10. && reco_tau_jet_id > min_tau_id) {
        TLorentzVector gentau = (pdg_l1 == 15) ? lv1 : lv2;
        hRecoTauPt->Fill(recotau.Pt(), wt);
        hGenTauPt ->Fill(gentau.Pt(), wt);
        hTauPtDiff->Fill(recotau.Pt() - gentau.Pt(), wt);
        hRecoTauEta->Fill(recotau.Eta(), wt);
        hGenTauEta ->Fill(gentau.Eta(), wt);
        hGenTauEtaVsPt->Fill(std::fabs(gentau.Eta()), gentau.Pt(), wt);
      }

      hUnfold->Fill(unfold);
      hWeight->Fill(wt/xs);
      hLogWeight->Fill((std::fabs(wt) > 0.) ? std::log10(std::fabs(wt)) : -99.);
    }

    cout << "N(test) events = " << ntest << endl;
    cout << "Negative fraction = " << neg_frac << ", effective negative fraction = " << (nneg*1.)/nseen << endl;
    f->Close();
  }

  cout << "Saw " << nseen << " events, dataset events = " << nnorm << endl;
  TH1* hnorm = new TH1D("hevents", "Event count", 1, -10, 10);
  hnorm->Fill(1,nnorm);

  //evaluate some reco-tau efficiencies (if applicable)
  TH1* hRecoTauEff = (TH1*) hRecoTauPt->Clone("hrecotaueff");
  hRecoTauEff->Divide(hTauPt);
  hRecoTauEff->SetTitle("Reco tau efficiency");
  TH1* hGenTauEff = (TH1*) hGenTauPt->Clone("hgentaueff");
  hGenTauEff->Divide(hTauPt);
  hGenTauEff->SetTitle("Reco tau efficiency by Gen p_{T}");
  TH1* hGenTauEtaEff = (TH1*) hGenTauEta->Clone("hgentauetaeff");
  hGenTauEtaEff->Divide(hTauEta);
  hGenTauEtaEff->SetTitle("Reco tau efficiency by Gen #eta");
  TH2* hGenTauEtaVsPtEff = (TH2*) hGenTauEtaVsPt->Clone("hgentauetavspteff");
  hGenTauEtaVsPtEff->Divide(hTauEtaVsPt);
  hGenTauEtaVsPtEff->SetTitle("Reco tau efficiency by Gen (|#eta|, p_{T})");

  const TString basename = Form("figures/%s_%s%s_%i", selec.Data(), (isEmbed) ? "embed" : "dy50", (tight) ? "_tight" : "", year);
  gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s", basename.Data(), basename.Data()));
  gSystem->Exec("[ ! -d histograms ] && mkdir histograms");

  cout << "Histograms have " << hOnePt->GetEntries() << " entries, integral = " << hOnePt->Integral() << endl;
  TString outname = basename;
  outname.ReplaceAll("figures/", "histograms/");
  TFile* fout = new TFile(Form("%s.hist", outname.Data()), "RECREATE");
  hnorm            ->Write();
  hOnePt           ->Write();
  hTwoPt           ->Write();
  hOneEta          ->Write();
  hTwoEta          ->Write();
  hElectronPt      ->Write();
  hMuonPt          ->Write();
  hTauPt           ->Write();
  hElectronEta     ->Write();
  hMuonEta         ->Write();
  hTauEta          ->Write();
  hTauOnePt        ->Write();
  hTauTwoPt        ->Write();
  hTauOneEta       ->Write();
  hTauTwoEta       ->Write();
  hLeadPt          ->Write();
  hTrailPt         ->Write();
  hLeadEta         ->Write();
  hTrailEta        ->Write();
  hZMass           ->Write();
  hZPt             ->Write();
  hZEta            ->Write();
  hDiLepMass       ->Write();
  hDiLepPt         ->Write();
  hDiLepEta        ->Write();
  hNRecoTau        ->Write();
  hRecoTauPt       ->Write();
  hGenTauPt        ->Write();
  hTauPtDiff       ->Write();
  hTauEtaVsPt      ->Write();
  hRecoTauEff      ->Write();
  hGenTauEff       ->Write();
  hRecoTauEta      ->Write();
  hGenTauEta       ->Write();
  hGenTauEtaEff    ->Write();
  hGenTauEtaVsPt   ->Write();
  hGenTauEtaVsPtEff->Write();
  hLogWeight       ->Write();
  hZEtaVsPt        ->Write();
  fout->Close();

  print_figure(hOnePt, Form("%s/onept.png", basename.Data()));
  print_figure(hTwoPt, Form("%s/twopt.png", basename.Data()));

  print_figure(hOneEta, Form("%s/oneeta.png", basename.Data()));
  print_figure(hTwoEta, Form("%s/twoeta.png", basename.Data()));

  print_figure(hElectronPt, Form("%s/electronpt.png", basename.Data()));
  print_figure(hMuonPt, Form("%s/muonpt.png", basename.Data()));
  print_figure(hTauPt, Form("%s/taupt.png", basename.Data()));

  print_figure(hElectronEta, Form("%s/electroneta.png", basename.Data()));
  print_figure(hMuonEta, Form("%s/muoneta.png", basename.Data()));
  print_figure(hTauEta, Form("%s/taueta.png", basename.Data()));

  print_figure(hTauOnePt, Form("%s/tauonept.png", basename.Data()));
  print_figure(hTauTwoPt, Form("%s/tautwopt.png", basename.Data()));

  print_figure(hTauOneEta, Form("%s/tauoneeta.png", basename.Data()));
  print_figure(hTauTwoEta, Form("%s/tautwoeta.png", basename.Data()));

  print_figure(hLeadPt, Form("%s/leadpt.png", basename.Data()));
  print_figure(hTrailPt, Form("%s/trailpt.png", basename.Data()));

  print_figure(hLeadEta, Form("%s/leadeta.png", basename.Data()));
  print_figure(hTrailEta, Form("%s/traileta.png", basename.Data()));

  print_figure(hZMass, Form("%s/zmass.png", basename.Data()));
  print_figure(hZPt, Form("%s/zpt.png", basename.Data()));
  print_figure(hZEta, Form("%s/zeta.png", basename.Data()));

  print_figure(hDiLepMass, Form("%s/dilepmass.png", basename.Data()));
  print_figure(hDiLepPt, Form("%s/dileppt.png", basename.Data()));
  print_figure(hDiLepEta, Form("%s/dilepeta.png", basename.Data()));

  print_figure(hNRecoTau, Form("%s/nrecotau.png", basename.Data()));
  print_figure(hRecoTauPt, Form("%s/recotaupt.png", basename.Data()));
  print_figure(hGenTauPt, Form("%s/gentaupt.png", basename.Data()));
  print_figure(hTauPtDiff, Form("%s/tauptdiff.png", basename.Data()));
  print_figure(hRecoTauEff, Form("%s/recotaueff.png", basename.Data()));
  print_figure(hGenTauEff, Form("%s/gentaueff.png", basename.Data()));
  print_figure(hRecoTauEta, Form("%s/recotaueta.png", basename.Data()));
  print_figure(hGenTauEta, Form("%s/gentaueta.png", basename.Data()));
  print_figure(hGenTauEtaEff, Form("%s/gentauetaeff.png", basename.Data()));

  print_figure(hUnfold, Form("%s/unfold.png", basename.Data()));
  print_figure(hWeight, Form("%s/weight.png", basename.Data()));
  print_figure(hLogWeight, Form("%s/logweight.png", basename.Data()));

  gStyle->SetOptStat(0);
  gStyle->SetPaintTextFormat(".2f");
  TCanvas* c = new TCanvas();
  hZEtaVsPt->Draw("colz");
  c->SetLogy(true);
  c->Print(Form("%s/zetavspt.png", basename.Data()));
  c->SetLogy(false);

  hTauEtaVsPt->Draw("colz");
  c->Print(Form("%s/tauetavspt.png", basename.Data()));
  hGenTauEtaVsPt->Draw("colz");
  c->Print(Form("%s/gentauetavspt.png", basename.Data()));
  hGenTauEtaVsPtEff->Draw("colz text");
  c->Print(Form("%s/gentauetavspteff.png", basename.Data()));

  delete c;
  return 0;
}
