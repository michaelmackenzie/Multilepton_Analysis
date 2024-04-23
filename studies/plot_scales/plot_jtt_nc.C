//Compare the bias region and measurement region j-->tau non-closure

int plot_jtt_nc() {
  TString path = gSystem->Getenv("CMSSW_BASE");
  path += "/src/CLFVAnalysis/scale_factors/";

  for(TString channel : {"mutau", "etau"}) {
    for(TString process : {"WJets", "QCD"}) {
      const int nominal_set = (process == "WJets") ? 31 : 1030;
      const int bias_set    = (process == "WJets") ? 88 : 1095;
      TFile* f_n = TFile::Open(Form("%sjet_to_tau_correction_%s_%s_%i_2016_2017_2018.root", path.Data(), channel.Data(), process.Data(), nominal_set), "READ");
      TFile* f_b = TFile::Open(Form("%sjet_to_tau_correction_%s_%s_%i_2016_2017_2018.root", path.Data(), channel.Data(), process.Data(), bias_set   ), "READ");

      for(TString hist : {"PtScale", "EtaScale", "OneMetDeltaPhi"}) {
        TH1* h_n = (TH1*) f_n->Get(hist.Data());
        TH1* h_b = (TH1*) f_b->Get(hist.Data());
        if(!h_n || !h_b) {
          cout << __func__ << ": Histogram " << hist.Data() << " for channel " << channel.Data() << " and process " << process.Data() << " not found\n";
          continue;
        }
        h_n->SetLineWidth(2);
        h_n->SetLineColor(kBlue);
        h_b->SetLineWidth(2);
        h_b->SetLineColor(kRed);

        gStyle->SetOptStat(0);
        TCanvas c("c", "c", 1000, 1100);
        TPad pad1("pad1", "pad1", 0., 0.3, 1., 1.0); pad1.Draw();
        TPad pad2("pad2", "pad2", 0., 0.0, 1., 0.3); pad2.Draw();
        pad1.SetBottomMargin(0.05); pad2.SetTopMargin(0.03);

        pad1.cd();

        h_n->Draw("hist");
        h_b->Draw("E1 same");
        h_n->GetYaxis()->SetRangeUser(0.6, 1.4);
        if(hist == "PtScale") {
          h_n->SetTitle("p_{T}(l) correction");
        } else if(hist == "EtaScale") {
          h_n->SetTitle("|#eta(#tau)| correction");
        } else if(hist == "OneMetDeltaPhi") {
          h_n->SetTitle("|#Delta#phi(l, MET)| correction");
        }
        h_n->GetYaxis()->SetLabelSize(0.04);
        h_n->GetXaxis()->SetLabelSize(0.04);

        TLegend leg(0.6, 0.7, 0.9, 0.9);
        leg.AddEntry(h_n, "Nominal");
        leg.AddEntry(h_b, "Bias");
        leg.Draw();

        pad2.cd();
        TH1* hratio = (TH1*) h_b->Clone("hratio");
        hratio->Divide(h_n);
        hratio->SetLineWidth(2);
        hratio->SetLineColor(kRed);
        hratio->SetYTitle("Bias / Nominal");
        hratio->Draw("E1");
        hratio->GetYaxis()->SetTitleSize(0.13);
        hratio->GetYaxis()->SetTitleOffset(0.35);
        hratio->GetYaxis()->SetLabelSize(0.08);
        hratio->GetXaxis()->SetLabelSize(0.08);
        hratio->GetYaxis()->SetRangeUser(0.6, 1.4);

        TLine line(hratio->GetXaxis()->GetXmin(), 1., hratio->GetXaxis()->GetXmax(), 1.);
        line.SetLineWidth(2);
        line.SetLineColor(kBlack);
        line.SetLineStyle(kDashed);
        line.Draw("same");

        hist.ToLower();
        hist.ReplaceAll("scale", "");
        c.SaveAs(Form("figures/jtt_nc_vs_bias_%s_%s_%s.png", channel.Data(), process.Data(), hist.Data()));

        delete hratio;
      }
      f_n->Close();
      f_b->Close();
    }
  }

  return 0;
}
