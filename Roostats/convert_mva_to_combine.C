//Script to prepare a rootfile and datacard for Higgs Combine tools
#include "mva_systematic_names.C"
bool use_fake_bkg_norm_ = false; //add a large uncertainty on j->tau/qcd norm to be fit by data

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
  if     (selection.Contains("_")) set_offset = CLFVHistMaker::kEMu;
  else if(selection.Contains("etau")) set_offset = CLFVHistMaker::kETau;

  TRandom3* rnd = new TRandom3(seed); //for generating mock data
  bool isHiggs = selection.Contains("h");

  //determine the signal name and branching ratio
  TString selec = selection;  selec.ReplaceAll("_e", ""); selec.ReplaceAll("_mu", "");
  TString signame = selec;
  signame.ReplaceAll("z", "Z"); signame.ReplaceAll("h", "H");
  signame.ReplaceAll("m", "M"); signame.ReplaceAll("e", "E"); signame.ReplaceAll("t", "T");
  CrossSections xs;
  double bxs = xs.GetCrossSection(isHiggs ? "H" : "Z");
  double xs_sig = xs.GetCrossSection(signame.Data());
  double br_sig = xs_sig / bxs;
  double sig_scale = 1.;
  if(!useDefaultBr_) { //use fixed example branching fraction
    double example_br = (isHiggs) ? 1.e-3 : 1.e-6;
    sig_scale = example_br / br_sig;
    br_sig = example_br;
  }
  //////////////////////////////////////////////////////////////////
  // Read in the nominal histograms
  //////////////////////////////////////////////////////////////////

  TFile* fInput = TFile::Open(Form("histograms/%s_%s_%i_%s.hist", selection.Data(), hist.Data(), set, year_string.Data()), "READ");
  if(!fInput) return 1;

  if(verbose_ > 1) fInput->ls();

  THStack* hstack = (THStack*) fInput->Get("hstack");
  if(!hstack) {cout << "Background stack not found!\n"; return 2;}
  TH1D* hbkg = (TH1D*) fInput->Get("hbackground");
  if(!hbkg) {cout << "Background histogram not found!\n"; return 3;}
  TH1D* hsig = (TH1D*) fInput->Get(selec.Data());
  if(!hsig) {cout << "Signal histogram not found!\n"; return 4;}
  hsig->Scale(sig_scale);

  //////////////////////////////////////////////////////////////////
  // Read in the systematic histograms
  //////////////////////////////////////////////////////////////////

  vector<THStack*> hsys_stacks;
  vector<TH1D*> hsys_signals;
  for(int isys = 1; isys < 300; isys += 2) {
     //take only the up/down systematics from the sets < 50, skipping the _sys set. Above 50, only up/down
    if(isys < 43 && (isys % 3) == 0) isys +=1;
    if(isys == 49) isys = 50; //skip to get to set 50
    auto sys_info = systematic_name(isys, selection);
    TString name = sys_info.first;
    TString type = sys_info.second;
    if(name == "") continue;
    if(verbose_ > 0) cout << "Using sys " << isys << ", " << isys+1 << " as systematic " << name.Data() << endl;
    if(name != systematic_name(isys+1, selection).first) cout << "!!! Sys " << isys << ", " << isys+1 << " have different names!\n";
    THStack* hstack_up   = (THStack*) fInput->Get(Form("hstack_sys_%i", isys));
    THStack* hstack_down = (THStack*) fInput->Get(Form("hstack_sys_%i", isys+1));
    TH1D* hsig_up        = (TH1D*)    fInput->Get(Form("%s_sys_%i", selec.Data(), isys));
    TH1D* hsig_down      = (TH1D*)    fInput->Get(Form("%s_sys_%i", selec.Data(), isys+1));
    if(!hstack_up || !hstack_down || !hsig_up || !hsig_down) {
      cout << "Systematic histograms for " << name.Data() << " not found!\n"
           << "stack up = " << hstack_up << " stack down = " << hstack_down
           << " sig up = " << hsig_up << " sig down = " << hsig_down << endl;
      continue; //need all 4 to use
    }
    hsig_up->Scale(sig_scale);
    hsig_down->Scale(sig_scale);
    hstack_down->SetTitle(Form("sys_%i", isys)); //store the systematic number in the title
    hstack_up  ->SetTitle(name.Data()); //Form("sys_%i", isys)); //store the systematic name in the title
    hsig_up    ->SetTitle(type.Data()); //Form("sys_%i", isys));
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
  hdata->SetTitle("Asimov Data");
  for(int ibin = 1; ibin <= hdata->GetNbinsX(); ++ibin) {
    double nentries = std::max(0., hdata->GetBinContent(ibin));
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
    hname.ReplaceAll(Form("_%s_1_%i", hist.Data(), set+set_offset), "");
    hname.ReplaceAll(Form("_%s_event_%i", hist.Data(), set+set_offset), "");
    hname.ReplaceAll(Form("_%s_1_event_%i", hist.Data(), set+set_offset), "");
    hname.ReplaceAll("s_", "");
    hname.ReplaceAll(" ", "");
    hname.ReplaceAll("#", "");
    hname.ReplaceAll("/", "");
    hname.ReplaceAll("->", "To");
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
  gSystem->Exec(Form("echo \"# * autoMCStats 0\n\">> %s", alt_card.Data())); //default to commenting out MC uncertainties
  alt_card = filepath; alt_card.ReplaceAll(".txt", "_mcstat.txt");
  gSystem->Exec(Form("cp %s %s", filepath.Data(), alt_card.Data()));
  gSystem->Exec(Form("echo \"* autoMCStats 0\n\">> %s", alt_card.Data())); //default to including MC uncertainties

  //////////////////////////////////////////////////////////////////
  // Print the systematics to the card
  //////////////////////////////////////////////////////////////////

  int nsys = hsys_stacks.size()/2;
  TString qcd_bkg_line = "";
  TString jtt_bkg_line = "";
  for(int isys = 0; isys < nsys; ++isys) {
    THStack* hstack_up   = hsys_stacks [2*isys  ];
    THStack* hstack_down = hsys_stacks [2*isys+1];
    TH1D*    hsig_up     = hsys_signals[2*isys  ];
    TH1D*    hsig_down   = hsys_signals[2*isys+1];
    TString name = hstack_up->GetTitle();
    TString type = hsig_up->GetTitle();
    if(name == "") continue; //systematic we don't care about
    if(verbose_ > 0) cout << "Processing systematic " << name.Data() << endl;
    TString sys = Form("%-13s %5s ", name.Data(), type.Data());
    hsig_up->SetName(Form("%s_%sUp", selection.Data(), name.Data()));
    hsig_up->Write();
    hsig_down->SetName(Form("%s_%sDown", selection.Data(), name.Data()));
    hsig_down->Write();
    bool do_fake_bkg_line = (qcd_bkg_line == "");
    if(do_fake_bkg_line) {
      qcd_bkg_line = Form("%-13s %5s      1", "QCDNorm", "lnN");
      jtt_bkg_line = Form("%-13s %5s      1", "JetToTauNorm", "lnN");
    }
    if(type == "shape") {
      sys += Form("%6i", 1);
    } else {
      double sys_up   = (hsig->Integral() > 0.) ? hsig->Integral()/hsig_up->Integral() : 0.;
      double sys_down = (hsig->Integral() > 0.) ? hsig->Integral()/hsig_down->Integral() : 0.;
      double val = sys_up; //max((sys_up < 1.) ? 1./sys_up : sys_up, (sys_down < 1.) ? 1./sys_down : sys_down);
      sys += Form("%6.3f", val);
    }
    for(int ihist = 0; ihist < hstack_up->GetNhists(); ++ihist) {
      TH1D* hbkg_i_up = (TH1D*) hstack_up->GetHists()->At(ihist);
      if(!hbkg_i_up) {cout << "Systematic " << isys << " Background (up) hist " << ihist << " not retrieved!\n"; continue;}
      TH1D* hbkg_i_down = (TH1D*) hstack_down->GetHists()->At(ihist);
      if(!hbkg_i_down) {cout << "Systematic " << isys << " Background (down) hist " << ihist << " not retrieved!\n"; continue;}
      TH1D* hbkg_i = (TH1D*) hstack->GetHists()->At(ihist);
      if(!hbkg_i) {cout << "Background hist " << ihist << " not retrieved! Exiting...\n"; break;}
      TString hname = hbkg_i_up->GetName();
      TString isys_set = hstack_down->GetTitle();
      isys_set.ReplaceAll("sys_", "");
      hname.ReplaceAll(Form("_%s_%s_%i", hist.Data(), isys_set.Data(), set+set_offset), "");
      hname.ReplaceAll(Form("_%s_%s_systematic_%i", hist.Data(), isys_set.Data(), set+set_offset), "");
      hname.ReplaceAll(Form("_%s_systematic_%i", hist.Data(), set+set_offset), "");
      hname.ReplaceAll(Form("_%s_1_systematic_%i", hist.Data(), set+set_offset), "");
      // hname.ReplaceAll(Form("_%s_%s_systematic_%i", hist.Data(), isys_set.Data(), set+set_offset), "");
      // hname.ReplaceAll(Form("_%s_1_systematic_%i", hist.Data(), set+set_offset), "");
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
      if(do_fake_bkg_line && hname == "QCD")   qcd_bkg_line += Form("%15.3f", 1.30); //additional 30% uncertainty
      else if(do_fake_bkg_line)                qcd_bkg_line += Form("%15i", 1);
      if(do_fake_bkg_line && hname == "MisID") jtt_bkg_line += Form("%15.3f", 1.30); //additional 30% uncertainty
      else if(do_fake_bkg_line)                jtt_bkg_line += Form("%15i", 1);

      if(type == "shape") {
        sys += Form("%15i", 1);
      } else {
        double sys_up   = (hbkg_i->Integral() > 0.) ? hbkg_i->Integral()/hbkg_i_up->Integral() : 0.;
        double sys_down = (hbkg_i->Integral() > 0.) ? hbkg_i->Integral()/hbkg_i_down->Integral() : 0.;
        double val = sys_up; //max((sys_up < 1.) ? 1./sys_up : sys_up, (sys_down < 1.) ? 1./sys_down : sys_down);
        sys += Form("%15.3f", val);
      }
    }
    gSystem->Exec(Form("echo \"%s \">> %s", sys.Data() , filepath.Data()));
  }
  if(use_fake_bkg_norm_) {
    gSystem->Exec(Form("echo \"%s \">> %s", qcd_bkg_line.Data() , filepath.Data()));
    gSystem->Exec(Form("echo \"%s \">> %s", jtt_bkg_line.Data() , filepath.Data()));
  }
  gSystem->Exec(Form("echo \"\n* autoMCStats 0\n\">> %s", filepath.Data()));

  fOut->Close();
  return status;
}
