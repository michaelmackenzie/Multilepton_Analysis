//Approximate the upper limit on the branching fraction using MVA output trees and Higgs Combine
bool dryRun_ = false;
TString MVA_;
TString dir_;

TString get_file_name(TString selection, int set, vector<int> years, int version) {
  TString file = "training_background_clfv_";
  bool ishiggs = selection.Contains("h");
  if(ishiggs) file += "higgs_";
  else        file += "Z0_";
  selection.ReplaceAll("z", ""); selection.ReplaceAll("h","");
  file += "nano_" + selection + "_";
  for(int year : years) {
    file += year;
    file += "_";
  }
  file += set;
  if(version > -1) file += Form("_v%i", version);
  return file;
}

int add_systematic(TString sys_wt, TString sys_name, bool up, TTree* tTrain, TTree* tTest, TString tag, TH1* hBkgNom, TH1* hSigNom) {
  if(sys_wt != "" && !tTrain->GetBranch(sys_wt.Data())) {
    cout << "Undefined systematic weight " << sys_wt.Data() << endl;
    return 1;
  }
  TH1* hBkg = (TH1*) hBkgNom->Clone(Form("bkg_%s", sys_name.Data()));
  TH1* hSig = (TH1*) hSigNom->Clone(Form("sig_%s", sys_name.Data()));
  if(sys_wt == "") { //no weight change
    hBkg->Write();
    hSig->Write();
    return 0;
  }
  hBkg->Reset();
  hSig->Reset();
  TH1* hBkgTrain = (TH1*) hBkg->Clone("hBkgTrain_sys");
  TH1* hBkgTest  = (TH1*) hBkg->Clone("hBkgTest_sys");
  TH1* hSigTrain = (TH1*) hSig->Clone("hSigTrain_sys");
  TH1* hSigTest  = (TH1*) hSig->Clone("hSigTest_sys");
  tTrain->Draw(Form("%s >> hBkgTrain_sys", MVA_.Data()), Form("fulleventweightlum%s%s)*(issignal < 0.5)", (up) ? "*(" : "*(2.-", sys_wt.Data()), "goff");
  tTest ->Draw(Form("%s >> hBkgTest_sys ", MVA_.Data()), Form("fulleventweightlum%s%s)*(issignal < 0.5)", (up) ? "*(" : "*(2.-", sys_wt.Data()), "goff");
  tTrain->Draw(Form("%s >> hSigTrain_sys", MVA_.Data()), Form("fulleventweightlum%s%s)*(issignal > 0.5)", (up) ? "*(" : "*(2.-", sys_wt.Data()), "goff");
  tTest ->Draw(Form("%s >> hSigTest_sys ", MVA_.Data()), Form("fulleventweightlum%s%s)*(issignal > 0.5)", (up) ? "*(" : "*(2.-", sys_wt.Data()), "goff");
  hBkg->Add(hBkgTrain); hBkg->Add(hBkgTest);
  hSig->Add(hSigTrain); hSig->Add(hSigTest);
  hBkg->Write();
  hSig->Write();
  cout << "Systematic: " << sys_name.Data() << " rates: Sig = " << hSig->Integral() << " Bkg = " << hBkg->Integral() << endl;

  TCanvas* c = new TCanvas("c", "c", 800, 1000);
  TPad* pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1.0); pad1->Draw();
  TPad* pad2 = new TPad("pad2", "pad2", 0., 0.0, 1., 0.3); pad2->Draw();
  pad1->cd();

  gStyle->SetOptStat(0);
  hBkgNom->SetFillStyle(0);
  hSigNom->SetFillStyle(0);
  hBkg->SetFillStyle(0);
  hSig->SetFillStyle(0);
  hBkgNom->SetTitle(sys_name.Data());
  hBkgNom->Draw("hist");
  hBkg->SetLineStyle(kDashed);
  hBkg->Draw("hist same");
  hSigNom->Scale(100.);
  hSig->Scale(100.);
  hSigNom->Draw("hist same");
  hSig->SetLineStyle(kDashed);
  hSig->Draw("hist same");

  pad2->cd();
  TH1* hSigRatio = (TH1*) hSig->Clone("hSigRatio");
  hSigRatio->SetTitle("");
  hSigRatio->Divide(hSigNom);
  hSigRatio->Draw("hist");
  TH1* hBkgRatio = (TH1*) hBkg->Clone("hBkgRatio");
  hBkgRatio->Divide(hBkgNom);
  hBkgRatio->Draw("hist same");
  hSigRatio->GetYaxis()->SetRangeUser(0.95, 1.05);
  hSigRatio->GetXaxis()->SetLabelSize(0.07);
  hSigRatio->GetYaxis()->SetLabelSize(0.07);

  TLine* line = new TLine(hSigRatio->GetXaxis()->GetXmin(), 1., hSigRatio->GetXaxis()->GetXmax(), 1.);
  line->SetLineWidth(2);
  line->SetLineColor(kRed);
  line->Draw("same");

  c->SaveAs(Form("%s/%s%s.png", dir_.Data(), tag.Data(), sys_name.Data()));
  delete c;
  delete hSigRatio;
  delete hBkgRatio;

  hSigNom->Scale(1./100.);
  hSig->Scale(1./100.);
  delete hBkgTrain;
  delete hBkgTest;
  delete hSigTrain;
  delete hSigTest;
  return 0;
}

int approx_combine_limit(TString selection = "zmutau", int version = -1,
                 TString MVA = "BDT_MM",
                 int set = 8, vector<int> years = {2016, 2017, 2018}) {
  MVA_ = MVA;
  TString year_string = "";
  for(int year : years) {
    if(year_string == "") year_string += year;
    else                  year_string += Form("_%i", year);
  }

  const int singleCat = selection.Contains("_") + selection.Contains("_h");
  selection.ReplaceAll("_h", ""); //for BXtau_h

  if(singleCat) {
    cout << "Single category not yet implemented!\n";
    return -1;
  }
  if(singleCat) cout << "Using single category " << selection.Data() << endl;

  //Retrieve the training names
  TString nHad = (singleCat != 1) ? get_file_name(selection, set, years, version) : "";
  TString nLep = (singleCat != 2) ? get_file_name(selection+(selection.Contains("mutau") ? "_e" : "_mu"), set, years, version) : "";
  //Get the training files
  TFile* fHad = (singleCat != 1) ? TFile::Open(("../" + nHad + "/" + nHad + ".root").Data(), "READ") : nullptr;
  TFile* fLep = (singleCat != 2) ? TFile::Open(("../" + nLep + "/" + nLep + ".root").Data(), "READ") : nullptr;
  if((!fHad && singleCat != 1) || (!fLep && singleCat !=2)) return 1;

  //Get the TTrees
  TTree* tTrainHad = (fHad) ? (TTree*) fHad->Get(("tmva_" + nHad + "/TrainTree").Data()) : nullptr;
  TTree* tTestHad  = (fHad) ? (TTree*) fHad->Get(("tmva_" + nHad + "/TestTree" ).Data()) : nullptr;
  if(fHad && (!tTrainHad || !tTestHad)) return 2;
  tTrainHad->SetName("TrainHad");
  tTestHad->SetName("TestHad");
  TTree* tTrainLep = (TTree*) fLep->Get(("tmva_" + nLep + "/TrainTree").Data());
  TTree* tTestLep  = (TTree*) fLep->Get(("tmva_" + nLep + "/TestTree").Data());
  if(!tTrainLep || !tTestLep) return 3;
  tTrainLep->SetName("TrainLep");
  tTestLep->SetName("TestLep");

  cout << "Retrieved the trees, creating the background and signal PDFs!\n";

  const int nbins = 50;
  const double xmin(-1.), xmax(1.);
  TH1D* hBkgTrainHad = new TH1D("hBkgTrainHad", "hBkgTrainHad", nbins, xmin, xmax);
  TH1D* hBkgTestHad  = new TH1D("hBkgTestHad" , "hBkgTestHad" , nbins, xmin, xmax);
  TH1D* hBkgTrainLep = new TH1D("hBkgTrainLep", "hBkgTrainLep", nbins, xmin, xmax);
  TH1D* hBkgTestLep  = new TH1D("hBkgTestLep" , "hBkgTestLep" , nbins, xmin, xmax);
  TH1D* hSigTrainHad = new TH1D("hSigTrainHad", "hSigTrainHad", nbins, xmin, xmax);
  TH1D* hSigTestHad  = new TH1D("hSigTestHad" , "hSigTestHad" , nbins, xmin, xmax);
  TH1D* hSigTrainLep = new TH1D("hSigTrainLep", "hSigTrainLep", nbins, xmin, xmax);
  TH1D* hSigTestLep  = new TH1D("hSigTestLep" , "hSigTestLep" , nbins, xmin, xmax);

  tTrainHad->Draw(Form("%s >> hBkgTrainHad", MVA.Data()), "fulleventweightlum*(issignal < 0.5)", "goff");
  tTestHad ->Draw(Form("%s >> hBkgTestHad ", MVA.Data()), "fulleventweightlum*(issignal < 0.5)", "goff");
  tTrainLep->Draw(Form("%s >> hBkgTrainLep", MVA.Data()), "fulleventweightlum*(issignal < 0.5)", "goff");
  tTestLep ->Draw(Form("%s >> hBkgTestLep ", MVA.Data()), "fulleventweightlum*(issignal < 0.5)", "goff");
  tTrainHad->Draw(Form("%s >> hSigTrainHad", MVA.Data()), "fulleventweightlum*(issignal > 0.5)", "goff");
  tTestHad ->Draw(Form("%s >> hSigTestHad ", MVA.Data()), "fulleventweightlum*(issignal > 0.5)", "goff");
  tTrainLep->Draw(Form("%s >> hSigTrainLep", MVA.Data()), "fulleventweightlum*(issignal > 0.5)", "goff");
  tTestLep ->Draw(Form("%s >> hSigTestLep ", MVA.Data()), "fulleventweightlum*(issignal > 0.5)", "goff");

  TH1D* hBkgHad = (TH1D*) hBkgTrainHad->Clone("bkgHad"); hBkgHad->Add(hBkgTestHad);
  TH1D* hBkgLep = (TH1D*) hBkgTrainLep->Clone("bkgLep"); hBkgLep->Add(hBkgTestLep);
  TH1D* hSigHad = (TH1D*) hSigTrainHad->Clone("sigHad"); hSigHad->Add(hSigTestHad);
  TH1D* hSigLep = (TH1D*) hSigTrainLep->Clone("sigLep"); hSigLep->Add(hSigTestLep);

  //make figures
  TString base_name = Form("combine_%s_%s%s_%i_%s",
                           selection.Data(), MVA.Data(),
                           (version > -1) ? Form("_v%i", version) : "",
                           set, year_string.Data());
  dir_ = "figures/" + base_name;
  gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s", dir_.Data(), dir_.Data()));

  TCanvas* c_bkg = new TCanvas("c_bkg", "c_bkg", 1400, 600);
  c_bkg->Divide(2,1);
  hBkgHad->SetLineWidth(2); hBkgHad->SetFillStyle(3005);
  hBkgLep->SetLineWidth(2); hBkgLep->SetFillStyle(3005);
  hSigHad->SetLineWidth(2); hSigHad->SetFillStyle(3004);
  hSigLep->SetLineWidth(2); hSigLep->SetFillStyle(3004);
  hBkgHad->SetLineColor(kRed   ); hBkgHad->SetFillColor(kRed   );
  hBkgLep->SetLineColor(kBlue  ); hBkgLep->SetFillColor(kBlue  );
  hSigHad->SetLineColor(kViolet); hSigHad->SetFillColor(kViolet);
  hSigLep->SetLineColor(kGreen ); hSigLep->SetFillColor(kGreen );
  // gStyle->SetOptStat(0);
  c_bkg->cd(1);
  hSigHad->Scale(100.);
  hSigHad->Draw("hist"); hSigHad->SetTitle("Hadronic MVA scores");
  hBkgHad->Draw("hist same");
  hSigHad->GetYaxis()->SetRangeUser(0.1, 1.2*max(hSigHad->GetMaximum(), hBkgHad->GetMaximum()));
  c_bkg->cd(2);
  hSigLep->Scale(100.);
  hSigLep->Draw("hist"); hSigLep->SetTitle("Leptonic MVA scores");
  hBkgLep->Draw("hist same");
  hSigLep->GetYaxis()->SetRangeUser(0.1, 1.2*max(hSigLep->GetMaximum(), hBkgLep->GetMaximum()));
  c_bkg->SaveAs(Form("%s/nominal.png", dir_.Data()));
  hSigHad->Scale(1./100.);
  hSigLep->Scale(1./100.);

  //////////////////////////////////////////////////
  // Create datacards
  //////////////////////////////////////////////////
  ofstream datacard;
  datacard.open((base_name + ".txt").Data());
  datacard << "# -*- mode: tcl -*-\n"
           << "#Higgs Combine datacard for approximate limit testing\n"
           << "#Automatically generated for CLFVAnalysis work\n";

  CrossSections xs;
  double bxs = selection.Contains("h") ? xs.GetCrossSection("H") : xs.GetCrossSection("Z");
  TString signame = selection;
  signame.ReplaceAll("z", "Z"); signame.ReplaceAll("h", "H"); signame.ReplaceAll("e", "E"); signame.ReplaceAll("m", "M"); signame.ReplaceAll("t", "T");
  double sigxs = xs.GetCrossSection(signame.Data());
  double br_sig = sigxs/bxs;
  datacard << Form("#Using a signal branching fraction of %.3e\n\n", br_sig)
           << "imax 2 number of channels\n"
           << "jmax * number of backgrounds\n"
           << "kmax * number of nuisance parameters\n\n"
           << "-----------------------------------------------------------------------------------------------------------\n\n"
           << Form("shapes * * %s $CHANNEL/$PROCESS $CHANNEL/$PROCESS_$SYSTEMATIC\n\n",
                   (base_name + ".root").Data())
           << "-----------------------------------------------------------------------------------------------------------\n\n"
           << "bin                        had                          lep\n"
           << Form("observation  %18i %28i\n\n", (int) hBkgHad->Integral(), (int) hBkgLep->Integral())
           << "bin                        had                          had"
           << "                        lep                          lep\n"
           << "process                    sig                          bkg"
           << "                        sig                          bkg\n"
           << "process                     0                            1 "
           << "                         0                            2\n\n"
           << Form("rate         %18.1f %28.1f %26.1f %28.1f\n\n",
                   hSigHad->Integral(), hBkgHad->Integral(),
                   hSigLep->Integral(), hBkgLep->Integral())
           << "-----------------------------------------------------------------------------------------------------------\n\n";

  cout << "Hadronic nominal rates: Sig = " << hSigHad->Integral() << " Bkg = " << hBkgHad->Integral() << endl;
  cout << "Leptonic nominal rates: Sig = " << hSigLep->Integral() << " Bkg = " << hBkgLep->Integral() << endl;
  TFile* fout = new TFile((base_name + ".root").Data(), "RECREATE");
  auto hadDir = fout->mkdir("had");
  auto lepDir = fout->mkdir("lep");

  hadDir->cd();
  hBkgHad->SetName("bkg");      hBkgHad->Write();
  hSigHad->SetName("sig");      hSigHad->Write();
  hBkgHad->SetName("data_obs"); hBkgHad->Write();

  lepDir->cd();
  hBkgLep->SetName("bkg");      hBkgLep->Write();
  hSigLep->SetName("sig");      hSigLep->Write();
  hBkgLep->SetName("data_obs"); hBkgLep->Write();


  datacard << Form("%-13s shape         -                            1                          -                            1\n", "JetToTauNC");
  hadDir->cd();
  add_systematic("jettotaunonclosure", "JetToTauNCUp"  , true , tTrainHad, tTestHad, "had", hBkgHad, hSigHad);
  add_systematic("jettotaunonclosure", "JetToTauNCDown", false, tTrainHad, tTestHad, "had", hBkgHad, hSigHad);

  lepDir->cd();
  add_systematic("", "JetToTauNCUp"  , true , tTrainLep, tTestLep, "lep", hBkgLep, hSigLep);
  add_systematic("", "JetToTauNCDown", false, tTrainLep, tTestLep, "lep", hBkgLep, hSigLep);

  if(selection.EndsWith("mutau")) {
    datacard << Form("%-13s shape         -                            1                          -                            1\n", "TauMuID");
    hadDir->cd();
    add_systematic("jetantimu", "TauMuIDUp"  , true , tTrainHad, tTestHad, "had", hBkgHad, hSigHad);
    add_systematic("jetantimu", "TauMuIDDown", false, tTrainHad, tTestHad, "had", hBkgHad, hSigHad);

    lepDir->cd();
    add_systematic("", "TauMuIDUp"  , true , tTrainLep, tTestLep, "lep", hBkgLep, hSigLep);
    add_systematic("", "TauMuIDDown", false, tTrainLep, tTestLep, "lep", hBkgLep, hSigLep);
  } else if(selection.EndsWith("etau")) {
    datacard << Form("%-13s shape         -                            1                          -                            1\n", "TauEleID");
    hadDir->cd();
    add_systematic("jetantiele", "TauEleIDUp"  , true , tTrainHad, tTestHad, "had", hBkgHad, hSigHad);
    add_systematic("jetantiele", "TauEleIDDown", false, tTrainHad, tTestHad, "had", hBkgHad, hSigHad);

    lepDir->cd();
    add_systematic("", "TauEleIDUp"  , true , tTrainLep, tTestLep, "lep", hBkgLep, hSigLep);
    add_systematic("", "TauEleIDDown", false, tTrainLep, tTestLep, "lep", hBkgLep, hSigLep);
  }

  datacard << Form("%-13s shape         -                            1                          -                            1\n", "QCD");
  hadDir->cd();
  add_systematic("", "QCDUp"  , true , tTrainHad, tTestHad, "had", hBkgHad, hSigHad);
  add_systematic("", "QCDDown", false, tTrainHad, tTestHad, "had", hBkgHad, hSigHad);

  lepDir->cd();
  add_systematic("qcdsys", "QCDUp"  , true , tTrainLep, tTestLep, "lep", hBkgLep, hSigLep);
  add_systematic("qcdsys", "QCDDown", false, tTrainLep, tTestLep, "lep", hBkgLep, hSigLep);

  datacard << Form("%-13s shape         -                            1                          -                            1\n", "BTag");
  hadDir->cd();
  add_systematic("btagsys", "BTagUp"  , true , tTrainHad, tTestHad, "had", hBkgHad, hSigHad);
  add_systematic("btagsys", "BTagDown", false, tTrainHad, tTestHad, "had", hBkgHad, hSigHad);

  lepDir->cd();
  add_systematic("btagsys", "BTagUp"  , true , tTrainLep, tTestLep, "lep", hBkgLep, hSigLep);
  add_systematic("btagsys", "BTagDown", false, tTrainLep, tTestLep, "lep", hBkgLep, hSigLep);

  //FIXME: Ignoring ZpT effect on background, since will use embedding eventually
  datacard << Form("%-13s shape         1                            -                          1                            -\n", "ZpT");
  hadDir->cd();
  add_systematic("zptup"  , "ZpTUp"  , true , tTrainHad, tTestHad, "had", hBkgHad, hSigHad);
  add_systematic("zptdown", "ZpTDown", true , tTrainHad, tTestHad, "had", hBkgHad, hSigHad);

  lepDir->cd();
  add_systematic("zptup"  , "ZpTUp"  , true , tTrainLep, tTestLep, "lep", hBkgLep, hSigLep);
  add_systematic("zptdown", "ZpTDown", true , tTrainLep, tTestLep, "lep", hBkgLep, hSigLep);

  fout->Write();
  fout->Close();
  datacard << "\n# * autoMCStats 0\n";

  datacard.close();

  if(!dryRun_) {
    cout << "Passing datacard to Combine, performing estimates using Asimov a priori fits\nFull fit:\n";
    TString base_command = Form("combine -d %s -t -1 --noFitAsimov --run blind --rMin -5 --rMax 5 --rRelAcc 0.0001 --rAbsAcc 0.001",
                                (base_name + ".txt").Data());
    gSystem->Exec(Form("%s 2>&1 | grep \"Expected 50\"", base_command.Data()));
    cout << "No sytematics:\n";
    gSystem->Exec(Form("%s 2>&1 --freezeParameters allConstrainedNuisances | grep \"Expected 50\"", base_command.Data()));
    cout << "No ZpT sytematic:\n";
    gSystem->Exec(Form("%s 2>&1 --freezeParameters ZpT | grep \"Expected 50\"", base_command.Data()));
    cout << "No bTag sytematic:\n";
    gSystem->Exec(Form("%s 2>&1 --freezeParameters BTag | grep \"Expected 50\"", base_command.Data()));
    cout << "No QCD sytematic:\n";
    gSystem->Exec(Form("%s 2>&1 --freezeParameters QCD | grep \"Expected 50\"", base_command.Data()));
    cout << "No j-->tau sytematic:\n";
    gSystem->Exec(Form("%s 2>&1 --freezeParameters JetToTauNC | grep \"Expected 50\"", base_command.Data()));
    if(selection.EndsWith("mutau")) {
      cout << "No mu-->tau sytematic:\n";
      gSystem->Exec(Form("%s 2>&1 --freezeParameters TauMuID  | grep \"Expected 50\"", base_command.Data()));
    } else if(selection.EndsWith("etau")) {
      cout << "No e-->tau sytematic:\n";
      gSystem->Exec(Form("%s 2>&1 --freezeParameters TauEleID  | grep \"Expected 50\"", base_command.Data()));
    }
  }
  return 0;
}
