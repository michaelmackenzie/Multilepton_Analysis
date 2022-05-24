//Script to measure the transfer factors for jets faking hadronic taus
//from a loose anti-jet ID region to a tight anti-jet region
#include "histograms/datacards.C"

//use the dataplotter to manage normalizations and initializations
DataPlotter* dataplotter_ = 0;
int     verbose_       = 0    ;
int     noncl_verbose_ = 0    ;
int     drawFit_       = 1    ; //whether to draw the linear fits
TString process_       = ""   ; //Process of interest, where all others should be subtracted
bool    usingMCTaus_   = false; //whether or not the histogram set uses MC taus
TString selection_     = ""   ;
int     set_offset_    = 0    ; //offset to selection set
int     override_year_ = -1   ; //use a default year to get initial scales for a different year
TRandom3 rnd_(90);

//-------------------------------------------------------------------------------------------------------------------------------
// Standard axis labels
TString get_title(TString hist, TString type) {
  if(hist.Contains("onept"))          return "p_{T}^{1}";
  if(hist.Contains("twopt"))          return "p_{T}^{2}";
  if(hist.Contains("leppt"))          return "p_{T}^{ll}";
  if(hist.Contains("jettauoneeta"))   return "|#eta^{1}|";
  if(hist.Contains("jettautwoeta"))   return "|#eta^{2}|";
  if(hist.Contains("jettaulepeta"))   return "|#eta^{ll}|";
  if(hist.Contains("oneeta"))         return "#eta^{1}";
  if(hist.Contains("twoeta"))         return "#eta^{2}";
  if(hist.Contains("lepeta"))         return "#eta^{ll}";
  if(hist.Contains("lepdeltaphi"))    return "#Delta#phi_{ll}";
  if(hist.Contains("lepdeltaeta"))    return "#Delta#eta_{ll}";
  if(hist.Contains("lepdeltar"))      return "#DeltaR_{ll}";
  if(hist.Contains("deltar"))         return "#DeltaR_{ll}";
  if(hist.Contains("mtone"))          return "M_{T}(l^{1}, MET)";
  if(hist.Contains("mttwo"))          return "M_{T}(l^{2}, MET)";
  if(hist.Contains("mtlep"))          return "M_{T}(ll, MET)";
  if(hist.Contains("onemetdeltaphi")) return "#Delta#phi(l^{1}, MET)";
  if(hist.Contains("twometdeltaphi")) return "#Delta#phi(l^{2}, MET)";
  if(hist.Contains("lepmetdeltaphi")) return "#Delta#phi(ll, MET)";
  if(hist.Contains("onemetdphi"))     return "#Delta#phi(l^{1}, MET)";
  if(hist.Contains("twometdphi"))     return "#Delta#phi(l^{2}, MET)";
  if(hist.Contains("lepmetdphi"))     return "#Delta#phi(ll, MET)";
  if(hist.Contains("lepm"))           return "M_{ll}";
  if(hist.Contains("onereliso"))      return "iso^{1} / p_{T}^{1}";
  if(hist.Contains("tworeliso"))      return "iso^{2} / p_{T}^{2}";
  if(hist.Contains("dm"))             return "Decay Mode";
  if(hist.Contains("decaymode"))      return "Decay Mode";
  if(hist.Contains("tauspt"))         return "p_{T}^{#tau}";
  if(hist.Contains("tauseta"))        return "#eta^{#tau}";
  return hist;
  // xtitle.ReplaceAll("jettau", "");
  // xtitle.ReplaceAll("1", ""); xtitle.ReplaceAll("2", ""); xtitle.ReplaceAll("3", "");
  // bool addone = xtitle.Contains("one");
  // bool addtwo = xtitle.Contains("two");
  // xtitle.ReplaceAll("one","");
  // xtitle.ReplaceAll("two","");
  // xtitle.ReplaceAll("eta", "#eta"); xtitle.ReplaceAll("phi", "#phi"); xtitle.ReplaceAll("pt", "p_{T}");
  // if(addone) xtitle = xtitle + "^{1}";
  // if(addtwo) xtitle = xtitle + "^{2}";
  // xtitle.ReplaceAll("taus", "#tau ");
  // xtitle.ReplaceAll("dm", "Decay Mode");
}

//-------------------------------------------------------------------------------------------------------------------------------
// Get MC fake tau set corresponding to the given set
int get_mc_set(int set) {
  int set_mc = set;
  int remainder = set % 100;
  if(remainder >= 30 && remainder < 33) set_mc = set + 6; //For MC fakes, sets 30 - 32 use 36 - 38
  else if(remainder >= 85 && remainder < 88) set_mc = set - 5; //sets 85 - 87 use 80 - 82
  else if(remainder == 89) set_mc = set - 1; //set 89 uses set 88
  else if(remainder == 93) set_mc = set - 1; //set 93 uses set 92
  else if(remainder == 95) set_mc = set - 1; //set 95 uses set 94
  return set_mc;
}

//-------------------------------------------------------------------------------------------------------------------------------
// Smooth a 1D histogram
TH1* smooth_hist(TH1* h) {
  if(!h) return nullptr;
  TH1* hsmooth = (TH1*) h->Clone(Form("%s_smooth", h->GetName()));
  const static int nsmooth = 1; //number of times to run the smoothing algorithm
  hsmooth->Smooth(nsmooth);
  return hsmooth;
}

//-------------------------------------------------------------------------------------------------------------------------------
// Get a 1D QCD histogram
TH1* get_qcd_1d(TString hist, TString type, int set) {
  dataplotter_->rebinH_ = 1;
  int set_mc = get_mc_set(set);
  TH1* hQCD = dataplotter_->get_qcd(hist, type, set_mc);
  return hQCD;
}

//-------------------------------------------------------------------------------------------------------------------------------
// Get a 2D QCD histogram
TH1* get_qcd_2d(TString hist, TString type, int set) {
  dataplotter_->rebinH_ = 1;
  TH1* hQCD = dataplotter_->get_qcd_2D(hist, type, set);
  return hQCD;
}

//-------------------------------------------------------------------------------------------------------------------------------
//Get the 2D jet pT vs eta histogram
// isdata: 0 = MC true taus; 1 = data taus; -1 = MC fake taus
// icat  : 0 = inclusive; 1 = tight taus; 2 = loose taus
TH2* get_histogram(int setAbs, int ijet, int idm, int isdata, int icat) {
  /*
    Algorithm:

    Data (isdata = 1): Retrieve the data distribution with background processes subtracted (if processes is defined)
    If process is defined, subtract the fake tau distributions for all other processes from the data.
    FIXME: For non-QCD backgrounds, first get the QCD distribution as the Data - all true and fake processes, then subtract it from the data

    MC (isdata = 0): Retrieve the true MC tau distribution for all processes

    Fake MC (isdata = -1): Retrieve the fake MC tau tau distribution for the process of interest only
  */


  if(verbose_ > 0) cout << __func__ << ": Retrieving histogram for set = " << setAbs
                        << " jet-bin = " << ijet << " DM-bin = " << idm
                        << " cat-bin = " << icat << " and isData = " << isdata << endl;

  TH2* h = 0;
  TString name = (isdata < 0) ? "faketaumc" : "faketau"; //using MC jet->tau or not
  name += Form("%ijet%idm_%i", ijet, idm, icat);

  //name for MC fake taus to subtract process specific fake tau estimates from data
  TString faketauname = name;
  if(isdata >= 0) faketauname.ReplaceAll("faketau", "faketaumc");

  unsigned nfiles = dataplotter_->data_.size();
  //get the histogram for each process added to the dataplotter
  for(unsigned d = 0; d < nfiles; ++d) {
    //////////////////////////////////////////////////////////
    // Check if this histogram should be added
    if(process_ == "") { //not process-specific factors
      if(dataplotter_->isData_[d] != (isdata > 0)) continue; //check if using data or not
    } else if(isdata <= 0) { //MC taus, fake or real
      if(dataplotter_->isData_[d]) continue; //skip if data
      //check if this is the correct process if getting MC fake taus (always get all true taus, even in process-specific mode)
      if(isdata < 0 && dataplotter_->labels_[d] != process_ && process_ != "QCD") continue; //for QCD MC fake taus get histogram just to have a dummy histogram
    } else { //data, with other fake tau processes subtracted
      if(dataplotter_->labels_[d] == process_) continue; //skip correct process
    }

    //////////////////////////////////////////////////////////
    //Histogram is accepted, so add to the final histogram

    const bool bkgProcess = isdata == 1 && !dataplotter_->isData_[d]; //MC fake tau to subtract from data (process not interested in)
    const int set = (bkgProcess) ? get_mc_set(setAbs) : setAbs; //offset to the MC fake taus set if subtracting the process specific fake taus
    //add the fixed offset to the fake tau included set, so removing fake+real for other processes, real for interested one
    // if(bkgProcess && !usingMCTaus_ && set % 100 < 90) set += 6;
    // else if(bkgProcess && !usingMCTaus_) set -= 1; //set 93 uses set 92, 95 uses 94
    const TString hpath = Form("event_%i/%s", set, ((bkgProcess) ? faketauname : name).Data());
    if(verbose_ > 1) cout << "Retrieving histogram " << hpath.Data() << " for " << dataplotter_->names_[d].Data()
                          << " with scale = " << dataplotter_->scale_[d] << endl;
    TH2* hTmp = (TH2*) dataplotter_->data_[d]->Get(hpath.Data());
    if(!hTmp) {
      if(verbose_ >= 0) cout << "Histogram " << name.Data() << "/event/" << set << " for " << dataplotter_->names_[d].Data() << " not found!\n";
      continue;
    }
    hTmp = (TH2*) hTmp->Clone(Form("hTmp_%s", name.Data()));
    if(!dataplotter_->isData_[d]) hTmp->Scale(dataplotter_->scale_[d]);

    //for MC fake taus for QCD, just return an empty histogram since no MC fake QCD taus
    if(isdata < 0 && process_ == "QCD") {
      hTmp->Reset();
      hTmp->SetTitle(name.Data());
      return hTmp;
    }

    //subtract MC processes not interested in from the data
    if(bkgProcess) hTmp->Scale(-1.);

    if(verbose_ > 1) cout << "Histogram " << hpath.Data() << " has integral " << hTmp->Integral() << endl;
    if(!h) {h = hTmp; h->SetName(Form("h_%s", name.Data()));}
    else {h->Add(hTmp); delete hTmp;}
  }

  if(!h) return nullptr;

  //if using a non-QCD process and not in the same-sign region, subtract off the QCD with a same-sign selection estimate
  if(isdata == 1 && process_ != "" && process_ != "QCD" && ((setAbs % 2000) < 1000)) {
    //QCD ~ Data - true MC - fake MC = (Data - fake MC) - true MC
    const int set_qcd = setAbs + 1000; //same-sign region, assume transfer factor of 1
    TString tmp = process_;
    process_ = "QCD"; //set to QCD to subtract all other processes
    if(verbose_ > 0) cout << "--> Getting QCD histogram:\n";
    TH2* hQCD = get_histogram(set_qcd, ijet, idm, 1, icat);
    hQCD->Add(get_histogram(set_qcd, ijet, idm, 0, icat), -1.);
    process_ = tmp;
    //clip off negative bins
    for(int xbin = 0; xbin <= hQCD->GetNbinsX()+1; ++xbin) {
      for(int ybin = 0; ybin <= hQCD->GetNbinsY()+1; ++ybin) {
        if(hQCD->GetBinContent(xbin, ybin) < 0.) hQCD->SetBinContent(xbin, ybin, 0.);
      }
    }
    h->Add(hQCD, -1.);
  }

  if(verbose_ > 0) cout << "--- Histogram " << name.Data() << " set " << setAbs << " integral = " << h->Integral() << endl;
  //setup the histogram title and axis titles
  h->SetTitle(name.Data());
  h->SetXTitle("pT (GeV/c)");
  h->SetYTitle("#eta");
  return h;
}

//-------------------------------------------------------------------------------------------------------------------------------
// get a 1D histogram for closure tests
// isdata: 0 = MC; 1 = data; -1 = MC fake taus
TH1* get_closure_hist(TString hist, TString type, int set, int isdata) {

  /*
    Algorithm:

    Data (isdata = 1): Data with other fake hadronic tau processes subtracted (or just all of the data if the process isn't specified)
    For retrieving the data, subtract off the (real + fake) MC contributions from processes not interested in. For non-QCD
    processes, the Data - all real + fake MC processes in the same-sign selection are used to estimate QCD and then subtracted from the data

    MC (isdata = 0): True hadronic tau MC distribution for the specified process (or all true taus if the process isn't specified)
    For retrieving MC true taus (isdata = 0) for the specified process. True taus from other
    processes are not needed since the data fake taus includes subtracting the fake + real MC taus from data

    Fake MC (isdata = -1): Real + fake hadronic tau MC distribution for the specified process
    Essentially the same as the true MC, except if no process is specified nothing is returned
  */

  TH1* h = nullptr;
  const unsigned nfiles = dataplotter_->data_.size();
  //get the histogram for each process added to the dataplotter
  for(unsigned d = 0; d < nfiles; ++d) {
    //////////////////////////////////////////////////////////
    // Check if this histogram should be added
    if(process_ == "") { //not process-specific factors
      if(dataplotter_->isData_[d] != (isdata > 0)) continue; //check if using data or not
    } else if(isdata <= 0) { //MC taus, both real and fake
      if(dataplotter_->isData_[d]) continue; //skip if data
      //check if this is the correct process if getting MC fake taus (always get all true taus, even in process-specific mode)
      if(isdata < 0  && dataplotter_->labels_[d] != process_ && process_ != "QCD") continue; //for QCD MC fake taus get histogram just to have a dummy histogram
      //NOTE: for MC (isdata == 0), only retrieve the process true taus, NOT LIKE ABOVE WHERE THE HISTOGRAMS FILTER BY MC TRUTH
      if(isdata == 0 && process_ != "" && dataplotter_->labels_[d] != process_ && process_ != "QCD") continue;
    } else { //data, with other fake tau processes subtracted
      if(usingMCTaus_ && dataplotter_->labels_[d] == process_) continue; //skip correct process
    }

    //////////////////////////////////////////////////////////
    //Histogram is accepted, so add to the final histogram

    //if not the process of interest, offset to the set with MC fake taus to remove these as well from data
    const bool bkgprocess = isdata == 1 && !dataplotter_->isData_[d]; //MC to subtract from data
    int set_offset = 0;
    if(!usingMCTaus_ && bkgprocess && dataplotter_->labels_[d] != process_) {  //only subtract MC fake taus if in MC true set and from data
      set_offset = get_mc_set(set) - set;
    }
    const int setAbs = set + set_offset;
    const char* hpath = Form("%s_%i/%s", type.Data(), setAbs, hist.Data());
    if(noncl_verbose_ > 2) cout << "  Retrieving histogram " << hpath << " for " << dataplotter_->names_[d].Data()
                          << " with scale = " << dataplotter_->scale_[d] << endl;
    TH1* hTmp = (TH1*) dataplotter_->data_[d]->Get(hpath);
    if(!hTmp) {
      if(noncl_verbose_ >= 0) cout << "!!! Histogram " << hist.Data() << "/" << type.Data() << "/" << setAbs << " for " << dataplotter_->names_[d].Data() << " not found!\n";
      continue;
    }
    hTmp = (TH1*) hTmp->Clone(Form("hTmp_%s_%i_d%i", hist.Data(), set, isdata));
    if(!dataplotter_->isData_[d]) hTmp->Scale(dataplotter_->scale_[d]);

    //for MC fake taus for QCD, just return an empty histogram since no MC fake QCD taus
    if(isdata < 0 && process_ == "QCD") {
      hTmp->Reset();
      hTmp->SetTitle(hist.Data());
      return hTmp;
    }

    if(noncl_verbose_ > 1)
      cout << "  File name " << dataplotter_->names_[d].Data() << ": Histogram " << hpath << " has integral " << hTmp->Integral() << endl;

    //subtract MC processes (true + fakes not interested in) from data
    if(bkgprocess) hTmp->Scale(-1.);

    if(!h) {h = hTmp; h->SetName(Form("h_%s_%i_d%i", hist.Data(), set, isdata));}
    else {h->Add(hTmp); delete hTmp;}
  }

  if(!h) return nullptr;

  //Get the QCD contribution to subtract from data
  if(isdata == 1 && process_ != "" && process_ != "QCD") {
    // TString tmp = process_;
    // process_ = "QCD";
    // const int set_mc = get_mc_set(set);
    // TH1* hQCD = get_closure_hist(hist, type, set_mc, isdata);
    // process_ = tmp;
    // if(!hQCD) return nullptr;
    TH1* hQCD = get_qcd_1d(hist, type, set);
    if(hQCD)
      h->Add(hQCD, -1.);
    else std::cout << __func__ << ": QCD histogram is undefined for " << hist.Data() << "/" << type.Data() << "/" << set << std::endl;
  }

  if(noncl_verbose_ > 0)
    cout << "  --- Histogram " << hist.Data() << " set " << set << " integral = " << h->Integral() << endl;
  //setup the histogram title and axis titles
  TString title, xtitle, ytitle;
  dataplotter_->get_titles(hist, type, &xtitle, &ytitle, &title);
  h->SetTitle("");
  h->SetXTitle(xtitle.Data());
  h->SetYTitle("");
  return h;
}

//-------------------------------------------------------------------------------------------------------------------------------
// get a 1D Mis-ID histogram
TH1* get_misid_hist(TString hist, TString type, int set) {
  if(noncl_verbose_ > 0) cout << " Retrieving MisID histogram for " << hist.Data() << "/" << type.Data() << "/" << set << endl;

  //Data - true MC taus (- fake MC taus from other processes)
  //if using MC fake taus, get MC fake taus for this process, subtracting the MC true taus from the process
  TH1* hMisID = get_closure_hist(hist, type, set, (usingMCTaus_) ? -1 : 1);
  if(!hMisID) return nullptr;

  //If using MC taus, subtract the MC true taus from the distribution
  if(usingMCTaus_) {
    int set_nomc = set;
    if(set % 100 < 40) set_nomc = set - 6; //For no MC fakes, sets 36 - 38 use 30 - 32
    else if(set % 100 < 88) set_nomc = set + 5; //sets 80 - 82 use 85 - 87
    else if(set % 100 == 88) set_nomc = set + 1; //set 88 uses set 89
    // cout << " --- Using no MC fake tau set " << set_nomc << " for MC fake tau set " << set << endl;
    TH1* hTrue = get_closure_hist(hist, type, set_nomc, 0);
    if(!hTrue) return nullptr;
    hMisID->Add(hTrue, -1.);
  }

  const double norm = hMisID->Integral();
  if(norm > 0.) {
    for(int ibin = 1; ibin <= hMisID->GetNbinsX(); ++ibin) {
      if(hMisID->GetBinContent(ibin) < 0.) hMisID->SetBinContent(ibin, 0.);
    }
    hMisID->Scale(norm/hMisID->Integral());
  } else {
    cout << "Warning! Mis-ID histogram " << hist.Data() << "/" << type.Data() << "/" << set
         << " has <= 0 integral = " << norm << endl;
  }
  hMisID->SetName(Form("hmisid_%s_%s_%i", hist.Data(), type.Data(), set));
  hMisID->SetTitle(dataplotter_->misid_label_.Data());
  hMisID->SetLineColor(dataplotter_->misid_color_);
  hMisID->SetLineWidth(2);
  hMisID->SetFillColor(dataplotter_->misid_color_);
  hMisID->SetFillStyle(1001);
  return hMisID;
}

//-------------------------------------------------------------------------------------------------------------------------------
TCanvas* make_closure_canvas(int set1, int set2, PlottingCard_t card, TH1* &hTight, TH1* &hLoose) {
  dataplotter_->plot_data_ = 0;
  dataplotter_->include_qcd_ = 0;
  dataplotter_->include_misid_ = 0;
  dataplotter_->rebinH_ = card.rebin_;
  dataplotter_->doStatsLegend_ = false;
  hTight = nullptr; hLoose = nullptr;

  if(noncl_verbose_ > 0) {
    cout << __func__ << ": Making closure canvas for " << card.hist_.Data() << "/" << card.type_.Data() << " using sets "
         << set1 << " and " << set2 << endl;
  }
  //Get the MisID for the tight region
  hTight = get_misid_hist(card.hist_, card.type_, set1);
  //Get the MisID for the loose region
  hLoose = get_misid_hist(card.hist_, card.type_, set2); //jet --> tau estimate
  if(!hTight || !hLoose) return nullptr;

  if(hLoose->Integral() <= 0. || hTight->Integral() <= 0.) {
    cout << "Warning! Canvas " << card.type_.Data() << "/" << card.hist_.Data() << "/" << set1
         << " has at least 1 non-positive integral! Tight = " << hTight->Integral()
         << " Loose = " << hLoose->Integral() << endl;
  }
  if(card.rebin_ > 1) {hTight->Rebin(card.rebin_); hLoose->Rebin(card.rebin_);};

  hTight->SetLineColor(kBlack);
  hTight->SetMarkerColor(kBlack);
  hTight->SetMarkerStyle(20);
  hTight->SetMarkerSize(0.8);
  hTight->SetFillStyle(0);
  hTight->SetFillColor(0);

  TCanvas* c = new TCanvas("c1","c1",1200,800);
  TPad* pad1 = new TPad("pad1", "pad1", 0., 0.35, 1., 1.0 );
  TPad* pad2 = new TPad("pad2", "pad2", 0., 0.0 , 1., 0.35);
  pad1->SetBottomMargin(0.03);
  pad1->SetTopMargin(0.03);
  pad2->SetTopMargin(0.03);
  pad2->SetBottomMargin(0.3);
  pad1->Draw(); pad2->Draw();
  pad1->cd();
  hTight->Draw("E1");
  hLoose->Draw("hist same");
  hTight->GetXaxis()->SetRangeUser(card.xmin_, card.xmax_);
  hTight->GetYaxis()->SetRangeUser(0.1, 1.1*max(hLoose->GetMaximum(), hTight->GetMaximum()));
  hTight->Draw("E1 sames");
  hTight->GetXaxis()->SetLabelSize(0.);
  hTight->GetYaxis()->SetLabelSize(0.06);
  hTight->GetXaxis()->SetTitleSize(0.157);
  hTight->GetYaxis()->SetTitleSize(0.157);
  hTight->SetTitle("");

  TLegend* leg = new TLegend(0.61, 0.65, 0.9, 0.97);
  leg->AddEntry(hTight, "Tight j#rightarrow#tau");
  leg->AddEntry(hLoose, "j#rightarrow#tau Estimate");
  leg->SetTextSize(0.06);
  leg->Draw();

  pad1->SetGrid();

  //Add labels with the event number and predicted number
  TLatex datalabel;
  datalabel.SetNDC();
  datalabel.SetTextFont(72);
  datalabel.SetTextSize(0.07);
  datalabel.SetTextAlign(13);
  datalabel.DrawLatex(0.65, 0.62,  Form("Tight      : %9.1f", hTight->Integral()));
  datalabel.DrawLatex(0.65, 0.54 , Form("Estimate: %9.1f", hLoose->Integral()));

  //Draw the ratio plot
  pad2->cd();
  TH1* hratio = (TH1*) hTight->Clone(Form("hRatio_%s", hTight->GetName()));
  hratio->Divide(hLoose);
  hratio->Draw("E1");
  // TH1* hsmooth = smooth_hist(hratio);
  // hsmooth->SetLineColor(kBlue);
  // hsmooth->SetFillStyle(1001);
  // hsmooth->SetFillColor(kBlue);
  // hsmooth->Draw("L SAME");
  pad2->Update();

  TString xtitle = get_title(card.hist_, card.type_);
  hratio->GetYaxis()->SetRangeUser(0.5, 1.5);
  hratio->GetXaxis()->SetRangeUser(card.xmin_, card.xmax_);
  hratio->SetXTitle(xtitle.Data());
  hratio->SetYTitle("Tight / Estimate");
  hratio->SetTitle("");
  hratio->GetXaxis()->SetTitleOffset(0.75);
  hratio->GetYaxis()->SetTitleOffset(0.4);
  hratio->GetXaxis()->SetLabelSize(0.1);
  hratio->GetYaxis()->SetLabelSize(0.09);
  hratio->GetXaxis()->SetTitleSize(0.157);
  hratio->GetYaxis()->SetTitleSize(0.11);

  double xmin = (card.xmin_ < card.xmax_) ? card.xmin_ : hratio->GetBinLowEdge(1);
  double xmax = (card.xmin_ < card.xmax_) ? card.xmax_ : hratio->GetBinLowEdge(hratio->GetNbinsX()) + hratio->GetBinWidth(hratio->GetNbinsX());
  TLine* line = new TLine(xmin, 1., xmax, 1.);
  line->SetLineColor(kRed);
  line->SetLineWidth(2);
  line->Draw("sames");
  pad2->SetGrid();
  return c;
}

//-------------------------------------------------------------------------------------------------------------------------------
TCanvas* make_2d_closure_canvas(int set1, int set2, PlottingCard_t card, TH2* &hData, TH2* &hMisID) {
  dataplotter_->include_qcd_ = 0;
  dataplotter_->include_misid_ = 0;
  TH2* hbkg = dataplotter_->get_background_2D(card.hist_, card.type_, set1); //tight MC backgrounds
  if(!hbkg) return NULL;
  dataplotter_->include_qcd_ = 0;
  dataplotter_->include_misid_ = 1;
  hMisID = dataplotter_->get_data_2D(card.hist_, card.type_, set2); //jet --> tau estimate
  if(!hMisID) return NULL;
  hData = dataplotter_->get_data_2D(card.hist_, card.type_, set1); //tight Data
  if(!hData) return NULL;

  hData->Add(hbkg, -1.);
  double norm = hData->Integral();
  for(int xbin = 1; xbin < hData->GetNbinsX(); ++xbin) {
    for(int ybin = 1; ybin < hData->GetNbinsY(); ++ybin) {
      if(hData->GetBinContent(xbin, ybin) < 0.) hData->SetBinContent(xbin, ybin, 0.);
    }
  }

  hbkg = dataplotter_->get_background_2D(card.hist_, card.type_, set2); //loose MC backgrounds
  hMisID->Add(hbkg, -1.);
  norm = hMisID->Integral();
  for(int xbin = 1; xbin < hMisID->GetNbinsX(); ++xbin) {
    for(int ybin = 1; ybin < hMisID->GetNbinsY(); ++ybin) {
      if(hMisID->GetBinContent(xbin, ybin) < 0.) hMisID->SetBinContent(xbin, ybin, 0.);
    }
  }
  hMisID->Scale(norm/hMisID->Integral());

  TCanvas* c = new TCanvas("c1","c1",1500,600);
  c->Divide(3,1);
  TVirtualPad* pad;
  pad = c->cd(1);
  pad->SetRightMargin(0.12);
  hData->SetTitle("Data");
  hData->SetXTitle("p_T");
  hData->SetYTitle("#DeltaR");
  hData->Draw("colz");
  if(card.xmin_ < card.xmax_) hData->GetXaxis()->SetRangeUser(card.xmin_, card.xmax_);
  if(card.ymin_ < card.ymax_) hData->GetYaxis()->SetRangeUser(card.ymin_, card.ymax_);
  pad = c->cd(2);
  pad->SetRightMargin(0.12);
  hMisID->SetTitle("Jet#rightarrow#tau estimate");
  hMisID->SetXTitle("p_T");
  hMisID->SetYTitle("#DeltaR");
  hMisID->Draw("colz");
  if(card.xmin_ < card.xmax_) hMisID->GetXaxis()->SetRangeUser(card.xmin_, card.xmax_);
  if(card.ymin_ < card.ymax_) hMisID->GetYaxis()->SetRangeUser(card.ymin_, card.ymax_);
  pad = c->cd(3);
  pad->SetRightMargin(0.12);
  TH2* hRatio = (TH2*) hData->Clone("hRatio");
  hRatio->SetTitle("Data / Estimate");
  hRatio->Divide(hMisID);
  hRatio->Draw("colz");
  if(hRatio->GetMaximum() > 1.5) hRatio->GetZaxis()->SetRangeUser(0.5, 1.5);
  return c;
}

//-------------------------------------------------------------------------------------------------------------------------------
TCanvas* make_closure_canvas(int set1, int set2, PlottingCard_t card) {
  TH1 *hData, *hMC;
  return make_closure_canvas(set1, set2, card, hData, hMC);
}

//-------------------------------------------------------------------------------------------------------------------------------
TCanvas* make_2d_closure_canvas(int set1, int set2, PlottingCard_t card) {
  TH2 *hData, *hMC;
  return make_2d_closure_canvas(set1, set2, card, hData, hMC);
}

//-------------------------------------------------------------------------------------------------------------------------------
//make a rate canvas
TCanvas* make_jet_canvas(TH2* h[5][4], TString name) {
  TCanvas* c = new TCanvas(name.Data(), name.Data(), 1500, 900);
  TVirtualPad* pad;
  c->Divide(3,2);
  pad = c->cd(1);
  h[0][0]->Draw("colz");
  pad->SetRightMargin(0.12);
  pad->SetLogx();
  pad = c->cd(2);
  h[1][0]->Draw("colz");
  pad->SetRightMargin(0.12);
  pad->SetLogx();
  pad = c->cd(3);
  h[2][0]->Draw("colz");
  pad->SetRightMargin(0.12);
  pad->SetLogx();
  pad = c->cd(4);
  h[3][0]->Draw("colz");
  pad->SetRightMargin(0.12);
  pad->SetLogx();
  pad = c->cd(5);
  h[4][0]->Draw("colz");
  pad->SetRightMargin(0.12);
  pad->SetLogx();
  pad = c->cd(6);
  h[0][1]->Draw("colz");
  pad->SetRightMargin(0.12);
  pad->SetLogx();
  return c;
}

//-------------------------------------------------------------------------------------------------------------------------------
//make 1D plots in eta regions
TCanvas* make_eta_region_canvas(TH2* hnum, TH2* hdnm, TString name, bool iseff,
                                TF1 *&f1, TF1 *&f2, bool print_unc = false, TString tag = "") {
  int nbins = hnum->GetNbinsY();
  TH1* hetas[nbins];
  TCanvas* c = new TCanvas(name.Data(), name.Data(), 500*nbins, 450);
  TVirtualPad* pad;
  double cl = 0.68; //confidence level
  c->Divide(nbins, 1);
  TString decay_mode = "Decay Mode ";
  TString dm_n = hdnm->GetName();
  if     (dm_n.Contains("_0dm")) decay_mode += "0";
  else if(dm_n.Contains("_1dm")) decay_mode += "1";
  else if(dm_n.Contains("_2dm")) decay_mode += "10";
  else if(dm_n.Contains("_3dm")) decay_mode += "11";
  for(int ibin = 0; ibin < nbins; ++ibin) {
    hetas[ibin] = hnum->ProjectionX(Form("%s_eta%i", hnum->GetName(), ibin), ibin+1, ibin+1);
    hetas[ibin]->Divide(hdnm->ProjectionX(Form("%s_tmp_eta%i", hdnm->GetName(), ibin), ibin+1, ibin+1));
    pad = c->cd(ibin+1);
    //set bin error to 0 for <= bins to be ignored in the fit
    for(int jbin = 1; jbin <= hetas[ibin]->GetNbinsX(); ++jbin) {
      if(hetas[ibin]->GetBinContent(jbin) <= 0.) {
        hetas[ibin]->SetBinContent(jbin,1e-6);
        hetas[ibin]->SetBinError  (jbin,0);
      } else if(hetas[ibin]->GetBinContent(jbin) >= 0.9) { //also force a maximum efficiency
        hetas[ibin]->SetBinContent(jbin,0.9);
        hetas[ibin]->SetBinError  (jbin,0.9);
      }
    }
    TString fit_option = (verbose_ > 0) ? "S 0" : "Q S 0";
    if(!drawFit_) {
      fit_option += " 0";
      hetas[ibin]->Draw("E1");
    }
    TF1* func;
    const int mode = 4; //0: pol1 1: atan 2: pol1 + landau 3: pol1 + gaussian
    if(mode == 0) {
      func = new TF1("func", "[slope]*x + [intercept]", 0.,  999.);
      func->SetParameters(0.1, 0.1);
      func->SetParLimits(0, 0., 1.);
      func->SetParLimits(1, -10., 10.);
    } else if(mode == 1) {
      func = new TF1("func", "[scale]*(std::atan([slope]*(x + [xoffset]))/3.14159265 + 0.5) + (1 - [scale])*[yoffset]", 0.,  999.);
      func->SetParameters(0.1, 0.01, 0., 0.);
      // func->SetParLimits(func->GetParNumber("scale")  , 0., 1.); //0
      // func->SetParLimits(func->GetParNumber("slope")  , -1e5, 1e5); //1
      // func->SetParLimits(func->GetParNumber("xoffset"), -1e5, 1e5); //2
      // func->SetParLimits(func->GetParNumber("yoffset"), 0., 1.); //3
    } else if(mode == 2) {
      func = new TF1("func", "[offset] + [slope]*x + [scale]*TMath::Landau(x, [MPV], [Sigma])", 0., 999.);
      func->SetParameters(50., 10., 0.1, 1., 0.1);
      func->SetParLimits(func->GetParNumber("MPV"), -1.e4, 1.e4);    //0
      func->SetParLimits(func->GetParNumber("Sigma"), 1e-6, 1.e5);   //1
      func->SetParLimits(func->GetParNumber("offset"), -1.e3, 1.e3);   //2
      func->SetParLimits(func->GetParNumber("scale"), -1.e7, 1.e7); //3
      func->SetParLimits(func->GetParNumber("slope"), -10., 10.);    //4
    } else if(mode == 3) {
      func = new TF1("func", "[offset] + [slope]*x + [scale]*TMath::Gaus(x, [MPV], [Sigma])", 0., 999.);
      func->SetParameters(50., 10., 0.1, 1., 0.1);
      func->SetParLimits(func->GetParNumber("MPV"), -1.e4, 1.e4);    //0
      func->SetParLimits(func->GetParNumber("Sigma"), 1e-6, 1.e5);   //1
      func->SetParLimits(func->GetParNumber("offset"), -1.e3, 1.e3);   //2
      func->SetParLimits(func->GetParNumber("scale"), -1.e7, 1.e7); //3
      func->SetParLimits(func->GetParNumber("slope"), -10., 10.);    //4
    } else if(mode == 4) {
      func = new TF1("func", "max([offset] + [slope] / (x + [xoffset]), 1.e-5)", 19., 999.);
      func->SetParameters(0.1, -5., 10.);
      // func->SetParLimits(func->GetParNumber("offset"), -1e3, 1e3);   //0
      // func->SetParLimits(func->GetParNumber("slope"), -1.e3, 1.e3);  //1
      func->SetParLimits(func->GetParNumber("xoffset"), -19., 1e4);  //2
    }
    bool refit = false;
    int ifit = 0;
    int max_fits = 5;
    do {
      refit = false;
      auto fit_res = hetas[ibin]->Fit(func, fit_option.Data());
      const double chi_sq = func->GetChisquare() / hetas[ibin]->GetNbinsX();
      if(chi_sq > 1.5) {
        refit = true;
        ++ifit;
        cout << "Fit has chi^2 = " << chi_sq << " --> refitting!\n";
        if(ifit < max_fits && mode == 4) func->SetParameters(2*(0.5 - rnd_.Uniform()), 50.*(0.5 - rnd_.Uniform()), 20.*(0.5 - rnd_.Uniform()));
      }
      // if(!fit_res || fit_res == -1) {
      //   cout << "Fit result not returned!\n";
      // } else if(fit_res == 4) {
      //   cout << "Fit failed! Repeating with shifted values...\n";
      //   if(mode == 4) func->SetParameters(0.1, 1., 2.);
      //   fit_res = hetas[ibin]->Fit(func, fit_option.Data());
      // } else if(verbose_ > 0) {
      //   try {
      //     cout << "Fit address: " << fit_res << endl;
      //     fit_res->Print();
      //   } catch(exception e) {
      //     cout << "Printing the fit result failed: " << e.what() << endl;
      //   }
      // }
    } while(refit && ifit < max_fits);
    hetas[ibin]->SetLineWidth(2);
    hetas[ibin]->SetMarkerStyle(6);
    TString eta_region = Form("%.1f #leq |#eta| < %.1f, %s", hnum->GetYaxis()->GetBinLowEdge(ibin+1),
                              hnum->GetYaxis()->GetBinLowEdge(ibin+1)+hnum->GetYaxis()->GetBinWidth(ibin+1),
                              decay_mode.Data());
    hetas[ibin]->SetTitle(eta_region.Data());
    hetas[ibin]->SetYTitle("f_{#tau}");
    if(iseff) hetas[ibin]->GetYaxis()->SetRangeUser(0., 1.);
    pad->SetLogx(0);
    pad->Update();

    TF1* f = func;
    if(drawFit_ && f) {
      hetas[ibin]->Draw("E1");
      TH1* herr_2s = (TH1*) hetas[ibin]->Clone(Form("%s_err_2s", hetas[ibin]->GetName()));
      herr_2s->Reset();
      (TVirtualFitter::GetFitter())->GetConfidenceIntervals(herr_2s, 0.95);
      herr_2s->SetFillColor(kYellow);
      // herr_2s->SetFillStyle(3001);
      herr_2s->Draw("E3 same");
      TH1* herr_1s = (TH1*) hetas[ibin]->Clone(Form("%s_err_1s", hetas[ibin]->GetName()));
      herr_1s->Reset();
      (TVirtualFitter::GetFitter())->GetConfidenceIntervals(herr_1s, 0.68);
      herr_1s->SetFillColor(kGreen);
      // herr_1s->SetFillStyle(3001);
      herr_1s->Draw("E3 same");
      hetas[ibin]->Draw("E1 same"); //add again on top of confidence intervals
      f->Draw("same");
      if(print_unc) {
        herr_1s->SetName(Form("fit_1s_error_%s_%ieta", tag.Data(), ibin));
        herr_1s->Write();
      }
    } else {
      if(!drawFit_) gStyle->SetOptFit(0);
      if(!f) cout << "!!! " << __func__ << ": Fit function not found! Eta region " << eta_region.Data() << endl;
    }
    if(ibin == 0) f1 = f;
    else f2 = f;
  }
  return c;
}

//-------------------------------------------------------------------------------------------------------------------------------
//make a rate canvas by DM
TCanvas* make_canvas(TH2* h[4], TString name, bool iseff = false) {
  TCanvas* c = new TCanvas(name.Data(), name.Data(), 1000, 900);
  TVirtualPad* pad;
  c->Divide(2,2);
  pad = c->cd(1);
  h[0]->Draw("colz");
  if(iseff)
    h[0]->GetZaxis()->SetRangeUser(0.,1.2*h[0]->GetMaximum());
  pad->SetRightMargin(0.12);
  pad->SetLogx();
  pad = c->cd(2);
  h[1]->Draw("colz");
  if(iseff)
    h[1]->GetZaxis()->SetRangeUser(0.,1.2*h[1]->GetMaximum());
  pad->SetRightMargin(0.12);
  pad->SetLogx();
  pad = c->cd(3);
  h[2]->Draw("colz");
  if(iseff)
    h[2]->GetZaxis()->SetRangeUser(0.,1.2*h[2]->GetMaximum());
  pad->SetRightMargin(0.12);
  pad->SetLogx();
  pad = c->cd(4);
  h[3]->Draw("colz");
  if(iseff)
    h[3]->GetZaxis()->SetRangeUser(0.,1.2*h[3]->GetMaximum());
  pad->SetRightMargin(0.12);
  pad->SetLogx();
  return c;
}

//-------------------------------------------------------------------------------------------------------------------------------
//initialize the files and scales using a DataPlotter
Int_t initialize_plotter(TString base, TString path, int year) {
  if(dataplotter_) delete dataplotter_;
  dataplotter_ = new DataPlotter();
  dataplotter_->include_qcd_ = 0;
  dataplotter_->include_misid_ = 0;
  dataplotter_->verbose_ = max(0, verbose_ - 1);
  dataplotter_->qcd_scale_ = 1.;
  dataplotter_->embed_scale_ = embedScale_;
  years_ = {year};

  // splitWJ_ = 0;

  std::vector<dcard> cards;
  get_datacards(cards, selection_, true);

  CrossSections xs(useUL_, ZMode_); //cross section handler
  dataplotter_->set_luminosity(xs.GetLuminosity(year));
  if(verbose_ > 0) cout << "--- Dataplotter luminosity for " << year << " = " << dataplotter_->lum_ << endl;

  int status(0);
  for(auto card : cards)
    status += dataplotter_->add_dataset(card);

  status += dataplotter_->init_files();
  return status;
}

//-------------------------------------------------------------------------------------------------------------------------------
//Generate the plots and scale factors
Int_t scale_factors(TString selection = "mutau", TString process = "", int set1 = 31, int set2 = 2031, int year = 2016,
                    TString path = "nanoaods_dev") {

  ///////////////////////
  // Initialize params //
  ///////////////////////
  // if(process != "") {
  //   cout << "Warning! Setting process tag to null as not yet implemented!\n";
  //   process = "";
  // }
  process_ = process;
  usingMCTaus_ = ((set1 % 100) > 34 && (set1 % 100) < 40) || ((set1 % 100) >= 80 && (set1 % 100) < 85) || (set1 % 100) == 88 || (set1%100) == 92 || (set1%100) == 94;
  selection_ = selection;

  cout << "Process " << process.Data() << " with selection " << selection.Data() << ", MC fake taus = " << usingMCTaus_ << endl;

  TVirtualFitter::SetMaxIterations( 1e6 );
  //////////////////////
  // Initialize files //
  //////////////////////
  path = "root://cmseos.fnal.gov//store/user/mmackenz/histograms/" + path + "/";

  //get the absolute value of the set, offsetting by the selection
  set_offset_ = 0;
  if(selection == "mutau")      set_offset_ += CLFVHistMaker::kMuTau;
  else if(selection == "etau" ) set_offset_ += CLFVHistMaker::kETau ;
  else if(selection == "emu"  ) set_offset_ += CLFVHistMaker::kEMu  ;
  else if(selection == "mumu" ) set_offset_ += CLFVHistMaker::kMuMu ;
  else if(selection == "ee"   ) set_offset_ += CLFVHistMaker::kEE   ;
  const int set1Abs = set1 + set_offset_;
  const int set2Abs = set2 + set_offset_;

  //construct the general name of each file, not including the sample name
  TString baseName = "clfv_" + selection + "_clfv_";
  baseName += (override_year_ > 0) ? override_year_ : year;
  baseName += "_";

  //initialize the data files
  if(verbose_ > 0) std::cout << "Initializing the dataplotter" << std::endl;
  if(initialize_plotter(baseName, path, (override_year_ > 0) ? override_year_ : year)) {
    cout << "Dataplotter initialization failed!\n";
    return 1;
  }

  //////////////////////
  //  Get histograms  //
  //////////////////////

  const int njetmax = 2; //N(jets) in the event
  const int ndm     = 4; //hadronic tau decay modes
  const int ncat    = 3; //N(categories), 0: all, 1: loose, 2: tight
  TH2* hMCFakeTauRates    [ncat][njetmax+1][ndm]; //MC true fake taus
  TH2* hMCTrueTauRates    [ncat][njetmax+1][ndm]; //MC true genuine taus
  TH2* hDataFakeTauRates  [ncat][njetmax+1][ndm]; //Data taus
  TH2* hMCFakeTauRates_j  [ncat][ndm]; //MC true fake taus, no jet binning
  TH2* hMCTrueTauRates_j  [ncat][ndm]; //MC true genuine taus, no jet binning
  TH2* hDataFakeTauRates_j[ncat][ndm]; //Data taus, no jet binning
  TH2* hMCFakeTauEffs_j   [ndm]; //MC true fake taus, tight / (loose + tight) binned in DM
  TH2* hDataFakeTauEffs_j [ndm]; //Data taus, tight / (loose + tight) binned in DM
  for(int idm = 0; idm < ndm; ++idm) { //Decay mode loop
    for(int icat = 0; icat < ncat; ++icat) { //isolation loop
      hMCFakeTauRates_j  [icat][idm] = nullptr;
      hMCTrueTauRates_j  [icat][idm] = nullptr;
      hDataFakeTauRates_j[icat][idm] = nullptr;
      for(int ijet = 0; ijet <= njetmax; ++ijet) { //N(jets) loop
        /////////////////////////////////
        // Get the MC histograms
        hMCFakeTauRates[icat][ijet][idm] = get_histogram(set1Abs, ijet, idm, -1, icat); //get fake tau MC
        hMCTrueTauRates[icat][ijet][idm] = get_histogram(set1Abs, ijet, idm,  0, icat); //get true tau MC
        if(!hMCFakeTauRates[icat][ijet][idm] || !hMCTrueTauRates[icat][ijet][idm]) { //ensure histograms exis
          cout << "MC Fake or True Histogram for ijet = " << ijet << " idm = " << idm
               << " and icat = " << icat << " not found!\n";
          return 2;
        }
        //ensure weight sum objects are created
        if(!hMCFakeTauRates[icat][ijet][idm]->GetSumw2N()) hMCFakeTauRates[icat][ijet][idm]->Sumw2();
        if(!hMCTrueTauRates[icat][ijet][idm]->GetSumw2N()) hMCFakeTauRates[icat][ijet][idm]->Sumw2();
        //get the histograms from the second set as well, for selections where this is split into loose and tight histogram sets (i.e. not Z->ee/mumu + tau measurements)
        if(set1Abs != set2Abs) {
          hMCFakeTauRates[icat][ijet][idm]->Add(get_histogram(set2Abs, ijet, idm, -1, icat));
          hMCTrueTauRates[icat][ijet][idm]->Add(get_histogram(set2Abs, ijet, idm,  0, icat));
        }
        ///////////////////////////////
        //Get the data histogram
        hDataFakeTauRates[icat][ijet][idm] = get_histogram(set1Abs, ijet, idm, true, icat);
        if(!hDataFakeTauRates[icat][ijet][idm]) {
          cout << "Data Histogram for ijet = " << ijet << " idm = " << idm
               << " and icat = " << icat << " not found!\n";
          return 3;
        }
        //ensure weight sum objects are created
        if(!hDataFakeTauRates[icat][ijet][idm]->GetSumw2N()) hDataFakeTauRates[icat][ijet][idm]->Sumw2();
        if(set1Abs != set2Abs) {
          hDataFakeTauRates[icat][ijet][idm]->Add(get_histogram(set2Abs, ijet, idm, true, icat));
        }
        /////////////////////////////////
        // Handle histograms

        //add to the non-jet binned hists
        if(!(hMCFakeTauRates_j[icat][idm])) hMCFakeTauRates_j[icat][idm] = (TH2*) hMCFakeTauRates[icat][ijet][idm]->Clone(Form("hmc_fake_%icat_%idm", icat, idm));
        else hMCFakeTauRates_j[icat][idm]->Add(hMCFakeTauRates[icat][ijet][idm]);
        if(!(hMCTrueTauRates_j[icat][idm])) hMCTrueTauRates_j[icat][idm] = (TH2*) hMCTrueTauRates[icat][ijet][idm]->Clone(Form("hmc_true_%icat_%idm", icat, idm));
        else hMCTrueTauRates_j[icat][idm]->Add(hMCTrueTauRates[icat][ijet][idm]);
        if(!(hDataFakeTauRates_j[icat][idm])) hDataFakeTauRates_j[icat][idm] = (TH2*) hDataFakeTauRates[icat][ijet][idm]->Clone(Form("hdata_%icat_%idm", icat, idm));
        else hDataFakeTauRates_j[icat][idm]->Add(hDataFakeTauRates[icat][ijet][idm]);

        //subtract MC expectation of true taus from data
        hDataFakeTauRates[icat][ijet][idm]->Add(hMCTrueTauRates[icat][ijet][idm], -1);
      }
      hMCFakeTauRates_j[icat][idm]->SetTitle(hMCFakeTauRates_j[icat][idm]->GetName());
      hMCTrueTauRates_j[icat][idm]->SetTitle(hMCTrueTauRates_j[icat][idm]->GetName());
      hDataFakeTauRates_j[icat][idm]->SetTitle(hDataFakeTauRates_j[icat][idm]->GetName());
      //subtract MC expectation of true taus from data
      hDataFakeTauRates_j[icat][idm]->Add(hMCTrueTauRates_j[icat][idm], -1.);
    }
    //calculate eff of tight / (anti-iso + tight)
    hMCFakeTauEffs_j[idm] = (TH2*) hMCFakeTauRates_j[1][idm]->Clone(Form("hmc_eff_%idm", idm)); //tight taus
    hMCFakeTauEffs_j[idm]->Divide(hMCFakeTauRates_j[0][idm]); //divide by all taus
    hDataFakeTauEffs_j[idm] = (TH2*) hDataFakeTauRates_j[1][idm]->Clone(Form("hdata_eff_%idm", idm)); //tight taus
    hDataFakeTauEffs_j[idm]->Divide(hDataFakeTauRates_j[0][idm]); //divide by all taus
    hMCFakeTauEffs_j[idm]->SetTitle(hMCFakeTauEffs_j[idm]->GetName());
    hDataFakeTauEffs_j[idm]->SetTitle(hDataFakeTauEffs_j[idm]->GetName());
  }

  ///////////////////////////////////////
  // Process the rate histograms

  cout << "--- MC fake rates:\n";
  double nloosemc(0.), ntightmc(0.); //store overall N(tight/loose) MC taus
  for(int idm = 0; idm < ndm; ++idm) {
    cout << " DM = " << (idm + (idm > 1) * 8)
         << " N(loose) = " << hMCFakeTauRates_j[2][idm]->Integral()
         << " N(tight) = " << hMCFakeTauRates_j[1][idm]->Integral()
         << endl;
    nloosemc += hMCFakeTauRates_j[2][idm]->Integral();
    ntightmc += hMCFakeTauRates_j[1][idm]->Integral();
  }
  cout << " Total N(loose) = " << nloosemc << " N(tight) = " << ntightmc << endl;
  cout << "--- Data rates:\n";
  double nloosedata(0.), ntightdata(0.); //store overall N(tight/loose) MC taus
  for(int idm = 0; idm < ndm; ++idm) {
    cout << " DM = " << (idm + (idm > 1) * 8)
         << " N(loose) = " << hDataFakeTauRates_j[2][idm]->Integral()
         << " N(tight) = " << hDataFakeTauRates_j[1][idm]->Integral()
         << endl;
    nloosedata += hDataFakeTauRates_j[2][idm]->Integral();
    ntightdata += hDataFakeTauRates_j[1][idm]->Integral();
  }
  cout << " Total N(loose) = " << nloosedata << " N(tight) = " << ntightdata << endl;

  //////////////////////
  // Draw fake rates  //
  //////////////////////
  gStyle->SetOptStat(0);

  // TCanvas* c1  = make_jet_canvas(hDataFakeTauRates[0], "c_all_jetbinned_data");
  // TCanvas* c2  = make_jet_canvas(hMCFakeTauRates[0], "c_all_jetbinned_mc");
  // TCanvas* c3  = make_jet_canvas(hDataFakeTauRates[1], "c_tight_jetbinned_data");
  // TCanvas* c4  = make_jet_canvas(hMCFakeTauRates[1], "c_tight_jetbinned_mc");
  // TCanvas* c5  = make_jet_canvas(hDataFakeTauRates[2], "c_loose_jetbinned_data");
  // TCanvas* c6  = make_jet_canvas(hMCFakeTauRates[2], "c_loose_jetbinned_mc");
  TCanvas* c7  = make_canvas(hDataFakeTauRates_j[0], "c_all_data");
  TCanvas* c8  = make_canvas(hMCFakeTauRates_j[0]  , "c_all_mc_fake");
  TCanvas* c8b = make_canvas(hMCTrueTauRates_j[0]  , "c_all_mc_true");
  TCanvas* c9  = make_canvas(hDataFakeTauRates_j[1], "c_tight_data");
  TCanvas* c10 = make_canvas(hMCFakeTauRates_j[1]  , "c_tight_mc_fake");
  TCanvas* c10b= make_canvas(hMCTrueTauRates_j[1]  , "c_tight_mc_true");
  TCanvas* c11 = make_canvas(hDataFakeTauRates_j[2], "c_loose_data");
  TCanvas* c12 = make_canvas(hMCFakeTauRates_j[2]  , "c_loose_mc_fake");
  TCanvas* c12b= make_canvas(hMCTrueTauRates_j[2]  , "c_loose_mc_true");
  TCanvas* c13 = make_canvas(hDataFakeTauEffs_j    , "c_eff_data", true);
  TCanvas* c14 = make_canvas(hMCFakeTauEffs_j      , "c_eff_mc", true);


  //////////////////////
  //  Print results   //
  //////////////////////

  //construct general figure name
  TString name = "figures/fake_tau_";
  name += selection + "_";
  if(process_ != "") name += process_ + "_";
  name += year;
  name += "_";
  name += set1;
  name += "/";
  //ensure directories exist
  gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s", name.Data(), name.Data()));
  gSystem->Exec("[ ! -d rootfiles ] && mkdir rootfiles");

  //print canvases
  c7  ->Print((name+"data_tot_rate.png").Data());
  c8  ->Print((name+"mc_fake_tot_rate.png").Data());
  c8b ->Print((name+"mc_true_tot_rate.png").Data());
  c9  ->Print((name+"data_tight_rate.png").Data());
  c10 ->Print((name+"mc_fake_tight_rate.png").Data());
  c10b->Print((name+"mc_true_tight_rate.png").Data());
  c11 ->Print((name+"data_loose_rate.png").Data());
  c12 ->Print((name+"mc_fake_loose_rate.png").Data());
  c12b->Print((name+"mc_true_loose_rate.png").Data());
  c13 ->Print((name+"data_eff.png").Data());
  c14 ->Print((name+"mc_fake_eff.png").Data());

  const char* fname = Form("rootfiles/jet_to_tau_%s_%s%i_%i.root", selection.Data(), (process_ == "") ? "" : (process_+"_").Data(), set1, year);
  TFile* fOut = new TFile(fname, "RECREATE");

  TF1 *funcs[8], *mcfuncs[8];
  for(int idm = 0; idm < ndm; ++idm) {
    //Make data factors
    TCanvas* c = make_eta_region_canvas(hDataFakeTauRates_j[1][idm], hDataFakeTauRates_j[0][idm], Form("c_eta_dm%i_eff_data", idm), true,
                                        funcs[2*idm], funcs[2*idm+1], true, Form("%idm", idm));
    if(!funcs[2*idm] || !funcs[2*idm+1]) cout << "Error gettting fit functions for DM = " << idm << endl;
    else {
      funcs[2*idm]  ->SetName(Form("fit_func_%idm_0eta", idm));
      funcs[2*idm+1]->SetName(Form("fit_func_%idm_1eta", idm));
    }
    c->Print(Form("%sdata_dm%i_eff.png", name.Data(), idm));
    c->cd(1)->SetLogx(1); c->cd(2)->SetLogx(1); c->Update();
    c->Print(Form("%sdata_dm%i_eff_log.png", name.Data(), idm));

    //Make MC fake estimated factors
    c = make_eta_region_canvas(hMCFakeTauRates_j[1][idm], hMCFakeTauRates_j[0][idm], Form("c_eta_dm%i_eff_fake_mc", idm), true,
                               mcfuncs[2*idm], mcfuncs[2*idm+1], true, Form("mc_%idm", idm));
    if(!mcfuncs[2*idm] || !mcfuncs[2*idm+1]) cout << "Error gettting fit functions for DM = " << idm << endl;
    else {
      mcfuncs[2*idm]  ->SetName(Form("fit_mc_func_%idm_0eta", idm));
      mcfuncs[2*idm+1]->SetName(Form("fit_mc_func_%idm_1eta", idm));
    }
    c->Print(Form("%sfake_mc_dm%i_eff.png", name.Data(), idm));
    c->cd(1)->SetLogx(1); c->cd(2)->SetLogx(1); c->Update();
    c->Print(Form("%sfake_mc_dm%i_eff_log.png", name.Data(), idm));
  }

  for(int i = 0; i < 4; ++i) {
    hDataFakeTauEffs_j[i]->Write();
    if(hMCFakeTauEffs_j[i]) hMCFakeTauEffs_j[i]->Write();
    funcs[2*i]->Write();
    funcs[2*i+1]->Write();
    if(mcfuncs[2*i]  ) mcfuncs[2*i]  ->Write();
    if(mcfuncs[2*i+1]) mcfuncs[2*i+1]->Write();
  }

  fOut->Close();
  delete fOut;

  /////////////////////////////////////////////
  // Check closure of various variables
  TH1 *hData, *hMC, *hRatio;
  fOut = new TFile(Form("rootfiles/jet_to_tau_correction_%s_%s%i_%i.root", selection.Data(), (process_ == "") ? "" : (process_+"_").Data(), set1, year), "RECREATE");
  TCanvas* c;

  //total tau pT range
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettauonept" , "lep"  , 0), hData, hMC);
  if(c && hData && hMC) {
    hRatio = (TH1*) hData->Clone("PtScale");
    hRatio->Scale(hMC->Integral() / hData->Integral());
    hRatio->Divide(hMC);
    hRatio->Write();
    // hRatio = smooth_hist(hRatio); //store a smoothed version as well
    // hRatio->Write();
    c->Print(Form("%sjettau_onept.png", name.Data()));
  }

  //tau DM
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("taudecaymode" , "event"  , 0), hData, hMC);
  if(c && hData && hMC) {
    c->Print(Form("%staudecaymode.png", name.Data()));
  }

  //Binned in decay mode
  //DM 0
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettauonept_1" , "lep"  , 0), hData, hMC);
  if(c && hData && hMC) {
    hRatio = (TH1*) hData->Clone("PtScale_DM0");
    hRatio->Scale(hMC->Integral() / hData->Integral());
    hRatio->Divide(hMC);
    hRatio->Write();
    c->Print(Form("%sjettau_onept_DM_0.png", name.Data()));
  }

  //DM 1
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettauonept_2" , "lep"  , 0), hData, hMC);
  if(c && hData && hMC) {
    hRatio = (TH1*) hData->Clone("PtScale_DM1");
    hRatio->Scale(hMC->Integral() / hData->Integral());
    hRatio->Divide(hMC);
    hRatio->Write();
    c->Print(Form("%sjettau_onept_DM_1.png", name.Data()));
  }

  //DM 10
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettauonept_3" , "lep"  , 0), hData, hMC);
  if(c && hData && hMC) {
    hRatio = (TH1*) hData->Clone("PtScale_DM2");
    hRatio->Scale(hMC->Integral() / hData->Integral());
    hRatio->Divide(hMC);
    hRatio->Write();
    c->Print(Form("%sjettau_onept_DM_10.png", name.Data()));
  }

  //DM 11
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettauonept_4" , "lep"  , 0), hData, hMC);
  if(c && hData && hMC) {
    hRatio = (TH1*) hData->Clone("PtScale_DM3");
    hRatio->Scale(hMC->Integral() / hData->Integral());
    hRatio->Divide(hMC);
    hRatio->Write();
    c->Print(Form("%sjettau_onept_DM_11.png", name.Data()));
  }

  //QCD one pT binning
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettauoneptqcd_1" , "lep"  , 0), hData, hMC);
  if(c && hData && hMC) {
    hRatio = (TH1*) hData->Clone("PtScale_QCD");
    hRatio->Scale(hMC->Integral() / hData->Integral());
    hRatio->Divide(hMC);
    hRatio->Write();
    c->Print(Form("%sjettau_oneptqcd_1.png", name.Data()));
  }

  //tau eta correction
  c  = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettautwoeta" , "lep"  , 0), hData, hMC);
  if(c && hData && hMC) {
    hRatio = (TH1*) hData->Clone("EtaScale");
    hRatio->Scale(hMC->Integral() / hData->Integral());
    hRatio->Divide(hMC);
    hRatio->Write();
    c->Print(Form("%sjettau_twoeta.png", name.Data()));
  }

  c  = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettautwoetaqcd1" , "lep"  , 0), hData, hMC);
  if(c && hData && hMC) {
    hRatio = (TH1*) hData->Clone("EtaScale_QCD");
    hRatio->Scale(hMC->Integral() / hData->Integral());
    hRatio->Divide(hMC);
    hRatio->Write();
    c->Print(Form("%sjettau_twoetaqcd_1.png", name.Data()));
  }

  // di-lepton mass
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettaulepm1", "event", 0), hData, hMC);
  if(c && hData && hMC) {
    hRatio = (TH1*) hData->Clone("LepM");
    hRatio->Scale(hMC->Integral() / hData->Integral());
    hRatio->Divide(hMC);
    hRatio->Write();
    c->Print(Form("%sjettau_lepm1.png", name.Data()));
  }

  // di-lepton delta r
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettaudeltar1", "event", 0), hData, hMC);
  if(c && hData && hMC) {
    hRatio = (TH1*) hData->Clone("DeltaR");
    hRatio->Scale(hMC->Integral() / hData->Integral());
    hRatio->Divide(hMC);
    hRatio->Write();
    c->Print(Form("%sjettau_deltar1.png", name.Data()));
  }

  //////////////////////////
  //light lepton met dphi
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettauonemetdeltaphi", "lep", 0), hData, hMC);
  if(c && hData && hMC) {
    hRatio = (TH1*) hData->Clone("OneMetDeltaPhi");
    hRatio->Scale(hMC->Integral() / hData->Integral());
    hRatio->Divide(hMC);
    hRatio->Write();
    c->Print(Form("%sjettau_onemetdeltaphi.png", name.Data()));
  }

  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettauonemetdphiqcd1", "lep", 0), hData, hMC);
  if(c && hData && hMC) {
    hRatio = (TH1*) hData->Clone("OneMetDeltaPhi_QCD");
    hRatio->Scale(hMC->Integral() / hData->Integral());
    hRatio->Divide(hMC);
    hRatio->Write();
    c->Print(Form("%sjettau_onemetdeltaphi_qcd_1.png", name.Data()));
  }

  //Decay mode binned light lepton met dphi
  // DM 0
  c  = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettauonemetdeltaphi_0", "lep", 0), hData, hMC);
  if(c && hData && hMC) {
    hRatio = (TH1*) hData->Clone("OneMetDeltaPhi0");
    hRatio->Scale(hMC->Integral() / hData->Integral());
    hRatio->Divide(hMC);
    hRatio->Write();
    c->Print(Form("%sjettau_onemetdeltaphi_DM_0.png", name.Data()));
  }

  // DM 1
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettauonemetdeltaphi_1", "lep", 0), hData, hMC);
  if(c && hData && hMC) {
    hRatio = (TH1*) hData->Clone("OneMetDeltaPhi1");
    hRatio->Scale(hMC->Integral() / hData->Integral());
    hRatio->Divide(hMC);
    hRatio->Write();
    c->Print(Form("%sjettau_onemetdeltaphi_DM_1.png", name.Data()));
  }

  // DM 10
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettauonemetdeltaphi_2", "lep", 0), hData, hMC);
  if(c && hData && hMC) {
    hRatio = (TH1*) hData->Clone("OneMetDeltaPhi2");
    hRatio->Scale(hMC->Integral() / hData->Integral());
    hRatio->Divide(hMC);
    hRatio->Write();
    c->Print(Form("%sjettau_onemetdeltaphi_DM_10.png", name.Data()));
  }

  // DM 11
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettauonemetdeltaphi_3", "lep", 0), hData, hMC);
  if(c && hData && hMC) {
    hRatio = (TH1*) hData->Clone("OneMetDeltaPhi3");
    hRatio->Scale(hMC->Integral() / hData->Integral());
    hRatio->Divide(hMC);
    hRatio->Write();
    c->Print(Form("%sjettau_onemetdeltaphi_DM_11.png", name.Data()));
  }

  /////////////////////////////////////////////
  // Bias tests
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettaulepm2", "event", 0), hData, hMC);
  if(c && hData && hMC) {
    hRatio = (TH1*) hData->Clone("LepMBias");
    hRatio->Divide(hMC); //No integral matching since bias test should include shape + rate
    hRatio->Write();
    c->Print(Form("%sjettau_lepm_bias.png", name.Data()));
    //Add a bias option that only corrects the shape
    hRatio = (TH1*) hData->Clone("LepMBiasShape");
    hMC->Scale(hData->Integral() / hMC->Integral());
    hRatio->Divide(hMC);
    hRatio->Write();
  }

  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettaumtlep2", "event", 0), hData, hMC);
  if(c && hData && hMC) {
    hRatio = (TH1*) hData->Clone("MTLepBias");
    hRatio->Divide(hMC); //No integral matching since bias test should include shape + rate
    hRatio->Write();
    c->Print(Form("%sjettau_mtlep_bias.png", name.Data()));
  }

  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettaudeltar2", "event", 0), hData, hMC);
  if(c && hData && hMC) {
    hRatio = (TH1*) hData->Clone("DeltaRBias");
    hRatio->Divide(hMC); //No integral matching since bias test should include shape + rate
    hRatio->Write();
    c->Print(Form("%sjettau_deltar_bias.png", name.Data()));
  }

  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettaumtone2", "event", 0), hData, hMC);
  if(c && hData && hMC) {
    hRatio = (TH1*) hData->Clone("MTOneBias");
    hRatio->Divide(hMC); //No integral matching since bias test should include shape + rate
    hRatio->Write();
    c->Print(Form("%sjettau_mtone_bias.png", name.Data()));
  }

  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettaumttwo2", "event", 0), hData, hMC);
  if(c && hData && hMC) {
    hRatio = (TH1*) hData->Clone("MTTwoBias");
    hRatio->Divide(hMC); //No integral matching since bias test should include shape + rate
    hRatio->Write();
    c->Print(Form("%sjettau_mttwo_bias.png", name.Data()));
  }

  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettauonereliso_1", "lep", 0), hData, hMC);
  if(c && hData && hMC) {
    hRatio = (TH1*) hData->Clone("OneIsoBias");
    hRatio->Divide(hMC); //No integral matching since bias test should include shape + rate
    hRatio->Write();
    c->Print(Form("%sjettau_onereliso_bias.png", name.Data()));
  }

  fOut->Close();

  /////////////////////////////////////////////
  // Additional figures / closure tests

  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettautwometdeltaphi"  , "lep"  , 0               )); if(c) c->Print(Form("%sjettau_twometdeltaphi.png"   , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettautwometdeltaphi0" , "lep"  , 0               )); if(c) c->Print(Form("%sjettau_twometdeltaphi_0.png" , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettauonemetdeltaphi0" , "lep"  , 0               )); if(c) c->Print(Form("%sjettau_onemetdeltaphi_0.png" , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("onept"                 , "lep"  , 0, 2,  20., 100.)); if(c) c->Print(Form("%sonept.png"                   , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("onereliso"             , "lep"  , 0, 1,   0.,  0.5)); if(c) c->Print(Form("%sonereliso.png"               , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettauonereliso_0"     , "lep"  , 0, 0,   0.,  0.5)); if(c) c->Print(Form("%sjettau_onereliso.png"        , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettauoneptqcd_0"      , "lep"  , 0, 0,   0., 200.)); if(c) c->Print(Form("%sjettau_oneptqcd_0.png"       , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettauonemetdphiqcd0"  , "lep"  , 0, 0,   1.,  -1.)); if(c) c->Print(Form("%sjettau_onemetdphiqcd_0.png"  , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettautwoetaqcd0"      , "lep"  , 0, 0,   1.,  -1.)); if(c) c->Print(Form("%sjettau_twoetaqcd_0.png"      , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettautwopt0"          , "lep"  , 0, 0,   1.,  -1.)); if(c) c->Print(Form("%sjettau_twopt_0.png"          , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettauonept0"          , "lep"  , 0, 0,   1.,  -1.)); if(c) c->Print(Form("%sjettau_onept_0.png"          , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("onept11"               , "lep"  , 0, 2,  20., 100.)); if(c) c->Print(Form("%sonept11.png"                 , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("oneeta"                , "lep"  , 0, 2,  -3.,   3.)); if(c) c->Print(Form("%soneeta.png"                  , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("twopt"                 , "lep"  , 0, 2,  20., 100.)); if(c) c->Print(Form("%stwopt.png"                   , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("twopt11"               , "lep"  , 0, 2,  20., 100.)); if(c) c->Print(Form("%stwopt11.png"                 , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("twoeta"                , "lep"  , 0, 2,  -3.,   3.)); if(c) c->Print(Form("%stwoeta.png"                  , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("tausdm"                , "event", 0, 1,   0.,  12.)); if(c) c->Print(Form("%stausdm.png"                  , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("tauspt"                , "event", 0, 1,  20., 100.)); if(c) c->Print(Form("%stauspt.png"                  , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("mtone"                 , "event", 0, 5,   0., 150.)); if(c) c->Print(Form("%smtone.png"                   , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettaumtone0"          , "event", 0               )); if(c) c->Print(Form("%sjettau_mtone0.png"           , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettaumtone1"          , "event", 0               )); if(c) c->Print(Form("%sjettau_mtone1.png"           , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("mttwo"                 , "event", 0, 5,   0., 150.)); if(c) c->Print(Form("%smttwo.png"                   , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettaumttwo0"          , "event", 0               )); if(c) c->Print(Form("%sjettau_mttwo0.png"           , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettaumttwo1"          , "event", 0               )); if(c) c->Print(Form("%sjettau_mttwo1.png"           , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("mtlep"                 , "event", 0, 2,   0., 150.)); if(c) c->Print(Form("%smtlep.png"                   , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettaumtlep0"          , "event", 0               )); if(c) c->Print(Form("%sjettau_mtlep0.png"           , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettaumtlep1"          , "event", 0               )); if(c) c->Print(Form("%sjettau_mtlep1.png"           , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("lepm"                  , "event", 0, 5,  50., 170.)); if(c) c->Print(Form("%slepm.png"                    , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettaulepm0"           , "event", 0               )); if(c) c->Print(Form("%sjettau_lepm0.png"            , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("lepmestimate"          , "event", 0, 5, 50.,  200.)); if(c) c->Print(Form("%slepmestimate.png"            , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("lepmestimatetwo"       , "event", 0, 5, 50.,  200.)); if(c) c->Print(Form("%slepmestimatetwo.png"         , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettaudeltar0"         , "event", 0               )); if(c) c->Print(Form("%sjettau_deltar0.png"          , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettaudeltar1"         , "event", 0               )); if(c) c->Print(Form("%sjettau_deltar1.png"          , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("ntaus"                 , "event", 0, 1,   0.,   7.)); if(c) c->Print(Form("%sntaus.png"                   , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("njets"                 , "event", 0, 1,   0.,   7.)); if(c) c->Print(Form("%snjets.png"                   , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("njets20"               , "event", 0, 1,   0.,   7.)); if(c) c->Print(Form("%snjets20.png"                 , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("lepdeltar"             , "event", 0, 2,   0.,   6.)); if(c) c->Print(Form("%slepdeltar.png"               , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("lepdeltaphi"           , "event", 0, 2,   0.,   4.)); if(c) c->Print(Form("%slepdeltaphi.png"             , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettauoner"            , "lep"  , 0               )); if(c) c->Print(Form("%sjettau_oner.png"             , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettautwor"            , "lep"  , 0               )); if(c) c->Print(Form("%sjettau_twor.png"             , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("jettautwopt"           , "lep"  , 0               )); if(c) c->Print(Form("%sjettau_twopt.png"            , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("onemetdeltaphi"        , "lep"  , 0, 1,  0.,   4. )); if(c) c->Print(Form("%sonemetdeltaphi.png"          , name.Data()));
  c = make_closure_canvas(set1Abs, set2Abs, PlottingCard_t("twometdeltaphi"        , "lep"  , 0, 1,  0.,   4. )); if(c) c->Print(Form("%stwometdeltaphi.png"          , name.Data()));
  // make_2d_canvas(setAbs1, setAbs2, PlottingCard_t("jettautwoptvsr", "lep", 0), true);
  // make_2d_canvas(setAbs1, setAbs2, PlottingCard_t("twoptvsonept"  , "lep", 0), true);

  //Make stack plots
  dataplotter_->include_misid_ = 1;
  dataplotter_->data_over_mc_ = 0;
  c = dataplotter_->plot_stack("twopt", "lep"  , set1Abs); if(c) c->Print(Form("%sstack_twopt_%i.png", name.Data(), set1));
  c = dataplotter_->plot_stack("onept", "lep"  , set1Abs); if(c) c->Print(Form("%sstack_onept_%i.png", name.Data(), set1));
  c = dataplotter_->plot_stack("lepm" , "event", set1Abs); if(c) c->Print(Form("%sstack_lepm_%i.png" , name.Data(), set1));
  c = dataplotter_->plot_stack("leppt", "event", set1Abs); if(c) c->Print(Form("%sstack_leppt_%i.png", name.Data(), set1));

  return 0;
}
