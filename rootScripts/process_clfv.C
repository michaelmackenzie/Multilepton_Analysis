//information for debugging
#include "histogramming_config.C"
#include "process_card.C"
bool copyConfig_   = true; //create a new config file to use to prevent changes
bool newProcess_   = true; //run card processing in a new process to avoid memory issues
int  maxProcesses_ = 4; //maximum number of new processes to run at once

using namespace CLFV;

Int_t count_processes() {
  TString res = gSystem->GetFromPipe("ps -elf | grep ${USER} | grep process_card.C | grep -v grep | awk '{++count}END{print count}'");
  return res.Atoi();
}

Int_t process_clfv() {

  /******************************/
  /* Define NANO AOD processing */
  /******************************/
  TString nanoaod_path;
  vector<datacard_t> nanocards = get_data_cards(nanoaod_path);
  //cross section handler
  CrossSections xs(useUL_);

  TStopwatch* timer = new TStopwatch();

  //ensure the log directory exists
  gSystem->Exec("[ ! -d log ] && mkdir log");

  //loop over data formats, 0 = bltTrees from CLFVAnalyzer, 1 = trees from converting Nano AODS
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
      name.ReplaceAll("LFVAnalysis_", "");
      name.ReplaceAll("2016.root", "");
      name.ReplaceAll("2017.root", "");
      name.ReplaceAll("2018.root", "");
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
    if(newProcess_) {
      while(count_processes() >= maxProcesses_) {
        sleep(10); //wait to submit until fewer than the maximum are running
      }
      gSystem->Exec(Form("root.exe -q -b -l \"process_card.C(\\\"%s\\\", \\\"%s\\\", %f, %i, %i, %i)\" >| log/out_%i.log 2>&1 &",
                         nanoaod_path.Data(), nanocards[i].fname_.Data(), nanocards[i].xsec_,
                         nanocards[i].isData_, nanocards[i].combine_, category, category
                         )
                    );
      sleep(2); //add 2 sec buffer between loops
    } else { //process within this process
      process_card(nanoaod_path.Data(), nanocards[i].fname_.Data(), nanocards[i].xsec_,
                   nanocards[i].isData_, nanocards[i].combine_, category
                   );
    }
  } //end file loop
  while(count_processes() > 0) { //wait for all jobs to finish
    sleep(10);
  }
  //report the time spent histogramming
  const Double_t realTime = timer->RealTime();
  if(realTime > 180. ) printf("Processing time: %7.2fmin Wall time\n",realTime/60.);
  else                 printf("Processing time: %7.2fs Wall time\n"  ,realTime);

  return 0;
}
