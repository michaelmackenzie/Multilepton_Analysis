//Test the fit envelope bias using individual MultiDimFit toys
#include "../tools/extract_grid_pull.C"

int test_envelope_bias(TString card, const int ntoys = 1, const double rgen = 0.,
                       const char* gen_arg = "", const char* fit_arg = "", const char* tag = "") {

  //Ensure the combine card/workspace exists
  if(gSystem->AccessPathName(card.Data())) {
    cout << __func__ <<": Input " << card.Data() << " not found\n";
    return 1;
  }

  //For simplicity, convert to a workspace if not already
  if(card.EndsWith(".txt")) {
    TString txt_file = card;
    card.ReplaceAll(".txt", "_workspace.root");
    cout << "Converting card " << txt_file.Data() << " to workspace " << card.Data() << endl;
    gSystem->Exec(Form("text2workspace.py %s -o %s", txt_file.Data(), card.Data()));
    //Check if the conversion succeeded
    if(gSystem->AccessPathName(card.Data())) {
      cout << __func__ <<": Output workspace " << card.Data() << " not found\n";
      return 1;
    }
  }

  TH1* hpulls = new TH1D("hpulls", "Fit pulls", 100, -5, 5);
  TH1* hr     = new TH1D("hr"    , "Fit results", 100, min(-10., -3*rgen), max(10., 3.*rgen));

  //Loop through toys, generating and fitting a single toy at a time
  const int base_seed = 100;
  const double rmin(-10.), rmax(10.);
  double r_fit;
  for(int itoy = 0; itoy < ntoys; ++itoy) {
    cout << "Fitting toy " << itoy << endl;

    //Generate a toy
    const int seed = base_seed + itoy;
    const TString gen_command = Form("combine -d %s -M GenerateOnly -t 1 --saveToys -m 125 -n .env_bias -s %i %s >| gen.log 2>&1", card.Data(), seed, gen_arg);
    if(itoy == 0) cout << gen_command.Data() << endl;
    gSystem->Exec(gen_command.Data());
    const char* gen_file = Form("higgsCombine.env_bias.GenerateOnly.mH125.%i.root", seed);
    //Check if the conversion succeeded
    if(gSystem->AccessPathName(gen_file)) {
      cout << __func__ <<": Generation failed for workspace " << card.Data() << endl;
      return 2;
    }

    //Fit the toy
    TString fit_command = Form("combine -d %s -M MultiDimFit --algo grid -t 1 --toysFile=%s -m 125 -n .env_bias -s %i %s", card.Data(), gen_file, seed, fit_arg);
    fit_command += " --saveNLL --cminDefaultMinimizerStrategy=0 --X-rtd REMOVE_CONSTANT_ZERO_POINT=1 --X-rtd MINIMIZER_freezeDisassociatedParams";
    fit_command += " --X-rtd MINIMIZER_multiMin_hideConstant --X-rtd MINIMIZER_multiMin_maskConstraints --saveToys";
    fit_command += Form(" --setParameterRanges r=%.1f,%.1f --points 100 >| fit.log 2>&1", rmin, rmax);
    if(itoy == 0) cout << fit_command.Data() << endl;
    gSystem->Exec(fit_command.Data());
    const char* fit_file = Form("higgsCombine.env_bias.MultiDimFit.mH125.%i.root", seed);
    //Check if the conversion succeeded
    if(gSystem->AccessPathName(fit_file)) {
      cout << __func__ <<": MultiDimFit failed for workspace " << card.Data() << endl;
      return 3;
    }

    //Extract the pull
    const double sigma = extract_grid_pull(fit_file, rgen, &r_fit);
    if(std::fabs(sigma) < 10.) { //no minimization error
      hpulls->Fill(sigma);
      hr->Fill(r_fit);
      cout << "Toy " << itoy << " has pull " << sigma << " for best fit r = " << r_fit << endl;
    } else {
      cout << "Toy " << itoy << " has pull " << sigma << " --> Minimization error!\n";
    }

    //Clean up
    gSystem->Exec(Form("rm %s; rm %s", gen_file, fit_file));
  }

  //Print the results
  card.ReplaceAll("_workspace.root", Form("_env_bias%s", tag));
  card.ReplaceAll(".root"          , Form("_env_bias%s", tag));

  TCanvas c;

  gStyle->SetOptFit(1110);
  TF1* fgaus = new TF1("fgaus", "[norm]*TMath::Gaus(x, [mean], [sigma], true)", -5, 5);
  fgaus->SetParameters(1., 1., 1.);
  hpulls->Fit(fgaus);
  hpulls->SetLineWidth(2);
  hpulls->Draw("hist");
  fgaus->Draw("same");
  c.SaveAs((card + "_pull.png").Data());

  hr->Draw("hist");
  c.SaveAs((card + "_r.png").Data());
  return 0;
}
