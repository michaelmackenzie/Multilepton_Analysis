// Script to find b-tag scale factors as a funtion of pT and eta

namespace {
  vector<TFile*> files_;
  vector<TTree*> trees_;
  vector<TString> names_;
  vector<TString> titles_;
  struct Eff_t {
    float bin;
    float res;
    float err;
  };
}

Int_t init_files() {
  const char* gridpath = "/eos/uscms/store/user/mmackenz/batch_output/jet_study_2016_20200414_165135/";
  const char* files[] = {"output_ttbar_inclusive.root",
			 "output_DYJetsToLL_M-50_amcatnlo.root",
			 "output_DYJetsToLL_M-10to50_amcatnlo.root"};
  const char* dsetnames[] = {"ttbar_inclusive",
			     "zjets_m-50_amcatnlo",
			     "zjets_m-10to50_amcatnlo"};
  const char* dsettitles[] = {"t#bar{t}",
			      "Z+Jets (M>50)",
			      "Z+Jets (10<M<50)"};
  const bool process[] = {true,
			  true,
			  true};
  Int_t status = 0;
  for(unsigned index = 0; index < 3; ++index) {
    if(!process[index]) {continue;} //add NULL to each in the vector to preserve indexing?
    files_.push_back(TFile::Open(Form("%s%s", gridpath, files[index]), "READ"));
    names_.push_back(dsetnames[index]); //always push back
    titles_.push_back(dsettitles[index]);
    if(!files_[index]) {printf("File %s not found!\n", files[index]);
      trees_.push_back(NULL); ; ++status; continue;}
    
    trees_.push_back((TTree*) files_[index]->Get(Form("jets/bltTree_%s", dsetnames[index])));
    if(!trees_[index]) {printf("Tree jets/bltTree_%s not found!\n", dsetnames[index]); ++status;}
  }
  gStyle->SetOptStat(0);
  return status;
}

TCanvas* plot_var(TString var, TCut cut = "") {
  TCanvas* c = new TCanvas();
  bool first = true;
  TH1F* hAxis = 0;
  Int_t colors[] = {kRed-7, kRed-3, kGreen-7 , kViolet+6, kCyan-7, kRed+3,kOrange-9,kBlue+1, kYellow-7};
  Double_t m = -1.;
  for(unsigned index = 0; index < trees_.size(); ++index) {
    TTree* tree = trees_[index];
    const char* hname = Form("h_%s", names_[index].Data());
    if(first) {tree->Draw(Form("%s>>%s", var.Data(),hname), cut, "hist");}
    else tree->Draw(Form("%s>>%s", var.Data(),hname), cut, "hist sames");
    TH1F* h = (TH1F*) gDirectory->Get(hname);
    h->SetLineColor(colors[index]);
    h->Scale(1./h->Integral());
    m = max(h->GetMaximum(), m);
    h->SetTitle(titles_[index].Data());
    h->SetLineWidth(2);
    if(first){hAxis = h; first = false;}
  }
  if(hAxis) {
    c->BuildLegend();
    hAxis->SetTitle("");
    hAxis->SetXTitle(var.Data());
    hAxis->SetAxisRange(m/1.e4, m*1.2, "Y");
  }
  return c;
}

//plot a single tree, but separating the different flavors
TCanvas* plot_single_tree(TString var, unsigned index, vector<TString> cuts = {""},
			  vector<TString> labels = {}) {
  if(trees_.size() == 0) {
    cout << "Files not initialized!\n";
    return NULL;
  }
  TCanvas* c = new TCanvas();
  bool first = true;
  TH1F* hAxis = 0;
  Int_t colors[] = {kRed-7, kRed-3, kGreen-7 , kViolet+6, kCyan-7, kRed+3,kOrange-9,kBlue+1, kYellow-7};
  Double_t m = -1.;
  TTree* tree = trees_[index];
  TLegend* leg = new TLegend(0.2, 0.9, 0.8, 0.8);
  leg->SetNColumns(3);
  leg->SetLineWidth(0);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  for(unsigned cut_index = 0; cut_index < cuts.size(); ++cut_index) {
    const char* hname = (labels.size() == 0) ? Form("h_%i", cut_index) : Form("h_%s", labels[cut_index].Data());
    cout << "Drawing hist " << hname << endl;
    if(first) {tree->Draw(Form("%s>>%s", var.Data(),hname), cuts[cut_index].Data(), "hist");}
    else tree->Draw(Form("%s>>%s", var.Data(),hname), cuts[cut_index].Data(), "hist same");
    TH1F* h = (TH1F*) gDirectory->Get(hname);
    h->SetLineColor(colors[cut_index]);
    h->Scale(1./h->GetBinWidth(1)/h->Integral());
    m = max(h->GetMaximum(), m);
    h->SetTitle((labels.size() == 0) ? cuts[cut_index].Data() : labels[cut_index].Data());
    h->SetLineWidth(2);
    leg->AddEntry(h);
    if(first){hAxis = h; first = false;}
  }
  if(hAxis) {
    leg->Draw();
    hAxis->SetTitle(titles_[index].Data());
    hAxis->SetXTitle(var.Data());
    hAxis->SetAxisRange(m/1.e4, m*1.2, "Y");
  }
  c->SetGrid();
  return c;
}

//plot a single tree b-tag efficiency
/**
   Parameters:
   var: variable the efficiency is a function of
   index: index of the dataset in the field vectors
   mvacut: cut value for tagging a b-jet
   bins: vector of the binning of the var
   id_cut: identifying the relevant selection (b jets, nJets, etc)
   label: label for this plot
   debug: print extra information for debugging
 **/
TCanvas* plot_btag_eff(TString var, unsigned index, float mva_cut, vector<float> bins,
		       TString id_cut, TString label, bool debug = false) {
  if(trees_.size() == 0) {
    cout << "Files not initialized!\n";
    return NULL;
  }
  TCanvas* c = new TCanvas();
  float prev_bin = -1.e6;
  double effs[bins.size()];
  double xbins[bins.size()];
  double effs_err[bins.size()];
  double xbins_err[bins.size()];
  unsigned i = 0;
  TTree* tree = trees_[index];
  double m = -1.;
  // auto cachesize = 500000000U; //100MB
  // tree->SetCacheSize(cachesize);
  // tree->AddBranchToCache("*",true);
  // tree->LoadBaskets();

  for(float bin : bins) {
    TString cut = "eventWeight*(";
    if(id_cut != "") cut += id_cut + "&&";
    if(prev_bin > -1e5) {cut += var + ">="; cut += prev_bin; cut += "&&";}
    cut += var; cut += "<"; cut+= bin;
    TString b_id_cut = cut;
    cut += ")";
    b_id_cut += "&&jetsbMVA>"; b_id_cut += mva_cut;
    b_id_cut += ")";
    if(debug)
      cout << "bin: " << bin << endl
	   << "denom cut: " << cut.Data() << endl
	   << "numer cut: " << b_id_cut.Data() << endl;

    //set some info before evaluating in case of continue
    xbins[i] = (prev_bin < -1e5) ? (bin)/2. : (bin + prev_bin)/2.;
    xbins_err[i] = (prev_bin < -1e5) ? (bin)/2. : (bin - prev_bin)/2.;
    effs[i] = -1.;
    effs_err[i] = -1.;
    prev_bin = bin;
    
    //get denominator
    tree->Draw(Form("%s>>hist", var.Data()), cut.Data());
    TH1F* h = (TH1F*) gDirectory->Get("hist");
    if(!h) {printf("Histogram not found!\n"); continue;}
    double error_d = -1.;
    double denom = h->IntegralAndError(1, h->GetNbinsX()+1, error_d);
    delete h;
    if(debug)
      cout << "Denominator = " << denom << endl;

    //get numerator
    tree->Draw(Form("%s>>hist", var.Data()), b_id_cut.Data());
    h = (TH1F*) gDirectory->Get("hist");
    if(!h) {printf("Histogram not found!\n"); continue;}
    double error_n = -1.;
    double numer = h->IntegralAndError(1, h->GetNbinsX()+1, error_n);
    delete h;
    if(debug)
      cout << "Numerator = " << numer << endl
	   << "Ratio = " << numer/denom << endl;

    effs[i] = numer/denom;
    m = max(effs[i], m);
    effs_err[i] = effs[i]*sqrt(error_d*error_d/denom/denom + error_n*error_n/numer/numer);
    ++i;
  }
  TGraphErrors* g = new TGraphErrors(bins.size(), xbins, effs, xbins_err, effs_err);
  g->SetTitle(Form("%s;%s;efficiency", label.Data(), var.Data()));
  g->SetMarkerStyle(20);
  g->SetLineWidth(2);
  g->SetLineColor(kAzure);
  g->Draw("AP");
  g->GetYaxis()->SetRangeUser(m/1.e3, m*1.2);
  // g->GetXaxis()->SetRangeUser(, m*1.2);
  c->SetGrid();
  cout << "Final efficiency results: (bin upper edges, eff, err)" << endl << "Bins = {";
  for(unsigned i = 0; i < g->GetN(); ++i)
    cout << bins[i] << ", " ;
  cout << endl << "};\n results = " << endl;
  for(unsigned i = 0; i < g->GetN(); ++i)
    cout << g->GetY()[i] << ", ";
  cout << endl << "};\n errors = " << endl;
  for(unsigned i = 0; i < g->GetN(); ++i)
    cout << effs_err[i] << ", ";
  cout << "};" << endl;
    
  return c;
}

//get pre-evaluated efficiencies
vector<Eff_t> get_efficiencies(unsigned index, TString mvaname,
					   TString category, TString var) {
  vector<Eff_t> effs;
  if(var == "jetsPt") {
    float ptbins[] = {50.f, 70.f, 100.f, 140.f, 200.f, 300.f, 600.f};
    vector<float> results;
    vector<float> errors;
    if(index == 0) { //ttbar
      if(mvaname == "loose") {
	if(category == "b-jet") {
	  results = {0.836311, 0.864787,  0.879155, 0.884378, 0.88678, 0.8781, 0.865072};
	  errors = {0.000367552, 0.000396685, 0.000400064, 0.000508027, 0.000747139, 0.00134093, 0.00289472};
	} else if(category == "c-jet") {
	  results = {0.396046, 0.416373, 0.433649, 0.442893, 0.462809, 0.47551, 0.512201};
	  errors = {0.000444464, 0.00054264, 0.000588442, 0.000765667, 0.00110557, 0.00182248, 0.00314552};
	} else if(category == "udsg-jet") {
	  results = {0.097719, 0.0937078, 0.0998718, 0.109096, 0.135816, 0.165499, 0.213616}; 
	  errors = {7.86024e-05, 0.000104879, 0.000122772, 0.000169662, 0.00026146, 0.000432395, 0.00078046};
	}
      } else if(mvaname == "medium") {
	if(category == "b-jet") {
	  results = {0.645459, 0.689443, 0.708177, 0.711061, 0.701379, 0.668631, 0.623636};
	  errors = {0.000305688, 0.000337155, 0.000342362, 0.000434116, 0.000631023, 0.00110297, 0.00229358};
	} else if(category == "c-jet") {
	  results = {0.127154, 0.136049, 0.143137, 0.146914, 0.150344, 0.149071, 0.157098};
	  errors = {0.000226284, 0.000277756, 0.000301812, 0.00039309, 0.00055858, 0.00090053, 0.00152436};
	} else if(category == "udsg-jet") {
	  results = {0.011332, 0.0101566, 0.010858, 0.0120828, 0.0151716, 0.0191084, 0.0264264}; 
	  errors = {2.56603e-05, 3.3122e-05, 3.87154e-05, 5.37866e-05, 8.2321e-05, 0.000136858, 0.000251304};
	}
      } else if(mvaname == "tight") {
	if(category == "b-jet") {
	  results = {0.44209, 0.496234, 0.512855, 0.512881, 0.490063, 0.43645, 0.355234};
	  errors = {0.000236864, 0.000269226, 0.000274237, 0.00034676, 0.000493761, 0.000826962, 0.0015826};
	} else if(category == "c-jet") {
	  results = {0.0245871, 0.0281465, 0.0302593, 0.0324758, 0.0328596, 0.031942, 0.032537};
	  errors = {9.48395e-05, 0.000120196, 0.000131695, 0.000175338, 0.000247403, 0.000395363, 0.000656914};
	} else if(category == "udsg-jet") {
	  results = {0.00174914, 0.0013178, 0.001262, 0.00144956, 0.00170886, 0.00205872, 0.00327515}; 
	  errors = {1.00406e-05, 1.18812e-05, 1.31218e-05, 1.8501e-05, 2.73388e-05, 4.43854e-05, 8.72765e-05};
	}
      }
    } else if(index == 1) { //zjets_m-50
      if(mvaname == "loose") {
	if(category == "b-jet") {
	  results = {0.804883, 0.834316, 0.860931, 0.870292, 0.869345, 0.863288, 0.845355};
	  errors = {0.00169432, 0.00256564, 0.00332894, 0.00493349, 0.007332, 0.0119603, 0.0218144};
	} else if(category == "c-jet") {
	  results = {0.362259, 0.381641, 0.415776, 0.43066, 0.451909, 0.462033, 0.497665};
	  errors = {0.000761032, 0.00115492, 0.00156067, 0.00231332, 0.00347503, 0.00562532, 0.0103616};
	} else if(category == "udsg-jet") {
	  results = {0.0906212, 0.0931565, 0.102946, 0.107694, 0.127825, 0.148592, 0.190666};
	  errors = {9.18437e-05, 0.000140032, 0.000213912, 0.00032872, 0.000525798, 0.000890677, 0.00179027};
	}
      } else if(mvaname == "medium") {
	if(category == "b-jet") {
	  results = {0.610924, 0.659014, 0.693501, 0.703945, 0.68631, 0.664821, 0.615822};
	  errors = {0.0013946, 0.00216843, 0.0028502, 0.00423567, 0.00618824, 0.0099222, 0.0174304};
	} else if(category == "c-jet") {
	  results = {0.112448, 0.123346, 0.138767, 0.144652, 0.147483, 0.146666, 0.158628};
	  errors = {0.000383082, 0.000592016, 0.000808437, 0.00119944, 0.00176442, 0.00280432, 0.00515354};
	} else if(category == "udsg-jet") {
	  results = {0.011132, 0.0123947, 0.0131651, 0.0130452, 0.0152617, 0.0176056, 0.0243839};
	  errors = {3.09948e-05, 4.91302e-05, 7.31917e-05, 0.000109192, 0.000172006, 0.000287765, 0.000592239};
	}
      } else if(mvaname == "tight") {
	if(category == "b-jet") {
	  results = {0.417383, 0.471948, 0.501802, 0.510188, 0.47944, 0.437995, 0.366898};
	  errors = {0.00108132, 0.00172862, 0.00228335, 0.0033955, 0.00484586, 0.00748501, 0.01237};
	} else if(category == "c-jet") {
	  results = {0.0215407, 0.0255216, 0.0289561, 0.0315761, 0.0343303, 0.0330352, 0.0299039};
	  errors = {0.000160697, 0.000257236, 0.00035089, 0.000531764, 0.000808234, 0.00126477, 0.00210292};
	} else if(category == "udsg-jet") {
	  results = {0.00108924, 0.0012674, 0.00134701, 0.0013362, 0.00171072, 0.00212744, 0.00302041};
	  errors = {9.63978e-06, 1.55965e-05, 2.32557e-05, 3.46835e-05, 5.70867e-05, 9.87992e-05, 0.000206945};
	}
      }
    } else if(index == 2) { //zjets_m-10to50
      if(mvaname == "loose") {
	if(category == "b-jet") {
	  results = {0.813728, 0.847539, 0.853988, 0.85578, 0.85353, 0.813148, 0.859923};
	  errors = {0.00961195, 0.01535, 0.0206611, 0.0319972, 0.0501526, 0.08054, 0.16593};
	} else if(category == "c-jet") {
	  results = {0.387195, 0.405599, 0.428143, 0.440702, 0.460628, 0.472792, 0.520176};
	  errors = {0.00254248, 0.00407459, 0.00579162, 0.00942489, 0.0154477, 0.02809, 0.057405};
	} else if(category == "udsg-jet") {
	  results = {0.0876487, 0.0832056, 0.0927173, 0.102704, 0.129389, 0.158795, 0.186992};
	  errors = {0.000478625, 0.000739186, 0.00103305, 0.0016461, 0.00285098, 0.00532989, 0.0109067};
	}
      } else if(mvaname == "medium") {
	if(category == "b-jet") {
	  results = {0.619169, 0.675511, 0.694333, 0.690736, 0.686635, 0.64882, 0.686042};
	  errors = {0.00792314, 0.0130524, 0.017813, 0.0274347, 0.0429672, 0.0686786, 0.140961};
	} else if(category == "c-jet") {
	  results = {0.120265, 0.128785, 0.140022, 0.149205, 0.144601, 0.161946, 0.164287};
	  errors = {0.00127392, 0.00205829, 0.00295635, 0.00489598, 0.00765382, 0.0145612, 0.0281267};
	} else if(category == "udsg-jet") {
	  results = {0.00988638, 0.00841069, 0.00975739, 0.0112196, 0.0141448, 0.0182396, 0.0210951};
	  errors = {0.000154598, 0.00022627, 0.000320833, 0.000518171, 0.000889613, 0.00169788, 0.00338571};
	}
      } else if(mvaname == "tight") {
	if(category == "b-jet") {
	  results = {0.429812, 0.497909, 0.506488, 0.51641, 0.487687, 0.398935, 0.531996};
	  errors = {0.00620437, 0.0105989, 0.0143393, 0.0224613, 0.0339774, 0.0495434, 0.118259};
	} else if(category == "c-jet") {
	  results = {0.0231096, 0.0263022, 0.0280759, 0.0375554, 0.0326714, 0.0371737, 0.0373};
	  errors = {0.000533926, 0.000886999, 0.00125849, 0.0023417, 0.00344459, 0.00654918, 0.0126748};
	} else if(category == "udsg-jet") {
	  results = {0.000971361, 0.000796317, 0.00104941, 0.00142268, 0.00138753, 0.00117723, 0.00165514};
	  errors = {4.8313e-05, 6.93968e-05, 0.000104871, 0.000184607, 0.000275672, 0.000420964, 0.00095675};
	}
      }
    } //end index 2
    for(unsigned i = 0; i < results.size(); ++i) {
      Eff_t eff;
      eff.bin = ptbins[i]; eff.res = results[i]; eff.err = errors[i];
      effs.push_back(eff);
    }
  } //end jetsPt
  return effs;
}

// get a graph of the efficiencies for the given dataset, tightness, and true gen flavor
TGraphErrors* get_effs(unsigned index, TString mvaname, TString category, TString var = "jetsPt") {
  auto effs = get_efficiencies(index, mvaname, category, var);
  double xs[effs.size()];
  double ys[effs.size()];
  double xs_errs[effs.size()];
  double ys_errs[effs.size()];
  double prev_bin = -1e6;
  double m = -1.;
  for(unsigned i = 0; i < effs.size(); ++i) {
    auto eff = effs[i];
    xs[i] = eff.bin;
    ys[i] = eff.res;
    xs_errs[i] = (prev_bin < -1.e5) ? xs[i]/2. : -(prev_bin - xs[i])/2.;
    xs[i] = (prev_bin < -1.e5) ? xs[i]/2. : (prev_bin + xs[i])/2.;
    prev_bin = eff.bin;
    ys_errs[i] = eff.err;
    m = max(m, ys[i]);
  }
  TGraphErrors* g = new TGraphErrors(effs.size(), xs, ys, xs_errs, ys_errs);
  g->SetLineWidth(2);
  g->SetMarkerStyle(20);
  g->SetLineColor(kBlue);
  g->SetMaximum(m);
  return g;
}

// plot a graph of the efficiencies for the given dataset, tightness, and true gen flavor
TCanvas* plot_effs(unsigned index, TString mvaname, TString category, TString var = "jetsPt") {
  TCanvas* c = new TCanvas();
  TGraphErrors* g = get_effs(index, mvaname, category, var);
  g->Draw("AP");
  double m = g->GetMaximum();
  g->GetYaxis()->SetRangeUser(m/1.e4, m*1.2);
  c->SetGrid();
  return c;
}

// plot graphs of the efficiencies all dataset, given a tightness and true gen flavor
TCanvas* compare_effs(TString mvaname, TString category, TString var = "jetsPt") {
  if(trees_.size() == 0) {
    cout << "Initializing the files\n";
    init_files();
  }
    
  TCanvas* c = new TCanvas();
  Int_t colors[] = {kRed-3, kGreen-7, kViolet+6, kCyan-7, kRed+3,kOrange-9,kBlue+1, kRed-7, kYellow-7};
  bool first = true;
  TGraphErrors* gAxis = 0;
  
  TLegend* leg = new TLegend(0.2, 0.9, 0.8, 0.8);
  leg->SetNColumns(3);
  leg->SetLineWidth(0);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);

  double m = -1;
  for(unsigned index = 0; index < trees_.size(); ++index) {
    TGraphErrors* g =get_effs(index, mvaname, category, var);
    g->SetName(("g_"+names_[index]).Data());
    g->SetLineColor(colors[index]);
    if(first) {
      g->Draw("AP");
      gAxis = g;
      first = false;
    } else
      g->Draw("P");
    m = max(m,g->GetMaximum());
    leg->AddEntry(g,titles_[index].Data(),"L");
  }
  if(m > 0.)
    gAxis->GetYaxis()->SetRangeUser(m/1.e4, m*1.3);
  else
    gAxis->GetYaxis()->SetRangeUser(1.e-5, 1.);
  leg->Draw();
  gAxis->SetTitle((("True " + mvaname) + (" " + category) + (" efficiencies;" + var) + ";efficiency").Data());
  c->SetGrid();
  return c;
}

Int_t print_comparisons(TString var = "jetsPt") {
  vector<TString> mvanames = {"loose", "medium", "tight"};
  vector<TString> categories = {"b-jet", "c-jet", "udsg-jet"};
  Int_t status = 0;
  for(TString mva : mvanames) {
    for(TString cat : categories) {
      auto c = compare_effs(mva, cat, var);
      if(c) {
	c->Print(("figures/compare_"+mva+"_"+cat+"_efficiencies.png").Data());
	delete c;
      } else
	++status;
    }
  }
  return status;
}

Int_t print_standard_plots() {
  Int_t status = 0;
  if(trees_.size() == 0) init_files();
  bool process[] = {true, false, false};
  vector<float> ptbins = {50.f, 70.f, 100.f, 140.f, 200.f, 300.f, 600.f};
  vector<float> mvaids = {-0.5884f, 0.4432f, 0.9432f};
  vector<TString> mvanames = {"loose", "medium", "tight"};
  vector<TString> categories = {"b-jet", "c-jet", "udsg-jet"};
  vector<bool> doCat = {true, true, true};
  vector<TString> cat_ids = {"abs(jetsGenFlavor)==5",
			     "abs(jetsGenFlavor)==4",
			     "abs(abs(jetsGenFlavor) - 4.5)>1"};
  for(unsigned index = 0; index < sizeof(process)/sizeof(*process); ++ index) {
    TCanvas* c = 0;
    // c = plot_single_tree("jetsbMVA",
    // 			 index,
    // 			 {"eventWeight*(abs(jetsGenFlavor) == 5)",
    // 			     "eventWeight*(abs(jetsGenFlavor) == 4)",
    // 			     "eventWeight*(abs(abs(jetsGenFlavor) - 4.5) > 1)"},
    // 			 {"b", "c", "udsg"});
    // if(c) {
    //   c->Print(Form("figures/%s_bmva_flavors.png", names_[index].Data()));
    //   c->SetLogy();
    //   c->Print(Form("figures/%s_bmva_flavors_log.png", names_[index].Data()));
    // } else
    //   ++status;
    for(unsigned cat = 0; cat < categories.size(); ++cat) {
      if(!doCat[cat]) continue;
      for(unsigned mva = 0; mva < mvaids.size(); ++mva) {
	cout << "Getting efficiencies for " << mvanames[mva].Data()
	     << " ID for true " << categories[cat].Data()
	     << " on the " << names_[index].Data() << " sample" << endl;
	c = plot_btag_eff("jetsPt", index, mvaids[mva],
			  ptbins, cat_ids[cat],
			  ((titles_[index]+" ") + (mvanames[mva] + " " + categories[cat]))
			  ,false);
	if(c) {
	  c->Print(Form("figures/%s_%s_%s_eff.png",
			names_[index].Data(), mvanames[mva].Data(),
			categories[cat].Data()));
	} else
	  ++status;
      }
    }
  }
  return status;
}

// Getting efficiencies for loose ID for true c-jet on the ttbar_inclusive sample
// Final efficiency results: (bin upper edge, eff)
// (50, 0.396046), (70, 0.416373), (100, 0.433649), (140, 0.442893), (200, 0.462809), (300, 0.47551), (6
// 00, 0.512201), 
// Info in <TCanvas::Print>: png file figures/ttbar_inclusive_loose_c-jet_eff.png has been created
// Getting efficiencies for medium ID for true c-jet on the ttbar_inclusive sample
// Final efficiency results: (bin upper edge, eff)
// (50, 0.127154), (70, 0.136049), (100, 0.143137), (140, 0.146914), (200, 0.150344), (300, 0.149071), (
// 600, 0.157098), 
// Info in <TCanvas::Print>: png file figures/ttbar_inclusive_medium_c-jet_eff.png has been created
// Getting efficiencies for tight ID for true c-jet on the ttbar_inclusive sample
// Final efficiency results: (bin upper edge, eff)
// (50, 0.0245871), (70, 0.0281465), (100, 0.0302593), (140, 0.0324758), (200, 0.0328596), (300, 0.03194
// 2), (600, 0.032537), 
// Info in <TCanvas::Print>: png file figures/ttbar_inclusive_tight_c-jet_eff.png has been created
// Getting efficiencies for loose ID for true udsg-jet on the ttbar_inclusive sample
// Final efficiency results: (bin upper edge, eff)
// (50, 0.097719), (70, 0.0937078), (100, 0.0998718), (140, 0.109096), (200, 0.135816), (300, 0.165499),
//  (600, 0.213616), 
// Info in <TCanvas::Print>: png file figures/ttbar_inclusive_loose_udsg-jet_eff.png has been created
// Getting efficiencies for medium ID for true udsg-jet on the ttbar_inclusive sample
// Final efficiency results: (bin upper edge, eff)
// (50, 0.011332), (70, 0.0101566), (100, 0.010858), (140, 0.0120828), (200, 0.0151716), (300, 0.0191084
// ), (600, 0.0264264), 
// Info in <TCanvas::Print>: png file figures/ttbar_inclusive_medium_udsg-jet_eff.png has been created
// Getting efficiencies for tight ID for true udsg-jet on the ttbar_inclusive sample
// Final efficiency results: (bin upper edge, eff)
// (50, 0.00174914), (70, 0.0013178), (100, 0.001262), (140, 0.00144956), (200, 0.00170886), (300, 0.002
// 05872), (600, 0.00327515), 
// Info in <TCanvas::Print>: png file figures/ttbar_inclusive_tight_udsg-jet_eff.png has been created
// Getting efficiencies for loose ID for true c-jet on the zjets_m-50_amcatnlo sample
// Final efficiency results: (bin upper edge, eff)
// (50, 0.362259), (70, 0.381641), (100, 0.415776), (140, 0.43066), (200, 0.451909), (300, 0.462033), (6
// 00, 0.497665), 
// Info in <TCanvas::Print>: png file figures/zjets_m-50_amcatnlo_loose_c-jet_eff.png has been created
// Getting efficiencies for medium ID for true c-jet on the zjets_m-50_amcatnlo sample
// Final efficiency results: (bin upper edge, eff)
// (50, 0.112448), (70, 0.123346), (100, 0.138767), (140, 0.144652), (200, 0.147483), (300, 0.146666), (
// 600, 0.158628), 
// Info in <TCanvas::Print>: png file figures/zjets_m-50_amcatnlo_medium_c-jet_eff.png has been created
// Getting efficiencies for tight ID for true c-jet on the zjets_m-50_amcatnlo sample
// Final efficiency results: (bin upper edge, eff)
// (50, 0.0215407), (70, 0.0255216), (100, 0.0289561), (140, 0.0315761), (200, 0.0343303), (300, 0.03303
// 52), (600, 0.0299039), 
// Info in <TCanvas::Print>: png file figures/zjets_m-50_amcatnlo_tight_c-jet_eff.png has been created
// Getting efficiencies for loose ID for true udsg-jet on the zjets_m-50_amcatnlo sample
// Final efficiency results: (bin upper edge, eff)
// (50, 0.0906212), (70, 0.0931565), (100, 0.102946), (140, 0.107694), (200, 0.127825), (300, 0.148592), (600, 0.190666), 
// Info in <TCanvas::Print>: png file figures/zjets_m-50_amcatnlo_loose_udsg-jet_eff.png has been created
// Getting efficiencies for medium ID for true udsg-jet on the zjets_m-50_amcatnlo sample
// Final efficiency results: (bin upper edge, eff)
// (50, 0.011132), (70, 0.0123947), (100, 0.0131651), (140, 0.0130452), (200, 0.0152617), (300, 0.0176056), (600, 0.0243839), 
// Info in <TCanvas::Print>: png file figures/zjets_m-50_amcatnlo_medium_udsg-jet_eff.png has been created
// Getting efficiencies for tight ID for true udsg-jet on the zjets_m-50_amcatnlo sample
// Final efficiency results: (bin upper edge, eff)
// (50, 0.00108924), (70, 0.0012674), (100, 0.00134701), (140, 0.0013362), (200, 0.00171072), (300, 0.00212744), (600, 0.00302041), 
// Info in <TCanvas::Print>: png file figures/zjets_m-50_amcatnlo_tight_udsg-jet_eff.png has been created
// Getting efficiencies for loose ID for true c-jet on the zjets_m-10to50_amcatnlo sample
// Final efficiency results: (bin upper edge, eff)
// (50, 0.387195), (70, 0.405599), (100, 0.428143), (140, 0.440702), (200, 0.460628), (300, 0.472792), (600, 0.520176), 
// Info in <TCanvas::Print>: png file figures/zjets_m-10to50_amcatnlo_loose_c-jet_eff.png has been created
// Getting efficiencies for medium ID for true c-jet on the zjets_m-10to50_amcatnlo sample
// Final efficiency results: (bin upper edge, eff)
// (50, 0.120265), (70, 0.128785), (100, 0.140022), (140, 0.149205), (200, 0.144601), (300, 0.161946), (600, 0.164287), 
// Info in <TCanvas::Print>: png file figures/zjets_m-10to50_amcatnlo_medium_c-jet_eff.png has been created
// Getting efficiencies for tight ID for true c-jet on the zjets_m-10to50_amcatnlo sample
// Final efficiency results: (bin upper edge, eff)
// (50, 0.0231096), (70, 0.0263022), (100, 0.0280759), (140, 0.0375554), (200, 0.0326714), (300, 0.0371737), (600, 0.0373), 
// Info in <TCanvas::Print>: png file figures/zjets_m-10to50_amcatnlo_tight_c-jet_eff.png has been created
// Getting efficiencies for loose ID for true udsg-jet on the zjets_m-10to50_amcatnlo sample
// Final efficiency results: (bin upper edge, eff)
// (50, 0.0876487), (70, 0.0832056), (100, 0.0927173), (140, 0.102704), (200, 0.129389), (300, 0.158795), (600, 0.186992), 
// Info in <TCanvas::Print>: png file figures/zjets_m-10to50_amcatnlo_loose_udsg-jet_eff.png has been created
// Getting efficiencies for medium ID for true udsg-jet on the zjets_m-10to50_amcatnlo sample
// Final efficiency results: (bin upper edge, eff)
// (50, 0.00988638), (70, 0.00841069), (100, 0.00975739), (140, 0.0112196), (200, 0.0141448), (300, 0.0182396), (600, 0.0210951), 
// Info in <TCanvas::Print>: png file figures/zjets_m-10to50_amcatnlo_medium_udsg-jet_eff.png has been created
// Getting efficiencies for tight ID for true udsg-jet on the zjets_m-10to50_amcatnlo sample
// Final efficiency results: (bin upper edge, eff)
// (50, 0.000971361), (70, 0.000796317), (100, 0.00104941), (140, 0.00142268), (200, 0.00138753), (300, 0.00117723), (600, 0.00165514), 
// Info in <TCanvas::Print>: png file figures/zjets_m-10to50_amcatnlo_tight_udsg-jet_eff.png has been cr
// eated
// (Int_t) 0
