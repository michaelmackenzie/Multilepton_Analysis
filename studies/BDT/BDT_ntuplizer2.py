import ROOT as rt
from array import array
import math
import argparse





def MT(pT1, phi1, pT2, phi2):
  Dphi= phi1-phi2
  if (Dphi>3.14): Dphi -= 6.28
  if (Dphi<-3.14): Dphi += 6.28
  return math.sqrt(2*pT1*pT2*(1- math.cos(Dphi)))

class lepton:
  def __init__ (self, pt, eta, phi, mass, charge, ID, iso, dxy, dxyErr, dz, dzErr, idx):
     self.pt = pt[idx]
     self.eta = eta[idx]
     self.phi = phi[idx]
     self.mass = mass
     self.charge = charge[idx]
     self.ID=ID[idx]
     self.iso=iso[idx]
     self.dxy=abs(dxy[idx])
     self.dz=abs(dz[idx])
     if dxyErr[idx]>0:
       self.sdxy=abs(dxy[idx])/dxyErr[idx]
     else:
       self.sdxy=0
     if dzErr[idx]>0:
       self.sdz=abs(dz[idx])/dzErr[idx]
     else:
       self.sdz=0
     
   

input_files_mc={
"emu":"/eos/cms/store/cmst3/group/bpark/gkaratha/Nano_acc_SgnZMuE_2022Jul05_unbiased_v3p3/LFVAnalysis_ZEMu_2018_10218V1/crab_Nano_acc_SgnZMuE_2022Jul05_unbiased_v3p3/220705_121912/0000/*.root",
"mumu":"/eos/cms/store/cmst3/group/bpark/gkaratha/Nano_acc_BkgDY50_mumu_2022Jul05_unbiased_v3p3/DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8/crab_Nano_acc_BkgDY50_mumu_2022Jul05_unbiased_v3p3/220705_121704/0000/*.root",
"ee":"/eos/cms/store/cmst3/group/bpark/gkaratha/Nano_acc_BkgDY50_ee_2022Jul05_unbiased_v3p3/DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8/crab_Nano_acc_BkgDY50_ee_2022Jul05_unbiased_v3p3/220705_121817/0000/*.root"

}






if __name__ == "__main__":

   

   parser = argparse.ArgumentParser()
   parser.add_argument("-o", dest="name",default="test", type=str,help="output root name")
   parser.add_argument("--dilep", dest="dilep",default="emu", type=str,help="flavour of leptons; options: ee emu mumu")
#   parser.add_argument("--gen-match", dest="gen_match",default=False, action='store_true',help="whether to gen mstch with DR<0.08")
   parser.add_argument("--maxEvt", dest="maxEvt",default=-1, type=int,help="Max events to run")
   args, unknown = parser.parse_known_args()


   cc = rt.TChain("Events")   

   cc.Add(input_files_mc[args.dilep])
   
   
   #DY= 75% train 25% test or 180k train 60k test
   #TT= 239400 train 79800 test (train stop at 239400 test start at 239400 end 319200)
   #WW= 414000 train 138000 test (train stop at 414000 test start 414000 end 552000)
   #TW= 59400 train 19800 test (train stop 59400 test start 59400 end 79200)
   #Signal= 0.75 train 0.25 test
   start_from_fraction=-0.75
   stop_at_fraction=-1.0 #DY 0.75 #TT=-1
     
   start_from_evt=-1
   stop_at_evt=-1
   
   mass_cut=None #options: None(no "") "loose"(70-110) "tight" (80-100)
   
   triggers=["HLT_IsoMu24","HLT_Ele32_WPTight_Gsf"]
   
   #triggers=["HLT_IsoMu24"]
   lep1="Muon"
   lep2="Electron"
   
   Iso_cut=0.15
   sDxy_cut=3.0
   sDz_cut=4.7
   Pt2_cut=20
   Pt1_cut=[25,34] #follows the trigger ordering
   Eta1_cut=2.4
   Eta2_cut=2.5
   OrderLep_difFlavour=True
   GenMatch=True
   
   if args.dilep=="ee":
      lep1="Electron"
      triggers=["HLT_Ele32_WPTight_Gsf"]   
      Pt1_cut=[34]
      Eta1_cut=2.5

   if args.dilep=="mumu": 
      lep2="Muon"
      triggers=["HLT_IsoMu24"]   
      Pt1_cut=[25]
      Eta2_cut=2.4

   print cc.GetEntries()
   
   fout = rt.TFile("bdt_tree_v2_"+args.dilep+"_"+args.name+".root","RECREATE")
   outtree = rt.TTree("mytree","mytree")
   
   
   
   met = array('f',[0]); met_phi = array('f',[0]); 
   pt_l1 = array('f',[0]); eta_l1 = array('f',[0]); phi_l1 = array('f',[0])
   mass_l1 = array('f',[0])
   pt_l2 = array('f',[0]); eta_l2 = array('f',[0]); phi_l2 = array('f',[0])
   mass_l2 = array('f',[0])
   pt_j1 = array('f',[0]);
   ht = array('f',[0]); njets = array('f',[0]);
   mt_l1 = array('f',[0]); mt_l2 = array('f',[0]);
   pt_ll = array('f',[0]); eta_ll = array('f',[0]); phi_ll = array('f',[0]); 
   mass_ll = array('f',[0])
   ratio_met_ptll= array('f',[0]); ratio_met_ht= array('f',[0])
   met_significance=array('f',[0])
   st = array('f',[0]); ratio_ptl2_ptl1 = array('f',[0]);
   
   for name,branch in zip( ["met","met_phi","pt_l1","eta_l1","phi_l1","pt_l2","eta_l2","phi_l2","pt_j1","ht","njets","mt_l1","mt_l2","pt_ll","eta_ll","phi_ll","mass_ll","ratio_met_ptll","ratio_met_ht", "met_significance","st","ratio_ptl2_ptl1","mass_l1","mass_l2"], [met,met_phi,pt_l1,eta_l1,phi_l1,pt_l2,eta_l2,phi_l2,pt_j1,ht,njets,mt_l1,mt_l2,pt_ll,eta_ll,phi_ll,mass_ll,ratio_met_ptll,ratio_met_ht, met_significance,st,ratio_ptl2_ptl1,mass_l1,mass_l2] ):
     outtree.Branch(name,branch,name+"/F")
   
   ievt=-1;
   for evt in cc:
     ievt+=1
     if ievt%10000==0: print ievt
     if args.maxEvt>0 and args.maxEvt<ievt:break
     if stop_at_evt>-1 and ievt>stop_at_evt: break
     if start_from_evt>-1 and ievt<start_from_evt: continue
     if stop_at_fraction>-1 and ievt>cc.GetEntries()*stop_at_fraction: break
     if start_from_fraction>-1 and ievt<cc.GetEntries()*start_from_fraction: continue
     Fired = False
     for trg in triggers:
       if getattr(evt,trg):
          Fired=True
     if not Fired: continue
     if lep1==lep2 and getattr(evt,"n"+lep1)<2:
        continue
     elif lep1!=lep2 and (getattr(evt,"n"+lep1)<1 or getattr(evt,"n"+lep2)<1):
        continue
     
     idx1=0; idx2=0
     if lep1==lep2:
       idx2=1
   
     if lep1=="Muon":
        veclep1 = lepton( evt.Muon_pt,  evt.Muon_eta,  evt.Muon_phi, 0.105, evt.Muon_charge, evt.Muon_mediumPromptId, evt.Muon_pfRelIso04_all, evt.Muon_dxy, evt.Muon_dxyErr,  evt.Muon_dz, evt.Muon_dzErr, idx1)
     else:
        veclep1 = lepton( evt.Electron_pt,  evt.Electron_eta,  evt.Electron_phi, 0.000511, evt.Electron_charge, evt.Electron_mvaFall17V2noIso_WP90, evt.Electron_pfRelIso03_all, evt.Electron_dxy, evt.Electron_dxyErr,  evt.Electron_dz, evt.Electron_dzErr, idx1)
   
     if lep2=="Muon":
        veclep2 = lepton( evt.Muon_pt,  evt.Muon_eta,  evt.Muon_phi, 0.105, evt.Muon_charge, evt.Muon_mediumPromptId, evt.Muon_pfRelIso04_all, evt.Muon_dxy, evt.Muon_dxyErr,  evt.Muon_dz, evt.Muon_dzErr, idx2)
     else:
        veclep2 = lepton( evt.Electron_pt,  evt.Electron_eta,  evt.Electron_phi, 0.000511, evt.Electron_charge, evt.Electron_mvaFall17V2noIso_WP90, evt.Electron_pfRelIso03_all, evt.Electron_dxy, evt.Electron_dxyErr,  evt.Electron_dz, evt.Electron_dzErr, idx2)
     if getattr(evt,"nBJet")>0: continue   
     if veclep1.dxy>0.2 or  veclep1.dz>0.5: continue
     if veclep2.dxy>0.2 or  veclep2.dz>0.5: continue
     if not veclep1.ID or not veclep2.ID: continue
     if Iso_cut>0 and (veclep1.iso>Iso_cut or veclep2.iso>Iso_cut): continue
     if sDxy_cut>0 and (veclep1.sdxy>sDxy_cut or veclep2.sdxy>sDxy_cut): continue
     if sDz_cut>0 and (veclep1.sdz>sDz_cut or veclep2.sdz>sDz_cut): continue
     if  veclep1.charge ==  veclep2.charge: continue
     if abs(veclep1.eta)>Eta1_cut or abs(veclep2.eta)>Eta2_cut: continue
     if Pt2_cut>0 and ( veclep1.pt<Pt2_cut or  veclep2.pt<Pt2_cut): continue
     if GenMatch:
        if lep1!=lep2:
           if getattr(evt,"Gen"+lep1+"_recoIdx")!=0 or getattr(evt,"Gen"+lep2+"_recoIdx")!=0 or getattr(evt,"Gen"+lep1+"_recoDR")>0.08 or getattr(evt,"Gen"+lep2+"_recoDR")>0.08:
               continue;
        else:
           if getattr(evt,"Gen"+lep1+"Plus_recoIdx")!=0 and getattr(evt,"Gen"+lep1+"Plus_recoIdx")!=1:
              continue
           if getattr(evt,"Gen"+lep2+"Minus_recoIdx")!=0 and getattr(evt,"Gen"+lep2+"Minus_recoIdx")!=0:
              continue
           if getattr(evt,"Gen"+lep1+"Plus_recoDR")>0.08 or getattr(evt,"Gen"+lep2+"Minus_recoDR")>0.08:
              continue 
     if len( triggers)==1:
        if veclep1.pt<Pt1_cut[0]: continue
     elif len( triggers)>1:
        fired = False
        if getattr(evt,triggers[0]) and veclep1.pt>Pt1_cut[0]:
           fired=True
        if getattr(evt,triggers[1]) and veclep2.pt>Pt1_cut[1]: 
           fired=True
        if not fired: continue
   
     vlor_lep1=rt.TLorentzVector()
     vlor_lep2=rt.TLorentzVector()
   
     if OrderLep_difFlavour and lep2!=lep1:
        if veclep1.pt>veclep2.pt:
           vlor_lep1.SetPtEtaPhiM(veclep1.pt,veclep1.eta,veclep1.phi,veclep1.mass)
           vlor_lep2.SetPtEtaPhiM(veclep2.pt,veclep2.eta,veclep2.phi,veclep2.mass)
        else:
           vlor_lep1.SetPtEtaPhiM(veclep2.pt,veclep2.eta,veclep2.phi,veclep2.mass)
           vlor_lep2.SetPtEtaPhiM(veclep1.pt,veclep1.eta,veclep1.phi,veclep1.mass)
     else:
        vlor_lep1.SetPtEtaPhiM(veclep1.pt,veclep1.eta,veclep1.phi,veclep1.mass)
        vlor_lep2.SetPtEtaPhiM(veclep2.pt,veclep2.eta,veclep2.phi,veclep2.mass)
     if ((vlor_lep1+vlor_lep2).M()<80 or (vlor_lep1+vlor_lep2).M()>100) and mass_cut=="tight":
       continue
     if ((vlor_lep1+vlor_lep2).M()<70 or (vlor_lep1+vlor_lep2).M()>110) and mass_cut=="loose": 
       continue
     ht[0]=getattr(evt,"HT")
     met[0] = getattr(evt,"PuppiMET_pt")
     met_phi[0] = getattr(evt,"PuppiMET_phi") 
     pt_l1[0] = vlor_lep1.Pt(); eta_l1[0] = vlor_lep1.Eta(); 
     phi_l1[0] = vlor_lep1.Phi(); mass_l1[0] = vlor_lep1.M()
     pt_l2[0] = vlor_lep2.Pt(); eta_l2[0] = vlor_lep2.Eta(); 
     phi_l2[0] = vlor_lep2.Phi(); mass_l2[0] = vlor_lep2.M()
   
     njets[0] = getattr(evt,"nJet")
     mt_l1[0] = MT(pt_l1[0], phi_l1[0], met[0], met_phi[0])
     mt_l2[0] = MT(pt_l2[0], phi_l2[0], met[0], met_phi[0])
   
     pt_ll[0] = (vlor_lep1+vlor_lep2).Pt(); 
     eta_ll[0] = (vlor_lep1+vlor_lep2).Eta(); 
     phi_ll[0] = (vlor_lep1+vlor_lep2).Phi(); 
     mass_ll[0] = (vlor_lep1+vlor_lep2).M()
   
     if pt_ll[0]==0:
        ratio_met_ptll[0]=0;
     else:
        ratio_met_ptll[0]=met[0]/pt_ll[0]; 
     if ht[0]>0:
       ratio_met_ht[0]= met[0]/ht[0]
     else:
        ratio_met_ht[0]=0.
   
     met_significance[0]=getattr(evt,"MET_significance")
     st[0] = ht[0]+pt_l1[0]+pt_l2[0]
     ratio_ptl2_ptl1[0]=pt_l2[0]/pt_l1[0]
   
     if evt.nJet==0: 
        pt_j1[0]=0
     else:
        pt_j1[0]=evt.Jet_pt[0]
   
     outtree.Fill()
   
   outtree.Write()
   fout.Close()
