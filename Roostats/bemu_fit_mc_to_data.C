//Fit the background templates to the data in the sidebands
#include "bemu_fit_bkg_mc.C"

bool   fit_flat_bkgs_  = true ; //fit flat-ish background contributions (WW, ttbar, QCD, and (not flat) Z->mumu)
bool   fit_dy_bkg_     = false; //fit the Z->tautau background
int    smooth_hists_   =     0; //number of times to smooth non-fit background histograms
double zmumu_scale_    =   -1.; //if >= 0 scale the Z->ee/mumu contribution
bool   save_templates_ = false; //save MC templates in an output file
TString tag_           =    ""; //tag for output figure directory

vector<int> years_;
int         set_  ;
//---------------------------------------------------------------------------------------------------------------------------------------
// Figure directory
const char* get_fig_dir() {
  TString years_s = Form("%i", years_[0]);
  for(unsigned i = 1; i < years_.size(); ++i) years_s += Form("_%i", years_[i]);
  return Form("plots/latest_production/%s/zemu_mc_fits_%i%s", years_s.Data(), set_, tag_.Data());
}

//---------------------------------------------------------------------------------------------------------------------------------------
// Main function
int bemu_fit_mc_to_data(int set = 13, vector<int> years = {2016,2017,2018}) {

  if(years.size() == 0) return -1;
  years_ = years;
  set_ = set;
  TString years_s = Form("%i", years_[0]);
  for(unsigned i = 1; i < years_.size(); ++i) years_s += Form("_%i", years_[i]);

  //for output figures
  const char* fig_dir = get_fig_dir();
  gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s", fig_dir, fig_dir));
  gStyle->SetOptFit(1110);

  //Open the MC histogram file
  TFile* fMC = TFile::Open(Form("histograms/zemu_lepm_%i_%s.hist", set, years_s.Data()), "READ");
  if(!fMC) return -1;

  //Get the data
  TH1* data = (TH1*) fMC->Get("hdata");
  if(!data) return -1;

  //blind the data
  const double xmin(70.), xmax(110.);
  const double blind_min(86.), blind_max(96.);
  for(int ibin = data->FindBin(blind_min + 1.e-3); ibin <= data->FindBin(blind_max - 1.e-3); ++ibin) {
    data->SetBinContent(ibin, 0.);
    data->SetBinError  (ibin, 0.);
  }

  //trim not used regions
  data = make_safe(data, xmin, xmax);

  //Get the MC background stack
  THStack* stack_in = (THStack*) fMC->Get("hstack");
  if(!stack_in) return -2;

  //Replace background distributions where needed
  const int rebin = 1; //rebin the distributions to help statistical uncertainty
  THStack* stack = new THStack("bkg_stack", "Background stack");
  for(int ihist = 0; ihist < stack_in->GetNhists(); ++ihist) {
    TH1* h = (TH1*) stack_in->GetHists()->At(ihist);
    // h->Rebin(rebin);
    h = make_safe(h, xmin, xmax);
    cout << h->GetName() << ": " << h->GetTitle() << endl;
    bool isflat(false);
    isflat |= TString(h->GetName()).Contains("QCD"  );
    isflat |= TString(h->GetName()).Contains("Top"  );
    isflat |= TString(h->GetName()).Contains("Other");
    isflat |= TString(h->GetName()).Contains("Z->ee");
    bool isdy = TString(h->GetName()).Contains("#tau#tau");
    bool isembed = TString(h->GetName()).Contains("Embedding");
    if(isflat) { //flat-ish distributions
      if(fit_flat_bkgs_) {
        fit_and_replace(h, xmin, xmax, get_fig_dir(), set, rebin);
      } else if(smooth_hists_ > 0) h->Smooth(smooth_hists_);
    }
    if(isdy) { //Z->tautau
      if(fit_dy_bkg_) {
        fit_and_replace(h, xmin, xmax, get_fig_dir(), set, rebin);
      } else if(smooth_hists_ > 0) h->Smooth(smooth_hists_);
    }
    if(!isflat && !isdy && smooth_hists_ > 0) h->Smooth(smooth_hists_); //any leftover histogram
    if(zmumu_scale_ >= 0. && TString(h->GetName()).Contains("Z->ee")) h->Scale(zmumu_scale_);
    //scale Embedding to match the Drell-Yan MC normalization
    if(isembed) {
      if     (set == 13 || set == 74 || set == 84) h->Scale(6.52/5.70);
      else if(set == 12 || set == 73 || set == 83) h->Scale(1.78/1.66);
      else if(set == 11 || set == 72 || set == 82) h->Scale(8.94/8.43);
    }
    stack->Add(h);
  }

  //plot the overall background stack
  {
    TCanvas* c = new TCanvas();
    data->Draw("E1");
    stack->Draw("hist noclear same");
    data->GetYaxis()->SetRangeUser(0., 1.2*max(data->GetMaximum(), stack->GetMaximum()));
    c->SaveAs(Form("%s/stack.png", get_fig_dir()));
    delete c;
  }

  //Create the RooFit objects
  RooRealVar obs("obs", "M_{e#mu}", 90., xmin, xmax);
  obs.setBins(data->GetNbinsX());
  obs.setRange("full", xmin, xmax);
  obs.setRange("LowSideband", xmin, blind_min);
  obs.setRange("HighSideband", blind_max, xmax);
  obs.setRange("BlindRegion", blind_min, blind_max);
  RooDataHist data_hist("data_hist", "Data", RooArgList(obs), data);

  //Create a background PDF for both the Drell-Yan/tau tau and the WW/ttbar
  TH1* hDY(nullptr);
  TH1* hWW(nullptr);
  for(auto o : *(stack->GetHists())) {
    auto h = (TH1*) o;
    TString name(h->GetName());
    if(name.Contains("Z") || name.Contains("#tau")) {
      if(!hDY) hDY = (TH1*) h->Clone("hDY");
      else hDY->Add(h);
    } else {
      if(!hWW) hWW = (TH1*) h->Clone("hWW");
      else hWW->Add(h);
    }
  }
  if(!hWW || !hDY) return -10;

  RooDataHist dy("dy", "DY background", RooArgList(obs), hDY);
  RooDataHist ww("ww", "WW background", RooArgList(obs), hWW);
  RooHistPdf dyPDF("dyPDF", "DY PDF", obs, dy);
  RooHistPdf wwPDF("wwPDF", "WW PDF", obs, ww);

  //Add the two background components together
  const double n_mc = hDY->Integral() + hWW->Integral();
  RooRealVar* n_dy  = new RooRealVar("n_dy", "N(DY)", 0.5*hDY->Integral(), 0., 10.*n_mc);
  RooRealVar* n_ww  = new RooRealVar("n_ww", "N(WW)", 0.5*hWW->Integral(), 0., 10.*n_mc);
  RooAddPdf* totPDF = new RooAddPdf("total_PDF", "total PDF", RooArgList(dyPDF, wwPDF), RooArgList(*n_dy, *n_ww), false);

  auto fit_res = totPDF->fitTo(data_hist, RooFit::PrintLevel(-1), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1), RooFit::Range("LowSideband,HighSideband"), RooFit::Save());
  if(fit_res->status() != 0) { //fit failed
    cout << "##################################################" << endl
         << "## Fit failed!\n"
         << "##################################################" << endl;
    return -1;
  }

  //get the scale factors
  const double dy_scale = n_dy->getVal() / hDY->Integral();
  const double ww_scale = n_ww->getVal() / hWW->Integral();
  printf("Fit scale factors: DY: %.4f; WW: %.4f\n", dy_scale, ww_scale);

  //Plot the results
  {
    hDY->Scale(dy_scale);
    hWW->Scale(ww_scale);
    TH1* hTotal = (TH1*) hDY->Clone("hTotal");
    hTotal->Add(hWW);
    hTotal->SetLineWidth(2);
    hTotal->SetLineColor(kBlue);
    hTotal->SetMarkerColor(kBlue);
    TCanvas* c = new TCanvas();
    TRatioPlot plot(data, hTotal);
    plot.Draw();
    c->SaveAs(Form("%s/fit.png", get_fig_dir()));
    delete c;
    delete hTotal;
  }

  fMC->Close();
  return 0;
}
