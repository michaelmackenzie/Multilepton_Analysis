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

void combine_efficiencies(int Mode = 0, bool isMuon = true, int year = 2016, int period = -1, bool doCC = false) {

  TFile* fData = TFile::Open(Form("rootfiles/efficiencies_%s_mode-%i_mc-0_%i%s.root", (isMuon) ? "mumu" : "ee", Mode, year, (period >= 0) ? Form("_period_%i", period) : ""), "READ");
  TFile* fMC   = TFile::Open(Form("rootfiles/efficiencies_%s_mode-%i_mc-1_%i%s.root", (isMuon) ? "mumu" : "ee", Mode, year, (period >= 0) ? Form("_period_%i", period) : ""), "READ");
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
  if(isMuon && Mode == 2)
    hScale->GetZaxis()->SetRangeUser(0.8, 1.2);
  else
    hScale->GetZaxis()->SetRangeUser(0.2, 1.5);

  TString basename = Form("%s_mode-%i_%i", (isMuon) ? "mumu" : "ee", Mode, year);
  if(period >= 0) basename += Form("_period_%i", period);
  if(doCC) basename += "_cc";
  c->SaveAs(Form("figures/scales_%s.png", basename.Data()));
  c->SetLogy();
  c->SaveAs(Form("figures/scales_%s_log.png", basename.Data()));

  TFile* fout = new TFile(Form("rootfiles/embedding_eff_%s.root", basename.Data()), "RECREATE");
  fout->cd();
  hData ->SetTitle(Form("%s_mode-%i", (isMuon) ? "mumu" : "ee", Mode));
  hMC   ->SetTitle(Form("%s_mode-%i", (isMuon) ? "mumu" : "ee", Mode));
  hScale->SetTitle(Form("%s_mode-%i", (isMuon) ? "mumu" : "ee", Mode));
  hData->Write();
  hMC->Write();
  hScale->Write();
  fout->Write();
  fout->Close();
}
