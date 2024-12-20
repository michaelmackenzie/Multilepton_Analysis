// Test the CMS colors

void test_colors() {
  vector<int> colors = {kCMSColor_1,
    kCMSColor_2,
    kCMSColor_3,
    kCMSColor_4,
    kCMSColor_5,
    kCMSColor_6,
    kCMSColor_7,
    kCMSColor_8,
    kCMSColor_9,
    kCMSColor_10,
    kCMSColor_S_1,
    kCMSColor_S_2,
    kCMSColor_S_3,
    kCMSColor_S_4,
    kCMSColor_S_5,
    kCMSColor_S_6
  };
  const unsigned ncolors = colors.size();

  TCanvas c;
  for(unsigned icolor = 0; icolor < ncolors; ++icolor) {
    TH1* h = new TH1F(Form("h_%i", icolor), "CMS Colors", ncolors, 1, ncolors+1);
    h->Fill(icolor+1);
    h->SetFillColor(colors[icolor]);
    if(icolor == 0) h->Draw("hist");
    else h->Draw("hist same");
  }
  gStyle->SetOptStat(0);
  c.SaveAs("test_colors.png");
}
