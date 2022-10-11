//Script to prepare a rootfile and datacard for Higgs Combine tools

#include "mva_systematic_names.C"
#include "convert_mva_to_combine.C"

Int_t control_region_datacard(int set = 8, TString selection = "mumu",
                              TString signal = "zmutau",
                              vector<int> years = {2016, 2017, 2018},
                              int seed = 90) {

  //check if separating by year, and if so call this function for each year
  if(separate_years_ && years.size() > 1) {
    int status = 0;
    for(int year : years) {
      status += control_region_datacard(set, selection, signal, {year}, seed);
    }
    return status;
  }

  //////////////////////////////////////////////////////////////////
  // Initialize relevant variables
  //////////////////////////////////////////////////////////////////

  int status(0);
  TString hist = "lepm";
  if(selection != "mumu" && selection != "ee") {
    cout << "Unidentified selection " << selection.Data() << endl;
    return -1;
  }

  TString year_string = "";
  for(unsigned i = 0; i < years.size(); ++i) {
    if(i > 0) year_string += "_";
    year_string += years[i];
  }

  const int set_offset = (selection == "ee") ? HistMaker::kEE : HistMaker::kMuMu;
  const bool isHiggs = signal.Contains("h");

  //determine the signal name and branching ratio
  TString selec = signal;  selec.ReplaceAll("_e", ""); selec.ReplaceAll("_mu", "");
  TString signame = selec;
  signame.ReplaceAll("z", "Z"); signame.ReplaceAll("h", "H");
  signame.ReplaceAll("m", "M"); signame.ReplaceAll("e", "E"); signame.ReplaceAll("t", "T");
  CrossSections xs;
  const double bxs = xs.GetCrossSection(isHiggs ? "H" : "Z");
  const double xs_sig = xs.GetCrossSection(signame.Data());
  double br_sig = xs_sig / bxs;
  double sig_scale = 1.;
  if(!useDefaultBr_) { //use fixed example branching fraction
    const double example_br = (isHiggs) ? 1.e-3 : 1.e-6;
    sig_scale = example_br / br_sig;
    br_sig = example_br;
  }
  //////////////////////////////////////////////////////////////////
  // Read in the nominal histograms
  //////////////////////////////////////////////////////////////////

  TFile* fInput = TFile::Open(Form("histograms/%s_CR_lepm_%i_%s.hist", selection.Data(), set, year_string.Data()), "READ");
  if(!fInput) return 1;

  if(verbose_ > 1) fInput->ls();

  THStack* hstack = (THStack*) fInput->Get("hstack");
  if(!hstack) {cout << "Background stack not found!\n"; return 2;}
  TH1* hbkg = (TH1*) fInput->Get("hbackground");
  if(!hbkg) {cout << "Background histogram not found!\n"; return 3;}
  TH1* hsig = (TH1*) fInput->Get(selec.Data());
  if(!hsig) {
    cout << "Signal histogram not found! Continuing by setting its rate to 0...\n";
  }
  else {
    hsig->Scale(sig_scale);
  }
  //////////////////////////////////////////////////////////////////
  // Read in the systematic histograms
  //////////////////////////////////////////////////////////////////

  vector<THStack*> hsys_stacks;
  vector<TH1*> hsys_signals;
  for(int isys = 1; isys < 300; isys += 2) {
     //take only the up/down systematics from the sets < 50, skipping the _sys set. Above 50, only up/down
    if(isys < 43 && (isys % 3) == 0) isys +=1;
    // if(isys == 49) isys = 50; //skip to get to set 50
    if(isys == 99) isys = 100; //skip to get to 100
    auto sys_info = systematic_name(isys, signal, years[0]); //FIXME: take the first year for now
    TString name = sys_info.first;
    TString type = sys_info.second;
    if(name == "" || name.Contains("Tau")) continue;
    if(verbose_ > 0) cout << "Using sys " << isys << ", " << isys+1 << " as systematic " << name.Data() << endl;
    if(name != systematic_name(isys+1, signal, years[0]).first) cout << "!!! Sys " << isys << ", " << isys+1 << " have different names!\n";
    THStack* hstack_up   = (THStack*) fInput->Get(Form("hstack_sys_%i", isys));
    THStack* hstack_down = (THStack*) fInput->Get(Form("hstack_sys_%i", isys+1));
    TH1* hsig_up        = (TH1*)    fInput->Get(Form("%s_sys_%i", selec.Data(), isys));
    TH1* hsig_down      = (TH1*)    fInput->Get(Form("%s_sys_%i", selec.Data(), isys+1));
    if(!hstack_up || !hstack_down) {
      cout << "Systematic histograms for " << name.Data() << " not found!\n"
           << "stack up = " << hstack_up << " stack down = " << hstack_down
           << " sig up = " << hsig_up << " sig down = " << hsig_down << endl;
      continue; //need both background stacks
    }
    if(hsig_up) {
      hsig_up->Scale(sig_scale);
      hsig_up->SetTitle(type.Data()); //Form("sys_%i", isys));
    } else hsig_up = new TH1D(Form("hsig_%i_up", isys), type.Data(), 1, 0, 1);
    if(hsig_down) {
      hsig_down->Scale(sig_scale);
      hsig_down->SetTitle(type.Data()); //Form("sys_%i", isys));
    } else hsig_down = new TH1D(Form("hsig_%i_down", isys), type.Data(), 1, 0, 1);
    hstack_down->SetTitle(Form("sys_%i", isys)); //store the systematic number in the title
    hstack_up  ->SetTitle(name.Data()); //Form("sys_%i", isys)); //store the systematic name in the title
    hsys_stacks.push_back(hstack_up);
    hsys_stacks.push_back(hstack_down);
    hsys_signals.push_back(hsig_up);
    hsys_signals.push_back(hsig_down);
  }

  //////////////////////////////////////////////////////////////////
  // Configure the output file
  //////////////////////////////////////////////////////////////////

  TString outName = Form("combine_mva_%s_cr_%s_%i_%s.root", signal.Data(), selection.Data(), set, year_string.Data());
  TFile* fOut = new TFile(("datacards/"+year_string+"/"+outName).Data(), "RECREATE");
  auto dir = fOut->mkdir(Form("%sCount", selection.Data()));
  dir->cd();


  //////////////////////////////////////////////////////////////////
  // Generate toy data
  //////////////////////////////////////////////////////////////////

  const double mass_min =  70.01;
  const double mass_max = 109.99;
  double error;

  TH1D* hdata = new TH1D("data_obs", "Asimov data", 1, 0, 1);
  hdata->SetBinContent(1, hbkg->Integral(hbkg->FindBin(mass_min), hbkg->FindBin(mass_max)));
  hdata->SetBinError(1, sqrt(hdata->GetBinContent(1)));
  hdata->Write();

  //////////////////////////////////////////////////////////////////
  // Configure the data card
  //////////////////////////////////////////////////////////////////

  //Create directory for the data cards if needed
  gSystem->Exec(Form("[ ! -d datacards/%s ] && mkdir -p datacards/%s", year_string.Data(), year_string.Data()));
  TString filepath = Form("datacards/%s/combine_mva_%s_cr_%s_%i_%s.txt", year_string.Data(), signal.Data(), selection.Data(), set, year_string.Data());
  gSystem->Exec(Form("echo \"# -*- mode: tcl -*-\">| %s", filepath.Data()));
  gSystem->Exec(Form("echo \"#Auto generated counting card for CLFVAnalysis \">> %s", filepath.Data()));
  gSystem->Exec(Form("echo \"#Signal branching fraction used: %.3e \n\">> %s", br_sig, filepath.Data()));
  gSystem->Exec(Form("echo \"imax %2zu number of channels \">> %s", (unsigned long) 1, filepath.Data()));
  gSystem->Exec(Form("echo \"jmax  * number of backgrounds \">> %s", filepath.Data()));
  gSystem->Exec(Form("echo \"kmax  * number of nuisance parameters \n\">> %s", filepath.Data()));
  gSystem->Exec(Form("echo \"----------------------------------------------------------------------------------------------------------- \">> %s", filepath.Data()));
  gSystem->Exec(Form("echo \"shapes * * %s \\$CHANNEL/\\$PROCESS \\$CHANNEL/\\$PROCESS_\\$SYSTEMATIC\">> %s", outName.Data(), filepath.Data()));
  gSystem->Exec(Form("echo \"----------------------------------------------------------------------------------------------------------- \n\">> %s", filepath.Data()));

  //Start each line, building for each background process
  TString bins   = "bin                  " + selection + "Count"; //channel definition, 1 per channel
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
  if(!hsig) {
    hsig = new TH1D(signal.Data(), signal.Data(), 1, 0, 1);
  } else {
    double nsig = hsig->IntegralAndError(hsig->FindBin(mass_min), hsig->FindBin(mass_max), error);
    hsig = new TH1D(hsig->GetName(), hsig->GetTitle(), 1, 0, 1);
    hsig->SetBinContent(1, nsig);
    hsig->SetBinError(1, error);
  }
  double nominal_sig = hsig->Integral();
  vector<double> nominal_bkgs;
  
  bins_p += Form("%15s", (selection+"Count").Data());
  proc_l += Form("%15s", signal.Data());
  proc_c +=      "            0   ";
  rate   += Form("%15.1f", hsig->Integral());
  hsig->SetName(signal.Data());
  hsig->Write(); //add to the output file
  for(int ihist = 0; ihist < hstack->GetNhists(); ++ihist) {
    TH1* hbkg_i = (TH1*) hstack->GetHists()->At(ihist);
    if(!hbkg_i) {cout << "Background hist " << ihist << " not retrieved!\n"; continue;}
    TString hname = hbkg_i->GetName();
    hname.ReplaceAll(Form("_%s_%i", hist.Data(), set+set_offset), "");
    hname.ReplaceAll(Form("_%s_event_%i", hist.Data(), set+set_offset), "");
    hname.ReplaceAll("s_", "");
    hname.ReplaceAll(" ", "");
    hname.ReplaceAll("#", "");
    hname.ReplaceAll("/", "");
    hname.ReplaceAll("->", "To");
    hname.ReplaceAll("tautauEmbedding", "Embedding"); //shorten  the embedding name
    const double nbkg = hbkg_i->IntegralAndError(hbkg_i->FindBin(mass_min), hbkg_i->FindBin(mass_max), error);
    hbkg_i = new TH1D(hname.Data(), hbkg_i->GetTitle(), 1, 0, 1);
    hbkg_i->SetBinContent(1, nbkg);
    hbkg_i->SetBinError(1, error);
    nominal_bkgs.push_back(nbkg);
    bins_p += Form("%15s", (selection+"Count").Data());
    proc_l += Form("%15s", hname.Data());
    proc_c += Form("          %2i   ", process_value(hname));
    rate   += Form("%15.1f", hbkg_i->Integral());
    hbkg_i->Write(); //add to the output file
    if(nominal_bkgs[ihist] <= 0.) cout << "Nominal backgroud count for " << hname.Data() << " is <= 0 = " << nominal_bkgs[ihist] << endl;
  }

  //Print the contents of the card
  gSystem->Exec(Form("echo \"%s \">> %s", bins.Data(), filepath.Data()));
  gSystem->Exec(Form("echo \"%s \n\">> %s", obs.Data() , filepath.Data()));
  gSystem->Exec(Form("echo \"%s \">> %s", bins_p.Data() , filepath.Data()));
  gSystem->Exec(Form("echo \"%s \">> %s", proc_l.Data() , filepath.Data()));
  gSystem->Exec(Form("echo \"%s \n\">> %s", proc_c.Data() , filepath.Data()));
  gSystem->Exec(Form("echo \"%s \n\">> %s", rate.Data() , filepath.Data()));
  gSystem->Exec(Form("echo \"----------------------------------------------------------------------------------------------------------- \n\">> %s", filepath.Data()));


  //////////////////////////////////////////////////////////////////
  // Print the systematics to the card
  //////////////////////////////////////////////////////////////////

  int nsys = hsys_stacks.size()/2;
  for(int isys = 0; isys < nsys; ++isys) {
    THStack* hstack_up   = hsys_stacks [2*isys  ];
    THStack* hstack_down = hsys_stacks [2*isys+1];
    TH1*     hsig_up     = hsys_signals[2*isys  ];
    TH1*     hsig_down   = hsys_signals[2*isys+1];
    TString name = hstack_up->GetTitle();
    TString type = hsig_up->GetTitle();
    if(name == "") continue; //systematic we don't care about
    if(verbose_ > 0) cout << "Processing systematic " << name.Data() << endl;
    if(hstack->GetNhists() != hstack_up->GetNhists() || hstack->GetNhists() != hstack_down->GetNhists()) {
      cout << "Stacks for systematic " << name.Data() << " have different number of hists!\n";
      continue;
    }
    {
      double nsig = hsig_up->IntegralAndError(hsig_up->FindBin(mass_min), hsig_up->FindBin(mass_max), error);
      hsig_up = new TH1D(hsig_up->GetName(), hsig_up->GetTitle(), 1, 0, 1);
      hsig_up->SetBinContent(1, nsig);
      hsig_up->SetBinError(1, error);
    }
    {
      double nsig = hsig_down->IntegralAndError(hsig_down->FindBin(mass_min), hsig_down->FindBin(mass_max), error);
      hsig_down = new TH1D(hsig_down->GetName(), hsig_down->GetTitle(), 1, 0, 1);
      hsig_down->SetBinContent(1, nsig);
      hsig_down->SetBinError(1, error);
    }
    TString sys = Form("%-13s %5s ", name.Data(), type.Data());
    hsig_up->SetName(Form("%s_%sUp", signal.Data(), name.Data()));
    hsig_up->Write();
    hsig_down->SetName(Form("%s_%sDown", signal.Data(), name.Data()));
    hsig_down->Write();
    if(type == "shape") {
      sys += Form("%6i", (name.Contains("Tau") ? 0 : 1));
    } else {
      double sys_up   = (nominal_sig > 0.) ? nominal_sig/hsig_up->Integral() : 1.;
      double sys_down = (nominal_sig > 0.) ? nominal_sig/hsig_down->Integral() : 1.;
      double val = sys_up; //max((sys_up < 1. && sys_up > 0.) ? 1./sys_up : sys_up, (sys_down < 1. && sys_down > 0.) ? 1./sys_down : sys_down);
      sys += Form("%6.3f", val);
    }
    for(int ihist = 0; ihist < hstack_up->GetNhists(); ++ihist) {
      TH1D* hbkg_i_up = (TH1D*) hstack_up->GetHists()->At(ihist);
      if(!hbkg_i_up) {cout << "Systematic " << isys << " Background (up) hist " << ihist << " not retrieved!\n"; continue;}
      TH1* hbkg_i_down = (TH1*) hstack_down->GetHists()->At(ihist);
      if(!hbkg_i_down) {cout << "Systematic " << isys << " Background (down) hist " << ihist << " not retrieved!\n"; continue;}
      TH1* hbkg_i = (TH1*) hstack->GetHists()->At(ihist);
      if(!hbkg_i) {cout << "Background hist " << ihist << " not retrieved! Exiting...\n"; break;}
      double nbkg = hbkg_i_up->IntegralAndError(hbkg_i_up->FindBin(mass_min), hbkg_i_up->FindBin(mass_max), error);
      hbkg_i_up = new TH1D(hbkg_i_up->GetName(), hbkg_i_up->GetTitle(), 1, 0, 1);
      hbkg_i_up->SetBinContent(1, nbkg);
      hbkg_i_up->SetBinError(1, error);
      nbkg = hbkg_i_down->IntegralAndError(hbkg_i_down->FindBin(mass_min), hbkg_i_down->FindBin(mass_max), error);
      hbkg_i_down = new TH1D(hbkg_i_down->GetName(), hbkg_i_down->GetTitle(), 1, 0, 1);
      hbkg_i_down->SetBinContent(1, nbkg);
      hbkg_i_down->SetBinError(1, error);

      TString hname = hbkg_i_up->GetName();
      TString isys_set = hstack_down->GetTitle();
      isys_set.ReplaceAll("sys_", "");
      hname.ReplaceAll(Form("_%s_%s_%i", hist.Data(), isys_set.Data(), set+set_offset), "");
      hname.ReplaceAll(Form("_%s_%s_systematic_%i", hist.Data(), isys_set.Data(), set+set_offset), "");
      hname.ReplaceAll(Form("_%s_systematic_%i", hist.Data(), set+set_offset), "");
      hname.ReplaceAll("s_", "");
      hname.ReplaceAll(" ", "");
      hname.ReplaceAll("#", "");
      hname.ReplaceAll("/", "");
      hname.ReplaceAll("->", "To");
      TString hname_up   = Form("%s_%sUp"  , hname.Data(), hstack_up->GetTitle());
      TString hname_down = Form("%s_%sDown", hname.Data(), hstack_up->GetTitle());
      hbkg_i_up->SetName(hname_up.Data());
      hbkg_i_up->Write(); //add to the output file
      hbkg_i_down->SetName(hname_down.Data());
      hbkg_i_down->Write(); //add to the output file
      if(type == "shape") {
        sys += Form("%15i", (name.Contains("Tau") ? 0 : 1));
      } else {
        double sys_up   = (nominal_bkgs[ihist] > 0.) ? nominal_bkgs[ihist]/hbkg_i_up->Integral() : 1.;
        double sys_down = (nominal_bkgs[ihist] > 0.) ? nominal_bkgs[ihist]/hbkg_i_down->Integral() : 1.;
        double val = sys_up; //max((sys_up < 1. && sys_up > 0.) ? 1./sys_up : sys_up, (sys_down < 1. && sys_down > 0.) ? 1./sys_down : sys_down);
        sys += Form("%15.3f", val);
      }
    }
    gSystem->Exec(Form("echo \"%s \">> %s", sys.Data() , filepath.Data()));
  }
  gSystem->Exec(Form("echo \"\n* autoMCStats 0\n\">> %s", filepath.Data()));

  fOut->Close();
  return status;
}
