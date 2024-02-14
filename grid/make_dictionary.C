//Read the list of datasets and output a python dictionary for them
#include "../rootScripts/histogramming_config.C"

int make_dictionary() {
  TString path;
  auto cards = get_data_cards(path);
  std::ofstream outfile;
  outfile.open("file_dictionary.py");
  if(!outfile.is_open()) return 10;
  outfile << "# Store information about datasets to process\n\n"
          << "def add_samples(sampleDict):\n"
          << "  sampleDict[\"2016_mc\"   ] = []\n"
          << "  sampleDict[\"2016_embed\"] = []\n"
          << "  sampleDict[\"2016_data\" ] = []\n"
          << "  sampleDict[\"2017_mc\"   ] = []\n"
          << "  sampleDict[\"2017_embed\"] = []\n"
          << "  sampleDict[\"2017_data\" ] = []\n"
          << "  sampleDict[\"2018_mc\"   ] = []\n"
          << "  sampleDict[\"2018_embed\"] = []\n"
          << "  sampleDict[\"2018_data\" ] = []\n";
  for(auto card : cards) {
    TString file = card.fname_;
    int year = 2016;
    if(file.Contains("2017")) year = 2017;
    if(file.Contains("2018")) year = 2018;
    const char* tag = (card.isData_) ? "data" : (file.Contains("Embed")) ? "embed" : "mc";
    outfile << Form("  sampleDict[%-12s].append(%-50s);\n", Form("\"%i_%s\"", year, tag), Form("\"%s\"", file.Data()));
  }
  outfile.close();
  return 0;
}
