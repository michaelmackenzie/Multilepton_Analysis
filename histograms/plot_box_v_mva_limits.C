/**
   Script to overlay the limit gain vs signal efficiency using the
   MVA training and using the box cuts
 **/
TString box_path_ = "../CutsetTraining/scripts/";
TString mva_path_ = "canvases/clfv_zdecays/";

TCanvas* plot_limits(TString selection, int set) {

  bool isHiggs = selection.Contains("h");
  TString mva_file = mva_path_;
  TString box_file = box_path_;
  TString selec = "";
  TString hist = "";
  if(selection.Contains("mutau")) {
    selec = "mutau";
    if(isHiggs) hist = "mva0";
    else        hist = "mva1";
  } else if(selection.Contains("etau")) {
    selec = "etau";
    if(isHiggs) hist = "mva2";
    else        hist = "mva3";
  } else if(selection.Contains("emu")) {
    selec = "emu";
    if(isHiggs) hist = "mva4";
    else        hist = "mva5";
  } else {
    printf("Unknown selection %s!\n", selection.Data());
    return NULL;
  }
  //first build mva file path
  mva_file += selec;
  mva_file += "/sig_vsEff_";
  if(isHiggs) mva_file += "H->";
  else        mva_file += "Z->";
  mva_file += selec;
  mva_file += "_";
  mva_file += hist;
  mva_file += "_set_";
  mva_file += set;
  mva_file += ".root";

  //next build box file path
  box_file += "training_background_ztautau_";
  if(isHiggs) box_file += "higgs_";
  else        box_file += "Z0_";
  box_file += selec;
  box_file += "_";
  box_file += set;
  box_file += ".root";

  TFile* fmva = TFile::Open(mva_file.Data(), "READ");
  TFile* fbox = TFile::Open(box_file.Data(), "READ");
  if(!fmva || !fbox)
    return NULL;

  //Get the canvases from each
  TCanvas* cmva = (TCanvas*) fmva->Get("sig");
  if(!cmva)
    return NULL;
  TCanvas* cbox = (TCanvas*) fbox->Get("Function_Canvas");
  if(!cbox)
    return NULL;

  //get the box cut graph and draw it on the mva cut canvas
  cmva->cd();
  cmva->Draw();
  TGraph* gbox = (TGraph*) cbox->GetPrimitive("Function_Graph");
  if(!gbox)
    return NULL;
  gbox->SetLineColor(kGreen+2);
  auto leg = (TLegend*) cmva->GetPrimitive("TPave");
  for(TObject* o : *(cmva->GetListOfPrimitives())) {
    if(o->InheritsFrom(TGraph::Class())) {
      auto gmva = (TGraph*) o;
      auto le = leg->GetEntry();
      le->SetLabel("MVA Cut Limit Gain");
      // leg->GetEntry(gmva, "MVA Cut Limit Gain", "L");
    }
  }
  gbox->Draw();
  leg->AddEntry(gbox, "Box Cut Limit Gain", "L");
  return cmva;
}

TCanvas* print_limits(TString selection, int set) {
  TCanvas* c = plot_limits(selection, set);
  if(!c) return c;
  TString selec = "";
  if(selection.Contains("mutau")) {
    selec = "mutau";
  } else if(selection.Contains("etau")) {
    selec = "etau";
  } else if(selection.Contains("emu")) {
    selec = "emu";
  } else {
    printf("Unknown selection %s!\n", selection.Data());
    return NULL;
  }
  c->Print(Form("figures/clfv_zdecays/%s/sig_%s_mva_v_box_set_%i.png", selec.Data(), selection.Data(), set));
  return c;
}
