#ifndef __TITLES__HH
#define __TITLES__HH

//c++ includes

//ROOT includes
#include "TString.h"

namespace CLFV {

  class Titles {
  public:
    Titles() {}
    ~Titles() {}

    //-----------------------------------------------------------------------------
    static void get_titles(TString hist, TString setType, TString selection, TString* xtitle, TString* ytitle, TString* title) {
      TString lep1("l1"), lep2("l2");
      if     (selection == "mutau"   ) {lep1 = "#mu" ; lep2 = "#tau";}
      else if(selection == "etau"    ) {lep1 = "e"   ; lep2 = "#tau";}
      else if(selection == "emu"     ) {lep1 = "e"   ; lep2 = "#mu" ;}
      else if(selection.Contains("_")) {lep1 = "e"   ; lep2 = "#mu" ;}
      else if(selection == "ee"      ) {lep1 = "e1"  ; lep2 = "e2"  ;}
      else if(selection == "mumu"    ) {lep1 = "#mu1"; lep2 = "#mu2";}
      hist.ReplaceAll("lepes_one", ""); //lepton energy scale variation histograms
      hist.ReplaceAll("lepes_two", "");
      if(hist == "lepdelrvsphi") {
        *xtitle = Form("#DeltaR");
        *ytitle = Form("#Delta#phi");
        *title  = Form("#DeltaR vs #Delta#phi Between Leptons");
      }
      else if(hist.Contains("prime")) {
        *xtitle = hist;
        xtitle->ReplaceAll("prime", " #tilde{");
        xtitle->ReplaceAll("met", "MET");
        xtitle->ReplaceAll("lepone", lep1.Data());
        xtitle->ReplaceAll("leptwo", lep2.Data());
        xtitle->ReplaceAll("0", "");
        xtitle->ReplaceAll("1", "");
        xtitle->ReplaceAll("2", "");
        if(xtitle->Contains("p")) {
          xtitle->ReplaceAll("p", "p_{");
          *xtitle = *xtitle + "}";
        }
        xtitle->ReplaceAll("{e", "{E");
        *xtitle = *xtitle + "}";
      }
      else if(hist == "sysm" && setType == "event") {
        *xtitle = "M_{ll#gamma} (GeV/c^{2})";
        *ytitle = "";
        *title  = Form("Mass of the Photon + Lepton System");
      }
      else if(hist == "ntriggered") {
        *xtitle = "N(triggering leptons)";
        *ytitle = "";
        *title  = Form("N(leptons) that fired a trigger");
      }
      else if(hist == "met") {
        *xtitle = "Missing Transverse Energy (GeV)";
        *ytitle = "";
        *title  = Form("Missing Transverse Energy");
      }
      else if(hist == "metsignificance" || hist == "puppimetsig") {
        *xtitle = "MET significance";
        *ytitle = "";
        *title  = Form("MET significance");
      }
      else if(hist == "pfmet") {
        *xtitle = "PF Missing Transverse Energy (GeV)";
        *ytitle = "";
        *title  = Form("Missing Transverse Energy");
      }
      else if(hist == "puppmet") {
        *xtitle = "PUPPI Missing Transverse Energy (GeV)";
        *ytitle = "";
        *title  = Form("Missing Transverse Energy");
      }
      else if(hist == "detectormet") {
        *xtitle = "Detector MET";
        *ytitle = "";
        *title  = Form("Detector MET");
      }
      else if(hist == "nupt") {
        *xtitle = "p_{T}(#nu)";
        *ytitle = "";
        *title  = Form("p_{T}(#nu)");
      }
      else if(hist == "lepptoverm") {
        *xtitle = "p_{T}^{ll} / M_{ll}";
        *ytitle = "";
        *title  = Form("pT Over Mass of the Lepton System");
      }
      else if(hist == "lepm") {
        *xtitle = "M_{ll} (GeV/c^{2})";
        *ytitle = "";
        *title  = Form("Mass of the Lepton System");
      }
      else if(hist == "lepmestimate") {
        *xtitle = "M_{ll}^{Col} (GeV/c^{2})";
        *ytitle = "";
        *title  = Form("Mass Estimate of the Lepton System");
      }
      else if(hist == "lepmestimatetwo") {
        *xtitle = "M_{ll}^{Col} (GeV/c^{2})";
        *ytitle = "";
        *title  = Form("Mass Estimate of the Lepton System");
      }
      else if(hist == "lepe") {
        *xtitle = "E_{ll} (GeV)";
        *ytitle = "";
        *title  = Form("Energy of the Lepton System");
      }
      else if(hist == "lepp") {
        *xtitle = "P_{ll} (GeV)";
        *ytitle = "";
        *title  = Form("Momentum of the Lepton System");
      }
      else if(hist == "leppt" || hist == "leppt1" || hist == "leppt2") {
        *xtitle = "p_{T}^{ll} (GeV/c)";
        *ytitle = "";
        *title  = Form("pT of the Lepton System");
      }
      else if(hist == "lepsvptoverm") {
        *xtitle = "SVfit pT / M_{ll}";
        *ytitle = "";
        *title  = Form("SVfit pT Over Mass of the Lepton System");
      }
      else if(hist == "lepsvm") {
        *xtitle = "SVfit M_{ll} (GeV/c^{2})";
        *ytitle = "";
        *title  = Form("SVfit Mass of the Lepton System");
      }
      else if(hist == "lepsve") {
        *xtitle = "SVfit E_{ll} (GeV)";
        *ytitle = "";
        *title  = Form("SVfit Energy of the Lepton System");
      }
      else if(hist == "lepsvp") {
        *xtitle = "SVfit P_{ll} (GeV)";
        *ytitle = "";
        *title  = Form("SVfit Momentum of the Lepton System");
      }
      else if(hist == "lepsvpt") {
        *xtitle = "SVfit pT (GeV/c)";
        *ytitle = "";
        *title  = Form("SVfit pT of the Lepton System");
      }
      else if(hist == "pt" && setType == "photon") {
        *xtitle = "p_{T}^{#gamma} (GeV/c)";
        *ytitle = "";
        *title  = Form("pT of the Photon");
      }
      else if(hist == "onept") {
        *xtitle = Form("p_{T}^{%s} (GeV/c)", lep1.Data());
        *ytitle = "";
        *title  = Form("pT of the First Lepton");
      }
      else if(hist == "twopt") {
        *xtitle = Form("p_{T}^{%s} (GeV/c)", lep2.Data());
        *ytitle = "";
        *title  = Form("pT of the Second Lepton");
      }
      else if(hist == "ptdiff") {
        *xtitle = Form("p_{T}^{%s} - p_{T}^{%s} (GeV/c)", lep1.Data(), lep2.Data());
        *ytitle = "";
        *title  = Form("Lepton pT difference");
      }
      else if(hist == "ptratio" || hist == "lepptratio") {
        *xtitle = Form("p_{T}^{%s} / p_{T}^{%s}", lep1.Data(), lep2.Data());
        *ytitle = "";
        *title  = Form("Lepton pT ratio");
      }
      else if(hist == "pttrailoverlead") {
        *xtitle = "p_{T}(trail) / p_{T}(lead)";
        *ytitle = "";
        *title  = "Lepton pT ratio";
      }
      else if(hist == "onesvpt") {
        *xtitle = "SVFit p_{T}^{1} (GeV/c)";
        *ytitle = "";
        *title  = Form("SVFit pT of the Leading Lepton");
      }
      else if(hist == "onesvdeltapt") {
        *xtitle = Form("SVFit p_{T}^{%s} - pT_{%s} (GeV/c)", lep1.Data(), lep1.Data());
        *ytitle = "";
        *title  = Form("SVFit pT Change of the Leading Lepton");
      }
      else if(hist == "twosvpt") {
        *xtitle = "SVFit p_{T}^{l2} (GeV/c)";
        *ytitle = "";
        *title  = Form("SVFit pT of the Trailing Lepton");
      }
      else if(hist == "twosvdeltapt") {
        *xtitle = "SVFit p_{T}^{l2} - pT_{l2} (GeV/c)";
        *ytitle = "";
        *title  = Form("SVFit pT Change of the Trailing Lepton");
      }
      else if(hist == "onem") {
        *xtitle = Form("M_{%s} (GeV/c^{2})", lep1.Data());
        *ytitle = "";
        *title  = Form("Mass of the Leading Lepton");
      }
      else if(hist == "twom") {
        *xtitle = Form("M_{%s} (GeV/c^{2})", lep2.Data());
        *ytitle = "";
        *title  = Form("Mass of the Trailing Lepton");
      }
      else if(hist == "onesvm") {
        *xtitle = "SVFit M_{l} (GeV/c^{2})";
        *ytitle = "";
        *title  = Form("SVFit Mass of the Leading Lepton");
      }
      else if(hist == "twosvm") {
        *xtitle = "SVFit M_{#tau} (GeV/c^{2})";
        *ytitle = "";
        *title  = Form("SVFit Mass of the Trailing Lepton");
      }
      else if(hist == "oneiso") {
        *xtitle = Form("%s Isolation", lep1.Data());
        *ytitle = "";
        *title  = Form("Isolation of the Leading Lepton");
      }
      else if(hist == "twoiso") {
        *xtitle = Form("%s Isolation", lep2.Data());
        *ytitle = "";
        *title  = Form("Isolation of the Trailing Lepton");
      }
      else if(hist == "twoid1" && (selection == "mutau" || selection == "etau")) {
        *xtitle = "tau MVA anti-ele ID";
        *ytitle = "";
        *title  = "";
      }
      else if(hist == "twoid2" && (selection == "mutau" || selection == "etau")) {
        *xtitle = "tau MVA anti-mu ID";
        *ytitle = "";
        *title  = "";
      }
      else if(hist == "lepdeltar") {
        *xtitle = "#DeltaR_{ll}";
        *ytitle = "";
        *title  = Form("#Delta R of the Lepton System");
      }
      else if(hist == "lepdeltaeta") {
        *xtitle = "#Delta#eta_{ll}";
        *ytitle = "";
        *title  = Form("#Delta#eta of the Lepton System");
      }
      else if(hist == "lepdeltaphi") {
        *xtitle = "#Delta#phi_{ll}";
        *ytitle = "";
        *title  = Form("#Delta#phi of the Lepton System");
      }
      else if(hist == "lepeta") {
        *xtitle = "#eta_{ll}";
        *ytitle = "";
        *title  = Form("#eta of the Lepton System");
      }
      else if(hist == "lepphi") {
        *xtitle = "#phi_{ll}";
        *ytitle = "";
        *title  = Form("#phi of the Lepton System");
      }
      else if(hist == "lepsveta") {
        *xtitle = "SVfit #eta_{ll}";
        *ytitle = "";
        *title  = Form("SVfit #eta of the Lepton System");
      }
      else if(hist == "lepsvphi") {
        *xtitle = "SVfit #phi_{ll}";
        *ytitle = "";
        *title  = Form("SVfit #phi of the Lepton System");
      }
      else if(hist == "pxivis0") {
        *xtitle = "#vec{P}_{T}^{vis}#bullet#hat{#zeta}";
        *ytitle = "";
        *title  = Form("Visible P_{T} projected onto the lepton bisector");
      }
      else if(hist == "pxiinv0") {
        *xtitle = "#vec{P}_{T}^{inv}#bullet#hat{#zeta}";
        *ytitle = "";
        *title  = Form("Invisible P_{T} projected onto the lepton bisector");
      }
      else if(hist == "pxidiff0") {
        *xtitle = "#vec{P}_{T}^{vis}#bullet#hat{#zeta} - #vec{P}_{T}^{inv}#bullet#hat{#zeta}";
        *ytitle = "";
        *title  = Form("Visible - invisible P_{T} projected onto the lepton bisector");
      }
      else if(hist == "pxidiff20") {
        *xtitle = "#vec{P}_{T}^{inv}#bullet#hat{#zeta} - f(#vec{P}_{T}^{vis}#bullet#hat{#zeta})";
        *ytitle = "";
        *title  = Form("Invisible - f(visible) P_{T} projected onto the lepton bisector");
      }
      else if(hist == "pxivisoverinv0") {
        *xtitle = "#vec{P}_{T}^{vis}#bullet#hat{#zeta} / #vec{P}_{T}^{inv}#bullet#hat{#zeta} ";
        *ytitle = "";
        *title  = Form("Visible P_{T} / invisible P_{T} projected onto the lepton bisector");
      }
      else if(hist == "pxiinvvsvis0") {
        *xtitle = "#vec{P}_{T}^{vis}#bullet#hat{#zeta}";
        *ytitle = Form("#vec{P}_{T}^{inv}#bullet#hat{#zeta} ");
        *title  = Form("Invisible P_{T} vs visible P_{T} projected onto the lepton bisector");
      }
      else if(hist == "pxivis1") {
        *xtitle = "#vec{P}_{T}^{vis}#bullet#hat{#zeta}";
        *ytitle = "";
        *title  = Form("Visible P_{T} projected onto the lepton, lepton+#gamma bisector");
      }
      else if(hist == "pxiinv1") {
        *xtitle = "#vec{P}_{T}^{inv}#bullet#hat{#zeta}";
        *ytitle = "";
        *title  = Form("Invisible P_{T} projected onto the lepton, lepton+#gamma bisector");
      }
      else if(hist == "pxidiff1") {
        *xtitle = "#vec{P}_{T}^{vis}#bullet#hat{#zeta} - #vec{P}_{T}^{inv}#bullet#hat{#zeta}";
        *ytitle = "";
        *title  = Form("Visible - invisible P_{T} projected onto the lepton, lepton+#gamma bisector");
      }
      else if(hist == "pxidiff21") {
        *xtitle = "#vec{P}_{T}^{inv}#bullet#hat{#zeta} - f(#vec{P}_{T}^{vis}#bullet#hat{#zeta})";
        *ytitle = "";
        *title  = Form("Invisible - f(visible) P_{T} projected onto the lepton, lepton+#gamma bisector");
      }
      else if(hist == "pxivisoverinv1") {
        *xtitle = "#vec{P}_{T}^{vis}#bullet#hat{#zeta} / #vec{P}_{T}^{inv}#bullet#hat{#zeta} ";
        *ytitle = "";
        *title  = Form("Visible P_{T} / invisible P_{T} projected onto the lepton, lepton+#gamma bisector");
      }
      else if(hist == "pxiinvvsvis1") {
        *xtitle = "#vec{P}_{T}^{vis}#bullet#hat{#zeta}";
        *ytitle = Form("#vec{P}_{T}^{inv}#bullet#hat{#zeta} ");
        *title  = Form("Invisible P_{T} vs visible P_{T} projected onto the lepton, lepton+#gamma bisector");
      }
      else if(hist == "pxivis2") {
        *xtitle = "#vec{P}_{T}^{vis}#bullet#hat{#zeta}";
        *ytitle = "";
        *title  = Form("Visible P_{T} projected onto the di-lepton, #gamma bisector");
      }
      else if(hist == "pxiinv2") {
        *xtitle = "#vec{P}_{T}^{inv}#bullet#hat{#zeta}";
        *ytitle = "";
        *title  = Form("Invisible P_{T} projected onto the di-lepton, #gamma bisector");
      }
      else if(hist == "pxidiff2") {
        *xtitle = "#vec{P}_{T}^{vis}#bullet#hat{#zeta} - #vec{P}_{T}^{inv}#bullet#hat{#zeta}";
        *ytitle = "";
        *title  = Form("Visible - invisible P_{T} projected onto the di-lepton, #gamma bisector");
      }
      else if(hist == "pxidiff22") {
        *xtitle = "#vec{P}_{T}^{inv}#bullet#hat{#zeta} - f(#vec{P}_{T}^{vis}#bullet#hat{#zeta})";
        *ytitle = "";
        *title  = Form("Invisible - f(visible) P_{T} projected onto the di-lepton, #gamma bisector");
      }
      else if(hist == "pxivisoverinv2") {
        *xtitle = "#vec{P}_{T}^{vis}#bullet#hat{#zeta} / #vec{P}_{T}^{inv}#bullet#hat{#zeta} ";
        *ytitle = "";
        *title  = Form("Visible P_{T} / invisible P_{T} projected onto the di-lepton, #gamma bisector");
      }
      else if(hist == "pxiinvvsvis2") {
        *xtitle = "#vec{P}_{T}^{vis}#bullet#hat{#zeta}";
        *ytitle = Form("#vec{P}_{T}^{inv}#bullet#hat{#zeta} ");
        *title  = Form("Invisible P_{T} vs visible P_{T} projected onto the di-lepton, #gamma bisector");
      }
      else if(hist == "masssvfit") {
        *xtitle = "M_{#tau#tau} (GeV/c^{2})";
        *ytitle = "";
        *title  = Form("SVFit di-tau mass");
      }
      else if(hist == "masserrsvfit") {
        *xtitle = "M_{#tau#tau} Predicted Error (GeV/c^{2})";
        *ytitle = "";
        *title  = Form("SVFit di-tau mass error");
      }
      else if(hist == "njets") {
        *xtitle = "Number of Jets";
        *ytitle = "";
        *title  = Form("Number of Jets");
      }
      else if(hist == "njets20") {
        *xtitle = "Number of Jets";
        *ytitle = "";
        *title  = Form("Number of Jets");
      }
      else if(hist == "nbjets") {
        *xtitle = "Number of tight ID b-Jets";
        *ytitle = "";
        *title  = Form("Number of tight ID b-Jets");
      }
      else if(hist == "nbjetsm") {
        *xtitle = "Number of medium ID b-Jets";
        *ytitle = "";
        *title  = Form("Number of medium ID b-Jets");
      }
      else if(hist == "nbjetsl") {
        *xtitle = "Number of loose ID b-Jets";
        *ytitle = "";
        *title  = Form("Number of loose ID b-Jets");
      }
      else if(hist == "nbjets20") {
        *xtitle = "Number of tight ID b-Jets";
        *ytitle = "";
        *title  = Form("Number of tight ID b-Jets");
      }
      else if(hist == "nbjets20m") {
        *xtitle = "Number of medium ID b-Jets";
        *ytitle = "";
        *title  = Form("Number of medium ID b-Jets");
      }
      else if(hist == "nbjets20l") {
        *xtitle = "Number of loose ID b-Jets";
        *ytitle = "";
        *title  = Form("Number of loose ID b-Jets");
      }
      else if(hist == "nfwdjets") {
        *xtitle = "Number of Forward Jets";
        *ytitle = "";
        *title  = Form("Number of Forward Jets");
      }
      else if(hist == "nphotons") {
        *xtitle = "Number of Photons";
        *ytitle = "";
        *title  = Form("Number of Photons");
      }
      else if(hist == "mtmu") {
        *xtitle = "MT(MET,#mu)";
        *ytitle = "";
        *title  = Form("MT(MET,#mu)");
      }
      else if(hist == "mte") {
        *xtitle = "MT(MET,e)";
        *ytitle = "";
        *title  = Form("MT(MET,e)");
      }
      else if(hist == "mttau") {
        *xtitle = "MT(MET,#tau)";
        *ytitle = "";
        *title  = Form("MT(MET,#tau)");
      }
      else if(hist.BeginsWith("mtone")) {
        *xtitle = Form("M_{T}(%s,MET)", lep1.Data());
        *ytitle = "";
        *title  = "";
      }
      else if(hist.BeginsWith("mttwo")) {
        *xtitle = Form("M_{T}(%s,MET)", lep2.Data());
        *ytitle = "";
        *title  = "";
      }
      else if(hist == "mtlead") {
        *xtitle = "MT(lead,MET)";
        *ytitle = "";
        *title  = "";
      }
      else if(hist == "mttrail") {
        *xtitle = "MT(trail,MET)";
        *ytitle = "";
        *title  = "";
      }
      else if(hist == "mtlep") {
        *xtitle = "M_{T}(ll,MET)";
        *ytitle = "";
        *title  = "";
      }
      else if(hist == "oneeta") {
        *xtitle = Form("#eta^{%s}", lep1.Data());
        *ytitle = "";
        *title  = "";
      }
      else if(hist == "twoeta") {
        *xtitle = Form("#eta^{%s}", lep2.Data());
        *ytitle = "";
        *title  = "";
      }
      else if(hist == "ptsum0") {
        *xtitle = "pT_{l} + pT_{#tau} + MET";
        *ytitle = "";
        *title  = Form("pT_{l} + pT_{#tau} + MET");
      }
      else if(hist == "ptsum1") {
        *xtitle = "pT_{l} + pT_{#tau} + pT_{#gamma} + MET";
        *ytitle = "";
        *title  = Form("pT_{l} + pT_{#tau} + pT_{#gamma} + MET");
      }
      else if(hist == "pt1sum0") {
        *xtitle = "pT_{l} + MET";
        *ytitle = "";
        *title  = Form("pT_{l} + MET");
      }
      else if(hist == "pt1sum1") {
        *xtitle = "pT_{#tau} + MET";
        *ytitle = "";
        *title  = Form("pT_{#tau} + MET");
      }
      else if(hist == "pt1sum2") {
        *xtitle = "pT_{#tau} + pT_{l}";
        *ytitle = "";
        *title  = Form("pT_{#tau} + pT_{l}");
      }
      else if(hist == "pt1sum3") {
        *xtitle = "pT_{#tau} + pT_{l} - MET";
        *ytitle = "";
        *title  = Form("pT_{#tau} + pT_{l} - MET");
      }
      else if(hist == "jetpt") {
        *xtitle = "Leading jet p_{T}";
        *ytitle = "";
        *title  = Form("Leading jet p_{T}");
      }
      else if(hist == "ht") {
        *xtitle = "p_{T} of (#Sigma #vec{P}_{Jet})";
        *ytitle = "";
        *title  = Form("Jet vector pT sum");
      }
      else if(hist == "htphi") {
        *xtitle = "#phi of (#Sigma #vec{P}_{Jet})";
        *ytitle = "";
        *title  = Form("Jet vector sum #phi");
      }
      else if(hist == "htsum") {
        *xtitle = " #Sigma (p_{T} of #vec{P}_{Jet})";
        *ytitle = "";
        *title  = Form("Jet scalar pT sum");
      }
      else if(hist.Contains("mva")) {
        *xtitle = "BDT score";
        *ytitle = "";
        *title  = "BDT Score";
      }
      else if(hist.Contains("prob")) {
        *xtitle = "BDT score probability";
        *ytitle = "";
        *title  = "BDT Score Probability";
      }
      else if(hist.Contains("cdf")) {
        *xtitle = "BDT score CDF";
        *ytitle = "";
        *title  = "BDT Score CDF";
      }
      else if(hist == "metdeltaphi") {
        *xtitle = "#Delta#phi(MET,ll)";
        *ytitle = "";
        *title  = "#Delta#phi between MET and di-lepton";
      }
      else if(hist == "htdeltaphi") {
        *xtitle = "#Delta#phi(#SigmaJets,ll)";
        *ytitle = "";
        *title  = "#Delta#phi between Jet sum and di-lepton";
      }
      else if(hist == "leponedeltaphi") {
        *xtitle = Form("#Delta#phi(%s,ll)", lep1.Data());
        *ytitle = "";
        *title  = "#Delta#phi between lepton 1 and di-lepton";
      }
      else if(hist == "leptwodeltaphi") {
        *xtitle = Form("#Delta#phi(%s,ll)", lep2.Data());
        *ytitle = "";
        *title  = "#Delta#phi between lepton 2 and di-lepton";
      }
      else if(hist == "onemetdeltaphi") {
        *xtitle = Form("#Delta#phi(%s,MET)", lep1.Data());
        *ytitle = "";
        *title  = "#Delta#phi between MET and lepton 1";
      }
      else if(hist == "twometdeltaphi") {
        *xtitle = Form("#Delta#phi(%s,MET)", lep2.Data());
        *ytitle = "";
        *title  = "#Delta#phi between MET and lepton 2";
      }
      else if(hist == "oneiso") {
        *xtitle = Form("%s Iso", lep1.Data());
        *ytitle = "";
        *title  = "l_{1} Iso";
      }
      else if(hist == "onereliso") {
        *xtitle = Form("%s Iso/p_{T}", lep1.Data());
        *ytitle = "";
        *title  = "l_{1} Iso/p_{T}";
      }
      else if(hist == "twoiso") {
        *xtitle = Form("%s Iso", lep2.Data());
        *ytitle = "";
        *title  = "l_{2} Iso";
      }
      else if(hist == "tworeliso") {
        *xtitle = Form("%s Iso/p_{T}", lep2.Data());
        *ytitle = "";
        *title  = "l_{2} Iso/p_{T}";
      }
      else if(hist == "oned0") {
        *xtitle = Form("%s D0", lep1.Data());
        *ytitle = "";
        *title  = "l_{1} D0";
      }
      else if(hist == "onedxy") {
        *xtitle = Form("%s d_{xy}", lep1.Data());
        *ytitle = "";
        *title  = "l_{1} d_{xy}";
      }
      else if(hist == "onedz") {
        *xtitle = Form("%s d_{z}", lep1.Data());
        *ytitle = "";
        *title  = "l_{1} d_{z}";
      }
      else if(hist == "onedxysig") {
        *xtitle = Form("%s d_{xy}/#sigma_{xy}", lep1.Data());
        *ytitle = "";
        *title  = "l_{1} d_{xy}/#sigma_{xy}";
      }
      else if(hist == "onedzsig") {
        *xtitle = Form("%s d_{z}/#sigma_{z}", lep1.Data());
        *ytitle = "";
        *title  = "l_{1} d_{xy}/#sigma_{z}";
      }
      else if(hist == "twod0") {
        *xtitle = Form("%s D0", lep2.Data());
        *ytitle = "";
        *title  = "l_{2} D0";
      }
      else if(hist == "twodxy") {
        *xtitle = Form("%s d_{xy}", lep2.Data());
        *ytitle = "";
        *title  = "l_{2} d_{xy}";
      }
      else if(hist == "twodz") {
        *xtitle = Form("%s d_{z}", lep2.Data());
        *ytitle = "";
        *title  = "l_{2} d_{z}";
      }
      else if(hist == "twodxysig") {
        *xtitle = Form("%s d_{xy}/#sigma_{xy}", lep2.Data());
        *ytitle = "";
        *title  = "l_{2} d_{xy}/#sigma_{xy}";
      }
      else if(hist == "twodzsig") {
        *xtitle = Form("%s d_{z}/#sigma_{z}", lep2.Data());
        *ytitle = "";
        *title  = "l_{2} d_{xy}/#sigma_{z}";
      }
      else if(hist == "metvspt") {
        *xtitle = "p_{T}^{ll}";
        *ytitle = "MET";
        *title  = "MET vs di-lepton pT";
      }
      else if(hist == "twoptvsonept") {
        *xtitle = "p_{T}^{l1}";
        *ytitle = "p_{T}^{l2}";
        *title  = "Lepton 2 pT vs Lepton 1 pT";
      }
      else if(hist.Contains("deltaalpham")) { //Mass estimated using alpha formulas
        *xtitle = "M_{#alpha}";
        *ytitle = "";
        *title  = "M_{#alpha}";
      }
      else if(hist.Contains("deltaalpha")) {
        *xtitle = "#Delta#alpha";
        *ytitle = "";
        *title  = "#Delta#alpha";
      }
      else if(hist == "beta0") {
        *xtitle = Form("#alpha(%s)", lep2.Data()); //indices are inverted
        *ytitle = "";
        *title  = "#alpha";
      }
      else if(hist == "beta1") {
        *xtitle = Form("#alpha(%s)", lep1.Data()); //indices are inverted
        *ytitle = "";
        *title  = "#alpha";
      }
      else if(hist.Contains("taudeepantimu")) {
        *xtitle = "#tau anti-#mu ID";
        *ytitle = "";
        *title  = "#tau anti-#mu ID";
      }
      else if(hist.Contains("taudeepantiele")) {
        *xtitle = "log_{2}(#tau anti-e ID + 1)";
        *ytitle = "";
        *title  = "log_{2}(#tau anti-e ID + 1)";
      }
      else if(hist.Contains("taudeepantijet")) {
        *xtitle = "log_{2}(#tau anti-Jet ID + 1)";
        *ytitle = "";
        *title  = "log_{2}(#tau anti-Jet ID + 1)";
      }

      if(hist.EndsWith("overm")) { *xtitle = *xtitle + " / M_{ll}"; }
    }

    //-----------------------------------------------------------------------------
    static void get_titles(TString hist, TString setType, TString selection, TString& xtitle, TString& ytitle, TString& title) {
      get_titles(hist, setType, selection, &xtitle, &ytitle, &title);
    }

  };
}
#endif
