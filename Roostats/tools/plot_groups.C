//Plot uncertainty impacts from standard groups
#include "extract_impact.C"

int plot_groups(const char* file, const char* tag = "", const bool doObs = false, const bool run = true) {

  vector<TString> groups = {
                            "EmbedUnfold_Total",
                            "MuonID_Total"     ,
                            "EleID_Total"      ,
                            "EleTrig_Total"    ,
                            "MuonTrig_Total"   ,
                            "Prefire_Total"    ,
                            "ZPt_Total"        ,
                            "Lumi_Total"       ,
                            "BTag_Total"       ,
                            "Pileup_Total"     ,
                            "TauJetID_Total"   ,
                            "TauEleID_Total"   ,
                            "TauMuID_Total"    ,
                            "MuonES_Total"     ,
                            "EleES_Total"      ,
                            "TauES_Total"      ,
                            "TauMuES_Total"    ,
                            "TauEleES_Total"   ,
                            "JER_JES"          ,
                            "Theory_Total"     ,
                            "QCD_Stat"         ,
                            "QCD_NC"           ,
                            "QCD_Bias"         ,
                            "JetToTau_Stat"    ,
                            "JetToTau_NC"      ,
                            "JetToTau_Bias"    ,
                            "JetToTau_Comp"    ,
                            "autoMCStats"      ,
                            "All_Systematics"

  };

  if(run) {
    TString args = "-M FitDiagnostics --rMin -10 --rMax 10 --stepSize 0.05 --setRobustFitTolerance 0.001 --setCrossingTolerance 5e-6";
    args += " --cminDefaultMinimizerStrategy=0 --cminApproxPreFitTolerance 0.1 --cminPreScan --cminPreFit 1";
    gSystem->Exec(Form("combine %s -n _groupFit_Test%s_Nominal  -d %s %s", args.Data(), tag, file, (doObs) ? "" : "-t -1"));
    for(TString group : groups) {
      if(group == "All_Systematics") {
        cout << "Fitting without any systematics\n";
        gSystem->Exec(Form("combine %s -n _groupFit_Test%s_%s  -d %s %s --freezeParameters allConstrainedNuisances",
                           args.Data(), tag, group.Data(), file, (doObs) ? "" : "-t -1"));
      } else {
        cout << "Fitting without group " << group.Data() << endl;
        gSystem->Exec(Form("combine %s -n _groupFit_Test%s_%s  -d %s %s --freezeNuisanceGroups %s",
                           args.Data(), tag, group.Data(), file, (doObs) ? "" : "-t -1", group.Data()));
      }
    }
  }

  //Add a pure statistics uncertainty group
  groups.push_back("Statistical");


  const int n = groups.size();
  double rs[n+1], ups[n+1], downs[n+1];
  double ys[n+1], yup[n+1], ydown[n+1];
  ys   [n] = n+1; //total fit results
  ydown[n] = 0.1;
  yup  [n] = 0.1;
  fiteffect_t effect;
  double max_val(-1.e10), min_val(1.e10);
  for(int index = 0; index < n; ++index) {
    TString group = groups[index];
    cout << "Evaluating group " << group.Data() << endl;
    ys   [index] = index+1;
    ydown[index] = 0.1;
    yup  [index] = 0.1;

    if(group == "Statistical") {
      //Get information for nominal and all systematics impact
      const int nom_index = n;
      const int sys_index = n - 2; //FIXME: All_Systematics assumed second to last for now
      const double nom_up   = ups  [nom_index] - rs[nom_index];
      const double nom_down = downs[nom_index] - rs[nom_index];
      const double sys_up   = ups  [sys_index] - rs[sys_index];
      const double sys_down = downs[sys_index] - rs[sys_index];
      rs   [index] = rs[nom_index];
      ups  [index] = std::sqrt(max(0., std::pow(nom_up  ,2) - std::pow(sys_up  , 2)));
      downs[index] = std::sqrt(max(0., std::pow(nom_down,2) - std::pow(sys_down, 2)));
      printf("Estimating statistical as: %.4f -%.4f +%.4f\n", rs[index], ups[index], downs[index]);
    } else {
      int status = extract_impact(Form("fitDiagnostics_groupFit_Test%s_Nominal.root", tag), Form("fitDiagnostics_groupFit_Test%s_%s.root", tag, group.Data()), &effect);
      if(status) {
        rs   [index] = -999;
        ups  [index] = -999;
        downs[index] = -999;
        continue;
      }
      rs   [index] = effect.r_group_;
      ups  [index] = effect.impact_up_;
      downs[index] = effect.impact_down_;
      rs   [n]     = effect.r_nom_;
      ups  [n]     = effect.up_nom_;
      downs[n]     = effect.down_nom_;
    }
    max_val = max(max_val, rs[index]+ups[index]);
    max_val = max(max_val, rs[n]+ups[n]);
    min_val = min(min_val, rs[index]-downs[index]);
    min_val = min(min_val, rs[n]-downs[n]);
  }

  TGraph* g = new TGraphAsymmErrors(n+1, rs, ys, ups, downs, yup, ydown);
  TCanvas c("c", "c", 700, 1000);
  c.SetTopMargin(0.1); c.SetBottomMargin(0.1);
  g->SetLineWidth(2);
  g->SetLineColor(kBlue);
  g->SetFillColor(kBlue);
  g->SetTitle("Systematic uncertainties; #sigma_{r};");
  g->Draw("APE2");
  const double xbuffer = 0.1*(max_val-min_val);
  g->GetXaxis()->SetRangeUser(min_val-xbuffer,max_val+xbuffer);
  g->GetYaxis()->SetRangeUser(0, n+2);
  g->GetYaxis()->SetLabelSize(0); //remove number labels


  //Draw the category labels
  TLatex label;
  label.SetNDC();
  label.SetTextFont(72);
  label.SetTextColor(kBlack);
  label.SetTextSize(0.05);
  label.SetTextAlign(13);
  label.SetTextAngle(0);
  label.SetTextSize(0.03);
  for(int index = 0; index <= n; ++index) {
    const double ystart = 0.1;
    const double yend   = 0.85;
    const double yloc = ystart + (yend-ystart)*(index+1.)/n;
    if(index < n) {
      // cout << "Group " << groups[index].Data() << " y location: " << yloc << endl;
      label.DrawLatex(0.01, yloc, Form("%s", groups[index].Data()));
    } else {
      label.DrawLatex(0.01, yloc, "Total");
    }
  }
  c.SetGridx();
  c.Modified(); c.Update();

  c.SaveAs(Form("groups%s.png", tag));
  return 0;
}
