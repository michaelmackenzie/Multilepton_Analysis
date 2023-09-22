//inspect limits from combine toy MC processing

int inspect_limits(TString file) {

  TFile* f = TFile::Open(file.Data(), "READ");
  if(!f) return 1;
  TTree* tree = (TTree*) f->Get("limit");
  if(!tree) {
    cout << "Limit tree not found\n";
    return 2;
  }

  const int nentries = tree->GetEntries();
  if(nentries % 6 != 0) {
    cout << "Unexpected multiple of N(entries) = " << nentries << endl;
    return 3;
  }
  const int ntoys = nentries / 6;
  if(ntoys <= 0) return 4;

  double expected[ntoys], up_1[ntoys], up_2[ntoys], down_1[ntoys], down_2[ntoys], obs[ntoys];
  double val;
  tree->SetBranchAddress("limit", &val);
  double min_val(1.e20), max_val(-1.e20);
  for(int itoy = 0; itoy < ntoys; ++itoy) {
    tree->GetEntry(6*itoy + 2); expected[itoy] = val;
    tree->GetEntry(6*itoy + 0); down_2  [itoy] = expected[itoy] - val;
    tree->GetEntry(6*itoy + 1); down_1  [itoy] = expected[itoy] - val;
    tree->GetEntry(6*itoy + 3); up_1    [itoy] = val - expected[itoy];
    tree->GetEntry(6*itoy + 4); up_2    [itoy] = val - expected[itoy];
    tree->GetEntry(6*itoy + 5); obs     [itoy] = val;
    //get min/max obs values
    min_val = min(val, min_val);
    max_val = max(val, max_val);
  }

  //Make histograms
  TH1* hobs  = new TH1D("hobs", "Observed limits", 2.5*sqrt(ntoys), min_val - (max_val-min_val)*0.1, max_val + (max_val-min_val)*0.1);
  TH1* hexp  = new TH1D("hexp", "Expected limits", 3.5*sqrt(ntoys), min_val - (max_val-min_val)*0.1, max_val + (max_val-min_val)*0.1);
  TH1* hdiff = new TH1D("hdiff", "Observed - expected limits", 2*sqrt(ntoys), min_val-max_val, max_val-min_val);
  for(int itoy = 0; itoy < ntoys; ++itoy) {
    hobs->Fill(obs[itoy]);
    hexp->Fill(expected[itoy]);
    hdiff->Fill(obs[itoy] - expected[itoy]);
  }

  gStyle->SetOptStat(1111);
  TString cname = file;
  cname.ReplaceAll(".root", ".png");
  TCanvas c("c","c",1200,600);
  c.Divide(2,1);
  c.cd(1);
  hobs->SetLineWidth(2);
  hobs->Draw("hist");
  // hexp->Draw("E1 same");
  // hobs->GetYaxis()->SetRangeUser(0., 1.1*max(hobs->GetMaximum(), hexp->GetMaximum()));
  hobs->SetXTitle("r");
  c.cd(2);
  hdiff->SetLineWidth(2);
  hdiff->Draw("hist");
  hdiff->SetXTitle("r(obs) - r(exp)");
  c.SaveAs(cname.Data());

  printf("Mean (obs) = %.3f +- %.3f\nMean (exp) = %.3f +- %.3f\nMean obs/exp = %.3f +- %.3f\n",
         hobs->GetMean(), hobs->GetMeanError(),
         hexp->GetMean(), hexp->GetMeanError(),
         hobs->GetMean()/hexp->GetMean(),
         hobs->GetMeanError()/hexp->GetMean() //assume expected error is small
         );

  sort(obs, obs + ntoys);
  const double median = obs[ntoys/2]; //round up in the case of even number of events
  printf("Median observation seen: %.3f\n", median);
  return 0;
}
