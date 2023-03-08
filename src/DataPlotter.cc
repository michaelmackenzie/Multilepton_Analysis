#include "interface/DataPlotter.hh"
using namespace CLFV;

//--------------------------------------------------------------------------------------------------------------------
// general method to get a list of histograms, either for data, signal, or backgrounds
std::vector<TH1*> DataPlotter::get_histograms(TString hist, TString setType, Int_t set, Int_t Mode, TString process, ScaleUncertainty_t* sys_scale) {
  //list of histograms to return
  std::vector<TH1*> histograms;

  if(verbose_ > 5) printf("%s: Retrieving histograms for %s/%s/%i, Mode %i, and process %s\n",
                          __func__, hist.Data(), setType.Data(), set, Mode, process.Data());
  //ensure the histogram mode is defined
  if(Mode != kData && Mode != kSignal && Mode != kBackground && Mode != kAny) {
    std::cout << __func__ << ": Unknown histogram list mode: " << Mode << std::endl;
    return histograms;
  }
  if(sys_scale && sys_scale->scale_ <= 0.) {
    printf("%s: Warning! Scale uncertainty %s has scale <= 0 (%f)\n", __func__, sys_scale->name_.Data(), sys_scale->scale_);
  }

  //for combining histograms of the same label
  std::map<TString, unsigned int> indexes;

  for(UInt_t i = 0; i < inputs_.size(); ++i) {
    Data_t& input = inputs_[i];
    histograms.push_back(nullptr);
    const bool isData = input.isData_;
    const bool isSignal = input.isSignal_;
    const bool isBackground = !(isData || isSignal);
    const bool isEmbed = input.isEmbed_;
    if(Mode != kAny) {
      if(Mode != kData && isData) continue;
      if(Mode != kSignal && isSignal) continue;
      if(Mode != kBackground && isBackground) continue;
    }
    if(process != "" && input.label_ != process) continue;
    TH1* tmp = (TH1*) input.data_->Get(Form("%s_%i/%s", setType.Data(), set, hist.Data()));
    if(!tmp) {
      printf("%s: Histogram %s/%s/%i for %s (%s) %i not found! Continuing...\n",
             __func__, hist.Data(), setType.Data(), set, input.name_.Data(), input.label_.Data(), input.dataYear_);
      continue;
    }
    if(verbose_ > 7) printf(" retrieved histogram from input %i (%i %s)\n", i, input.dataYear_, input.name_.Data());
    //check if the systematic has been filled
    if(replace_missing_sys_ && tmp->GetEntries() == 0 && setType == "systematic") {
      TString base_name(hist(0,hist.Last('_')));
      base_name += "_0";
      TH1* tmp_base = (TH1*) input.data_->Get(Form("%s_%i/%s", setType.Data(), set, base_name.Data()));
      if(tmp_base && tmp_base->GetEntries() > 0) {
        if(verbose_ > 1) printf("%s: Replacing missing systematic %s in input %s with the base histogram\n", __func__, hist.Data(), input.label_.Data());
        tmp = tmp_base;
      } else if(!tmp_base) {
        printf("%s: Replacement for missing systematic %s in input %s not found!\n", __func__, hist.Data(), input.label_.Data());
      }
    }

    // tmp->SetBit(kCanDelete);
    tmp = (TH1*) tmp->Clone("htmp");
    tmp->SetDirectory(0);
    tmp->SetLineColor(input.color_);
    tmp->SetFillColor(input.color_);
    tmp->SetMarkerColor(input.color_);

    //determine the scale to apply, depending on if there's a scale uncertainty
    double scale = input.scale_;
    if(sys_scale) {
      bool passed = true;
      passed &= sys_scale->process_ == "" || sys_scale->process_ == input.name_;
      passed &= sys_scale->tag_ == "" || input.name_.Contains(sys_scale->tag_);
      passed &= sys_scale->veto_ == "" || !input.name_.Contains(sys_scale->veto_);
      passed &= sys_scale->year_ < 0 || sys_scale->year_ == input.dataYear_;
      passed &= ( isData && sys_scale->data_ ) || !isData;
      passed &= (!isData && sys_scale->mc_   ) || isData;
      passed &= (isEmbed && sys_scale->embed_) || !isEmbed;
      if(passed) {
        scale *= sys_scale->scale_;
        if(verbose_ > 1) printf("%s: Applying %f scale to %s input\n", __func__, sys_scale->scale_, input.name_.Data());
      }
    }
    tmp->Scale(scale);

    histograms[i] = tmp;
    if(density_plot_) density(histograms[i]);
    unsigned int index = i;
    if(indexes.count(input.label_)) {
      index = indexes[input.label_];
    } else {
      indexes.insert({input.label_, i});
    }
    if(index !=  i)  {
      histograms[index]->Add(histograms[i]);
      delete histograms[i];
      histograms[i] = nullptr;
    } else {
      TString hname = Form("%s_%s_%i", input.label_.Data(), hist.Data(), set);
      if(density_plot_) hname += "_density";
      auto o = gDirectory->Get(hname.Data());
      if(o) delete o;
      histograms[index]->SetName(hname.Data());
      histograms[index]->SetTitle(input.label_.Data()); //store the label in the histogram title
    }
  }
  //return only meaningful histograms
  std::vector<TH1*> hresults;
  for(unsigned int i = 0; i < inputs_.size(); ++i) {
    TH1* h = histograms[i];
    if(h) {
      if(h->Integral() >= 0. || include_empty_) {
        if(h->Integral() <= 0.) h->Scale(0.); //set to be 0 if < 0
        else if(clip_negative_) { //remove negative bin edges, rescale the positive bins to conserve the rate
          const double rate = h->Integral();
          for(int ibin = 1; ibin <= h->GetNbinsX(); ++ibin) {
            if(h->GetBinContent(ibin) < 0.) h->SetBinContent(ibin, 0.);
          }
          h->Scale(rate/h->Integral());
        }
        hresults.push_back(h);
      } else {
        delete h;
      }
    }
  }

  return hresults;
}

//--------------------------------------------------------------------------------------------------------------------
std::vector<TH1*> DataPlotter::get_signal(TString hist, TString setType, Int_t set, ScaleUncertainty_t* sys_scale) {
  std::vector<TH1*> histograms = get_histograms(hist, setType, set, kSignal, "", sys_scale);
  if(verbose_ > 2) printf("%s: Retrieved the list of %i signal histograms\n", __func__, (int) histograms.size());
  for(unsigned index = 0; index < histograms.size(); ++index) {
    if(verbose_ > 7) printf(" Updating drawing settings for signal %i (%s)\n", (int) index, histograms[index]->GetTitle());
    // const int i_color = index % (sizeof(signal_colors_) / sizeof(*signal_colors_));
    histograms[index]->SetFillStyle(signal_fill_style_);
    // histograms[index]->SetFillColor(signal_colors_[i_color]);
    // histograms[index]->SetLineColor(signal_colors_[i_color]);
    // histograms[index]->SetMarkerColor(signal_colors_[i_color]);
    histograms[index]->SetLineWidth(3);
    if(rebinH_ > 1) histograms[index]->Rebin(rebinH_);
    if(verbose_ > 8) printf(" Getting stats info for signal %i (%s)\n", (int) index, histograms[index]->GetTitle());
    const Double_t integral = (histograms[index]->Integral((density_plot_ > 0) ? "width" : "")
                               + histograms[index]->GetBinContent(0)
                               + histograms[index]->GetBinContent(histograms[index]->GetNbinsX()+1));
    const char* stats = (doStatsLegend_) ? Form(": #scale[0.8]{%.2e}", integral) : "";
    const TString label = histograms[index]->GetTitle();
    Double_t signal_scale = signal_scale_;
    if(signal_scales_.find(label) != signal_scales_.end()) {
      signal_scale = signal_scales_[label];
    }
    if(verbose_ > 8) printf(" Setting stats info for signal %i (%s)\n", (int) index, histograms[index]->GetTitle());
    histograms[index]->SetTitle(Form("%s%s%s", label.Data(),
                                     (signal_scale == 1.) ? "" : Form(" (x%.0f)",signal_scale), stats));
    if(verbose_ > 8) printf(" Scaling signal %i (%s) by %.3f\n", (int) index, histograms[index]->GetTitle(), signal_scale);
    histograms[index]->Scale(signal_scale);
    if(verbose_ > 0) std::cout << histograms[index]->GetTitle() << " signal histogram has integral "
                               << integral << std::endl;
  }
  if(verbose_ > 5) printf(" Returning the list of %i signal histograms\n", (int) histograms.size());
  return histograms;
}

//--------------------------------------------------------------------------------------------------------------------
TH1* DataPlotter::get_data_mc_diff(TString hist, TString setType, Int_t set, ScaleUncertainty_t* sys_scale) {
  TString hname = Form("Diff_%s_%s_%i", hist.Data(), setType.Data(), set);
  if(density_plot_) hname += "_density";
  {
    auto o = gDirectory->Get(hname.Data());
    if(o) delete o;
  }

  //get the data histogram
  const Int_t tmpRebin = rebinH_;
  rebinH_ = 1; //don't rebin the data yet
  TH1* hData = get_data(hist, setType, set, sys_scale);
  rebinH_ = tmpRebin;
  if(!hData) return nullptr;

  //redefine the data histogram as the Difference histogram
  hData->SetName(hname.Data());

  //get the MC background histograms
  std::vector<TH1*> backgrounds = get_histograms(hist, setType, set, kBackground, "", sys_scale);
  TH1* hBackground = nullptr;
  for(unsigned index = 0; index < backgrounds.size(); ++index) {
    if(!hBackground) {
      hBackground = (TH1*) backgrounds[index]->Clone("Diff_bkg_tmp");
    } else {
      hBackground->Add(backgrounds[index]);
    }
    delete backgrounds[index];
  }

  //check if the background is defined
  if(!hBackground) {
    std::cout << __func__ << ": Warning! No background MC histogram found when calculating Data - MC histogram in set "
              << set << std::endl;
    return nullptr;
  }

  //subtract the MC backgrounds from the data
  hData->Add(hBackground, -1.);
  delete hBackground;

  //return the difference
  return hData;
}

//--------------------------------------------------------------------------------------------------------------------
TH1* DataPlotter::get_data(TString hist, TString setType, Int_t set, ScaleUncertainty_t* sys_scale) {
  TString hname = Form("hData_%s_%i", hist.Data(), set);
  if(density_plot_) hname += "_density";
  {
    auto o = gDirectory->Get(hname.Data());
    if(o) delete o;
  }
  std::vector<TH1*> histograms = get_histograms(hist, setType, set, kData, "", sys_scale);
  TH1* hdata = nullptr;
  for(unsigned index = 0; index < histograms.size(); ++index) {
    if(!hdata) {
      hdata = (TH1*) histograms[index]->Clone(hname.Data());
    } else {
      hdata->Add(histograms[index]);
    }
    delete histograms[index];
  }
  if(!hdata) return nullptr;
  hdata->SetLineWidth(2);
  hdata->SetMarkerStyle(20);
  const Double_t integral = (hdata->Integral((density_plot_ > 0) ? "width" : "")
                             + hdata->GetBinContent(0)
                             + hdata->GetBinContent(hdata->GetNbinsX()+1));
  const char* stats = (doStatsLegend_) ? Form(": #scale[0.8]{%.2e}", integral) : "";
  hdata->SetTitle(Form("Data%s",stats));
  if(verbose_ > 0) std::cout << "Data histogram has integral " << integral  << std::endl;
  if(rebinH_ > 0) hdata->Rebin(rebinH_);
  return hdata;
}

//--------------------------------------------------------------------------------------------------------------------
TH2* DataPlotter::get_data_2D(TString hist, TString setType, Int_t set) {
  const int rebin_prev = rebinH_;
  rebinH_ = 1;
  TH2* hdata = (TH2*) get_data(hist, setType, set);
  if(!hdata) return nullptr;
  rebinH_ = rebin_prev;
  if(rebinH_ > 0) {
    hdata->RebinX(rebinH_);
    hdata->RebinY(rebinH_);
  }
  return hdata;
}

//--------------------------------------------------------------------------------------------------------------------
TH1* DataPlotter::get_qcd(TString hist, TString setType, Int_t set, ScaleUncertainty_t* sys_scale) {
  TString hname = Form("QCD_%s_%s_%i", hist.Data(), setType.Data(), set);
  if(density_plot_) hname += "_density";
  {
    auto o = gDirectory->Get(hname.Data());
    if(o) delete o;
  }
  //check if in a same-sign region, where QCD estimate is undefined
  if((set >= qcd_offset_ && set < misid_offset_) || (set >= misid_offset_ + qcd_offset_)) return nullptr;

  //get the data histogram
  const Int_t set_qcd = set + qcd_offset_;
  TH1* hData = get_data_mc_diff(hist, setType, set_qcd, sys_scale);
  if(!hData) return nullptr;

  //redefine the data histogram as the QCD histogram
  hData->SetName(hname.Data());

  if(rebinH_ > 1) hData->Rebin(rebinH_);

  //include the Mis-ID background subtraction if included
  TH1* hMisID = (include_misid_) ? get_misid(hist, setType, set_qcd, sys_scale) : nullptr;
  if(hMisID) {hData->Add(hMisID, -1.); delete hMisID;}


  //store the integral before clipping negative bins
  const double nbefore = hData->Integral(0, hData->GetNbinsX()+1, (density_plot_ > 0) ? "width" : "");

  //set all bins >= 0, including over/underflow
  for(int i = 0; i <= hData->GetNbinsX()+1; ++i) {
    if(hData->GetBinContent(i) < 0.) {
      hData->SetBinContent(i,0.);
    }
  }

  //ensure the normalization doesn't change
  const double nafter = hData->Integral(0, hData->GetNbinsX()+1, (density_plot_ > 0) ? "width" : "");

  if(nbefore < 0.) hData->Scale(0.);
  else if(nafter > 0.) hData->Scale(qcd_scale_*(nbefore/nafter));

  const double nqcd = hData->Integral(0, hData->GetNbinsX()+1, (density_plot_ > 0) ? "width" : "");

  const char* stats = (doStatsLegend_) ? Form(": #scale[0.8]{%.2e}", nqcd) : "";
  hData->SetTitle(Form("QCD%s",stats));
  if(fill_alpha_ < 1.) {
    hData->SetLineColorAlpha(qcd_color_,fill_alpha_);
    hData->SetFillColorAlpha(qcd_color_,fill_alpha_);
  } else {
    hData->SetLineColor(qcd_color_);
    hData->SetFillColor(qcd_color_);
  }
  return hData;
}

//--------------------------------------------------------------------------------------------------------------------
TH2* DataPlotter::get_qcd_2D(TString hist, TString setType, Int_t set) {
  TString hname = Form("QCD_%s_%s_%i", hist.Data(), setType.Data(), set);
  if(density_plot_) hname += "_density";
  {
    auto o = gDirectory->Get(hname.Data());
    if(o) delete o;
  }
  //check if in a same-sign region, where QCD estimate is undefined
  if((set >= qcd_offset_ && set < misid_offset_) || (set >= misid_offset_ + qcd_offset_)) return nullptr;

  //get the data histogram
  const Int_t set_qcd = set + qcd_offset_;
  TH2* hData = (TH2*) get_data_mc_diff(hist, setType, set_qcd);
  if(!hData) return nullptr;

  //redefine the data histogram as the QCD histogram
  hData->SetName(hname.Data());

  if(rebinH_ > 1) {
    hData->RebinX(rebinH_);
    hData->RebinY(rebinH_);
  }

  //include the Mis-ID background subtraction if included
  TH2* hMisID = (include_misid_) ? get_misid_2D(hist, setType, set_qcd) : nullptr;
  if(hMisID) {hData->Add(hMisID, -1.); delete hMisID;}


  //store the integral before clipping negative bins
  const double nbefore = hData->Integral(0, hData->GetNbinsX()+1, 0, hData->GetNbinsY()+1,
                                         (density_plot_ > 0) ? "width" : "");

  //set all bins >= 0, including over/underflow
  for(int i = 0; i <= hData->GetNbinsX()+1; ++i) {
    for(int j = 0; j <= hData->GetNbinsY()+1; ++j) {
      if(hData->GetBinContent(i,j) < 0.)
        hData->SetBinContent(i,j,0.);
    }
  }

  //ensure the normalization doesn't change
  const double nafter = hData->Integral(0, hData->GetNbinsX()+1, 0, hData->GetNbinsY()+1,
                                         (density_plot_ > 0) ? "width" : "");
  if(nbefore < 0.) hData->Scale(0.);
  else if(nafter > 0.) hData->Scale(qcd_scale_*(nbefore/nafter));

  const double nqcd = hData->Integral(0, hData->GetNbinsX()+1, 0, hData->GetNbinsY()+1,
                                         (density_plot_ > 0) ? "width" : "");

  const char* stats = (doStatsLegend_) ? Form(": #scale[0.8]{%.2e}", nqcd) : "";
  hData->SetTitle(Form("QCD%s",stats));
  return hData;
}

//--------------------------------------------------------------------------------------------------------------------
TH1* DataPlotter::get_misid(TString hist, TString setType, Int_t set, ScaleUncertainty_t* sys_scale) {
  TString hname = Form("MisID_%s_%s_%i", hist.Data(), setType.Data(), set);
  if(density_plot_) hname += "_density";
  {
    auto o = gDirectory->Get(hname.Data());
    if(o) delete o;
  }
  //check if in a loose ID region, where Mis-ID estimate is undefined
  if(set >= misid_offset_) return nullptr;

  //get the data histogram
  const Int_t set_misid = set + misid_offset_;
  TH1* hData = get_data_mc_diff(hist, setType, set_misid, sys_scale);
  if(!hData) return nullptr;

  //redefine the data histogram as the MisID histogram
  hData->SetName(hname.Data());

  if(rebinH_ > 1) hData->Rebin(rebinH_);

  //include the QCD background subtraction if included
  TH1* hQCD = (include_qcd_) ? get_qcd(hist, setType, set_misid, sys_scale) : nullptr;
  if(hQCD) {hData->Add(hQCD, -1.); delete hQCD;}

  //store the integral before clipping negative bins
  const double nbefore = hData->Integral(0, hData->GetNbinsX()+1, (density_plot_ > 0) ? "width" : "");

  //set all bins >= 0, including over/underflow
  for(int i = 0; i <= hData->GetNbinsX()+1; ++i) {
    if(hData->GetBinContent(i) < 0.)
      hData->SetBinContent(i,0.);
  }

  //ensure the normalization doesn't change
  const double nafter = hData->Integral(0, hData->GetNbinsX()+1, (density_plot_ > 0) ? "width" : "");

  if(nbefore < 0.) hData->Scale(0.);
  else if(nafter > 0.) hData->Scale(nbefore/nafter);

  const double nmisid = hData->Integral(0, hData->GetNbinsX()+1, (density_plot_ > 0) ? "width" : "");

  const char* stats = (doStatsLegend_) ? Form(": #scale[0.8]{%.2e}", nmisid) : "";
  hData->SetTitle(Form("%s%s",misid_label_.Data(), stats));
  if(fill_alpha_ < 1.) {
    hData->SetLineColorAlpha(misid_color_,fill_alpha_);
    hData->SetFillColorAlpha(misid_color_,fill_alpha_);
  } else {
    hData->SetLineColor(misid_color_);
    hData->SetFillColor(misid_color_);
  }
  return hData;
}

//--------------------------------------------------------------------------------------------------------------------
TH2* DataPlotter::get_misid_2D(TString hist, TString setType, Int_t set) {
  TString hname = Form("MisID_%s_%s_%i", hist.Data(), setType.Data(), set);
  if(density_plot_) hname += "_density";
  {
    auto o = gDirectory->Get(hname.Data());
    if(o) delete o;
  }
  //check if in a loose ID region, where Mis-ID estimate is undefined
  if(set >= misid_offset_) return nullptr;

  //get the data histogram
  const Int_t set_misid = set + misid_offset_;
  TH2* hData = (TH2*) get_data_mc_diff(hist, setType, set_misid);
  if(!hData) return nullptr;

  //redefine the data histogram as the MisID histogram
  hData->SetName(hname.Data());

  if(rebinH_ > 1) {
    hData->RebinX(rebinH_);
    hData->RebinY(rebinH_);
  }

  //include the QCD background subtraction if included
  TH2* hQCD = (include_qcd_) ? get_qcd_2D(hist, setType, set_misid) : nullptr;
  if(hQCD) {hData->Add(hQCD, -1.); delete hQCD;}

  //store the integral before clipping negative bins
  const double nbefore = hData->Integral(0, hData->GetNbinsX()+1,
                                        0, hData->GetNbinsY()+1,
                                         (density_plot_ > 0) ? "width" : "");

  //set all bins >= 0, including over/underflow
  for(int i = 0; i <= hData->GetNbinsX()+1; ++i) {
    for(int j = 0; j <= hData->GetNbinsY()+1; ++i) {
      if(hData->GetBinContent(i,j) < 0.)
        hData->SetBinContent(i,j,0.);
    }
  }

  //ensure the normalization doesn't change
  const double nafter = hData->Integral(0, hData->GetNbinsX()+1,
                                        0, hData->GetNbinsY()+1,
                                        (density_plot_ > 0) ? "width" : "");

  if(nbefore < 0.) hData->Scale(0.);
  else if(nafter > 0.) hData->Scale(nbefore/nafter);

  const double nmisid = hData->Integral(0, hData->GetNbinsX()+1,
                                        0, hData->GetNbinsY()+1,
                                        (density_plot_ > 0) ? "width" : "");

  const char* stats = (doStatsLegend_) ? Form(": #scale[0.8]{%.2e}", nmisid) : "";
  hData->SetTitle(Form("%s%s",misid_label_.Data(), stats));
  return hData;
}

//--------------------------------------------------------------------------------------------------------------------
THStack* DataPlotter::get_stack(TString hist, TString setType, Int_t set, ScaleUncertainty_t* sys_scale) {

  //Get the data-driven backgrounds if applicable
  TH1* hQCD   = (include_qcd_)   ? get_qcd  (hist,setType,set,sys_scale) : nullptr;
  TH1* hMisID = (include_misid_) ? get_misid(hist,setType,set,sys_scale) : nullptr;
  if(hQCD   && verbose_ > 0) std::cout << "QCD histogram has integral " << hQCD->Integral((density_plot_ > 0) ? "width" : "") << std::endl;
  if(hMisID && verbose_ > 0) std::cout << "MisID histogram has integral " << hMisID->Integral((density_plot_ > 0) ? "width" : "") << std::endl;

  //Create the stack
  TString stack_name = Form("stack_%s_%s_%i%s", hist.Data(), setType.Data(), set, (density_plot_) ? "_density" : "");
  {
    auto o = gDirectory->Get(stack_name.Data());
    if(o) delete o;
  }
  THStack* hstack = new THStack(stack_name.Data(),stack_name.Data());

  //Get the background histograms
  std::vector<TH1*> histograms = get_histograms(hist, setType, set, kBackground, "", sys_scale);

  //Loop through the histograms and add them to the stack
  for(unsigned index = 0; index < histograms.size(); ++index) {
    // const int i_color = index % (sizeof(background_colors_) / sizeof(*background_colors_));
    // if(fill_alpha_ < 1.) {
    //   histograms[index]->SetFillColorAlpha(background_colors_[i_color],fill_alpha_);
    //   histograms[index]->SetLineColorAlpha(background_colors_[i_color],fill_alpha_);
    // } else {
    //   histograms[index]->SetFillColor(background_colors_[i_color]);
    //   histograms[index]->SetLineColor(background_colors_[i_color]);
    // }
    histograms[index]->SetLineWidth(2);
    histograms[index]->SetMarkerStyle(20);
    if(rebinH_ > 1) histograms[index]->Rebin(rebinH_);
    const Double_t integral = (histograms[index]->Integral((density_plot_ > 0) ? "width" : "")
                               + histograms[index]->GetBinContent(0)
                               + histograms[index]->GetBinContent(histograms[index]->GetNbinsX()+1));
    const char* stats = (doStatsLegend_) ? Form(": #scale[0.8]{%.2e}", integral) : "";
    const TString label = histograms[index]->GetTitle();
    histograms[index]->SetTitle(Form("%s%s", label.Data(), stats));
    if(verbose_ > 0) std::cout << histograms[index]->GetTitle() << " background histogram has integral "
                               << integral << std::endl;
    hstack->Add(histograms[index]);
  }

  //Add the data-driven histograms if available
  if(hMisID) hstack->Add(hMisID);
  if(hQCD) hstack->Add(hQCD);

  return hstack;
}

//--------------------------------------------------------------------------------------------------------------------
TH2* DataPlotter::get_background_2D(TString hist, TString setType, Int_t set) {
  TString hname = Form("Bkg_%s_%s_%i", hist.Data(), setType.Data(), set);
  if(density_plot_) hname += "_density";
  {
    auto o = gDirectory->Get(hname.Data());
    if(o) delete o;
  }

  //get the MC background histograms
  std::vector<TH1*> backgrounds = get_histograms(hist, setType, set, kBackground);
  TH2* hBackground = nullptr;
  for(unsigned index = 0; index < backgrounds.size(); ++index) {
    if(!hBackground) {
      hBackground = (TH2*) backgrounds[index]->Clone(hname.Data());
    } else {
      hBackground->Add(backgrounds[index]);
    }
    delete backgrounds[index];
  }

  if(!hBackground) {
    std::cout << __func__ << ": No background histogram found!\n";
    return nullptr;
  }

  TH2* hQCD   = (include_qcd_)   ? get_qcd_2D  (hist, setType, set) : nullptr;
  TH2* hMisID = (include_misid_) ? get_misid_2D(hist, setType, set) : nullptr;
  if(hQCD) {hBackground->Add(hQCD); delete hQCD;}
  if(hMisID) {hBackground->Add(hMisID); delete hMisID;}
  hBackground->SetTitle("Bkg");
  return hBackground;
}

//--------------------------------------------------------------------------------------------------------------------
TH1* DataPlotter::get_process(TString process, TString hist, TString setType, Int_t set) {
  std::vector<TH1*> histograms = get_histograms(hist, setType, set, kAny, process);
  if(histograms.size() == 0) {
    printf("DataPlotter:%s: No histogram with process label \"%s\" found!\n", __func__, process.Data());
    return nullptr;
  }
  if(histograms.size() != 1) {
    printf("DataPlotter:%s: Several histograms (%i) with process label \"%s\" found!\n", __func__, (int) histograms.size(), process.Data());
    return nullptr;
  }
  TH1* h = histograms[0];
  h->SetLineWidth(3);
  h->SetFillStyle(0);
  if(rebinH_ > 1) h->Rebin(rebinH_);
  const Double_t integral = (h->Integral((density_plot_ > 0) ? "width" : "")
                             + h->GetBinContent(0)
                             + h->GetBinContent(h->GetNbinsX()+1));
  const char* stats = (doStatsLegend_) ? Form(": #scale[0.8]{%.2e}", integral) : "";
  const TString label = h->GetTitle();
  h->SetTitle(Form("%s%s", label.Data(), stats));
  if(verbose_ > 0) std::cout << h->GetTitle() << " process histogram has integral "
                             << integral << std::endl;

  return h;
}

// //--------------------------------------------------------------------------------------------------------------------
// TH2* DataPlotter::get_signal_2D(TString hist, TString setType, Int_t set) {
//   {
//     auto o = gDirectory->Get("hSignal");
//     if(o) delete o;
//   }
//   TH2* h = 0;
//   for(UInt_t i = 0; i < data_.size(); ++i) {
//     if(isData_[i]) continue;
//     if(!isSignal_[i]) continue;
//     TH2* tmp = (TH2*) data_[i]->Get(Form("%s_%i/%s",setType.Data(), set, hist.Data()));
//     if(!tmp) continue;
//     if(!h) h = tmp;
//     else h->Add(tmp);
//     const char* stats = (doStatsLegend_) ? Form(": #scale[0.8]{%.2e}", h->Integral((density_plot_ > 0) ? "width" : "")
//                                                 +h->GetBinContent(0)+h->GetBinContent(h->GetNbinsX()+1)) : "";

//     h->SetTitle(Form("%s%s", labels_[i].Data(), stats));
//   }
//   if(!h) return nullptr;
//   h->SetLineWidth(2);
//   h->SetLineColor(kAzure-2);
//   h->SetFillColor(kAzure-2);
//   h->SetFillStyle(3001);

//   h->SetName("hSignal");
//   if(rebinH_ > 0) {
//     h->RebinX(rebinH_);
//     h->RebinY(rebinH_);
//   }

//   return h;
// }

//--------------------------------------------------------------------------------------------------------------------
TH1* DataPlotter::get_stack_uncertainty(THStack* hstack, TString hname) {
  if(!hstack || hstack->GetNhists() == 0)
    return nullptr;
  TH1* hlast = (add_bkg_hists_manually_) ? (TH1*) hstack->GetHists()->Last() : (TH1*) hstack->GetStack()->Last();
  if(!hlast) return nullptr;
  //clone last histogram to match the setup
  TH1* huncertainty = (TH1*) hlast->Clone(hname.Data());
  huncertainty->SetDirectory(0);
  if(add_bkg_hists_manually_) {huncertainty->Clear(); huncertainty->Reset();}
  huncertainty->SetTitle("Bkg #pm#sigma(Stat)");
  huncertainty->SetName(hname.Data());
  huncertainty->SetFillColor(kGray+1);
  huncertainty->SetLineColor(kGray+1);
  huncertainty->SetFillStyle(3001);
  huncertainty->SetMarkerSize(0.); //so no marker
  if(add_bkg_hists_manually_) {
    for(TObject* o : *(hstack->GetHists())) {
      if(o->InheritsFrom(TH1::Class())) {
        huncertainty->Add((TH1*) o);
      }
    }
  }
  return huncertainty;
}

// //--------------------------------------------------------------------------------------------------------------------
// TCanvas* DataPlotter::plot_single_2Dhist(TString hist, TString setType, Int_t set, TString label) {

//   TH2* h = 0; //histogram

//   {
//     auto o = gDirectory->Get(Form("h2d_%s_%i",hist.Data(),set));
//     if(o) delete o;
//   }

//   TString name = label;
//   name.ReplaceAll("#", "");
//   name.ReplaceAll("->", "");
//   TCanvas* c = new TCanvas(Form("c_2D_%s_%s_%i",name.Data(),hist.Data(),set),Form("c_2D_%s_%s_%i",name.Data(),hist.Data(),set), canvas_x_, canvas_y_);
//   c->SetTopMargin(0.06);
//   c->SetRightMargin(single_2d_right_margin_);
//   c->SetLeftMargin(0.087);

//   if(label == "MisID") {
//     h = get_misid_2D(hist, setType, set);
//   } else if(label == "QCD") {
//     h = get_qcd_2D(hist, setType, set);
//   } else {
//     for(UInt_t i = 0; i < data_.size(); ++i) {
//       if(label != labels_[i]) continue;

//       //get histogram
//       TH2* htmp = (TH2*) data_[i]->Get(Form("%s_%i/%s",setType.Data(), set, hist.Data()));
//       if(!htmp) {printf("No hist %s in %s, continuing\n",hist.Data(), fileNames_[i].Data());continue;}
//       htmp = (TH2*) htmp->Clone("tmp");
//       //scale to cross section and luminosity
//       htmp->Scale(scale_[i]);
//       if(rebinH_ > 1) {
//         htmp->RebinX(rebinH_);
//         htmp->RebinY(rebinH_);
//       }
//       if(h) {
//         h->Add(htmp);
//         delete htmp;
//       }
//       else h = htmp;

//     }
//   }
//   if(!h) {
//     std::cout << "Failed to find histogram with label " << label.Data() << std::endl;
//     return nullptr;
//   }
//   // h->SetBit(kCanDelete);
//   const char* stats = (doStatsLegend_) ? Form(": #scale[0.8]{%.2e}", h->Integral()
//                                               +h->GetBinContent(0)+h->GetBinContent(h->GetNbinsX()+1)) : "";
//   TH2* data = 0;
//   if(plot_data_) {
//     data = get_data_2D(hist, setType, set);
//     if(!data) {
//       printf("%s: Warning! Data not found!\n", __func__);
//     } else {
//       gStyle->SetPalette(kRainBow);
//       if(normalize_2ds_ && data->Integral() > 0. && logZ_) {
//              data->Scale(1./data->Integral());
//       }
//       // data->Scale(1./data->GetMaximum());
//       data->Draw(single_2d_data_style_.Data());
//     }
//     if(normalize_2ds_&&h->Integral() > 0.) {
//       if(logZ_) {
//         // h->Scale(2.*data->GetMaximum()/h->GetMaximum());
//         data->GetZaxis()->SetRangeUser(1.e-5, 1.5);
//         // h->GetZaxis()->SetRangeUser(1.e-6, 0.1);
//       } else {
//         Double_t mx      = h->GetMaximum();
//         Double_t data_mx = data->GetMaximum();
//         h->GetZaxis()->SetRangeUser(1.e-3*std::min(mx,data_mx),std::max(mx, data_mx)*1.e2);
//         data->GetZaxis()->SetRangeUser(1.e-3*std::min(mx, data_mx),1.e2*std::max(mx,data_mx));
//       }
//     }
//   } else {
//     gStyle->SetPalette(kRainBow);
//     Double_t mx      = h->GetMaximum();
//     if(logZ_)
//       h->GetZaxis()->SetRangeUser(1.e-4*mx, 2.*mx);
//     else
//       h->GetZaxis()->SetRangeUser(1.e-2,1.2*mx);
//   }

//   // if(data) data->SetBit(kCanDelete);
//   TH2* hAxis = (plot_data_ && data) ? data : h;
//   h->SetTitle(Form("%s%s", label.Data(),stats));
//   if(plot_data_) {
//     h->SetLineColor(single_2d_color_);
//     if(fill_alpha_ < 1.)
//       h->SetMarkerColorAlpha(single_2d_color_,0.5);
//     else
//       h->SetMarkerColor(single_2d_color_);
//     h->SetMarkerStyle(single_2d_marker_style_);
//     h->SetMarkerSize(single_2d_marker_size_);
//     h->SetContour(single_2d_ncontour_);
//     h->SetLineWidth(single_2d_linewidth_);
//     // h->RebinX(2); h->RebinY(2);
//   }
//   h->SetName(Form("h2D_%s_%s_%i",label.Data(),hist.Data(),set));
//   if(plot_data_ && data) h->Draw(Form("same %s", single_2d_style_.Data()));
//   else                   h->Draw("colz");

//   //get axis titles
//   TString xtitle;
//   TString ytitle;
//   TString title;
//   Titles::get_titles(hist,setType,selection_,&xtitle,&ytitle,&title);

//   c->SetGrid();
//   if(single_2d_legend_) c->BuildLegend(); //0.6, 0.9, 0.9, 0.45, "", "L");
//   if(yMin_ <= yMax_)hAxis->GetYaxis()->SetRangeUser(yMin_,yMax_);
//   if(xMin_ <= xMax_)hAxis->GetXaxis()->SetRangeUser(xMin_,xMax_);
//   //draw text on plots
//   draw_luminosity(true);
//   draw_cms_label(true);
//   hAxis->SetXTitle(xtitle.Data());
//   hAxis->SetYTitle(ytitle.Data());
//   hAxis->GetXaxis()->SetTitleSize(0.05);
//   hAxis->GetYaxis()->SetTitleSize(0.05);
//   hAxis->GetXaxis()->SetTitleOffset(0.8);
//   hAxis->GetYaxis()->SetTitleOffset(0.8);
//   if(plot_title_) hAxis->SetTitle (title.Data());
//   else            hAxis->SetTitle (""); //no title, overwrite current with empty string
//   if(logY_) c->SetLogy();
//   if(logZ_) c->SetLogz();
//   return c;

// }

// //--------------------------------------------------------------------------------------------------------------------
// TCanvas* DataPlotter::plot_2Dhist(TString hist, TString setType, Int_t set) {

//   std::vector<TH2*> h; //list of histograms
//   //check if QCD is defined for this set
//   //  TH2* hQCD = (include_qcd_) ? get_qcd(hist,setType,set) : nullptr;

//   //array of colors and fills for each label
//   Int_t color[] = {kRed+1, kYellow+1,kYellow+3,kBlue+1, kRed+3, kViolet-2, kGreen-2, kOrange-9};
//   //  Int_t fill[]  = {1001,3005,1001,1001,3005,1001,1001,1001};

//   {
//     auto o = gDirectory->Get(Form("h2d_%s_%i",hist.Data(),set));
//     if(o) delete o;
//   }

//   TCanvas* c = new TCanvas(Form("h2d_%s_%i",hist.Data(),set),Form("h2D_%s_%i",hist.Data(),set), canvas_x_, canvas_y_);
//   c->SetTopMargin(0.06);
//   c->SetRightMargin(single_2d_right_margin_);
//   c->SetLeftMargin(0.087);

//   //maximum z value of histograms, for plotting
//   //  double m = 0.;

//   //for combining histograms of the same process
//   std::map<TString, int> indexes;

//   std::map<TString, int> colors; //map label to index for the color array
//   int n_colors = 0; //number of colors used so far

//   for(UInt_t i = 0; i < data_.size(); ++i) {
//     //push null to not mess up indexing
//     if(isData_[i]) {h.push_back(nullptr);continue;}

//     //get histogram
//     TH2* htmp = (TH2*) data_[i]->Get(Form("%s_%i/%s",setType.Data(), set, hist.Data()));
//     if(!htmp) {printf("No hist %s in %s, continuing\n",hist.Data(), fileNames_[i].Data());continue;}
//     h.push_back((TH2*) htmp->Clone("tmp"));
//     //scale to cross section and luminosity
//     h[i]->Scale(scale_[i]);

//     if(rebinH_ > 1) {
//       h[i]->RebinX(rebinH_);
//       h[i]->RebinY(rebinH_);
//     }
//     //if the first, add to map, else get first of this label
//     int index = i;
//     int i_color = n_colors;
//     if(indexes.count(labels_[i])) {
//       index = indexes[labels_[i]];
//       i_color = colors[labels_[i]];
//     } else {
//       indexes.insert({labels_[i], i});
//       colors.insert({labels_[i], n_colors});
//       n_colors++;
//     }
//     //if not first, add this to first histogram of this label
//     TString name = labels_[index];
//     if(index != (int)i) {
//       h[index]->Add(h[i]);
//       delete h[i];
//     } else {
//       //set plotting info
//       h[index]->SetLineColor(color[i_color]);
//       h[index]->SetMarkerColor(color[i_color]);
//       h[index]->SetName(Form("h2D_%s_%s_%i",name.Data(),hist.Data(),set));
//     }
//     const char* stats = (doStatsLegend_) ? Form(": #scale[0.8]{%.2e}", h[index]->Integral()
//                                                 +h[index]->GetBinContent(0)+h[index]->GetBinContent(h[index]->GetNbinsX()+1)) : "";
//     h[index]->SetTitle(Form("%s%s", name.Data(), stats));
//   }
//   //plot each histogram, remember which is first for axis setting
//   bool first = true;
//   int ind = 0;
//   auto it = indexes.begin();
//   while(it != indexes.end()) {
//     if(normalize_2ds_ && h[it->second]->Integral() > 0.) h[it->second]->Scale(1./h[it->second]->Integral());
//     if(first) {
//       h[it->second]->Draw("");
//       ind = it->second;
//     }
//     else
//       h[it->second]->Draw("same");
//     first = false;
//     it++;
//   }
//   //plot QCD
//   //  if(h.size() == 0 && hQCD) hQCD->Draw("hist");
//   //  else if(hQCD) hQCD->Draw("hist same");

//   //get axis titles
//   TString xtitle;
//   TString ytitle;
//   TString title;
//   Titles::get_titles(hist,setType,selection_,&xtitle,&ytitle,&title);

//   c->SetGrid();
//   c->BuildLegend();//0.6, 0.9, 0.9, 0.45, "", "L");
//   auto o = c->GetPrimitive("TPave");
//   if(o) {
//     auto tl = (TLegend*) o;
//     tl->SetDrawOption("L");
//     tl->SetTextSize(legend_txt_);
//     tl->SetY2NDC(legend_y2_);
//     tl->SetY1NDC(legend_y1_);
//     tl->SetX1NDC(((doStatsLegend_) ? legend_x1_stats_ : legend_x1_));
//     tl->SetX2NDC(legend_x2_);
//     tl->SetEntrySeparation(legend_sep_);
//     tl->SetFillColor(0);
//     tl->SetLineWidth(0);
//     c->Update();
//   }

//   TH2* hAxis = (h.size() > 0) ? h[ind] : 0;
//   if(!hAxis) return nullptr;

//   if(yMin_ <= yMax_)hAxis->GetYaxis()->SetRangeUser(yMin_,yMax_);
//   if(xMin_ <= xMax_)hAxis->GetXaxis()->SetRangeUser(xMin_,xMax_);
//   //draw text on plots
//   draw_luminosity();
//   draw_cms_label();
//   hAxis->SetXTitle(xtitle.Data());
//   hAxis->SetYTitle(ytitle.Data());
//   hAxis->GetXaxis()->SetTitleSize(axis_font_size_);
//   hAxis->GetYaxis()->SetTitleSize(axis_font_size_);
//   if(plot_title_) hAxis->SetTitle (title.Data());
//   else hAxis->SetTitle (""); //no title, overwrite current with empty string
//   if(logY_) c->SetLogy();
//   if(logZ_) c->SetLogz();
//   return c;

// }

// //--------------------------------------------------------------------------------------------------------------------
// TCanvas* DataPlotter::plot_hist(TString hist, TString setType, Int_t set) {

//   std::vector<TH1*> h; //list of histograms
//   //check if QCD is defined for this set
//   TH1* hQCD = (include_qcd_) ? get_qcd(hist,setType,set) : nullptr;
//   if(hQCD) hQCD->SetFillStyle(0);
//   TH1* hMisID = (include_misid_) ? get_misid(hist,setType,set) : nullptr;
//   if(hMisID) hMisID->SetFillStyle(0);
//   // if(hQCD) hQCD->SetBit(kCanDelete);
//   //array of colors and fills for each label
//   Int_t bkg_color[] = {kRed+1, kRed-2, kYellow+1,kSpring-1 , kViolet-2, kGreen-2, kRed+3,kOrange-9,kBlue+1};
//   Int_t bkg_fill[]  = {     0,      0,         0,         0,         0,        0,      0,        0,      0};//1001,3005,1001,1001,3005,1001,1001,1001};
//   Int_t sig_color[] = {kBlue, kOrange+10, kGreen+4, kViolet-2, kYellow+3,kOrange-9,kBlue+1};
//   Int_t sig_fill[]  = {0,          0,          0,         0,        0,         0,       0};//3002,3001,3003,3003,3005,3006,3003,3003};
//   TLegend* leg = new TLegend((doStatsLegend_) ? legend_x1_stats_ : legend_x1_, legend_y1_, legend_x2_, legend_y2_);
//   if(hQCD) leg->AddEntry(hQCD, hQCD->GetTitle(), "L");
//   if(hMisID) leg->AddEntry(hMisID, hMisID->GetTitle(), "L");
//   // leg->SetDrawOption("L");
//   leg->SetBorderSize(0);
//   leg->SetTextSize(legend_txt_);
//   // leg->SetEntrySeparation(legend_sep_);
//   {
//     auto o = gDirectory->Get(Form("h_%s_%i",hist.Data(),set));
//     if(o) delete o;
//   }
//   TCanvas* c = new TCanvas(Form("h_%s_%i",hist.Data(),set),Form("h_%s_%i",hist.Data(),set), canvas_x_, canvas_y_);

//   //maximum y value of histograms, for plotting
//   double m = 0.;

//   //for combining histograms of the same process
//   std::map<TString, int> indexes;

//   std::map<TString, int> colors; //map label to index for the color array
//   int n_bkg_colors = 0; //number of bkg colors used so far
//   int n_sig_colors = 0; //number of sig colors used so far

//   for(UInt_t i = 0; i < data_.size(); ++i) {
//     //push null to not mess up indexing
//     if(isData_[i]) {h.push_back(nullptr);continue;}

//     //get histogram
//     h.push_back((TH1*) data_[i]->Get(Form("%s_%i/%s",setType.Data(), set, hist.Data())));
//     if(!h[i]) {printf("No hist %s in %s, continuing\n",hist.Data(), fileNames_[i].Data());continue;}
//     h[i] = (TH1*) h[i]->Clone("htmp");
//     h[i]->SetDirectory(0);
//     //scale to cross section and luminosity
//     h[i]->Scale(scale_[i]);
//     if(rebinH_ > 0) h[i]->Rebin(rebinH_);

//     //if the first, add to map, else get first of this label
//     int index = i;
//     int i_color = (isSignal_[i]) ? n_sig_colors : n_bkg_colors;
//     if(indexes.count(labels_[i])) {
//       index = indexes[labels_[i]];
//       i_color = colors[labels_[i]];
//     } else {
//       indexes.insert({labels_[i], i});
//       colors.insert({labels_[i], (isSignal_[i] ? n_sig_colors : n_bkg_colors)});
//       if(isSignal_[i])
//         n_sig_colors++;
//       else
//         n_bkg_colors++;
//     }
//     //if not first, add this to first histogram of this label
//     TString name = labels_[index];
//     if(index != (int)i) {
//       h[index]->Add(h[i]);
//       delete h[i];
//     } else {
//       //set plotting info
//       {
//         auto o = gDirectory->Get(Form("h_%s_%s_%i",name.Data(),hist.Data(),set));
//         if(o) delete o;
//       }
//       if(isSignal_[i]) {
//         h[index]->SetFillStyle(sig_fill[i_color]);
//         h[index]->SetFillColor(sig_color[i_color]);
//         if(fill_alpha_ < 1.)
//           h[index]->SetFillColorAlpha(sig_color[i_color],fill_alpha_);
//         else
//           h[index]->SetFillColor(sig_color[i_color]);
//         h[index]->SetLineColor(sig_color[i_color]);
//         h[index]->SetLineWidth(4);
//       } else {
//         h[index]->SetFillStyle(bkg_fill[i_color]);
//         if(fill_alpha_ < 1.)
//           h[index]->SetFillColorAlpha(bkg_color[i_color],fill_alpha_);
//         else
//           h[index]->SetFillColor(bkg_color[i_color]);
//         h[index]->SetLineColor(bkg_color[i_color]);
//         h[index]->SetLineWidth(3);
//       }
//       h[index]->SetMarkerStyle(20);
//       h[index]->SetName(Form("h_%s_%s_%i",name.Data(),hist.Data(),set));
//     }
//     const char* stats = (doStatsLegend_) ? Form(": #scale[0.8]{%.2e}", h[index]->Integral((density_plot_ > 0) ? "width" : "")
//                                                 +h[index]->GetBinContent(0)+h[index]->GetBinContent(h[index]->GetNbinsX()+1)) : "";
//     Double_t signal_scale = signal_scale_;
//     if(signal_scales_.find(labels_[index]) != signal_scales_.end()) signal_scale = signal_scales_[labels_[index]];
//     if(isSignal_[index])
//       h[index]->SetTitle(Form("%s%s%s", name.Data(),
//                               (signal_scale == 1. || normalize_1ds_) ? "" : Form(" (x%.1f)",signal_scale),
//                               stats));
//     else
//       h[index]->SetTitle(Form("%s%s", name.Data(),stats));
//     if(isSignal_[i] && signal_scale > 1.) h[i]->Scale(signal_scale);
//     if((int) i == index) leg->AddEntry(h[index], h[index]->GetTitle(), "L");
//   }
//   //plot each histogram, remember which is first for axis setting
//   bool first = true;
//   int ind = 0;
//   auto it = indexes.begin();
//   while(it != indexes.end()) {
//     if(h[it->second]->GetEntries() == 0) continue;
//     if(first) {
//       if(normalize_1ds_) h[it->second]->Scale(1./h[it->second]->Integral());
//       m = std::max(m,h[it->second]->GetMaximum());
//       h[it->second]->Draw("hist");
//       ind = it->second;
//       first = false;
//     }
//     else {
//       if(normalize_1ds_) h[it->second]->Scale(1./h[it->second]->Integral());
//       m = std::max(m,h[it->second]->GetMaximum());
//       h[it->second]->Draw("same hist");
//     }
//     it++;
//   }
//   //plot QCD
//   if(hQCD && normalize_1ds_) hQCD->Scale(1./hQCD->Integral());
//   if(h.size() == 0 && hQCD) hQCD->Draw("hist");
//   else if(hQCD) hQCD->Draw("hist same");
//   if(hQCD) m = std::max(m,hQCD->GetMaximum());
//   if(hMisID && normalize_1ds_) hMisID->Scale(1./hMisID->Integral());
//   if(h.size() == 0 && hMisID) hMisID->Draw("hist");
//   else if(hMisID) hMisID->Draw("hist same");
//   if(hMisID) m = std::max(m,hMisID->GetMaximum());

//   //get axis titles
//   TString xtitle;
//   TString ytitle;
//   TString title;
//   Titles::get_titles(hist,setType,selection_,&xtitle,&ytitle,&title);

//   c->SetGrid();
//   c->SetTopMargin(0.06);
//   c->SetRightMargin(0.05);
//   c->SetLeftMargin(0.087);
//   // c->BuildLegend();//0.6, 0.9, 0.9, 0.45, "", "L");

//   TH1* hAxis = (h.size() > 0) ? h[ind] : hQCD;
//   if(!hAxis) return nullptr;

//   if(yMin_ <= yMax_)hAxis->SetAxisRange(yMin_,yMax_,"Y");
//   else              hAxis->SetAxisRange((normalize_1ds_) ? m*1.e-4 : 1.e-1, m*1.2,"Y");
//   if(xMin_ < xMax_) hAxis->SetAxisRange(xMin_,xMax_,"X");
//   //draw text on plots
//   draw_luminosity();
//   draw_cms_label();
//   hAxis->SetXTitle(xtitle.Data());
//   hAxis->GetXaxis()->SetTitleSize(axis_font_size_);
//   hAxis->GetYaxis()->SetTitleSize(axis_font_size_);
//   if(plot_y_title_) hAxis->SetYTitle(ytitle.Data());
//   if(plot_title_) hAxis->SetTitle (title.Data());
//   else hAxis->SetTitle (""); //no title, overwrite current with empty string
//   if(logY_) c->SetLogy();
//   // auto o = c->GetPrimitive("TPave");
//   // if(o) {
//   //   c->Modified();
//   //   c->Update();
//   //   auto tl = (TLegend*) o;
//   //   tl->SetDrawOption("L");
//   //   tl->SetTextSize(legend_txt_);
//   //   tl->SetY2NDC(legend_y2_);
//   //   tl->SetY1NDC(legend_y1_);
//   //   tl->SetX1NDC(((doStatsLegend_) ? legend_x1_stats_ : legend_x1_));
//   //   tl->SetX2NDC(legend_x2_);
//   //   tl->SetEntrySeparation(legend_sep_);
//   //   c->Modified();
//   //   c->Update();
//   // } else {
//   //   printf("Warning! Legend not found\n");
//   // }
//   leg->Draw();
//   return c;

// }

//--------------------------------------------------------------------------------------------------------------------
TCanvas* DataPlotter::plot_stack(TString hist, TString setType, Int_t set) {
  if(verbose_ > 0) std::cout << "Plotting stack with hist = " << hist.Data()
                             << " type = " << setType.Data() << " and set = "
                             << set << std::endl;
  //get stack and data histogram
  THStack* hstack = get_stack(hist,setType,set);
  if(!hstack || hstack->GetNhists() <= 0) {
    printf("Null or empty stack! %s/%s/%i\n", hist.Data(), setType.Data(), set);
    return nullptr;
  }
  if(verbose_ > 0) std::cout << "Retrieved the stack with " << hstack->GetNhists() << " histograms\n";

  // hstack->SetBit(kCanDelete);
  TH1* d = get_data(hist, setType, set);
  if(verbose_ > 0 && !d) std::cout << "Failed to retrieve the data histogram\n";
  else if(verbose_ > 0)  std::cout << "Retrieved the data histogram\n";

  std::vector<TH1*> hsignal = get_signal(hist,setType,set);
  if(verbose_ > 0) {
    std::cout << "Retrieved the signals with " << hsignal.size() << " histograms\n";
  }
  {
    auto o = gDirectory->Get(Form("s_%s_%i",hist.Data(),set));
    if(o) delete o;
  }

  ////////////////////////////////////////////////////
  // Configure the canvas
  ////////////////////////////////////////////////////

  TCanvas* c = new TCanvas(Form("s_%s_%i%s",hist.Data(),set, (density_plot_) ? "_density" : ""),
                           Form("s_%s_%i%s",hist.Data(),set, (density_plot_) ? "_density" : ""),
                           canvas_x_, canvas_y_);
  //split the top into main stack and bottom into Data/MC if plotting data
  TPad *pad1 = nullptr, *pad2 = nullptr;

  //Add axis tick marks to all sides of the pads
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);

  //Normalize to the stack integral if normalizing, for simplicity
  if(d && normalize_1ds_ && d->Integral() > 0.) {
    d->Scale(((TH1*)hstack->GetStack()->Last())->Integral((density_plot_ > 0) ? "width" : "") / d->Integral((density_plot_ > 0) ? "width" : ""));
  }
  if(normalize_1ds_) {
    for(TH1* hsig : hsignal) {
      if(hsig->Integral() > 0.)
        hsig->Scale(((TH1*)hstack->GetStack()->Last())->Integral((density_plot_ > 0) ? "width" : "") / hsig->Integral((density_plot_ > 0) ? "width" : ""));
    }
  }
  const bool plot_single = plot_data_ == 0 && data_over_mc_ >= 0; //single plot canvas

  if((plot_data_ && d && data_over_mc_) || (plot_data_ == 0 && data_over_mc_ < 0)) {
    pad1 = new TPad("pad1","pad1",upper_pad_x1_, upper_pad_y1_, upper_pad_x2_, upper_pad_y2_); //xL yL xH xH, (0,0) = bottom left
    pad2 = new TPad("pad2","pad2",lower_pad_x1_, lower_pad_y1_, lower_pad_x2_, lower_pad_y2_);
    pad1->SetTopMargin(upper_pad_topmargin_);
    if(data_over_mc_ < 0)
      pad2->SetTopMargin(lower_pad_sigbkg_topmargin_);
    else
      pad2->SetTopMargin(lower_pad_topmargin_);
    pad1->SetBottomMargin(upper_pad_botmargin_);
    pad2->SetBottomMargin(lower_pad_botmargin_);
    pad1->Draw();
    pad2->Draw();
  } else { //only plot the stack, no ratio plot
    pad1 = new TPad("pad1","pad1",0.0,0.0,1,1); //xL yL xH xH, (0,0) = bottom left
    pad1->SetTopMargin(upper_pad_topmargin_);
    pad1->Draw();
  }
  pad1->cd();
  //store maximum of stack/Data
  double max_val(1.e-5), min_val(1.e20);


  //get axis titles
  TString xtitle;
  TString ytitle;
  TString title;
  Titles::get_titles(hist,setType,selection_,&xtitle,&ytitle,&title);
  if(stack_signal_) {
    for(unsigned int i = 0; i < hsignal.size(); ++i) {
      if(hsignal[i]) hstack->Add(hsignal[i]);
    }
  }
  TH1* hstack_hist = nullptr;
  if(stack_as_hist_) {
    hstack_hist = (TH1*) hstack->GetStack()->Last()->Clone("hBackground");
    hstack_hist->SetDirectory(0);
    hstack_hist->SetFillStyle(3002);
    hstack_hist->SetFillColor(total_background_color_);
    hstack_hist->SetLineColor(total_background_color_);
    hstack_hist->SetTitle("Background");
    hstack_hist->Draw("hist");
  } else {
    //draw stack, preserving style set for each histogram
    hstack->Draw("hist noclear");
  }

  if(!stack_signal_) {
    for(unsigned int i = 0; i < hsignal.size(); ++i) {
      if(hsignal[i] && hsignal[i]->GetEntries() > 0) {
        hsignal[i]->Draw("hist same");
        max_val = std::max(max_val, Utilities::H1Max(hsignal[i], xMin_, xMax_));
        min_val = std::min(min_val, Utilities::H1Min(hsignal[i], xMin_, xMax_));
      }
    }
  }

  TH1* huncertainty = get_stack_uncertainty(hstack,Form("uncertainty_%s_%i", hist.Data(), set));
  if(stack_uncertainty_)
    huncertainty->Draw("E2 SAME");

  int ndata = 0.;
  double nmc = 0.;
  std::map<TString, double> nsig;
  if(d && plot_data_) {
    ndata = d->Integral((density_plot_ > 0) ? "width" : "") + d->GetBinContent(0)+d->GetBinContent(d->GetNbinsX()+1);
  }
  for(unsigned i = 0; i < hsignal.size(); ++i) {
    if(!doStatsLegend_ && hsignal[i]->GetEntries() > 0) {
      Double_t signal_scale = signal_scale_;
      if(signal_scales_.find(hsignal[i]->GetName()) != signal_scales_.end()) {
        signal_scale = signal_scales_[hsignal[i]->GetName()];
      }
      nsig[hsignal[i]->GetName()] = (hsignal[i]->Integral((density_plot_ > 0) ? "width" : "") + hsignal[i]->GetBinContent(0)+hsignal[i]->GetBinContent(hsignal[i]->GetNbinsX()+1))/signal_scale;
    }
  }
  nmc = huncertainty->Integral((density_plot_ > 0) ? "width" : "") + huncertainty->GetBinContent(0)+huncertainty->GetBinContent(huncertainty->GetNbinsX()+1);

  //blind if needed
  if(blindxmin_.size() > 0 && d && plot_data_) {
    const unsigned nbins = d->GetNbinsX();
    for(unsigned bin = 1; bin <= nbins; ++bin) {
      const double binlow = d->GetBinLowEdge(bin);
      const double binhigh = binlow + d->GetBinWidth(bin);
      if(isBlind(binlow, binhigh))
        d->SetBinContent(bin, 0.);
    }
  }

  //draw the data with error bars
  if(plot_data_ && d) d->Draw("E same");

  if(d && plot_data_ > 0) {
    max_val = std::max(max_val, Utilities::H1Max(d, xMin_, xMax_));
    min_val = std::min(min_val, Utilities::H1Min(d, xMin_, xMax_));
  }
  max_val = std::max(max_val, Utilities::H1Max((TH1*) hstack->GetStack()->Last(), xMin_, xMax_));
  min_val = std::min(min_val, Utilities::H1Min((TH1*) hstack->GetStack()->Last(), xMin_, xMax_));

  // pad1->BuildLegend();//0.6, 0.9, 0.9, 0.45, "", "L");
  pad1->SetGrid();
  //draw text
  if(draw_statistics_) draw_data(ndata,nmc,nsig);
  draw_luminosity(plot_single);
  draw_cms_label(plot_single);

  TLegend* leg = new TLegend((doStatsLegend_ || plot_single) ? legend_x1_stats_ : legend_x1_, legend_y1_, legend_x2_, legend_y2_);
  leg->SetTextSize((doStatsLegend_ || plot_single) ? 0.75*legend_txt_ : legend_txt_);
  leg->SetNColumns(legend_ncol_);
  if(d) leg->AddEntry(d, d->GetTitle(), "PL");
  if(stack_uncertainty_) leg->AddEntry(huncertainty, huncertainty->GetTitle(), "F");
  if(!stack_signal_) {
    for(unsigned int i = 0; i < hsignal.size(); ++i) {
      leg->AddEntry(hsignal[i], hsignal[i]->GetTitle(), "FL");
    }
  }
  for(int i = 0; i < hstack->GetNhists(); ++i) {
    TH1* h = (TH1*) hstack->GetHists()->At(i);
    leg->AddEntry(h, h->GetTitle(), "F");
  }
  // leg->SetEntrySeparation(legend_sep_);
  leg->SetFillStyle(0);
  leg->SetFillColor(0);
  leg->SetLineColor(0);
  leg->SetLineStyle(0);
  leg->Draw();

  pad1->Update();

  ////////////////////////////////////////////////////////////////
  //Make a Data/MC histogram
  ////////////////////////////////////////////////////////////////

  if(plot_data_ && !d) {
    printf("Warning! Data histogram is Null! Skipping Data/MC plot\n");
  }
  TH1* hDataMC = nullptr;
  if(plot_data_ && d && data_over_mc_ > 0)
    hDataMC = (TH1*) d->Clone("hDataMC");
  else if(data_over_mc_ < 0) //make signal/background histograms instead
    hDataMC = (TH1*) hstack->GetStack()->Last()->Clone("hRatio");
  if(hDataMC) hDataMC->SetDirectory(0);
  TGraphErrors* hDataMCErr = nullptr;
  int nb = (hDataMC) ? hDataMC->GetNbinsX() : -1;
  std::vector<TH1*> hSignalsOverMC;
  if(hDataMC && data_over_mc_ > 0) {
    hDataMC->Clear();
    hDataMC->SetName("hDataMC");
    hDataMC->SetTitle("");
    double x[nb];
    double y[nb];
    double xerr[nb];
    double yerr[nb];
    for(int i = 1; i <= nb; ++i) {
      double dataVal = 0;
      double dataErr = 0;
      dataVal = d->GetBinContent(i);
      dataErr= d->GetBinError(i);
      dataErr = sqrt(dataVal);
      double mcVal = huncertainty->GetBinContent(i);
      double mcErr = huncertainty->GetBinError(i);
      x[i-1] = hDataMC->GetBinCenter(i);
      y[i-1] = 1.;
      xerr[i-1] = huncertainty->GetBinWidth(i)/2.;
      yerr[i-1] = (mcVal > 0.) ? mcErr/mcVal : 0.;
      if(dataVal == 0 || mcVal == 0) {hDataMC->SetBinContent(i,-1); continue;}
      // double err = sqrt(mcErr*mcErr + dataErr*dataErr);
      double errRatio = (dataVal/mcVal)*(dataVal/mcVal)*(dataErr*dataErr/(dataVal*dataVal));
      errRatio = sqrt(errRatio);
      double ratio = dataVal/mcVal;
      hDataMC->SetBinContent(i,ratio);
      hDataMC->SetBinError(i,errRatio);
    }
    hDataMCErr = new TGraphErrors(nb, x, y, xerr, yerr);
    hDataMCErr->SetFillStyle(3001);
    hDataMCErr->SetFillColor(kGray+1);
  } else if(hDataMC && data_over_mc_ < 0) {
    hDataMC->SetName("hRatio");
    for(TH1* h : hsignal) {
      hSignalsOverMC.push_back((TH1*) h->Clone(Form("%s_over_mc", h->GetName())));
      hSignalsOverMC.back()->SetDirectory(0);
    }
    for(TH1* h : hSignalsOverMC) {
      if(data_over_mc_ == -2) //~significance squared = sig*sig / data
        h->Multiply(h);
      h->Divide(hDataMC);
    }
  }

  ////////////////////////////////////////////////////////////////
  // Configure axes and titles
  ////////////////////////////////////////////////////////////////

  TAxis *top_xaxis(nullptr), *top_yaxis(nullptr), *bot_xaxis(nullptr);
  if(pad1) {pad1->Modified(); pad1->Update();} if(pad2) {pad2->Modified(); pad2->Update();} c->Modified(); c->Update();
  if(hDataMC) {
    bot_xaxis = hDataMC->GetXaxis();
  }
  if(hstack_hist) {
    top_xaxis = hstack_hist->GetXaxis();
    top_yaxis = hstack_hist->GetYaxis();
  } else {
    top_xaxis = hstack->GetXaxis();
    top_yaxis = hstack->GetYaxis();
  }
  if(bot_xaxis) bot_xaxis->SetTitle(xtitle.Data());
  else if(top_xaxis) top_xaxis->SetTitle(xtitle.Data());
  else {
    std::cout << "Stack x-axis not defined to make axis title!\n";
    return c;
  }
  if(top_yaxis) top_yaxis->SetTitle(ytitle.Data());
  else {
    std::cout << "Stack y-axis not defined to make axis title!\n";
    return c;
  }

  if(!top_xaxis || !top_yaxis) {
    std::cout << "Upper pad axes are not defined!\n";
    return c;
  }

  top_xaxis->SetTitleSize(axis_font_size_);
  top_yaxis->SetTitleSize(axis_font_size_);
  double ymin = yMin_;
  double ymax = yMax_;
  if(ymin > ymax) {
    ymin = std::max(0.8, min_val*((logY_) ? 1./log_buffer_ : 1./linear_buffer_));
    if(logY_) {
      ymax = (max_val < ymin) ? log_buffer_*ymin : ymin*std::pow(10, log_buffer_*std::log10(max_val/ymin));
    } else {
      ymax = linear_buffer_*std::max(max_val, ymin);
    }
  }
  top_yaxis->SetRangeUser(ymin, ymax);
  if(xMin_ < xMax_) top_xaxis->SetRangeUser(xMin_,xMax_);

  if(hstack_hist) {
    if(plot_title_) hstack_hist->SetTitle (title.Data());
    else hstack_hist->SetTitle("");
    hstack_hist->SetMinimum(ymin);
    hstack_hist->SetMaximum(ymax);
  } else {
    if(plot_title_) hstack->SetTitle (title.Data());
    else hstack->SetTitle("");
    hstack->SetMinimum(ymin);
    hstack->SetMaximum(ymax);
  }

  if(xMin_ < xMax_ && hDataMC) hDataMC->GetXaxis()->SetRangeUser(xMin_,xMax_);

  if(logY_) {
    pad1->SetLogy();
  }
  c->SetGrid();
  if(plot_data_ && hDataMC && data_over_mc_ > 0) {
    pad2->cd();
    pad2->SetGrid();
    c->SetGrid();
    hDataMC->Draw("E");
    TLine* line = new TLine((xMax_ < xMin_) ? hDataMC->GetBinCenter(1)-hDataMC->GetBinWidth(1)/2. : xMin_, 1.,
                            (xMax_ < xMin_) ? hDataMC->GetBinCenter(hDataMC->GetNbinsX())+hDataMC->GetBinWidth(1)/2. : xMax_, 1.);
    line->SetLineColor(kRed);
    line->Draw("same");

    hDataMC->GetYaxis()->SetTitle("Data/MC");
    hDataMC->GetXaxis()->SetTitleSize(axis_font_size_);
    hDataMC->GetXaxis()->SetTitleOffset(x_title_offset_);
    hDataMC->GetXaxis()->SetLabelSize(x_label_size_);
    hDataMC->GetYaxis()->SetTitleSize(axis_font_size_);
    hDataMC->GetYaxis()->SetTitleOffset(y_title_offset_);
    hDataMC->GetYaxis()->SetLabelSize(y_label_size_);
    max_val = hDataMC->GetMaximum();
    double mn = hDataMC->GetMinimum();
    mn = std::max(0.2*mn,5e-1);
    max_val = 1.2*max_val;
    max_val = std::min(max_val, 2.0);
    // hDataMC->GetYaxis()->SetRangeUser(mn,m);
    hDataMC->GetYaxis()->SetRangeUser(ratio_plot_min_, ratio_plot_max_);
    //  hDataMC->GetXaxis()->SetLabelOffset(0.5);

    hDataMC->SetMarkerStyle(20);
    //  hDataMC->SetName("hDataMC");
    if(hDataMCErr)
      hDataMCErr->Draw("E2");
  } else if(hDataMC && data_over_mc_ < 0) {
    pad2->cd();
    pad2->SetGrid();
    c->SetGrid();
    max_val = 0.;
    for(unsigned index = 0; index < hSignalsOverMC.size(); ++index) {
      Double_t signal_scale = signal_scale_;
      TString label = hSignalsOverMC[index]->GetName();
      label.ReplaceAll(Form("_%s_%i%s_over_mc", hist.Data(), set, (density_plot_) ? "_density" : ""), "");
      if(signal_scales_.find(label) != signal_scales_.end()) signal_scale = signal_scales_[label];
      if(signal_scale > 0.) {
        hSignalsOverMC[index]->Scale(1./signal_scale);
        if(data_over_mc_ == -2) hSignalsOverMC[index]->Scale(1./signal_scale); //divide out the (A*s)^2 = A^2(s^2) multiplier
      }
      if(index == 0) hSignalsOverMC[index]->Draw("hist E1");
      else           hSignalsOverMC[index]->Draw("hist E1 same");
      max_val = std::max(max_val, hSignalsOverMC[index]->GetMaximum());
    }
    if(hSignalsOverMC.size() > 0) {
      hSignalsOverMC[0]->GetXaxis()->SetTitle(xtitle.Data());
      if(data_over_mc_ == -2) hSignalsOverMC[0]->GetYaxis()->SetTitle("S^{2}/B");
      else                    hSignalsOverMC[0]->GetYaxis()->SetTitle("S/B");
      hSignalsOverMC[0]->GetXaxis()->SetTitleSize(axis_font_size_);
      hSignalsOverMC[0]->GetXaxis()->SetTitleOffset(x_title_offset_);
      hSignalsOverMC[0]->GetXaxis()->SetLabelSize(x_label_size_);
      hSignalsOverMC[0]->GetYaxis()->SetTitleSize(axis_font_size_);
      hSignalsOverMC[0]->GetYaxis()->SetTitleOffset(y_title_offset_);
      hSignalsOverMC[0]->GetYaxis()->SetLabelSize(y_label_size_);
      hSignalsOverMC[0]->SetAxisRange(max_val/1.e4,max_val*5., "Y");
      hSignalsOverMC[0]->SetTitle("");
      if(xMin_ < xMax_) hSignalsOverMC[0]->GetXaxis()->SetRangeUser(xMin_,xMax_);
      pad2->SetLogy();
    }
  }

  if(stack_as_hist_) {
    for(auto o : *hstack->GetHists()) { //clear out the un-needed histograms
      delete o;
    }
    delete hstack;
  }

  return c;

}

//--------------------------------------------------------------------------------------------------------------------
//Get a TGraph to represent the errors of a histogram represented in other histogrms
TGraphAsymmErrors* DataPlotter::get_errors(TH1* h, TH1* h_p, TH1* h_m, bool ratio, double& r_min, double& r_max) {

  int nbins = h->GetNbinsX();
  double xs[nbins], ys[nbins], p_errs[nbins], m_errs[nbins], x_errs[nbins];
  r_min = 10.; r_max = -1.;
  for(int ibin = 1; ibin <= nbins; ++ibin) {
    xs[ibin-1] = h->GetBinCenter(ibin);
    x_errs[ibin-1] = h->GetBinWidth(ibin)/2.;
    ys[ibin-1] = h->GetBinContent(ibin);
    double p_err = h_p->GetBinContent(ibin) - ys[ibin-1];
    double m_err = (!h_m) ? 0. : h_m->GetBinContent(ibin) - ys[ibin-1];
    p_errs[ibin-1] = std::max(0., std::max(m_err, p_err));
    m_errs[ibin-1] = abs(std::min(0., std::min(m_err, p_err)));
    if(ratio) {
      if(ys[ibin-1] > 0.) {
        p_errs[ibin-1] /= ys[ibin-1];
        m_errs[ibin-1] /= ys[ibin-1];
      } else {
        p_errs[ibin-1] = 0.;
        m_errs[ibin-1] = 0.;
      }
      ys[ibin-1] = 1.;
    }
    r_min = std::min(r_min, ys[ibin-1] - m_errs[ibin-1]);
    r_max = std::max(r_max, ys[ibin-1] + p_errs[ibin-1]);
  }
  TGraphAsymmErrors* g = new TGraphAsymmErrors(nbins, xs, ys, x_errs, x_errs, m_errs, p_errs);
  g->SetName(Form("g%s_%s", (ratio) ? "_r" : "", h->GetName()));
  g->SetFillColor(h->GetFillColor());
  g->SetLineColor(h->GetLineColor());
  g->SetFillStyle(3001);
  g->SetLineWidth(3);
  return g;
}

//--------------------------------------------------------------------------------------------------------------------
TCanvas* DataPlotter::plot_systematic(TString hist, Int_t set, Int_t systematic, ScaleUncertainty_t* sys_scale) {
  TString c_name = Form("sys_%s_%i_%i", hist.Data(), systematic, set);
  while(TObject* o = gDirectory->Get(c_name.Data())) delete o;

  if(sys_scale) {
    std::cout << __func__ << ": Systematic scale plotting not yet implemented!\n";
    return nullptr;
  }

  //Get background stacks
  THStack* hstack_b = get_stack(hist+"_0"                               , "systematic", set);
  THStack* hstack_p = get_stack(Form("%s_%i", hist.Data(), systematic  ), "systematic", set);
  THStack* hstack_m = (single_systematic_) ? 0 : get_stack(Form("%s_%i", hist.Data(), systematic+1), "systematic", set);

  if(!hstack_b || !hstack_p || !(single_systematic_ || hstack_m)) return nullptr;

  //Get data if needed
  TH1* d = (plot_data_ > 0) ? get_data(hist+"_0", "systematic", set) : 0;
  if(plot_data_ > 0 && !d) return nullptr;

  //Normalize to the stack integral if normalizing, for simplicity
  if(d && normalize_1ds_ && d->Integral() > 0.) {
    d->Scale(((TH1*)hstack_b->GetStack()->Last())->Integral() / d->Integral());
  }

  //blind if needed
  if(blindxmin_.size() > 0 && d) {
    unsigned nbins = d->GetNbinsX();
    for(unsigned bin = 1; bin <= nbins; ++bin) {
      double binlow = d->GetBinLowEdge(bin);
      double binhigh = binlow + d->GetBinWidth(bin);
      if(isBlind(binlow, binhigh))
        d->SetBinContent(bin, 0.);
    }
  }

  //Get signal histograms
  std::vector<TH1*> signals_b = get_signal(hist+"_0"                               , "systematic", set);
  std::vector<TH1*> signals_p = get_signal(Form("%s_%i", hist.Data(), systematic  ), "systematic", set);
  std::vector<TH1*> signals_m = (single_systematic_) ? std::vector<TH1*>{} : get_signal(Form("%s_%i", hist.Data(), systematic+1), "systematic", set);
  if(signals_b.size() != signals_p.size() || (!single_systematic_ && signals_b.size() != signals_m.size())) {
    std::cout << __func__ << ": Error! Signal vectors not equal for base (" << signals_b.size() <<") and shifted (+"
              << signals_p.size() << ", -" << signals_m.size() << ") hist = " << hist.Data() << " sys = " << systematic << " histograms!\n";
    return nullptr;
  }

  if(normalize_1ds_) {
    for(TH1* hsig : signals_b) {
      if(hsig->Integral() > 0.)
        hsig->Scale(((TH1*)hstack_b->GetStack()->Last())->Integral() / hsig->Integral());
    }
    for(TH1* hsig : signals_p) {
      if(hsig->Integral() > 0.)
        hsig->Scale(((TH1*)hstack_b->GetStack()->Last())->Integral() / hsig->Integral());
    }
    for(TH1* hsig : signals_m) {
      if(hsig->Integral() > 0.)
        hsig->Scale(((TH1*)hstack_b->GetStack()->Last())->Integral() / hsig->Integral());
    }
  }
  //Setup the canvas
  TCanvas* c = new TCanvas(c_name.Data(),
                           c_name.Data(),
                           canvas_x_, canvas_y_);
  //split the top into main stacks and bottom into Orig / Systematic
  TPad* pad1 = new TPad("pad1","pad1",upper_pad_x1_, upper_pad_y1_, upper_pad_x2_, upper_pad_y2_); //xL yL xH xH, (0,0) = bottom left
  TPad* pad2 = new TPad("pad2","pad2",lower_pad_x1_, lower_pad_y1_, lower_pad_x2_, lower_pad_y2_);
  pad1->SetTopMargin(upper_pad_topmargin_);
  if(data_over_mc_ < 0)
    pad2->SetTopMargin(lower_pad_sigbkg_topmargin_);
  else
    pad2->SetTopMargin(lower_pad_topmargin_);
  pad1->SetBottomMargin(upper_pad_botmargin_);
  pad2->SetBottomMargin(lower_pad_botmargin_);
  pad1->Draw();
  pad2->Draw();
  pad1->cd();

  ///////////////////////////////////////////////////////////////////////////////////////
  //Draw the background stack with +- systematic as histograms
  ///////////////////////////////////////////////////////////////////////////////////////

  if(hstack_b->GetNhists() == 0 || hstack_p->GetNhists() == 0|| (!single_systematic_ && hstack_m->GetNhists() == 0)) {
    printf("%s: Systematics stacks may be empty! N(hists): base = %i, +1 = %i, -1 = %i\n", __func__,
           hstack_b->GetNhists(), hstack_p->GetNhists(), (single_systematic_) ? 0 : hstack_m->GetNhists());
    return nullptr;
  }

  TH1* h_b = (TH1*) hstack_b->GetStack()->Last();
  TH1* h_p = (TH1*) hstack_p->GetStack()->Last();
  TH1* h_m = (single_systematic_) ? 0 : (TH1*) hstack_m->GetStack()->Last();

  if(!h_b || !h_p || (!h_m && !single_systematic_)) {
    printf("%s: Systematics histograms not found! Stack N(hists): base = %i, +1 = %i, -1 = %i\n", __func__,
           hstack_b->GetNhists(), hstack_p->GetNhists(), (single_systematic_) ? 0 : hstack_m->GetNhists());
    return nullptr;
  }

  h_b->SetName(Form("h_bkg_%i_sys_%i", set, systematic));
  h_b->SetLineColor(kRed-3);
  h_b->SetLineWidth(2);
  h_b->SetFillColor(0);
  TGraph* g_stack = get_errors(h_b, h_p, h_m);
  g_stack->SetFillColor(h_b->GetLineColor());
  g_stack->SetLineColor(h_b->GetLineColor());
  g_stack->Draw("AP2");
  h_b->Draw("same hist");
  g_stack->Draw("P2");

  //Draw the data
  if(d) d->Draw("E same");

  TLegend* leg = new TLegend(legend_sys_x1_, legend_sys_y1_, legend_sys_x2_, legend_sys_y1_ - 0.1*(1+signals_b.size()));
  leg->SetTextSize(legend_txt_);
  leg->SetFillStyle(0);
  leg->SetFillColor(0);
  leg->SetLineColor(0);
  leg->SetLineStyle(0);
  if(d) leg->AddEntry(d, "Data");
  leg->AddEntry(g_stack, "Background", "FL");

  double m = std::max((!h_m) ? 0. : h_m->GetMaximum(), h_p->GetMaximum());

  for(unsigned index = 0; index < signals_b.size(); ++index) {
    if(!signals_b[index] || !signals_p[index] || (!single_systematic_ && !signals_m[index])) {
      std::cout << __func__ << ": Systematic histograms missing for signal " << index << std::endl;
      return nullptr;
    }
    TGraph* g = get_errors(signals_b[index], signals_p[index], (single_systematic_) ? 0 : signals_m[index]);
    signals_b[index]->SetFillColor(0);
    signals_b[index]->Draw("hist same");
    signals_b[index]->SetLineWidth(2);
    g->Draw("P2");
    m = std::max(m, std::max((single_systematic_) ? 0. : signals_m[index]->GetMaximum(), signals_p[index]->GetMaximum()));
    leg->AddEntry(g, signals_b[index]->GetTitle(), "FL");
  }
  leg->Draw();

  //Configure the axes
  g_stack->GetYaxis()->SetRangeUser(0.1, 1.2*m);
  if(xMin_ < xMax_) g_stack->GetXaxis()->SetRangeUser(xMin_, xMax_);

  TString xtitle, ytitle, title;
  Titles::get_titles(hist, "event", selection_, &xtitle, &ytitle, &title);
  if(xtitle == "") Titles::get_titles(hist, "lep", selection_, &xtitle, &ytitle, &title);

  g_stack->SetTitle("");
  draw_cms_label(false);
  draw_luminosity(false);
  pad1->SetGrid();
  if(logY_) pad1->SetLogy();

  ///////////////////////////////////////////////////////////////////////////////////////
  //Draw the ratio histograms on the lower pad
  ///////////////////////////////////////////////////////////////////////////////////////

  pad2->cd();

  double r_min, r_max;
  TGraph* g_r_stack = get_errors(h_b, h_p, h_m, true, r_min, r_max);
  if(verbose_ > 1) printf("%s: Ratio min/max for Bkg = %.4f/%.4f\n", __func__, r_min, r_max);
  g_r_stack->SetFillColor(g_r_stack->GetLineColor());
  g_r_stack->SetFillStyle(3001);
  g_r_stack->Draw("ALE2");
  for(unsigned index = 0; index < signals_b.size(); ++index) {
    double r_min_s, r_max_s;
    TGraph* g = get_errors(signals_b[index], signals_p[index], (single_systematic_) ? 0 : signals_m[index], true, r_min_s, r_max_s);
    if(verbose_ > 1) printf("%s: Ratio min/max for %s = %.4f/%.4f\n", __func__, signals_b[index]->GetTitle(), r_min_s, r_max_s);
    if(index == 2) g->SetLineWidth(g->GetLineWidth()-1);
    if(index == 0) {
      g->SetFillColor(g->GetLineColor());
      g->SetFillStyle(3004);
      g->Draw("PL");
      g->Draw("PLE2");
    } else {
      g->Draw("PL");
    }
    r_min = std::min(r_min, r_min_s);
    r_max = std::max(r_max, r_max_s);
  }
  //calculate data / MC
  TH1* hdata_ratio = (TH1*) d->Clone(Form("%s_ratio", d->GetName()));
  hdata_ratio->SetDirectory(0);
  hdata_ratio->Divide(h_b);
  r_max = std::min(2., std::max(r_max, hdata_ratio->GetMaximum()));
  if(verbose_ > 1) printf("%s: Ratio plot min/max = %.3f/%.3f\n", __func__, r_min, r_max);
  r_min = std::min(0.9, 1. + 1.15*(r_min - 1.));
  r_max = std::max(1.1, 1. + 1.15*(r_max - 1.));
  const double r_diff = std::max(1. - r_min, r_max - 1.); //make the plot window symmetric
  r_min = 1. - r_diff;
  r_max = 1. + r_diff;
  if(verbose_ > 1) printf("%s: Expanded Ratio plot min/max = %.3f/%.3f\n", __func__, r_min, r_max);
  g_r_stack->GetYaxis()->SetRangeUser(r_min, r_max);
  if(xMin_ < xMax_) g_r_stack->GetXaxis()->SetRangeUser(xMin_, xMax_);
  g_r_stack->GetYaxis()->SetTitleSize(axis_font_size_);
  g_r_stack->GetXaxis()->SetTitleSize(axis_font_size_);
  g_r_stack->GetXaxis()->SetLabelSize(x_label_size_);
  g_r_stack->GetYaxis()->SetLabelSize(y_label_size_);
  g_r_stack->GetXaxis()->SetTitleOffset(x_title_offset_);
  g_r_stack->GetYaxis()->SetTitleOffset(y_title_offset_);
  g_r_stack->SetTitle(Form(";%s;", xtitle.Data()));

  //draw data / MC
  hdata_ratio->Draw("same");
  pad2->SetGrid();

  return c;
}

//--------------------------------------------------------------------------------------------------------------------
TCanvas* DataPlotter::plot_cdf(TString hist, TString setType, Int_t set, TString label) {

  TH1* hCDF = 0;
  for(UInt_t i = 0; i < inputs_.size(); ++i) {
    Data_t& input = inputs_[i];
    if(input.label_ == label) {
      TH1* tmp = (TH1*) input.data_->Get(Form("%s_%i/%s", setType.Data(), set, hist.Data()));
      if(!tmp) continue;
      tmp = (TH1*) tmp->Clone("tmp");
      // tmp->SetBit(kCanDelete);
      tmp->Scale(input.scale_);

      if(hCDF) {
        hCDF->Add(tmp);
        delete tmp;
      }
      else {
        hCDF = tmp;
        hCDF->SetName("hCDF");
      }
    }
  }
  if(!hCDF) {
    printf("Histogram for CDF making with label %s not found!\n", label.Data());
    return nullptr;
  }

  //Build CDF (CDF(x) = integral(pdf(x')dx', -infinity, x))
  TH1* tmp = (TH1*) hCDF->Clone("tmp");
  tmp->Scale(1./tmp->Integral()/tmp->GetBinWidth(1));
  const int nbins = tmp->GetNbinsX();
  for(Int_t bin = 1; bin <= nbins; ++bin)
    hCDF->SetBinContent(bin, tmp->Integral(0, bin)*tmp->GetBinWidth(1));
  hCDF->Scale(1./hCDF->GetMaximum());
  // tmp->SetBit(kCanDelete);

  int rebinH = rebinH_;
  rebinH_ = 1;
  //With cdf made, now loop through histograms and generate transforms of each
  THStack* hstack = get_stack(hist, setType, set);
  TList* hists = hstack->GetHists();
  {
    auto o = gDirectory->Get(Form("%s_%i_cdf",hist.Data(),set));
    if(o) delete o;
  }
  THStack* hcdfstack = new THStack(Form("%s_%i_cdf",hist.Data(),set),Form("%s_cdf",hist.Data()));
  std::vector<TH1*> htransforms;
  double xtransmin = 0.;
  double xtransmax = 1.7;
  int ntransbins = (xtransmax-xtransmin)/0.02; //0.02 width bins
  for(TObject* hist : *hists) {
    TH1* htmp = (TH1*) hist;
    while(auto o = gDirectory->Get(Form("%s_trans_%i", htmp->GetName(),set))) {
      if(o) delete o;
    }
    TH1* htrans = new TH1D(Form("%s_trans_%i", htmp->GetName(),set), htmp->GetTitle(),
                            ntransbins, xtransmin, xtransmax);
    // htrans->SetBit(kCanDelete);

    for(Int_t bin = 1; bin <= htmp->GetNbinsX(); ++bin) {
      Double_t y = hCDF->GetBinContent(bin);
      htrans->Fill(y, htmp->GetBinContent(bin));
    }
    htrans->SetLineColor(htmp->GetLineColor());
    if(fill_alpha_ < 1.)
      htrans->SetFillColorAlpha(htmp->GetFillColor(),fill_alpha_);
    else
      htrans->SetFillColor(htmp->GetFillColor());
    htrans->SetFillStyle(htmp->GetFillStyle());
    htrans->SetLineWidth(htmp->GetLineWidth());
    htrans->SetMarkerStyle(htmp->GetMarkerStyle());
    hcdfstack->Add(htrans);
  }

  {
    auto o = gDirectory->Get("data_cdf");
    if(o) delete o;
  }
  TH1* d = get_data(hist, setType, set);
  TH1* data_cdf = new TH1D("data_cdf", d->GetTitle(),
                            ntransbins, xtransmin, xtransmax);
  // data_cdf->SetBit(kCanDelete);
  for(Int_t bin = 1; bin <= d->GetNbinsX(); ++bin) {
    Double_t y = hCDF->GetBinContent(bin);
    if(isBlind(y)) continue; //blinding data
    data_cdf->Fill(y, d->GetBinContent(bin));
    data_cdf->SetBinError(data_cdf->FindBin(y), sqrt(data_cdf->GetBinContent(data_cdf->FindBin(y))));
  }

  data_cdf->SetLineColor(d->GetLineColor());
  data_cdf->SetFillColor(d->GetFillColor());
  data_cdf->SetFillStyle(d->GetFillStyle());
  data_cdf->SetLineWidth(d->GetLineWidth());
  data_cdf->SetMarkerStyle(d->GetMarkerStyle());
  delete d;

  TCanvas* c = new TCanvas(Form("cdf_%s_%s_%i",hist.Data(),label.Data(),set),Form("cdf_%s_%s_%i",hist.Data(),label.Data(),set), canvas_x_, canvas_y_);
  TPad *pad1 = 0, *pad2 = 0;


  if(plot_data_) {
    pad1 = new TPad("pad1","pad1",upper_pad_x1_, upper_pad_y1_, upper_pad_x2_, upper_pad_y2_); //xL yL xH xH, (0,0) = bottom left
    pad2 = new TPad("pad2","pad2",lower_pad_x1_, lower_pad_y1_, lower_pad_x2_, lower_pad_y2_);
    pad1->SetTopMargin(upper_pad_topmargin_);
    pad2->SetTopMargin(lower_pad_topmargin_);
    pad1->SetBottomMargin(upper_pad_botmargin_);
    pad2->SetBottomMargin(lower_pad_botmargin_);
    pad1->Draw();
    pad2->Draw();
  } else {
    pad1 = new TPad("pad1","pad1",0.0,0.0,1,1); //xL yL xH xH, (0,0) = bottom left
    pad1->Draw();
  }
  pad1->cd();

  hcdfstack->Draw("hist noclear");
  std::vector<TH1*> hsignal = get_signal(hist,setType,set);
  std::vector<TH1*> hsignalcdf;
  //store the y-axis range info
  double max_val(1.e-5), min_val(1.e20);
  for(TH1* signal : hsignal) {
    auto o = gDirectory->Get(Form("%s_trans", signal->GetName()));
    if(o) delete o;
    TH1* htrans = new TH1D(Form("%s_trans", signal->GetName()), signal->GetTitle(),
                            ntransbins, xtransmin, xtransmax);
    // htrans->SetBit(kCanDelete);

    for(Int_t bin = 1; bin <= signal->GetNbinsX(); ++bin) {
      Double_t y = hCDF->GetBinContent(bin);
      htrans->Fill(y, signal->GetBinContent(bin));
    }
    htrans->SetLineColor(signal->GetLineColor());
    if(fill_alpha_ < 1.)
      htrans->SetFillColorAlpha(signal->GetFillColor(),fill_alpha_);
    else
      htrans->SetFillColor(signal->GetFillColor());
    htrans->SetFillStyle(signal->GetFillStyle());
    htrans->SetLineWidth(signal->GetLineWidth());
    htrans->SetMarkerStyle(signal->GetMarkerStyle());
    htrans->Draw("hist same");
    max_val = std::max(max_val, Utilities::H1Max(htrans, xMin_, xMax_));
    min_val = std::min(min_val, Utilities::H1Min(htrans, xMin_, xMax_));
    if(verbose_ > 3) std::cout << " " << htrans->GetTitle() << ": min = " << min_val << " max = " << max_val << std::endl;
    hsignalcdf.push_back(htrans);
  }
  if(plot_data_) data_cdf->Draw("same");
  delete hCDF; //done with the CDF

  if(data_cdf && plot_data_ > 0) {
    max_val = std::max(max_val, Utilities::H1Max(data_cdf, xMin_, xMax_));
    //FIXME: Include data minimum, accounting for blinding
    // min_val = std::min(min_val, Utilities::H1Min(data_cdf, xMin_, xMax_));
    if(verbose_ > 3) std::cout << " " << data_cdf->GetTitle() << ": min = " << min_val << " max = " << max_val << std::endl;
  }
  max_val = std::max(max_val, Utilities::H1Max((TH1*) hcdfstack->GetStack()->Last(), xMin_, xMax_));
  min_val = std::min(min_val, Utilities::H1Min((TH1*) hcdfstack->GetStack()->Last(), xMin_, xMax_));
  if(verbose_ > 3) std::cout << " stack: min = " << min_val << " max = " << max_val << std::endl;

  TH1* hDataMC = (plot_data_) ? (TH1*) data_cdf->Clone("hDataMC") : 0;
  // TGraphErrors* hDataMCErr = 0;
  double nmc = 0.;
  int ndata = 0;
  int nb = (data_cdf) ? data_cdf->GetNbinsX() : -1;
  if(hDataMC) {
    // hDataMC->SetBit(kCanDelete);
    hDataMC->Clear();
    TH1* hlast = get_stack_uncertainty(hcdfstack,Form("uncertainty_cdf_%i", set));
    nmc = hlast->Integral((density_plot_ > 0) ? "width" : "");
    nmc += hlast->GetBinContent(0);
    nmc += hlast->GetBinContent(nb+1);
    ndata = data_cdf->Integral((density_plot_ > 0) ? "width" : "");
    ndata += data_cdf->GetBinContent(0);
    ndata += data_cdf->GetBinContent(nb+1);
    for(int i = 1; i <= nb; ++i) {
      double dataVal = 0;
      double dataErr = 0;
      dataVal = data_cdf->GetBinContent(i);
      dataErr = data_cdf->GetBinError(i);
      dataErr = sqrt(dataVal);
      double mcVal = hlast->GetBinContent(i);
      if(dataVal == 0 || mcVal == 0) {hDataMC->SetBinContent(i,-1); continue;}
      double ratio = dataVal/mcVal;
      double errRatio = (ratio)*(ratio)*(dataErr*dataErr/(dataVal*dataVal));
      errRatio = sqrt(errRatio);
      hDataMC->SetBinContent(i,ratio);
      hDataMC->SetBinError(i,errRatio);
    }
  }

  TLegend* leg = new TLegend((doStatsLegend_/* || plot_single*/) ? legend_x1_stats_ : legend_x1_, legend_y1_, legend_x2_, legend_y2_);
  leg->SetTextSize((doStatsLegend_/* || plot_single*/) ? 0.75*legend_txt_ : legend_txt_);
  leg->SetNColumns(legend_ncol_);
  if(plot_data_) leg->AddEntry(data_cdf, data_cdf->GetTitle(), "PL");
  // if(stack_uncertainty_) leg->AddEntry(huncertainty, huncertainty->GetTitle(), "F");
  if(!stack_signal_) {
    for(unsigned int i = 0; i < hsignalcdf.size(); ++i) {
      leg->AddEntry(hsignalcdf[i], hsignalcdf[i]->GetTitle(), "FL");
    }
  }
  for(int i = 0; i < hcdfstack->GetNhists(); ++i) {
    TH1* h = (TH1*) hcdfstack->GetHists()->At(i);
    leg->AddEntry(h, h->GetTitle(), "F");
  }
  // leg->SetEntrySeparation(legend_sep_);
  leg->SetFillStyle(0);
  leg->SetFillColor(0);
  leg->SetLineColor(0);
  leg->SetLineStyle(0);
  leg->Draw();

  pad1->SetGrid();
  pad1->Update();

  //get axis titles
  TString xtitle;
  TString ytitle;
  TString title;
  Titles::get_titles(hist,setType,selection_,&xtitle,&ytitle,&title);

  //draw text
  draw_luminosity();
  draw_cms_label();

  if(plot_data_ && hDataMC) hDataMC->GetXaxis()->SetTitle(Form("%s CDF", xtitle.Data()));
  else hcdfstack->GetXaxis()->SetTitle(Form("%s CDF", xtitle.Data()));
  if(plot_y_title_) hcdfstack->GetYaxis()->SetTitle(ytitle.Data());
  hcdfstack->GetXaxis()->SetTitleSize(axis_font_size_);
  hcdfstack->GetYaxis()->SetTitleSize(axis_font_size_);

  double ymin = yMin_;
  double ymax = yMax_;
  if(verbose_ > 2) std::cout << "Min val = " << min_val << " max val = " << max_val << std::endl;
  if(ymin > ymax) {
    ymin = std::max(0.8, min_val*((logY_) ? 1./log_buffer_ : 1./linear_buffer_));
    if(logY_) {
      ymax = (max_val < ymin) ? log_buffer_*ymin : ymin*std::pow(10, log_buffer_*std::log10(max_val/ymin));
    } else {
      ymax = linear_buffer_*std::max(max_val, ymin);
    }
  }
  if(verbose_ > 2) std::cout << "Y min = " << ymin << " y max = " << ymax << std::endl;

  hcdfstack->GetYaxis()->SetRangeUser(ymin,ymax);
  hcdfstack->SetMinimum(ymin);
  hcdfstack->SetMaximum(ymax);
  if(plot_data_ && xMin_ < xMax_ && hDataMC) hDataMC->GetXaxis()->SetRangeUser(xMin_,xMax_);
  if(xMin_ < xMax_) hcdfstack->GetXaxis()->SetRangeUser(xMin_,xMax_);

  if(plot_title_) hcdfstack->SetTitle (title.Data());
  else hcdfstack->SetTitle("");

  if(logY_) {
    if(plot_data_)pad1->SetLogy();
    else          c->SetLogy();
  }
  c->SetGrid();
  if(plot_data_ && hDataMC) {
    pad2->cd();
    pad2->SetGrid();
    c->SetGrid();
    hDataMC->Draw("E");
    TLine* line = new TLine((xMax_ < xMin_) ? hDataMC->GetBinCenter(1)-hDataMC->GetBinWidth(1)/2. : xMin_, 1.,
                            (xMax_ < xMin_) ? hDataMC->GetBinCenter(hDataMC->GetNbinsX())+hDataMC->GetBinWidth(1)/2. : xMax_, 1.);
    line->SetLineColor(kRed);
    line->Draw("same");

    hDataMC->GetYaxis()->SetTitle("Data/MC");
    hDataMC->GetXaxis()->SetTitleSize(axis_font_size_);
    hDataMC->GetXaxis()->SetTitleOffset(x_title_offset_);
    hDataMC->GetXaxis()->SetLabelSize(x_label_size_);
    hDataMC->GetYaxis()->SetTitleSize(axis_font_size_);
    hDataMC->GetYaxis()->SetTitleOffset(y_title_offset_);
    hDataMC->GetYaxis()->SetLabelSize(y_label_size_);
    double m = hDataMC->GetMaximum();
    double mn = hDataMC->GetMinimum();
    mn = std::max(0.2*mn,5e-1);
    m = 1.2*m;
    m = std::min(m, 2.0);
    hDataMC->GetYaxis()->SetRangeUser(mn,m);
    hDataMC->SetMinimum(mn);
    hDataMC->SetMaximum(m);
    //  hDataMC->GetXaxis()->SetLabelOffset(0.5);

    hDataMC->SetMarkerStyle(20);
  }

  rebinH_ = rebinH;
  return c;
}

//--------------------------------------------------------------------------------------------------------------------
TCanvas* DataPlotter::plot_significance(TString hist, TString setType, Int_t set, TString label,
                                        bool dir = true, Double_t line_val = -1., bool doVsEff = false,
                                        TString label1 = "", TString label2 = "") {

  //Get the histogram definining the signal significance
  TH1* hSignal = nullptr;
  TString sig_name = Form("hSig_signal_%s_%s_%i", hist.Data(), setType.Data(), set);
  {
    auto o = gDirectory->Get(sig_name.Data());
    if(o) delete o;
  }
  //get the signal histogram
  TString hist_name = Form("%s_%i/%s", setType.Data(), set, hist.Data());
  for(UInt_t i = 0; i < inputs_.size(); ++i) {
    Data_t& input = inputs_[i];
    if(input.label_ == label) { //look for correct label
      TH1* tmp = (TH1*) input.data_->Get(hist_name.Data());
      if(!tmp) {
        printf("%s: Histogram %s not found for data set %i (%s), continuing...\n", __func__, hist_name.Data(),
               (int) i, label.Data());
        continue;
      }
      auto o = gDirectory->Get("htmp");
      if(o) delete o;
      tmp = (TH1*) tmp->Clone("htmp");
      tmp->SetDirectory(0);
      // tmp->SetBit(kCanDelete);
      tmp->Scale(input.scale_);

      if(hSignal) {
        hSignal->Add(tmp);
        delete tmp;
      }
      else {
        hSignal = tmp;
        hSignal->SetName(sig_name.Data());
      }
    }
  }

  if(!hSignal) {
    printf("Histogram for significance making with label %s not found!\n", label.Data());
    return nullptr;
  }

  //don't rebin for improved statistics
  const int rebinH = rebinH_;
  rebinH_ = 1;

  //take the signal histogram as a template for x-binnning of the efficiency histogram
  TString eff_name = Form("hSig_efficiency_%s_%s_%i", hist.Data(), setType.Data(), set);
  {
    auto o = gDirectory->Get(eff_name.Data());
    if(o) delete o;
  }
  TH1* hEfficiency = (TH1*) hSignal->Clone(eff_name.Data());
  hEfficiency->Clear(); hEfficiency->Reset();

  //get the background stack
  THStack* hstack = get_stack(hist, setType, set);
  TString bkg_name = Form("hSig_background_%s_%s_%i", hist.Data(), setType.Data(), set);
  {
    auto o = gDirectory->Get(bkg_name.Data());
    if(o) delete o;
  }
  TH1* hlast = (TH1*) hstack->GetStack()->Last()->Clone(bkg_name.Data());

  //clean up memory
  for(auto htmp : *hstack->GetHists())
    delete htmp;
  delete hstack;

  //parameters for limit loop
  const UInt_t nbins = hSignal->GetNbinsX();
  UInt_t maxbin = nbins;
  UInt_t minbin = 0;
  if(limit_xmax_ > limit_xmin_) { //define a specific window to look in
    maxbin = hSignal->FindBin(limit_xmax_) - 1; //don't include the found bin
    minbin = hSignal->FindBin(limit_xmin_);
  }

  const double clsig = 1.644853627; // 95% CL value

  double xs[nbins], xerrs[nbins]; //for significance graph
  double sigs[nbins], sigsErrUp[nbins], sigsErrDown[nbins];
  double init_sig = -1.;

  double p(0.05), tolerance(0.001), val(-1.);
  Significances significances(p, tolerance, useCLs_, 10);
  double max_score(-1.), max_value(-1.);
  for(UInt_t bin = 1; bin <= nbins; ++bin) {
    xs[bin-1] = (dir) ? hSignal->GetBinLowEdge(bin) : hSignal->GetBinLowEdge(bin) + hSignal->GetBinWidth(bin);
    sigs[bin-1] = 0.;
    xerrs[bin-1] = 0.;
    sigsErrUp[bin-1] = 0.;
    sigsErrDown[bin-1] = 0.;
    if(bin > maxbin) continue;
    if(bin <= minbin) continue;
    double bkgerr(0.), sigerr(0.);
    const double bkgval = (dir) ? hlast->IntegralAndError(bin, maxbin, bkgerr) : hlast->IntegralAndError(minbin, bin, bkgerr);
    const double sigval = (dir) ? hSignal->IntegralAndError(bin, maxbin, sigerr) : hSignal->IntegralAndError(minbin, bin, sigerr);
    if(init_sig < 0. && sigval > 0.) init_sig = sigval;
    //plot vs signal efficiency instead
    if(doVsEff) xs[bin-1] = sigval/init_sig;
    hEfficiency->SetBinContent(bin, sigval);
    if(bkgval <= 0. || sigval <= 0.) continue;
    double significance = sigval/sqrt(bkgval)/clsig; //not really significance but ratio of signal to background fluctuation
    if(doExactLimit_)  //get 95% CL by finding signal scale so poisson prob n < n_bkg for mu = n_bkg + n_sig = 0.05
      significance = significances.LimitGain(sigval, bkgval, val);
    sigs[bin-1] = significance;
    if(max_value < significance) {max_value = significance; max_score = xs[bin-1];}
    //Add MC uncertainty band
    if(limit_mc_err_range_) {
      //bkg+1 sigma
      significance = sigval/sqrt(bkgval+bkgerr)/clsig; //not really significance but ratio of signal to background fluctuation
      if(doExactLimit_)  //get 95% CL by finding signal scale so poisson prob n < n_bkg for mu = n_bkg + n_sig = 0.05
        significance = significances.LimitGain(sigval, bkgval+bkgerr, val);
      sigsErrUp[bin-1] = sigs[bin-1]-significance;
      //bkg-1 sigma
      significance = sigval/sqrt(std::max(0.1, bkgval-bkgerr))/clsig; //not really significance but ratio of signal to background fluctuation
      if(doExactLimit_)  //get 95% CL by finding signal scale so poisson prob n < n_bkg for mu = n_bkg + n_sig = 0.05
        significance = significances.LimitGain(sigval, std::max(0.1, bkgval-bkgerr), val);
      sigsErrDown[bin-1] = significance-sigs[bin-1];
    }
  }

  //clean up memory
  delete hlast;
  delete hSignal;

  //print information about maximimum
  printf("Maximum limit gain of %.5e at %.5f\n", max_value, max_score);

  TCanvas* c = new TCanvas(Form("sig_%s_%i",hist.Data(), set), Form("sig_%s_%i",hist.Data(),set), canvas_x_, canvas_y_);
  c->SetTopMargin(0.055);
  // c->SetRightMargin(0.12);
  //get axis titles
  TString xtitle;
  TString ytitle;
  TString title;
  Titles::get_titles(hist,setType,selection_,&xtitle,&ytitle,&title);

  TString lim_name = Form("hSig_limits_%s_%s_%i", hist.Data(), setType.Data(), set);
  {
    auto o = gDirectory->Get(lim_name.Data());
    if(o) delete o;
  }
  TGraph* gSignificance = (limit_mc_err_range_) ? new TGraphAsymmErrors(nbins, xs, sigs, xerrs, xerrs, sigsErrUp, sigsErrDown) :
    new TGraph(nbins, xs, sigs);
  if(verbose_ > 1) {
    std::cout << "Printing up to 15 significance bins (" << nbins << " total):\n";
    for(int bin = 1; bin < std::min(1.*nbins, 1.*16); ++bin)
      std::cout << "Bin " << bin << ": " << "(x, +xerr, -xerr, y, +yerr, -yerr) = ("
                << xs[bin-1] << ", " << xerrs[bin-1] << ", " << xerrs[bin-1] << ", " << sigs[bin-1]
                << ", " << sigsErrDown[bin-1] << ", " << sigsErrUp[bin-1] <<")"
                << std::endl;
  }

  gSignificance->SetName(lim_name.Data());
  gSignificance->SetTitle(Form("; %s; %s",
                               (doVsEff) ? "Efficiency" : (xtitle+" Cut Value").Data(),
                               (doExactLimit_) ? "Limit gain factor" : Form("s/(%.3f*#sqrt{b})", clsig))
                          );
  gSignificance->SetLineColor(kBlue);
  if(limit_mc_err_range_) gSignificance->SetFillColor(kGreen);
  gSignificance->SetLineWidth(3);
  if(limit_mc_err_range_)
    gSignificance->Draw("APL3");
  gSignificance->Draw(((limit_mc_err_range_) ? "LX" : "APL"));
  TAxis* yaxis = gSignificance->GetYaxis();
  yaxis->SetRangeUser(0., sig_plot_range_);
  yaxis->SetLabelColor(kBlue);
  yaxis->SetTitleColor(kBlue);
  yaxis->SetTitleSize(0.04);
  gSignificance->GetXaxis()->SetTitleSize(0.04);
  TLine* line;
  TLine* second_line = 0;
  if(xMax_ > xMin_) {
    gSignificance->GetXaxis()->SetRangeUser(xMin_, xMax_);
    line = new TLine(xMin_, 1., xMax_, 1.);
    if(line_val > 0.) second_line = new TLine(xMin_, line_val, xMax_, line_val);
  } else {
    line = new TLine(xs[0], 1., xs[nbins-1], 1.);
    if(line_val > 0.) second_line = new TLine(xs[0], line_val, xs[nbins-1], line_val);
  }
  if(second_line) {
    second_line->SetLineColor(kTeal);
    second_line->SetLineWidth(4);
    second_line->Draw("sames");
  }
  line->SetLineColor(kRed);
  line->SetLineWidth(4);
  line->Draw("sames");
  //draw text
  draw_luminosity(true);
  draw_cms_label(true);
  c->Update();

  if(!doVsEff) {
    //scale to put on same plot
    double rightmax = 1.2*hEfficiency->GetMaximum(); //maximum of second axis
    double scale = gPad->GetUymax()/rightmax;
    hEfficiency->SetLineColor(kGreen+2);
    hEfficiency->SetMarkerColor(kGreen+2);
    hEfficiency->SetMarkerStyle(6);

    hEfficiency->Scale(scale);
    hEfficiency->Draw("P same");

    //make second axis on right side of plot
    TGaxis* axis = new TGaxis(gPad->GetUxmax(), gPad->GetUymin(),
                              gPad->GetUxmax(), gPad->GetUymax(), 0, rightmax, 510, "+L");
    axis->SetTitle(Form("n_{%s}",label.Data()));
    axis->SetLabelColor(kGreen+2);
    axis->SetTitleColor(kGreen+2);
    axis->SetTitleSize(0.05);
    axis->SetTitleOffset(-0.7);
    axis->Draw();
  }
  TLegend* leg = new TLegend(0.55, (line_val > 0.) ? 0.55 : 0.6, 0.89, 0.8);
  leg->AddEntry(gSignificance, (doExactLimit_) ? "Limit Gain" : "Significance/#sigma_{95%}", "L");
  if(!doVsEff) leg->AddEntry(hEfficiency, "N(Signal)", "L");
  leg->AddEntry(line, (label1 == "") ? "Current Limit" : label1.Data(), "L");
  if(second_line) leg->AddEntry(second_line, (label2 == "") ? "Previous Limit" : label2.Data(), "L");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->Draw();
  rebinH_ = rebinH;
  if(logY_) c->SetLogy();
  c->SetGrid();
  return c;
}

//--------------------------------------------------------------------------------------------------------------------
TCanvas* DataPlotter::print_stack(TString hist, TString setType, Int_t set, TString tag) {
  TCanvas* c = plot_stack(hist,setType,set);
  if(!c) return c;
  c->Print(GetFigureName(setType, hist, set, "stack", tag));
  return c;
}

//--------------------------------------------------------------------------------------------------------------------
TCanvas* DataPlotter::print_systematic(TString hist, Int_t set, Int_t systematic, TString tag, ScaleUncertainty_t* sys_scale) {
  TCanvas* c = plot_systematic(hist,set,systematic, sys_scale);
  if(!c) return c;
  TString label = "";
  if(single_systematic_) label += "single_";
  label += systematic;
  if(only_signal_ != "") {
    label += "_";
    label += only_signal_;
    label.ReplaceAll("#", "");
    label.ReplaceAll("->", "");
    label.ToLower();
  }
  c->Print(GetFigureName(label, hist, set, "sys", tag));
  return c;
}

// //--------------------------------------------------------------------------------------------------------------------
// TCanvas* DataPlotter::print_hist(TString hist, TString setType, Int_t set, TString tag) {
//   TCanvas* c = plot_hist(hist,setType,set);
//   if(!c) return c;
//   TString year_dir = "";
//   c->Print(GetFigureName(setType, hist, set, "hist", tag));
//   return c;
// }

// //--------------------------------------------------------------------------------------------------------------------
// TCanvas* DataPlotter::print_2Dhist(TString hist, TString setType, Int_t set, TString tag) {
//   TCanvas* c = plot_2Dhist(hist,setType,set);
//   if(!c) return c;
//   c->Print(GetFigureName(setType, hist, set, "hist2D", tag));
//   return c;
// }

// //--------------------------------------------------------------------------------------------------------------------
// TCanvas* DataPlotter::print_single_2Dhist(TString hist, TString setType, Int_t set, TString label, TString tag) {
//   TCanvas* c = plot_single_2Dhist(hist,setType,set,label);
//   if(!c) return c;
//   label.ReplaceAll("#",""); //for ease of use in bash
//   label.ReplaceAll(" ", "");
//   label.ReplaceAll("/", "");
//   c->Print(GetFigureName(setType, hist, set, "hist2D_" + label, tag));
//   return c;
// }

//--------------------------------------------------------------------------------------------------------------------
TCanvas* DataPlotter::print_cdf(TString hist, TString setType, Int_t set, TString label, TString tag) {
  TCanvas* c = plot_cdf(hist,setType,set,label);
  if(!c) return c;
  label.ReplaceAll("#",""); //for ease of use in bash
  label.ReplaceAll(" ", "");
  label.ReplaceAll("/", "");
  c->Print(GetFigureName(setType, hist, set, "cdf_" + label, tag));
  return c;
}

//--------------------------------------------------------------------------------------------------------------------
TCanvas* DataPlotter::print_significance(TString hist, TString setType, Int_t set, TString label,
                                         bool dir, Double_t line_val, bool doVsEff,
                                         TString label1, TString label2, TString tag) {
  TCanvas* c = plot_significance(hist,setType,set,label, dir, line_val, doVsEff, label1, label2);
  if(!c) return c;
  label.ReplaceAll("#",""); //for ease of use in bash
  label.ReplaceAll(" ", "");
  label.ReplaceAll("/", "");
  if(doVsEff) label = "vsEff_" + label;
  c->Print(GetFigureName(setType, hist, set, "sig_" + label, tag, true));
  return c;
}

//--------------------------------------------------------------------------------------------------------------------
Int_t DataPlotter::print_stacks(std::vector<TString> hists, std::vector<TString> setTypes, std::vector<Int_t> sets
                                , std::vector<Double_t> xMaxs, std::vector<Double_t> xMins, std::vector<Int_t> rebins
                                , std::vector<Double_t> signal_scales = {}, std::vector<Int_t> base_rebins = {}) {
  if(hists.size() != setTypes.size()) {
    printf("hist size != hist type size!\n");
    return 1;
  }
  if(hists.size() != xMaxs.size()) {
    printf("hist size != xMaxs size!\n");
    return 2;
  }
  if(hists.size() != xMins.size()) {
    printf("hist size != xMins size!\n");
    return 3;
  }
  if(hists.size() != rebins.size()) {
    printf("hist size != rebins size!\n");
    return 4;
  }
  if(signal_scales.size() > 0 && signal_scales.size() != sets.size()) {
    printf("signal scales size != sets size!\n");
    return 5;
  }
  if(base_rebins.size() > 0 && base_rebins.size() != sets.size()) {
    printf("base rebins size != sets size!\n");
    return 6;
  }
  Int_t set_index = 0;
  Int_t base_rebin = 1;
  for(Int_t set : sets) {
    if(signal_scales.size() > 0) signal_scale_ = signal_scales[set_index];
    for(UInt_t i = 0; i < hists.size(); ++i) {
      TString hist(hists[i]);
      TString setType(setTypes[i]);
      xMax_ = xMaxs[i];
      xMin_ = xMins[i];
      rebinH_ = rebins[i]*base_rebin;
      if(rebinH_ == 0) rebinH_ = 1;
      TCanvas* c = print_stack(hist,setType,set);
      Int_t status = (c) ? 0 : 1;
      if(verbose_ > 0) printf("Printing Data/MC stack %s %s set %i has status %i\n",setType.Data(),hist.Data(),set,status);
      Empty_Canvas(c);
    }
    ++set_index;
  }
  return 0;
}

// //--------------------------------------------------------------------------------------------------------------------
// Int_t DataPlotter::print_hists(std::vector<TString> hists, std::vector<TString> setTypes, std::vector<Int_t> sets
//                                , std::vector<Double_t> xMaxs, std::vector<Double_t> xMins, std::vector<Int_t> rebins
//                                , std::vector<Double_t> signal_scales = {}, std::vector<Int_t> base_rebins = {}) {
//   if(hists.size() != setTypes.size()) {
//     printf("hist size != hist type size!\n");
//     return 1;
//   }
//   if(hists.size() != xMaxs.size()) {
//     printf("hist size != xMaxs size!\n");
//     return 2;
//   }
//   if(hists.size() != xMins.size()) {
//     printf("hist size != xMins size!\n");
//     return 3;
//   }
//   if(hists.size() != rebins.size()) {
//     printf("hist size != rebins size!\n");
//     return 4;
//   }
//   if(signal_scales.size() > 0 && signal_scales.size() != sets.size()) {
//     printf("signal scales size != sets size!\n");
//     return 5;
//   }
//   if(base_rebins.size() > 0 && base_rebins.size() != sets.size()) {
//     printf("base rebins size != sets size!\n");
//     return 6;
//   }
//   Int_t set_index = 0;
//   Int_t base_rebin = 1;
//   for(Int_t set : sets) {
//     if(signal_scales.size() > 0) signal_scale_ = signal_scales[set_index];
//     if(base_rebins.size() > 0) base_rebin = base_rebins[set_index];
//     for(UInt_t i = 0; i < hists.size(); ++i) {
//       TString hist(hists[i]);
//       TString setType(setTypes[i]);
//       xMax_ = xMaxs[i];
//       xMin_ = xMins[i];
//       rebinH_ = rebins[i]*base_rebin;
//       TCanvas* c = print_hist(hist,setType,set);
//       Int_t status = (c) ? 0 : 1;
//       if(verbose_ > 0) printf("Printing Data/MC hist %s %s set %i has status %i\n",setType.Data(),hist.Data(),set,status);
//       Empty_Canvas(c);
//     }
//     ++set_index;
//   }

//   return 0;
// }


//--------------------------------------------------------------------------------------------------------------------
Int_t DataPlotter::init_files() {

  UInt_t nInputs = inputs_.size();
  for(UInt_t i = 0; i < nInputs; ++i) {
    Data_t& input = inputs_[i];
    if(verbose_ > 0)
      std::cout << "Initializing input dataset: filepath = " << input.fileName_.Data()
                << " name = " << input.name_.Data()
                << " label = " << input.label_.Data()
                << " isData = " << input.isData_
                << " xsec = " << input.xsec_
                << " isSignal = " << input.isSignal_ << std::endl;
    input.file_ = TFile::Open(input.fileName_.Data(),"READ");
    if(input.file_) {
      input.data_ = (TFile*) input.file_->Get("Data");
      if(!input.data_) {
        printf("File %s Data folder not found! Exiting\n", input.fileName_.Data());
        return 2;
      }
    }
    else {
      printf("File %s not found! Exiting\n", input.fileName_.Data());
      return 1;
    }
  }
  for(UInt_t i = 0; i < nInputs; ++i) {
    Data_t& input = inputs_[i];
    if(input.isData_ || input.isEmbed_) input.scale_ = input.xsec_; //xsec is 1 for data, 1 or slightly above for embedding to account for failed jobs
    else {
      // loop through the directory and add events from each event histogram (in case of hadd combined dataset)
      int nevents = 0;
      TKey* key = 0;
      TIter nextkey(input.file_->GetListOfKeys());
      TH1* events = 0;
      int nseen = 0;
      while((key = (TKey*)nextkey())) {
        TObject* obj = key->ReadObj();
        if(obj->InheritsFrom(TH1::Class())) {
          events = (TH1*) obj;
          nevents += events->GetBinContent(1); //total events
          nevents -= 2.*events->GetBinContent(10); //negative weight events
          if(debug_ > 0) printf("%s %s: bin 1, bin 11: %.0f, %.0f\n", input.name_.Data(), events->GetName(),
                                events->GetBinContent(1), events->GetBinContent(10));
          ++nseen;
        }
      }
      if(nevents <= 0.) {
        std::cout << "Error! No events in the events histogram for " << input.name_.Data() << std::endl;
        return 10;
      }
      if(nseen != 1) {
        std::cout << "Error! None or more than 1 events histogram found for " << input.name_.Data() << std::endl;
        return 11;
      }
      input.ngenerated_ = nevents;
      input.scale_ = (1./(nevents)*input.xsec_*((lums_.size() > 0) ? lums_[input.dataYear_] : lum_));
    }
    if(input.scale_ <= 0.) std::cout << __func__ << ": Warning! Dataset " << i << " ("
                                     << input.name_.Data() << ") has <= 0 scale = " << input.scale_
                                     << std::endl;
  }

  gStyle->SetTitleW(0.8f);
  gStyle->SetOptStat(0);
  if(useOpenGL_) gStyle->SetCanvasPreferGL(kTRUE);
  rnd_ = new TRandom(seed_);

  return 0;
}

//--------------------------------------------------------------------------------------------------------------------
Int_t DataPlotter::add_dataset(TString filepath, TString name, TString label, bool isData, double xsec, bool isSignal, bool isEmbed, int color) {
  Data_t input;
  input.fileName_ = filepath;
  input.name_ = name;
  input.label_ = label;
  input.isData_ = isData;
  input.isEmbed_ = isEmbed;
  input.xsec_ = (isData) ? 1. : xsec;
  input.isSignal_ = isSignal;
  input.color_ = color;
  int dataYear = 0;
  if(filepath.Contains("2016"))      dataYear = 2016;
  else if(filepath.Contains("2017")) dataYear = 2017;
  else if(filepath.Contains("2018")) dataYear = 2018;
  input.dataYear_ = dataYear;
  if(verbose_ > 0)
    std::cout << "Added dataset, filepath = " << filepath.Data()
              << " name = " << name.Data()
              << " label = " << label.Data()
              << " isData = " << isData
              << " xsec = " << xsec
              << " isSignal = " << isSignal
              << " year = " << dataYear
              << std::endl;

  inputs_.push_back(input);

  return 0;
}
