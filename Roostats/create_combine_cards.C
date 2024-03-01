//Create a Higgs Combine environment for two search channels
#include "convert_mva_to_combine.C"
#include "control_region_datacard.C"
// #include "combine_channels.C"

bool use_control_regions_ = false; //use mumu/ee control regions
int control_region_set_   = 8; //use -1 to set to input set

int combine_channels(int set, TString selection, vector<int> years) {
  TString had = selection;
  TString lep = selection;
  if(selection.Contains("mutau")) lep += "_e" ;
  else                            lep += "_mu";
  TString year_string = Form("%i", years[0]);
  for(int i = 1; i < years.size(); ++i) year_string += Form("_%i", years[i]);
  gSystem->cd(Form("datacards/%s", year_string.Data()));
  TString command = Form("combineCards.py had=combine_mva_%s_%i_%s.txt lep=combine_mva_%s_%i_%s.txt",
                         had.Data(), set, year_string.Data(), lep.Data(), set, year_string.Data());
  TString outfile = Form("combine_mva_total_%s_%i_%s.txt", selection.Data(), set, year_string.Data());
  command += Form(" >| %s", outfile.Data());
  gSystem->Exec(command.Data());
  gSystem->Exec(command.Data());
  gSystem->cd("../..");
  return 0;
}

int create_combine_cards_set(int set = 8, TString selection = "zmutau",
                             vector<int> years = {2016, 2017, 2018}, int had_only = 0) {
  int status(0);
  TString year_string = Form("%i", years[0]);
  for(int i = 1; i < years.size(); ++i) year_string += Form("_%i", years[i]);

  if(control_region_set_ < 0) control_region_set_ = set;

  if(had_only >= 0) {
    cout << "Creating hadronic tau card...\n";
    status += convert_mva_to_combine(set, selection, years);
  }
  if(had_only <= 0) {
    cout << "Creating leptonic tau card...\n";
    if(selection.Contains("mutau"))
      status += convert_mva_to_combine(set, selection+"_e", years);
    else if(selection.Contains("etau"))
      status += convert_mva_to_combine(set, selection+"_mu", years);
  }
  if(use_control_regions_ && had_only == 0) {
    cout << "Creating mumu control region card...\n";
    status += control_region_datacard(control_region_set_, "mumu", selection, years);
    cout << "Creating ee control region card...\n";
    status += control_region_datacard(control_region_set_, "ee"  , selection, years);
    if(status) return status;
  }

  if(had_only == 0) {
    cout << "Combining the two signal channels...\n";
    if(separate_years_) {
      for(int year : years) {
        status += combine_channels(set, selection, {year});
      }
    } else {
      status += combine_channels(set, selection, years);
    }
  }

  //FIXME: Combine years using combineCards.py instead of a merged fit!
  if(separate_years_ && years.size() > 1) {
    TString command_2 = "combineCards.py"; //hadronic+leptonic fit
    TString command_3 = "combineCards.py"; //hadronic fit
    TString command_4 = "combineCards.py"; //leptonic fit
    TString command_5 = "combineCards.py"; //mumu region
    TString command_6 = "combineCards.py"; //ee region
    TString command_1 = "";
    TString had = selection;
    TString lep = selection;
    if(selection.Contains("mutau")) lep += "_e" ;
    else                            lep += "_mu";
    for(int year : years) {
      // command_1 += Form("cp datacards/%i/combine_mva_total_%s_%i_%i.txt datacards/%s/; ", year, selection.Data(), set, year, year_string.Data());
      if(had_only >= 0) {
        command_1 += Form("cp datacards/%i/combine_mva_%s_%i_%i.txt  datacards/%s/; ", year, had.Data(), set, year, year_string.Data());
        command_1 += Form("cp datacards/%i/combine_mva_%s_%i_%i.root datacards/%s/; ", year, had.Data(), set, year, year_string.Data());
      }
      if(had_only <= 0) {
        command_1 += Form("cp datacards/%i/combine_mva_%s_%i_%i.txt  datacards/%s/; ", year, lep.Data(), set, year, year_string.Data());
        command_1 += Form("cp datacards/%i/combine_mva_%s_%i_%i.root datacards/%s/; ", year, lep.Data(), set, year, year_string.Data());
      }
      if(use_control_regions_) {
        command_1 += Form("cp datacards/%i/combine_mva_%s_cr_mumu_%i_%i.txt  datacards/%s/; ", year, selection.Data(), control_region_set_, year, year_string.Data());
        command_1 += Form("cp datacards/%i/combine_mva_%s_cr_ee_%i_%i.txt    datacards/%s/; ", year, selection.Data(), control_region_set_, year, year_string.Data());
        command_1 += Form("cp datacards/%i/combine_mva_%s_cr_mumu_%i_%i.root datacards/%s/; ", year, selection.Data(), control_region_set_, year, year_string.Data());
        command_1 += Form("cp datacards/%i/combine_mva_%s_cr_ee_%i_%i.root   datacards/%s/; ", year, selection.Data(), control_region_set_, year, year_string.Data());
        command_5 += Form(" y%i=combine_mva_%s_cr_mumu_%i_%i.txt", year, selection.Data(), control_region_set_, year); //mumu region
        command_6 += Form(" y%i=combine_mva_%s_cr_ee_%i_%i.txt"  , year, selection.Data(), control_region_set_, year); //ee region
      }
      command_2 += Form(" y%i=combine_mva_total_%s_%i_%i.txt", year, selection.Data(), set, year); //total
      command_3 += Form(" y%i=combine_mva_%s_%i_%i.txt"      , year, had.Data()      , set, year); //hadronic
      command_4 += Form(" y%i=combine_mva_%s_%i_%i.txt"      , year, lep.Data()      , set, year); //leptonic
    }
    command_2 += Form(" >| combine_mva_total_%s_%i_%s.txt", selection.Data(), set, year_string.Data());
    command_3 += Form(" >| combine_mva_%s_%i_%s.txt"      , had.Data()      , set, year_string.Data());
    command_4 += Form(" >| combine_mva_%s_%i_%s.txt"      , lep.Data()      , set, year_string.Data());
    // cout << command_1.Data() << endl;
    cout << "Copying individual year cards...\n";
    gSystem->Exec(command_1.Data());
    gSystem->cd(Form("datacards/%s/", year_string.Data()));
    if(had_only == 0) {
      // cout << command_2.Data() << endl;
      cout << "Merging the total fit cards...\n";
      gSystem->Exec(command_2.Data());
    }
    cout << "Merging the individual channel fit cards...\n";
    if(had_only >= 0)
      gSystem->Exec(command_3.Data());
    if(had_only <= 0)
      gSystem->Exec(command_4.Data());
    if(use_control_regions_ && had_only == 0) {
      cout << "Merging the control region cards...\n";
      command_5 += Form(" >| combine_mva_%s_cr_mumu_%i_%s.txt", selection.Data(), control_region_set_, year_string.Data());
      command_6 += Form(" >| combine_mva_%s_cr_ee_%i_%s.txt"  , selection.Data(), control_region_set_, year_string.Data());
      gSystem->Exec(command_5.Data());
      gSystem->Exec(command_6.Data());
    }

    gSystem->cd("../..");
    // separate_years_ = false;
    // return create_combine_cards_set(set, selection, years);
  }

  if(use_control_regions_ && had_only == 0) {
    cout << "Combining signal and control region cards...\n";
    TString outdir = "datacards/";
    outdir += year_string;
    TString command = Form("combineCards.py sr=%s/combine_mva_total_%s_%i_%s.txt",
                           outdir.Data(), selection.Data(), set, year_string.Data());
    command += Form(" mcr=%s/combine_mva_%s_cr_mumu_%i_%s.txt",
                    outdir.Data(), selection.Data(), control_region_set_, year_string.Data());
    command += Form(" ecr=%s/combine_mva_%s_cr_ee_%i_%s.txt",
                    outdir.Data(), selection.Data(), control_region_set_, year_string.Data());
    command += Form(" >| %s/combine_mva_total_cr_%s_%i_%s.txt",
                    outdir.Data(), selection.Data(), set, year_string.Data());
    cout << "Executing command:\n" << command.Data() << endl;
    gSystem->Exec(command.Data());
    command = Form("sed -i \'s/datacards\\/%s\\///g\' %s/combine_mva_total_cr_%s_%i_%s.txt",
                   year_string.Data(), outdir.Data(), selection.Data(), set, year_string.Data());
    cout << "Executing command:\n" << command.Data() << endl;
    gSystem->Exec(command.Data());
  }

  return status;
}

int create_combine_cards(vector<int> sets, TString selection = "zmutau",
                         vector<int> years = {2016, 2017, 2018}, int had_only = 0) {
  int status(0);
  for(int set : sets) status += create_combine_cards_set(set, selection, years, had_only);
  return status;
}
