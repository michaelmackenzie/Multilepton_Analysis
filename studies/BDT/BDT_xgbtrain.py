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


# def train_test_split(X,Y,test_size,random_state):
#   X_train_ind = np.random.choice(range(X.shape[0]), size=(5000,), replace=False)
#   X_train = X.sample(frac = 1. - test_size, random_state = random_state)
#   X_test = X.drop(X_train.index)
#   Y_train = Y.sample(frac = 1. - test_size, random_state = random_state)
#   Y_test = Y.drop(Y_train.index)
#   return X_train, X_test, Y_train, Y_test
  

 
def check_rm_files(files=[]):
  for fl in files:
     if os.path.isfile(fl): os.system("rm "+fl )


if __name__ == "__main__":
  print "--- BDT XGBoost Training ---"
  parser = argparse.ArgumentParser()
  parser.add_argument("--ntree", dest="ntree",default=1000, type=int,  help="number of trees")
  parser.add_argument("--depth", dest="depth",default=6, type=int,  help="tree depth")
  parser.add_argument("--lrate", dest="lrate",default=0.1, type=float,  help="learning rate")
  parser.add_argument("--subsample", dest="subsample", default=0.8, type=float, help="fraction of evts")
  parser.add_argument("--gamma", dest="gamma", default=3.0, type=float, help="gamma factor")
  parser.add_argument("--nodeweight", dest="nodeweight", default=1.0, type=float, help="weight for node in order to be split")
  parser.add_argument("--scaleweight", dest="scaleweight", default=1.0, type=float, help="")
  parser.add_argument("--lossfunction", dest="lossfunction", default="logistic", type=str, help="loss function")
  parser.add_argument("--modelname", dest="modelname", default="xgbmodel", type=str, help="model name to create or read if we use onetest option")
  parser.add_argument("--trainSgnFile", dest="trainSgnfile", default="bdt_tree_sgn_train.root", type=str, help="File to read. ")
  parser.add_argument("--trainBkgFile", dest="trainBkgfile", default="bdt_tree_bkg_train.root", type=str, help="File to read. ")
  parser.add_argument("--extraName", dest="extraName", default="",type=str, help="extra name in root files")
 

 
  args, unknown = parser.parse_known_args()
  
  for arg in unknown:
      print "warning uknown parameter",arg
  print "--> Parsed arguments"
    
  # selections
  selection="met<63 && mt_l1<99 && mt_l2<99"

  #load correct vars
  used_columns = ["met","ht","mt_l1","mt_l2","pt_ll","ratio_ptl2_ptl1"]
   
  model=args.modelname
  bdt=0;
  if args.trainSgnfile == None or args.trainBkgfile == None:
      print "provide train sgn file-exit"
      print "provide bkg train file-exit"
      exit()
  if selection!="":
    print "--> Selection applied:",selection
    signal= root_numpy.root2array(args.trainSgnfile, treename='mytree',branches=used_columns,selection=selection)
    backgr= root_numpy.root2array(args.trainBkgfile, treename='mytree',branches=used_columns,selection=selection)
  else:
    print "--> No selection applied"
    signal= root_numpy.root2array(args.trainSgnfile, treename='mytree',branches=used_columns)
    backgr= root_numpy.root2array(args.trainBkgfile, treename='mytree',branches=used_columns)
  signal=root_numpy.rec2array(signal)
  backgr=root_numpy.rec2array(backgr)

  print'train on', used_columns
  print "model name",model  
  for arg in vars(args): print "hyperparameter",arg,getattr(args, arg)

  X=np.concatenate((signal,backgr))
  Y=np.concatenate(([1 for i in range(len(signal))],[0 for i in range(len(backgr))]))
  X_train,X_test,Y_train,Y_test= train_test_split(X,Y,test_size=0.05,random_state=42)

  #model definition
  weightTrain= compute_sample_weight(class_weight='balanced', y=Y_train)
  weightTest= compute_sample_weight(class_weight='balanced', y=Y_test)
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

  print "finished"


