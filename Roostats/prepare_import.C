#include "convert_simultaneous_to_multipdf.C"

Int_t prepare_import(vector<int> sets = {8}, TString selection = "zemu",
                     vector<int> years = {2016, 2017, 2018}) {
  TString year_string = "";
  for(unsigned i = 0; i < years.size(); ++i) {
    if(i > 0) year_string += "_";
    year_string += years[i];
  }
  TString set_string = "";
  for(unsigned i = 0; i < sets.size(); ++i) {
    if(i > 0) set_string += "_";
    set_string += sets[i];
  }

  TString infile = Form("imports/combine_%s_%s_%s.root", year_string.Data(), selection.Data(), set_string.Data());
  TString outfile = Form("datacards/%s/combine_%s_%s.root", year_string.Data(), selection.Data(), set_string.Data());

  return convert_simultaneous_to_multipdf(infile, outfile);
}
