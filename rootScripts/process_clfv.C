//information for debugging
#include "histogramming_config.C"
bool copyConfig_ = true;

using namespace CLFV;

Int_t count_processes() {
  TString res = gSystem->GetFromPipe("ps -elf | grep process_clfv.C | grep -v grep | awk '{++count}END{print count}'");
  return res.Atoi();
}

Int_t process_clfv() {

  /******************************/
  /* Define NANO AOD processing */
  /******************************/
  TString nanoaod_path;
  vector<datacard_t> nanocards = get_data_cards(nanoaod_path);
  //cross section handler
  bool useUL = nanoaod_path.Contains("_UL");
  CrossSections xs(useUL);

  TStopwatch* timer = new TStopwatch();
  const int copy_num_ = (copyConfig_) ? count_processes() : -1;
  // if(copy_num_ >= 0) gSystem->Exec(Form("cp histogramming_config.C histogramming_config_%i.C", copy_num_));

  //loop over data formats, 0 = bltTrees from ZTauTauAnalyzer, 1 = trees from converting Nano AODS
  Int_t category = 0;
  unsigned nfiles = nanocards.size();
  for(unsigned i = 0; i < nfiles; ++i) {
    ++category; // could have just used i + 1?
    if(nanocards[i].process_)
      cout << "Loop " << i << ", category " << category
           << ", file " << nanocards[i].fname_.Data()
           << ", xsec " << nanocards[i].xsec_
           << ", doProcess " << nanocards[i].process_ << endl;
    if(!nanocards[i].process_) continue;
    //if combining Wlnu samples, weight each by the number of generated over total generated
    int year = 2016;
    if(nanocards[i].fname_.Contains("2017"))      year = 2017;
    else if(nanocards[i].fname_.Contains("2018")) year = 2018;
    nanocards[i].year_ = year;
    if(nanocards[i].combine_) { //only for 2016, 2017 Wlnu samples
      TString name = nanocards[i].fname_;
      name.ReplaceAll("clfv_", "");
      name.ReplaceAll("2016_", "");
      name.ReplaceAll("2017_", "");
      name.ReplaceAll("2018_", "");
      name.ReplaceAll(".tree", "");
      bool isext = name.Contains("-ext");
      name.ReplaceAll("-ext", "");
      long num1 = xs.GetGenNumber(name       , year); //get number of events per sample
      long num2 = xs.GetGenNumber(name+"-ext", year);
      if(isext && num1 > 0 && num2 > 0) {
        double frac = (num2*1./(num1+num2));
        cout << "Multiplying the " << name.Data() << "-ext cross section by " << frac
             << " to combine the extension samples!\n";
        nanocards[i].xsec_ *= frac;
      } else if(num1 > 0 && num2 > 0){
        double frac = (num1*1./(num1+num2));
        cout << "Multiplying the " << name.Data() << " cross section by " << frac
             << " to combine the extension samples!\n";
        nanocards[i].xsec_ *= frac;
      } else
        cout << "ERROR: Didn't find generation numbers for combining with sample name " << name.Data() << endl;
    } //end combine extension samples
    gSystem->Exec(Form("root.exe -q -b -l \"process_card.C(\\\"%s\\\", \\\"%s\\\", %f, %i, %i, %i, %i)\"",
                       nanoaod_path.Data(), nanocards[i].fname_.Data(), nanocards[i].xsec_,
                       nanocards[i].isData_, nanocards[i].combine_, useUL, category
                       )
                  );
    sleep(2); //add 2 sec buffer between loops
  } //end file loop
  //report the time spent histogramming
  Double_t realTime = timer->RealTime();
  printf("Processing time: %7.2fs Wall time\n",realTime);
  if(realTime > 600. ) printf("Processing time: %7.2fmin Wall time\n",realTime/60.);

  return 0;
}
