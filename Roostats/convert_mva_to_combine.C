//Script to prepare a rootfile and datacard for Higgs Combine tools
int verbose_ = 1;

//Get the name of the systematic
TString systematic_name(int sys, TString selection) {
  TString name = "";
  if     (sys ==   1 || sys ==   2) name = "EleID";
  else if(sys ==   4 || sys ==   5) name = "MuonID";
  else if(sys ==  10 || sys ==  11) name = "JetToTau";
  else if(sys ==  13 || sys ==  14) name = "ZpT";
  else if(sys ==  16 || sys ==  17) name = "EleRecoID";
  else if(sys ==  19 || sys ==  20) name = "MuonIsoID";
  else if(sys ==  22 || sys ==  23) name = "TauES";
  else if(sys ==  25 || sys ==  26) name = "QCDScale";
  else if(sys ==  28 || sys ==  29) name = "JetToTauNC";
  else if(selection.Contains("etau")  && (sys ==  37 || sys ==  38)) name = "TauMuID";
  else if(selection.Contains("mutau") && (sys ==  40 || sys ==  41)) name = "TauEleID";
  else if(sys >= 100 && sys < 106) name = Form("TauJetID%i", (sys-100)/2);
  else if(selection.Contains("mutau") && sys >= 110 && sys < 140) name = Form("TauMuID%i", (sys-110)/2);
  else if(selection.Contains("etau")  && sys >= 140 && sys < 158) name = Form("TauEleID%i", (sys-140)/2);
  return name;
}

Int_t convert_mva_to_combine(int set = 8, TString selection = "zmutau",
                             vector<int> years = {2016, 2017, 2018},
                             int seed = 90) {

  //////////////////////////////////////////////////////////////////
  // Initialize relevant variables
  //////////////////////////////////////////////////////////////////

  int status(0);
  TString hist;
  if     (selection == "hmutau"  ) hist = "mva0";
  else if(selection == "zmutau"  ) hist = "mva1";
  else if(selection == "hetau"   ) hist = "mva2";
  else if(selection == "zetau"   ) hist = "mva3";
  else if(selection == "hmutau_e") hist = "mva6";
  else if(selection == "zmutau_e") hist = "mva7";
  else if(selection == "hetau_mu") hist = "mva8";
  else if(selection == "zetau_mu") hist = "mva9";
  else {
    cout << "Unidentified selection " << selection.Data() << endl;
    return -1;
  }

  TString year_string = "";
  for(unsigned i = 0; i < years.size(); ++i) {
    if(i > 0) year_string += "_";
    year_string += years[i];
  }
  int set_offset = 0;
  if     (selection.Contains("_")) set_offset = ZTauTauHistMaker::kEMu;
  else if(selection.Contains("etau")) set_offset = ZTauTauHistMaker::kETau;

  TRandom3* rnd = new TRandom3(seed); //for generating mock data

  //determine the signal name and branching ratio
  TString selec = selection;  selec.ReplaceAll("_e", ""); selec.ReplaceAll("_mu", "");
  TString signame = selec;
  signame.ReplaceAll("z", "Z"); signame.ReplaceAll("h", "H");
  signame.ReplaceAll("m", "M"); signame.ReplaceAll("e", "E"); signame.ReplaceAll("t", "T");
  CrossSections xs;
  double bxs = xs.GetCrossSection((selection.Contains("z")) ? "Z" : "H");
  double xs_sig = xs.GetCrossSection(signame.Data());
  double br_sig = xs_sig / bxs;

  //////////////////////////////////////////////////////////////////
  // Read in the nominal histograms
  //////////////////////////////////////////////////////////////////

  TFile* fInput = TFile::Open(Form("histograms/%s_%s_%i_%s.hist", selection.Data(), hist.Data(), set, year_string.Data()), "READ");
  if(!fInput) return 1;

  THStack* hstack = (THStack*) fInput->Get("hstack");
  if(!hstack) {cout << "Background stack not found!\n"; return 2;}
  TH1D* hbkg = (TH1D*) fInput->Get("hbackground");
  if(!hbkg) {cout << "Background histogram not found!\n"; return 3;}
  TH1D* hsig = (TH1D*) fInput->Get(selec.Data());
  if(!hsig) {cout << "Signal histogram not found!\n"; return 4;}

  //////////////////////////////////////////////////////////////////
  // Read in the systematic histograms
  //////////////////////////////////////////////////////////////////

  vector<THStack*> hsys_stacks;
  vector<TH1D*> hsys_signals;
  for(int isys = 1; isys < 300; isys += 2) {
     //take only the up/down systematics from the sets < 50, skipping the _sys set. Above 50, only up/down
    if(isys < 50 && (isys % 3) == 0) isys +=1;
    if(isys == 49) isys = 50; //skip to get to set 50
    TString name = systematic_name(isys, selection);
    if(name == "") continue;
    if(verbose_ > 0) cout << "Using sys " << isys << ", " << isys+1 << " as systematic " << name.Data() << endl;
    if(name != systematic_name(isys+1, selection)) cout << "!!! Sys " << isys << ", " << isys+1 << " are different!\n";
    THStack* hstack_up   = (THStack*) fInput->Get(Form("hstack_sys_%i", isys));
    THStack* hstack_down = (THStack*) fInput->Get(Form("hstack_sys_%i", isys+1));
    TH1D* hsig_up        = (TH1D*)    fInput->Get(Form("%s_sys_%i", selec.Data(), isys));
    TH1D* hsig_down      = (TH1D*)    fInput->Get(Form("%s_sys_%i", selec.Data(), isys+1));
    if(!hstack_up || !hstack_down || !hsig_up || !hsig_down) continue; //need all 4 to use
    hstack_down->SetTitle(Form("sys_%i", isys)); //store the systematic number in the title
    hstack_up  ->SetTitle(name.Data()); //Form("sys_%i", isys)); //store the systematic name in the title
    hsig_up    ->SetTitle(name.Data()); //Form("sys_%i", isys));
    hsys_stacks.push_back(hstack_up);
    hsys_stacks.push_back(hstack_down);
    hsys_signals.push_back(hsig_up);
    hsys_signals.push_back(hsig_down);
  }

  //////////////////////////////////////////////////////////////////
  // Configure the output file
  //////////////////////////////////////////////////////////////////

  TString outName = Form("combine_mva_%s_%i.root", selection.Data(), set);
  TFile* fOut = new TFile(("datacards/"+year_string+"/"+outName).Data(), "RECREATE");
  auto dir = fOut->mkdir(hist.Data());
  dir->cd();


  //////////////////////////////////////////////////////////////////
  // Generate toy data
  //////////////////////////////////////////////////////////////////

  TH1D* hdata = (TH1D*) hbkg->Clone("data_obs");
  hdata->SetTitle("Toy MC Data");
  for(int ibin = 1; ibin <= hdata->GetNbinsX(); ++ibin) {
    int nentries = rnd->Poisson(hdata->GetBinContent(ibin));
    hdata->SetBinContent(ibin, nentries);
    hdata->SetBinError(ibin, sqrt(nentries));
  }
  hdata->Write();

  //////////////////////////////////////////////////////////////////
  // Configure the data card
  //////////////////////////////////////////////////////////////////

  //Create directory for the data cards if needed
  gSystem->Exec(Form("[ ! -d datacards/%s ] && mkdir -p datacards/%s", year_string.Data(), year_string.Data()));
  TString filepath = Form("datacards/%s/combine_mva_%s_%i.txt", year_string.Data(), selection.Data(), set);
  gSystem->Exec(Form("echo \"# -*- mode: tcl -*-\">| %s", filepath.Data()));
  gSystem->Exec(Form("echo \"#Auto generated counting card for CLFVAnalysis \">> %s", filepath.Data()));
  gSystem->Exec(Form("echo \"#Signal branching fraction used: %.3e \n\">> %s", br_sig, filepath.Data()));
  gSystem->Exec(Form("echo \"imax %2zu number of channels \">> %s", (unsigned long) 1, filepath.Data())); //FIXME: Add multiple channels
  gSystem->Exec(Form("echo \"jmax  * number of backgrounds \">> %s", filepath.Data()));
  gSystem->Exec(Form("echo \"kmax  * number of nuisance parameters \n\">> %s", filepath.Data()));
  gSystem->Exec(Form("echo \"----------------------------------------------------------------------------------------------------------- \">> %s", filepath.Data()));
  gSystem->Exec(Form("echo \"shapes * * %s \\$CHANNEL/\\$PROCESS \\$CHANNEL/\\$PROCESS_\\$SYSTEMATIC\">> %s", outName.Data(), filepath.Data()));
  gSystem->Exec(Form("echo \"----------------------------------------------------------------------------------------------------------- \n\">> %s", filepath.Data()));

  //Start each line, building for each background process
  TString bins   = "bin                     " + hist; //channel definition, 1 per channel
  TString bins_p = "bin          ";                   //per process per channel channel listing
  TString proc_l = "process      ";                   //process definition per channel
  TString proc_c = "process      ";                   //process class per channel
  TString rate   = "rate         ";                   //process rate per channel
  TString obs    = Form("observation  %15.0f",        //data observations, 1 per channel
                        hdata->Integral());

  //////////////////////////////////////////////////////////////////
  // Print the MC values to the card
  //////////////////////////////////////////////////////////////////

  //Add signal first
  bins_p += Form("%15s", hist.Data());
  proc_l += Form("%15s", selection.Data());
  proc_c +=      "            0   ";
  rate   += Form("%15.1f", hsig->Integral());
  hsig->SetName(selection.Data());
  hsig->Write(); //add to the output file
  for(int ihist = 0; ihist < hstack->GetNhists(); ++ihist) {
    TH1D* hbkg_i = (TH1D*) hstack->GetHists()->At(ihist);
    if(!hbkg_i) {cout << "Background hist " << ihist << " not retrieved!\n"; continue;}
    TString hname = hbkg_i->GetName();
    hname.ReplaceAll(Form("_%s_%i", hist.Data(), set+set_offset), "");
    hname.ReplaceAll("s_", "");
    hname.ReplaceAll(" ", "");
    hname.ReplaceAll("#", "");
    hname.ReplaceAll("/", "");
    bins_p += Form("%15s", hist.Data());
    proc_l += Form("%15s", hname.Data());
    proc_c +=      "           1   ";
    rate   += Form("%15.1f", hbkg_i->Integral());
    hbkg_i->SetName(hname.Data());
    hbkg_i->Write(); //add to the output file
  }

  //Print the contents of the card
  gSystem->Exec(Form("echo \"%s \">> %s", bins.Data(), filepath.Data()));
  gSystem->Exec(Form("echo \"%s \n\">> %s", obs.Data() , filepath.Data()));
  gSystem->Exec(Form("echo \"%s \">> %s", bins_p.Data() , filepath.Data()));
  gSystem->Exec(Form("echo \"%s \">> %s", proc_l.Data() , filepath.Data()));
  gSystem->Exec(Form("echo \"%s \n\">> %s", proc_c.Data() , filepath.Data()));
  gSystem->Exec(Form("echo \"%s \n\">> %s", rate.Data() , filepath.Data()));
  gSystem->Exec(Form("echo \"----------------------------------------------------------------------------------------------------------- \n\">> %s", filepath.Data()));

  //make a systematic free copy of the data card
  TString alt_card = filepath; alt_card.ReplaceAll(".txt", "_nosys.txt");
  gSystem->Exec(Form("cp %s %s", filepath.Data(), alt_card.Data()));
  gSystem->Exec(Form("echo \"* autoMCStats 0\n\">> %s", alt_card.Data()));

  //////////////////////////////////////////////////////////////////
  // Print the systematics to the card
  //////////////////////////////////////////////////////////////////

  int nsys = hsys_stacks.size()/2;
  for(int isys = 0; isys < nsys; ++isys) {
    THStack* hstack_up   = hsys_stacks [2*isys  ];
    THStack* hstack_down = hsys_stacks [2*isys+1];
    TH1D*    hsig_up     = hsys_signals[2*isys  ];
    TH1D*    hsig_down   = hsys_signals[2*isys+1];
    TString name = hstack_up->GetTitle();
    if(name == "") continue; //systematic we don't care about
    if(verbose_ > 0) cout << "Processing systematic " << name.Data() << endl;
    TString sys = Form("%-13s shape ", name.Data());
    hsig_up->SetName(Form("%s_%sUp", selection.Data(), name.Data()));
    hsig_up->Write();
    hsig_down->SetName(Form("%s_%sDown", selection.Data(), name.Data()));
    hsig_down->Write();
    sys += Form("%6i", 1);
    for(int ihist = 0; ihist < hstack_up->GetNhists(); ++ihist) {
      TH1D* hbkg_i_up = (TH1D*) hstack_up->GetHists()->At(ihist);
      if(!hbkg_i_up) {cout << "Systematic " << isys << " Background (up) hist " << ihist << " not retrieved!\n"; continue;}
      TH1D* hbkg_i_down = (TH1D*) hstack_down->GetHists()->At(ihist);
      if(!hbkg_i_down) {cout << "Systematic " << isys << " Background (down) hist " << ihist << " not retrieved!\n"; continue;}
      TString hname = hbkg_i_up->GetName();
      TString isys_set = hstack_down->GetTitle();
      isys_set.ReplaceAll("sys_", "");
      hname.ReplaceAll(Form("_%s_%s_%i", hist.Data(), isys_set.Data(), set+set_offset), "");
      hname.ReplaceAll("s_", "");
      hname.ReplaceAll(" ", "");
      hname.ReplaceAll("#", "");
      hname.ReplaceAll("/", "");
      TString hname_up   = Form("%s_%sUp"  , hname.Data(), hstack_up->GetTitle());
      TString hname_down = Form("%s_%sDown", hname.Data(), hstack_up->GetTitle());
      hbkg_i_up->SetName(hname_up.Data());
      hbkg_i_up->Write(); //add to the output file
      hbkg_i_down->SetName(hname_down.Data());
      hbkg_i_down->Write(); //add to the output file
      sys += Form("%15i", 1);
    }
    gSystem->Exec(Form("echo \"%s \">> %s", sys.Data() , filepath.Data()));
  }
  gSystem->Exec(Form("echo \"\n* autoMCStats 0\n\">> %s", filepath.Data()));

  fOut->Close();
  return status;
}
