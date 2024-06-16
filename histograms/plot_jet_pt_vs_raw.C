// Compare jet pT correction for data, Embedding, and MC jets

int year_;

struct plot_card_t {
  TString var_;
  double xmin_;
  double xmax_;
  int nbin_;
  TString name_;
  TString title_;
  bool log_;
  TString cut_;
  plot_card_t(TString var, double xmin, double xmax, int nbin, TString name, TString title,
              bool log = false, TString cut = "") : var_(var), xmin_(xmin), xmax_(xmax), nbin_(nbin),
                                                    name_(name), title_(title), log_(log), cut_(cut) {}
};


int print_figure(plot_card_t card, TTree* t_data, TTree* t_embed, TTree* t_mc) {
  // Create and fill the histograms

  const double xmin(card.xmin_), xmax(card.xmax_);
  TH1* h_data  = new TH1D("h_data" , card.title_.Data(), card.nbin_, xmin, xmax);
  TH1* h_embed = new TH1D("h_embed", card.title_.Data(), card.nbin_, xmin, xmax);
  TH1* h_mc    = new TH1D("h_mc"   , card.title_.Data(), card.nbin_, xmin, xmax);

  const char* jet_cut = "nJet > 0 && Jet_jetId[0] >= 6 && (Jet_pt[0] > 50. || Jet_puId[0] >= 6)";
  const char* mumu_cut = "SelectionFilter_LepM > 80. && SelectionFilter_LepM < 100. && nMuon == 2 && Muon_charge[0]*Muon_charge[1] < 0";
  const char* cut = Form("%s && %s %s", jet_cut, mumu_cut,
                         (card.cut_ == "") ? "" : ("&& " + card.cut_).Data());
  TString d_var(card.var_), m_var(card.var_);
  d_var.ReplaceAll("_nom", ""); //use normal branches, not JER corrected for data
  t_data ->Draw(Form("%s >> h_data" , d_var.Data()), cut, "goff", 1e6);
  t_embed->Draw(Form("%s >> h_embed", d_var.Data()), cut, "goff", 1e6);
  t_mc   ->Draw(Form("%s >> h_mc"   , m_var.Data()), cut, "goff", 1e6);

  if(h_mc->Integral() == 0. || h_embed->Integral() == 0. || h_data->Integral() == 0.) return 1;

  // Plot the results

  TCanvas c("c","c",600,700);
  TPad pad1("pad1", "pad1", 0., 0.3, 1., 1.0); pad1.Draw();
  TPad pad2("pad2", "pad2", 0., 0.0, 1., 0.3); pad2.Draw();
  pad1.SetBottomMargin(0.05);
  pad2.SetTopMargin(0.03);
  pad2.SetBottomMargin(0.20);

  pad1.cd();
  gStyle->SetOptStat(0);

  h_data ->Scale(1./h_data ->Integral());
  h_embed->Scale(1./h_embed->Integral());
  h_mc   ->Scale(1./h_mc   ->Integral());

  h_data->SetLineWidth(2);
  h_data->SetMarkerStyle(20);
  h_data->SetMarkerSize(0.8);
  h_data->Draw("E1");
  const double max_val = max(h_data->GetMaximum(), max(h_embed->GetMaximum(), h_mc->GetMaximum()));
  h_data->GetYaxis()->SetRangeUser((card.log_) ? max_val/1.e4 : 0., ((card.log_) ? 3. : 1.25)*max_val);
  if(card.log_) pad1.SetLogy();

  h_embed->SetLineWidth(2);
  h_embed->SetLineColor(kBlue);
  h_embed->Draw("hist same");

  h_mc->SetLineWidth(2);
  h_mc->SetLineColor(kRed);
  h_mc->Draw("hist same");

  // Add a legend
  TLegend leg(0.6, 0.75, 0.9, 0.9);
  leg.AddEntry(h_data, "Data");
  leg.AddEntry(h_embed, "Embedding");
  leg.AddEntry(h_mc, "MC");
  leg.Draw();

  // Make the ratio plot
  pad2.cd();

  TH1* h_embed_r = (TH1*) h_embed->Clone("h_embed_r"); h_embed_r->Divide(h_data);
  TH1* h_mc_r    = (TH1*) h_mc   ->Clone("h_mc_r"   ); h_mc_r   ->Divide(h_data);
  h_embed_r->SetTitle(";;Sim/Data");
  h_embed_r->Draw("hist");
  h_mc_r   ->Draw("hist same");
  TLine line(xmin, 1., xmax, 1.);
  line.SetLineWidth(2);
  line.SetLineStyle(kDashed);
  line.SetLineColor(kBlack);
  line.Draw("same");
  h_embed_r->GetYaxis()->SetRangeUser(0.5, 1.5);
  h_embed_r->GetXaxis()->SetLabelSize(0.08);
  h_embed_r->GetYaxis()->SetLabelSize(0.08);
  h_embed_r->GetXaxis()->SetTitleSize(0.1);
  h_embed_r->GetYaxis()->SetTitleSize(0.1);
  h_embed_r->GetXaxis()->SetTitleOffset(0.9);
  h_embed_r->GetYaxis()->SetTitleOffset(0.45);

  // Print the figure
  const char* dir = "figures/jet_pt_vs_raw";
  gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s", dir, dir));
  c.SaveAs(Form("%s/%s_%i.png", dir, card.name_.Data(), year_));

  // Clean up
  delete h_data;
  delete h_embed;
  delete h_mc;
  delete h_embed_r;
  delete h_mc_r;

  return 0;
}

int plot_jet_pt_vs_raw(const int year = 2018) {

  year_= year;

  //Retrieve the data files

  const char* path = "root://cmseos.fnal.gov//store/user/mmackenz/lfvanalysis_rootfiles";
  auto f_data  = TFile::Open(Form("%s/LFVAnalysis_SingleMuonRun%iB_%i.root", path, year, year                          ), "READ");
  auto f_embed = TFile::Open(Form("%s/LFVAnalysis_Embed-MuMu-B_%i.root"    , path, year                                ), "READ");
  auto f_mc    = TFile::Open(Form("%s/LFVAnalysis_DY50-%s_%i.root"         , path, (year == 2017) ? "ext" : "amc", year), "READ");

  if(!f_data || !f_embed || !f_mc) return 1;

  // Retrieve the ntuples

  auto t_data  = (TTree*) f_data ->Get("mumu");
  auto t_embed = (TTree*) f_embed->Get("mumu");
  auto t_mc    = (TTree*) f_mc   ->Get("mumu");

  if(!t_data || !t_embed || !t_mc) return 2;

  int status = 0;
  status += print_figure(plot_card_t("Jet_rawFactor[0]", -0.5, 0.5, 50, "raw_factor", "Jet (pT - raw pT)/pT"), t_data, t_embed, t_mc);
  status += print_figure(plot_card_t("Jet_pt[0]", 10., 100, 40, "jet_pt", "Jet pT"), t_data, t_embed, t_mc);
  status += print_figure(plot_card_t("Jet_eta[0]", -5., 5., 40, "jet_eta", "Jet #eta"), t_data, t_embed, t_mc);
  status += print_figure(plot_card_t("nBJet", 0., 5., 5, "nbjet", "N(b-tagged jets)", true), t_data, t_embed, t_mc);
  status += print_figure(plot_card_t("nJet", 0., 7., 7, "njet", "N(jets)", true), t_data, t_embed, t_mc);
  status += print_figure(plot_card_t("Jet_puId[0]", 0., 7., 7, "puid", "Jet PU ID"), t_data, t_embed, t_mc);
  status += print_figure(plot_card_t("Jet_btagDeepB[0]", 0., 1., 40, "btag", "B-tag score", true), t_data, t_embed, t_mc);

  status += print_figure(plot_card_t("Jet_rawFactor[0]", -0.5, 0.5, 50, "raw_factor_b", "Jet (pT - raw pT)/pT, |#eta| < 1.5", false, "abs(Jet_eta[0]) < 1.5"),
                         t_data, t_embed, t_mc);
  status += print_figure(plot_card_t("Jet_rawFactor[0]", -0.5, 0.5, 50, "raw_factor_e", "Jet (pT - raw pT)/pT, 1.5 < |#eta| < 2.4", false, "abs(Jet_eta[0]) > 1.5 && abs(Jet_eta[0]) 2.4"),
                         t_data, t_embed, t_mc);
  status += print_figure(plot_card_t("Jet_rawFactor[0]", -0.5, 0.5, 50, "raw_factor_f", "Jet (pT - raw pT)/pT, |#eta| > 2.4", false, "abs(Jet_eta[0]) > 2.4"),
                         t_data, t_embed, t_mc);
  return status;
}
