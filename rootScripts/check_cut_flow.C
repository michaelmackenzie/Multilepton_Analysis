//Plot cut-flow for a LFVAnalyzer file

TCanvas* check_cut_flow(const char* file, TString print_tag = "", const char* hist_name = "Data/hcutflow") {
  gStyle->SetOptStat(0);
  TFile* f = TFile::Open(file, "READ");
  if(!f) return NULL;

  TH1D* h = (TH1D*) f->Get(hist_name);
  if(!h) {
    cout << "Cut-flow histogram " << hist_name << " not found\n";
    return NULL;
  }

  h->SetLineColor(kBlue);
  h->SetLineWidth(2);
  TH1D* h_eff = (TH1D*) h->Clone("h_eff");
  int max_relevant = 0;
  for(int ibin = 1; ibin <= h->GetNbinsX(); ++ibin) {
    double bin_val = h->GetBinContent(ibin);
    double bin_prev = (ibin == 1) ? bin_val : h->GetBinContent(ibin-1);
    h_eff->SetBinContent(ibin, (bin_prev > 0.) ? bin_val / bin_prev : 0.);
    if(bin_val > 0.) max_relevant = ibin;
  }
  double nominal = h->GetBinContent(1);
  for(int ibin = 1; ibin <= h->GetNbinsX(); ++ibin) {
    h->SetBinContent(ibin, h->GetBinContent(ibin) / nominal);
  }
  TCanvas* c = new TCanvas("c_flow", "c_flow", 1200, 600);
  c->Divide(2,1);
  auto pad = c->cd(1);
  h->SetTitle("Total efficiency");
  h->Draw("hist");
  h->GetXaxis()->SetRange(0, min(h->GetNbinsX(), max_relevant+1));
  pad->SetLogy();
  c->cd(2);
  h_eff->SetTitle("Step efficiency");
  h_eff->Draw("hist");
  h_eff->GetXaxis()->SetRange(0, min(h->GetNbinsX(), max_relevant+1));

  if(print_tag != "") c->Print(("cut_flow_" + print_tag + ".png").Data());
  return c;
}
