//script to test histogramming a NANOAOD format file
using namespace CLFV;

int test_nano_histogramming(const TString filename) {
  TFile* file = TFile::Open(filename, "READ");
  if(!file) return 1;

  TTree* Events = (TTree*) file->Get("Events");
  if(!Events) {
    cout << "Events tree not found\n";
    return 2;
  }

  TTree* Runs = (TTree*) file->Get("Runs");
  if(!Events) {
    cout << "Runs tree not found\n";
    return 3;
  }

  //Get the normalization information
  Long64_t eventCount(0);
  Double_t genWtSum(0.), genWt2Sum(0.);
  if(Runs->GetBranch("genEventCount")) {
    Runs->SetBranchAddress("genEventCount", &eventCount);
    Runs->SetBranchAddress("genEventSumw" , &genWtSum);
    Runs->SetBranchAddress("genEventSumw2", &genWt2Sum);
  } else {
    Runs->SetBranchAddress("genEventCount_", &eventCount);
    Runs->SetBranchAddress("genEventSumw_" , &genWtSum);
    Runs->SetBranchAddress("genEventSumw2_", &genWt2Sum);
  }
  Runs->GetEntry(0);
  cout << "File " << filename << " has normalization info N(gen) = " << eventCount << " sum(wts) " << genWtSum << endl;

  const Double_t nevents = eventCount;
  const Double_t n_negative = std::max(0., 1. - genWtSum/eventCount);

  //Rename the tree to the file base name
  TString treename = filename; treename.ReplaceAll(".root", "");
  Events->SetName(treename.Data());

  CLFVHistMaker* histmaker = new CLFVHistMaker();
  //Initialize it to run on Z->e+mu signal sample for testing
  histmaker->fYear                 = filename.Contains("DY") ? 2018 : 2016;
  histmaker->fDYTesting            = false; //do a subset of the histogramming to save time/disk space
  histmaker->fIsData               = 0;
  histmaker->fIsDY                 = filename.Contains("DY");
  histmaker->fDYType               = -1; //don't split DY by generator process
  histmaker->fIsEmbed              = 0;
  histmaker->fIsSignal             = false;
  histmaker->fRemovePUWeights      = 2; //apply locally measured pileup weights
  histmaker->fUsePrefireWeights    = 1; //apply jet prefire weights
  histmaker->fUseJetPUIDWeights    = 1; //apply jet pileup ID weights
  histmaker->fUseBTagWeights       = 1; //apply b-jet tag weights
  histmaker->fRemoveTriggerWeights = 4; //apply P(event) trigger weights

  //specify the selection to consider
  histmaker->fSelection            = "emu";

  Events->Process(histmaker);

  return 0;
}
