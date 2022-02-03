//Combine the Data and MC efficiency measurements into a scale factor file

//fit the scale factors in bins of eta as a function of pT
void fit_scales(TH2F* h, TString tag) {
  //x-axis is the super cluster eta axis
  gSystem->Exec(Form("[ ! -d figures/scale_fits_%s ] && mkdir -p figures/scale_fits_%s", tag.Data(), tag.Data()));
  TVirtualFitter::SetMaxIterations( 1e6 );
  gStyle->SetOptFit(1111);
  for(int xbin = 1; xbin <= h->GetNbinsX(); ++xbin) {
    TH1D* hPt = h->ProjectionY("tmp", xbin, xbin);
    TF1* f;
    // f = new TF1(Form("%s_fit_%i", h->GetName(), xbin), "[offset] + [scale]/(x + [xoffset])", 28., 999.);
    // f->SetParLimits(f->GetParNumber("xoffset"), -28., 1.e5);
    // f->SetParameters(1., 1., -25.);
    f = new TF1(Form("%s_fit_%i", h->GetName(), xbin), "[slope]*std::atan([scale]*(x+[xoffset]))+[offset]", 10., 999.);
    f->SetParameters(0., 1., 1., -30.);
    // f = new TF1(Form("%s_fit_%i", h->GetName(), xbin), "[slope]*std::log([scale]*(x+[xoffset]))/x+[offset]", 28., 999.);
    // f->SetParLimits(f->GetParNumber("xoffset"), -28., 1.e5);
    // f->SetParameters(0., 1., 1., -10.);
    hPt->Fit(f, "R");
    TCanvas* c = new TCanvas();
    hPt->SetMarkerSize(0.8);
    hPt->SetMarkerStyle(20);
    hPt->SetLineColor(kBlue);
    hPt->SetLineWidth(2);
    hPt->Draw("E");

    c->SaveAs(Form("figures/scale_fits_%s/%s_bin_%i.png", tag.Data(), h->GetName(), xbin));
    f->Write();
    delete hPt;
  }
}

void combine_efficiencies(int Mode = 0, bool isMuon = true, int year = 2016, bool doCC = false) {

  TFile* fData = TFile::Open(Form("rootfiles/efficiencies_%s_mode-%i_mc-0_%i.root", (isMuon) ? "mumu" : "ee", Mode, year), "READ");
  TFile* fMC   = TFile::Open(Form("rootfiles/efficiencies_%s_mode-%i_mc-1_%i.root", (isMuon) ? "mumu" : "ee", Mode, year), "READ");
  if(!fData || !fMC) return;

  TH2F* hData  = (TH2F*) fData->Get((doCC) ? "hRatio_cc" : "hRatio");
  if(!hData) {
    cout << "Data histogram not found!\n";
    return;
  }
  hData->SetName("PtVsEtaData");
  TH2F* hMC  = (TH2F*) fMC->Get((doCC) ? "hRatio_cc" : "hRatio");
  if(!hMC) {
    cout << "MC histogram not found!\n";
    return;
  }
  hMC->SetName("PtVsEtaMC");

  TH2F* hScale = (TH2F*) hData->Clone("PtVsEtaSF");
  hScale->Divide(hMC);
  hScale->SetTitle("Data Efficiency / Embedding Efficiency");

  //Ensure all scale factors are reasonable
  for(int xbin = 1; xbin <= hScale->GetNbinsX(); ++xbin) {
    for(int ybin = 1; ybin <= hScale->GetNbinsY(); ++ybin) {
      const double binc = hScale->GetBinContent(xbin, ybin);
      if(binc > 3.) {
        cout << "Warning! Bin " << xbin << ", " << ybin << " ("
             << hScale->GetXaxis()->GetBinLowEdge(xbin) << " < x < " << hScale->GetXaxis()->GetBinLowEdge(xbin) + hScale->GetXaxis()->GetBinWidth(xbin)
             << ", "
             << hScale->GetYaxis()->GetBinLowEdge(ybin) << " < y < " << hScale->GetYaxis()->GetBinLowEdge(ybin) + hScale->GetYaxis()->GetBinWidth(ybin)
             << ") has a scale above 3 = " << binc << ", setting to 3!\n";
        hScale->SetBinContent(xbin, ybin, 3.);
        hScale->SetBinError(xbin, ybin, sqrt(3.));
      }
    }
  }

  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("c", "c", 1000, 700);
  hScale->Draw("colz");
  hScale->GetZaxis()->SetRangeUser(0.2, 1.5);

  c->SaveAs(Form("figures/scales_%s_mode-%i_%i%s.png", (isMuon) ? "mumu" : "ee", Mode, year, (doCC) ? "_cc" : ""));
  c->SetLogy();
  c->SaveAs(Form("figures/scales_%s_mode-%i_log_%i%s.png", (isMuon) ? "mumu" : "ee", Mode, year, (doCC) ? "_cc" : ""));

  TFile* fout = new TFile(Form("rootfiles/embedding_eff_%s_mode-%i_%i.root", (isMuon) ? "mumu" : "ee", Mode, year), "RECREATE");
  fout->cd();
  hData ->SetTitle(Form("%s_mode-%i", (isMuon) ? "mumu" : "ee", Mode));
  hMC   ->SetTitle(Form("%s_mode-%i", (isMuon) ? "mumu" : "ee", Mode));
  hScale->SetTitle(Form("%s_mode-%i", (isMuon) ? "mumu" : "ee", Mode));
  hData->Write();
  hMC->Write();
  hScale->Write();
  // fit_scales(hData , Form("%s_mode-%i_%i", (isMuon) ? "mumu" : "ee", Mode, year));
  // fit_scales(hMC   , Form("%s_mode-%i_%i", (isMuon) ? "mumu" : "ee", Mode, year));
  // fit_scales(hScale, Form("%s_mode-%i_%i", (isMuon) ? "mumu" : "ee", Mode, year));
  fout->Write();
  fout->Close();
}
