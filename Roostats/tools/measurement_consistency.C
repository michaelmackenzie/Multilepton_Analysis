//Script to process measurements for channels/years/etc. and plot the results
double scale_ = 1.;
bool speed_limit_     = true; //use Combine arguments to speed up limit calculation
bool preliminary_     = true;
bool add_values_      = true; //add text values of the limits to the plot
bool add_sigmas_      = true; //sigma for each category from expected (measured if blinded)
bool blinding_offset_ = true;
bool add_asimov_      = false; //add Asimov error bar sizes

struct config_t {
  TString name_;
  TString label_;
  vector<int> sets_;
  vector<int> years_;
  double scale_;
  double rmin_;
  double rmax_;

  config_t(TString name, TString label, vector<int> sets, vector<int> years, double scale = 1.):
    name_(name), label_(label), sets_(sets), years_(years), scale_(scale), rmin_(-50.), rmax_(50.) {}
};

int measurement_consistency(vector<config_t> configs, //info for each entry
                            TString tag, //tag for the output figure file name
                            TString selection = "zemu",
                            int processCards = 1) {

  if(configs.size() == 0) {
    cout << "No configuration cards given!\n";
    return 1;
  }

  //blinding information
  TRandom3 rnd((selection == "zmutau") ? 90 : (selection == "zetau") ? 91 : 92); //different seed for each selection, to not compare between plots
  const double offset = (blinding_offset_) ? 300.*(rnd.Uniform() - 0.5) : 0.; //offset the measurements by a fixed value if blinding
  const int nfiles = configs.size();
  double obs[nfiles], up[nfiles], down[nfiles], y[nfiles], yerr[nfiles], asimov_up[nfiles], asimov_down[nfiles], sigmas[nfiles];
  double max_val(-1.e10), min_val(1.e10);
  const double y_size = (nfiles < 5) ? 0.3 : 0.22; //half size of measurement bar in y-dimension

  if(processCards >= 0) { //processCards < 0 --> Use fixed values
    //Fit results for each input
    vector<TFile*> file_list;
    vector<TTree*> tree_list;
    vector<TTree*> asimov_list;

    //Loop through each input configuration
    for(config_t config : configs) {
      vector<int> years = config.years_;
      TString card      = config.name_;
      const double rmin = config.rmin_;
      const double rmax = config.rmax_;


      //Move to the proper directory
      if(years.size() == 0) { cout << "No years given!\n"; return 2; }
      TString year_string = Form("%i", years[0]);
      for(int i = 1; i < years.size(); ++i) year_string += Form("_%i", years[i]);
      TString dir = Form("datacards/%s", year_string.Data());
      cout << ">>> Using directory " << dir.Data() << endl;
      gSystem->cd(dir.Data());

      int status(0);
      if(processCards) {
        TString additional_command = "";
        if(speed_limit_) additional_command += " --cminDefaultMinimizerStrategy 0";
        if(selection == "zemu") {
          additional_command += " --X-rtd MINIMIZER_freezeDisassociatedParams";
          additional_command += " --X-rtd REMOVE_CONSTANT_ZERO_POINT=1";
          additional_command += " --X-rtd MINIMIZER_multiMin_hideConstants";
          additional_command += " --X-rtd MINIMIZER_multiMin_maskConstraints";
        } else { //tau channels
          additional_command += " --cminPreScan --cminPreFit 1 --cminApproxPreFitTolerance 0.1";
        }

        //Run combine on each datacard
        printf("Processing combine card %s/combine_%s.txt\n", dir.Data(), card.Data());
        TString command = Form("combine -M FitDiagnostics -d combine_%s.txt --name _%s --rMin %.1f --rMax %.1f %s >| fit_%s.log",
                               card.Data(),
                               card.Data(),
                               rmin, rmax,
                               additional_command.Data(),
                               card.Data());
        printf(">>> %s\n", command.Data());
        gSystem->Exec(command.Data());
        if(add_asimov_) { //evaluate the expected uncertainties using the Asimov template
          command.ReplaceAll(Form("--name _%s", card.Data()), Form("--name _%s_Asimov", card.Data()));
          command.ReplaceAll(" -d", " -t -1 -d");
          command.ReplaceAll(">| fit_", ">| fit_asimov_");
          printf(">>> %s\n", command.Data());
          gSystem->Exec(command.Data());
        }
      }

      TFile* f = TFile::Open(Form("fitDiagnostics_%s.root", card.Data()), "READ");
      if(!f) return 4;
      file_list.push_back(f);
      TTree* t = (TTree*) f->Get("tree_fit_sb");
      if(!t) {
        cout << "Tree for card name " << card.Data() << " not found\n";
        return 5;
      }
      t->SetName(Form("tree_fit_sb_%s", card.Data()));
      tree_list.push_back(t);
      if(add_asimov_) { //add the asimov expected uncertainties
        TFile* f = TFile::Open(Form("fitDiagnostics_%s_Asimov.root", card.Data()), "READ");
        if(!f) return 4;
        file_list.push_back(f);
        TTree* t = (TTree*) f->Get("tree_fit_sb");
        if(!t) {
          cout << "Tree for card name " << card.Data() << " not found\n";
          return 5;
        }
        t->SetName(Form("tree_fit_sb_%s_asimov", card.Data()));
        asimov_list.push_back(t);
      }
      gSystem->cd("../..");
    }

    for(int itree = 0; itree < nfiles; ++itree) {
      TTree* t = tree_list[itree];
      double r, rloerr, rhierr;
      const double scale = configs[itree].scale_;
      t->SetBranchAddress("r", &r);
      t->SetBranchAddress("rLoErr", &rloerr);
      t->SetBranchAddress("rHiErr", &rhierr);
      t->GetEntry(0);
      obs [itree] = scale*(r + offset);
      up  [itree] = scale*rhierr;
      down[itree] = scale*rloerr;

      y[itree] = nfiles - itree;
      yerr[itree] = y_size;
      max_val = max(max_val, obs[itree] + up  [itree]);
      min_val = min(min_val, obs[itree] - down[itree]);

      if(add_asimov_) {
        TTree* t_a = asimov_list[itree];
        t_a->SetBranchAddress("rLoErr", &rloerr);
        t_a->SetBranchAddress("rHiErr", &rhierr);
        t_a->GetEntry(0);
        asimov_up  [itree] = scale*rhierr;
        asimov_down[itree] = scale*rloerr;
      }

      printf("%s: r = %.5e (+%.5e, -%.5e) [%.5e - %.5e]", configs[itree].name_.Data(),
             obs[itree], up[itree], down[itree],
             obs[itree] - down[itree], obs[itree]  + up[itree]);
      if(add_asimov_) {
        printf(" asimov: (+%.5e, -%.5e)", asimov_up[itree], asimov_down[itree]);
      }
      printf("\n");
    }
    for(auto file : file_list) file->Close();
  } else { //use fixed values
    if(selection == "zemu" && nfiles == 4) {
      const double scale = 2.62e-7;
      obs[0] = -0.469*scale; up[0] = 1.359*scale; down[0] = 2.092*scale;
      obs[1] =  0.169*scale; up[1] = 0.576*scale; down[1] = 0.545*scale;
      obs[2] =  0.005*scale; up[2] = 0.577*scale; down[2] = 0.766*scale;
      obs[3] =  0.055*scale; up[3] = 0.386*scale; down[3] = 0.427*scale;
      y[0] = 4; yerr[0] = y_size;
      y[1] = 3; yerr[1] = y_size;
      y[2] = 2; yerr[2] = y_size;
      y[3] = 1; yerr[3] = y_size;
      for(int ibin = 0; ibin < nfiles; ++ibin) {
        max_val = max(max_val, obs[ibin] + up  [ibin]);
        min_val = min(min_val, obs[ibin] - down[ibin]);
        printf("%s: BR(zemu) = %.3e +%.3e -%.3e, %.3f sigma\n", configs[ibin].label_.Data(), obs[ibin], up[ibin], down[ibin],
               (obs[ibin] > 0.) ? obs[ibin]/down[ibin] : obs[ibin]/up[ibin]);
      }
    }
  }

  //measure the sigmas, assuming the last observation is the best in the blinded case
  const double reference = (blinding_offset_) ? obs[nfiles-1] : 0.;
  double chi_sq = 0.;
  for(int i = 0; i < nfiles; ++i) {
    sigmas[i] = (obs[i] - reference)/((obs[i] > reference) ? down[i] : up[i]);
    chi_sq += sigmas[i]*sigmas[i];
  }
  printf("Overall chi^2 = %.1f / %i = %.2f, p(chi^2) = %.3f\n",
         chi_sq, nfiles, chi_sq/nfiles, TMath::Prob(chi_sq, nfiles));

  TGraphAsymmErrors* gobs = new TGraphAsymmErrors(nfiles, obs, y, down, up, yerr, yerr);
  gobs->SetName("obs");
  gobs->SetFillColor(kGreen+1);
  gobs->SetMarkerStyle(20);
  gobs->SetMarkerSize(1.5);
  gobs->SetLineWidth(2);
  gobs->SetLineColor(kBlue);
  gobs->SetMarkerColor(kBlue);
  gobs->SetTitle(Form(";Observed BF(Z^{0} #rightarrow %s^{#pm}%s^{#mp});",
                      selection.BeginsWith("zmu") ? "#mu" : "e", selection.EndsWith("mu") ? "#mu" : "#tau"));

  const double ymax = 1.3*nfiles;
  const double ymin = 0.5;

  //calculate x-axis range
  const float xmax = max_val + ((add_sigmas_) ? 0.2 : 0.1)*fabs(max_val);
  const float xmin = min_val - ((add_values_) ? 0.2 : 0.1)*fabs(min_val);
  printf("Max val = %.2e, Min val = %.2e --> xrange = [%.2e , %.2e]\n", max_val, min_val, xmin, xmax);

  gStyle->SetOptStat(0);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);

  TCanvas* c = new TCanvas("c", "c", 800, 800);
  //Create a histogram to use as the axis
  TH1* haxis = new TH1F("haxis", "", 1, xmin, xmax);
  haxis->SetTitle(gobs->GetTitle());
  haxis->Draw();

  //Add the graphs to the plot

  if(add_asimov_) {
    TGraphAsymmErrors* gasimov = new TGraphAsymmErrors(nfiles, obs, y, asimov_down, asimov_up, yerr, yerr);
    gasimov->SetName("asimov");
    gasimov->SetFillColor(kGreen+1);
    gasimov->SetMarkerColor(0);
    gasimov->SetLineColor(0);
    gasimov->Draw("E2");
  }

  gobs->Draw((add_asimov_) ? "PE" : "PE2");
  haxis->GetYaxis()->SetRangeUser(ymin, ymax);
  haxis->GetXaxis()->SetRangeUser(xmin, xmax);
  haxis->GetYaxis()->SetLabelOffset(1e10);
  haxis->GetYaxis()->SetLabelSize(0);
  haxis->GetXaxis()->SetTitleOffset(1.2);

  TGaxis::SetMaxDigits(3);

  c->SetGridx();

  //Add a line, assuming the last entry is the overall result
  const double final_val = obs[nfiles-1];
  TLine line(final_val, ymin, final_val, ymax);
  line.SetLineWidth(2);
  line.SetLineStyle(kDashed);
  line.SetLineColor(kBlack);
  line.Draw("same");

  //Add labels to the plot
  TLatex label;
  label.SetNDC();

  //add CMS label
  label.SetTextFont(62);
  label.SetTextColor(1);
  label.SetTextSize(0.05);
  label.SetTextAlign(13);
  label.SetTextAngle(0);
  label.DrawLatex(0.13, 0.89, "CMS");
  if(preliminary_) {
    label.SetTextFont(72);
    label.SetTextSize(0.04);
    label.SetTextAlign(22);
    label.SetTextAngle(0);
    label.DrawLatex(0.23, 0.82, "Preliminary");
  }

  //Draw the category labels
  label.SetTextFont(72);
  label.SetTextSize(0.05);
  label.SetTextAlign(13);
  label.SetTextAngle(0);
  label.SetTextSize(0.03);
  for(int icard = 0; icard < nfiles; ++icard) {
    const double yoffset = 0.1; //axis starts here
    const double yloc = yoffset + 0.8*(icard + 0.8) / (ymax - ymin);
    const int index = nfiles-icard-1; //looping in reverse
    if(add_values_)
      label.DrawLatex(0.01, yloc, Form("%s: %.2e", configs[index].label_.Data(), obs[index]));
    else
      label.DrawLatex(0.01, yloc, Form("%s", configs[index].label_.Data()));
    if(add_sigmas_) {
      label.DrawLatex(0.82, yloc, Form("%.1f#sigma", sigmas[index]));
    }
  }

  gSystem->Exec("[ ! -d measurements ] && mkdir measurements");
  c->Print(Form("measurements/measurement_%s_%s.png", selection.Data(), tag.Data()));


  return 0;
}
