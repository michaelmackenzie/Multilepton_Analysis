TString path_ = "ztautau";
TString selection_ = "mutau";

TCanvas* plot_tautau(int set = 7, TString file = "htautau_gluglu") {

  TFile* f = new TFile(Form("%s/%s_%s_bltTree_%s.hist",
			    path_.Data(), path_.Data(),
			    selection_.Data(), file.Data()), "READ");
  f->Print();
  TH1F* hlepm   = (TH1F*) f->Get(Form("Data/event_%i/lepm"  , set));
  TH1F* hlepsvm = (TH1F*) f->Get(Form("Data/event_%i/lepsvm", set));
  if(!hlepm || !hlepsvm) {
    cout << "Histograms not found!\n";
    return NULL;
  }
  hlepm->SetLineColor(kBlue);
  hlepm->SetLineWidth(2);
  hlepm->SetTitle("Di-lepton Mass");
  hlepm->SetXTitle("M_{ll} (GeV/c^{2})");

  hlepsvm->SetLineColor(kRed);
  hlepsvm->SetLineWidth(2);

  TCanvas* c = new TCanvas("c_lepm","c_lepm", 900,600); 
  hlepm->Draw("hist");
  hlepsvm->Draw("hist same");
  TLegend* leg = new TLegend(0.5,0.7,0.9,0.9);
  leg->AddEntry(hlepm, "Original");
  leg->AddEntry(hlepsvm, "SVFit");
  leg->Draw("same");
  
  gStyle->SetOptStat(0);
  c->Print(Form("figures/clfv_zdecays/%s/svfit_massShift_%s_%i.png", selection_.Data(),file.Data(),set));
  return c;
}
