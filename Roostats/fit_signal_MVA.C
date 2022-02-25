//Script to fit signal MVA distribution
#include "DataInfo.C"

Int_t fit_signal_MVA(int set = 8, TString selection = "zmutau",
		     vector<int> years = {2016/*, 2017, 2018*/},
		     // TString base = "../histograms/nanoaods/") {
		     TString base = "../histograms/nanoaods_dev/") {

  TString hist;
  if     (selection == "hmutau"  ) hist = "mva0";
  else if(selection == "zmutau"  ) hist = "mva1";
  else if(selection == "hetau"   ) hist = "mva2";
  else if(selection == "zetau"   ) hist = "mva3";
  else if(selection == "hemu"    ) hist = "mva4";
  else if(selection == "zemu"    ) hist = "mva5";
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

  /////////////////////
  // Get Signal Info //
  /////////////////////
  DataInfo signalInfo(set, selection.Data());
  signalInfo.ReadData();
  TList* list = new TList;
  TFile* ftmp = new TFile("tmp.root", "RECREATE");
  TFile* f_sigs[years.size()];
  TTree* t_list[years.size()];
  TString selec = selection; selec.ReplaceAll("z", ""); selec.ReplaceAll("h", "");
  if(selec.Contains("_")) selec = "emu";
  TString signame = selection; signame.ReplaceAll("z", "Z"); signame.ReplaceAll("h", "H");
  signame.ReplaceAll("_e", ""); signame.ReplaceAll("_mu",""); signame.ReplaceAll("m", "M");
  signame.ReplaceAll("e", "E"); signame.ReplaceAll("t", "T");
  int set_offset = CLFVHistMaker::kEMu;
  if     (selec == "mutau") set_offset = CLFVHistMaker::kMuTau;
  else if(selec == "etau" ) set_offset = CLFVHistMaker::kETau;
  for(unsigned i = 0; i < years.size(); ++i) {
    int year = years[i];
    TString sig_name = base + "clfv_" + selec + "_clfv_";
    sig_name += year;
    sig_name += "_" + signame + ".hist";

    f_sigs[i] = TFile::Open(sig_name.Data(), "READ");
    if(!f_sigs[i]) {
      cout << "Signal file " << sig_name.Data() << " not found\n";
      return 3;
    }

    t_list[i] = (TTree*) f_sigs[i]->Get(Form("Data/tree_%i/tree_%i",
					     set+set_offset,
					     set+set_offset));
    if(!t_list[i]) {
      cout << "Signal tree in file " << sig_name.Data() << " for set "
	   << set << " + " << set_offset << " not found\n";
      return 4;
    }
    ftmp->cd();
    t_list[i]->SetName(Form("tree_%i_%i", year, set+set_offset));
    t_list[i]->Write();
    list->Add(t_list[i]);
    //save weighted N(signal) count
    TH1F* h = new TH1F("h", "h", 1, -1e9, 1e9);
    t_list[i]->Draw(Form("%s>>h", hist.Data()), "eventweight");
    signalInfo.datamap_[year] = h->Integral();
    delete h;
  }
  signalInfo.WriteData(); //write signal counts
  TTree* t_sig = TTree::MergeTrees(list);
  t_sig->SetName("signal_tree");
  std::cout << "All file elements retrieved!\n";

  //create a histogram for the PDF
  const int nbins = 40;
  const double xmin = -1.;
  const double xmax =  1.;
  TH1D* hmva = new TH1D("hmva", "hmva", nbins, xmin, xmax);
  t_sig->Draw((hist+">>hmva").Data(), "fulleventweightlum");
  RooRealVar mva(hist.Data(),"MVA score", xmin, xmax);
  RooDataHist dataMVA("sigMVA", "Signal MVA Data", RooArgList(mva), hmva);
  RooHistPdf sigMVAPDF("sigMVAPDF", "Signal MVA PDF", RooArgSet(mva), dataMVA);

  //Plot PDF
  auto xframe = mva.frame();
  sigMVAPDF.plotOn(xframe);

  auto c1 = new TCanvas();
  xframe->Draw();
  hmva->Scale(0.01*nbins/hmva->Integral());
  hmva->SetMarkerStyle(20);
  hmva->Draw("same P E1");
  
  gSystem->Exec(Form("[ ! -d plots/latest_production/%s ] && mkdir -p plots/latest_production/%s", year_string.Data(), year_string.Data()));
  c1->SaveAs(Form("plots/latest_production/%s/hist_signal_mva_%s_%s_%i.png", year_string.Data(), selection.Data(), hist.Data(), set));
  TFile* fOut = new TFile(Form("workspaces/hist_signal_mva_%s_%s_%s_%i.root", selection.Data(), hist.Data(), year_string.Data(), set), "RECREATE");
  fOut->cd();
  RooWorkspace ws("ws");
  ws.import(sigMVAPDF);
  ws.Write();
  fOut->Close();
  delete fOut;
  
  return 0;
}
