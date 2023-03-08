//Test the scale systematic uncertainty implementation
#include "dataplotter_clfv.C"

int test_scale_sys(TString selection = "mutau", TString path = "nanoaods_mva_dev_02") {
  years_={2016,2017,2018}; useEmbed_ = 1; hist_tag_ = "clfv"; splitDY_ = 1; splitWJ_ = 1; useLepTauSet_ = 1; includeHiggs_ = 0;
  int status = nanoaod_init(selection.Data(), path.Data());
  if(status) return status;

  int set = 8;
  if     (selection == "mutau"  ) set += HistMaker::kMuTau;
  else if(selection == "etau"   ) set += HistMaker::kETau;
  else if(selection == "emu"    ) set += HistMaker::kEMu;
  else if(selection == "mutau_e") set += HistMaker::kMuTauE;
  else if(selection == "etau_mu") set += HistMaker::kETauMu;
  else if(selection == "mumu"   ) set += HistMaker::kMuMu;
  else if(selection == "ee"     ) set += HistMaker::kEE;

  //10% MC/Embedding scale uncertainty
  CLFV::ScaleUncertainty_t sys_up  ("Test-up"  , 1.10, "", "", "", -1, false, true, false);
  CLFV::ScaleUncertainty_t sys_down("Test-down", 0.90, "", "", "", -1, false, true, false);

  dataplotter_->rebinH_ = 5;
  auto hdata   = dataplotter_->get_data  ("lepm", "event", set);
  auto hstack  = dataplotter_->get_stack ("lepm", "event", set);
  auto signals = dataplotter_->get_signal("lepm", "event", set);

  if(!hdata || !hstack || signals.size() == 0) {
    std::cout << "Nominal histograms not found!\n";
    return 10;
  }


  hdata = (TH1*) hdata->Clone("nominal_data");
  TH1* hbkg = (TH1*) hstack->GetStack()->Last()->Clone("nominal_bkg");
  TH1* hsig = (TH1*) signals[0]->Clone("nominal_sig");

  auto hdata_up   = dataplotter_->get_data  ("lepm", "event", set, &sys_up);
  auto hstack_up  = dataplotter_->get_stack ("lepm", "event", set, &sys_up);
  auto signals_up = dataplotter_->get_signal("lepm", "event", set, &sys_up);
  hdata_up = (TH1*) hdata_up->Clone("up_data");
  TH1* hbkg_up = (TH1*) hstack_up->GetStack()->Last()->Clone("up_bkg");
  TH1* hsig_up = (TH1*) signals_up[0]->Clone("up_sig");

  auto hdata_down   = dataplotter_->get_data  ("lepm", "event", set, &sys_down);
  auto hstack_down  = dataplotter_->get_stack ("lepm", "event", set, &sys_down);
  auto signals_down = dataplotter_->get_signal("lepm", "event", set, &sys_down);
  hdata_down = (TH1*) hdata_down->Clone("down_data");
  TH1* hbkg_down = (TH1*) hstack_down->GetStack()->Last()->Clone("down_bkg");
  TH1* hsig_down = (TH1*) signals_down[0]->Clone("down_sig");

  hbkg->SetLineColor(kRed);
  hbkg->SetLineWidth(3);
  hbkg->SetFillColor(0);
  hsig->SetLineColor(kBlue);
  hsig->SetFillColor(0);
  hsig->SetLineWidth(3);

  printf("Nominal: Data = %.0f; Bkg = %.1f; Signal = %.2f\n",
         hdata->Integral(), hbkg->Integral(), hsig->Integral());
  printf("Up     : Data = %.0f; Bkg = %.1f; Signal = %.2f\n",
         hdata_up->Integral(), hbkg_up->Integral(), hsig_up->Integral());
  printf("Down   : Data = %.0f; Bkg = %.1f; Signal = %.2f\n",
         hdata_down->Integral(), hbkg_down->Integral(), hsig_down->Integral());

  auto bkg_err = dataplotter_->get_errors(hbkg, hbkg_up, hbkg_down);
  auto sig_err = dataplotter_->get_errors(hsig, hsig_up, hsig_down);
  bkg_err->SetFillColor(kRed);
  sig_err->SetFillColor(kBlue);

  TCanvas c;
  hdata->Draw("E1");
  bkg_err->Draw("E2");
  hbkg->Draw("hist same");
  sig_err->Draw("E2");
  hsig->Draw("hist same");
  hdata->GetXaxis()->SetRangeUser(50., 170.);
  hdata->SetTitle("Scale uncertainty test");

  c.SaveAs("test_scale_sys.png");
  return 0;
}
