// Script to compare two different canvases, presumably generated with different MC samples

namespace {
  TFile* f1_ = 0;
  TFile* f2_ = 0;
  TCanvas* c1_ = 0;
  TCanvas* c2_ = 0;
  TString print_path_ = "";
  TString title1_ = "MadGraph"; //MC 1 name
  TString title2_ = "AMC@NLO";  //MC 2 name
}

TCanvas* compare_dataMC() {
  if(!c1_ || !c2_) {
    cout << "Canvases not initialized!" << endl;
    return NULL;
  }

  //Get stack from canvas 2, plot as a single histogram over canvas 1 stack

  // get pad 1, which contains the stack
  TPad* pad1_2 = (TPad*) c2_->GetPrimitive("pad1");
  if(!pad1_2) {
    cout << "Failed to find pad 1 in canvas 2!\n";
    return NULL;
  }

  TList* list = pad1_2->GetListOfPrimitives();
  THStack* hstack = 0;
  for(auto h : *list) {
    if(h->InheritsFrom("THStack")) {
      hstack = (THStack*) h;
      break;
    }
  }
  
  if(!hstack) {
    cout << "Failed to find the stack in pad 1 in canvas 2!\n";
    return NULL;
  }

  TH1F* hlast_2 = (TH1F*) hstack->GetStack()->Last();
  hlast_2->SetFillStyle(0);
  hlast_2->SetLineColor(kMagenta);
  hlast_2->SetLineWidth(2);
  hlast_2->SetMarkerSize(0);
  hlast_2->SetName("canvas_2_stack");
  hlast_2->SetTitle(Form("%s Stack", title2_.Data()));
  
  TCanvas* c = (TCanvas*) c1_->Clone();
  c->SetName("c1_clone");
  TPad* pad1_1 = (TPad*) c->GetPrimitive("pad1");
  if(!pad1_1) {
    cout << "Failed to find pad 1 in canvas 1 clone!\n";
    return NULL;
  }

  // auto o = pad1_1->GetPrimitive("TPave");
  // if(o) {
  //   TLegend* leg = (TLegend*) o;
  //   leg->AddEntry(hlast_2);
  //   leg->Draw();
  // }
  c->Draw();
  pad1_1->cd();
  hlast_2->Draw("hist same");
  TLegend* leg = new TLegend(0.3, 0.9, 0.6, 0.7);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  leg->AddEntry(hlast_2, hlast_2->GetTitle(), "L");
  leg->Draw("same");

  TPad* pad2_2 = (TPad*) c2_->GetPrimitive("pad2");
  if(!pad2_2) {
    cout << "Failed to find pad 2 in canvas 2!\n";
    return NULL;
  }
  TH1F* hDataMC_2 = (TH1F*) pad2_2->GetPrimitive("hDataMC");
  if(!hDataMC_2) {
    cout << "Failed to find data/mc in pad 2 in canvas 2!\n";
    return NULL;
  }
  
  TPad* pad2_1 = (TPad*) c->GetPrimitive("pad2");
  if(!pad2_1) {
    cout << "Failed to find pad 2 in canvas 1 clone!\n";
    return NULL;
  }
  pad2_1->cd();
  hDataMC_2->SetLineColor(kMagenta);
  hDataMC_2->SetFillStyle(0);
  hDataMC_2->SetLineWidth(2);
  hDataMC_2->Draw("hist same");

  c->Print(print_path_.Data());
  return c;
}


//Compare a single histogram in each canvas
TCanvas* compare_mc(TString label, bool normalize = false, double xmin = 1., double xmax = -1.) {
  if(!c1_ || !c2_) {
    cout << "Canvases not initialized!" << endl;
    return NULL;
  }

  //Get the histogram from each canvas and plot them together

  // get pad 1, which contains the stack
  TPad* pad1_1 = (TPad*) c1_->GetPrimitive("pad1");
  if(!pad1_1) {
    cout << "Failed to find pad 1 in canvas 1!\n";
    return NULL;
  }

  TList* list = pad1_1->GetListOfPrimitives();
  THStack* hstack = 0;
  for(auto h : *list) {
    if(h->InheritsFrom("THStack")) {
      hstack = (THStack*) h;
      break;
    }
  }
  
  if(!hstack) {
    cout << "Failed to find the stack in pad 1 in canvas 1!\n";
    return NULL;
  }

  //loop throught the stack, looking for the histogram label
  TH1F* h1 = 0;
  TList* hist_list = hstack->GetHists();
  for(auto hl : *hist_list) {
    TH1F* hist = (TH1F*) hl;
    TString title = hist->GetTitle();
    if(title.Contains(label.Data())) {
      h1 = hist;
      break;
    }
  }
  
  if(!h1) {
    cout << "Failed to find the histogram in pad 1 in canvas 1!\n";
    return NULL;
  }
  h1 = (TH1F*) h1->Clone();
  h1->SetName("hist_1");

  //now get the second histogram
  
  // get pad 1, which contains the stack
  TPad* pad1_2 = (TPad*) c2_->GetPrimitive("pad1");
  if(!pad1_2) {
    cout << "Failed to find pad 1 in canvas 2!\n";
    return NULL;
  }

  list = pad1_2->GetListOfPrimitives();
  hstack = 0;
  for(auto h : *list) {
    if(h->InheritsFrom("THStack")) {
      hstack = (THStack*) h;
      break;
    }
  }
  
  if(!hstack) {
    cout << "Failed to find the stack in pad 1 in canvas 2!\n";
    return NULL;
  }

  //loop throught the stack, looking for the histogram label
  TH1F* h2 = 0;
  hist_list = hstack->GetHists();
  for(auto hl : *hist_list) {
    TH1F* hist = (TH1F*) hl;
    TString title = hist->GetTitle();
    if(title.Contains(label.Data())) {
      h2 = hist;
      break;
    }
  }
  
  if(!h2) {
    cout << "Failed to find the histogram in pad 1 in canvas 2!\n";
    return NULL;
  }

  h2 = (TH1F*) h2->Clone();
  h2->SetName("hist_2");

  //plot the histograms together
  TCanvas* c = (TCanvas*) c1_->Clone();
  c->SetName("c1_clone");
  c->cd();
  c->Draw();
  h1->SetLineColor(kBlue);
  h1->SetFillColor(kGray);
  h1->SetFillStyle(3001);
  h1->SetLineWidth(2);
  if(normalize) h1->Scale(1./h1->Integral());
  h1->Draw("hist");
  h1->SetTitle(title1_.Data());
  
  // h2->SetLineColor(kGray);
  h2->SetMarkerColor(kRed);
  h2->SetLineColor(kGray+1);
  
  h2->SetFillStyle(0);
  h2->SetLineWidth(2);
  if(normalize) h2->Scale(1./h2->Integral());
  h2->Draw("same");
  h2->SetTitle(title2_.Data());
  h1->SetAxisRange((normalize) ? h1->GetMaximum()/1e4 : 1.e-1, 1.1*max(h1->GetMaximum(), h2->GetMaximum()), "Y");
  c->BuildLegend();

  //get the Data/MC histogram just for the x title and x range
  TPad* pad2_2 = (TPad*) c2_->GetPrimitive("pad2");
  if(!pad2_2) {
    cout << "Failed to find pad 2 in canvas 2!\n";
    return NULL;
  }
  TH1F* hDataMC_2 = (TH1F*) pad2_2->GetPrimitive("hDataMC");
  if(!hDataMC_2) {
    cout << "Failed to find data/mc in pad 2 in canvas 2!\n";
    return NULL;
  }
  h1->SetTitle(label.Data());
  h1->SetXTitle(hDataMC_2->GetXaxis()->GetTitle());
  if(xmax < xmin) h1->SetAxisRange(hstack->GetXaxis()->GetXmin(), hstack->GetXaxis()->GetXmax(), "X");
  else            h1->SetAxisRange(xmin, xmax, "X");

  
  TString print_path = print_path_;
  print_path.ReplaceAll(".png", Form("_%s.png", (label + ((normalize) ? "_norm" : "")).Data()));
  c->Print(print_path.Data());
  print_path.ReplaceAll(".png", "_log.png");
  c->SetLogy();
  c->Print(print_path.Data());
  return c;
}



Int_t initialize_canvases(const char* fileonename, const char* filetwoname) {
  //clean up last runs if existing
  if(f1_)
    delete f1_;
  if(f2_)
    delete f2_;

  if(c1_)
    delete c1_;
  if(c2_)
    delete c2_;
  
  //get files, check they exist
  f1_ = TFile::Open(fileonename, "READ");
  f2_ = TFile::Open(filetwoname, "READ");
  if(!f1_ || !f2_)
    return 1;

  print_path_ = fileonename;
  print_path_.ReplaceAll("canvases", "figures");
  print_path_.ReplaceAll(".root", "_compare.png");
  //loop through the files, save the first canvas found
  c1_ = 0;
  TKey* key = 0;
  TIter nextkey(f1_->GetListOfKeys());
  while((key = (TKey*) nextkey())) {
    TObject* obj = key->ReadObj();
    if(obj->InheritsFrom(TCanvas::Class())) {
      c1_ = (TCanvas*) obj;
      break;
    }
  }
  if(!c1_) {
    cout << "Canvas in " << fileonename << " not found!\n";
    return 2;
  }
  c1_->SetName(Form("%s_1", c1_->GetName()));

  c2_ = 0;
  key = 0;
  TIter nextkey2(f2_->GetListOfKeys());
  while((key = (TKey*) nextkey2())) {
    TObject* obj = key->ReadObj();
    if(obj->InheritsFrom(TCanvas::Class())) {
      c2_ = (TCanvas*) obj;
      break;
    }
  }
  if(!c2_) {
    cout << "Canvas in " << filetwoname << " not found!\n";
    return 2;
  }
  c2_->SetName(Form("%s_2", c2_->GetName()));

  gStyle->SetOptStat(0);
  return 0;
}

Int_t print_standard_comparisons() {
  Int_t status = 0;
  status += initialize_canvases("canvases/clfv_zdecays/mutau/wjetbinned_twopt_8.root", "canvases/clfv_zdecays/mutau/wjetamcnlo_twopt_8.root");
  compare_mc("W+Jets",false, 15, 60);
  compare_mc("W+Jets",true , 15, 60);
  compare_dataMC();
  status += initialize_canvases("canvases/clfv_zdecays/mutau/wjetbinned_onept_8.root", "canvases/clfv_zdecays/mutau/wjetamcnlo_onept_8.root");
  compare_mc("W+Jets",false, 20, 100);
  compare_mc("W+Jets",true , 20, 100);
  compare_dataMC();
  status += initialize_canvases("canvases/clfv_zdecays/mutau/wjetbinned_leppt_8.root", "canvases/clfv_zdecays/mutau/wjetamcnlo_leppt_8.root");
  compare_mc("W+Jets",false, 0, 100);
  compare_mc("W+Jets",true , 0, 100);
  compare_dataMC();
  status += initialize_canvases("canvases/clfv_zdecays/mutau/wjetbinned_lepm_8.root" , "canvases/clfv_zdecays/mutau/wjetamcnlo_lepm_8.root");
  compare_mc("W+Jets",false, 10, 200);
  compare_mc("W+Jets",true , 10, 200);
  compare_dataMC();
  status += initialize_canvases("canvases/clfv_zdecays/mutau/wjetbinned_njets_log_8.root" , "canvases/clfv_zdecays/mutau/wjetamcnlo_njets_log_8.root");
  compare_mc("W+Jets",false, 0, 10);
  compare_mc("W+Jets",true , 0, 10);
  compare_dataMC();
  return status;
}
