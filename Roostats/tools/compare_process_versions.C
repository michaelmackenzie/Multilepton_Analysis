// Compare process distributions in two different histogram versions

//---------------------------------------------------------------------------------------------------
int print_hists(TH1* h_1, TH1* h_2, TString name) {
  if(!h_1 || !h_2) return 1;

  gStyle->SetOptStat(0);
  TCanvas c("c","c",900,700);
  TPad pad1("pad1", "pad1", 0., 0.3, 1., 1.0); pad1.Draw();
  TPad pad2("pad2", "pad2", 0., 0.0, 1., 0.3); pad2.Draw();
  pad1.SetBottomMargin(0.03);
  pad2.SetTopMargin(0.03);

  pad1.cd();
  h_1->SetLineColor(kBlue);
  h_1->SetMarkerColor(kBlue);
  h_1->SetLineWidth(2);
  h_1->Draw("E1");
  h_2->SetLineColor(kRed);
  h_2->SetMarkerColor(kRed);
  h_2->SetMarkerStyle(24);
  h_2->SetLineWidth(2);
  h_2->Draw("E1 same");

  h_1->GetYaxis()->SetRangeUser(0., 1.2*max(h_1->GetMaximum(), h_2->GetMaximum()));
  h_1->SetTitle("");
  h_1->GetXaxis()->SetLabelSize(0.);

  pad2.cd();
  TH1* h_r = (TH1*) h_2->Clone("hratio");
  h_r->Divide(h_1);
  h_r->Draw("E1");
  h_r->GetYaxis()->SetRangeUser(0., 2.);
  h_r->SetTitle("");
  TLine line(h_r->GetXaxis()->GetXmin(), 1., h_r->GetXaxis()->GetXmax(), 1.);
  line.SetLineWidth(2);
  line.SetLineStyle(kDashed);
  line.SetLineColor(kBlack);
  line.Draw("same");

  h_r->GetXaxis()->SetLabelSize(0.08);
  h_r->GetYaxis()->SetLabelSize(0.08);



  c.SaveAs(name);

  delete h_r;
  return 0;
}

//---------------------------------------------------------------------------------------------------
TH1* get_histogram(TFile* f, TString process, TString h_name) {
  if(!f) return nullptr;
  auto stack = (THStack*) f->Get("hstack");
  if(!stack) return nullptr;
  TH1* h = nullptr;
  for(auto o : *(stack->GetHists())) {
    TString name = o->GetName();
    name.ReplaceAll("#", "");
    name.ReplaceAll("->", "To");
    name.ReplaceAll("/", "");
    name.ReplaceAll(" ", "");
    name.ReplaceAll("tautauEmbedding", "Embedding");
    if(name.BeginsWith(process)) {
      h = (TH1*) o->Clone(h_name);
      h->SetDirectory(0);
      break;
    }
  }
  return h;
}

//---------------------------------------------------------------------------------------------------
int compare_process_versions(TString process = "QCD", //"ZToeemumu",
                             TString selection = "zmutau",
                             TString dir_1 = "histograms_previous/hist_v09e/",
                             TString dir_2 = "histograms_previous/hist_v12/",
                             TString fig_dir = "figures/compare_v09e_v12") {

  if(!fig_dir.EndsWith("/")) fig_dir += "/";
  gSystem->Exec(Form("[ ! -d %s ] && mkdir -p %s", fig_dir.Data(), fig_dir.Data()));
  int status(0);

  //Compare the distributions in each year and mass region
  vector<int> years = {2016,2017,2018};
  vector<TString> cats = {"had", "lep"};
  vector<int> sets = {25,26,27,28};

  for(int year : years) {
    for(auto cat : cats) {
      for(int set : sets) {
        if(cat == "lep" && set == 28) continue;
        if(cat == "had" && process == "QCD") continue;
        if(cat == "lep" && process == "MisID") continue;

        TString mva = "mva";
        if(selection == "zmutau") mva += (cat == "had") ? "1" : "7";
        if(selection == "zetau" ) mva += (cat == "had") ? "3" : "9";
        //Open the relevant files
        TString name = Form("%s%s_%s_%i_%i", selection.Data(), (cat == "lep") ? (selection == "zmutau") ? "_e" : "_mu" : "",
                            mva.Data(), set, year);
        auto f_1 = TFile::Open(dir_1+name+".hist", "READ");
        auto f_2 = TFile::Open(dir_2+name+".hist", "READ");
        if(!f_1 || !f_2) {++status; continue;}
        auto h_1 = get_histogram(f_1, process, process+"_1");
        auto h_2 = get_histogram(f_2, process, process+"_2");
        status += print_hists(h_1, h_2, fig_dir + process + "_" + name + ".png");
        f_1->Close();
        f_2->Close();
      }
    }
  }
  return status;
}
