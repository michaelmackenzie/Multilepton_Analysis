//Script to compare MVA reader evaluation of MVA score to stored value

TString var_ = "BDT_MM"; //MVA name
TMVA::Reader* reader_ = 0;
Tree_t treeVars_;
TTree* tree_ = 0;
float score_ = -1.;
int verbose_ = 0;
bool doTest_ = true;
TH1F* hCompare_[3];
Long64_t maxEntries_ = 1e5;

//get file name from configuration
TString get_file_name(bool isHiggs = true, TString selection = "mutau",
                      vector<int> years = {2016, 2017, 2018}, int set = 8) {
  TString name = "training_background_ztautau_";
  name += (isHiggs) ? "higgs_nano_" : "Z0_nano_";
  name += selection + "_";
  for(int year : years) {
    name += year;
    name += "_";
  }
  // if(selection == "mutau")        set += ZTauTauHistMaker::kMuTau;
  // else if(selection == "etau")    set += ZTauTauHistMaker::kETau;
  // else if(selection == "emu")     set += ZTauTauHistMaker::kEMu;
  // else if(selection == "mutau_e") set += ZTauTauHistMaker::kEMu;
  // else if(selection == "etau_mu") set += ZTauTauHistMaker::kEMu;
  name += set;
  if(verbose_ > 0) cout << "Using file base name " << name.Data() << endl;
  return name;
}

//initialize the reader and setup the variables/spectators and tree addresses
int init_reader(TString selection, bool isHiggs) {
  int status(0);
  if(!tree_) {
    cout << __func__ << ": Tree not defined!\n";
    return 1;
  }
  if(reader_) delete reader_;
  reader_ = new TMVA::Reader("!Color:!Silent");
  TrkQualInit trkQualInit;
  selection = ((isHiggs) ? "h"+selection : "z"+selection);
  status += trkQualInit.InitializeVariables(*reader_, selection, treeVars_);
  status += trkQualInit.SetBranchAddresses(tree_, selection, treeVars_);
  tree_->SetBranchAddress(var_.Data(), &score_);
  return status;
}

//initialize comparison testing
int init(TString selection, bool isHiggs,
         vector<int> years, int set) {
  int status(0);
  TString fname = get_file_name(isHiggs, selection, years, set);
  TString fpath = Form("../%s/%s.root", fname.Data(), fname.Data());
  TFile* f = TFile::Open(fpath.Data(), "READ");
  if(!f) {
    cout << __func__ << ": File not found!\n";
    return 1;
  }

  TTree* ttest  = (TTree*) f->Get(Form("tmva_%s/TestTree" , fname.Data()));
  TTree* ttrain = (TTree*) f->Get(Form("tmva_%s/TrainTree", fname.Data()));
  if(doTest_) tree_ = ttest;
  else        tree_ = ttrain;

  if(!ttest || !ttrain) {
    cout << __func__ << ": Trees not found in " << fpath.Data() << endl;
    f->ls();
    if(f->Get(("tmva_"+fname).Data()))
      f->Get(("tmva_"+fname).Data())->ls();
    return 2;
  }
  status += init_reader(selection, isHiggs);
  if(status) return status;
  reader_->BookMVA("mva", Form("../%s/tmva_%s/%s_Weights/TMVAClassification_%s.weights.xml", fname.Data(), fname.Data(), fname.Data(), var_.Data()));
  cout << "Reader and tree setup\n";
  return status;
}

int compare_point(TString selection = "emu", bool isHiggs = true,
                   vector<int> years = {2016, 2017, 2018}, int set = 8) {
  int status = init(selection, isHiggs, years, set);

  treeVars_.lepdeltaphi    = 2.98682;
  treeVars_.lepptoverm     = 0.101844;
  treeVars_.leponeptoverm  = 0.528217;
  treeVars_.leptwoptoverm  = 0.460736;
  treeVars_.met            = 0.874168;
  treeVars_.mtoneoverm     = 0.0279408;
  treeVars_.mttwooverm     = 0.112298;
  treeVars_.onemetdeltaphi = 0.463361;
  treeVars_.twometdeltaphi = 2.83301;
  treeVars_.met            = 0.874168;
  treeVars_.jetpt          = 0.;

  double treeValue   = 0.0778808;
  double readerValue = reader_->EvaluateMVA("mva");

  cout << "Given = " << treeValue << " reader = " << readerValue
       << " diff = " << readerValue - treeValue << endl;
  return 0;
}

int compare_reader(TString selection = "mutau", bool isHiggs = false,
                   vector<int> years = {2016, 2017, 2018}, int set = 8) {
  if(maxEntries_ == 1) verbose_ = 2;
  int status = init(selection, isHiggs, years, set);
  if(status) return status;
  hCompare_[0] = new TH1F("score_r", "Score (Reader)", 100, -1., 1.);
  hCompare_[1] = new TH1F("score_t", "Score (Tree)"  , 100, -1., 1.);
  hCompare_[2] = new TH1F("score_d", "Score Difference", 100, -0.5, 0.5);
  Long64_t nentries = tree_->GetEntriesFast();
  cout << "Number of entries to process = " << nentries << endl;
  for(Long64_t entry = 0; entry < nentries; ++entry) {
    if(entry % 50000 == 0) cout << "Processing entry " << entry << "...\n";
    if(maxEntries_ > -1 && maxEntries_ <= entry) break;
    tree_->GetEntry(entry);
    float score = reader_->EvaluateMVA("mva");
    hCompare_[0]->Fill(score         , treeVars_.fulleventweightlum);
    hCompare_[1]->Fill(score_        , treeVars_.fulleventweightlum);
    hCompare_[2]->Fill(score - score_, treeVars_.fulleventweightlum);
    if(verbose_ > 1) cout << "Tree score = " << score_
                          << " reader = " << score << " diff = "
                          << score - score_ << endl;

  }
  TCanvas* c = new TCanvas("c_compare", "c_compare", 1100, 700);
  c->Divide(2,1);
  c->cd(1);
  hCompare_[0]->Draw("hist");
  hCompare_[1]->Draw("hist sames");
  hCompare_[0]->SetLineWidth(2);
  hCompare_[1]->SetLineWidth(2);
  hCompare_[1]->SetLineColor(kRed);
  hCompare_[0]->SetFillColor(kBlue);
  hCompare_[0]->SetFillStyle(3002);
  hCompare_[0]->SetTitle("MVA score using training branch vs Reader");
  hCompare_[0]->SetAxisRange(0.1,1.2*max(hCompare_[0]->GetMaximum(), hCompare_[1]->GetMaximum()), "Y");
  TLegend* leg = new TLegend(0.6, 0.75, 0.9, 0.9);
  leg->AddEntry(hCompare_[0], "Reader");
  leg->AddEntry(hCompare_[1], "Tree");
  leg->Draw("same");
  c->cd(2);
  hCompare_[2]->Draw("hist");
  hCompare_[2]->SetLineWidth(2);
  hCompare_[2]->SetTitle("MVA score (Reader - training branch)");
  gStyle->SetOptStat(0);

  if(maxEntries_ < 0)
    c->Print(Form("figures/compare_reader_scores_%s.png", ((isHiggs) ? "h"+selection : "z"+selection).Data()));
  return status;
}
