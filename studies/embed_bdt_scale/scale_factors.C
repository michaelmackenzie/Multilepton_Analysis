//Script to measure scale factors for embedded di-lepton BDT scores
#include "histograms/datacards.C"

//use the dataplotter to manage normalizations and initializations
DataPlotter* dataplotter_ = 0;
int     verbose_       = 0    ;
TString selection_     = ""   ;
int     set_offset_    = 0    ; //offset to selection set

//-------------------------------------------------------------------------------------------------------------------------------
//make a plot
Int_t plot(TH1* MC, TH1* data, TString name, TString dir) {
  //Draw the distributions
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetOptStat(0);

  TCanvas* c = new TCanvas("c","c", 1000, 1200);
  TPad* pad1 = new TPad("pad1", "pad1", 0., 0.25, 1., 1.0 ); pad1->Draw();
  TPad* pad2 = new TPad("pad2", "pad2", 0., 0.  , 1., 0.25); pad2->Draw();
  pad1->SetBottomMargin(0.04);
  pad2->SetTopMargin(0.03);
  pad1->cd();

  MC->Draw("hist");
  data->Draw("E1 same");
  MC->GetYaxis()->SetRangeUser(0.5, 1.2*max(MC->GetMaximum(), data->GetMaximum()));
  MC->SetTitle("");

  //Make ratio plot
  pad2->cd();
  TH1* ratio = (TH1*) data->Clone("ratio");
  ratio->Divide(MC);
  ratio->Draw("E1");
  TLine* line = new TLine(ratio->GetBinLowEdge(1), 1., ratio->GetBinLowEdge(ratio->GetNbinsX()) + ratio->GetBinWidth(ratio->GetNbinsX()), 1.);
  line->SetLineWidth(2);
  line->SetLineColor(kRed);
  line->Draw("same");
  ratio->GetYaxis()->SetRangeUser(0.6,1.4);
  ratio->SetTitle("");
  ratio->GetXaxis()->SetLabelSize(0.1);
  ratio->GetYaxis()->SetLabelSize(0.1);

  c->SaveAs(Form("%s/%s.png", dir.Data(), name.Data()));
  delete c;

  return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------
//initialize the files and scales using a DataPlotter
Int_t initialize_plotter(TString base, int year) {
  if(dataplotter_) delete dataplotter_;
  dataplotter_ = new DataPlotter();
  dataplotter_->include_qcd_ = 0;
  dataplotter_->include_misid_ = 0;
  dataplotter_->verbose_ = max(0, verbose_ - 1);
  dataplotter_->qcd_scale_ = 1.;
  dataplotter_->embed_scale_ = embedScale_;
  years_ = {year};
  hist_tag_ = "emb";
  useEmbed_ = 2; //use embedding in same-flavor regions

  std::vector<dcard> cards;
  get_datacards(cards, selection_, true);

  CrossSections xs(useUL_, ZMode_); //cross section handler
  dataplotter_->set_luminosity(xs.GetLuminosity(year));
  if(verbose_ > 0) cout << "--- Dataplotter luminosity for " << year << " = " << dataplotter_->lum_ << endl;

  int status(0);
  for(auto card : cards)
    status += dataplotter_->add_dataset(card);

  status += dataplotter_->init_files();
  return status;
}

//-------------------------------------------------------------------------------------------------------------------------------
//retrieve the distributions and make a plot
Int_t plot(TString hist, TString type, int set, TString name, TString dir) {
    //Retrieve the distributions
  THStack* stack = dataplotter_->get_stack(hist, type, set);
  if(!stack) {
    return 1;
  }
  TH1* data = dataplotter_->get_data(hist, type, set);
  if(!data) {
    return 2;
  }

  //Get the total background and normalize it to data
  TH1* MC = (TH1*) stack->GetStack()->Last()->Clone(Form("%s_bkg", stack->GetName()));
  const double scale = data->Integral() / MC->Integral();
  MC->Scale(scale);
  return plot(MC, data, name, dir);
}

//-------------------------------------------------------------------------------------------------------------------------------
//Generate the plots and scale factors
Int_t scale_factors(TString bdt = "zmutau", const int year = 2016, TString selection = "mumu", const int set = 8,
                    TString path = "nanoaods_emb_bdt") {

  ///////////////////////
  // Initialize params //
  ///////////////////////

  selection_ = selection;

  //////////////////////
  // Initialize files //
  //////////////////////

  //set the histogram directory
  hist_dir_ = path;

  //get the absolute value of the set, offsetting by the selection
  set_offset_ = 0;
  if(selection == "mutau")      set_offset_ += CLFVHistMaker::kMuTau;
  else if(selection == "etau" ) set_offset_ += CLFVHistMaker::kETau ;
  else if(selection == "emu"  ) set_offset_ += CLFVHistMaker::kEMu  ;
  else if(selection == "mumu" ) set_offset_ += CLFVHistMaker::kMuMu ;
  else if(selection == "ee"   ) set_offset_ += CLFVHistMaker::kEE   ;
  const int setAbs = set + set_offset_;

  //construct the general name of each file, not including the sample name
  TString baseName = "clfv_" + selection + "_clfv_";
  baseName += year;
  baseName += "_";

  //initialize the data files
  if(verbose_ > 0) std::cout << "Initializing the dataplotter" << std::endl;
  if(initialize_plotter(baseName, year)) {
    cout << "Dataplotter initialization failed!\n";
    return 1;
  }

  //////////////////////
  //  Get histograms  //
  //////////////////////

  //Define the BDT histogram of interest
  int mva_num = 0;
  if     (bdt.BeginsWith("z")    ) mva_num += 1;
  if     (bdt.EndsWith("etau"   )) mva_num += 2;
  else if(bdt.EndsWith("emu"    )) mva_num += 4;
  else if(bdt.EndsWith("mutau_e")) mva_num += 6;
  else if(bdt.EndsWith("etau_mu")) mva_num += 8;
  TString hist_name = Form("mva%i_4", mva_num); //take the F(p) form of the MVA

  //Retrieve the distributions
  THStack* stack = dataplotter_->get_stack(hist_name, "event", setAbs);
  if(!stack) {
    cout << "Stack not found!\n";
    return 2;
  }
  TH1* data = dataplotter_->get_data(hist_name, "event", setAbs);
  if(!data) {
    cout << "Data not found!\n";
    return 3;
  }
  data->SetName("data");

  //Get the total background and normalize it to data
  TH1* MC = (TH1*) stack->GetStack()->Last()->Clone("MC");
  const double scale = data->Integral() / MC->Integral();
  cout << "Scaling MC by " << scale << endl;
  MC->Scale(scale);

  //Save the MC / data scales
  gSystem->Exec("[ ! -d rootfiles ] && mkdir rootfiles");
  TFile* fout = new TFile(Form("rootfiles/emb_bdt_%s_%i.root", bdt.Data(), year), "RECREATE");
  TH1* ratio = (TH1*) data->Clone("ratio");
  ratio->Divide(MC);
  ratio->Write();
  fout->Close();

  //Draw the distributions
  TString dir = Form("figures/%s_%i", bdt.Data(), year);
  gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s/", dir.Data(), dir.Data()));

  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetOptStat(0);
  plot(MC, data, "bdt", dir);

  plot("lepm" , "event", setAbs, "lepm" , dir);
  plot("met"  , "event", setAbs, "met"  , dir);
  plot("mtlep", "event", setAbs, "mtlep", dir);
  plot("leppt", "event", setAbs, "leppt", dir);

  return 0;
}
