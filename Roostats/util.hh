#ifndef __FIT_UTIL__
#define __FIT_UTIL__

//---------------------------------------------------------------------------------------------------------------------
// Print a one-sided uncertainty plot
int print_mva_systematic_canvas(TH1* nominal, TH1* data, THStack* nom_stack, THStack* stack, TH1* nom_sig, TH1* sig,
                                const char* name, const char* fig_name) {
  //Check the inputs
  if(!data || !stack || !nom_sig || !sig || !nominal) return 1;

  const double xmin(0.), xmax(1.);
  const bool logy(true);

  //Make the canvas
  TCanvas* c = new TCanvas(name, name, 1200, 900);
  TPad* pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1. );
  TPad* pad2 = new TPad("pad2", "pad2", 0., 0. , 1., 0.3);
  pad1->SetBottomMargin(0.06);
  pad2->SetTopMargin(0.03);
  pad1->Draw(); pad2->Draw();
  pad1->cd();

  //Draw the total stack / data on top
  data->SetLineColor(kBlack);
  data->Draw("E1");
  stack->Draw("hist noclear same");
  sig->Draw("hist same");
  data->Draw("E1 same");
  data->GetXaxis()->SetRangeUser(0., 1.);
  const double max_val = max(Utilities::H1Max(data, xmin, xmax), Utilities::H1Max((TH1*) stack->GetStack()->Last(), xmin, xmax));
  data->GetYaxis()->SetRangeUser((logy) ? 0.5 : 0.9, ((logy) ? 2 : 1.1)*max_val);
  data->SetTitle("");
  if(logy) pad1->SetLogy();

  //Draw ratio plots on the bottom
  pad2->cd();
  vector<TH1*> h_to_delete;
  TH1* bkg_ratio = (TH1*) stack->GetStack()->Last()->Clone("bkg_ratio");
  bkg_ratio->Divide(nominal);
  TH1* sig_ratio = (TH1*) sig->Clone("sig_ratio");
  sig_ratio->Divide(nom_sig);
  TH1* data_ratio = (TH1*) data->Clone("data_ratio");
  data_ratio->Divide(nominal);
  h_to_delete.push_back(bkg_ratio);
  h_to_delete.push_back(sig_ratio);
  h_to_delete.push_back(data_ratio);

  const double max_bkg_val(Utilities::H1Max(bkg_ratio, xmin, xmax)), min_bkg_val(Utilities::H1Min(bkg_ratio, xmin, xmax));
  const double max_sig_val(Utilities::H1Max(sig_ratio, xmin, xmax)), min_sig_val(Utilities::H1Min(sig_ratio, xmin, xmax));
  double min_r = min((min_bkg_val <= 0.) ? 0.95 : min_bkg_val, (min_sig_val <= 0.) ? 0.95 : min_sig_val); //try to catch cases with val = 0
  double max_r = max(max_bkg_val, max_sig_val);
  min_r = max(0.85, min(0.95, min_r - 0.05*(1. - min_r))); //set the ratio bounds to be between 5-15% with a small buffer
  max_r = min(1.15, max(1.05, max_r + 0.05*(max_r - 1.)));

  bkg_ratio->SetLineColor(kRed-3);
  bkg_ratio->SetLineWidth(2);
  bkg_ratio->SetFillColor(0);
  sig_ratio->SetLineWidth(2);
  sig_ratio->SetFillColor(0);

  data_ratio->Draw("E1");
  sig_ratio->Draw("hist same");
  bkg_ratio->Draw("hist same");
  data_ratio->GetYaxis()->SetRangeUser(min_r, max_r);
  data_ratio->GetXaxis()->SetRangeUser(xmin, xmax);
  data_ratio->GetXaxis()->SetLabelSize(0.08);
  data_ratio->GetYaxis()->SetLabelSize(0.08);

  //Draw each stack component ratio
  const int nhists = stack->GetNhists();
  for(int ihist = 0; ihist < nhists; ++ihist) {
    TH1* h     = (TH1*) stack    ->GetHists()->At(ihist)->Clone(Form("h_stack_ratio_%i", ihist));
    TH1* h_nom = (TH1*) nom_stack->GetHists()->At(ihist);
    h->Add(h_nom, -1); //subtract off the nominal to get the shift
    //Add the nominal and then divide by it to get the relative effect on the total
    h->Add(nominal);
    h->Divide(nominal);
    int color = h_nom->GetLineColor();
    if(color == kYellow - 7) color = kOrange; //make some colors more visible
    if(color == kGreen  - 7) color = kGreen - 3;
    h->SetLineColor(color);
    h->SetLineWidth(3);
    h->SetFillColor(0);
    h->SetLineStyle(kDashed);
    h->Draw("hist same");
    h_to_delete.push_back(h);
  }

  //Print the figure
  c->SaveAs(fig_name);

  delete c;
  for(auto h : h_to_delete) delete h;

  return 0;
}

//---------------------------------------------------------------------------------------------------------------
// Get the difference between the data and a PDF: h = Data - PDF
TH1* data_pdf_diff(RooDataHist* data, RooAbsPdf* pdf, RooRealVar& obs, double norm = -1., double blind_min = 1., double blind_max = -1.) {
  if(!data || !pdf) return nullptr;
  TH1* h_pdf  = pdf ->createHistogram(Form("TMP_%s_pdf" , __func__), obs);
  TH1* h_data = data->createHistogram(Form("TMP_%s_data", __func__), obs);
  const double ndata((norm > 0.) ? norm : h_data->Integral()), npdf(h_pdf->Integral());
  if(npdf <= 0.) return nullptr;
  h_pdf->Scale(ndata / npdf);
  TH1* h_diff = (TH1*) h_data->Clone(Form("TMP_%s_diff" , __func__));
  h_diff->Add(h_pdf, -1.);
  h_diff->SetLineWidth(2);
  for(int ibin = 1; ibin <= h_diff->GetNbinsX(); ++ibin) {
    //check if blinded
    if(blind_min < blind_max) {
      const double x_low  = h_data->GetBinLowEdge(ibin);
      const double x_high = h_data->GetBinWidth(ibin) + x_low;
      if((x_low >= blind_min && x_low < blind_max) ||
         (x_high > blind_min && x_high <= blind_max)) {
        h_diff->SetBinContent(ibin, 0.);
        h_diff->SetBinError  (ibin, 0.);
        continue;
      }
    }
    const double data_val = h_data->GetBinContent(ibin);
    const double data_err = h_data->GetBinError  (ibin);
    h_diff->SetBinError(ibin, (data_err > 0.) ? data_err : (data_val > 0.) ? sqrt(data_val) : 0.);
  }
  return h_diff;
}

//---------------------------------------------------------------------------------------------------------------
// Get the difference between two PDFs for a given N(data): h = PDF_1 - PDF_2
TH1* pdf_pdf_diff(RooAbsPdf* pdf_1, RooAbsPdf* pdf_2, RooRealVar& obs, const double ndata) {
  if(!pdf_1 || !pdf_2) return nullptr;
  TH1* h_pdf_1 = pdf_1->createHistogram(Form("TMP_%s_pdf_1", __func__), obs);
  TH1* h_pdf_2 = pdf_2->createHistogram(Form("TMP_%s_pdf_2", __func__), obs);
  const double npdf_1(h_pdf_1->Integral()), npdf_2(h_pdf_2->Integral());
  if(npdf_1 <= 0. || npdf_2 <= 0.) return nullptr;
  h_pdf_1->Scale(ndata / npdf_1);
  h_pdf_2->Scale(ndata / npdf_2);
  TH1* h_diff = (TH1*) h_pdf_1->Clone(Form("TMP_%s_diff" , __func__));
  h_diff->Add(h_pdf_2, -1.);
  h_diff->SetLineWidth(2);
  return h_diff;
}

//-----------------------------------------------------------------------------------------------------------------------------------
// Count free PDF params
int count_pdf_params(RooAbsPdf* pdf) {
  int nfree = 0;
  auto vars = pdf->getVariables();
  auto itr = vars->createIterator();
  auto var = itr->Next();
  while(var) {
    if(!((RooAbsReal*) var)->isConstant()) ++nfree;
    var = itr->Next();
  }
  return max(nfree-1,0); //remove the observable from counting
}

void print_pdf_params(RooAbsPdf* pdf, const char* obs = nullptr) {
  auto vars = pdf->getVariables();
  auto itr = vars->createIterator();
  auto var = itr->Next();
  while(var) {
    if(!obs || TString(var->GetName()) != TString(obs)) {
      var->Print();
    }
    var = itr->Next();
  }
}


#endif
