//Script to make TTrees from trees in histogram files for MVA training and Roostats fitting
using namespace CLFV;

bool  backgroundOnly_ = false; //no signal tree
bool  dataOnly_       = false; //only data trees
bool  doHiggsDecays_  = false; //Z or H0 CLFV decay sets
bool  combineLepTau_  = false; //Combine emu and xtau categories into single tree
bool  doDY_           = true; //include Drell-Yan
bool  doWJets_        = true; //include W+Jets
bool  doTop_          = true; //include Top
bool  doDiboson_      = true; //include di-boson
bool  useLepSets_     = true; //use etau_mu/mutau_e sets
bool  debug_          = false; //debug the merging
Int_t verbose_        = 1; //how verbose the output should be
vector<int> years_    = {2016, 2017, 2018}; //years to merge
TString hist_tag_     = "clfv"; //CLFVHistMaker tag

//Combine the trees in a given file list
Int_t combine_trees(vector<TString> in_files, TString selection, int file_set, TString out_name) {
  TFile* fDList[in_files.size()];
  TFile* fList[in_files.size()];
  TTree* tList[in_files.size()]; //to keep trees in memory
  TList* list = new TList;
  Int_t filecount = 0;
  int nfiles = in_files.size();
  int set_offset = 0;
  if     (selection == "mutau"   ) set_offset = HistMaker::kMuTau;
  else if(selection == "etau"    ) set_offset = HistMaker::kETau;
  else if(selection == "emu"     ) set_offset = HistMaker::kEMu;
  else if(!useLepSets_ && selection.Contains("_")) set_offset = HistMaker::kEMu;
  else if(selection == "mutau_e" ) set_offset = HistMaker::kMuTauE;
  else if(selection == "etau_mu" ) set_offset = HistMaker::kETauMu;
  else if(selection == "mumu"    ) set_offset = HistMaker::kMuMu;
  else if(selection == "ee"      ) set_offset = HistMaker::kEE;

  for(int index = 0; index < nfiles; ++index) {
    fDList[filecount] = 0;
    fList[filecount] = 0;
    tList[filecount] = 0;
    const char* file_path = in_files[index];
    if(gSystem->AccessPathName(file_path)) {
      printf("File %s not found, continuing\n", file_path);
      continue;
    }
    if(verbose_ > 1) cout << "Index " << index << ": Using input filepath " << file_path << endl;
    fList[filecount] = TFile::Open(file_path, "READ");
    if(verbose_)
      printf("Getting tree %i for merging, using %s\n", filecount, file_path);
    if(!fList[filecount]) return 10;
    //if QCD, offset set to same sign selection
    int set_use = file_set + set_offset;
    if(combineLepTau_ && selection.Contains("tau") && !selection.Contains("_") && TString(file_path).Contains("_emu_"))
      set_use = file_set + HistMaker::kEMu;
    TString fname = fList[filecount]->GetName();
    if((fname.Contains("SingleMuon") || fname.Contains("SingleElectron")) && !dataOnly_) {//check if data-driven background
      if(selection == "mutau" || selection == "etau") //jet --> tau backgrounds
        set_use += HistMaker::fMisIDOffset;
      else                                            //QCD SS -> OS backgrounds
        set_use += HistMaker::fQcdOffset;
    }
    if(verbose_ > 1) cout << "Using file set " << set_use << endl;
    tList[filecount] = (TTree*) fList[filecount]->Get(Form("Data/tree_%i/tree_%i", set_use, set_use));
    if(!tList[filecount]) {
      printf("%s tree not found, continuing\n", file_path);
      continue;
    }
    if(debug_) tList[filecount]->Print();
    if(verbose_ > 1)
      cout << "Tree " << filecount << ": " << tList[filecount]
           << " Entries: " << tList[filecount]->GetEntriesFast() << endl;
    list->Add(tList[filecount]);
    filecount++;
  }
  if(list->GetSize() == 0) {
    printf("No trees found to merge, exiting\n");
    return 1;
  }

  if(verbose_ > 1)
    cout << "Collected " << list->GetSize() << " trees for merging!\n";

  TFile* out = new TFile(out_name.Data(),"RECREATE");
  TTree* t = 0;
  printf("Merging trees\n");
  if(verbose_ > 1) {
    for(int i = 0; i < list->GetSize(); ++i)
      cout << "Tree " << i << ": " << list->At(i)
           << " Entries " << tList[i]->GetEntriesFast() << endl;
  }
  t = TTree::MergeTrees(list);
  t->SetName("background_tree");
  for(int i = 0; i < list->GetSize(); ++i)
    delete list->At(i);

  out->cd();
  printf("Writing output tree\n");
  t->Write();
  delete t;
  printf("Writing output file\n");
  out->Write();
  out->Close();
  return 0;
}

//Create a file list and call the tree merging function for a given selection
Int_t make_background(int set = 8, TString selection = "mutau", TString base = "nanoaods_trees") {

  base = "root://cmseos.fnal.gov//store/user/mmackenz/histograms/" + base + "/";
  cout << "Beginning to make tree for selection " << selection.Data() << " with set " << set
       << " and histogram path " << base.Data()
       << endl;
  if(dataOnly_ && backgroundOnly_) {
    cout << "Set for both background and data only! Exiting...\n";
    return 1;
  }
  vector<TString> file_list;
  TString fileSelec = selection;
  if(!useLepSets_ && fileSelec.Contains("_")) //leptonic tau
    fileSelec = "emu";
  for(int year : years_) {
    vector<TString> nano_names = {"DY50"               ,
                                  "DY50-ext"           ,
                                  "DY50-amc"           ,
                                  "SingleAntiToptW"    ,
                                  "SingleToptW"        ,
                                  "WW"                 ,
                                  "WZ"                 ,
                                  "ZZ"                 ,
                                  "WWW"                ,
                                  "Wlnu-0"             ,
                                  "Wlnu-ext-0"         ,
                                  "Wlnu-1J"            ,
                                  "Wlnu-2J"            ,
                                  "Wlnu-3J"            ,
                                  "Wlnu-4J"            ,
                                  "ttbarToSemiLeptonic",
                                  "ttbarlnu"           ,
                                  "ZMuTau"             ,
                                  "ZETau"              ,
                                  "ZEMu"               ,
                                  "HMuTau"             ,
                                  "HETau"              ,
                                  "HEMu"
    };

    vector<int> doNanoProcess = {!dataOnly_   && doDY_ && year == 2017 //DY50
                                 , !dataOnly_ && doDY_ && year == 2017 //DY50-ext
                                 , !dataOnly_ && doDY_ && year != 2017 //DY50-amc
                                 , !dataOnly_ && doTop_ //tbar_tw
                                 , !dataOnly_ && doTop_ //t_tw
                                 , !dataOnly_ && doDiboson_ //WW
                                 , !dataOnly_ && doDiboson_ //WZ
                                 , !dataOnly_ && doDiboson_ //ZZ
                                 , !dataOnly_ && doDiboson_ //WWW
                                 , !dataOnly_ && doWJets_ //WJets
                                 , !dataOnly_ && doWJets_ && year != 2018 //WJets-ext
                                 , !dataOnly_ && doWJets_ //WJets-1J
                                 , !dataOnly_ && doWJets_ //WJets-2J
                                 , !dataOnly_ && doWJets_ //WJets-3J
                                 , !dataOnly_ && doWJets_ && year != 2017 //WJets-4J
                                 , !dataOnly_ && doTop_ //ttbar
                                 , !dataOnly_ && doTop_ //ttbar
                                 , !dataOnly_ && (!backgroundOnly_ && !doHiggsDecays_ && (selection.Contains("mutau"))) //zmutau
                                 , !dataOnly_ && (!backgroundOnly_ && !doHiggsDecays_ && (selection.Contains("etau") )) //zetau
                                 , !dataOnly_ && (!backgroundOnly_ && !doHiggsDecays_ && (selection == "emu")  ) //zetau
                                 , !dataOnly_ && (!backgroundOnly_ && doHiggsDecays_  && (selection.Contains("mutau"))) //hmutau
                                 , !dataOnly_ && (!backgroundOnly_ && doHiggsDecays_  && (selection.Contains("etau") )) //hetau
                                 , !dataOnly_ && (!backgroundOnly_ && doHiggsDecays_  && (selection == "emu")  ) //hetau
    };

    vector<TString> runs;
    if     (year == 2016) runs = {"B", "C", "D", "E", "F", "G", "H"};
    else if(year == 2017) runs = {"B", "C", "D", "E", "F"};
    else if(year == 2018) runs = {"A", "B", "C", "D"};
    else return 20;

    for(TString run : runs) {
      nano_names.push_back(Form("SingleMuon-%s"    , run.Data()));
      nano_names.push_back(Form("SingleElectron-%s", run.Data()));
      doNanoProcess.push_back(dataOnly_);
      doNanoProcess.push_back(dataOnly_);
      nano_names.push_back(Form("QCD_SingleMuon-%s"    , run.Data()));
      nano_names.push_back(Form("QCD_SingleElectron-%s", run.Data()));
      doNanoProcess.push_back(!dataOnly_ && !fileSelec.EndsWith("etau")); //QCD/Loose ID
      doNanoProcess.push_back(!dataOnly_ && !fileSelec.EndsWith("mutau")); //QCD/Loose ID
    }

    const int nfiles = nano_names.size();
    for(int i = 0; i < nfiles; ++i) {
      TString name = nano_names[i];
      if(verbose_ > 2) cout << "Index " << i << ": Input name " << name.Data() << endl;
      if(!doNanoProcess[i]) continue;
      if(verbose_ == 2) cout << "Index " << i << ": Input name " << name.Data() << endl;
      if(name.Contains("Single") && name.Contains("QCD")) name.ReplaceAll("QCD_", "");
      if(name.Contains("DY50")) {
        file_list.push_back(Form("%s%s_%s_%i_%s-1.hist",base.Data(),hist_tag_.Data(),fileSelec.Data(),year,name.Data()));
        file_list.push_back(Form("%s%s_%s_%i_%s-2.hist",base.Data(),hist_tag_.Data(),fileSelec.Data(),year,name.Data()));
      } else {
        file_list.push_back(Form("%s%s_%s_%i_%s.hist",base.Data(),hist_tag_.Data(),fileSelec.Data(),year,name.Data()));
      }
      if(combineLepTau_ && fileSelec.Contains("tau") && !fileSelec.Contains("_"))
        file_list.push_back(Form("%s%s_emu_%i_%s.hist",base.Data(),hist_tag_.Data(),year,name.Data()));
    }
  }

  TString type = ""; //which type of process is trained
  if     ( doDY_ && !doWJets_ && !doDiboson_ && !doTop_)
    type = "DY_";
  else if(!doDY_ &&  doWJets_ && !doDiboson_ && !doTop_)
    type = "WJets_";
  else if(!doDY_ && !doWJets_ &&  doDiboson_ && !doTop_)
    type = "DiBoson_";
  else if(!doDY_ && !doWJets_ && !doDiboson_ &&  doTop_)
    type = "Top_";
  else if( doDY_ &&  doWJets_ &&  doDiboson_ &&  doTop_)
    type = "";
  else
    printf("Unknown process combination! No name flag added\n");
  if(combineLepTau_ && fileSelec.Contains("tau") && !fileSelec.Contains("_")) type += "LepCombined_";
  if(backgroundOnly_) type += "bkg_";
  else if(dataOnly_)  type += "data_";
  else type += (doHiggsDecays_) ? "higgs_" : "Z0_";
  type += "nano_";
  if(verbose_ > 1)
    cout << "Background training type " << type.Data() << endl;
  TString year_string = "";
  for(int i = 0; i < years_.size(); ++i) {
    if(i > 0) year_string += "_";
    year_string += years_[i];
  }
  TString file_out = Form("background_%s_%s%s_%s_%i.tree",
                          hist_tag_.Data(), type.Data(), selection.Data(), year_string.Data(), set);
  return combine_trees(file_list, fileSelec, set, file_out);
}

//Create TTrees for training MVAs against each decay in all categories
Int_t make_all_backgrounds(TString base = "nanoaods_trees") {
  Int_t status = 0;
  verbose_ = 0;
  combineLepTau_ = false;
  cout << "Making Z decay backgrounds!\n";
  doHiggsDecays_ = false;
  //b-tag cut sets
  status += make_background(8, "mutau"  , base);
  status += make_background(8, "etau"   , base);
  status += make_background(8, "emu"    , base);
  status += make_background(8, "mutau_e", base);
  status += make_background(8, "etau_mu", base);
  cout << "Making Higgs decay backgrounds!\n";
  doHiggsDecays_ = true;
  //b-tag cut sets
  status += make_background(8, "mutau"  , base);
  status += make_background(8, "etau"   , base);
  status += make_background(8, "emu"    , base);
  status += make_background(8, "mutau_e", base);
  status += make_background(8, "etau_mu", base);
  return status;
}

//Create TTrees for all samples for all years
Int_t make_all_years(TString base = "nanoaods_trees") {
  Int_t status = 0;
  // cout << "Making 2016 backgrounds...\n";
  // years_ = {2016};
  // status += make_all_backgrounds(base);
  // cout << "Making 2017 backgrounds...\n";
  // years_ = {2017};
  // status += make_all_backgrounds(base);
  // cout << "Making 2018 backgrounds...\n";
  // years_ = {2018};
  // status += make_all_backgrounds(base);
  //make total background file for all years combined
  cout << "Making Run-II backgrounds...\n";
  years_ = {2016, 2017, 2018};
  status += make_all_backgrounds(base);
  return status;
}
