//Script to process measurements for channels/years/etc. and plot the results
double scale_ = 1.;
bool speed_limit_     = true; //use Combine arguments to speed up limit calculation
bool preliminary_     = true;
bool add_values_      = true; //add text values of the limits to the plot
bool blinding_offset_ = true;

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
                            bool processCards = true) {

  if(configs.size() == 0) {
    cout << "No configuration cards given!\n";
    return 1;
  }


  //Fit results for each input
  vector<TFile*> file_list;
  vector<TTree*> tree_list;

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
    }

    // TFile* f = TFile::Open(Form("higgsCombine_%s.FitDiagnostics.mH120.root", card.Data()), "READ");
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
    gSystem->cd("../..");
  }

  TRandom3 rnd((selection == "zmutau") ? 90 : (selection == "zetau") ? 91 : 92); //different seed for each selection, to not compare between plots
  const double offset = (blinding_offset_) ? 300.*(rnd.Uniform() - 0.5) : 0.; //offset the measurements by a fixed value if blinding
  const int nfiles = configs.size();
  double obs[nfiles], up[nfiles], down[nfiles], y[nfiles], yerr[nfiles];
  double max_val = -1.e9;
  double min_val =  1.e9;
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
    yerr[itree] = 0.2;
    max_val = max(max_val, obs[itree] + up  [itree]);
    min_val = min(min_val, obs[itree] - down[itree]);
    printf("%s: r = %.5e (+%.5e, -%.5e) [%.5e - %.5e]\n", configs[itree].name_.Data(),
           obs[itree], up[itree], down[itree],
           obs[itree] - down[itree], obs[itree]  + up[itree]);
  }

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
  const float xmax = max_val + 0.1*fabs(max_val);
  const float xmin = min_val - 0.1*fabs(min_val);
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
  gobs->Draw("PE");
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
  }

  gSystem->Exec("[ ! -d measurements ] && mkdir measurements");
  c->Print(Form("measurements/measurement_%s_%s.png", selection.Data(), tag.Data()));
  return 0;
}
