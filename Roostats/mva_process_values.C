//Define a value for a process name
#ifndef _PROCVALUES_
#define _PROCVALUES_

map<TString, int> values_;
int process_value(TString process) {
  if(values_.size() == 0) {
    //only search for signals separately
    values_["zmutau"] = 0;
    values_["zetau" ] = 0;
    values_["zemu"  ] = 0;
    values_["hmutau"] = 0;
    values_["hetau" ] = 0;
    values_["hemu"  ] = 0;

    values_["ZToeemumu"] = 1;
    values_["MisID"    ] = 2;
    values_["Top"      ] = 3;
    values_["Embedding"] = 4;
    values_["OtherVB"  ] = 5;
    values_["QCD"      ] = 6;
    values_["ZTotautau"] = 7;
  }
  if(values_.find(process) != values_.end()) return values_[process];

  //if not defined, add it
  values_[process] = values_.size();

  cout << "Adding process " << process.Data() << " with ID " << values_[process] << endl;
  return values_[process];
}

#endif
