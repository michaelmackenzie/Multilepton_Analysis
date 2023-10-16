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
      hbkg = (TH1*) hbkg->Clone(Form("%s_plt"));
      TH1* hbkg_sys = (TH1*) shifted->GetStack()->Last();
      hbkg_sys = (TH1*) hbkg_sys->Clone(Form("%s_plt"));

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
  };
}
#endif
