//Script to combine Higgs Combine data cards
#include <glob.h>

int combine_channels(int set = 8, TString selection = "zmutau",
                     vector<int> years = {2016, 2017, 2018}) {

  //////////////////////////////////////////////////////////////////
  // Initialize relevant variables
  //////////////////////////////////////////////////////////////////

  int status(0);
  TString year_string = "";
  for(unsigned i = 0; i < years.size(); ++i) {
    if(i > 0) year_string += "_";
    year_string += years[i];
  }

  std::string filepath = Form("datacards/%s/combine_mva_%s*_%i.txt",
                              year_string.Data(), selection.Data(), set);

  TString outName = Form("datacards/%s/combine_mva_total_%s_%i.txt",
                         year_string.Data(), selection.Data(), set);

  TString outRootName = Form("combine_mva_total_%s_%i.root",
                             selection.Data(), set);

  //////////////////////////////////////////////////////////////////
  // Get the list of files to process
  //////////////////////////////////////////////////////////////////

  glob_t g;
  glob(filepath.c_str(), GLOB_TILDE, nullptr, &g);
  vector<std::string> filelist;
  filelist.reserve(g.gl_pathc);
  cout << "There are " << g.gl_pathc << " files to process\n";
  for(size_t i = 0; i < g.gl_pathc; ++i) {
    filelist.emplace_back(g.gl_pathv[i]);
  }
  globfree(&g);

  //////////////////////////////////////////////////////////////////
  // Initialize output parameters
  //////////////////////////////////////////////////////////////////

  vector<TString> preamble;
  TString shapeline = Form("shapes * * %s \\$CHANNEL/\\$PROCESS \\$CHANNEL/\\$PROCESS_\\$SYSTEMATIC",
                           outRootName.Data());
  TString channels  = "bin        ";
  TString obs       = "observation";
  TString pro_p_ch  = "bin        ";
  TString processes = "process    ";
  TString pro_cat   = "process    ";
  TString rate      = "rate       ";
  TString filler = "-----------------------------------------------------------------------------------------------------------";
  std::map<TString, TString> systematics; //shape uncertainties
  std::map<TString, TString> systematics_lnN; //normalization uncertainties

  //////////////////////////////////////////////////////////////////
  // Read the input data cards
  //////////////////////////////////////////////////////////////////
  int nchannels = 0;
  for(size_t ifile = 0; ifile < filelist.size(); ++ifile) {
    if(filelist[ifile].find("_cr_") != string::npos) continue; //don't combine control regions
    ++nchannels;
    cout << "Reading file " << filelist[ifile].c_str() << endl;
    std::ifstream file(filelist[ifile].c_str());
    std::string str;
    bool first_bin = true;
    bool first_pro = true;
    while(std::getline(file, str)){
      TString current = str.c_str();
      if(current.Contains("#")) { //part of preamble
        if(ifile != 0) continue; //only add this once
        preamble.push_back(current);
      } else if(current.Contains("shapes ")) { //shape line --> don't need
        continue;
      } else if(current.Contains("--------")) { //just a filler line, skip
        continue;
      } else if(current.Contains("bin        ") && first_bin) { //line that defines channel
        first_bin = false;
        current.ReplaceAll("bin        ", "");
        channels += current;
        continue;
      } else if(current.Contains("observation")) { //channel observation
        current.ReplaceAll("observation", "");
        obs += current;
        continue;
      } else if(current.Contains("bin        ")) { //line that defines process per channel
        current.ReplaceAll("bin        ", "");
        pro_p_ch += current;
      } else if(current.Contains("process    ") && first_pro) { //line that defines each process
        first_pro = false;
        current.ReplaceAll("process    ", "");
        processes += current;
      } else if(current.Contains("process    ")) { //line that defines the process category
        current.ReplaceAll("process    ", "");
        pro_cat += current;
      } else if(current.Contains("rate       ")) { //line that defines the process category
        current.ReplaceAll("rate       ", "");
        rate += current;
      } else if(current.Contains(" shape ")) { //line that defines the systematic shape uncertainties
        TObjArray* split_line = current.Tokenize(" ");
        TString sys = ((TObjString*) split_line->At(0))->String();
        if(systematics.find(sys) != systematics.end()) { //already in the map
          current.ReplaceAll("shape", "   ");
          if(sys.Sizeof() < 7) //remove the extra space for 0-9 systematics
            current.ReplaceAll((sys+" ").Data(), "");
          else
            current.ReplaceAll(sys.Data(), "");
          systematics[sys] = systematics[sys] + current;
        } else {
          systematics[sys] = current;
        }
        continue;
      } else if(current.Contains(" lnN ")) { //line that defines the systematic lnN uncertainties
        TObjArray* split_line = current.Tokenize(" ");
        TString sys = ((TObjString*) split_line->At(0))->String();
        if(systematics_lnN.find(sys) != systematics_lnN.end()) { //already in the map
          current.ReplaceAll("lnN", "   ");
          if(sys.Sizeof() < 7) //remove the extra space for 0-9 systematics
            current.ReplaceAll((sys+" ").Data(), "");
          else
            current.ReplaceAll(sys.Data(), "");
          systematics_lnN[sys] = systematics_lnN[sys] + current;
        } else {
          systematics_lnN[sys] = current;
        }
        continue;
      }
    }
  }


  //////////////////////////////////////////////////////////////////
  // Write the output data card
  //////////////////////////////////////////////////////////////////
  for(size_t index = 0; index < preamble.size(); ++index) {
    if(index == 0)
      gSystem->Exec(Form("echo \"%s \" >| %s", preamble[index].Data(), outName.Data()));
    else
      gSystem->Exec(Form("echo \"%s \" >> %s", preamble[index].Data(), outName.Data()));
  }
  gSystem->Exec(Form("echo \"\nimax  %2i number of channels \" >> %s", nchannels, outName.Data()));
  gSystem->Exec(Form("echo \"jmax   * number of backgrounds \" >> %s", outName.Data()));
  gSystem->Exec(Form("echo \"kmax   * number of nuisance parameters \n\" >> %s", outName.Data()));
  gSystem->Exec(Form("echo \"%s \" >> %s", filler.Data(), outName.Data()));
  gSystem->Exec(Form("echo \"%s \" >> %s", shapeline.Data(), outName.Data()));
  gSystem->Exec(Form("echo \"%s \n\" >> %s", filler.Data(), outName.Data()));

  gSystem->Exec(Form("echo \"%s \" >> %s", channels.Data() , outName.Data()));
  gSystem->Exec(Form("echo \"%s \" >> %s", obs.Data()      , outName.Data()));
  gSystem->Exec(Form("echo \"%s \" >> %s", pro_p_ch.Data() , outName.Data()));
  gSystem->Exec(Form("echo \"%s \" >> %s", processes.Data(), outName.Data()));
  gSystem->Exec(Form("echo \"%s \" >> %s", pro_cat.Data()  , outName.Data()));
  gSystem->Exec(Form("echo \"%s \n\" >> %s", rate.Data()     , outName.Data()));
  gSystem->Exec(Form("echo \"%s \n\" >> %s", filler.Data(), outName.Data()));

  //add a systematic free version
  TString alt_file = outName; alt_file.ReplaceAll(".txt", "_nosys.txt");
  gSystem->Exec(Form("cp %s %s", outName.Data(), alt_file.Data()));
  gSystem->Exec(Form("echo \"# * autoMCStats 0\n\" >> %s", alt_file.Data()));
  alt_file = outName; alt_file.ReplaceAll(".txt", "_mcstat.txt");
  gSystem->Exec(Form("cp %s %s", outName.Data(), alt_file.Data()));
  gSystem->Exec(Form("echo \"* autoMCStats 0\n\" >> %s", alt_file.Data()));

  //print systematics
  std::map<TString, TString>::iterator itr;
  for(itr = systematics.begin(); itr != systematics.end(); itr++) {
    gSystem->Exec(Form("echo \"%s \" >> %s", itr->second.Data(), outName.Data()));
  }
  for(itr = systematics_lnN.begin(); itr != systematics_lnN.end(); itr++) {
    gSystem->Exec(Form("echo \"%s \" >> %s", itr->second.Data(), outName.Data()));
  }
  gSystem->Exec(Form("echo \"\n* autoMCStats 0\n\" >> %s", outName.Data()));

  //////////////////////////////////////////////////////////////////
  // Merge the relevant data files
  //////////////////////////////////////////////////////////////////
  gSystem->Exec(Form("hadd -f datacards/%s/%s `ls -d datacards/%s/combine_mva_%s*_%i.root | grep -v _cr_`",
                     year_string.Data(), outRootName.Data(), year_string.Data(), selection.Data(), set));

  return status;
}
