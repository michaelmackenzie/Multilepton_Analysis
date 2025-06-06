//information for debugging
#include "histogramming_config.C"
#include "process_card.C"

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
  config_t config(get_config());
  vector<TString>& selections = config.selections_;

  if(selections.size() > 0) {
    cout << "Selections for histogramming: { ";
    for(TString selection : selections) {
      cout << selection.Data() << " ";
    }
    cout << "}\n";
  }

  TStopwatch* timer = new TStopwatch();

  //ensure the log directory exists
  gSystem->Exec("[ ! -d log ] && mkdir log");

  //loop over data formats, 0 = bltTrees from CLFVAnalyzer, 1 = trees from converting Nano AODS
  Int_t category = 0;
  unsigned nfiles = nanocards.size();
  for(unsigned i = 0; i < nfiles; ++i) {
    ++category; // could have just used i + 1?
    if(!nanocards[i].process_) continue;
    if(tag_.size() > 0) {
      bool skip = true;
      for(TString tag : tag_) {
        if(tag == "" || nanocards[i].fname_.Contains(tag.Data())) {skip = false; break;}
      }
      if(skip) continue;
    }
    if(veto_.size() > 0) {
      bool skip = false;
      for(TString veto : veto_) {
        if(veto != "" && nanocards[i].fname_.Contains(veto.Data())) skip = true;
      }
      if(skip) continue;
    }
    cout << "Loop " << i << ", category " << category
         << ", file " << nanocards[i].fname_.Data()
         << ", xsec " << nanocards[i].xsec_
         << endl;
    //if combining Wlnu samples, weight each by the number of generated over total generated
    int year = 2016;
    if(nanocards[i].fname_.Contains("2017"))      year = 2017;
    else if(nanocards[i].fname_.Contains("2018")) year = 2018;
    nanocards[i].year_ = year;
    if(nanocards[i].combine_) { //only for 2016, 2017 Wlnu samples
      TString name = nanocards[i].fname_;
      name.ReplaceAll("LFVAnalysis_", "");
      name.ReplaceAll("_2016.root", "");
      name.ReplaceAll("_2017.root", "");
      name.ReplaceAll("_2018.root", "");
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
      for(TString selection : selections) {
        TString command = Form("root.exe -q -b -l \"process_card.C(\\\"%s\\\", \\\"%s\\\", %f, %i, %i, %i",
                               nanoaod_path.Data(), nanocards[i].fname_.Data(), nanocards[i].xsec_,
                               nanocards[i].isData_, nanocards[i].combine_, category
                               );
        if(splitSelecs_) command += Form(", \\\"%s\\\"", selection.Data());
        command += Form(")\" >| log/out_%i%s.log 2>&1 &", category, (splitSelecs_) ? ("_"+selection).Data() : "");
        if(splitSelecs_ && selections.size() > 1) printf(" Submitting %-8s histogramming...\n", selection.Data());
        gSystem->Exec(command.Data());
        // TString return_val = gSystem->GetFromPipe(Form("grep \"(int) \" log/out_%i.log| tail -n 1", category));
        // if(return_val != "(int) 0") printf("--> File %s (category %i) didn't pass processing!\n", nanocards[i].fname_.Data(), category);
        while(count_processes() >= maxProcesses_) {
          sleep(10); //wait to submit until fewer than the maximum are running
        }
        sleep(2); //add 2 sec buffer between loops
        if(!splitSelecs_) break; //submit only once if the job is processing all selections
      }
    } else { //process within this process
      process_card(nanoaod_path.Data(), nanocards[i].fname_.Data(), nanocards[i].xsec_,
                   nanocards[i].isData_, nanocards[i].combine_, category
                   );
    }
  } //end file loop
  while(newProcess_ && count_processes() > 0) { //wait for all jobs to finish
    sleep(10);
  }
  //report the time spent histogramming
  const Double_t realTime = timer->RealTime();
  if(realTime > 180. ) printf("Processing time: %7.2fmin Wall time\n",realTime/60.);
  else                 printf("Processing time: %7.2fs Wall time\n"  ,realTime);

  return 0;
}
