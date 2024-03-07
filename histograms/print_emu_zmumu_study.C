//Print figures studying Z->e+mu vs Z->mu+mu
#include "dataplotter_clfv.C"
const bool show_updated_ = true;

struct hist_t {
  TString hist_;
  TString title_;
  double xmin_;
  double xmax_;
  TString type_;
  int set_offset_;
  bool logy_;
  hist_t(TString hist, TString title, double xmin = 1., double xmax = -1.,
         TString type = "lep", int set_offset = 0, bool logy = false) :
    hist_(hist), title_(title), xmin_(xmin), xmax_(xmax),
    type_(type), set_offset_(set_offset), logy_(logy) {}
};

int print_figure(hist_t& data, const char* dir) {
  TString hist = data.hist_;
  TString type = data.type_;
  const char* title = data.title_.Data();
  double xmin = data.xmin_;
  double xmax = data.xmax_;
  int offset = data.set_offset_;

  //get the nominal and updated histograms
  auto hsig_n = dataplotter_->get_process("Z->e#mu (10^{-5})", hist, type, 208+offset);
  auto hbkg_n = dataplotter_->get_process("Z->ee/#mu#mu"     , hist, type, 208+offset);
  auto hsig   = dataplotter_->get_process("Z->e#mu (10^{-5})", hist, type, 220+offset);
  auto hbkg   = dataplotter_->get_process("Z->ee/#mu#mu"     , hist, type, 220+offset);
  if(!hsig_n || !hsig) {
    cout << "Signal " << hist.Data() << " histogram not found\n";
    return 1;
  }
  if(!hbkg_n || !hbkg) {
    cout << "Background " << hist.Data() << " histogram not found\n";
    return 1;
  }
  if(hbkg_n->Integral() <= 0. || hsig_n->Integral() <= 0.) {
    cout << "Nominal histogram " << hist.Data() << " not filled!\n";
  }
  if(hbkg->Integral() <= 0. || hsig->Integral() <= 0.) {
    cout << "Updated histogram " << hist.Data() << " not filled!\n";
  }

  hsig  ->Scale(1./hsig_n->Integral()); //normalize to the starting rates
  hbkg  ->Scale(1./hbkg_n->Integral());
  hsig_n->Scale(1./hsig_n->Integral());
  hbkg_n->Scale(1./hbkg_n->Integral());

  //Create the canvas
  TCanvas* c = new TCanvas(("c"+hist).Data(), ("c"+hist).Data(), 1000, 1100);
  TPad* pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1.0); pad1->Draw();
  TPad* pad2 = new TPad("pad2", "pad2", 0., 0.0, 1., 0.3); pad2->Draw();
  pad1->SetBottomMargin(0.05); pad2->SetTopMargin(0.03);

  //Draw the histograms
  pad1->cd();
  pad1->SetGrid();
  hsig_n->SetLineColor(kBlue);
  hsig_n->SetLineWidth(3);
  hsig  ->SetLineColor(hsig_n->GetLineColor());
  hsig  ->SetLineStyle(kDashed);
  hsig  ->SetLineWidth(3);
  hbkg_n->SetLineColor(kRed);
  hbkg_n->SetLineWidth(3);
  hbkg  ->SetLineColor(hbkg_n->GetLineColor());
  hbkg  ->SetLineStyle(kDashed);
  hbkg  ->SetLineWidth(3);
  hsig_n->Draw("hist");
  if(show_updated_)
    hsig  ->Draw("hist same");
  hbkg_n->Draw("hist same");
  if(show_updated_)
    hbkg  ->Draw("hist same");
  hsig_n->SetTitle(title);
  if(xmax < xmin) {
    xmin = hsig_n->GetXaxis()->GetXmin();
    xmax = hsig_n->GetXaxis()->GetXmax();
  }
  hsig_n->GetYaxis()->SetRangeUser(0, 1.3*max(hsig_n->GetMaximum(), hbkg_n->GetMaximum())); //nominals must be higher than updated
  hsig_n->GetXaxis()->SetRangeUser(xmin, xmax);

  TLegend* leg = new TLegend(0.2, 0.75, 0.8, 0.9);
  leg->SetNColumns(2);
  leg->AddEntry(hsig_n, "Nominal Z->e#mu"  , "L" );
  if(show_updated_)
    leg->AddEntry(hsig  , Form("Updated Z->e#mu (#epsilon = %.3f)", hsig->Integral())  , "L" );
  leg->AddEntry(hbkg_n, "Nominal Z->#mu#mu", "L" );
  if(show_updated_)
    leg->AddEntry(hbkg  , Form("Updated Z->#mu#mu (#epsilon = %.3f)", hbkg->Integral())  , "L" );
  leg->Draw();

  //Draw the ratio plots
  pad2->cd();
  pad2->SetGrid();
  TH1* hr_n = (TH1*) hsig_n->Clone(Form("%s_ratio", hsig_n->GetName()));
  hr_n->Divide(hbkg_n);
  hr_n->SetLineColor(hsig_n->GetLineColor());
  hr_n->SetLineWidth(3);
  TH1* hr = (TH1*) hsig->Clone(Form("%s_ratio", hsig->GetName()));
  hr->Divide(hbkg);
  hr->Scale(hbkg->Integral() / hsig->Integral()); //remove any normalization difference
  hr->SetLineColor(hsig->GetLineColor());
  hr->SetLineStyle(hsig->GetLineStyle());
  hr->SetLineWidth(3);

  hr_n->Draw("L E1");
  if(show_updated_)
    hr  ->Draw("L E1 same");
  pad2->Update();

  hr_n->SetTitle("");
  hr_n->SetXTitle("");
  hr_n->GetYaxis()->SetTitle("Sig/Bkg");
  hr_n->GetYaxis()->SetTitleSize(0.13);
  hr_n->GetYaxis()->SetTitleOffset(0.30);
  hr_n->GetYaxis()->SetLabelSize(0.08);
  hr_n->GetXaxis()->SetLabelSize(0.08);
  hr_n->GetYaxis()->SetRangeUser(0., 3.);
  hr_n->GetXaxis()->SetRangeUser(xmin, xmax);

  TLine* line = new TLine(xmin, 1., xmax, 1.);
  line->SetLineColor(kBlack);
  line->SetLineStyle(kDashed);
  line->SetLineWidth(2);
  line->Draw("same");

  if(offset == 0)
    c->SaveAs(Form("%s/%s.png", dir, hist.Data()));
  else
    c->SaveAs(Form("%s/%s_offset_%i.png", dir, hist.Data(), offset));
  delete c;

  return 0;
}


int print_emu_zmumu_study(TString dir = "nanoaods_emu_zmumu_study", vector<int> years = {2016,2017,2018}) {
  //setup the datacards
  years_     = years;
  hist_tag_  = "clfv";
  useEmbed_  = 0;

  TString tag = "";
  if(dir.EndsWith("endcap")) tag += "_endcap";
  if(dir.EndsWith("barrel")) tag += "_barrel";
  if(!show_updated_) tag += "_base";
  int status(0);

  status += nanoaod_init("emu", dir, Form("zmumu_study%s", tag.Data()));
  if(status) return status;

  vector<hist_t> hists = {
                                         hist_t("eleconvveto"     , "Electron conversion veto ID"),
                                         hist_t("elepfcand"       , "Electron PF candidate ID"),
                                         hist_t("elelosthits"     , "Electron N(lost inner hits)"),
                                         hist_t("eleetaeta"       , "Electron #sigma_{i#etai#eta}"),
                                         hist_t("eler9"           , "Electron R9"),
                                         hist_t("elecutid"        , "Electron cut-based ID"),
                                         hist_t("eleenergyerr"    , "Electron energy error / pT"),
                                         hist_t("ele3dpverr"      , "Electron 3D PV displacement #sigma"),
                                         hist_t("elescetoverpt"   , "Electron Super-cluster E_{T} / p_{T}"),
                                         hist_t("elehoe"          , "Electron H / E", 0., 0.15),
                                         hist_t("eleeinvminuspinv", "Electron 1/E - 1/P", -0.1, 0.05),
                                         hist_t("onereliso"       , "Electron rel. isolation"),
                                         hist_t("oneid1"          , "Electron ID", 0., 5.),
                                         hist_t("onedxy"          , "Electron d_{xy}"),
                                         hist_t("onedz"           , "Electron d_{z}"),
                                         hist_t("onedxysig"       , "Electron d_{xy} #sigma"),
                                         hist_t("onedzsig"        , "Electron d_{z} #sigma"),
                                         hist_t("dxydiff"         , "#Deltad_{xy}", -0.04, 0.04),
                                         hist_t("dzdiff"          , "#Deltad_{z}"),
                                         hist_t("onept"           , "Electron p_{T}"),
                                         hist_t("ptratio"         , "Electron p_{T} / Muon p_{T}"),
                                         hist_t("onepz"           , "Electron p_{Z}"),
                                         hist_t("oneeta"          , "Electron #eta"),
                                         hist_t("onesceta"        , "Electron #eta_{SC}"),
                                         hist_t("onejetoverlap"   , "Electron jet overlap ID"),
                                         hist_t("onemetdeltaphi"  , "Electron #Delta#phi(e, MET)"),
                                         hist_t("onegenflavor"    , "Electron gen-level flavor"),
                                         hist_t("twopt"           , "Muon p_{T}"),
                                         hist_t("twoeta"          , "Muon #eta"),
                                         hist_t("twogenflavor"    , "Muon gen-level flavor"),
                                         hist_t("lepm"            , "Di-lepton mass", 70., 110., "event"),
                                         hist_t("lepm"            , "Same-sign Di-lepton mass", 70., 110., "event", 1000)
  };

  TString year_s = Form("%i", years[0]);
  for(int i = 1; i < years.size(); ++i) year_s += Form("_%i", years[i]);
  for(hist_t& hist : hists) status += print_figure(hist, Form("figures/zmumu_study%s/emu/%s", tag.Data(), year_s.Data()));
  return status;
}
