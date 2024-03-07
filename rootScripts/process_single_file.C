//process histogramming of a single dataset file

#include "histogramming_config.C"
#include "process_card.C"

using namespace CLFV;

Int_t count_processes() {
  TString res = gSystem->GetFromPipe("ps -elf | grep process_card.C | grep -v grep | awk '{++count}END{print count}'");
  return res.Atoi();
}

Int_t process_single_file(TString file) {

  /******************************/
  /* Define NANO AOD processing */
  /******************************/
  TString nanoaod_path;
  vector<datacard_t> nanocards = get_data_cards(nanoaod_path);
  //cross section handler
  CrossSections xs(useUL_);
  datacard_t card;
  bool found = false;
  for(auto card_i : nanocards) {
    if(card_i.fname_ == file) {
      found = true;
      card = card_i;
      break;
    }
  }
  if(!found) {
    printf("ERROR: Dataset %s not found, exiting!\n", file.Data());
    return 1;
  }

  TStopwatch* timer = new TStopwatch();

  if(newProcess_) gSystem->Exec("[ ! -d log ] && mkdir log");

  cout << "File " << card.fname_.Data()
       << ", xsec " << card.xsec_
       << endl;
  //if combining Wlnu samples, weight each by the number of generated over total generated
  int year = 2016;
  if(card.fname_.Contains("2017"))      year = 2017;
  else if(card.fname_.Contains("2018")) year = 2018;
  card.year_ = year;
  if(card.combine_) { //only for 2016, 2017 Wlnu samples
    TString name = card.fname_;
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
      card.xsec_ *= frac;
    } else if(num1 > 0 && num2 > 0){
      double frac = (num1*1./(num1+num2));
      cout << "Multiplying the " << name.Data() << " cross section by " << frac
           << " to combine the extension samples!\n";
      card.xsec_ *= frac;
    } else
      cout << "ERROR: Didn't find generation numbers for combining with sample name " << name.Data() << endl;
  } //end combine extension samples

  //check for W+jets, where multiple versions are processed at once
  if(card.fname_.Contains("Wlnu_") || card.fname_.Contains("Wlnu-ext_")) maxProcesses_ = 1; //only process 1 selection at once

  //Process the card
  if(newProcess_) {
    config_t config(get_config());
    vector<TString>& selections = config.selections_;
    for(TString selection : selections) {
      TString command = Form("root.exe -q -b -l \"process_card.C(\\\"%s\\\", \\\"%s\\\", %f, %i, %i, 1",
                             nanoaod_path.Data(), card.fname_.Data(), card.xsec_,
                             card.isData_, card.combine_);
      if(splitSelecs_) command += Form(", \\\"%s\\\"", selection.Data());
      command += Form(")\" >| log/out%s.log 2>&1 &", (splitSelecs_) ? ("_"+selection).Data() : "");
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
    process_card(nanoaod_path.Data(), card.fname_.Data(), card.xsec_,
                 card.isData_, card.combine_, 1
                 );
  }

  while(newProcess_ && count_processes() > 0) { //wait for all jobs to finish
    sleep(10);
  }

  //report the time spent histogramming
  const Double_t realTime = timer->RealTime();
  if(realTime > 180. ) printf("Processing time: %7.2fmin Wall time\n",realTime/60.);
  else                 printf("Processing time: %7.2fs Wall time\n"  ,realTime);

  return 0;
}
