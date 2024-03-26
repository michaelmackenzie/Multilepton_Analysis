#ifndef __CONVERT_SIMULTANEOUS__
#define __CONVERT_SIMULTANEOUS__

//Convert a RooSimultaneous to a RooMultiPdf
RooMultiPdf* make_multipdf(RooSimultaneous& sim, RooCategory& cat) {
  sim.SetName("roosim_bkg");
  RooArgList pdfs;
  for(int ipdf = 0; ipdf < cat.numTypes(); ++ipdf) {
    auto pdf = sim.getPdf(Form("index_%i", ipdf));
    pdfs.add(*pdf);
  }
  RooMultiPdf* multi_pdf = new RooMultiPdf("bkg", "MultiPdf background", cat, pdfs);
  return multi_pdf;
}

int convert_simultaneous_to_multipdf(TString filein, TString fileout) {

  TFile* fIn = TFile::Open(filein.Data(), "READ");
  if(!fIn) return 1;
  TFile* fOut = new TFile(fileout.Data(), "RECREATE");

  for(auto key : *(fIn->GetListOfKeys())) {
    TObject* o = fIn->Get(key->GetName());
    if(o->IsA()->InheritsFrom(RooWorkspace::Class())) {
      cout << key->GetName() << ": RooWorkspace\n";
      RooWorkspace* ws = (RooWorkspace*) o;
      RooSimultaneous* sim = (RooSimultaneous*) ws->pdf("bkg");
      if(!sim) {
        cout << "RooSimultaneous not found in RooWorkspace " << ws->GetName() << endl;
        return 2;
      }
      TString name = ws->GetName();
      name.ReplaceAll("lepm", "cat");
      RooCategory* cat = (RooCategory*) ws->obj(name.Data());
      if(!cat) {
        cout << "RooCategory not found in RooWorkspace " << ws->GetName() << endl;
        return 3;
      }
      RooMultiPdf* multi = make_multipdf(*sim, *cat);
      ws->import(*multi, RooFit::RecycleConflictNodes());
    }
    fOut->cd();
    o->Write();
  }
  fOut->Close();
  fIn->Close();
  return 0;
}

#endif
