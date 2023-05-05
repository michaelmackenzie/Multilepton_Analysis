import ROOT as rt
from array import array
import math


def MT(pT1, phi1, pT2, phi2):
  Dphi= phi1-phi2
  if (Dphi>3.14): Dphi -= 6.28
  if (Dphi<-3.14): Dphi += 6.28
  return math.sqrt(2*pT1*pT2*(1- math.cos(Dphi)))





cc = rt.TChain("Events")

#dy
#cc.Add("/eos/cms/store/cmst3/group/bpark/gkaratha/Nano_BkgDY50_2022Jun24_RunUL_v3p3/DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8/crab_Nano_BkgDY50_2022Jun24_RunUL_v3p3/220624_141436/0000/*.root")
#TT
#cc.Add("/eos/cms/store/cmst3/group/bpark/gkaratha/Nano_BkgTT2L_2022Jun24_RunUL_v3p3/TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8/crab_Nano_BkgTT2L_2022Jun24_RunUL_v3p3/220624_141305/lower_stats/*.root")
#WW
#cc.Add("/eos/cms/store/cmst3/group/bpark/gkaratha/Nano_BkgWW2L_2022Jun24_RunUL_v3p3/WWTo2L2Nu_TuneCP5_13TeV-powheg-pythia8/crab_Nano_BkgWW2L_2022Jun24_RunUL_v3p3/220624_141455/0000/*.root")
#TW
#cc.Add("/eos/cms/store/cmst3/group/bpark/gkaratha/Nano_BkgTW_2022Jun24_RunUL_v3p3/ST_tW_top_5f_inclusiveDecays_TuneCP5_13TeV-powheg-pythia8/crab_Nano_BkgTW_2022Jun24_RunUL_v3p3/220624_141327/0000/*.root")
#signal
########test###########
cc.Add("/eos/cms/store/cmst3/group/bpark/gkaratha/Nano_acc_SgnZMuE_2022Jul05_unbiased_v3p3/LFVAnalysis_ZEMu_2018_10218V1/crab_Nano_acc_SgnZMuE_2022Jul05_unbiased_v3p3/220705_121912/0000/*.root")
#######train#######
#cc.Add("/eos/cms/store/cmst3/group/bpark/gkaratha/Nano_SgnZMuE2_2022Jun24_RunUL_v3p3/LFVAnalysis_ZEMu_2017_934V2/crab_Nano_SgnZMuE2_2022Jun24_RunUL_v3p3/220624_141832/0000/*.root")
#cc.Add("/eos/cms/store/cmst3/group/bpark/gkaratha/Nano_SgnZMuE3_2022Jun24_RunUL_v3p3/LFVAnalysis_ZEMu_2017_934V1/crab_Nano_SgnZMuE3_2022Jun24_RunUL_v3p3/220624_141850/0000/*.root")
#cc.Add("/eos/cms/store/cmst3/group/bpark/gkaratha/Nano_SgnZMuE4_2022Jun24_RunUL_v3p3/ZEMuAnalysis_10218V2/crab_Nano_SgnZMuE4_2022Jun24_RunUL_v3p3/220624_141908/0000/*.root")
#data
#cc.Add("/eos/cms/store/cmst3/group/bpark/gkaratha/Nano_ULRun2018B_2022Jun24_RunUL_v3p3/SingleMuon/crab_Nano_ULRun2018B_2022Jun24_RunUL_v3p3/220624_140207/0000/*.root")
#cc.Add("/eos/cms/store/cmst3/group/bpark/gkaratha/Nano_ULRun2018C_2022Jun25_RunUL_v3p3/SingleMuon/crab_Nano_ULRun2018C_2022Jun25_RunUL_v3p3/220624_222615/0000/*.root")
#cc.Add("/eos/cms/store/cmst3/group/bpark/gkaratha/Nano_ULRun2018D_2022Jun25_RunUL_v3p3/SingleMuon/crab_Nano_ULRun2018D_2022Jun25_RunUL_v3p3/220624_222653/0000/*.root")



name="v1_test"
#DY= 75% train 25% test or 180k train 60k test
#TT= 239400 train 79800 test (train stop at 239400 test start at 239400 end 319200)
#WW= 414000 train 138000 test (train stop at 414000 test start 414000 end 552000)
#TW= 59400 train 19800 test (train stop 59400 test start 59400 end 79200)
#Signal= 0.75 train 0.25 test
start_from_fraction=-0.75
stop_at_fraction=-1.0 #DY 0.75 #TT=-1
  
start_from_evt=-1
stop_at_evt=-1

mass_cut=False



print cc.GetEntries()

fout = rt.TFile("bdt_tree_"+name+".root","RECREATE")
outtree = rt.TTree("mytree","mytree")

met = array('f',[0]); met_phi = array('f',[0]); 
pt_mu = array('f',[0]); eta_mu = array('f',[0]); phi_mu = array('f',[0])
pt_e = array('f',[0]); eta_e = array('f',[0]); phi_e = array('f',[0])
pt_j1 = array('f',[0]);
ht = array('f',[0]); njets = array('f',[0]);
mt_mu = array('f',[0]); mt_e = array('f',[0]);
pt_ll = array('f',[0]); eta_ll = array('f',[0]); phi_ll = array('f',[0]); mass_ll = array('f',[0])
deta_ll = array('f',[0]); ratio_met_ptll= array('f',[0]); ratio_met_ht= array('f',[0])
dphill_met= array('f',[0]); massll_met= array('f',[0]); met_significance=array('f',[0])
st = array('f',[0]); ratio_pte_ptmu = array('f',[0]);

for name,branch in zip( ["met","met_phi","pt_mu","eta_mu","phi_mu","pt_e","eta_e","phi_e","pt_j1","ht","njets","mt_mu","mt_e","pt_ll","eta_ll","phi_ll","mass_ll","deta_ll","ratio_met_ptll","ratio_met_ht", "dphill_met", "massll_met", "met_significance","st","ratio_pte_ptmu"], [met,met_phi,pt_mu,eta_mu,phi_mu,pt_e,eta_e,phi_e,pt_j1,ht,njets,mt_mu,mt_e,pt_ll,eta_ll,phi_ll,mass_ll,deta_ll,ratio_met_ptll,ratio_met_ht, dphill_met, massll_met, met_significance,st,ratio_pte_ptmu] ):
  outtree.Branch(name,branch,name+"/F")

ievt=-1;
for evt in cc:
  ievt+=1
  if ievt%10000==0: print ievt
#  if ievt<cc.GetEntries()*0.75: continue
  if stop_at_evt>-1 and ievt>stop_at_evt: break
  if start_from_evt>-1 and ievt<start_from_evt: continue
  if stop_at_fraction>-1 and ievt>cc.GetEntries()*stop_at_fraction: break
  if start_from_fraction>-1 and ievt<cc.GetEntries()*start_from_fraction: continue
  if not evt.HLT_IsoMu24: continue
  if evt.nMuon==0 or evt.nElectron==0: continue
  if evt.Electron_charge[0]==evt.Muon_charge[0]: continue
  if evt.Electron_pt[0]<20 or evt.Muon_pt[0]<25: continue
  if abs(evt.Electron_eta[0])>2.5 or abs(evt.Muon_eta[0])>2.4: continue
  if evt.nBJet>0: continue
  if (not evt.Muon_mediumPromptId[0]) or evt.Muon_pfRelIso04_all[0]>0.15 or abs(evt.Muon_dxy[0])/abs(evt.Muon_dxyErr[0])>3.0 or abs(evt.Muon_dz[0])/evt.Muon_dzErr[0]>4.7: continue
  if (not evt.Electron_mvaFall17V2noIso_WP90[0]) or evt.Electron_pfRelIso03_all[0]>0.15 or abs(evt.Electron_dxy[0])>0.2 or abs(evt.Electron_dz[0])>0.5 or abs(evt.Electron_dxy[0])/evt.Electron_dxyErr[0]>3.0 or abs(evt.Electron_dz[0])/evt.Electron_dzErr[0]>4.7: continue
  vec_mu = rt.TLorentzVector()
  vec_e = rt.TLorentzVector()
  vec_mu.SetPtEtaPhiM(evt.Muon_pt[0],evt.Muon_eta[0], evt.Muon_phi[0],0.105)
  vec_e.SetPtEtaPhiM(evt.Electron_pt[0],evt.Electron_eta[0], evt.Electron_phi[0],0.000511)
  if ( (vec_mu+vec_e).M()<80 or (vec_mu+vec_e).M()>100) and mass_cut: 
     continue
  if (vec_mu+vec_e).M()<70 or (vec_mu+vec_e).M()>110:
     continue
  ht[0]=0
  met[0] = evt.PuppiMET_pt;  met_phi[0] = evt.PuppiMET_phi
  pt_mu[0] = vec_mu.Pt(); eta_mu[0] = vec_mu.Eta(); phi_mu[0] = vec_mu.Phi()
  pt_e[0] = vec_e.Pt(); eta_e[0] = vec_e.Eta(); phi_e[0] = vec_e.Phi()
  #ht[0] = evt.HT;  njets[0] = evt.nJet
  for ijet in range(evt.nJet):
    vjet = rt.TLorentzVector ()
    vjet.SetPtEtaPhiM(evt.Jet_pt[ijet],evt.Jet_eta[ijet],evt.Jet_phi[ijet],0)
    if vjet.DeltaR(vec_mu)>0.3 and vjet.DeltaR(vec_e)>0.3:
       ht[0]+=vjet.Pt()
       njets[0]+=1
  mt_mu[0] = MT(pt_mu[0], phi_mu[0], met[0], met_phi[0])
  mt_e[0] = MT(pt_e[0], phi_e[0], met[0], met_phi[0])
  pt_ll[0] = (vec_mu+vec_e).Pt(); eta_ll[0] = (vec_mu+vec_e).Eta(); phi_ll[0] = (vec_mu+vec_e).Phi(); mass_ll[0] = (vec_mu+vec_e).M()
  deta_ll[0] = abs(vec_mu.Eta()-vec_e.Eta())
  ratio_met_ptll[0]=evt.PuppiMET_pt/(vec_mu+vec_e).Pt(); 
  if ht[0]>0:
    ratio_met_ht[0]= evt.PuppiMET_pt/ht[0]
  else:
     ratio_met_ht[0]=0.
  vec_met=rt.TLorentzVector()
  vec_met.SetPtEtaPhiM(evt.PuppiMET_pt,0,evt.PuppiMET_phi,0)
  dphill_met[0]=(vec_mu+vec_e).DeltaPhi(vec_met)
  massll_met[0]= (vec_mu+vec_e+vec_met).M()
  met_significance[0]=evt.MET_significance
  st[0] = ht[0]+met[0]+pt_mu[0]+pt_e[0]
  ratio_pte_ptmu[0]=pt_e[0]/pt_mu[0]
  if evt.nJet==0: 
     pt_j1[0]=0
  else:
     pt_j1[0]=evt.Jet_pt[0]
  outtree.Fill()

outtree.Write()
fout.Close()
