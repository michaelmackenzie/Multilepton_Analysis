//Read/Write data information

class DataInfo : public TObject {
public:

  DataInfo(int set = 8, TString tag = "muon") : set_(set), tag_(tag){
  }

  //read data
  void ReadData() {
    std::ifstream file(Form("data/data_%s_set_%i.txt", tag_.Data(), set_));
    std::string str;
    while(std::getline(file, str)){
      std::string year_s, num_s;
      std::istringstream stream_s(str);
      std::getline(stream_s, year_s, ' ');
      std::getline(stream_s, num_s, ' ');
      datamap_[std::stoi(year_s)] = std::stod(num_s);
    }
  }

  //write data
  void WriteData() {
    TString filename = Form("data/data_%s_set_%i.txt", tag_.Data(), set_);
    gSystem->Exec("[ ! -d data ] && mkdir data");
    gSystem->Exec(Form("rm %s", filename.Data()));
    auto itr = datamap_.begin();
    while(itr != datamap_.end()) {
      gSystem->Exec(Form("echo \"%i %.2f\" >> %s", itr->first, itr->second, filename.Data()));
      itr++;
    }
  }

  int set_;
  TString tag_;
  std::map<int, double> datamap_;
};
