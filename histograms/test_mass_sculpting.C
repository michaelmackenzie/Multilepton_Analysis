//Test plotting the mass sculpting vs BDT score for the Z->e+mu search
#include "dataplotter_clfv.C"

int test_mass_sculpting(TString hist_dir = "nanoaods_mva_emu_nosys", TString proc = "Other VB") {
  //split off the WW from the W+jets
  combineVB_ = 0;
  //setup the datacards
  years_ = {2016,2017,2018};
  hist_dir_ = hist_dir;
  useEmbed_ = 0;
  int status = nanoaod_init("emu", hist_dir_, "mass_sculpting");
  if(status) return status;

  //get the BDT score vs mass 2D histogram
  auto hists  = dataplotter_->get_histograms("lepmvsmva0", "event", 208, DataPlotter::kAny, proc);
  if(hists.size() == 0) return 10;
  if(hists.size() > 1) {
    cout << "Too many histograms found for process " << proc.Data() << endl;
    return 20;
  }
  TH2* hmvsmva = (TH2*) hists[0];

  if(hmvsmva->Integral() <= 0.) {
    cout << "Integral is non-positive\n";
    return 30;
  }

  //Define the 4 BDT score regions and get the mass projections
  const int bin_1 = 1;
  const int bin_2 = hmvsmva->GetXaxis()->FindBin(0.25);
  const int bin_3 = hmvsmva->GetXaxis()->FindBin(0.50);
  const int bin_4 = hmvsmva->GetXaxis()->FindBin(0.75);
  const int bin_5 = hmvsmva->GetNbinsX();

  TH1* h_1 = hmvsmva->ProjectionY("mva_0_25"  , bin_1  , bin_2);
  TH1* h_2 = hmvsmva->ProjectionY("mva_25_50" , bin_2+1, bin_3);
  TH1* h_3 = hmvsmva->ProjectionY("mva_50_75" , bin_3+1, bin_4);
  TH1* h_4 = hmvsmva->ProjectionY("mva_75_100", bin_4+1, bin_5);

  //Draw the results
  h_1->SetLineColor(kRed);
  h_2->SetLineColor(kGreen);
  h_3->SetLineColor(kYellow);
  h_4->SetLineColor(kBlue);

  h_1->SetFillColor(0);
  h_2->SetFillColor(0);
  h_3->SetFillColor(0);
  h_4->SetFillColor(0);

  h_1->SetLineWidth(3);
  h_2->SetLineWidth(3);
  h_3->SetLineWidth(3);
  h_4->SetLineWidth(3);

  h_1->Scale(1./h_1->Integral());
  h_2->Scale(1./h_2->Integral());
  h_3->Scale(1./h_3->Integral());
  h_4->Scale(1./h_4->Integral());

  TCanvas* c = new TCanvas();
  h_1->Draw("hist");
  h_2->Draw("hist same");
  h_3->Draw("hist same");
  h_4->Draw("hist same");

  h_1->GetYaxis()->SetRangeUser(0.5*h_1->GetMaximum(), 1.2*h_1->GetMaximum());
  h_1->GetXaxis()->SetRangeUser(70., 110.);

  TLegend* leg = new TLegend(0.6, 0.7, 0.9, 0.9);
  leg->AddEntry(h_1, "0-0.25");
  leg->AddEntry(h_2, "0.25-0.50");
  leg->AddEntry(h_3, "0.50-0.75");
  leg->AddEntry(h_4, "0.75-1");
  leg->Draw();

  h_1->SetTitle("");
  h_1->SetXTitle("M_{ll}");

  //Remove some common unwanted strings in the process name
  proc.ReplaceAll(" ", "");
  proc.ReplaceAll("#rightarrow", "");
  proc.ReplaceAll("#", "");
  proc.ReplaceAll(">", "");
  proc.ReplaceAll("(10^{-5})", "");
  proc.ReplaceAll("}", "");
  proc.ReplaceAll("{", "");
  proc.ReplaceAll("^", "");
  proc.ReplaceAll("-", "");
  proc.ReplaceAll("+", "");
  proc.ReplaceAll("/", "");

  //Save the figure with both linear and log scale
  c->SaveAs(Form("figures/mass_sculpting/emu/2016_2017_2018/mass_sculpting_%s.png", proc.Data()));
  h_1->GetYaxis()->SetRangeUser(h_1->GetMaximum()/1.e4, 2.*h_1->GetMaximum());
  c->SetLogy();
  c->SaveAs(Form("figures/mass_sculpting/emu/2016_2017_2018/mass_sculpting_%s_log.png", proc.Data()));
  return 0;
}
