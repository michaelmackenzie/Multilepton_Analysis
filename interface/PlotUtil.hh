#ifndef __PLOTUTIL__HH
#define __PLOTUTIL__HH

//c++ includes
#include <iostream>
#include <vector>

//ROOT includes
#include "TH1.h"
#include "TH2.h"
#include "THStack.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TAxis.h"
#include "TLine.h"
#include "TDirectory.h"
#include "TFolder.h"
#include "TTree.h"
#include "TBranch.h"
#include "TCanvas.h"
#include "TPad.h"

namespace CLFV {

  class PlotUtil {
  public:
    PlotUtil() {}
    ~PlotUtil() {}

    //Define relevant structs
    struct SysCard_t {
      THStack* nominal;
      THStack* up;
      THStack* down;
      std::vector<TH1*> signals;
      std::vector<TH1*> signals_up;
      std::vector<TH1*> signals_down;
      TH1* data;
    };

    //------------------------------------------------------------------------------------------------------------------------------------------------------
    static TCanvas* PlotShifts(SysCard_t card) {

      //Retrieve relevant inputs from the card
      THStack* nominal = card.nominal;
      THStack* up      = card.up;
      THStack* down    = card.down;
      TH1*     data    = card.data;
      std::vector<TH1*> signals      = card.signals;
      std::vector<TH1*> signals_up   = card.signals_up;
      std::vector<TH1*> signals_down = card.signals_down;

      //Check input is as expected
      if(nominal->GetNhists() == 0) {
        printf("PlotUtil::%s: No stacked histograms given!\n", __func__);
        return nullptr;
      }
      if(nominal->GetNhists() != up->GetNhists() || nominal->GetNhists() != down->GetNhists()) {
        printf("PlotUtil::%s: Nominal and shifted stacked histogram lists are not equal!\n", __func__);
        return nullptr;
      }
      if(signals.size() != signals_up.size() || signals.size() != signals_down.size()) {
        printf("PlotUtil::%s: Nominal and shifted signal histogram lists are not equal!\n", __func__);
        return nullptr;
      }

      //Get the overall background histograms
      TH1* hbkg = (TH1*) nominal->GetStack()->Last();
      hbkg = (TH1*) hbkg->Clone(Form("%s_plt", nominal->GetName()));
      TH1* hbkg_sys = (TH1*) up->GetStack()->Last();
      hbkg_sys = (TH1*) hbkg_sys->Clone(Form("%s_plt", up->GetName()));

      //Make the canvas layout
      TCanvas* c = new TCanvas("c_plot_shift", "c_plot_shift", 1200, 900);
      TPad* pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1. );
      TPad* pad2 = new TPad("pad2", "pad2", 0., 0. , 1., 0.3);
      pad1->Draw(); pad2->Draw();
      pad1->cd();

      hbkg->Draw("hist");
      hbkg_sys->Draw("hist same");
      if(data) data->Draw("E1 same");

      return c;
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------------
    // Plot the data, background, and signal including a data / background and (data + signal) / background
    static TCanvas* PlotRatio(TH1* data, TH1* background, TH1* signal, const bool signal_ratio = false, const bool log_y = false) {

      if(!data || !background) {
        printf("PlotUtil::%s: Need to provide data and background histograms\n", __func__);
        return nullptr;
      }

      // Make the canvas layout
      TCanvas* c = new TCanvas("c_plot_ratio", "c_plot_ratio", 1200, 900);
      TPad* pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1. );
      TPad* pad2 = new TPad("pad2", "pad2", 0., 0. , 1., 0.3);
      pad1->SetBottomMargin(0.06);
      pad2->SetTopMargin(0.03);
      pad1->Draw(); pad2->Draw();
      pad1->cd();

      data->Draw("E1");
      background->Draw("hist same");
      if(signal) signal->Draw("hist same");
      data->Draw("E1 same");

      const double max_val = std::max(data->GetMaximum(), (signal) ? std::max(signal->GetMaximum(), background->GetMaximum()) : background->GetMaximum());
      if(log_y) {data->GetYaxis()->SetRangeUser(0.1, 2.0*max_val); pad1->SetLogy();}
      else       data->GetYaxis()->SetRangeUser(0.1, 1.2*max_val);

      pad2->cd();
      TH1* ratio = (TH1*) data->Clone(Form("%s_ratio", background->GetName()));
      ratio->Divide(background);
      ratio->SetLineColor(background->GetLineColor());
      ratio->SetLineWidth(background->GetLineWidth());
      ratio->Draw("E1");
      ratio->GetYaxis()->SetRangeUser(0.5, 1.5);
      if(signal && signal_ratio) {
        TH1* sig_ratio = (TH1*) data->Clone(Form("%s_ratio", background->GetName()));
        sig_ratio->Add(signal);
        sig_ratio->Divide(background);
        sig_ratio->SetLineColor(signal->GetLineColor());
        sig_ratio->SetLineWidth(signal->GetLineWidth());
        sig_ratio->Draw("E1 same");
      }

      TLine* line = new TLine(ratio->GetXaxis()->GetXmin(), 1., ratio->GetXaxis()->GetXmax(), 1.);
      line->SetLineColor(kRed);
      line->SetLineWidth(2);
      line->Draw("same");

      ratio->SetTitle("");
      data ->SetXTitle("");
      ratio->GetXaxis()->SetLabelSize(0.09);
      ratio->GetYaxis()->SetLabelSize(0.09);
      ratio->GetYaxis()->SetTitleSize(0.1);
      ratio->GetYaxis()->SetTitleOffset(0.35);

      return c;
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------------
    // Plot the data, background, and signal including a data / background and (data + signal) / background
    static TCanvas* PlotRatio(TH1* data, THStack* background, TH1* signal, const bool signal_ratio = false, const bool log_y = false) {

      if(!data || !background) {
        printf("PlotUtil::%s: Need to provide data histogram and background stack\n", __func__);
        return nullptr;
      }
      auto c = PlotRatio(data, (TH1*) background->GetStack()->Last(), signal, signal_ratio, log_y);
      TPad* pad = (TPad*) c->GetListOfPrimitives()->At(0);
      if(pad) {
        pad->cd();
        background->Draw("hist noclear same");
        if(signal) signal->Draw("hist same");
        data->Draw("E1 same");
      }

      return c;
    }
  };
}
#endif
