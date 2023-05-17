print "--> Importing libraries"
print "--> Importing XGBoost libraries"
import xgboost
from xgboost import XGBClassifier
import argparse
print "--> Importing SciKit Learn joblib"
from sklearn.externals import joblib

if __name__ == "__main__":
  print "--- Converting XGBoost Training to JSON ---"
  parser = argparse.ArgumentParser()
  parser.add_argument("--model", dest="model", default="xgbmodel", type=str, help="model name to read in")
  parser.add_argument("--outname", dest="outname", default=None, type=str, help="model name write out")
  parser.add_argument("--dump", dest="dump", default=False, action='store_true', help="write out model dump")
 

 
  args, unknown = parser.parse_known_args()
  
  for arg in unknown:
      print "warning uknown parameter",arg
  print "--> Parsed arguments"
    
   
  model=args.model
  if ".pkl" not in model:
    bdt=joblib.load(model+".pkl")
  else:
    bdt=joblib.load(model)
  if not bdt:
    print "Failed to load BDT"
    exit()

  #save weight
  outname=args.outname
  if outname is None: outname = model
  bdt.get_booster().save_model(outname+'.json')
  if args.dump:
    print "--> Printing information about the BDT:"
    # dump = bdt.get_booster().dump_model(outname+'.txt',dump_format='txt')
    # print bdt.get_params()
    print bdt
    print bdt.get_booster().feature_names
    print bdt.get_booster().feature_types

  print "Finished"


