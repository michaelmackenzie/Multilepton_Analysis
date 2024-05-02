#ifndef __FIT_UTIL__
#define __FIT_UTIL__

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
