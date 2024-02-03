//Script to compare data, embedding, and MC

int print_figure(TFile* f_data, TFile* f_embed, TFile* f_mc, const char* name, TString dir) {
  //Retrieve the histograms
  TH1* h_data  = (TH1*) f_data ->Get(name);
  TH1* h_embed = (TH1*) f_embed->Get(name);
  TH1* h_mc    = (TH1*) f_mc   ->Get(name);
  if(!h_data || !h_embed || !h_mc) {
    cout << __func__ << ": Histogram " << name << " not found!\n";
    return 1;
  }
  h_data->SetName(Form("%s_data", name));
  h_embed->SetName(Form("%s_embed", name));
  h_mc->SetName(Form("%s_mc", name));
  h_data ->Scale(1./h_data ->Integral());
  h_embed->Scale(1./h_embed->Integral());
  h_mc   ->Scale(1./h_mc   ->Integral());

  //Create the plot
  TCanvas* c = new TCanvas("c", "c", 1000, 1100);
  TPad* pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1.0); pad1->Draw();
  TPad* pad2 = new TPad("pad2", "pad2", 0., 0.0, 1., 0.3); pad2->Draw();
  pad1->SetBottomMargin(0.05); pad2->SetTopMargin(0.03);

  //Draw the histograms
  pad1->cd();
  pad1->SetGrid();
  h_data->SetMarkerStyle(20);
  h_data->SetMarkerSize(0.8);
  h_embed->SetLineColor(kRed);
  h_embed->SetLineWidth(2);
  h_mc   ->SetLineColor(kBlue);
  h_mc   ->SetLineWidth(2);
  h_data->Draw("E1");
  h_embed->Draw("hist same");
  h_mc->Draw("hist same");
  h_data->Draw("E1 same");
  h_data->GetYaxis()->SetRangeUser(0, 1.2*max(h_data->GetMaximum(), max(h_embed->GetMaximum(), h_mc->GetMaximum())));

  TLegend* leg = new TLegend(0.7, 0.75, 0.9, 0.9);
  leg->AddEntry(h_data , "Data"     , "PL");
  leg->AddEntry(h_embed, "Embedding", "L" );
  leg->AddEntry(h_mc   , "MC"       , "L" );
  leg->Draw();

  //Draw the ratio plots
  pad2->cd();
  pad2->SetGrid();
  TH1* hr_embed = (TH1*) h_data->Clone(Form("%s_ratio", h_embed->GetName()));
  TH1* hr_mc    = (TH1*) h_data->Clone(Form("%s_ratio"   , h_mc   ->GetName()));
  hr_embed->Divide(h_embed);
  hr_mc   ->Divide(h_mc);
  hr_embed->SetLineColor(h_embed->GetLineColor());
  hr_mc   ->SetLineColor(h_mc   ->GetLineColor());
  hr_embed->SetLineWidth(2);
  hr_mc   ->SetLineWidth(2);

  hr_embed->Draw("hist");
  hr_mc->Draw("hist same");
  pad2->Update();

  hr_embed->SetTitle("");
  hr_embed->SetXTitle("");
  hr_embed->GetYaxis()->SetTitle("Data/Est.");
  hr_embed->GetYaxis()->SetTitleSize(0.13);
  hr_embed->GetYaxis()->SetTitleOffset(0.30);
  hr_embed->GetYaxis()->SetLabelSize(0.08);
  hr_embed->GetXaxis()->SetLabelSize(0.08);
  hr_embed->GetYaxis()->SetRangeUser(0.5, 1.5);

  hr_mc->SetTitle("");
  hr_mc->SetXTitle("");
  hr_mc->GetYaxis()->SetTitle("Data/Est.");
  hr_mc->GetYaxis()->SetTitleSize(0.13);
  hr_mc->GetYaxis()->SetTitleOffset(0.30);
  hr_mc->GetYaxis()->SetLabelSize(0.08);
  hr_mc->GetXaxis()->SetLabelSize(0.08);
  hr_mc->GetYaxis()->SetRangeUser(0.5, 1.5);

  TLine* line = new TLine(hr_embed->GetXaxis()->GetXmin(), 1., hr_embed->GetXaxis()->GetXmax(), 1.);
  line->SetLineColor(kBlack);
  line->SetLineStyle(kDashed);
  line->SetLineWidth(2);
  line->Draw("same");


  c->SaveAs(Form("%s/%s.png", dir.Data(), name));
  delete c;
  return 0;
}

int compare(bool isMuon = true, int year = 2016, int period = -1, int max_entries = -1) {

  //Read in the histogram files
  TFile* f_data  = TFile::Open(Form("histograms/%s_mc-0_%i.hist", (isMuon) ? "mumu" : "ee", year), "READ");
  TFile* f_embed = TFile::Open(Form("histograms/%s_mc-1_%i.hist", (isMuon) ? "mumu" : "ee", year), "READ");
  TFile* f_mc    = TFile::Open(Form("histograms/%s_mc-2_%i.hist", (isMuon) ? "mumu" : "ee", year), "READ");
  if(!f_data || !f_embed || !f_mc) return -1;

  ///////////////////////////////////////
  // Plot and save the results
  ///////////////////////////////////////

  gStyle->SetOptStat(0);

  TString dir = Form("figures/val_%s_%i%s", (isMuon) ? "mumu" : "ee", year, (period > -1) ? Form("_period_%i", period) : "");
  gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s ", dir.Data(), dir.Data()));

  int status(0);

  status += print_figure(f_data, f_embed, f_mc, "mass"      , dir);
  status += print_figure(f_data, f_embed, f_mc, "massup"    , dir);
  status += print_figure(f_data, f_embed, f_mc, "massdown"  , dir);
  status += print_figure(f_data, f_embed, f_mc, "genmass"   , dir);
  status += print_figure(f_data, f_embed, f_mc, "pt"        , dir);
  status += print_figure(f_data, f_embed, f_mc, "genpt"     , dir);
  status += print_figure(f_data, f_embed, f_mc, "eta"       , dir);
  status += print_figure(f_data, f_embed, f_mc, "geneta"    , dir);
  status += print_figure(f_data, f_embed, f_mc, "onept"     , dir);
  status += print_figure(f_data, f_embed, f_mc, "twopt"     , dir);
  status += print_figure(f_data, f_embed, f_mc, "oneptup"   , dir);
  status += print_figure(f_data, f_embed, f_mc, "twoptup"   , dir);
  status += print_figure(f_data, f_embed, f_mc, "oneptdown" , dir);
  status += print_figure(f_data, f_embed, f_mc, "twoptdown" , dir);
  status += print_figure(f_data, f_embed, f_mc, "onepterr"  , dir);
  status += print_figure(f_data, f_embed, f_mc, "twopterr"  , dir);
  status += print_figure(f_data, f_embed, f_mc, "onerelerr" , dir);
  status += print_figure(f_data, f_embed, f_mc, "tworelerr" , dir);

  //Compare the resolution parameters

  //means
  TGraphErrors* g_mean_emb = (TGraphErrors*) f_embed->Get("g_means");
  TGraphErrors* g_mean_mc  = (TGraphErrors*) f_mc   ->Get("g_means");
  if(g_mean_emb && g_mean_mc) {
    auto g_mean = CLFV::PlotUtil::GraphDifference(g_mean_mc, g_mean_emb);
    if(!g_mean) {
      cout << "Mean difference not found!\n";
    } else {
      TCanvas * c = new TCanvas();
      g_mean->SetMarkerSize(0.8);
      g_mean->SetMarkerStyle(20);
      g_mean->SetLineWidth(2);
      g_mean->Draw("APE");
      g_mean->SetTitle("MC - Embedding resolution mean;p_{T};#Delta#mu");
      c->SaveAs(Form("%s/resolution_mean_diff.png", dir.Data()));
    }
  } else {
    cout << "Resolution means not found\n";
  }

  //widths
  TGraphErrors* g_width_emb = (TGraphErrors*) f_embed->Get("g_widths");
  TGraphErrors* g_width_mc  = (TGraphErrors*) f_mc   ->Get("g_widths");
  if(g_width_emb && g_width_mc) {
    auto g_width = CLFV::PlotUtil::GraphRatio(g_width_mc, g_width_emb);
    if(!g_width) {
      cout << "Width ratio not found!\n";
    } else {
      TCanvas * c = new TCanvas();
      g_width->SetMarkerSize(0.8);
      g_width->SetMarkerStyle(20);
      g_width->SetLineWidth(2);
      g_width->Draw("APE");
      g_width->SetTitle("MC / Embedding resolution width;p_{T};#sigma ratio");
      c->SaveAs(Form("%s/resolution_width_ratio.png", dir.Data()));
    }
  } else {
    cout << "Resolution widths not found\n";
  }
  return status;
}
