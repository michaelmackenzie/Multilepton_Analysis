#Train an XGBoost for the Z->X+tau_{l/had} searches

print "--> Importing libraries"
import matplotlib 
matplotlib.use('pdf')
import numpy as np
print "--> Importing SciKit Learn train/test split"
from sklearn.model_selection import train_test_split
print "--> Importing XGBoost libraries"
import xgboost
from xgboost import XGBClassifier
# this wrapper makes it possible to train on subset of features
print "--> Importing SciKit Learn Classifier"
from rep.estimators import SklearnClassifier
print "--> Importing ROOT Numpy"
import root_numpy
import argparse
print "--> Importing SciKit Learn joblib"
from sklearn.externals import joblib
from sklearn.utils.class_weight import compute_sample_weight
import time
import os
print "--> Importing ROOT"
import ROOT

 
def check_rm_files(files=[]):
  for fl in files:
     if os.path.isfile(fl): os.system("rm "+fl )

def get_variables(selection, version = 0):

  if selection == "mutau":
    if version is 0:
      variables = ['lepm', 'lepmestimate', 'beta1', 'beta2', 'mtone', 'twometdeltaphi', 'ptratio', 'lepdeltaphi', 'leppt', 'jetpt']
    else:
      print 'Undefined selection and version combination (%s, %i)' % (selection, version)
      return []
  elif selection == "etau":
    if version is 0:
      variables = ['lepm', 'lepmestimate', 'beta1', 'beta2', 'mtone', 'twometdeltaphi', 'ptratio', 'lepdeltaphi', 'leppt', 'jetpt']
    else:
      print 'Undefined selection and version combination (%s, %i)' % (selection, version)
      return []
    print variables
  elif selection == "mutau_e":
    if version is 0:
      variables = ['lepm', 'lepmestimate', 'beta1', 'beta2', 'mttwo', 'onemetdeltaphi', 'ptratio', 'lepdeltaphi', 'leppt', 'jetpt']
    else:
      print 'Undefined selection and version combination (%s, %i)' % (selection, version)
      return []
  elif selection == "etau_mu":
    if version is 0:
      variables = ['lepm', 'lepmestimate', 'beta1', 'beta2', 'mtone', 'twometdeltaphi', 'ptratio', 'lepdeltaphi', 'leppt', 'jetpt']
    else:
      print 'Undefined selection and version combination (%s, %i)' % (selection, version)
      return []
  else:
    print 'Undefined selection (%s)' % (selection)
    return []

  print variables
  return variables

if __name__ == "__main__":
  print "--- BDT XGBoost Training ---"
  ROOT.gROOT.SetBatch(1)

  parser = argparse.ArgumentParser()
  parser.add_argument("--channel"     , dest="channel"     , default="mutau"   , type=str  , help="Final state (mutau, etau, mutau_e, or etau_mu)")
  parser.add_argument("--version"     , dest="version"     , default=0         , type=int  , help="BDT version")
  parser.add_argument("--ntree"       , dest="ntree"       , default=1200      , type=int  , help="number of trees")
  parser.add_argument("--depth"       , dest="depth"       , default=6         , type=int  , help="tree depth")
  parser.add_argument("--lrate"       , dest="lrate"       , default=0.02      , type=float, help="learning rate")
  parser.add_argument("--subsample"   , dest="subsample"   , default=0.8       , type=float, help="fraction of evts")
  parser.add_argument("--gamma"       , dest="gamma"       , default=3.0       , type=float, help="gamma factor")
  parser.add_argument("--nodeweight"  , dest="nodeweight"  , default=1.0       , type=float, help="weight for node in order to be split")
  parser.add_argument("--scaleweight" , dest="scaleweight" , default=1.0       , type=float, help="")
  parser.add_argument("--lossfunction", dest="lossfunction", default="logistic", type=str  , help="loss function")
  parser.add_argument("--modelname"   , dest="modelname"   , default="Z0"      , type=str  , help="model name on output")
 

 
  args, unknown = parser.parse_known_args()
  
  for arg in unknown:
      print "Warning! Unknown parameter", arg
  print "--> Parsed arguments"

  # #Load the CLFVAnalysis library
  # ROOT.gInterpreter.AddIncludePath("./")
  # ROOT.gInterpreter.AddIncludePath("./include")
  # ROOT.gInterpreter.AddIncludePath("%s/include" % (ROOT.gSystem.Getenv("ROOTSYS")))
  # ROOT.gSystem.Load("lib/libCLFVAnalysis.so")

  #Load the variables to use
  selection = args.channel
  version   = args.version
  variables = get_variables(selection, version)
  print 'variables used:', variables

  input_file = 'trees/background_clfv_Z0_nano_%s_2016_2017_2018_8_noweight.tree' % (selection)

  model='%s.%s.xgboost.2016_2017_2018' % (selection, args.modelname)
  bdt=0;

  print "--> Getting background and signal trees"
  # signal = root_numpy.root2array(input_file, treename='background_tree',branches=variables,selection=sig_cut)
  # backgr = root_numpy.root2array(input_file, treename='background_tree',branches=variables,selection=bkg_cut)
  signal = root_numpy.root2array(input_file, treename='signal'    ,branches=variables)
  backgr = root_numpy.root2array(input_file, treename='background',branches=variables)
  signal = root_numpy.rec2array(signal)
  backgr = root_numpy.rec2array(backgr)


  print'train on', variables
  print "model name",model  
  for arg in vars(args): print "hyperparameter",arg,getattr(args, arg)

  X=np.concatenate((signal,backgr))
  Y=np.concatenate(([1 for i in range(len(signal))],[0 for i in range(len(backgr))]))
  X_train,X_test,Y_train,Y_test= train_test_split(X,Y,test_size=0.05,random_state=42)

  #model definition
  weightTrain= compute_sample_weight(class_weight='balanced', y=Y_train)
  weightTest = compute_sample_weight(class_weight='balanced', y=Y_test)
  bdt=XGBClassifier(max_depth=args.depth,n_estimators=args.ntree,learning_rate=args.lrate, min_child_weight=args.nodeweight, gamma=args.gamma, subsample=args.subsample, scale_pos_weight=args.scaleweight, objective= 'binary:'+args.lossfunction) 

  #training
  start = time.clock()
  bdt.fit(X_train,Y_train,sample_weight=weightTrain)
  elapsed = time.clock()
  elapsed = elapsed - start

  #save weight
  print "train time: ", elapsed,"saving model"
  bdt.get_booster().save_model(model+'.json')
  joblib.dump(bdt,model+'.pkl')

  

  #------------------------------------------------------------------
  # Evaluate the BDT

  bkg_bdt_scores = [x[1] for x in bdt.predict_proba(backgr)]
  sig_bdt_scores = [x[1] for x in bdt.predict_proba(signal)]
  hsig = ROOT.TH1F('hsig', 'BDT score distribution; BDT score', 30, 0, 1)
  hbkg = ROOT.TH1F('hbkg', 'BDT score distribution; BDT score', 30, 0, 1)
  for val in sig_bdt_scores: hsig.Fill(val)
  for val in bkg_bdt_scores: hbkg.Fill(val)
  hsig.Scale(1./hsig.Integral())
  hbkg.Scale(1./hbkg.Integral())

  #------------------------------------------------------------------
  # Plot the BDT performance

  figure_dir = 'figures/%s' % (selection)
  os.system('[ ! -d %s ] && mkdir -p %s' % (figure_dir, figure_dir))

  ROOT.gStyle.SetOptStat(0)
  canvas = ROOT.TCanvas("c","c",700,600)
  hsig.Draw("hist")
  hbkg.Draw("hist same")
  hsig.Draw("hist same")
  hsig.SetLineColor(ROOT.kBlue)
  hsig.SetFillColor(ROOT.kBlue)
  hsig.SetFillStyle(3004)
  hsig.SetLineWidth(2)
  hbkg.SetLineColor(ROOT.kRed)
  hbkg.SetFillColor(ROOT.kRed)
  hbkg.SetFillStyle(3005)
  hbkg.SetLineWidth(2)

  max_val = max(hsig.GetMaximum(), hbkg.GetMaximum())
  hsig.GetYaxis().SetRangeUser(0., 1.2*max_val)
  canvas.SaveAs('%s/bdt.png' % (figure_dir))
  hsig.GetYaxis().SetRangeUser(max_val/1.e4, 5.*max_val)
  canvas.SetLogy()
  canvas.SaveAs('%s/bdt_log.png' % (figure_dir))


  print "finished"


