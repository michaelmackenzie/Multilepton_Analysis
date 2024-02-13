//Test Z->e+mu background fit closure when fitting the MC
#include "bemu_fit_bkg_mc.C"

bool   fit_flat_bkgs_ = true;  //fit flat-ish background contributions (WW, ttbar, QCD, and (not flat) Z->mumu)
bool   fit_dy_bkg_    = false; //fit the Z->tautau background
int    smooth_hists_  =   2;   //number of times to smooth non-fit background histograms
double zmumu_scale_   = -1.;   //if >= 0 scale the Z->ee/mumu contribution
int    use_multi_pdf_ =   0;   //use multi-pdf instead of a single pdf FIXME: Not currently working
TString tag_          = "_embed_zmumu_cuts";    //tag for output figure directory

//---------------------------------------------------------------------------------------------------------------------------------------
// Main function: test the background fit closure of the MC
int test_bemu_mc_closure(int set = 13, vector<int> years = {2016,2017,2018}, const int index = -1) {

  if(years.size() == 0) return -1;
  TString years_s = Form("%i", years[0]);
  for(unsigned i = 1; i < years.size(); ++i) years_s += Form("_%i", years[i]);

  //for outpur figures
  const char* fig_dir = Form("plots/latest_production/%s/zemu_mc_closure_%i%s", years_s.Data(), set, tag_.Data());
  gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s", fig_dir, fig_dir));
  gStyle->SetOptFit(1110);

  //Open the workspace file
  const char* fname = Form("datacards/%s/combine_bemu_zemu_%i.root", years_s.Data(), set);
  TFile* f = TFile::Open(fname, "READ");
  if(!f) return -1;

  //Retrieve the workspace
  RooWorkspace* ws = (RooWorkspace*) f->Get(Form("lepm_%i", set));
  if(!ws) return 1;

  //Retrieve the signal PDF
  RooAbsPdf* sigPDF = ws->pdf(Form("zemu"));
  if(!sigPDF) {ws->Print(); return 2;}

  //Retrieve the signal PDF
  RooAbsPdf* bkgPDF(nullptr);
  if(use_multi_pdf_) bkgPDF = ws->pdf(Form("bkg"));
  else               bkgPDF = ws->pdf(Form("bst_%i_order_3", set));
  if(!bkgPDF) {ws->Print(); return 3;}

  //Get the observable
  RooRealVar* obs = (RooRealVar*) ws->var(Form("lepm_%i", set));
  if(!obs) return 4;

  //Get the envelope index variable
  RooCategory* cat = (RooCategory*) ws->obj(Form("cat_%i", set));
  if(!cat) return 5;
  if(index >= 0) cat->setIndex(index);
  if(use_multi_pdf_ == 0 || index >= 0) cat->setConstant(true);

  //Try repairing the multi-pdf with a single PDF if requested
  if(use_multi_pdf_ == 2) {
    RooArgList pdf_list;
    auto bst = bkgPDF = ws->pdf(Form("bst_%i_order_3", set));
    if(!bst) return -5;
    pdf_list.add(*bst);
    bkgPDF = new RooMultiPdf("bkgPDF", "Background PDF envelope", *cat, pdf_list);
  }

  //turn off the energy scale uncertainties that are not currently constrained
  auto elec_es_shift = ws->var("elec_ES_shift");
  if(elec_es_shift) elec_es_shift->setConstant(true);
  auto muon_es_shift = ws->var("muon_ES_shift");
  if(muon_es_shift) muon_es_shift->setConstant(true);

  //Open the MC histogram file
  TFile* fMC = TFile::Open(Form("histograms/zemu_lepm_%i_%s.hist", set, years_s.Data()), "READ");
  if(!fMC) return -1;

  //Get the signal histogram for normalization
  TH1* sig_hist = (TH1*) fMC->Get("zemu");
  if(!sig_hist) return -2;
  const double sig_yield_norm = sig_hist->Integral() / 5.; //Signal yield corresponding to BR(Z->e+mu) = 1e-7

  //Get the MC background stack
  THStack* stack_in = (THStack*) fMC->Get("hstack");
  if(!stack_in) return -2;

  //Replace background distributions where needed
  const int rebin = 2; //rebin the distributions to help statistical uncertainty
  THStack* stack = new THStack("bkg_stack", "Background stack");
  for(int ihist = 0; ihist < stack_in->GetNhists(); ++ihist) {
    TH1* h = (TH1*) stack_in->GetHists()->At(ihist);
    h->Rebin(rebin);
    h = make_safe(h, obs->getMin(), obs->getMax());
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
        fit_and_replace(h, obs->getMin(), obs->getMax(), Form("plots/latest_production/%s/zemu_mc_closure_%i%s", years_s.Data(), set, tag_.Data()));
      } else if(smooth_hists_ > 0) h->Smooth(smooth_hists_);
    }
    if(isdy) { //Z->tautau
      if(fit_dy_bkg_) {
        fit_and_replace(h, obs->getMin(), obs->getMax(), Form("plots/latest_production/%s/zemu_mc_closure_%i%s", years_s.Data(), set, tag_.Data()));
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
    // stack_in->Draw("hist noclear");
    stack->Draw("hist noclear");
    c->SaveAs(Form("%s/stack.png", Form("plots/latest_production/%s/zemu_mc_closure_%i%s", years_s.Data(), set, tag_.Data())));
    delete c;
  }

  //Get the MC histogram for generating data
  TH1* hMC = (TH1*) stack->GetStack()->Last()->Clone("hbkg");
  hMC = make_safe(hMC, obs->getMin(), obs->getMax());
  obs->setBins(hMC->GetNbinsX()); //ensure the binning matches
  const int n_init_mc = hMC->Integral() + 0.5;
  RooDataHist mc("mc", "MC data", RooArgList(*obs), hMC);
  RooHistPdf mcPDF("mcPDF", "MC PDF", *obs, mc);

  //Add the signal and background PDFs to make a total PDF
  RooRealVar* n_sig  = new RooRealVar("n_sig", "N(signal)", 0., -n_init_mc, n_init_mc);
  RooRealVar* n_bkg  = new RooRealVar("n_bkg", "N(background)", n_init_mc, 0., 5.*n_init_mc);
  RooAddPdf* totPDF = new RooAddPdf("total_PDF", "total PDF", RooArgList(*bkgPDF, *sigPDF), RooArgList(*n_bkg, *n_sig), false);


  //Datasets to generate
  const int ngen = (use_multi_pdf_) ? 1000 : 1000;

  //Output info
  const double sig_range = max(1000., 5.*std::sqrt(n_init_mc));
  TH1* hNSig = new TH1D("nsig", "N(signal) fit; N(signal)", 200, -sig_range, sig_range);
  TH1* hNBkg = new TH1D("nbkg", "N(background) fit; N(background", 200,n_init_mc*0.7,n_init_mc*1.3);
  TH1* hBrSg = new TH1D("brsg", "Fit signal branching fraction; BR(Z#rightarrowe#mu)x10^{7}", 200, -sig_range/sig_yield_norm, sig_range/sig_yield_norm);
  TH1* hPull = new TH1D("pull", "Signal pull; pull",100, -6., 6.);
  TH1* hCat  = new TH1D("cat" , "Envelope index; index",10, 0., 10.);

  TRandom3 rnd(90);
  bool print_neg(true), print_pos(true);
  for(int igen = 0; igen < ngen; ++igen) {
    const int nmc = rnd.Poisson(hMC->Integral());
    RooDataHist* data = mcPDF.generateBinned(RooArgSet(*obs), nmc);
    if(use_multi_pdf_) {
      auto fit_res = totPDF->fitTo(*data, RooFit::Save(), RooFit::Optimize(false), RooFit::GlobalObservables(*cat));
      if(fit_res->status() != 0) { //fit failed
        cout << "##################################################" << endl
             << "## Fit failed!\n"
             << "##################################################" << endl;
        continue;
      }
    } else {
      auto fit_res = totPDF->fitTo(*data, RooFit::PrintLevel(-1), RooFit::Warnings(0), RooFit::PrintEvalErrors(-1), RooFit::Save());
      if(fit_res->status() != 0) { //fit failed
        cout << "##################################################" << endl
             << "## Fit failed!\n"
             << "##################################################" << endl;
        continue;
      }
    }
    hNSig->Fill(n_sig->getVal());
    hNBkg->Fill(n_bkg->getVal());
    hPull->Fill(n_sig->getVal()/n_sig->getError());
    hBrSg->Fill(n_sig->getVal()/sig_yield_norm);
    hCat->Fill(cat->getIndex());

    if(print_neg && n_sig->getVal() < 0.) { //print first negative fit
      auto frame = obs->frame();
      data->plotOn(frame);
      totPDF->plotOn(frame, RooFit::LineColor(kRed));
      totPDF->plotOn(frame, RooFit::Components(bkgPDF->GetName()), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
      totPDF->plotOn(frame, RooFit::Components(sigPDF->GetName()), RooFit::LineColor(kBlue), RooFit::LineStyle(kDashed));
      TCanvas* c = new TCanvas();
      frame->Draw();
      c->SaveAs(Form("plots/latest_production/%s/zemu_mc_closure_%i%s/fit_neg.png", years_s.Data(), set, tag_.Data()));
      print_neg = false;
    }
    if(print_pos && n_sig->getVal() > 0.) { //print first positive fit
      auto frame = obs->frame();
      data->plotOn(frame);
      totPDF->plotOn(frame, RooFit::LineColor(kRed));
      totPDF->plotOn(frame, RooFit::Components(bkgPDF->GetName()), RooFit::LineColor(kRed), RooFit::LineStyle(kDashed));
      totPDF->plotOn(frame, RooFit::Components(sigPDF->GetName()), RooFit::LineColor(kBlue), RooFit::LineStyle(kDashed));
      TCanvas* c = new TCanvas();
      frame->Draw();
      c->SaveAs(Form("plots/latest_production/%s/zemu_mc_closure_%i%s/fit_pos.png", years_s.Data(), set, tag_.Data()));
      print_pos = false;
    }
  }

  TCanvas* c = new TCanvas();
  hNSig->Draw("hist");
  c->SaveAs(Form("plots/latest_production/%s/zemu_mc_closure_%i%s/nsig.png", years_s.Data(), set, tag_.Data()));
  hNBkg->Draw("hist");
  c->SaveAs(Form("plots/latest_production/%s/zemu_mc_closure_%i%s/nbkg.png", years_s.Data(), set, tag_.Data()));
  hBrSg->Draw("hist");
  c->SaveAs(Form("plots/latest_production/%s/zemu_mc_closure_%i%s/br.png", years_s.Data(), set, tag_.Data()));
  if(use_multi_pdf_) {
    hCat->Draw("hist");
    c->SaveAs(Form("plots/latest_production/%s/zemu_mc_closure_%i%s/cat.png", years_s.Data(), set, tag_.Data()));
  }

  TF1* fgaus = new TF1("fgaus", "[norm]*TMath::Gaus(x, [mean], [sigma], true)", -6, 6);
  fgaus->SetParameters(1., 1., 1.);
  hPull->Fit(fgaus);
  hPull->Draw("hist");
  fgaus->Draw("same");
  c->SaveAs(Form("plots/latest_production/%s/zemu_mc_closure_%i%s/pull.png", years_s.Data(), set, tag_.Data()));

  delete c;

  f->Close();
  fMC->Close();
  return 0;
}
