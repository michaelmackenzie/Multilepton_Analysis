//Compare cutflow histogram between two histogram files

int compare_cutflow(const char* file1, const char* file2) {

  //Retrieve the files
  TFile* f1 = TFile::Open(file1, "READ");
  TFile* f2 = TFile::Open(file2, "READ");
  if(!f1 || ! f2) {
    return 1;
  }

  //Retrieve the cutflow histogram
  TH1* h1 = (TH1*) f1->Get("Data/hcutflow");
  TH1* h2 = (TH1*) f2->Get("Data/hcutflow");
  if(!h1 || !h2) {
    cout << "Histograms not found!\n";
    return 2;
  }

  //Plot the results, scaled to efficiency
  TCanvas* c = new TCanvas();
  gStyle->SetOptStat(0);
  h1->Scale(1./h1->GetBinContent(1));
  h2->Scale(1./h2->GetBinContent(1));

  h1->Draw("text hist");
  h2->Draw("hist same");

  h2->SetLineColor(kRed);
  h2->SetLineStyle(kDashed);

  //Only use the relevant range of the cutflow
  h1->GetXaxis()->SetRange(0, h1->GetBinCenter(min(h1->GetNbinsX(), h1->FindLastBinAbove(0.)+2)));

  //Save the plot
  c->SaveAs("cutflow.png");

  return 0;
}
