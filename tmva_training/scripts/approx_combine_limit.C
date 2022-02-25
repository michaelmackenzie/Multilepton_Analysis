//Approximate the upper limit on the branching fraction using MVA output trees and Higgs Combine
bool dryRun_ = false;

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

int approx_combine_limit(TString selection = "zmutau", int version = -1,
                 TString MVA = "BDT_MM",
                 int set = 8, vector<int> years = {2016, 2017, 2018}) {

  TString year_string = "";
  for(int year : years) {
    if(year_string == "") year_string += year;
    else                  year_string += Form("_%i", year);
  }

  //Retrieve the training names
  TString nHad = get_file_name(selection, set, years, version);
  TString nLep = get_file_name(selection+(selection.Contains("mutau") ? "_e" : "_mu"), set, years, version);
  //Get the training files
  TFile* fHad = TFile::Open(("../" + nHad + "/" + nHad + ".root").Data(), "READ");
  TFile* fLep = TFile::Open(("../" + nLep + "/" + nLep + ".root").Data(), "READ");
  if(!fHad || !fLep) return 1;

  //Get the TTrees
  TTree* tTrainHad = (TTree*) fHad->Get(("tmva_" + nHad + "/TrainTree").Data());
  TTree* tTestHad  = (TTree*) fHad->Get(("tmva_" + nHad + "/TestTree").Data());
  if(!tTrainHad || !tTestHad) return 2;
  tTrainHad->SetName("TrainHad");
  tTestHad->SetName("TestHad");
  TTree* tTrainLep = (TTree*) fLep->Get(("tmva_" + nLep + "/TrainTree").Data());
  TTree* tTestLep  = (TTree*) fLep->Get(("tmva_" + nLep + "/TestTree").Data());
  if(!tTrainLep || !tTestLep) return 3;
  tTrainLep->SetName("TrainLep");
  tTestLep->SetName("TestLep");

  cout << "Retrieved the trees, creating the background and signal PDFs!\n";

  TH1D* hBkgTrainHad = new TH1D("hBkgTrainHad", "hBkgTrainHad", 50, -1., 1.);
  TH1D* hBkgTestHad  = new TH1D("hBkgTestHad" , "hBkgTestHad" , 50, -1., 1.);
  TH1D* hBkgTrainLep = new TH1D("hBkgTrainLep", "hBkgTrainLep", 50, -1., 1.);
  TH1D* hBkgTestLep  = new TH1D("hBkgTestLep" , "hBkgTestLep" , 50, -1., 1.);
  TH1D* hSigTrainHad = new TH1D("hSigTrainHad", "hSigTrainHad", 50, -1., 1.);
  TH1D* hSigTestHad  = new TH1D("hSigTestHad" , "hSigTestHad" , 50, -1., 1.);
  TH1D* hSigTrainLep = new TH1D("hSigTrainLep", "hSigTrainLep", 50, -1., 1.);
  TH1D* hSigTestLep  = new TH1D("hSigTestLep" , "hSigTestLep" , 50, -1., 1.);

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

  //Plot the histograms
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
  hSigHad->Draw("hist"); hSigHad->SetTitle("Hadronic MVA scores");
  hBkgHad->Draw("hist same");
  c_bkg->cd(2);
  hSigLep->Draw("hist"); hSigLep->SetTitle("Leptonic MVA scores");
  hBkgLep->Draw("hist same");

  //////////////////////////////////////////////////
  // Create datacards
  //////////////////////////////////////////////////
  TString base_name = Form("combine_%s_%s%s_%i_%s",
                           selection.Data(), MVA.Data(),
                           (version > -1) ? Form("_v%i", version) : "",
                           set, year_string.Data());
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
  datacard << Form("#Using a signal branching fraction of %.3e\n", br_sig)
           << "imax 2 number of channels\n"
           << "jmax * number of backgrounds\n"
           << "kmax * number of nuisance parameters\n\n"
           << "-----------------------------------------------------------------------------------------------------------\n"
           << Form("shapes * * %s $CHANNEL/$PROCESS $CHANNEL/$PROCESS_$SYSTEMATIC\n\n",
                   (base_name + ".root").Data())
           << "-----------------------------------------------------------------------------------------------------------\n\n"
           << "bin                        had                          lep\n"
           << Form("observation  %19i %19i\n\n", (int) hBkgHad->Integral(), (int) hBkgLep->Integral())
           << "bin                        had                          had"
           << "                        lep                          lep\n"
           << "process                    sig                          bkg"
           << "                        sig                          bkg\n"
           << "process                     0                            1 "
           << "                         0                            1\n\n"
           << Form("rate         %19.1f %19.1f %19.1f %19.1f\n\n",
                   hSigHad->Integral(), hBkgHad->Integral(),
                   hSigLep->Integral(), hBkgLep->Integral())
           << "-----------------------------------------------------------------------------------------------------------\n\n";

  //Add systemtatics
  datacard << Form("%-13s shape         -                            1                          -                            1\n", "JetToTauNC");
  // <<Form("%-13s shape         1                            1                          1                            1\n", "ZpT");
  datacard << "\n# * autoMCStatsu 0\n"; //start with statistics ignored
  datacard.close();

  TFile* fout = new TFile((base_name + ".root").Data(), "RECREATE");
  auto hadDir = fout->mkdir("had");
  auto lepDir = fout->mkdir("lep");

  tTrainHad->Draw(Form("%s >> hBkgTrainHad", MVA.Data()), "fulleventweightlum*jettotaunonclosure*(issignal < 0.5)", "goff");
  tTestHad ->Draw(Form("%s >> hBkgTestHad ", MVA.Data()), "fulleventweightlum*jettotaunonclosure*(issignal < 0.5)", "goff");
  tTrainLep->Draw(Form("%s >> hBkgTrainLep", MVA.Data()), "fulleventweightlum*jettotaunonclosure*(issignal < 0.5)", "goff");
  tTestLep ->Draw(Form("%s >> hBkgTestLep ", MVA.Data()), "fulleventweightlum*jettotaunonclosure*(issignal < 0.5)", "goff");
  tTrainHad->Draw(Form("%s >> hSigTrainHad", MVA.Data()), "fulleventweightlum*jettotaunonclosure*(issignal > 0.5)", "goff");
  tTestHad ->Draw(Form("%s >> hSigTestHad ", MVA.Data()), "fulleventweightlum*jettotaunonclosure*(issignal > 0.5)", "goff");
  tTrainLep->Draw(Form("%s >> hSigTrainLep", MVA.Data()), "fulleventweightlum*jettotaunonclosure*(issignal > 0.5)", "goff");
  tTestLep ->Draw(Form("%s >> hSigTestLep ", MVA.Data()), "fulleventweightlum*jettotaunonclosure*(issignal > 0.5)", "goff");
  hadDir->cd();
  hBkgHad->SetName("bkg"); hBkgHad->Write(); hBkgHad->SetName("data_obs"); hBkgHad->Write();
  hSigHad->SetName("sig"); hSigHad->Write();
  hBkgHad = (TH1D*) hBkgTrainHad->Clone("bkg_JetToTauNCUp"); hBkgHad->Add(hBkgTestHad);
  hSigHad = (TH1D*) hSigTrainHad->Clone("sig_JetToTauNCUp"); hSigHad->Add(hSigTestHad);
  hBkgHad->Write();
  hSigHad->Write();
  lepDir->cd();
  hBkgLep->SetName("bkg"); hBkgLep->Write(); hBkgLep->SetName("data_obs"); hBkgLep->Write();
  hSigLep->SetName("sig"); hSigLep->Write();
  hBkgLep = (TH1D*) hBkgTrainLep->Clone("bkg_JetToTauNCUp"); hBkgLep->Add(hBkgTestLep);
  hSigLep = (TH1D*) hSigTrainLep->Clone("sig_JetToTauNCUp"); hSigLep->Add(hSigTestLep);
  hBkgLep->Write();
  hSigLep->Write();
  tTrainHad->Draw(Form("%s >> hBkgTrainHad", MVA.Data()), "(fulleventweightlum / jettotaunonclosure)*(issignal < 0.5)", "goff");
  tTestHad ->Draw(Form("%s >> hBkgTestHad ", MVA.Data()), "(fulleventweightlum / jettotaunonclosure)*(issignal < 0.5)", "goff");
  tTrainLep->Draw(Form("%s >> hBkgTrainLep", MVA.Data()), "(fulleventweightlum / jettotaunonclosure)*(issignal < 0.5)", "goff");
  tTestLep ->Draw(Form("%s >> hBkgTestLep ", MVA.Data()), "(fulleventweightlum / jettotaunonclosure)*(issignal < 0.5)", "goff");
  tTrainHad->Draw(Form("%s >> hSigTrainHad", MVA.Data()), "(fulleventweightlum / jettotaunonclosure)*(issignal > 0.5)", "goff");
  tTestHad ->Draw(Form("%s >> hSigTestHad ", MVA.Data()), "(fulleventweightlum / jettotaunonclosure)*(issignal > 0.5)", "goff");
  tTrainLep->Draw(Form("%s >> hSigTrainLep", MVA.Data()), "(fulleventweightlum / jettotaunonclosure)*(issignal > 0.5)", "goff");
  tTestLep ->Draw(Form("%s >> hSigTestLep ", MVA.Data()), "(fulleventweightlum / jettotaunonclosure)*(issignal > 0.5)", "goff");
  hadDir->cd();
  hBkgHad = (TH1D*) hBkgTrainHad->Clone("bkg_JetToTauNCDown"); hBkgHad->Add(hBkgTestHad);
  hSigHad = (TH1D*) hSigTrainHad->Clone("sig_JetToTauNCDown"); hSigHad->Add(hSigTestHad);
  hBkgHad->Write();
  hSigHad->Write();
  lepDir->cd();
  hBkgLep = (TH1D*) hBkgTrainLep->Clone("bkg_JetToTauNCDown"); hBkgLep->Add(hBkgTestLep);
  hSigLep = (TH1D*) hSigTrainLep->Clone("sig_JetToTauNCDown"); hSigLep->Add(hSigTestLep);
  hBkgLep->Write();
  hSigLep->Write();

  fout->Write();
  fout->Close();

  if(!dryRun_)
    gSystem->Exec(Form("combine -d %s -t -1 --rMin -10 --rMax 20",
                       (base_name + ".txt").Data()));
  return 0;
}
