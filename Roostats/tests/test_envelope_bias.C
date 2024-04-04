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
  TH1* hindex = new TH1D("hindex", "Index", 10, 0, 10);

  int category = 11;
  if     (card.Contains("_10_")) category = 10;
  else if(card.Contains("_11_")) category = 11;
  else if(card.Contains("_12_")) category = 12;
  else if(card.Contains("_13_")) category = 13;

  //Loop through toys, generating and fitting a single toy at a time
  const int base_seed = 100;
  const double rmin(-10.), rmax(10.);
  double r_fit;
  for(int itoy = 0; itoy < ntoys; ++itoy) {
    cout << "Fitting toy " << itoy << endl;

    const int seed = base_seed + itoy;

    const bool use_generate_only = true;
    const bool clean_up = true;

    if(use_generate_only) {
      //Generate a toy
      //George's command:
      const TString gen_command = Form("combine -M GenerateOnly -d %s -t 1 --saveToys -m 125 -n .env_bias -s %i %s >| gen.log 2>&1", card.Data(), seed, gen_arg);
      if(itoy == 0) cout << gen_command.Data() << endl;
      gSystem->Exec(gen_command.Data());
      const char* gen_file = Form("higgsCombine.env_bias.GenerateOnly.mH125.%i.root", seed);
      //Check if the conversion succeeded
      if(gSystem->AccessPathName(gen_file)) {
        cout << __func__ <<": Generation failed for workspace " << card.Data() << endl;
        return 2;
      }
    }

    //Fit the toy
    //George's command:
    TString fit_command = Form("combine -M MultiDimFit -d %s --algo grid --points 100 -t 1 -m 125 -n .env_bias -s %i %s", card.Data(), seed, fit_arg);
    if(use_generate_only) {
      fit_command += Form(" --toysFile=higgsCombine.env_bias.GenerateOnly.mH125.%i.root", seed);
    } else {
      fit_command += Form(" %s", gen_arg);
    }
    fit_command += " --saveNLL --cminDefaultMinimizerStrategy 0";
    fit_command += " --X-rtd REMOVE_CONSTANT_ZERO_POINT=1";
    fit_command += " --X-rtd MINIMIZER_multiMin_hideConstant --X-rtd MINIMIZER_multiMin_maskConstraints --X-rtd MINIMIZER_freezeDisassociatedParams";
    fit_command += Form(" --rMin %.1f --rMax %.1f --saveSpecifiedIndex cat_%i >| fit.log 2>&1", rmin, rmax, category);
    // fit_command += Form(" --setParameterRanges r=%.1f,%.1f --points 100 >| fit.log 2>&1", rmin, rmax);
    if(itoy == 0) cout << fit_command.Data() << endl;
    gSystem->Exec(fit_command.Data());
    const char* fit_file = Form("higgsCombine.env_bias.MultiDimFit.mH125.%i.root", seed);
    //Check if the conversion succeeded
    if(gSystem->AccessPathName(fit_file)) {
      cout << __func__ <<": MultiDimFit failed for workspace " << card.Data() << endl;
      return 3;
    }


      //Get the best fit index
    TFile* f = TFile::Open(fit_file, "READ");
    if(!f) continue;
    TTree* tree = (TTree*) f->Get("limit");
    if(!tree) continue;
    int fit_index(9);
    tree->SetBranchAddress(Form("cat_%i", category), &fit_index);
    tree->GetEntry(0);

    //Extract the pull
    const double sigma = extract_grid_pull(fit_file, rgen, &r_fit);
    if(std::fabs(sigma) < 10.) { //no minimization error
      hpulls->Fill(sigma);
      hr->Fill(r_fit);
      hindex->Fill(fit_index);
    } else {
      cout << "Toy " << itoy << " has pull " << sigma << " --> Minimization error!\n";
    }
    cout << "Toy " << itoy << " has pull " << sigma << " for best fit r = " << r_fit << " and pdf index " << fit_index << endl;

    //Clean up
    if(clean_up) {
      gSystem->Exec(Form("rm %s", fit_file));
      if(use_generate_only) {
        gSystem->Exec(Form("rm higgsCombine.env_bias.GenerateOnly.mH125.%i.root", seed));
      }
    }
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

  hindex->Draw("hist");
  c.SaveAs((card + "_index.png").Data());
  return 0;
}
