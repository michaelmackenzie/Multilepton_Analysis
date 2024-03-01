//Script to validate the uncertainty in a stack

void test_stack_uncertainty(TString file = "../histograms/zmutau_mva1_25_2018.hist",
                            TString path = "hstack",
                            double xmin = 0., double xmax = 1.) {
  TFile* f = TFile::Open(file.Data(), "READ");
  if(!f) return;
  THStack* hstack = (THStack*) f->Get(path.Data());
  if(!hstack) return;

  TCanvas* c = new TCanvas("c", "c", 1200, 700);
  c->Divide(2,1);
  c->cd(1);
  TH1D* hlast = (TH1D*) hstack->GetStack()->Last();
  hlast->Draw("hist E");
  hlast->SetAxisRange(xmin, xmax, "X");
  c->cd(2);
  TH1D* h = (TH1D*) hstack->GetHists()->At(0)->Clone("h_test");
  for(int ihist = 1; ihist < hstack->GetNhists(); ++ihist) {
    h->Add((TH1D*) hstack->GetHists()->At(ihist));
  }
  h->Draw("hist E");
  h->SetAxisRange(xmin, xmax, "X");

  TH1D* hdiff = new TH1D("hdiff", "Error difference / avg. error", 100, -1., 1.);
  for(int ibin = 1; ibin <= h->GetNbinsX(); ++ibin) {
    double ediff = hlast->GetBinError(ibin) - h->GetBinError(ibin);
    double eavg  = (hlast->GetBinError(ibin) + h->GetBinError(ibin))/2.;
    hdiff->Fill((eavg > 0.) ? ediff/eavg : 0.);
  }
  TCanvas* c2 = new TCanvas;
  hdiff->Draw();
}
