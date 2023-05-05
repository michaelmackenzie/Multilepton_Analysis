import matplotlib 
matplotlib.use('pdf')
import numpy as np
from sklearn.model_selection import train_test_split
from xgboost import XGBClassifier
# this wrapper makes it possible to train on subset of features
from rep.estimators import SklearnClassifier
import root_numpy
import argparse
from sklearn.externals import joblib
from sklearn.utils.class_weight import compute_sample_weight
import time
import os
import ROOT



 
def check_rm_files(files=[]):
  for fl in files:
     if os.path.isfile(fl): os.system("rm "+fl )

def evaluate_bdt(bdt,data_file,bdt_cols,common_cols,selection,modelname):
  check_rm_files([modelname+".root",modelname+".pkl"])
  if selection !="":
    print "selection applied",selection
    dataSample= root_numpy.root2array(data_file, treename='mytree',branches=bdt_cols,selection=selection)
    commonVars= root_numpy.root2array(data_file, treename='mytree',branches=common_cols,selection=selection)
  else:
    dataSample= root_numpy.root2array(data_file, treename='mytree',branches=bdt_cols)
    commonVars= root_numpy.root2array(data_file, treename='mytree',branches=common_cols)
  dataSample=root_numpy.rec2array(dataSample)
  decisions=[x[1] for x in bdt.predict_proba(dataSample)]
  decisions=np.array(decisions,dtype=np.float64)
  decisions.dtype=[("xgb",np.float64)]   
  for i in [decisions,commonVars]:
    root_numpy.array2root(i,modelname+".root","mytreefit")
  return decisions.shape[0]


if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  parser.add_argument("--modelname", dest="modelname", default="xgbmodel.pkl", type=str, help="model name to create or read if we use onetest option")
  parser.add_argument("--measureFile", dest="measureFile", default="bdt_tree_v2_emu_test.root", type=str, help="File to read.no default")
  parser.add_argument("--mainName", dest="mainName", default="Zemu",type=str, help="overwrites the 'model' part of the naming convension")
  parser.add_argument("--extraName", dest="extraName", default="",type=str, help="extra name in root files")

  
  args, unknown = parser.parse_known_args()
  
  for arg in unknown:
      print "warning uknown parameter",arg

  selection="met<63 && mt_l1<99 && mt_l2<99"
  #load correct vars
  
  used_columns=['met', 'ht', 'mt_l1', 'mt_l2', 'pt_ll', 'ratio_ptl2_ptl1']
  

  model=args.modelname
  if ".pkl" not in model:
     bdt=joblib.load(model+".pkl") 
  else:
     bdt=joblib.load(model)
  if args.mainName=="": args.mainName=model
  if args.extraName!="": args.extraName= args.mainName+"_"+args.extraName
  else: args.extraName= args.mainName
  print "Evaluating BDT..."
  start_time = time.time()
  nentries = evaluate_bdt(bdt,args.measureFile,used_columns,["met","mass_ll"],selection,"forMeas_"+args.extraName)
  finish_time = time.time()
  print "Evaluation took %.3fs (%.1f Hz)" % (finish_time - start_time, nentries / (finish_time - start_time))




