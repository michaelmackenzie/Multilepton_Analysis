//Create a Higgs Combine environment for two search channels
#include "convert_mva_to_combine.C"
#include "combine_channels.C"

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
  if(status) return status;

  cout << "Combining the two channels...\n";
  status += combine_channels(set, selection, years);

  return status;
}
