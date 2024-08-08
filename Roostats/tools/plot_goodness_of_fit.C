//plot the goodness of fit results

int plot_goodness_of_fit(const char* obs_file, const char* toy_file, const char* tag = "", const int verbose = 0) {

  //Retrieve the input data
  TFile* f_obs = TFile::Open(obs_file, "READ");
  if(!f_obs) return 1;
  TFile* f_toy = TFile::Open(toy_file, "READ");
  if(!f_toy) return 1;

  TTree* t_obs = (TTree*) f_obs->Get("limit");
  if(!t_obs) return 2;
  TTree* t_toy = (TTree*) f_toy->Get("limit");
  if(!t_toy) return 2;

  if(verbose > 1) t_obs->Print();
  const double obs_val = t_obs->GetMaximum("limit");
  const double min_val = min(obs_val, t_toy->GetMinimum("limit"));
  const double max_val = max(obs_val, t_toy->GetMaximum("limit"));

  TH1* h_toy = new TH1D("h_toy", "h_toy", 25, min_val - 0.05*(max_val-min_val), max_val + 0.05*(max_val-min_val));
  t_toy->Draw("limit >> h_toy", "", "0");
  const int bin_obs = h_toy->FindBin(obs_val);

  //Clone of toys, only filling high-side tail
  TH1* h_toy_right = (TH1*) h_toy->Clone("h_toy_right");
  for(int ibin = 1; ibin < bin_obs; ++ibin) h_toy_right->SetBinContent(ibin, 0);

  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas();
  h_toy->SetLineColor(kBlue);
  h_toy->SetLineWidth(2);
  h_toy->Draw("hist");
  h_toy->GetYaxis()->SetRangeUser(0., 1.2*h_toy->GetMaximum());
  h_toy->SetTitle("Goodness of fit");
  h_toy->SetXTitle("test statistic");
  h_toy_right->SetFillColor(kBlue);
  h_toy_right->SetFillStyle(3003);
  h_toy_right->SetLineColor(kBlue);
  h_toy_right->SetLineWidth(2);
  h_toy_right->Draw("hist same");

  auto arr = new TArrow(obs_val,0.15*h_toy->GetMaximum(),obs_val,0.,0.03,"|>");
  arr->SetLineWidth(3);
  arr->SetLineColor(kRed);
  arr->SetFillColor(kRed);
  arr->Draw();


  const double p_obs = h_toy->Integral(bin_obs, h_toy->GetNbinsX())/h_toy->Integral();
  printf("Goodness of fit: Observed = %.3f, p(observed) = %.3f\n", obs_val, p_obs);

    //Add labels to the plot
  TLatex label;
  label.SetNDC();
  label.SetTextFont(72);
  label.SetTextSize(0.05);
  label.SetTextAlign(13);
  label.SetTextAngle(0);
  label.SetTextSize(0.05);
  label.DrawLatex(0.75, 0.86, Form("p = %.3f", p_obs));

  c->SaveAs(Form("goodness_of_fit%s.png", tag));
  return 0;
}
