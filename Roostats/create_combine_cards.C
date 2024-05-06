//Create a Higgs Combine environment for two search channels
#include "convert_mva_to_combine.C"

int combine_channels(int set, TString selection, vector<int> years, TString tag = "") {
  TString had = selection;
  TString lep = selection;
  if(selection.Contains("mutau")) lep += "_e" ;
  else                            lep += "_mu";
  TString year_string = Form("%i", years[0]);
  for(int i = 1; i < years.size(); ++i) year_string += Form("_%i", years[i]);
  gSystem->cd(Form("datacards/%s", year_string.Data()));
  TString command = Form("combineCards.py had=combine_mva_%s_%i%s_%s.txt lep=combine_mva_%s_%i%s_%s.txt",
                         had.Data(), set, tag.Data(), year_string.Data(), lep.Data(), set, tag.Data(), year_string.Data());
  TString outfile = Form("combine_mva_total_%s_%i%s_%s.txt", selection.Data(), set, tag.Data(), year_string.Data());
  command += Form(" >| %s", outfile.Data());
  gSystem->Exec(command.Data());
  gSystem->Exec(command.Data());
  gSystem->cd("../..");
  return 0;
}

int create_combine_cards_set(int set = 8, TString selection = "zmutau",
                             vector<int> years = {2016, 2017, 2018}, int had_only = 0, TString tag = "") {
  int status(0);
  TString year_string = Form("%i", years[0]);
  for(int i = 1; i < years.size(); ++i) year_string += Form("_%i", years[i]);

  const int seed = 90;

  if(had_only >= 0) {
    cout << "Creating hadronic tau card...\n";
    status += convert_mva_to_combine(set, selection, years, seed, tag);
  }
  if(had_only <= 0) {
    cout << "Creating leptonic tau card...\n";
    if(selection.Contains("mutau"))
      status += convert_mva_to_combine(set, selection+"_e", years, seed, tag);
    else if(selection.Contains("etau"))
      status += convert_mva_to_combine(set, selection+"_mu", years, seed, tag);
  }

  if(had_only == 0) {
    cout << "Combining the two signal channels...\n";
    if(separate_years_) {
      for(int year : years) {
        status += combine_channels(set, selection, {year}, tag);
      }
    } else {
      status += combine_channels(set, selection, years, tag);
    }
  }

  if(separate_years_ && years.size() > 1) {
    TString command_2 = "combineCards.py"; //hadronic+leptonic fit
    TString command_3 = "combineCards.py"; //hadronic fit
    TString command_4 = "combineCards.py"; //leptonic fit
    TString command_1 = "";
    TString had = selection;
    TString lep = selection;
    if(selection.Contains("mutau")) lep += "_e" ;
    else                            lep += "_mu";
    for(int year : years) {
      if(had_only >= 0) {
        command_1 += Form("cp datacards/%i/combine_mva_%s_%i%s_%i.txt  datacards/%s/; ", year, had.Data(), set, tag.Data(), year, year_string.Data());
        command_1 += Form("cp datacards/%i/combine_mva_%s_%i%s_%i.root datacards/%s/; ", year, had.Data(), set, tag.Data(), year, year_string.Data());
      }
      if(had_only <= 0) {
        command_1 += Form("cp datacards/%i/combine_mva_%s_%i%s_%i.txt  datacards/%s/; ", year, lep.Data(), set, tag.Data(), year, year_string.Data());
        command_1 += Form("cp datacards/%i/combine_mva_%s_%i%s_%i.root datacards/%s/; ", year, lep.Data(), set, tag.Data(), year, year_string.Data());
      }
      command_2 += Form(" y%i=combine_mva_total_%s_%i%s_%i.txt", year, selection.Data(), set, tag.Data(), year); //total
      command_3 += Form(" y%i=combine_mva_%s_%i%s_%i.txt"      , year, had.Data()      , set, tag.Data(), year); //hadronic
      command_4 += Form(" y%i=combine_mva_%s_%i%s_%i.txt"      , year, lep.Data()      , set, tag.Data(), year); //leptonic
    }
    command_2 += Form(" >| combine_mva_total_%s_%i%s_%s.txt", selection.Data(), set, tag.Data(), year_string.Data());
    command_3 += Form(" >| combine_mva_%s_%i%s_%s.txt"      , had.Data()      , set, tag.Data(), year_string.Data());
    command_4 += Form(" >| combine_mva_%s_%i%s_%s.txt"      , lep.Data()      , set, tag.Data(), year_string.Data());
    cout << "Copying individual year cards...\n";
    gSystem->Exec(command_1.Data());
    gSystem->cd(Form("datacards/%s/", year_string.Data()));
    if(had_only == 0) {
      cout << "Merging the total fit cards...\n";
      gSystem->Exec(command_2.Data());
    }
    cout << "Merging the individual channel fit cards...\n";
    if(had_only >= 0)
      gSystem->Exec(command_3.Data());
    if(had_only <= 0)
      gSystem->Exec(command_4.Data());

    gSystem->cd("../..");
  }

  return status;
}

int create_combine_cards(vector<int> sets, TString selection = "zmutau",
                         vector<int> years = {2016, 2017, 2018}, int had_only = 0, TString tag = "") {
  int status(0);
  for(int set : sets) status += create_combine_cards_set(set, selection, years, had_only, tag);
  return status;
}
