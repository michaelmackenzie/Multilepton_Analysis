//Plot the output of the ChannelCompatibilityCheck output
double scale_ = 1.;
bool blinding_offset_ = true;
bool preliminary_ = true;

int plot_channel_compatibility(const char* file_name, TString tag) {

  TFile* f = TFile::Open(file_name, "READ");
  if(!f) return 1;

  RooFitResult* fit_nom = (RooFitResult*) f->Get("fit_nominal");
  RooFitResult* fit_alt = (RooFitResult*) f->Get("fit_alternate");
  if(!fit_nom || !fit_alt) {
    cout << "RooFitResult objects not found\n";
    return 2;
  }

  const bool mutau = tag.Contains("zmutau");
  const bool etau  = tag.Contains("zetau");
  const bool emu   = tag.Contains("zemu");

  //POI value --> physical value
  const double scale = 1.e-6;

  //Get the nominal signal rate
  double nom(0.), nom_err_up(0.), nom_err_down(0.);
  {
    auto list = fit_nom->floatParsFinal();
    for(auto obj : list) {
      if(TString(obj->GetName()) == "r") {
        auto var = (RooRealVar*) obj;
        nom          = scale*var->getVal();
        nom_err_up   = scale*var->getError();
        nom_err_down = scale*var->getError();
      }
    }
  }

  //Retrieve each alternate fit result
  vector<TString> names;
  vector<double> alt, alt_err_up, alt_err_down;
  {
    auto list = fit_alt->floatParsFinal();
    for(auto obj : list) {
      TString name = obj->GetTitle();
      if(name.Contains("ChannelCompatibilityCheck")) {
        auto var = (RooRealVar*) obj;
        alt         .push_back(scale*var->getVal());
        alt_err_up  .push_back(scale*var->getError());
        alt_err_down.push_back(scale*var->getError());

        //determine the label for the variable
        name.ReplaceAll("_ChannelCompatibilityCheck_r_", "");
        name.ReplaceAll("_mva_25", " Central");
        name.ReplaceAll("_mva_26", " High");
        name.ReplaceAll("_mva_27", " Low");
        name.ReplaceAll("_mva_28", " Z->ll");
        name.ReplaceAll("_y", " ");
        if(mutau) {
          name.ReplaceAll("lep", "#mu#tau_{e}");
          name.ReplaceAll("had", "#mu#tau_{h}");
        } else if(etau) {
          name.ReplaceAll("lep", "e#tau_{#mu}");
          name.ReplaceAll("had", "e#tau_{h}");
        }
        names.push_back(name);

      }
    }
  }
  cout << "Found " << names.size() << " alternate fits\n";
  if(names.size() == 0) return 3;

  const double y_size = 0.2;

  const int nentries = names.size() + 1;
  TString labels[nentries];
  double obs[nentries], up[nentries], down[nentries], y[nentries], yerr[nentries];
  double r, rloerr, rhierr;

  //Retrieve the fit info
  double min_val(1.e10), max_val(-1.e10);
  for(int entry = 0; entry < nentries; ++entry) {
    r      = (entry == 0) ? nom          : alt         [entry-1];
    rloerr = (entry == 0) ? nom_err_up   : alt_err_up  [entry-1];
    rhierr = (entry == 0) ? nom_err_down : alt_err_down[entry-1];
    obs [entry] = r;
    up  [entry] = rhierr;
    down[entry] = rloerr;
    labels[entry] = (entry == 0) ? "Nominal" : names[entry-1];

    y[entry] = entry+1;
    yerr[entry] = y_size;
    max_val = max(max_val, obs[entry] + up  [entry]);
    min_val = min(min_val, obs[entry] - down[entry]);


    printf("%2i: r = %.5e (+%.5e, -%.5e) [%.5e - %.5e]\n", entry,
           obs[entry], up[entry], down[entry],
           obs[entry] - down[entry], obs[entry]  + up[entry]);
  }
  f->Close();

  //Constrain the axis range to ignore very uncertain fits
  // min_val = max(min_val, nom - 8.*nom_err_down);
  // max_val = min(max_val, nom + 8.*nom_err_up  );

  const double reference = obs[0];
  double chi_sq(0.), sigmas[nentries];
  for(int i = 0; i < nentries; ++i) {
    sigmas[i] = (obs[i] - reference)/((obs[i] > reference) ? down[i] : up[i]);
    chi_sq += sigmas[i]*sigmas[i];
  }
  printf("Overall chi^2 = %.1f / %i = %.2f, p(chi^2) = %.3f\n",
         chi_sq, nentries-1, chi_sq/(nentries-1), TMath::Prob(chi_sq, nentries-1));

  TGraphAsymmErrors* gobs = new TGraphAsymmErrors(nentries, obs, y, down, up, yerr, yerr);
  gobs->SetName("obs");
  gobs->SetFillColor(kGreen+1); //kBlue-9); //kGreen+1
  gobs->SetLineColor(kBlack); //kBlue
  gobs->SetMarkerColor(kBlack); //kBlue
  gobs->SetMarkerStyle(20); //20
  gobs->SetMarkerSize(1.5); //1.5
  gobs->SetLineWidth(2);
  gobs->SetTitle(Form(";Observed BF(Z^{0} #rightarrow %s^{#pm}%s^{#mp});",
                      tag.Contains("zmu") ? "#mu" : "e", tag.Contains("emu") ? "#mu" : "#tau"));

  const double ymax = 1.3*nentries;
  const double ymin = 0.5;

  //calculate x-axis range
  const float range = std::fabs(max_val - min_val);
  const float xmax = max_val + (0.10)*range;
  const float xmin = min(0., min_val) - (0.10)*range;
  // printf("Max val = %.2e, Min val = %.2e --> xrange = [%.2e , %.2e]\n", max_val, min_val, xmin, xmax);
  // printf("yrange = [%.2f , %.2f]\n", ymin, ymax);

  gStyle->SetOptStat(0);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);

  TCanvas* c = new TCanvas("c", "c", 800, 800);
  c->SetLeftMargin(0.21);
  //Create a histogram to use as the axis
  TH1* haxis = new TH1F("haxis", "", 1, xmin, xmax);
  haxis->SetTitle(gobs->GetTitle());
  haxis->Draw();

  //Add the graphs to the plot

  gobs->Draw("PE2");
  haxis->GetYaxis()->SetRangeUser(ymin, ymax);
  haxis->GetXaxis()->SetRangeUser(xmin, xmax);
  haxis->GetYaxis()->SetLabelOffset(1e10);
  haxis->GetYaxis()->SetLabelSize(0);
  haxis->GetXaxis()->SetTitleOffset(1.2);

  TGaxis::SetMaxDigits(3);

  c->SetGridx();

  //Add a line, assuming the last entry is the overall result
  const double final_val = obs[0];
  TLine line(final_val, ymin, final_val, ymax);
  line.SetLineWidth(2);
  line.SetLineStyle(kDashed);
  line.SetLineColor(kRed);
  line.Draw("same");

  TLine line_sm(0., ymin, 0., ymax);
  line_sm.SetLineWidth(2);
  line_sm.SetLineStyle(kSolid);
  line_sm.SetLineColor(kBlack);
  line_sm.Draw("same");

  //Add labels to the plot
  TLatex label;
  label.SetNDC();

  //add CMS label
  label.SetTextFont(62);
  label.SetTextColor(1);
  label.SetTextSize(0.05);
  label.SetTextAlign(13);
  label.SetTextAngle(0);
  label.DrawLatex(0.25, 0.89, "CMS");
  if(preliminary_) {
    label.SetTextFont(72);
    label.SetTextSize(0.04);
    label.SetTextAlign(22);
    label.SetTextAngle(0);
    label.DrawLatex(0.35, 0.82, "Preliminary");
  }

  //Draw the category labels
  label.SetTextFont(62);
  // label.SetTextFont(72);
  label.SetTextAlign(32);
  label.SetTextAngle(0);
  label.SetTextSize(0.027);
  for(int index = 0; index < nentries; ++index) {
    const double yoffset = 0.1; //axis starts here
    const double yloc = yoffset + 0.8*(y[index] - ymin) / (ymax - ymin);
    label.DrawLatex(0.20, yloc, Form("%15s", labels[index].Data()));
  }

  c->Print(Form("compatibility_%s.png", tag.Data()));


  return 0;
}
