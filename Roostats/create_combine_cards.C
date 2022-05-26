//Create a Higgs Combine environment for two search channels
#include "convert_mva_to_combine.C"
#include "control_region_datacard.C"
#include "combine_channels.C"

bool use_control_regions_ = false; //use mumu/ee control regions

int create_combine_cards(int set = 8, TString selection = "zmutau",
                         vector<int> years = {2016, 2017, 2018}) {
  int status(0);
  cout << "Creating hadronic tau card...\n";
  status += convert_mva_to_combine(set, selection, years);
  cout << "Creating leptonic tau card...\n";
  if(selection.Contains("mutau"))
    status += convert_mva_to_combine(set, selection+"_e", years);
  else if(selection.Contains("etau"))
    status += convert_mva_to_combine(set, selection+"_mu", years);
  if(use_control_regions_) {
    cout << "Creating mumu control region card...\n";
    status += control_region_datacard(set, "mumu", selection, years);
    cout << "Creating ee control region card...\n";
    status += control_region_datacard(set, "ee"  , selection, years);
    if(status) return status;
  }

  cout << "Combining the two signal channels...\n";
  status += combine_channels(set, selection, years);

  if(use_control_regions_) {
    cout << "Combining signal and control region cards...\n";
    TString outdir = "datacards/";
    TString year_string = Form("%i", years[0]);
    for(int i = 1; i < years.size(); ++i) year_string += Form("_%i", years[i]);
    outdir += year_string;
    TString command = Form("combineCards.py signal_region=%s/combine_mva_total_%s_%i.txt",
                           outdir.Data(), selection.Data(), set);
    command += Form(" mumu_cr=%s/combine_mva_%s_cr_mumu_%i.txt",
                    outdir.Data(), selection.Data(), set);
    command += Form(" ee_cr=%s/combine_mva_%s_cr_ee_%i.txt",
                    outdir.Data(), selection.Data(), set);
    command += Form(" >| %s/combine_mva_total_cr_%s_%i.txt",
                    outdir.Data(), selection.Data(), set);
    cout << "Executing command:\n" << command.Data() << endl;
    gSystem->Exec(command.Data());
    command = Form("sed -i \'s/datacards\\/%s\\///g\' %s/combine_mva_total_cr_%s_%i.txt",
                   year_string.Data(), outdir.Data(), selection.Data(), set);
    cout << "Executing command:\n" << command.Data() << endl;
    gSystem->Exec(command.Data());
  }

  return status;
}
