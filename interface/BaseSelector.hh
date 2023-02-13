//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sun Feb 12 11:45:40 2023 by ROOT version 6.14/09
// from TChain mutau/
//////////////////////////////////////////////////////////

#ifndef BaseSelector_h
#define BaseSelector_h

#include <TROOT.h>
#include <TTree.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <TH2.h>
#include <TStyle.h>

namespace CLFV {

  class BaseSelector : public TSelector {
  public :
    TTreeReader     fReader;  //!the tree reader
    TTree          *fChain = 0;   //!pointer to the analyzed TTree or TChain

    // Readers to access the data (delete the ones you do not need).
    TTreeReaderValue<UInt_t> run = {fReader, "run"};
    TTreeReaderValue<UInt_t> luminosityBlock = {fReader, "luminosityBlock"};
    TTreeReaderValue<ULong64_t> event = {fReader, "event"};
    TTreeReaderValue<UInt_t> nElectron = {fReader, "nElectron"};
    TTreeReaderArray<Float_t> Electron_deltaEtaSC = {fReader, "Electron_deltaEtaSC"};
    // TTreeReaderArray<Float_t> Electron_dr03EcalRecHitSumEt = {fReader, "Electron_dr03EcalRecHitSumEt"};
    // TTreeReaderArray<Float_t> Electron_dr03HcalDepth1TowerSumEt = {fReader, "Electron_dr03HcalDepth1TowerSumEt"};
    // TTreeReaderArray<Float_t> Electron_dr03TkSumPt = {fReader, "Electron_dr03TkSumPt"};
    // TTreeReaderArray<Float_t> Electron_dr03TkSumPtHEEP = {fReader, "Electron_dr03TkSumPtHEEP"};
    TTreeReaderArray<Float_t> Electron_dxy = {fReader, "Electron_dxy"};
    TTreeReaderArray<Float_t> Electron_dxyErr = {fReader, "Electron_dxyErr"};
    TTreeReaderArray<Float_t> Electron_dz = {fReader, "Electron_dz"};
    TTreeReaderArray<Float_t> Electron_dzErr = {fReader, "Electron_dzErr"};
    TTreeReaderArray<Float_t> Electron_eCorr = {fReader, "Electron_eCorr"};
    // TTreeReaderArray<Float_t> Electron_eInvMinusPInv = {fReader, "Electron_eInvMinusPInv"};
    TTreeReaderArray<Float_t> Electron_energyErr = {fReader, "Electron_energyErr"};
    TTreeReaderArray<Float_t> Electron_eta = {fReader, "Electron_eta"};
    TTreeReaderArray<Float_t> Electron_hoe = {fReader, "Electron_hoe"};
    // TTreeReaderArray<Float_t> Electron_ip3d = {fReader, "Electron_ip3d"};
    // TTreeReaderArray<Float_t> Electron_jetPtRelv2 = {fReader, "Electron_jetPtRelv2"};
    // TTreeReaderArray<Float_t> Electron_jetRelIso = {fReader, "Electron_jetRelIso"};
    TTreeReaderArray<Float_t> Electron_mass = {fReader, "Electron_mass"};
    // TTreeReaderArray<Float_t> Electron_miniPFRelIso_all = {fReader, "Electron_miniPFRelIso_all"};
    // TTreeReaderArray<Float_t> Electron_miniPFRelIso_chg = {fReader, "Electron_miniPFRelIso_chg"};
    // TTreeReaderArray<Float_t> Electron_mvaFall17V1Iso = {fReader, "Electron_mvaFall17V1Iso"};
    // TTreeReaderArray<Float_t> Electron_mvaFall17V1noIso = {fReader, "Electron_mvaFall17V1noIso"};
    TTreeReaderArray<Float_t> Electron_mvaFall17V2Iso = {fReader, "Electron_mvaFall17V2Iso"};
    TTreeReaderArray<Float_t> Electron_mvaFall17V2noIso = {fReader, "Electron_mvaFall17V2noIso"};
    // TTreeReaderArray<Float_t> Electron_mvaSpring16GP = {fReader, "Electron_mvaSpring16GP"};
    // TTreeReaderArray<Float_t> Electron_mvaSpring16HZZ = {fReader, "Electron_mvaSpring16HZZ"};
    TTreeReaderArray<Float_t> Electron_pfRelIso03_all = {fReader, "Electron_pfRelIso03_all"};
    TTreeReaderArray<Float_t> Electron_pfRelIso03_chg = {fReader, "Electron_pfRelIso03_chg"};
    TTreeReaderArray<Float_t> Electron_phi = {fReader, "Electron_phi"};
    TTreeReaderArray<Float_t> Electron_pt = {fReader, "Electron_pt"};
    TTreeReaderArray<Float_t> Electron_r9 = {fReader, "Electron_r9"};
    TTreeReaderArray<Float_t> Electron_scEtOverPt = {fReader, "Electron_scEtOverPt"};
    // TTreeReaderArray<Float_t> Electron_sieie = {fReader, "Electron_sieie"};
    // TTreeReaderArray<Float_t> Electron_sip3d = {fReader, "Electron_sip3d"};
    // TTreeReaderArray<Float_t> Electron_mvaTTH = {fReader, "Electron_mvaTTH"};
    TTreeReaderArray<Int_t> Electron_charge = {fReader, "Electron_charge"};
    // TTreeReaderArray<Int_t> Electron_cutBased = {fReader, "Electron_cutBased"};
    // TTreeReaderArray<Int_t> Electron_cutBased_Fall17_V1 = {fReader, "Electron_cutBased_Fall17_V1"};
    // TTreeReaderArray<Int_t> Electron_cutBased_HLTPreSel = {fReader, "Electron_cutBased_HLTPreSel"};
    // TTreeReaderArray<Int_t> Electron_cutBased_Spring15 = {fReader, "Electron_cutBased_Spring15"};
    // TTreeReaderArray<Int_t> Electron_cutBased_Sum16 = {fReader, "Electron_cutBased_Sum16"};
    TTreeReaderArray<Int_t> Electron_jetIdx = {fReader, "Electron_jetIdx"};
    TTreeReaderArray<Int_t> Electron_pdgId = {fReader, "Electron_pdgId"};
    TTreeReaderArray<Int_t> Electron_photonIdx = {fReader, "Electron_photonIdx"};
    TTreeReaderArray<Int_t> Electron_tightCharge = {fReader, "Electron_tightCharge"};
    // TTreeReaderArray<Int_t> Electron_vidNestedWPBitmap = {fReader, "Electron_vidNestedWPBitmap"};
    // TTreeReaderArray<Int_t> Electron_vidNestedWPBitmapHEEP = {fReader, "Electron_vidNestedWPBitmapHEEP"};
    // TTreeReaderArray<Int_t> Electron_vidNestedWPBitmapSpring15 = {fReader, "Electron_vidNestedWPBitmapSpring15"};
    // TTreeReaderArray<Int_t> Electron_vidNestedWPBitmapSum16 = {fReader, "Electron_vidNestedWPBitmapSum16"};
    TTreeReaderArray<Bool_t> Electron_convVeto = {fReader, "Electron_convVeto"};
    // TTreeReaderArray<Bool_t> Electron_cutBased_HEEP = {fReader, "Electron_cutBased_HEEP"};
    TTreeReaderArray<Bool_t> Electron_isPFcand = {fReader, "Electron_isPFcand"};
    TTreeReaderArray<UChar_t> Electron_lostHits = {fReader, "Electron_lostHits"};
    // TTreeReaderArray<Bool_t> Electron_mvaFall17V1Iso_WP80 = {fReader, "Electron_mvaFall17V1Iso_WP80"};
    // TTreeReaderArray<Bool_t> Electron_mvaFall17V1Iso_WP90 = {fReader, "Electron_mvaFall17V1Iso_WP90"};
    // TTreeReaderArray<Bool_t> Electron_mvaFall17V1Iso_WPL = {fReader, "Electron_mvaFall17V1Iso_WPL"};
    // TTreeReaderArray<Bool_t> Electron_mvaFall17V1noIso_WP80 = {fReader, "Electron_mvaFall17V1noIso_WP80"};
    // TTreeReaderArray<Bool_t> Electron_mvaFall17V1noIso_WP90 = {fReader, "Electron_mvaFall17V1noIso_WP90"};
    // TTreeReaderArray<Bool_t> Electron_mvaFall17V1noIso_WPL = {fReader, "Electron_mvaFall17V1noIso_WPL"};
    TTreeReaderArray<Bool_t> Electron_mvaFall17V2Iso_WP80 = {fReader, "Electron_mvaFall17V2Iso_WP80"};
    TTreeReaderArray<Bool_t> Electron_mvaFall17V2Iso_WP90 = {fReader, "Electron_mvaFall17V2Iso_WP90"};
    TTreeReaderArray<Bool_t> Electron_mvaFall17V2Iso_WPL = {fReader, "Electron_mvaFall17V2Iso_WPL"};
    TTreeReaderArray<Bool_t> Electron_mvaFall17V2noIso_WP80 = {fReader, "Electron_mvaFall17V2noIso_WP80"};
    TTreeReaderArray<Bool_t> Electron_mvaFall17V2noIso_WP90 = {fReader, "Electron_mvaFall17V2noIso_WP90"};
    TTreeReaderArray<Bool_t> Electron_mvaFall17V2noIso_WPL = {fReader, "Electron_mvaFall17V2noIso_WPL"};
    // TTreeReaderArray<Bool_t> Electron_mvaSpring16GP_WP80 = {fReader, "Electron_mvaSpring16GP_WP80"};
    // TTreeReaderArray<Bool_t> Electron_mvaSpring16GP_WP90 = {fReader, "Electron_mvaSpring16GP_WP90"};
    // TTreeReaderArray<Bool_t> Electron_mvaSpring16HZZ_WPL = {fReader, "Electron_mvaSpring16HZZ_WPL"};
    TTreeReaderArray<UChar_t> Electron_seedGain = {fReader, "Electron_seedGain"};
    TTreeReaderValue<UInt_t> nGenPart = {fReader, "nGenPart"};
    TTreeReaderArray<Float_t> GenPart_eta = {fReader, "GenPart_eta"};
    TTreeReaderArray<Float_t> GenPart_mass = {fReader, "GenPart_mass"};
    TTreeReaderArray<Float_t> GenPart_phi = {fReader, "GenPart_phi"};
    TTreeReaderArray<Float_t> GenPart_pt = {fReader, "GenPart_pt"};
    TTreeReaderArray<Int_t> GenPart_genPartIdxMother = {fReader, "GenPart_genPartIdxMother"};
    TTreeReaderArray<Int_t> GenPart_pdgId = {fReader, "GenPart_pdgId"};
    TTreeReaderArray<Int_t> GenPart_status = {fReader, "GenPart_status"};
    TTreeReaderArray<Int_t> GenPart_statusFlags = {fReader, "GenPart_statusFlags"};
    // TTreeReaderValue<Float_t> Generator_binvar = {fReader, "Generator_binvar"};
    // TTreeReaderValue<Float_t> Generator_scalePDF = {fReader, "Generator_scalePDF"};
    // TTreeReaderValue<Float_t> Generator_weight = {fReader, "Generator_weight"};
    // TTreeReaderValue<Float_t> Generator_x1 = {fReader, "Generator_x1"};
    // TTreeReaderValue<Float_t> Generator_x2 = {fReader, "Generator_x2"};
    // TTreeReaderValue<Float_t> Generator_xpdf1 = {fReader, "Generator_xpdf1"};
    // TTreeReaderValue<Float_t> Generator_xpdf2 = {fReader, "Generator_xpdf2"};
    // TTreeReaderValue<Int_t> Generator_id1 = {fReader, "Generator_id1"};
    // TTreeReaderValue<Int_t> Generator_id2 = {fReader, "Generator_id2"};
    // TTreeReaderValue<UInt_t> nGenVisTau = {fReader, "nGenVisTau"};
    // TTreeReaderArray<Float_t> GenVisTau_eta = {fReader, "GenVisTau_eta"};
    // TTreeReaderArray<Float_t> GenVisTau_mass = {fReader, "GenVisTau_mass"};
    // TTreeReaderArray<Float_t> GenVisTau_phi = {fReader, "GenVisTau_phi"};
    // TTreeReaderArray<Float_t> GenVisTau_pt = {fReader, "GenVisTau_pt"};
    // TTreeReaderArray<Int_t> GenVisTau_charge = {fReader, "GenVisTau_charge"};
    // TTreeReaderArray<Int_t> GenVisTau_genPartIdxMother = {fReader, "GenVisTau_genPartIdxMother"};
    // TTreeReaderArray<Int_t> GenVisTau_status = {fReader, "GenVisTau_status"};
    TTreeReaderValue<Float_t> genWeight = {fReader, "genWeight"};
    TTreeReaderValue<UInt_t> nJet = {fReader, "nJet"};
    // TTreeReaderArray<Float_t> Jet_area = {fReader, "Jet_area"};
    // TTreeReaderArray<Float_t> Jet_btagCMVA = {fReader, "Jet_btagCMVA"};
    TTreeReaderArray<Float_t> Jet_btagCSVV2 = {fReader, "Jet_btagCSVV2"};
    TTreeReaderArray<Float_t> Jet_btagDeepB = {fReader, "Jet_btagDeepB"};
    TTreeReaderArray<Float_t> Jet_btagDeepC = {fReader, "Jet_btagDeepC"};
    TTreeReaderArray<Float_t> Jet_btagDeepFlavB = {fReader, "Jet_btagDeepFlavB"};
    TTreeReaderArray<Float_t> Jet_btagDeepFlavC = {fReader, "Jet_btagDeepFlavC"};
    // TTreeReaderArray<Float_t> Jet_chEmEF = {fReader, "Jet_chEmEF"};
    // TTreeReaderArray<Float_t> Jet_chFPV0EF = {fReader, "Jet_chFPV0EF"};
    // TTreeReaderArray<Float_t> Jet_chFPV1EF = {fReader, "Jet_chFPV1EF"};
    // TTreeReaderArray<Float_t> Jet_chFPV2EF = {fReader, "Jet_chFPV2EF"};
    // TTreeReaderArray<Float_t> Jet_chFPV3EF = {fReader, "Jet_chFPV3EF"};
    // TTreeReaderArray<Float_t> Jet_chHEF = {fReader, "Jet_chHEF"};
    TTreeReaderArray<Float_t> Jet_eta = {fReader, "Jet_eta"};
    TTreeReaderArray<Float_t> Jet_mass = {fReader, "Jet_mass"};
    // TTreeReaderArray<Float_t> Jet_muEF = {fReader, "Jet_muEF"};
    // TTreeReaderArray<Float_t> Jet_muonSubtrFactor = {fReader, "Jet_muonSubtrFactor"};
    // TTreeReaderArray<Float_t> Jet_neEmEF = {fReader, "Jet_neEmEF"};
    // TTreeReaderArray<Float_t> Jet_neHEF = {fReader, "Jet_neHEF"};
    TTreeReaderArray<Float_t> Jet_phi = {fReader, "Jet_phi"};
    TTreeReaderArray<Float_t> Jet_pt = {fReader, "Jet_pt"};
    // TTreeReaderArray<Float_t> Jet_puIdDisc = {fReader, "Jet_puIdDisc"};
    // TTreeReaderArray<Float_t> Jet_qgl = {fReader, "Jet_qgl"};
    // TTreeReaderArray<Float_t> Jet_rawFactor = {fReader, "Jet_rawFactor"};
    // TTreeReaderArray<Float_t> Jet_bRegCorr = {fReader, "Jet_bRegCorr"};
    // TTreeReaderArray<Float_t> Jet_bRegRes = {fReader, "Jet_bRegRes"};
    // TTreeReaderArray<Float_t> Jet_cRegCorr = {fReader, "Jet_cRegCorr"};
    // TTreeReaderArray<Float_t> Jet_cRegRes = {fReader, "Jet_cRegRes"};
    // TTreeReaderArray<Int_t> Jet_electronIdx1 = {fReader, "Jet_electronIdx1"};
    // TTreeReaderArray<Int_t> Jet_electronIdx2 = {fReader, "Jet_electronIdx2"};
    TTreeReaderArray<Int_t> Jet_jetId = {fReader, "Jet_jetId"};
    // TTreeReaderArray<Int_t> Jet_muonIdx1 = {fReader, "Jet_muonIdx1"};
    // TTreeReaderArray<Int_t> Jet_muonIdx2 = {fReader, "Jet_muonIdx2"};
    // TTreeReaderArray<Int_t> Jet_nConstituents = {fReader, "Jet_nConstituents"};
    // TTreeReaderArray<Int_t> Jet_nElectrons = {fReader, "Jet_nElectrons"};
    // TTreeReaderArray<Int_t> Jet_nMuons = {fReader, "Jet_nMuons"};
    TTreeReaderArray<Int_t> Jet_puId = {fReader, "Jet_puId"};
    // TTreeReaderValue<Float_t> LHE_HT = {fReader, "LHE_HT"};
    // TTreeReaderValue<Float_t> LHE_HTIncoming = {fReader, "LHE_HTIncoming"};
    // TTreeReaderValue<Float_t> LHE_Vpt = {fReader, "LHE_Vpt"};
    // TTreeReaderValue<Float_t> LHE_AlphaS = {fReader, "LHE_AlphaS"};
    TTreeReaderValue<UChar_t> LHE_Njets = {fReader, "LHE_Njets"};
    // TTreeReaderValue<UChar_t> LHE_Nb = {fReader, "LHE_Nb"};
    // TTreeReaderValue<UChar_t> LHE_Nc = {fReader, "LHE_Nc"};
    // TTreeReaderValue<UChar_t> LHE_Nuds = {fReader, "LHE_Nuds"};
    // TTreeReaderValue<UChar_t> LHE_Nglu = {fReader, "LHE_Nglu"};
    // TTreeReaderValue<UChar_t> LHE_NpNLO = {fReader, "LHE_NpNLO"};
    // TTreeReaderValue<UChar_t> LHE_NpLO = {fReader, "LHE_NpLO"};
    TTreeReaderValue<Float_t> GenMET_phi = {fReader, "GenMET_phi"};
    TTreeReaderValue<Float_t> GenMET_pt = {fReader, "GenMET_pt"};
    // TTreeReaderValue<Float_t> MET_MetUnclustEnUpDeltaX = {fReader, "MET_MetUnclustEnUpDeltaX"};
    // TTreeReaderValue<Float_t> MET_MetUnclustEnUpDeltaY = {fReader, "MET_MetUnclustEnUpDeltaY"};
    // TTreeReaderValue<Float_t> MET_covXX = {fReader, "MET_covXX"};
    // TTreeReaderValue<Float_t> MET_covXY = {fReader, "MET_covXY"};
    // TTreeReaderValue<Float_t> MET_covYY = {fReader, "MET_covYY"};
    TTreeReaderValue<Float_t> MET_phi = {fReader, "MET_phi"};
    TTreeReaderValue<Float_t> MET_pt = {fReader, "MET_pt"};
    TTreeReaderValue<Float_t> MET_significance = {fReader, "MET_significance"};
    TTreeReaderValue<Float_t> MET_sumEt = {fReader, "MET_sumEt"};
    // TTreeReaderValue<Float_t> MET_sumPtUnclustered = {fReader, "MET_sumPtUnclustered"};
    TTreeReaderValue<UInt_t> nMuon = {fReader, "nMuon"};
    TTreeReaderArray<Float_t> Muon_dxy = {fReader, "Muon_dxy"};
    TTreeReaderArray<Float_t> Muon_dxyErr = {fReader, "Muon_dxyErr"};
    // TTreeReaderArray<Float_t> Muon_dxybs = {fReader, "Muon_dxybs"};
    TTreeReaderArray<Float_t> Muon_dz = {fReader, "Muon_dz"};
    TTreeReaderArray<Float_t> Muon_dzErr = {fReader, "Muon_dzErr"};
    TTreeReaderArray<Float_t> Muon_eta = {fReader, "Muon_eta"};
    // TTreeReaderArray<Float_t> Muon_ip3d = {fReader, "Muon_ip3d"};
    // TTreeReaderArray<Float_t> Muon_jetPtRelv2 = {fReader, "Muon_jetPtRelv2"};
    // TTreeReaderArray<Float_t> Muon_jetRelIso = {fReader, "Muon_jetRelIso"};
    TTreeReaderArray<Float_t> Muon_mass = {fReader, "Muon_mass"};
    // TTreeReaderArray<Float_t> Muon_miniPFRelIso_all = {fReader, "Muon_miniPFRelIso_all"};
    // TTreeReaderArray<Float_t> Muon_miniPFRelIso_chg = {fReader, "Muon_miniPFRelIso_chg"};
    TTreeReaderArray<Float_t> Muon_pfRelIso03_all = {fReader, "Muon_pfRelIso03_all"};
    TTreeReaderArray<Float_t> Muon_pfRelIso03_chg = {fReader, "Muon_pfRelIso03_chg"};
    TTreeReaderArray<Float_t> Muon_pfRelIso04_all = {fReader, "Muon_pfRelIso04_all"};
    TTreeReaderArray<Float_t> Muon_phi = {fReader, "Muon_phi"};
    TTreeReaderArray<Float_t> Muon_pt = {fReader, "Muon_pt"};
    TTreeReaderArray<Float_t> Muon_ptErr = {fReader, "Muon_ptErr"};
    // TTreeReaderArray<Float_t> Muon_segmentComp = {fReader, "Muon_segmentComp"};
    // TTreeReaderArray<Float_t> Muon_sip3d = {fReader, "Muon_sip3d"};
    // TTreeReaderArray<Float_t> Muon_tkRelIso = {fReader, "Muon_tkRelIso"};
    // TTreeReaderArray<Float_t> Muon_tunepRelPt = {fReader, "Muon_tunepRelPt"};
    // TTreeReaderArray<Float_t> Muon_mvaLowPt = {fReader, "Muon_mvaLowPt"};
    // TTreeReaderArray<Float_t> Muon_mvaTTH = {fReader, "Muon_mvaTTH"};
    TTreeReaderArray<Int_t> Muon_charge = {fReader, "Muon_charge"};
    // TTreeReaderArray<Int_t> Muon_jetIdx = {fReader, "Muon_jetIdx"};
    TTreeReaderArray<Int_t> Muon_nStations = {fReader, "Muon_nStations"};
    TTreeReaderArray<Int_t> Muon_nTrackerLayers = {fReader, "Muon_nTrackerLayers"};
    TTreeReaderArray<Int_t> Muon_pdgId = {fReader, "Muon_pdgId"};
    TTreeReaderArray<Int_t> Muon_tightCharge = {fReader, "Muon_tightCharge"};
    TTreeReaderArray<Int_t> Muon_fsrPhotonIdx = {fReader, "Muon_fsrPhotonIdx"};
    // TTreeReaderArray<UChar_t> Muon_highPtId = {fReader, "Muon_highPtId"};
    // TTreeReaderArray<Bool_t> Muon_highPurity = {fReader, "Muon_highPurity"};
    TTreeReaderArray<Bool_t> Muon_inTimeMuon = {fReader, "Muon_inTimeMuon"};
    TTreeReaderArray<Bool_t> Muon_isGlobal = {fReader, "Muon_isGlobal"};
    TTreeReaderArray<Bool_t> Muon_isPFcand = {fReader, "Muon_isPFcand"};
    TTreeReaderArray<Bool_t> Muon_isTracker = {fReader, "Muon_isTracker"};
    TTreeReaderArray<Bool_t> Muon_looseId = {fReader, "Muon_looseId"};
    TTreeReaderArray<Bool_t> Muon_mediumId = {fReader, "Muon_mediumId"};
    TTreeReaderArray<Bool_t> Muon_mediumPromptId = {fReader, "Muon_mediumPromptId"};
    // TTreeReaderArray<UChar_t> Muon_miniIsoId = {fReader, "Muon_miniIsoId"};
    // TTreeReaderArray<UChar_t> Muon_multiIsoId = {fReader, "Muon_multiIsoId"};
    // TTreeReaderArray<UChar_t> Muon_mvaId = {fReader, "Muon_mvaId"};
    // TTreeReaderArray<UChar_t> Muon_pfIsoId = {fReader, "Muon_pfIsoId"};
    // TTreeReaderArray<Bool_t> Muon_softId = {fReader, "Muon_softId"};
    // TTreeReaderArray<Bool_t> Muon_softMvaId = {fReader, "Muon_softMvaId"};
    TTreeReaderArray<Bool_t> Muon_tightId = {fReader, "Muon_tightId"};
    // TTreeReaderArray<UChar_t> Muon_tkIsoId = {fReader, "Muon_tkIsoId"};
    // TTreeReaderArray<Bool_t> Muon_triggerIdLoose = {fReader, "Muon_triggerIdLoose"};
    // TTreeReaderValue<UInt_t> nPhoton = {fReader, "nPhoton"};
    // TTreeReaderArray<Float_t> Photon_eta = {fReader, "Photon_eta"};
    // TTreeReaderArray<Float_t> Photon_mass = {fReader, "Photon_mass"};
    // TTreeReaderArray<Float_t> Photon_mvaID = {fReader, "Photon_mvaID"};
    // TTreeReaderArray<Float_t> Photon_mvaID_Fall17V1p1 = {fReader, "Photon_mvaID_Fall17V1p1"};
    // TTreeReaderArray<Float_t> Photon_mvaID_Spring16nonTrigV1 = {fReader, "Photon_mvaID_Spring16nonTrigV1"};
    // TTreeReaderArray<Float_t> Photon_pfRelIso03_all = {fReader, "Photon_pfRelIso03_all"};
    // TTreeReaderArray<Float_t> Photon_pfRelIso03_chg = {fReader, "Photon_pfRelIso03_chg"};
    // TTreeReaderArray<Float_t> Photon_phi = {fReader, "Photon_phi"};
    // TTreeReaderArray<Float_t> Photon_pt = {fReader, "Photon_pt"};
    // TTreeReaderArray<Bool_t> Photon_electronVeto = {fReader, "Photon_electronVeto"};
    // TTreeReaderArray<Bool_t> Photon_mvaID_WP80 = {fReader, "Photon_mvaID_WP80"};
    // TTreeReaderArray<Bool_t> Photon_mvaID_WP90 = {fReader, "Photon_mvaID_WP90"};
    TTreeReaderValue<Float_t> Pileup_nTrueInt = {fReader, "Pileup_nTrueInt"};
    TTreeReaderValue<Int_t> Pileup_nPU = {fReader, "Pileup_nPU"};
    TTreeReaderValue<Float_t> PuppiMET_phi = {fReader, "PuppiMET_phi"};
    TTreeReaderValue<Float_t> PuppiMET_phiJERUp = {fReader, "PuppiMET_phiJERUp"};
    TTreeReaderValue<Float_t> PuppiMET_phiJESUp = {fReader, "PuppiMET_phiJESUp"};
    TTreeReaderValue<Float_t> PuppiMET_pt = {fReader, "PuppiMET_pt"};
    TTreeReaderValue<Float_t> PuppiMET_ptJERUp = {fReader, "PuppiMET_ptJERUp"};
    TTreeReaderValue<Float_t> PuppiMET_ptJESUp = {fReader, "PuppiMET_ptJESUp"};
    TTreeReaderValue<Float_t> PuppiMET_sumEt = {fReader, "PuppiMET_sumEt"};
    // TTreeReaderValue<UInt_t> nGenIsolatedPhoton = {fReader, "nGenIsolatedPhoton"};
    // TTreeReaderArray<Float_t> GenIsolatedPhoton_eta = {fReader, "GenIsolatedPhoton_eta"};
    // TTreeReaderArray<Float_t> GenIsolatedPhoton_mass = {fReader, "GenIsolatedPhoton_mass"};
    // TTreeReaderArray<Float_t> GenIsolatedPhoton_phi = {fReader, "GenIsolatedPhoton_phi"};
    // TTreeReaderArray<Float_t> GenIsolatedPhoton_pt = {fReader, "GenIsolatedPhoton_pt"};
    TTreeReaderValue<UInt_t> nTau = {fReader, "nTau"};
    // TTreeReaderArray<Float_t> Tau_chargedIso = {fReader, "Tau_chargedIso"};
    TTreeReaderArray<Float_t> Tau_dxy = {fReader, "Tau_dxy"};
    TTreeReaderArray<Float_t> Tau_dz = {fReader, "Tau_dz"};
    TTreeReaderArray<Float_t> Tau_eta = {fReader, "Tau_eta"};
    TTreeReaderArray<Float_t> Tau_leadTkDeltaEta = {fReader, "Tau_leadTkDeltaEta"};
    TTreeReaderArray<Float_t> Tau_leadTkDeltaPhi = {fReader, "Tau_leadTkDeltaPhi"};
    TTreeReaderArray<Float_t> Tau_leadTkPtOverTauPt = {fReader, "Tau_leadTkPtOverTauPt"};
    TTreeReaderArray<Float_t> Tau_mass = {fReader, "Tau_mass"};
    // TTreeReaderArray<Float_t> Tau_neutralIso = {fReader, "Tau_neutralIso"};
    TTreeReaderArray<Float_t> Tau_phi = {fReader, "Tau_phi"};
    // TTreeReaderArray<Float_t> Tau_photonsOutsideSignalCone = {fReader, "Tau_photonsOutsideSignalCone"};
    TTreeReaderArray<Float_t> Tau_pt = {fReader, "Tau_pt"};
    // TTreeReaderArray<Float_t> Tau_puCorr = {fReader, "Tau_puCorr"};
    // TTreeReaderArray<Float_t> Tau_rawAntiEle = {fReader, "Tau_rawAntiEle"};
    // TTreeReaderArray<Float_t> Tau_rawAntiEle2018 = {fReader, "Tau_rawAntiEle2018"};
    // TTreeReaderArray<Float_t> Tau_rawDeepTau2017v2p1VSe = {fReader, "Tau_rawDeepTau2017v2p1VSe"};
    // TTreeReaderArray<Float_t> Tau_rawDeepTau2017v2p1VSjet = {fReader, "Tau_rawDeepTau2017v2p1VSjet"};
    // TTreeReaderArray<Float_t> Tau_rawDeepTau2017v2p1VSmu = {fReader, "Tau_rawDeepTau2017v2p1VSmu"};
    // TTreeReaderArray<Float_t> Tau_rawIso = {fReader, "Tau_rawIso"};
    // TTreeReaderArray<Float_t> Tau_rawIsodR03 = {fReader, "Tau_rawIsodR03"};
    // TTreeReaderArray<Float_t> Tau_rawMVAnewDM2017v2 = {fReader, "Tau_rawMVAnewDM2017v2"};
    // TTreeReaderArray<Float_t> Tau_rawMVAoldDM = {fReader, "Tau_rawMVAoldDM"};
    // TTreeReaderArray<Float_t> Tau_rawMVAoldDM2017v1 = {fReader, "Tau_rawMVAoldDM2017v1"};
    // TTreeReaderArray<Float_t> Tau_rawMVAoldDM2017v2 = {fReader, "Tau_rawMVAoldDM2017v2"};
    // TTreeReaderArray<Float_t> Tau_rawMVAoldDMdR032017v2 = {fReader, "Tau_rawMVAoldDMdR032017v2"};
    TTreeReaderArray<Int_t> Tau_charge = {fReader, "Tau_charge"};
    TTreeReaderArray<Int_t> Tau_decayMode = {fReader, "Tau_decayMode"};
    // TTreeReaderArray<Int_t> Tau_jetIdx = {fReader, "Tau_jetIdx"};
    // TTreeReaderArray<Int_t> Tau_rawAntiEleCat = {fReader, "Tau_rawAntiEleCat"};
    // TTreeReaderArray<Int_t> Tau_rawAntiEleCat2018 = {fReader, "Tau_rawAntiEleCat2018"};
    TTreeReaderArray<UChar_t> Tau_idAntiEle = {fReader, "Tau_idAntiEle"};
    TTreeReaderArray<UChar_t> Tau_idAntiEle2018 = {fReader, "Tau_idAntiEle2018"};
    TTreeReaderArray<UChar_t> Tau_idAntiMu = {fReader, "Tau_idAntiMu"};
    TTreeReaderArray<Bool_t> Tau_idDecayMode = {fReader, "Tau_idDecayMode"};
    TTreeReaderArray<Bool_t> Tau_idDecayModeNewDMs = {fReader, "Tau_idDecayModeNewDMs"};
    TTreeReaderArray<UChar_t> Tau_idDeepTau2017v2p1VSe = {fReader, "Tau_idDeepTau2017v2p1VSe"};
    TTreeReaderArray<UChar_t> Tau_idDeepTau2017v2p1VSjet = {fReader, "Tau_idDeepTau2017v2p1VSjet"};
    TTreeReaderArray<UChar_t> Tau_idDeepTau2017v2p1VSmu = {fReader, "Tau_idDeepTau2017v2p1VSmu"};
    TTreeReaderArray<UChar_t> Tau_idMVAnewDM2017v2 = {fReader, "Tau_idMVAnewDM2017v2"};
    TTreeReaderArray<UChar_t> Tau_idMVAoldDM = {fReader, "Tau_idMVAoldDM"};
    TTreeReaderArray<UChar_t> Tau_idMVAoldDM2017v1 = {fReader, "Tau_idMVAoldDM2017v1"};
    TTreeReaderArray<UChar_t> Tau_idMVAoldDM2017v2 = {fReader, "Tau_idMVAoldDM2017v2"};
    TTreeReaderArray<UChar_t> Tau_idMVAoldDMdR032017v2 = {fReader, "Tau_idMVAoldDMdR032017v2"};
    TTreeReaderValue<UInt_t> nTrigObj = {fReader, "nTrigObj"};
    TTreeReaderArray<Float_t> TrigObj_pt = {fReader, "TrigObj_pt"};
    TTreeReaderArray<Float_t> TrigObj_eta = {fReader, "TrigObj_eta"};
    TTreeReaderArray<Float_t> TrigObj_phi = {fReader, "TrigObj_phi"};
    // TTreeReaderArray<Float_t> TrigObj_l1pt = {fReader, "TrigObj_l1pt"};
    // TTreeReaderArray<Float_t> TrigObj_l1pt_2 = {fReader, "TrigObj_l1pt_2"};
    // TTreeReaderArray<Float_t> TrigObj_l2pt = {fReader, "TrigObj_l2pt"};
    TTreeReaderArray<Int_t> TrigObj_id = {fReader, "TrigObj_id"};
    // TTreeReaderArray<Int_t> TrigObj_l1iso = {fReader, "TrigObj_l1iso"};
    // TTreeReaderArray<Int_t> TrigObj_l1charge = {fReader, "TrigObj_l1charge"};
    TTreeReaderArray<Int_t> TrigObj_filterBits = {fReader, "TrigObj_filterBits"};
    // TTreeReaderValue<Float_t> PV_ndof = {fReader, "PV_ndof"};
    // TTreeReaderValue<Float_t> PV_x = {fReader, "PV_x"};
    // TTreeReaderValue<Float_t> PV_y = {fReader, "PV_y"};
    // TTreeReaderValue<Float_t> PV_z = {fReader, "PV_z"};
    // TTreeReaderValue<Float_t> PV_chi2 = {fReader, "PV_chi2"};
    // TTreeReaderValue<Float_t> PV_score = {fReader, "PV_score"};
    // TTreeReaderValue<Int_t> PV_npvs = {fReader, "PV_npvs"};
    TTreeReaderValue<Int_t> PV_npvsGood = {fReader, "PV_npvsGood"};
    TTreeReaderArray<Int_t> Electron_genPartIdx = {fReader, "Electron_genPartIdx"};
    TTreeReaderArray<UChar_t> Electron_genPartFlav = {fReader, "Electron_genPartFlav"};
    TTreeReaderArray<Int_t> Jet_genJetIdx = {fReader, "Jet_genJetIdx"};
    TTreeReaderArray<Int_t> Jet_hadronFlavour = {fReader, "Jet_hadronFlavour"};
    TTreeReaderArray<Int_t> Jet_partonFlavour = {fReader, "Jet_partonFlavour"};
    TTreeReaderArray<Int_t> Muon_genPartIdx = {fReader, "Muon_genPartIdx"};
    TTreeReaderArray<UChar_t> Muon_genPartFlav = {fReader, "Muon_genPartFlav"};
    // TTreeReaderValue<Float_t> MET_fiducialGenPhi = {fReader, "MET_fiducialGenPhi"};
    // TTreeReaderValue<Float_t> MET_fiducialGenPt = {fReader, "MET_fiducialGenPt"};
    // TTreeReaderArray<UChar_t> Electron_cleanmask = {fReader, "Electron_cleanmask"};
    // TTreeReaderArray<UChar_t> Jet_cleanmask = {fReader, "Jet_cleanmask"};
    // TTreeReaderArray<UChar_t> Muon_cleanmask = {fReader, "Muon_cleanmask"};
    // TTreeReaderArray<UChar_t> Tau_cleanmask = {fReader, "Tau_cleanmask"};
    TTreeReaderArray<Int_t> Tau_genPartIdx = {fReader, "Tau_genPartIdx"};
    TTreeReaderArray<UChar_t> Tau_genPartFlav = {fReader, "Tau_genPartFlav"};
    // TTreeReaderValue<Bool_t> Flag_HBHENoiseFilter = {fReader, "Flag_HBHENoiseFilter"};
    // TTreeReaderValue<Bool_t> Flag_HBHENoiseIsoFilter = {fReader, "Flag_HBHENoiseIsoFilter"};
    // TTreeReaderValue<Bool_t> Flag_CSCTightHaloFilter = {fReader, "Flag_CSCTightHaloFilter"};
    // TTreeReaderValue<Bool_t> Flag_CSCTightHaloTrkMuUnvetoFilter = {fReader, "Flag_CSCTightHaloTrkMuUnvetoFilter"};
    // TTreeReaderValue<Bool_t> Flag_CSCTightHalo2015Filter = {fReader, "Flag_CSCTightHalo2015Filter"};
    // TTreeReaderValue<Bool_t> Flag_globalTightHalo2016Filter = {fReader, "Flag_globalTightHalo2016Filter"};
    // TTreeReaderValue<Bool_t> Flag_globalSuperTightHalo2016Filter = {fReader, "Flag_globalSuperTightHalo2016Filter"};
    // TTreeReaderValue<Bool_t> Flag_HcalStripHaloFilter = {fReader, "Flag_HcalStripHaloFilter"};
    // TTreeReaderValue<Bool_t> Flag_hcalLaserEventFilter = {fReader, "Flag_hcalLaserEventFilter"};
    // TTreeReaderValue<Bool_t> Flag_EcalDeadCellTriggerPrimitiveFilter = {fReader, "Flag_EcalDeadCellTriggerPrimitiveFilter"};
    // TTreeReaderValue<Bool_t> Flag_EcalDeadCellBoundaryEnergyFilter = {fReader, "Flag_EcalDeadCellBoundaryEnergyFilter"};
    // TTreeReaderValue<Bool_t> Flag_ecalBadCalibFilter = {fReader, "Flag_ecalBadCalibFilter"};
    // TTreeReaderValue<Bool_t> Flag_goodVertices = {fReader, "Flag_goodVertices"};
    // TTreeReaderValue<Bool_t> Flag_eeBadScFilter = {fReader, "Flag_eeBadScFilter"};
    // TTreeReaderValue<Bool_t> Flag_ecalLaserCorrFilter = {fReader, "Flag_ecalLaserCorrFilter"};
    // TTreeReaderValue<Bool_t> Flag_trkPOGFilters = {fReader, "Flag_trkPOGFilters"};
    // TTreeReaderValue<Bool_t> Flag_chargedHadronTrackResolutionFilter = {fReader, "Flag_chargedHadronTrackResolutionFilter"};
    // TTreeReaderValue<Bool_t> Flag_muonBadTrackFilter = {fReader, "Flag_muonBadTrackFilter"};
    // TTreeReaderValue<Bool_t> Flag_BadChargedCandidateFilter = {fReader, "Flag_BadChargedCandidateFilter"};
    // TTreeReaderValue<Bool_t> Flag_BadPFMuonFilter = {fReader, "Flag_BadPFMuonFilter"};
    // TTreeReaderValue<Bool_t> Flag_BadChargedCandidateSummer16Filter = {fReader, "Flag_BadChargedCandidateSummer16Filter"};
    // TTreeReaderValue<Bool_t> Flag_BadPFMuonSummer16Filter = {fReader, "Flag_BadPFMuonSummer16Filter"};
    // TTreeReaderValue<Bool_t> Flag_trkPOG_manystripclus53X = {fReader, "Flag_trkPOG_manystripclus53X"};
    // TTreeReaderValue<Bool_t> Flag_trkPOG_toomanystripclus53X = {fReader, "Flag_trkPOG_toomanystripclus53X"};
    // TTreeReaderValue<Bool_t> Flag_trkPOG_logErrorTooManyClusters = {fReader, "Flag_trkPOG_logErrorTooManyClusters"};
    TTreeReaderValue<Bool_t> Flag_METFilters = {fReader, "Flag_METFilters"};
    // TTreeReaderValue<Bool_t> HLT_Ele25_WPTight_Gsf = {fReader, "HLT_Ele25_WPTight_Gsf"};
    // TTreeReaderValue<Bool_t> HLT_Ele25_eta2p1_WPTight_Gsf = {fReader, "HLT_Ele25_eta2p1_WPTight_Gsf"};
    TTreeReaderValue<Bool_t> HLT_Ele27_WPTight_Gsf = {fReader, "HLT_Ele27_WPTight_Gsf"};
    // TTreeReaderValue<Bool_t> HLT_Ele27_WPTight_Gsf_L1JetTauSeeded = {fReader, "HLT_Ele27_WPTight_Gsf_L1JetTauSeeded"};
    // TTreeReaderValue<Bool_t> HLT_Ele27_eta2p1_WPTight_Gsf = {fReader, "HLT_Ele27_eta2p1_WPTight_Gsf"};
    // TTreeReaderValue<Bool_t> HLT_Ele30_WPTight_Gsf = {fReader, "HLT_Ele30_WPTight_Gsf"};
    // TTreeReaderValue<Bool_t> HLT_Ele30_eta2p1_WPTight_Gsf = {fReader, "HLT_Ele30_eta2p1_WPTight_Gsf"};
    TTreeReaderValue<Bool_t> HLT_Ele32_WPTight_Gsf = {fReader, "HLT_Ele32_WPTight_Gsf"};
    TTreeReaderValue<Bool_t> HLT_Ele32_WPTight_Gsf_L1DoubleEG = {fReader, "HLT_Ele32_WPTight_Gsf_L1DoubleEG"};
    // TTreeReaderValue<Bool_t> HLT_Ele32_eta2p1_WPTight_Gsf = {fReader, "HLT_Ele32_eta2p1_WPTight_Gsf"};
    // TTreeReaderValue<Bool_t> HLT_IsoMu16_eta2p1_MET30 = {fReader, "HLT_IsoMu16_eta2p1_MET30"};
    // TTreeReaderValue<Bool_t> HLT_IsoMu16_eta2p1_MET30_LooseIsoPFTau50_Trk30_eta2p1 = {fReader, "HLT_IsoMu16_eta2p1_MET30_LooseIsoPFTau50_Trk30_eta2p1"};
    // TTreeReaderValue<Bool_t> HLT_IsoMu17_eta2p1 = {fReader, "HLT_IsoMu17_eta2p1"};
    // TTreeReaderValue<Bool_t> HLT_IsoMu17_eta2p1_LooseIsoPFTau20 = {fReader, "HLT_IsoMu17_eta2p1_LooseIsoPFTau20"};
    // TTreeReaderValue<Bool_t> HLT_IsoMu17_eta2p1_LooseIsoPFTau20_SingleL1 = {fReader, "HLT_IsoMu17_eta2p1_LooseIsoPFTau20_SingleL1"};
    // TTreeReaderValue<Bool_t> HLT_IsoMu18 = {fReader, "HLT_IsoMu18"};
    // TTreeReaderValue<Bool_t> HLT_IsoMu19_eta2p1_LooseIsoPFTau20 = {fReader, "HLT_IsoMu19_eta2p1_LooseIsoPFTau20"};
    // TTreeReaderValue<Bool_t> HLT_IsoMu19_eta2p1_LooseIsoPFTau20_SingleL1 = {fReader, "HLT_IsoMu19_eta2p1_LooseIsoPFTau20_SingleL1"};
    // TTreeReaderValue<Bool_t> HLT_IsoMu19_eta2p1_MediumIsoPFTau32_Trk1_eta2p1_Reg = {fReader, "HLT_IsoMu19_eta2p1_MediumIsoPFTau32_Trk1_eta2p1_Reg"};
    // TTreeReaderValue<Bool_t> HLT_IsoMu19_eta2p1_LooseCombinedIsoPFTau20 = {fReader, "HLT_IsoMu19_eta2p1_LooseCombinedIsoPFTau20"};
    // TTreeReaderValue<Bool_t> HLT_IsoMu19_eta2p1_MediumCombinedIsoPFTau32_Trk1_eta2p1_Reg = {fReader, "HLT_IsoMu19_eta2p1_MediumCombinedIsoPFTau32_Trk1_eta2p1_Reg"};
    // TTreeReaderValue<Bool_t> HLT_IsoMu19_eta2p1_TightCombinedIsoPFTau32_Trk1_eta2p1_Reg = {fReader, "HLT_IsoMu19_eta2p1_TightCombinedIsoPFTau32_Trk1_eta2p1_Reg"};
    // TTreeReaderValue<Bool_t> HLT_IsoMu21_eta2p1_MediumCombinedIsoPFTau32_Trk1_eta2p1_Reg = {fReader, "HLT_IsoMu21_eta2p1_MediumCombinedIsoPFTau32_Trk1_eta2p1_Reg"};
    // TTreeReaderValue<Bool_t> HLT_IsoMu21_eta2p1_TightCombinedIsoPFTau32_Trk1_eta2p1_Reg = {fReader, "HLT_IsoMu21_eta2p1_TightCombinedIsoPFTau32_Trk1_eta2p1_Reg"};
    // TTreeReaderValue<Bool_t> HLT_IsoMu20 = {fReader, "HLT_IsoMu20"};
    // TTreeReaderValue<Bool_t> HLT_IsoMu21_eta2p1_LooseIsoPFTau20_SingleL1 = {fReader, "HLT_IsoMu21_eta2p1_LooseIsoPFTau20_SingleL1"};
    // TTreeReaderValue<Bool_t> HLT_IsoMu21_eta2p1_LooseIsoPFTau50_Trk30_eta2p1_SingleL1 = {fReader, "HLT_IsoMu21_eta2p1_LooseIsoPFTau50_Trk30_eta2p1_SingleL1"};
    // TTreeReaderValue<Bool_t> HLT_IsoMu21_eta2p1_MediumIsoPFTau32_Trk1_eta2p1_Reg = {fReader, "HLT_IsoMu21_eta2p1_MediumIsoPFTau32_Trk1_eta2p1_Reg"};
    // TTreeReaderValue<Bool_t> HLT_IsoMu22 = {fReader, "HLT_IsoMu22"};
    // TTreeReaderValue<Bool_t> HLT_IsoMu22_eta2p1 = {fReader, "HLT_IsoMu22_eta2p1"};
    TTreeReaderValue<Bool_t> HLT_IsoMu24 = {fReader, "HLT_IsoMu24"};
    TTreeReaderValue<Bool_t> HLT_IsoMu27 = {fReader, "HLT_IsoMu27"};
    TTreeReaderValue<Bool_t> HLT_Mu50 = {fReader, "HLT_Mu50"};
    TTreeReaderValue<Float_t> PrefireWeight = {fReader, "PrefireWeight"};
    TTreeReaderValue<Float_t> PrefireWeight_Up = {fReader, "PrefireWeight_Up"};
    TTreeReaderValue<Float_t> PrefireWeight_Down = {fReader, "PrefireWeight_Down"};
    TTreeReaderArray<Bool_t> Muon_TaggedAsRemovedByTau = {fReader, "Muon_TaggedAsRemovedByTau"};
    TTreeReaderArray<Bool_t> Tau_TaggedAsRemovedByMuon = {fReader, "Tau_TaggedAsRemovedByMuon"};
    TTreeReaderArray<Int_t> Muon_TauOverlapIdx = {fReader, "Muon_TauOverlapIdx"};
    TTreeReaderArray<Int_t> Tau_MuonOverlapIdx = {fReader, "Tau_MuonOverlapIdx"};
    TTreeReaderArray<Bool_t> Electron_TaggedAsRemovedByTau = {fReader, "Electron_TaggedAsRemovedByTau"};
    TTreeReaderArray<Bool_t> Tau_TaggedAsRemovedByElectron = {fReader, "Tau_TaggedAsRemovedByElectron"};
    TTreeReaderArray<Int_t> Electron_TauOverlapIdx = {fReader, "Electron_TauOverlapIdx"};
    TTreeReaderArray<Int_t> Tau_ElectronOverlapIdx = {fReader, "Tau_ElectronOverlapIdx"};
    TTreeReaderValue<Bool_t> SelectionFilter_MuTau = {fReader, "SelectionFilter_MuTau"};
    TTreeReaderValue<Bool_t> SelectionFilter_ETau = {fReader, "SelectionFilter_ETau"};
    TTreeReaderValue<Bool_t> SelectionFilter_EMu = {fReader, "SelectionFilter_EMu"};
    TTreeReaderValue<Bool_t> SelectionFilter_MuMu = {fReader, "SelectionFilter_MuMu"};
    TTreeReaderValue<Bool_t> SelectionFilter_EE = {fReader, "SelectionFilter_EE"};
    TTreeReaderValue<Float_t> SelectionFilter_LepM = {fReader, "SelectionFilter_LepM"};
    TTreeReaderArray<Float_t> Muon_corrected_pt = {fReader, "Muon_corrected_pt"};
    TTreeReaderArray<Float_t> Muon_correctedUp_pt = {fReader, "Muon_correctedUp_pt"};
    TTreeReaderArray<Float_t> Muon_correctedDown_pt = {fReader, "Muon_correctedDown_pt"};
    TTreeReaderArray<Float_t> Jet_pt_raw = {fReader, "Jet_pt_raw"};
    TTreeReaderArray<Float_t> Jet_pt_nom = {fReader, "Jet_pt_nom"};
    TTreeReaderArray<Float_t> Jet_mass_raw = {fReader, "Jet_mass_raw"};
    TTreeReaderArray<Float_t> Jet_mass_nom = {fReader, "Jet_mass_nom"};
    TTreeReaderArray<Float_t> Jet_corr_JEC = {fReader, "Jet_corr_JEC"};
    TTreeReaderArray<Float_t> Jet_corr_JER = {fReader, "Jet_corr_JER"};
    TTreeReaderValue<Float_t> MET_pt_nom = {fReader, "MET_pt_nom"};
    TTreeReaderValue<Float_t> MET_phi_nom = {fReader, "MET_phi_nom"};
    TTreeReaderValue<Float_t> MET_pt_jer = {fReader, "MET_pt_jer"};
    TTreeReaderValue<Float_t> MET_phi_jer = {fReader, "MET_phi_jer"};
    TTreeReaderArray<Float_t> Jet_pt_jerUp = {fReader, "Jet_pt_jerUp"};
    TTreeReaderArray<Float_t> Jet_mass_jerUp = {fReader, "Jet_mass_jerUp"};
    TTreeReaderValue<Float_t> MET_pt_jerUp = {fReader, "MET_pt_jerUp"};
    TTreeReaderValue<Float_t> MET_phi_jerUp = {fReader, "MET_phi_jerUp"};
    TTreeReaderArray<Float_t> Jet_pt_jesTotalUp = {fReader, "Jet_pt_jesTotalUp"};
    TTreeReaderArray<Float_t> Jet_mass_jesTotalUp = {fReader, "Jet_mass_jesTotalUp"};
    TTreeReaderValue<Float_t> MET_pt_jesTotalUp = {fReader, "MET_pt_jesTotalUp"};
    TTreeReaderValue<Float_t> MET_phi_jesTotalUp = {fReader, "MET_phi_jesTotalUp"};
    TTreeReaderValue<Float_t> MET_pt_unclustEnUp = {fReader, "MET_pt_unclustEnUp"};
    TTreeReaderValue<Float_t> MET_phi_unclustEnUp = {fReader, "MET_phi_unclustEnUp"};
    TTreeReaderArray<Float_t> Jet_pt_jerDown = {fReader, "Jet_pt_jerDown"};
    TTreeReaderArray<Float_t> Jet_mass_jerDown = {fReader, "Jet_mass_jerDown"};
    TTreeReaderValue<Float_t> MET_pt_jerDown = {fReader, "MET_pt_jerDown"};
    TTreeReaderValue<Float_t> MET_phi_jerDown = {fReader, "MET_phi_jerDown"};
    TTreeReaderArray<Float_t> Jet_pt_jesTotalDown = {fReader, "Jet_pt_jesTotalDown"};
    TTreeReaderArray<Float_t> Jet_mass_jesTotalDown = {fReader, "Jet_mass_jesTotalDown"};
    TTreeReaderValue<Float_t> MET_pt_jesTotalDown = {fReader, "MET_pt_jesTotalDown"};
    TTreeReaderValue<Float_t> MET_phi_jesTotalDown = {fReader, "MET_phi_jesTotalDown"};
    TTreeReaderValue<Float_t> MET_pt_unclustEnDown = {fReader, "MET_pt_unclustEnDown"};
    TTreeReaderValue<Float_t> MET_phi_unclustEnDown = {fReader, "MET_phi_unclustEnDown"};
    TTreeReaderArray<Bool_t> Muon_TaggedAsRemovedByJet = {fReader, "Muon_TaggedAsRemovedByJet"};
    TTreeReaderArray<Bool_t> Jet_TaggedAsRemovedByMuon = {fReader, "Jet_TaggedAsRemovedByMuon"};
    TTreeReaderArray<Int_t> Muon_JetOverlapIdx = {fReader, "Muon_JetOverlapIdx"};
    TTreeReaderArray<Int_t> Jet_MuonOverlapIdx = {fReader, "Jet_MuonOverlapIdx"};
    TTreeReaderArray<Bool_t> Electron_TaggedAsRemovedByJet = {fReader, "Electron_TaggedAsRemovedByJet"};
    TTreeReaderArray<Bool_t> Jet_TaggedAsRemovedByElectron = {fReader, "Jet_TaggedAsRemovedByElectron"};
    TTreeReaderArray<Int_t> Electron_JetOverlapIdx = {fReader, "Electron_JetOverlapIdx"};
    TTreeReaderArray<Int_t> Jet_ElectronOverlapIdx = {fReader, "Jet_ElectronOverlapIdx"};
    TTreeReaderArray<Bool_t> Tau_TaggedAsRemovedByJet = {fReader, "Tau_TaggedAsRemovedByJet"};
    TTreeReaderArray<Bool_t> Jet_TaggedAsRemovedByTau = {fReader, "Jet_TaggedAsRemovedByTau"};
    TTreeReaderArray<Int_t> Tau_JetOverlapIdx = {fReader, "Tau_JetOverlapIdx"};
    TTreeReaderArray<Int_t> Jet_TauOverlapIdx = {fReader, "Jet_TauOverlapIdx"};
    TTreeReaderValue<Float_t> JetPUIDWeight = {fReader, "JetPUIDWeight"};
    TTreeReaderValue<Int_t> nBJetMedium = {fReader, "nBJetMedium"};
    TTreeReaderValue<Int_t> nBJetTight = {fReader, "nBJetTight"};
    TTreeReaderValue<UInt_t> nBJet = {fReader, "nBJet"};
    TTreeReaderArray<Float_t> BJet_pt = {fReader, "BJet_pt"};
    TTreeReaderArray<Float_t> BJet_eta = {fReader, "BJet_eta"};
    TTreeReaderArray<Float_t> BJet_phi = {fReader, "BJet_phi"};
    TTreeReaderArray<Float_t> BJet_btagDeepB = {fReader, "BJet_btagDeepB"};
    TTreeReaderArray<Float_t> BJet_btagDeepC = {fReader, "BJet_btagDeepC"};
    TTreeReaderArray<Float_t> BJet_partonFlavour = {fReader, "BJet_partonFlavour"};
    TTreeReaderArray<Int_t> BJet_idx = {fReader, "BJet_idx"};
    TTreeReaderArray<Int_t> BJet_WPID = {fReader, "BJet_WPID"};
    TTreeReaderValue<Float_t> HT = {fReader, "HT"};
    TTreeReaderArray<Float_t> Jet_btagSF_deepcsv_L_down = {fReader, "Jet_btagSF_deepcsv_L_down"};
    TTreeReaderArray<Float_t> Jet_btagSF_deepcsv_L = {fReader, "Jet_btagSF_deepcsv_L"};
    TTreeReaderArray<Float_t> Jet_btagSF_deepcsv_L_up = {fReader, "Jet_btagSF_deepcsv_L_up"};
    TTreeReaderArray<Float_t> Jet_btagSF_deepcsv_T_down = {fReader, "Jet_btagSF_deepcsv_T_down"};
    TTreeReaderArray<Float_t> Jet_btagSF_deepcsv_T = {fReader, "Jet_btagSF_deepcsv_T"};
    TTreeReaderArray<Float_t> Jet_btagSF_deepcsv_T_up = {fReader, "Jet_btagSF_deepcsv_T_up"};
    TTreeReaderValue<Int_t> GenZll_Idx = {fReader, "GenZll_Idx"};
    TTreeReaderValue<Int_t> GenZll_LepOne_Idx = {fReader, "GenZll_LepOne_Idx"};
    TTreeReaderValue<Int_t> GenZll_LepTwo_Idx = {fReader, "GenZll_LepTwo_Idx"};
    TTreeReaderValue<Int_t> GenZll_LepOne_DecayIdx = {fReader, "GenZll_LepOne_DecayIdx"};
    TTreeReaderValue<Int_t> GenZll_LepTwo_DecayIdx = {fReader, "GenZll_LepTwo_DecayIdx"};
    TTreeReaderValue<Float_t> GenZll_pt = {fReader, "GenZll_pt"};
    TTreeReaderValue<Float_t> GenZll_LepOne_pt = {fReader, "GenZll_LepOne_pt"};
    TTreeReaderValue<Float_t> GenZll_LepTwo_pt = {fReader, "GenZll_LepTwo_pt"};
    TTreeReaderValue<Float_t> GenZll_eta = {fReader, "GenZll_eta"};
    TTreeReaderValue<Float_t> GenZll_LepOne_eta = {fReader, "GenZll_LepOne_eta"};
    TTreeReaderValue<Float_t> GenZll_LepTwo_eta = {fReader, "GenZll_LepTwo_eta"};
    TTreeReaderValue<Float_t> GenZll_phi = {fReader, "GenZll_phi"};
    TTreeReaderValue<Float_t> GenZll_LepOne_phi = {fReader, "GenZll_LepOne_phi"};
    TTreeReaderValue<Float_t> GenZll_LepTwo_phi = {fReader, "GenZll_LepTwo_phi"};
    TTreeReaderValue<Float_t> GenZll_mass = {fReader, "GenZll_mass"};
    TTreeReaderValue<Float_t> GenZll_LepOne_mass = {fReader, "GenZll_LepOne_mass"};
    TTreeReaderValue<Float_t> GenZll_LepTwo_mass = {fReader, "GenZll_LepTwo_mass"};
    TTreeReaderValue<Float_t> GenZll_pdgId = {fReader, "GenZll_pdgId"};
    TTreeReaderValue<Float_t> GenZll_LepOne_pdgId = {fReader, "GenZll_LepOne_pdgId"};
    TTreeReaderValue<Float_t> GenZll_LepTwo_pdgId = {fReader, "GenZll_LepTwo_pdgId"};
    TTreeReaderValue<Int_t> nGenTau = {fReader, "nGenTau"};
    TTreeReaderValue<Int_t> nGenMuon = {fReader, "nGenMuon"};
    TTreeReaderValue<Int_t> nGenElectron = {fReader, "nGenElectron"};
    TTreeReaderValue<Float_t> puWeight = {fReader, "puWeight"};
    TTreeReaderValue<Float_t> puWeightUp = {fReader, "puWeightUp"};
    TTreeReaderValue<Float_t> puWeightDown = {fReader, "puWeightDown"};


    BaseSelector(TTree * /*tree*/ =0) { }
    virtual ~BaseSelector() { }
    virtual Int_t   Version() const { return 2; }
    virtual void    Begin(TTree *tree);
    virtual void    SlaveBegin(TTree *tree);
    virtual void    Init(TTree *tree);
    virtual Bool_t  Notify();
    virtual Bool_t  Process(Long64_t entry);
    virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
    virtual void    SetOption(const char *option) { fOption = option; }
    virtual void    SetObject(TObject *obj) { fObject = obj; }
    virtual void    SetInputList(TList *input) { fInput = input; }
    virtual TList  *GetOutputList() const { return fOutput; }
    virtual void    SlaveTerminate();
    virtual void    Terminate();

    ClassDef(BaseSelector,0);

  };
}
#endif

#ifdef BaseSelector_cxx
using namespace CLFV;
void BaseSelector::Init(TTree *tree)
{
  // The Init() function is called when the selector needs to initialize
  // a new tree or chain. Typically here the reader is initialized.
  // It is normally not necessary to make changes to the generated
  // code, but the routine can be extended by the user if needed.
  // Init() will be called many times when running on PROOF
  // (once per file to be processed).
  printf("BaseSelector::%s: Setting up reader\n", __func__);
  fChain = tree;
  fReader.SetTree(tree);
}

Bool_t BaseSelector::Notify()
{
  // The Notify() function is called when a new file is opened. This
  // can be either for a new TTree in a TChain or when when a new TTree
  // is started when using PROOF. It is normally not necessary to make changes
  // to the generated code, but the routine can be extended by the
  // user if needed. The return value is currently not used.

  return kTRUE;
}


#endif // #ifdef BaseSelector_cxx
