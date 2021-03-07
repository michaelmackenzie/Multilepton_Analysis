//Plot the parameter differences from fits over uncertainty on the difference
void parameter_unc() {
  //               (DM,eta): (0,0)  (0,1)   (1,0)   (1,1)   (10,0)   (10,1)   (11,0)
  double p0_above_45_v[] = {0.2073, 0.1745, 0.149 , 0.1146, 0.07559, 0.07656, 0.04677, 0.01946};
  double p0_above_45_s[] = {0.0093, 0.0146, 0.005 , 0.0077, 0.00640, 0.01016, 0.00540, 0.00834};
  double p0_below_45_v[] = {0.2285, 0.2006, 0.1517, 0.1328, 0.08727, 0.06538, 0.03768, 0.01797};
  double p0_below_45_s[] = {0.0226, 0.0344, 0.0140, 0.0198, 0.01787, 0.02537, 0.01412, 0.01912};

  double p1_above_45_v[] = {-0.0002585,-0.002062 , 0.0001553, 0.0003562, 0.001168, 0.0007812, 0.0002334, 0.0004559};
  double p1_above_45_s[] = { 0.0002767, 0.004544 , 0.0001442, 0.0002285, 0.000194, 0.0003152, 0.0001451, 0.0002441};
  double p1_below_45_v[] = {-0.0003067,-0.0007146, 0.0006091, 3.515e-5 , 0.001255, 0.001157 , 0.000253, 0.000502};
  double p1_below_45_s[] = { 0.0008178, 0.0011930, 0.0005059, 7.165e-4 , 0.000663, 0.000952 , 0.000503, 0.000679};

  //              (DM,eta): (0,0)   (0,1)   (1,0)   (1,1)   (10,0)   (10,1)   (11,0)
  double p0_above_60_v[] = {0.1711, 0.1729, 0.1357, 0.1011, 0.07368, 0.08324, 0.05068, 0.002089};
  double p0_above_60_s[] = {0.0127, 0.0191, 0.0068, 0.0091, 0.00880, 0.01395, 0.00768, 0.010903};
  double p0_below_60_v[] = {0.2330, 0.1638, 0.1599, 0.1136, 0.08470, 0.07595, 0.02630, 0.027920};
  double p0_below_60_s[] = {0.0130, 0.0225, 0.0069, 0.0126, 0.00880, 0.01361, 0.00970, 0.011380};

  double p1_above_60_v[] = { 0.0002913, -0.0005184, 0.0002945, 0.0002485, 0.001087, 0.0007281, 0.0001509, 0.0007649};
  double p1_above_60_s[] = { 0.0003340,  0.0005127, 0.0001702, 0.0002325, 0.000232, 0.0003793, 0.0001680, 0.0003063};
  double p1_below_60_v[] = {-0.0006346,  0.0005799, 0.0001053, 0.0008019, 0.001108, 0.0007063, 0.0008332, 0.0002302};
  double p1_below_60_s[] = { 0.0004405,  0.0007986, 0.0002330, 0.0004485, 0.000309, 0.0004845, 0.0003425, 0.0003821};

  const int n = sizeof(p0_above_45_v)/sizeof(double);
  double p0_45_pulls[n], p1_45_pulls[n];
  double p0_60_pulls[n], p1_60_pulls[n];
  TH1F* hp_45 = new TH1F("hpull_45", "p0 pulls (45)", 40, -4., 4.);
  TH1F* hp_60 = new TH1F("hpull_60", "p0 pulls (60)", 40, -4., 4.);
  for(int i = 0; i < n; ++i) {
    p0_45_pulls[i] = ((p0_above_45_v[i] - p0_below_45_v[i])/
                      sqrt(p0_above_45_s[i]*p0_above_45_s[i] + p0_below_45_s[i]*p0_below_45_s[i]));
    p1_45_pulls[i] = ((p1_above_45_v[i] - p1_below_45_v[i])/
                      sqrt(p1_above_45_s[i]*p1_above_45_s[i] + p1_below_45_s[i]*p1_below_45_s[i]));
    hp_45->Fill(p0_45_pulls[i]);
    hp_45->Fill(p1_45_pulls[i]);
    p0_60_pulls[i] = ((p0_above_60_v[i] - p0_below_60_v[i])/
                      sqrt(p0_above_60_s[i]*p0_above_60_s[i] + p0_below_60_s[i]*p0_below_60_s[i]));
    p1_60_pulls[i] = ((p1_above_60_v[i] - p1_below_60_v[i])/
                      sqrt(p1_above_60_s[i]*p1_above_60_s[i] + p1_below_60_s[i]*p1_below_60_s[i]));
    hp_60->Fill(p0_60_pulls[i]);
    hp_60->Fill(p1_60_pulls[i]);
    printf("(DM,eta) = (%i,%i) pulls: p0(45, 60) = (%6.3f, %6.3f) p1(45, 60) = (%6.3f, %6.3f)\n",(i/2),i%2,
           p0_45_pulls[i], p0_60_pulls[i], p1_45_pulls[i], p1_60_pulls[i]);
  }
  TF1* fgaus = new TF1("fgaus", "gausn", -4., 4.);
  fgaus->Print();
  fgaus->FixParameter(0, hp_45->GetEntries());

  TCanvas* c = new TCanvas();
  hp_45->SetLineWidth(2);
  hp_45->SetFillStyle(3001);
  hp_45->SetFillColor(kBlue);
  hp_45->SetTitle("Parameter pulls");
  hp_45->SetXTitle("(p_{i}^{above} - p_{i}^{below})/#sigma_{#Delta p}");
  hp_45->Fit("fgaus");
  hp_45->Draw();
  c->Print("figures/pulls_ptregions_45.png");

  c = new TCanvas();
  fgaus->FixParameter(0, hp_60->GetEntries());
  hp_60->SetLineWidth(2);
  hp_60->SetFillStyle(3001);
  hp_60->SetFillColor(kBlue);
  hp_60->SetTitle("Parameter pulls");
  hp_60->SetXTitle("(p_{i}^{above} - p_{i}^{below})/#sigma_{#Delta p}");
  hp_60->Fit("fgaus");
  hp_60->Draw();
  c->Print("figures/pulls_ptregions_60.png");
}
