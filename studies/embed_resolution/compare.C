//Script to compare data, embedding, and MC
bool compare_UL_  = false; //compare UL MC to Legacy MC
int correct_res_  = 0;
int correct_es_   = 0;
bool make_scales_ = true; //create files with resolution correction factors
bool is_muon_;

//-------------------------------------------------------------------------------------------------------------------------
// Print a standard figure comparing embedding to MC with an up/down variation
int print_sys_figure(TH1* h_mc, TH1* h_embed, TH1* h_embed_up, TH1* h_embed_down, const char* name, TString dir, const char* tag = "") {
  if(!h_mc || !h_embed || !h_embed_up || !h_embed_down) {
    cout << __func__ << ": Histogram " << name << " not found!\n";
    return 1;
  }
  h_mc        ->Scale(1./h_mc        ->Integral());
  h_embed     ->Scale(1./h_embed     ->Integral());
  h_embed_up  ->Scale(1./h_embed_up  ->Integral());
  h_embed_down->Scale(1./h_embed_down->Integral());

  //Create the plot
  TCanvas* c = new TCanvas("c", "c", 1000, 1100);
  TPad* pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1.0); pad1->Draw();
  TPad* pad2 = new TPad("pad2", "pad2", 0., 0.0, 1., 0.3); pad2->Draw();
  pad1->SetBottomMargin(0.05); pad2->SetTopMargin(0.03);

  //Draw the histograms
  pad1->cd();
  pad1->SetGrid();
  h_mc->SetMarkerStyle(20);
  h_mc->SetMarkerSize(0.8);
  h_embed->SetLineColor(kBlue);
  h_embed->SetLineWidth(2);
  h_embed_up->SetLineColor(kRed);
  h_embed_up->SetLineWidth(2);
  h_embed_down->SetLineColor(kRed);
  h_embed_down->SetLineWidth(2);
  h_mc->Draw("E1");
  h_embed_up  ->Draw("hist same");
  h_embed_down->Draw("hist same");
  h_embed     ->Draw("hist same");
  h_mc->Draw("E1 same");
  h_mc->GetYaxis()->SetRangeUser(0, 1.2*max(h_mc->GetMaximum(), max(h_embed_down->GetMaximum(), h_embed_up->GetMaximum())));

  TLegend* leg = new TLegend(0.6, 0.75, 0.9, 0.9);
  leg->AddEntry(h_mc      , "MC"                 , "PL");
  leg->AddEntry(h_embed   , "Embedding"          , "L" );
  leg->AddEntry(h_embed_up, "Embedding (up/down)", "L" );
  leg->Draw();

  //Draw the ratio plots
  pad2->cd();
  pad2->SetGrid();
  TH1* hr_embed      = (TH1*) h_mc->Clone(Form("%s_ratio", h_embed     ->GetName()));
  TH1* hr_embed_up   = (TH1*) h_mc->Clone(Form("%s_ratio", h_embed_up  ->GetName()));
  TH1* hr_embed_down = (TH1*) h_mc->Clone(Form("%s_ratio", h_embed_down->GetName()));
  hr_embed     ->Divide(h_embed);
  hr_embed_up  ->Divide(h_embed_up);
  hr_embed_down->Divide(h_embed_down);
  hr_embed     ->SetLineColor(h_embed     ->GetLineColor());
  hr_embed_up  ->SetLineColor(h_embed_up  ->GetLineColor());
  hr_embed_down->SetLineColor(h_embed_down->GetLineColor());
  hr_embed     ->SetLineWidth(2);
  hr_embed_up  ->SetLineWidth(2);
  hr_embed_down->SetLineWidth(2);

  hr_embed     ->Draw("hist");
  hr_embed_up  ->Draw("same hist");
  hr_embed_down->Draw("same hist");
  hr_embed     ->Draw("same hist");
  pad2->Update();

  hr_embed->SetTitle("");
  hr_embed->SetXTitle("");
  hr_embed->GetYaxis()->SetTitle("MC/Embed.");
  hr_embed->GetYaxis()->SetTitleSize(0.13);
  hr_embed->GetYaxis()->SetTitleOffset(0.30);
  hr_embed->GetYaxis()->SetLabelSize(0.08);
  hr_embed->GetXaxis()->SetLabelSize(0.08);
  hr_embed->GetYaxis()->SetRangeUser(0.5, 1.5);
  hr_embed_up->GetYaxis()->SetRangeUser(0.5, 1.5);
  hr_embed_down->GetYaxis()->SetRangeUser(0.5, 1.5);

  TLine* line = new TLine(hr_embed->GetXaxis()->GetXmin(), 1., hr_embed->GetXaxis()->GetXmax(), 1.);
  line->SetLineColor(kBlack);
  line->SetLineStyle(kDashed);
  line->SetLineWidth(2);
  line->Draw("same");


  c->SaveAs(Form("%s/mc_sys_%s%s.png", dir.Data(), name, tag));
  delete c;
  return 0;
}

int print_sys_figure(TFile* f_mc, TFile* f_embed, const char* name, TString dir, const char* tag = "") {
  //Retrieve the histograms
  TH1* h_mc         = (TH1*) f_mc   ->Get(name);
  TH1* h_embed      = (TH1*) f_embed->Get(name);
  TH1* h_embed_up   = (TH1*) f_embed->Get(Form("%sup"  , name));
  TH1* h_embed_down = (TH1*) f_embed->Get(Form("%sdown", name));
  if(!h_mc || !h_embed || !h_embed_up || !h_embed_down) {
    cout << __func__ << ": Histogram " << name << " not found!\n";
    return 1;
  }
  h_mc->SetName(Form("%s_mc", name));
  h_embed->SetName(Form("%s_embed", name));
  h_embed_up->SetName(Form("%s_embed_up", name));
  h_embed_down->SetName(Form("%s_embed_down", name));
  return print_sys_figure(h_mc, h_embed, h_embed_up, h_embed_down, name, dir, tag);
}


//-------------------------------------------------------------------------------------------------------------------------
// Print a standard figure comparing 3 histograms
int print_figure(TH1* h_data, TH1* h_embed, TH1* h_mc, const char* name, TString dir, const char* tag = "",
                 const char* leg1 = "Data", const char* leg2 = "Embedding", const char* leg3 = "MC") {
  if(!h_data || !h_embed || !h_mc) {
    cout << __func__ << ": Histogram " << name << " not found!\n";
    return 1;
  }
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
  leg->AddEntry(h_data , leg1, "PL");
  leg->AddEntry(h_embed, leg2, "L" );
  leg->AddEntry(h_mc   , leg3, "L" );
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


  c->SaveAs(Form("%s/%s%s.png", dir.Data(), name, tag));
  delete c;
  return 0;
}

int print_figure(TFile* f_data, TFile* f_embed, TFile* f_mc, const char* name, TString dir, const char* tag = "",
                 const char* leg1 = "Data", const char* leg2 = "Embedding", const char* leg3 = "MC") {
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
  return print_figure(h_data, h_embed, h_mc, name, dir, tag, leg1, leg2, leg3);
}

//compare resolution fit parameters
int res_fit_comparison(const char* name, TFile* f_embed, TFile* f_mc, TString dir, bool print_info = false) {
  int status(0);
  //means
  TGraphErrors* g_mean_emb = (TGraphErrors*) f_embed->Get(Form("g_means_%s", name));
  TGraphErrors* g_mean_mc  = (TGraphErrors*) f_mc   ->Get(Form("g_means_%s", name));
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
      // g_mean->Fit("pol1");
      c->SaveAs(Form("%s/resolution_%s_mean_diff.png", dir.Data(), name));
      if(make_scales_ && correct_es_ == 0) {
        g_mean->SetName(Form("%s_mean_diff", name));
        g_mean->Write();
      }
      delete c;
      delete g_mean;
    }
  } else {
    cout << "Resolution " << name << " means not found\n";
    ++status;
  }

  //widths
  TGraphErrors* g_width_emb = (TGraphErrors*) f_embed->Get(Form("g_widths_%s", name));
  TGraphErrors* g_width_mc  = (TGraphErrors*) f_mc   ->Get(Form("g_widths_%s", name));
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
      TF1* func = new TF1("func", "[0] + [1]*x");
      func->SetParameters((correct_res_) ? 1. : 1.5, 0.001);
      // g_width->Fit(func, "w");
      if(correct_res_) {
        g_width->GetYaxis()->SetRangeUser(0.5, 1.5);
      } else {
        if(is_muon_)
          g_width->GetYaxis()->SetRangeUser(0.5, 1.5);
        else
          g_width->GetYaxis()->SetRangeUser(1., 2.5);
      }
      // func->Draw("same");
      c->SaveAs(Form("%s/resolution_%s_width_ratio.png", dir.Data(), name));

      if(print_info) {
        printf("Width ratios: %s\n", name);
        for(int ipoint = 0; ipoint < g_width->GetN(); ++ipoint) {
          const double x  = g_width->GetX()[ipoint];
          const double xe = g_width->GetEXhigh()[ipoint];
          const double y  = g_width->GetY()[ipoint];
          printf(" %5.1f - %5.1f: %.3f\n", x-xe, x+xe, y);
        }
      }
      if(make_scales_ && correct_res_ == 0) {
        g_width->SetName(Form("%s_width_ratio", name));
        g_width->Write();
      }
      delete c;
      delete g_width;
      delete func;
    }
  } else {
    cout << "Resolution " << name << " widths not found\n";
    ++status;
  }
  return status;
}

int compare(bool isMuon = true, int year = 2016, int period = -1, int correct_res = 0, int correct_es = 0) {
  is_muon_ = isMuon;
  if(!isMuon) {
    correct_res_ = correct_res;
    correct_es_  = correct_es ;
  } else {
    correct_res_ = 0;
    correct_es_  = 0;
  }

  //Read in the histogram files
  TFile* f_data  = TFile::Open(Form("histograms/%s_mc-0_r0_e0_%i.hist"  , (isMuon) ? "mumu" : "ee"                           , year), "READ");
  TFile* f_embed = TFile::Open(Form("histograms/%s_mc-1_r%i_e%i_%i.hist", (isMuon) ? "mumu" : "ee", correct_res_, correct_es_, year), "READ");
  TFile* f_mc    = TFile::Open(Form("histograms/%s_mc-2_r0_e0_%i.hist"  , (isMuon) ? "mumu" : "ee"                           , year), "READ");
  TFile* f_mc_ul = TFile::Open(Form("histograms/%s_mc-3_r0_e0_%i.hist"  , (isMuon) ? "mumu" : "ee"                           , year), "READ");
  if(!f_data || !f_embed || !f_mc) return -1;
  if(compare_UL_ && !f_mc_ul) return -1;

  ///////////////////////////////////////
  // Plot and save the results
  ///////////////////////////////////////

  gStyle->SetOptStat(0);

  TString dir = Form("figures/val_%s_r%i_e%i_%i%s", (isMuon) ? "mumu" : "ee", correct_res_, correct_es_,  year, (period > -1) ? Form("_period_%i", period) : "");
  gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s ", dir.Data(), dir.Data()));

  int status(0);

  status += print_figure(f_data, f_embed, f_mc, "mass"      , dir);
  status += print_figure(f_data, f_embed, f_mc, "massup"    , dir);
  status += print_figure(f_data, f_embed, f_mc, "massesdown", dir);
  status += print_figure(f_data, f_embed, f_mc, "massesup"  , dir);
  status += print_figure(f_data, f_embed, f_mc, "massdown"  , dir);
  status += print_figure(f_data, f_embed, f_mc, "genmass"   , dir);
  status += print_figure(f_data, f_embed, f_mc, "pt"        , dir);
  status += print_figure(f_data, f_embed, f_mc, "genpt"     , dir);
  status += print_figure(f_data, f_embed, f_mc, "eta"       , dir);
  status += print_figure(f_data, f_embed, f_mc, "geneta"    , dir);
  status += print_figure(f_data, f_embed, f_mc, "oneeta"    , dir);
  status += print_figure(f_data, f_embed, f_mc, "twoeta"    , dir);
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

  //Compare Embedding resolution shifts
  status += print_figure((TH1*) f_data->Get("mass"), (TH1*) f_embed->Get("mass"), (TH1*) f_embed->Get("massup"), "mass_embed_up", dir, "", "Data", "Embedding", "Embedding up");
  // status += print_figure((TH1*) f_mc->Get("onerelerr"), (TH1*) f_embed->Get("onerelerr"), (TH1*) f_embed->Get("onerelerrup"), "mc_embed_res", dir, "", "MC", "Embedding", "Embedding up");
  status += print_sys_figure(f_mc, f_embed, "onerelerr", dir, "");

  {
    TH2* h_mc_pt_vs_err = (TH2*) f_mc->Get("lepptvsres");
    TH2* h_emb_pt_vs_err = (TH2*) f_embed->Get("lepptvsres");
    TH2* h_emb_pt_vs_err_up = (TH2*) f_embed->Get("lepptvsresup");
    if(h_mc_pt_vs_err && h_emb_pt_vs_err && h_emb_pt_vs_err_up) { //compare the resolution in each pT bin
      const int nbins = h_mc_pt_vs_err->GetNbinsX();
      for(int bin = 1; bin <= nbins; ++bin) {
        TH1* hmc     = h_mc_pt_vs_err    ->ProjectionY("mc", bin, bin);
        TH1* hemb    = h_emb_pt_vs_err   ->ProjectionY("emb", bin, bin);
        TH1* hemb_up = h_emb_pt_vs_err_up->ProjectionY("embup", bin, bin);
        status += print_figure(hmc, hemb, hemb_up, Form("mc_embed_res_%i", bin), dir, "", "MC", "Embedding", "Embedding up");
        delete hmc;
        delete hemb;
        delete hemb_up;
      }
    }
  }

  //Compare MC and UL MC
  if(compare_UL_) {
    status += print_figure(f_data, f_mc, f_mc_ul, "mass", dir, "_UL", "Data", "MC", "UL MC");
    status += print_figure((TH1*) f_mc_ul->Get("mass"), (TH1*) f_mc->Get("mass"), (TH1*) f_mc->Get("massesup"  ), "mass_es_up"  , dir, "_UL", "UL MC", "MC", "MC up"  );
    status += print_figure((TH1*) f_mc_ul->Get("mass"), (TH1*) f_mc->Get("mass"), (TH1*) f_mc->Get("massesdown"), "mass_es_down", dir, "_UL", "UL MC", "MC", "MC down");
  }

  //Compare the resolution parameters

  //Save the graphs if uncorrected version
  TFile* fOut(nullptr);
  if(make_scales_) {
    if(correct_res_ == 0 && correct_es_ == 0) fOut = new TFile(Form("rootfiles/embed_scale_correction_%s_%i.root"     , (isMuon) ? "muon" : "electron", year), "RECREATE");
    else if(correct_res_ == 0)                fOut = new TFile(Form("rootfiles/embed_resolution_correction_%s_%i.root", (isMuon) ? "muon" : "electron", year), "RECREATE");
  }
  if(fOut) fOut->cd();
  status += res_fit_comparison("lepptvsres"      , f_embed, f_mc, dir);
  for(int igraph = 0; ; ++igraph) {
    const char* name = Form("lepptvsres_eta_%i", igraph);
    if(f_embed->Get(Form("g_widths_%s", name))) status += res_fit_comparison(name, f_embed, f_mc, dir);
    else break;
  }
  if(fOut) fOut->Close();
  return status;
}
