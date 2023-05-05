import ROOT as rt
from Helper_functions import *
import math
import matplotlib
matplotlib.use('pdf')
import matplotlib.pyplot as plt
from numpy import cov
from root_numpy import root2array
import pandas as pd
from scipy.stats import spearmanr
from scipy.stats import pearsonr
import os

PlotBDT_SgnBkg=False
PlotROC_curve=False
PlotSignificanceScanData=False
PlotSignificanceScanMC=False
PlotLimitScanData=False
PlotLimitScan=False; name_txt="test_limit"
PlotCorrelationMatrix=False
PlotMassSculptingTest=True

sgn_file="forMeas_sgn.root"
sgn_cuts="(85<mass_ll && mass_ll<95)"
bkg_file="forMeas_data.root"
bkg_cuts="(mass_ll<85 || mass_ll>95)"
mass_sculpt_file="forMeas_ww.root"
tree_name="mytreefit"
bdt_name="xgb"

#For both scan WP
scan_min=0.0
scan_max=1.0
scan_step=0.05

#for data scan
total_signal=40000.
BF=47.737
lumi=60.0

#for data limit scan extra
xsec=6435000./(3.*0.0337) #xsec[femto]*BR(Z->ll)[idio me tou MC]/(BR->ll*3)

#for MC scan WP
expect_sgn = 540
expect_bkg = 4500


#For correlation
correlation_vars=["mass_ll","pt_ll","met","ht","mt_mu","mt_e","ratio_pte_ptmu"]





rt.gROOT.SetBatch(True)

if PlotBDT_SgnBkg:
   sgn_fl=rt.TFile(sgn_file,"READ")
   bkg_fl=rt.TFile(bkg_file,"READ")
   sgn_tree = sgn_fl.Get(tree_name)
   bkg_tree = bkg_fl.Get(tree_name) 
   hsgn_bdt = rt.TH1F("hsgn_bdt","",30,0,1)
   hbkg_bdt = rt.TH1F("hbkg_bdt","",30,0,1)
   sgn_tree.Draw(bdt_name+">>hsgn_bdt",sgn_cuts)
   bkg_tree.Draw(bdt_name+">>hbkg_bdt",bkg_cuts)
   plot_histos( [hsgn_bdt, hbkg_bdt],["Sgn","Bkg"], [rt.kRed, rt.kBlue], "bdt_plot", "BDT",True, False, "TL")
   

if PlotROC_curve:
   sgn_fl=rt.TFile(sgn_file,"READ")
   bkg_fl=rt.TFile(bkg_file,"READ")
   sgn_tree = sgn_fl.Get(tree_name)
   bkg_tree = bkg_fl.Get(tree_name)
   hsgn_bdt = rt.TH1F("hsgn_bdt","",30,0,1)
   hbkg_bdt = rt.TH1F("hbkg_bdt","",30,0,1)
   sgn_tree.Draw(bdt_name+">>hsgn_bdt",sgn_cuts)
   bkg_tree.Draw(bdt_name+">>hbkg_bdt",bkg_cuts)   
   rocy = []
   rocx = []
   
   for ibin in range(hsgn_bdt.GetNbinsX()):
     rocy.append(hsgn_bdt.Integral(ibin,hsgn_bdt.GetNbinsX())/hsgn_bdt.Integral())
     rocx.append(hbkg_bdt.Integral(ibin,hsgn_bdt.GetNbinsX())/hbkg_bdt.Integral())
   print rocy,rocx
   plot_graphs([rocx],[rocy],[rt.kBlack],"roc","Bkg Eff","Signal Eff")

if PlotSignificanceScanMC:
   sgn_fl=rt.TFile(sgn_file,"READ")
   bkg_fl=rt.TFile(bkg_file,"READ")
   sgn_tree = sgn_fl.Get(tree_name)
   bkg_tree = bkg_fl.Get(tree_name)
   sgn_yields = [] 
   bkg_yields = [] 
   cuts=[]
   significance=[]
   hsgn_den = rt.TH1F("hsgn_den","",1,0,1000)
   hbkg_den = rt.TH1F("hbkg_den","",1,0,1000)
   sgn_tree.Draw("mass_ll>>hsgn_den",sgn_cuts)
   bkg_tree.Draw("mass_ll>>hbkg_den",bkg_cuts)   
   sgn_den=hsgn_den.GetBinContent(1)   
   bkg_den=hbkg_den.GetBinContent(1) 
   for icut in range( int((scan_max-scan_min)/scan_step) ):
      cut = icut*scan_step
      hsgn_mass = rt.TH1F("hsgn_mass_"+str(cut),"",1,0,1000)
      hbkg_mass = rt.TH1F("hbkg_mass_"+str(cut),"",1,0,1000)
      sgn_tree.Draw("mass_ll>>hsgn_mass_"+str(cut),sgn_cuts+" && xgb>"+str(cut))
      bkg_tree.Draw("mass_ll>>hbkg_mass_"+str(cut),bkg_cuts+" && xgb>"+str(cut))
      sgn_yields.append(  hsgn_mass.GetBinContent(1)/sgn_den*expect_sgn )
      bkg_yields.append(  hbkg_mass.GetBinContent(1)/bkg_den*expect_bkg )
      cuts.append(cut)
      significance.append( sgn_yields[-1]/math.sqrt(sgn_yields[-1]+bkg_yields[-1])  )
   plot_graphs([sgn_yields],[bkg_yields],[rt.kBlack],"yields","Sgn Yield","Bkg Yield")
   plot_graphs([cuts],[significance],[rt.kBlack],"significance","BDT","Significance")



if PlotSignificanceScanData:
   sgn_fl=rt.TFile(sgn_file,"READ")
   bkg_fl=rt.TFile(bkg_file,"READ")
   sgn_tree = sgn_fl.Get(tree_name)
   bkg_tree = bkg_fl.Get(tree_name)
   sgn_yields = [] 
   bkg_yields = [] 
   cuts=[]
   significance=[]
   expo_fit= rt.TF1("expo_fit","expo",70,110)
   for icut in range( int((scan_max-scan_min)/scan_step) ):
      cut = icut*scan_step
      hsgn_mass = rt.TH1F("hsgn_mass_"+str(cut),"",1,0,1000)
      hbkg_mass = rt.TH1F("hbkg_mass_"+str(cut),"",80,70,110)
      sgn_tree.Draw("mass_ll>>hsgn_mass_"+str(cut),sgn_cuts+" && xgb>"+str(cut))
      bkg_tree.Draw("mass_ll>>hbkg_mass_"+str(cut),bkg_cuts+" && xgb>"+str(cut))
      sgn_yields.append(  hsgn_mass.Integral()/total_signal * BF * lumi )
      hbkg_mass.Fit("expo_fit","LR")
      bkg_yields.append( expo_fit.Integral(85,95)/hbkg_mass.GetBinWidth(1) )
      cfit=rt.TCanvas("cfit_"+str(cut),"",700,700)
      hbkg_mass.Draw()
      cfit.SaveAs("cfit_"+str(cut)+".png")
      cuts.append(cut)
      significance.append(hsgn_mass.Integral()/total_signal * BF * lumi / math.sqrt( expo_fit.Integral(85,95)/hbkg_mass.GetBinWidth(1)+hsgn_mass.Integral()/total_signal * BF * lumi ) )
   plot_graphs([sgn_yields],[bkg_yields],[rt.kBlack],"yields","Sgn Yield","Bkg Yield")
   plot_graphs([cuts],[significance],[rt.kBlack],"significance","BDT","Significance")


if PlotLimitScanData:
   sgn_fl=rt.TFile(sgn_file,"READ")
   bkg_fl=rt.TFile(bkg_file,"READ")
   sgn_tree = sgn_fl.Get(tree_name)
   bkg_tree = bkg_fl.Get(tree_name)
   sgn_yields = [] 
   bkg_yields = [] 
   cuts=[]
   limit_N=[]
   limit_BF=[]
   expo_fit= rt.TF1("expo_fit","expo",70,110)
   for icut in range( int((scan_max-scan_min)/scan_step) ):
      cut = icut*scan_step
      hsgn_mass = rt.TH1F("hsgn_mass_"+str(cut),"",1,0,1000)
      hbkg_mass = rt.TH1F("hbkg_mass_"+str(cut),"",80,70,110)
      sgn_tree.Draw("mass_ll>>hsgn_mass_"+str(cut),sgn_cuts+" && xgb>"+str(cut))
      bkg_tree.Draw("mass_ll>>hbkg_mass_"+str(cut),bkg_cuts+" && xgb>"+str(cut))
      sgn_yields.append(  hsgn_mass.Integral()/total_signal * BF * lumi )
      hbkg_mass.Fit("expo_fit","LR")
      bkg_yields.append( expo_fit.Integral(85,95)/hbkg_mass.GetBinWidth(1) )
      cfit=rt.TCanvas("cfit_"+str(cut),"",700,700)
      hbkg_mass.Draw()
      cfit.SaveAs("cfit_"+str(cut)+".png")
      cuts.append(cut)
      limit_N.append(1.64*math.sqrt( expo_fit.Integral(85,95)/hbkg_mass.GetBinWidth(1) )/(hsgn_mass.Integral()/total_signal))
      limit_BF.append(1.64*math.sqrt( expo_fit.Integral(85,95)/hbkg_mass.GetBinWidth(1) )/(hsgn_mass.Integral()/total_signal*lumi*xsec))
      
   plot_graphs([sgn_yields],[bkg_yields],[rt.kBlack],"yields","Sgn Yield","Bkg Yield")
   plot_graphs([cuts],[limit_N],[rt.kBlack],"limitN","BDT","Limit N_{signal}")
   plot_graphs([cuts],[limit_BF],[rt.kBlack],"limitBF","BDT","Limit BF")





if PlotLimitScan:
   sgn_fl=rt.TFile(sgn_file,"READ")
   bkg_fl=rt.TFile(bkg_file,"READ")
   sgn_tree = sgn_fl.Get(tree_name)
   bkg_tree = bkg_fl.Get(tree_name)
   sgn_yields = []
   bkg_yields = []
   cuts=[]
   expo_fit= rt.TF1("expo_fit","expo",70,110)
   for icut in range( int((scan_max-scan_min)/scan_step) ):
      cut = icut*scan_step
      hsgn_mass = rt.TH1F("hsgn_mass_"+str(cut),"",1,0,1000)
      hbkg_mass = rt.TH1F("hbkg_mass_"+str(cut),"",80,70,110)
      sgn_tree.Draw("mass_ll>>hsgn_mass_"+str(cut),sgn_cuts+" && xgb>"+str(cut))
      bkg_tree.Draw("mass_ll>>hbkg_mass_"+str(cut),bkg_cuts+" && xgb>"+str(cut))
      sgn_yields.append(  hsgn_mass.Integral()/total_signal * BF * lumi )
      hbkg_mass.Fit("expo_fit","LR")
      bkg_yields.append( expo_fit.Integral(85,95)/hbkg_mass.GetBinWidth(1) )
      cfit=rt.TCanvas("cfit_"+str(cut),"",700,700)
      hbkg_mass.Draw()
      cfit.SaveAs("cfit_"+str(cut)+".png")
      cuts.append(cut)
   card="imax    1 number of bins\n"
   card+="jmax    1 number of processes minus 1\n"
   card+="kmax    * number of nuisance parameters\n"
   card+="-"*80+"\n"
   card+="-"*80+"\n"
   card+="bin          signal_region\n"

   for icut in range(len(cuts)):
     current_card=card
     current_card+="observation  {0}\n".format(str(bkg_yields[icut]))
     current_card+="-"*80+"\n"
     current_card+="bin          signal_region  signal_region\n"
     current_card+="process      bkg            signal\n"
     current_card+="process      1              0\n"
     current_card+="rate     {0}            {1}".format(str(bkg_yields[icut]),str(sgn_yields[icut]))
     with open("temp_datacrd.txt",'w') as dc:
        dc.write(current_card)
     os.system('echo "BDT {0}" >> {1}.txt'.format(str(cuts[icut]),name_txt))
     os.system("combine -M AsymptoticLimits temp_datacrd.txt -m {0} >> {1}.txt".format(str(cuts[icut]),name_txt))

   limits=[]
   cuts2=[]
   with open(name_txt+".txt",'r') as dc:  
     lines = dc.readlines()
     for line in lines:
        words=line.split()
        if len(words)>0:    
          if words[0] == "BDT":
             cuts2.append(float(words[1]))
          if words[0] == "Observed":
             limits.append(float(words[4]))
   plot_graphs([cuts2],[limits],[rt.kBlack],"limits","BDT","Limit")        
          

if PlotCorrelationMatrix:
   for fl,name in zip([sgn_file,bkg_file],["signal","bkg"]):
     src=root2array(fl,treename='mytreefit',branches=correlation_vars)
     data=pd.DataFrame(src)
     corrmat = data.corr()
     print name,corrmat
     fig, ax1 = plt.subplots(ncols=1, figsize=(6,5))
     opts = {'cmap': plt.get_cmap("RdBu"), 'vmin': -1, 'vmax': +1}
     heatmap1 = ax1.pcolor(corrmat, **opts)
     plt.colorbar(heatmap1, ax=ax1)
     ax1.set_title("Correlations")
     labels = corrmat.columns.values
     for ax in (ax1,):
       # shift location of ticks to center of the bins
       ax.set_xticks(np.arange(len(labels))+0.5, minor=False)
       ax.set_yticks(np.arange(len(labels))+0.5, minor=False)
       ax.set_xticklabels(labels, minor=False, ha='right', rotation=70)
       ax.set_yticklabels(labels, minor=False)
     plt.tight_layout()
     plt.title('CMS Preliminary')
     plt.savefig("CorMatrix_"+name+".png")



if PlotMassSculptingTest:
   bkg_fl=rt.TFile(mass_sculpt_file,"READ")
   bkg_tree = bkg_fl.Get(tree_name)  
   hmass_bdt = rt.TH2F("hmass_bdt","", int((scan_max-scan_min)/scan_step),scan_min,scan_max,40,70,110)
   hbkg_bdt0 = rt.TH1F("hbkg_bdt0","",20,70,110)
   hbkg_bdt0p5 = rt.TH1F("hbkg_bdt0p5","",20,70,110)
   hbkg_bdt0p7 = rt.TH1F("hbkg_bdt0p7","",20,70,110)
   for icut in range( int((scan_max-scan_min)/scan_step) ):
     cut = icut*scan_step
     
     if cut==0:
        print "here",cut
        bkg_tree.Draw("mass_ll>>hbkg_bdt0","xgb>0.0")
     if cut==0.5:
        print "here",cut
        bkg_tree.Draw("mass_ll>>hbkg_bdt0p5","xgb>0.5")
     if cut==0.75:
        print "here",cut
        bkg_tree.Draw("mass_ll>>hbkg_bdt0p7","xgb>0.75")
     hbkg_mass = rt.TH1F("hbkg_mass_"+str(cut),"",40,70,110)
     bkg_tree.Draw("mass_ll>>hbkg_mass_"+str(cut),"xgb>"+str(cut))
     for imass in range(1,40):
       hmass_bdt.SetBinContent(icut,imass,hbkg_mass.GetBinContent(imass))
     hbkg_mass.GetYaxis().SetRangeUser(0,600)
     plot_histos([hbkg_mass],["WW"],[rt.kBlack],"mass_"+str(cut),"m(#mu,e)")
   hbkg_bdt0.GetYaxis().SetRangeUser(0,0.1)
   plot_histos([hbkg_bdt0, hbkg_bdt0p5, hbkg_bdt0p7],["BDT>0","BDT>0.5","BDT>0.75"],[rt.kBlack,rt.kBlue,rt.kRed],"mass_triplebdt","m(#mu,e)",norm=True,rng=[0,0.1])
   plot_2d_histo(hmass_bdt, "bdt_vs_mass", "COLZ", "BDT", "m(#mu,e)")
   src=root2array(mass_sculpt_file,treename='mytreefit',branches=["xgb","mass_ll"])
   corr_spear, _ = spearmanr(src["xgb"], src["mass_ll"])
   corr_pears, _ = pearsonr(src["xgb"], src["mass_ll"])
   print "spearman",corr_spear,"pearson",corr_pears
   
