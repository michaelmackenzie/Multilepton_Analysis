//Script to prepare a rootfile and datacard for Higgs Combine tools
#ifndef __MVA_CARDS__
#define __MVA_CARDS__

#include "mva_systematic_names.C"
#include "mva_process_values.C"

#include <iostream>
#include <fstream>

bool   use_fake_bkg_norm_ = false; //add a large uncertainty on j->tau/qcd norm to be fit by data
bool   use_sys_           =  true; //add systematic uncertainties
bool   separate_years_    =  true; //separate each year of data
int    blind_data_        =    2 ; //0: no blinding; 1: kill high BDT score regions; 2: use ~Asimov instead of data
double blind_cut_         =    0.;

void add_group(map<TString,vector<TString>>& groups, TString sys, TString group) {
  if(groups.find(group) != groups.end()) groups[group].push_back(sys);
  else groups[group] = {sys};
}

//ensure reasonable bin values
void make_safe(TH1* h) {
  for(int ibin = 0; ibin <= h->GetNbinsX()+1; ++ibin) {
    const double binc(h->GetBinContent(ibin));
    const double bine(h->GetBinError  (ibin));
    if(!std::isfinite(binc) || !std::isfinite(bine) || binc < 0.) {
      h->SetBinContent(ibin, 0.);
      h->SetBinError  (ibin, 0.);
    }
    if(binc < bine) {
      h->SetBinError(ibin, binc);
    }
  }
}

Int_t convert_mva_to_combine(int set = 8, TString selection = "zmutau",
                             vector<int> years = {2016, 2017, 2018},
                             int seed = 90) {

  //check if separating by year, and if so call this function for each year
  if(separate_years_ && years.size() > 1) {
    int status = 0;
    for(int year : years) {
      status += convert_mva_to_combine(set, selection, {year}, seed);
    }
    return status;
  }

  //////////////////////////////////////////////////////////////////
  // Initialize relevant variables
  //////////////////////////////////////////////////////////////////

  int status(0);
  TString hist;
  if     (selection == "hmutau"  ) {hist = "mva0"; blind_cut_ =  0.00;}
  else if(selection == "zmutau"  ) {hist = "mva1"; blind_cut_ =  0.00;}
  else if(selection == "hetau"   ) {hist = "mva2"; blind_cut_ =  0.00;}
  else if(selection == "zetau"   ) {hist = "mva3"; blind_cut_ =  0.00;}
  else if(selection == "hmutau_e") {hist = "mva6"; blind_cut_ =  0.00;}
  else if(selection == "zmutau_e") {hist = "mva7"; blind_cut_ = -0.05;}
  else if(selection == "hetau_mu") {hist = "mva8"; blind_cut_ =  0.00;}
  else if(selection == "zetau_mu") {hist = "mva9"; blind_cut_ = -0.05;}
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
  if     (selection.Contains("_")) set_offset = HistMaker::kEMu;
  else if(selection.Contains("etau")) set_offset = HistMaker::kETau;

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
  TH1* hbkg = (TH1*) fInput->Get("hbackground");
  if(!hbkg) {cout << "Background histogram not found!\n"; return 3;}
  TH1* hsig = (TH1*) fInput->Get(selec.Data());
  if(!hsig) {cout << "Signal histogram not found!\n"; return 4;}
  hsig->Scale(sig_scale);
  TH1* hdata = (TH1*) fInput->Get("hdata");
  if(!hdata) {cout << "Data histogram not found!\n"; return 5;}

  make_safe(hsig);
  make_safe(hbkg);

  //////////////////////////////////////////////////////////////////
  // Read in the systematic histograms
  //////////////////////////////////////////////////////////////////

  vector<THStack*> hsys_stacks;
  vector<TH1*> hsys_signals;

  if(use_sys_) {
    TString prev = "";
    const int max_sys = (use_scale_sys_) ? kMaxSystematics+kMaxScaleSystematics : kMaxSystematics;
    for(int isys = 1; isys < max_sys; ++isys) {
      auto sys_info = systematic_name(isys, selection, years[0]); //FIXME: take the first year in the list for now
      TString name = sys_info.first;
      if(name == "") continue;
      if(name == prev) continue; //ensure no repetition
      prev = name;
      TString type = sys_info.second;
      TString down_name = systematic_name(isys+1, selection, years[0]).first;
      if(name != down_name) {
        cout << "!!! Sys " << isys << " (" << name.Data() << "), " << isys+1 << " (" << down_name.Data() << ") have different names! Skipping...\n";
        continue;
      }
      if(verbose_ > 0) cout << "Using sys " << isys << ", " << isys+1 << " as systematic " << name.Data() << endl;
      THStack* hstack_up   = (THStack*) fInput->Get(Form("hstack_sys_%i", isys));
      THStack* hstack_down = (THStack*) fInput->Get(Form("hstack_sys_%i", isys+1));
      TH1* hsig_up        = (TH1*)    fInput->Get(Form("%s_sys_%i", selec.Data(), isys));
      TH1* hsig_down      = (TH1*)    fInput->Get(Form("%s_sys_%i", selec.Data(), isys+1));
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
  }

  //////////////////////////////////////////////////////////////////
  // Configure the output file
  //////////////////////////////////////////////////////////////////

  gSystem->Exec(Form("[ ! -d datacards/%s ] && mkdir -p datacards/%s", year_string.Data(), year_string.Data()));
  TString outName = Form("combine_mva_%s_%i_%s.root", selection.Data(), set, year_string.Data());
  TFile* fOut = new TFile(("datacards/"+year_string+"/"+outName).Data(), "RECREATE");
  auto dir = fOut->mkdir(hist.Data());
  dir->cd();


  //////////////////////////////////////////////////////////////////
  // Generate toy data, if requested
  //////////////////////////////////////////////////////////////////

  TH1* hdata_obs;
  if(blind_data_ == 2) {
    hdata_obs = (TH1*) hbkg->Clone("data_obs");
    hdata_obs->SetTitle("Asimov Data");
    make_safe(hdata_obs);
    for(int ibin = 1; ibin <= hdata_obs->GetNbinsX(); ++ibin) {
      const double nentries = std::max(0., hdata_obs->GetBinContent(ibin));
      hdata_obs->SetBinContent(ibin, nentries);
      hdata_obs->SetBinError(ibin, sqrt(nentries));
    }
  } else {
    hdata_obs = (TH1*) hdata->Clone("data_obs");
    hdata_obs->SetTitle("Observed data");
    if(blind_data_ == 1) {
      for(int ibin = hdata_obs->FindBin(blind_cut_); ibin <= hdata_obs->GetNbinsX(); ++ibin) {
        //kill data
        hdata_obs->SetBinContent(ibin, 0.);
        hdata_obs->SetBinError  (ibin, 0.);
      }
      for(int ibin = hsig->FindBin(blind_cut_); ibin <= hsig->GetNbinsX(); ++ibin) {
        //kill signal
        hsig->SetBinContent(ibin, 0.);
        hsig->SetBinError  (ibin, 0.);
      }
      for(int ibin = hbkg->FindBin(blind_cut_); ibin <= hbkg->GetNbinsX(); ++ibin) {
        //kill background
        hbkg->SetBinContent(ibin, 0.);
        hbkg->SetBinError  (ibin, 0.);
      }
    }
  }
  hdata_obs->Write();

  //////////////////////////////////////////////////////////////////
  // Configure the data card
  //////////////////////////////////////////////////////////////////

  //Create directory for the data cards if needed
  gSystem->Exec(Form("[ ! -d datacards/%s ] && mkdir -p datacards/%s", year_string.Data(), year_string.Data()));
  TString filepath = Form("datacards/%s/combine_mva_%s_%i_%s.txt", year_string.Data(), selection.Data(), set, year_string.Data());
  std::ofstream outfile;
  outfile.open(filepath.Data());
  if(!outfile.is_open()) return 10;

  outfile << "# -*- mode: tcl -*-\n";
  outfile << "#Auto generated counting card for CLFVAnalysis \n";
  outfile << Form("#Signal branching fraction used: %.3e \n\n", br_sig);
  outfile << Form("imax %2zu number of channels \n", (unsigned long) 1); //FIXME: Add multiple channels
  outfile << "jmax  * number of backgrounds \n";
  outfile << "kmax  * number of nuisance parameters \n\n";
  outfile << "----------------------------------------------------------------------------------------------------------- \n";
  outfile << Form("shapes * * %s $CHANNEL/$PROCESS $CHANNEL/$PROCESS_$SYSTEMATIC\n", outName.Data());
  outfile << "----------------------------------------------------------------------------------------------------------- \n\n";

  //Start each line, building for each background process
  TString bins   = "bin                     " + hist; //channel definition, 1 per channel
  TString bins_p = "bin          ";                   //per process per channel channel listing
  TString proc_l = "process      ";                   //process definition per channel
  TString proc_c = "process      ";                   //process class per channel
  TString rate   = "rate         ";                   //process rate per channel
  TString obs    = Form("observation  %15.0f",        //data observations, 1 per channel
                        hdata_obs->Integral());

  map<TString,vector<TString>> groups;

  //////////////////////////////////////////////////////////////////
  // Print the MC values to the card
  //////////////////////////////////////////////////////////////////

  //Add signal first
  TString selec_name = selection;
  selec_name.ReplaceAll("_e", "");
  selec_name.ReplaceAll("_mu", "");
  bins_p += Form("%15s", hist.Data());
  proc_l += Form("%15s", selec_name.Data());
  proc_c +=      "            0   ";
  rate   += Form("%15.3f", hsig->Integral());
  hsig->SetName(selec_name.Data());
  make_safe(hsig);
  hsig->Write(); //add to the output file
  for(int ihist = 0; ihist < hstack->GetNhists(); ++ihist) {
    TH1* hbkg_i = (TH1*) hstack->GetHists()->At(ihist);
    if(!hbkg_i) {cout << "Background hist " << ihist << " not retrieved!\n"; continue;}
    //kill sensitive region in data and MC if requested
    if(blind_data_ == 1) {
      for(int ibin = hbkg_i->FindBin(blind_cut_); ibin <= hbkg_i->GetNbinsX(); ++ibin) {
        //kill background
        hbkg_i->SetBinContent(ibin, 0.);
        hbkg_i->SetBinError  (ibin, 0.);
      }
    }

    TString hname = hbkg_i->GetName();
    hname = hname(0, hname.First('_'));
    hname.ReplaceAll(" ", "");
    hname.ReplaceAll("#", "");
    hname.ReplaceAll("/", "");
    hname.ReplaceAll("->", "To");
    hname.ReplaceAll("tautauEmbedding", "Embedding"); //shorten  the embedding name
    bins_p += Form("%15s", hist.Data());
    proc_l += Form("%15s", hname.Data());
    proc_c += Form("          %2i   ", process_value(hname));
    rate   += Form("%15.3f", hbkg_i->Integral());
    hbkg_i->SetName(hname.Data());

    //if embedding, increase bin-by-bin uncertainties to account for binomial gen-weight uncertainty
    if(embed_mode_ && embed_bin_stats_mode_ && hname.Contains("Embedding")) {
      //only apply to emu data (if using emu embedding) unless embed_bin_stat_mode_ = 2
      if((embed_mode_ == 1 && selection.Contains("_")) || (embed_bin_stats_mode_ == 2 && selection.EndsWith("tau"))) {
        const double p = (years[0] == 2016) ? 0.017 : (years[0] == 2017) ? 0.014 : 0.019;
        //approximate the binomial error with N = 1000
        const int N = 1000;
        const double binomial_error = std::sqrt(p*(1.-p)/N);
        //increase uncertainty by: sqrt(p^2 + error(p)^2) / p = sqrt(1 + error(p)^2/p^2)
        const double bin_error_scale = std::sqrt(1. + binomial_error*binomial_error/(p*p));
        printf("--> Increasing Embedding bin uncertainties by %.4f\n", bin_error_scale);
        for(int ibin = 1; ibin <= hbkg_i->GetNbinsX(); ++ibin) {
          const double bin_c = hbkg_i->GetBinContent(ibin);
          if(bin_c <= 0.) continue;
          const double bin_e = hbkg_i->GetBinError(ibin);
          if(bin_e <= 0.) continue;
          hbkg_i->SetBinError(ibin, bin_e*bin_error_scale);
        }
      }
    }
    make_safe(hbkg_i);
    hbkg_i->Write(); //add to the output file
  }

  //Print the contents of the card
  outfile << Form("%s \n", bins.Data());
  outfile << Form("%s \n\n", obs.Data());
  outfile << Form("%s \n", bins_p.Data());
  outfile << Form("%s \n", proc_l.Data());
  outfile << Form("%s \n\n", proc_c.Data());
  outfile << Form("%s \n\n", rate.Data());
  outfile << "----------------------------------------------------------------------------------------------------------- \n\n";

  //////////////////////////////////////////////////////////////////
  // Print the systematics to the card
  //////////////////////////////////////////////////////////////////

  int nsys = hsys_stacks.size()/2;
  TString qcd_bkg_line = "";
  TString jtt_bkg_line = "";
  const int nbkg_proc = hstack->GetNhists();

  for(int isys = 0; isys < nsys; ++isys) {
    THStack* hstack_up   = hsys_stacks [2*isys  ];
    THStack* hstack_down = hsys_stacks [2*isys+1];
    TH1*    hsig_up     = hsys_signals[2*isys  ];
    TH1*    hsig_down   = hsys_signals[2*isys+1];
    TString name = hstack_up->GetTitle();
    TString type = hsig_up->GetTitle();
    if(name == "") continue; //systematic we don't care about
    if(verbose_ > 0) cout << "Processing systematic " << name.Data() << " with type " << type.Data() << endl;

    if(hstack_up->GetNhists() != nbkg_proc || hstack_down->GetNhists() != nbkg_proc) {
      cout << "!!! Systematic " << name.Data() << " up/down stacks don't match the expected number of processes!\n";
      // continue;
    }
    //kill sensitive region if requested
    if(blind_data_ == 1) {
      for(int ibin = hsig_up->FindBin(blind_cut_); ibin <= hsig_up->GetNbinsX(); ++ibin) {
        //kill region
        hsig_up  ->SetBinContent(ibin, 0.);
        hsig_up  ->SetBinError  (ibin, 0.);
        hsig_down->SetBinContent(ibin, 0.);
        hsig_down->SetBinError  (ibin, 0.);
      }
    }

    TString sys = Form("%-13s %5s ", name.Data(), type.Data());
    hsig_up->SetName(Form("%s_%sUp", selec_name.Data(), name.Data()));
    make_safe(hsig_up);
    hsig_up->Write();
    hsig_down->SetName(Form("%s_%sDown", selec_name.Data(), name.Data()));
    make_safe(hsig_down);
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
      double val = sys_up; //use up to preserve correlation directions //(std::fabs(1.-sys_up) > std::fabs(1.-sys_down)) ? sys_up : sys_down;
      sys += Form("%6.4f", val);
    }
    for(int ihist = 0; ihist < nbkg_proc; ++ihist) {
      TH1* hbkg_i = (TH1*) hstack->GetHists()->At(ihist);
      if(!hbkg_i) {cout << "Background hist " << ihist << " not retrieved! Exiting...\n"; break;}
      TH1* hbkg_i_up = (TH1*) hstack_up->GetHists()->At(ihist);
      if(!hbkg_i_up) {
        cout << "Systematic " << isys << " (" << name.Data() << ") Background (up) hist "
                           << hbkg_i->GetTitle() << " not retrieved!\n";
        sys += Form("%6s", "-"); continue;
      }
      TH1* hbkg_i_down = (TH1*) hstack_down->GetHists()->At(ihist);
      if(!hbkg_i_down) {
        cout << "Systematic " << isys << " (" << name.Data() << ") Background (down) hist "
                           << hbkg_i->GetTitle() << " not retrieved!\n";
        sys += Form("%6s", "-"); continue;
      }

      //kill sensitive region if requested
      if(blind_data_ == 1) {
        for(int ibin = hbkg_i_up->FindBin(blind_cut_); ibin <= hbkg_i_up->GetNbinsX(); ++ibin) {
          //kill region
          hbkg_i_up  ->SetBinContent(ibin, 0.);
          hbkg_i_up  ->SetBinError  (ibin, 0.);
          hbkg_i_down->SetBinContent(ibin, 0.);
          hbkg_i_down->SetBinError  (ibin, 0.);
        }
      }
      //Hack to allow 0 rate histograms
      if(hbkg_i_up  ->Integral() == 0.) hbkg_i_up  ->SetBinContent(1, 1.e-5);
      if(hbkg_i_down->Integral() == 0.) hbkg_i_down->SetBinContent(1, 1.e-5);

      TString hname = hbkg_i_up->GetName();
      hname = hname(0, hname.First('_'));
      hname.ReplaceAll(" ", "");
      hname.ReplaceAll("#", "");
      hname.ReplaceAll("/", "");
      hname.ReplaceAll("->", "To");
      hname.ReplaceAll("tautauEmbedding", "Embedding"); //shorten  the embedding name
      TString hname_up   = Form("%s_%sUp"  , hname.Data(), hstack_up->GetTitle());
      TString hname_down = Form("%s_%sDown", hname.Data(), hstack_up->GetTitle());
      hbkg_i_up->SetName(hname_up.Data());
      make_safe(hbkg_i_up);
      hbkg_i_up->Write(); //add to the output file
      hbkg_i_down->SetName(hname_down.Data());
      make_safe(hbkg_i_down);
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

      //Add to a group list if a standard nuisance group
      if(name.BeginsWith("JetToTau")                    ) add_group(groups, name, "JetToTau_Total"    );
      if(name.BeginsWith("JetToTauAlt")                 ) add_group(groups, name, "JetToTau_Stat"     );
      if(name.BeginsWith("JetToTauNC")                  ) add_group(groups, name, "JetToTau_NC"       );
      if(name.BeginsWith("JetToTauBias")                ) add_group(groups, name, "JetToTau_Bias"     );
      if(name.BeginsWith("JetToTauComp")                ) add_group(groups, name, "JetToTau_Comp"     );
      if(name.BeginsWith("QCD")                         ) add_group(groups, name, "QCD_Total"         );
      if(name.BeginsWith("QCDAlt")                      ) add_group(groups, name, "QCD_Stat"          );
      if(name.BeginsWith("QCDNC")                       ) add_group(groups, name, "QCD_NC"            );
      if(name.BeginsWith("QCDBias")                     ) add_group(groups, name, "QCD_Bias"          );
      if(name.BeginsWith("JER")                         ) add_group(groups, name, "JER_JES"           );
      if(name.BeginsWith("JES")                         ) add_group(groups, name, "JER_JES"           );
      if(name.BeginsWith("BTag")                        ) add_group(groups, name, "BTag_Total"        );
      if(name.Contains("TauJetID")                      ) add_group(groups, name, "TauJetID_Total"    );
      if(name.BeginsWith("TauEleID")                    ) add_group(groups, name, "TauEleID_Total"    );
      if(name.BeginsWith("TauMuID")                     ) add_group(groups, name, "TauMuID_Total"     );
      if(name.Contains("MuonID")                        ) add_group(groups, name, "MuonID_Total"      );
      if(name.Contains("MuonIsoID")                     ) add_group(groups, name, "MuonID_Total"      );
      if(name.Contains("EleID") && !name.Contains("Tau")) add_group(groups, name, "EleID_Total"       );
      if(name.Contains("EleIsoID")                      ) add_group(groups, name, "EleID_Total"       );
      if(name.Contains("EleRecoID")                     ) add_group(groups, name, "EleID_Total"       );
      if(name.Contains("MuonES")                        ) add_group(groups, name, "MuonES_Total"      );
      if(name.Contains("EleES")                         ) add_group(groups, name, "EleES_Total"       );
      if(name.Contains("TauES")                         ) add_group(groups, name, "TauES_Total"       );
      if(name.Contains("EleTrig")                       ) add_group(groups, name, "EleTrig_Total"     );
      if(name.Contains("MuonTrig")                      ) add_group(groups, name, "MuonTrig_Total"    );
      if(name.Contains("ZPt")                           ) add_group(groups, name, "ZPt_Total"         );
      if(name.Contains("Pileup")                        ) add_group(groups, name, "Pileup_Total"      );
      if(name.Contains("Prefire")                       ) add_group(groups, name, "Prefire_Total"     );
      if(name.Contains("Theory")                        ) add_group(groups, name, "Theory_Total"      );
      if(name.Contains("XS_Embed")                      ) add_group(groups, name, "EmbedUnfold_Total" );
      else if(name.Contains("Lumi")                     ) add_group(groups, name, "Lumi_Total"        );
      else if(name.BeginsWith("XS_")                    ) add_group(groups, name, "XSec_Total"        );
    }
    outfile << Form("%s \n", sys.Data());
    if(verbose_ > 3) cout << sys.Data() << endl;
  }
  //print the groups
  outfile << "\n";
  for(auto group : groups) {
    if(verbose_ > 0) cout << "Adding group " << group.first.Data() << endl;
    outfile << Form("%-13s group =", group.first.Data());
    for(TString isys : group.second) outfile << " " << isys.Data();
    outfile << "\n";
  }

  if(use_fake_bkg_norm_) {
    outfile << Form("%s \n", qcd_bkg_line.Data());
    outfile << Form("%s \n", jtt_bkg_line.Data());
  }
  outfile << "\n* autoMCStats 0 1 1\n\n";
  outfile.close();

  fOut->Close();
  return status;
}

#endif
