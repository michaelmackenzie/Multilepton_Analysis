//Perform chebychev and bernstein polynomial F-tests

Double_t chebychev_fit(RooDataHist& data, RooRealVar& obs, int order) {
  vector<RooRealVar*> vars;
  RooArgList list;
  map<int, vector<double>> params;
  params[0] = {1.};
  params[1] = {-1., 0.4};
  params[2] = {-1., 0.5, -0.1};
  params[3] = {-1., 0.5, -0.1, -0.05};
  params[4] = {-1., 0.5, -0.1, -0.05, 0.02};
  params[5] = {-1., 0.5, -0.1, -0.05, 0.02, 0.};
  params[6] = {-1., 0.5, -0.1, -0.05, 0.02, 0., 0.};
  if(order > 6) return 1e10;
  for(int i = 0; i <= order; ++i) {
    vars.push_back(new RooRealVar(Form("cheb_tmp_%i", i), Form("cheb_tmp_%i", i), params[order][i], -5., 5.));
    list.add(*vars[i]);
  }
  RooRealVar n_bkg("n_cheb_tmp", "n(bkg)", 1e5, 0., 1e6);
  RooChebychev func("cheb_tmp_pdf", "cheb_tmp_pdf", obs, list);
  RooAddPdf tot("cheb_tmp_tot_pdf", "totPDF_cheb_tmp", RooArgList(func), RooArgList(n_bkg));
  // if(blind_data_&&false)
  //   tot.fitTo(data, RooFit::Range("LowSideband,HighSideband"));
  // else
  tot.fitTo(data, RooFit::PrintLevel(-1), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1));
  auto xframe = obs.frame();
  data.plotOn(xframe);
  tot.plotOn(xframe, RooFit::Range("FullRange"));
  double chi_sq = xframe->chiSquare() * (data.numEntries() - order - 2); //DOF = number of variables + normalization
  delete xframe;
  for(auto var : vars) delete var;
  return chi_sq;
}

Double_t bernstein_fit(RooDataHist& data, RooRealVar& obs, int order) {
  vector<RooRealVar*> vars;
  RooArgList list;
  for(int i = 0; i <= order; ++i) {
    vars.push_back(new RooRealVar(Form("bst_tmp_%i", i), Form("bst_tmp_%i", i), (i == 0) ? 1. : 0., -5., 5.));
    list.add(*vars[i]);
  }
  RooBernstein func("bst_tmp_pdf", "bst_tmp_pdf", obs, list);
  // if(blind_data_&&false)
  //   func.fitTo(data, RooFit::Range("LowSideband,HighSideband"));
  // else
  func.fitTo(data, RooFit::PrintLevel(-1), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1));
  auto xframe = obs.frame();
  data.plotOn(xframe);
  func.plotOn(xframe);
  double chi_sq = xframe->chiSquare() * (data.numEntries() - order - 2); //DOF = number of variables + normalization
  delete xframe;
  for(auto var : vars) delete var;
  return chi_sq;
}

Int_t perform_f_test(RooDataHist& data, RooRealVar& obs, TString function, int verbose = 0) {
  gErrorIgnoreLevel = kInfo; //ignore excess info
  RooMsgService::instance().setStreamStatus(1,false);
  int order = -1;
  double delta_chi_end = 3.85; //Threshold to end F-test
  double last_chi_sq = 1000*data.sumEntries();
  double chi_sq = last_chi_sq;
  double chi_min = last_chi_sq;
  int order_min = order;
  do {
    ++order; //increment the order
    last_chi_sq = chi_sq; //save the previous result
    //find the next order goodness of fit
    if(function == "Bernstein")
      chi_sq = bernstein_fit(data, obs, order); //find the next order goodness of fit
    else if(function == "Chebychev")
      chi_sq = chebychev_fit(data, obs, order);
    else {
      cout << "!!! Warning! " << __func__ << ": Unknown fitting function " << function.Data() << endl;
      return -1;
    }
    //save the best order
    if(chi_sq < chi_min && last_chi_sq - chi_sq > delta_chi_end) {
      chi_min = chi_sq;
      order_min = order;
    }
    if(verbose > 0) cout << endl << "### " << __func__ << ": Order " << order << " has chi^2 = " << chi_sq << " (prev = " << last_chi_sq << ")\n\n";
  } while(order < 6 && (last_chi_sq - chi_sq > delta_chi_end  ||
                        chi_min / data.numEntries() > 2 || order < 2
                        )
          ); //break when improvement is small
  if(verbose > 0)
    cout << "### " << __func__ << ": Minimum order is " << order_min << " with chi^2 " << chi_min
         << " with " << data.numEntries() << " data entries" << endl << endl;
  return order_min;
}
