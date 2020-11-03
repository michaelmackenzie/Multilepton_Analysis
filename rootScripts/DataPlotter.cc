#include "DataPlotter.hh"

void DataPlotter::get_titles(TString hist, TString setType, TString* xtitle, TString* ytitle, TString* title) {
  if(hist == "lepdelrvsphi") {
    *xtitle = Form("#DeltaR / %.1f",0.1*rebinH_);
    *ytitle = Form("#Delta#phi / %.1f",0.1*rebinH_);
    *title  = Form("#DeltaR vs #Delta#phi Between Leptons %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "sysm" && setType == "event") {
    *xtitle = "M_{ll#gamma} (GeV/c^{2})";
    *ytitle = Form("Events / %.0f GeV/c^{2}",1.*rebinH_);
    *title  = Form("Mass of the Photon + Lepton System %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "met") {
    *xtitle = "Missing Transverse Energy (GeV)";
    *ytitle = Form("Events / %.0f GeV",2.*rebinH_);
    *title  = Form("Missing Transverse Energy %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "pfmet") {
    *xtitle = "PF Missing Transverse Energy (GeV)";
    *ytitle = Form("Events / %.0f GeV",2.*rebinH_);
    *title  = Form("Missing Transverse Energy %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "puppmet") {
    *xtitle = "PUPPI Missing Transverse Energy (GeV)";
    *ytitle = Form("Events / %.0f GeV",2.*rebinH_);
    *title  = Form("Missing Transverse Energy %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "lepptoverm") {
    *xtitle = "pT_{ll} / M_{ll}";
    *ytitle = Form("Events / %.1f GeV/c^{2}",0.2*rebinH_);
    *title  = Form("pT Over Mass of the Lepton System %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "lepm") {
    *xtitle = "M_{ll} (GeV/c^{2})";
    *ytitle = Form("Events / %.0f GeV/c^{2}",1.*rebinH_);
    *title  = Form("Mass of the Lepton System %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "lepmestimate") {
    *xtitle = "M_{ll}^{Col} (GeV/c^{2})";
    *ytitle = Form("Events / %.0f GeV/c^{2}",1.*rebinH_);
    *title  = Form("Mass Estimate of the Lepton System %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "lepmestimatetwo") {
    *xtitle = "M_{ll}^{Col} (GeV/c^{2})";
    *ytitle = Form("Events / %.0f GeV/c^{2}",1.*rebinH_);
    *title  = Form("Mass Estimate of the Lepton System %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "lepe") {
    *xtitle = "E_{ll} (GeV)";
    *ytitle = Form("Events / %.0f GeV",1.*rebinH_);
    *title  = Form("Energy of the Lepton System %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "lepp") {
    *xtitle = "P_{ll} (GeV)";
    *ytitle = Form("Events / %.0f GeV",1.*rebinH_);
    *title  = Form("Momentum of the Lepton System %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "leppt") {
    *xtitle = "pT_{ll} (GeV/c)";
    *ytitle = Form("Events / %.0f GeV/c",2.*rebinH_);
    *title  = Form("pT of the Lepton System %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "lepsvptoverm") {
    *xtitle = "SVfit pT / M_{ll}";
    *ytitle = Form("Events / %.1f GeV/c^{2}",0.2*rebinH_);
    *title  = Form("SVfit pT Over Mass of the Lepton System %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "lepsvm") {
    *xtitle = "SVfit M_{ll} (GeV/c^{2})";
    *ytitle = Form("Events / %.0f GeV/c^{2}",1.*rebinH_);
    *title  = Form("SVfit Mass of the Lepton System %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "lepsve") {
    *xtitle = "SVfit E_{ll} (GeV)";
    *ytitle = Form("Events / %.0f GeV",1.*rebinH_);
    *title  = Form("SVfit Energy of the Lepton System %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "lepsvp") {
    *xtitle = "SVfit P_{ll} (GeV)";
    *ytitle = Form("Events / %.0f GeV",1.*rebinH_);
    *title  = Form("SVfit Momentum of the Lepton System %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "lepsvpt") {
    *xtitle = "SVfit pT (GeV/c)";
    *ytitle = Form("Events / %.0f GeV/c",2.*rebinH_);
    *title  = Form("SVfit pT of the Lepton System %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "pt" && setType == "photon") {
    *xtitle = "pT_{#gamma} (GeV/c)";
    *ytitle = Form("Events / %.0f GeV/c",2.*rebinH_);
    *title  = Form("pT of the Photon");
  }
  else if(hist == "onept") {
    *xtitle = "pT_{l} (GeV/c)";
    *ytitle = Form("Events / %.0f GeV/c",1.*rebinH_);
    *title  = Form("pT of the First Lepton %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "twopt") {
    *xtitle = "pT_{2} (GeV/c)";
    *ytitle = Form("Events / %.0f GeV/c",1.*rebinH_);
    *title  = Form("pT of the Second Lepton %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "onesvpt") {
    *xtitle = "SVFit pT_{l} (GeV/c)";
    *ytitle = Form("Events / %.0f GeV/c",1.*rebinH_);
    *title  = Form("SVFit pT of the Leading Lepton %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "onesvdeltapt") {
    *xtitle = "SVFit pT_{l1} - pT_{l1} (GeV/c)";
    *ytitle = Form("Events / %.0f GeV/c",1.*rebinH_);
    *title  = Form("SVFit pT Change of the Leading Lepton %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "twosvpt") {
    *xtitle = "SVFit pT_{l2} (GeV/c)";
    *ytitle = Form("Events / %.0f GeV/c",1.*rebinH_);
    *title  = Form("SVFit pT of the Trailing Lepton %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "twosvdeltapt") {
    *xtitle = "SVFit pT_{l2} - pT_{l2} (GeV/c)";
    *ytitle = Form("Events / %.0f GeV/c",1.*rebinH_);
    *title  = Form("SVFit pT Change of the Trailing Lepton %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "onem") {
    *xtitle = "M_{l} (GeV/c^{2})";
    *ytitle = Form("Events / %.0f GeV/c",1.*rebinH_);
    *title  = Form("Mass of the Leading Lepton %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "twom") {
    *xtitle = "M_{#tau} (GeV/c^{2})";
    *ytitle = Form("Events / %.0f GeV/c",1.*rebinH_);
    *title  = Form("Mass of the Trailing Lepton %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "onesvm") {
    *xtitle = "SVFit M_{l} (GeV/c^{2})";
    *ytitle = Form("Events / %.0f GeV/c",1.*rebinH_);
    *title  = Form("SVFit Mass of the Leading Lepton %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "twosvm") {
    *xtitle = "SVFit M_{#tau} (GeV/c^{2})";
    *ytitle = Form("Events / %.0f GeV/c",1.*rebinH_);
    *title  = Form("SVFit Mass of the Trailing Lepton %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "oneiso") {
    *xtitle = "Lepton Isolation";
    *ytitle = Form("Events / %.2f",.05*rebinH_);
    *title  = Form("Isolation of the Leading Lepton %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "twoiso") {
    *xtitle = "#tau Isolation";
    *ytitle = Form("Events / %.2f",.05*rebinH_);
    *title  = Form("Isolation of the Trailing Lepton %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "lepdeltar") {
    *xtitle = "#DeltaR_{ll}";
    *ytitle = Form("Events / %.1f",0.1*rebinH_);
    *title  = Form("#Delta R of the Lepton System %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "lepdeltaeta") {
    *xtitle = "#Delta#eta_{ll}";
    *ytitle = Form("Events / %.1f",0.1*rebinH_);
    *title  = Form("#Delta#eta of the Lepton System %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "lepdeltaphi") {
    *xtitle = "#Delta#phi_{ll}";
    *ytitle = Form("Events / %.1f",0.1*rebinH_);
    *title  = Form("#Delta#phi of the Lepton System %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "lepeta") {
    *xtitle = "#eta_{ll}";
    *ytitle = Form("Events / %.1f",0.1*rebinH_);
    *title  = Form("#eta of the Lepton System %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "lepphi") {
    *xtitle = "#phi_{ll}";
    *ytitle = Form("Events / %.1f",0.1*rebinH_);
    *title  = Form("#phi of the Lepton System %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "lepsveta") {
    *xtitle = "SVfit #eta_{ll}";
    *ytitle = Form("Events / %.1f",0.1*rebinH_);
    *title  = Form("SVfit #eta of the Lepton System %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
  }
  else if(hist == "lepsvphi") {
    *xtitle = "SVfit #phi_{ll}";
    *ytitle = Form("Events / %.1f",0.1*rebinH_);
    *title  = Form("SVfit #phi of the Lepton System %.1ffb^{-1} (#sqrt{#it{s}} = %.0f TeV)",lum_/1e3,rootS_);
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
    *xtitle = "Number of Jets (pT > 30)";
    *ytitle = "";
    *title  = Form("Number of Jets");
  }
  else if(hist == "njets25tot") {
    *xtitle = "Number of Jets (25 < pT)";
    *ytitle = "";
    *title  = Form("Number of Jets");
  }
  else if(hist == "njets25") {
    *xtitle = "Number of Jets (25 < pT < 30)";
    *ytitle = "";
    *title  = Form("Number of Jets");
  }
  else if(hist == "njets20") {
    *xtitle = "Number of Jets (20 < pT < 25)";
    *ytitle = "";
    *title  = Form("Number of Jets");
  }
  else if(hist == "njetstot") {
    *xtitle = "Number of Jets (20 < pT)";
    *ytitle = "";
    *title  = Form("Number of Jets");
  }
  else if(hist == "nbjets") {
    *xtitle = "Number of tight ID b-Jets (pT > 30)";
    *ytitle = "";
    *title  = Form("Number of tight ID b-Jets");
  }
  else if(hist == "nbjetsm") {
    *xtitle = "Number of medium ID b-Jets (pT > 30)";
    *ytitle = "";
    *title  = Form("Number of medium ID b-Jets");
  }
  else if(hist == "nbjetsl") {
    *xtitle = "Number of loose ID b-Jets (pT > 30)";
    *ytitle = "";
    *title  = Form("Number of loose ID b-Jets");
  }
  else if(hist == "nbjets25") {
    *xtitle = "Number of tight ID b-Jets (25 < pT < 30)";
    *ytitle = "";
    *title  = Form("Number of tight ID b-Jets");
  }
  else if(hist == "nbjets25m") {
    *xtitle = "Number of medium ID b-Jets (25 < pT < 30)";
    *ytitle = "";
    *title  = Form("Number of medium ID b-Jets");
  }
  else if(hist == "nbjets25l") {
    *xtitle = "Number of loose ID b-Jets (25 < pT < 30)";
    *ytitle = "";
    *title  = Form("Number of loose ID b-Jets");
  }
  else if(hist == "nbjetstot25") {
    *xtitle = "Number of tight ID b-Jets (25 < pT)";
    *ytitle = "";
    *title  = Form("Number of tight ID b-Jets");
  }
  else if(hist == "nbjetstot25m") {
    *xtitle = "Number of medium ID b-Jets (25 < pT)";
    *ytitle = "";
    *title  = Form("Number of medium ID b-Jets");
  }
  else if(hist == "nbjetstot25l") {
    *xtitle = "Number of loose ID b-Jets (25 < pT)";
    *ytitle = "";
    *title  = Form("Number of loose ID b-Jets");
  }
  else if(hist == "nbjets20") {
    *xtitle = "Number of tight ID b-Jets (20 < pT < 25)";
    *ytitle = "";
    *title  = Form("Number of tight ID b-Jets");
  }
  else if(hist == "nbjets20m") {
    *xtitle = "Number of medium ID b-Jets (20 < pT < 25)";
    *ytitle = "";
    *title  = Form("Number of medium ID b-Jets");
  }
  else if(hist == "nbjets20l") {
    *xtitle = "Number of loose ID b-Jets (20 < pT < 25)";
    *ytitle = "";
    *title  = Form("Number of loose ID b-Jets");
  }
  else if(hist == "nbjetstot") {
    *xtitle = "Number of tight ID b-Jets (20 < pT)";
    *ytitle = "";
    *title  = Form("Number of tight ID b-Jets");
  }
  else if(hist == "nbjetstotm") {
    *xtitle = "Number of medium ID b-Jets (20 < pT)";
    *ytitle = "";
    *title  = Form("Number of medium ID b-Jets");
  }
  else if(hist == "nbjetstotl") {
    *xtitle = "Number of loose ID b-Jets (20 < pT)";
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
  else if(hist == "ht") {
    *xtitle = "pT of (#Sigma #vec{P}_{Jet})";
    *ytitle = "";
    *title  = Form("Jet vector pT sum");
  }
  else if(hist == "htphi") {
    *xtitle = "#phi of (#Sigma #vec{P}_{Jet})";
    *ytitle = "";
    *title  = Form("Jet vector sum #phi");
  }
  else if(hist == "htsum") {
    *xtitle = " #Sigma (pT of #vec{P}_{Jet})";
    *ytitle = "";
    *title  = Form("Jet scalar pT sum");
  }
  else if(hist.Contains("mva")) {
    *xtitle = "MVA score";
    *ytitle = "";
    *title  = "MVA Score";
  }
  else if(hist.Contains("prob")) {
    *xtitle = "MVA score probability";
    *ytitle = "";
    *title  = "MVA Score Probability";
  }
  else if(hist.Contains("cdf")) {
    *xtitle = "MVA score CDF";
    *ytitle = "";
    *title  = "MVA Score CDF";
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
    *xtitle = "#Delta#phi(l_{1},ll)";
    *ytitle = "";
    *title  = "#Delta#phi between lepton 1 and di-lepton";
  }
  else if(hist == "leptwodeltaphi") {
    *xtitle = "#Delta#phi(l_{2},ll)";
    *ytitle = "";
    *title  = "#Delta#phi between lepton 2 and di-lepton";
  }
  else if(hist == "onemetdeltaphi") {
    *xtitle = "#Delta#phi(l_{1},MET)";
    *ytitle = "";
    *title  = "#Delta#phi between MET and lepton 1";
  }
  else if(hist == "twometdeltaphi") {
    *xtitle = "#Delta#phi(l_{2},MET)";
    *ytitle = "";
    *title  = "#Delta#phi between MET and lepton 2";
  }
  else if(hist == "oneiso") {
    *xtitle = "l_{1} Iso";
    *ytitle = "";
    *title  = "l_{1} Iso";
  }
  else if(hist == "onereliso") {
    *xtitle = "l_{1} Iso/pT";
    *ytitle = "";
    *title  = "l_{1} Iso/pT";
  }
  else if(hist == "twoiso") {
    *xtitle = "l_{2} Iso";
    *ytitle = "";
    *title  = "l_{2} Iso";
  }
  else if(hist == "tworeliso") {
    *xtitle = "l_{2} Iso/pT";
    *ytitle = "";
    *title  = "l_{2} Iso/pT";
  }
  else if(hist == "oned0") {
    *xtitle = "l_{1} D0";
    *ytitle = "";
    *title  = "l_{1} D0";
  }
  else if(hist == "twod0") {
    *xtitle = "l_{2} D0";
    *ytitle = "";
    *title  = "l_{2} D0";
  }
  else if(hist == "metvspt") {
    *xtitle = "pT_{ll}";
    *ytitle = "MET";
    *title  = "MET vs di-lepton pT";
  }
  else if(hist == "twoptvsonept") {
    *xtitle = "pT_{l1}";
    *ytitle = "pT_{l2}";
    *title  = "Lepton 2 pT vs Lepton 1 pT";
  }
  else if(hist.Contains("deltaalpha")) {
    *xtitle = "#Delta#alpha";
    *ytitle = "";
    *title  = "#Delta#alpha";
  }

}

vector<TH1D*> DataPlotter::get_signal(TString hist, TString setType, Int_t set) {
  vector<TH1D*> h;
  
  //for combining histograms of the same process
  map<TString, unsigned int> indexes;
  
  map<TString, int> colors; //index for the color array
  unsigned int n_colors = 0;
  
  for(UInt_t i = 0; i < data_.size(); ++i) {
    h.push_back(NULL);
    if(isData_[i]) continue;
    if(!isSignal_[i]) continue;
    TH1D* tmp = (TH1D*) data_[i]->Get(Form("%s_%i/%s", setType.Data(), set, hist.Data()));
    if(!tmp) continue;
    // tmp->SetBit(kCanDelete);
    tmp->Scale(scale_[i]);

    h[i] = tmp;
    if(rebinH_ > 1) h[i]->Rebin(rebinH_);
    unsigned int index = i;
    unsigned int i_color = n_colors;
    if(indexes.count(labels_[i])) {
      index = indexes[labels_[i]];
      i_color = colors[labels_[i]];
    } else {
      indexes.insert({labels_[i], i});
      colors.insert({labels_[i], n_colors});
      n_colors++;
    }
    TString name = labels_[index];
    if(index !=  i)  {
      h[index]->Add(h[i]);
      delete h[i];
    } else {
      TString hname = name; //Form("%s_%s_%i", name.Data(), hist.Data(), set);
      auto o = gDirectory->Get(hname.Data());
      if(o) delete o;
      h[index]->SetFillStyle(0);
      h[index]->SetFillColor(signal_colors_[i_color]);
      h[index]->SetLineColor(signal_colors_[i_color]);
      h[index]->SetMarkerColor(signal_colors_[i_color]);
      h[index]->SetLineWidth(3);
      h[index]->SetName(Form("%s",hname.Data()));
    }
    
    const char* stats = (doStatsLegend_) ? Form(" #scale[0.8]{(%.2e)}", h[index]->Integral()
						+h[index]->GetBinContent(0)+h[index]->GetBinContent(h[index]->GetNbinsX()+1)) : "";
    h[index]->SetTitle(Form("%s%s%s", name.Data(),
			    (signal_scale_ == 1.) ? "" : Form(" (x%.1f)",signal_scale_), stats));
  }
  //scale return only meaningful histograms
  vector<TH1D*> hsignals;
  for(unsigned int i = 0; i < h.size(); ++i) {
    if(h[i] && indexes[labels_[i]] == i && h[i]->Integral() > 0.) {
      h[i]->Scale(signal_scale_);
      hsignals.push_back(h[i]);
    }
  }
  
  return hsignals;
}

TH2D* DataPlotter::get_signal_2D(TString hist, TString setType, Int_t set) {
  {
    auto o = gDirectory->Get("hSignal");
    if(o) delete o;
  }
  TH2D* h = 0;
  for(UInt_t i = 0; i < data_.size(); ++i) {
    if(isData_[i]) continue;
    if(!isSignal_[i]) continue;
    TH2D* tmp = (TH2D*) data_[i]->Get(Form("%s_%i/%s",setType.Data(), set, hist.Data()));
    if(!tmp) continue;
    if(!h) h = tmp;
    else h->Add(tmp);
    const char* stats = (doStatsLegend_) ? Form(" #scale[0.8]{(%.2e)}", h->Integral()
						+h->GetBinContent(0)+h->GetBinContent(h->GetNbinsX()+1)) : "";

    h->SetTitle(Form("%s%s", labels_[i].Data(), stats));
  }
  if(!h) return NULL;
  h->SetLineWidth(2);
  h->SetLineColor(kAzure-2);
  h->SetFillColor(kAzure-2);
  h->SetFillStyle(3001);

  h->SetName("hSignal");
  if(rebinH_ > 0) {
    h->RebinX(rebinH_);
    h->RebinY(rebinH_);
  }

  return h;
}

TH1D* DataPlotter::get_data(TString hist, TString setType, Int_t set) {
  TString hname = Form("hData_%s_%i", hist.Data(), set);
  {
    auto o = gDirectory->Get(hname.Data());
    if(o) delete o;
  }
  TH1D* d = 0;
  for(UInt_t i = 0; i < data_.size(); ++i) {
    if(!isData_[i]) continue;
    TH1D* tmp = (TH1D*) data_[i]->Get(Form("%s_%i/%s",setType.Data(), set, hist.Data()));
    if(!tmp) continue;
    // tmp->SetBit(kCanDelete);
    if(!d) d = tmp;
    else d->Add(tmp);
  }
  if(!d) return NULL;
  d->SetLineWidth(2);
  d->SetMarkerStyle(20);
  const char* stats = (doStatsLegend_) ? Form(" #scale[0.8]{(%.2e)}", d->Integral()
					      +d->GetBinContent(0)+d->GetBinContent(d->GetNbinsX()+1)) : "";
  d->SetTitle(Form("Data%s",stats));
  d->SetName(hname.Data());
  if(rebinH_ > 0) d->Rebin(rebinH_);

  return d;
}

TH2D* DataPlotter::get_data_2D(TString hist, TString setType, Int_t set) {
  {
    auto o = gDirectory->Get("hData");
    if(o) delete o;
  }
  TH2D* d = 0;
  for(UInt_t i = 0; i < data_.size(); ++i) {
    if(!isData_[i]) continue;
    TH2D* tmp = (TH2D*) data_[i]->Get(Form("%s_%i/%s",setType.Data(), set, hist.Data()));
    if(!tmp) continue;
    if(!d) d = tmp;
    else d->Add(tmp);
  }
  if(!d) return NULL;
  d->SetLineWidth(2);
  d->SetMarkerStyle(20);
  const char* stats = (doStatsLegend_) ? Form(" #scale[0.8]{(%.2e)}", d->Integral()
					      +d->GetBinContent(0)+d->GetBinContent(d->GetNbinsX()+1)) : "";
  d->SetTitle(Form("Data%s",stats)); 
  d->SetName("hData");
  if(rebinH_ > 0) {
    d->RebinX(rebinH_);
    d->RebinY(rebinH_);
  }

  return d;
}

TH1D* DataPlotter::get_qcd(TString hist, TString setType, Int_t set) {
  {
    auto o = gDirectory->Get(Form("qcd_%s_%i", hist.Data(), set));
    if(o) delete o;
  }
  
  Int_t set_qcd = set + qcd_offset_;

  TH1D* hData = get_data(hist, setType, set_qcd);
  if(!hData) return hData;
  hData->SetName(Form("qcd_%s_%i",hist.Data(),set));      
  TH1D* hMC = 0;
  for(UInt_t i = 0; i < data_.size(); ++i) {
    if(isData_[i]) continue;
    TH1D* htmp = (TH1D*) data_[i]->Get(Form("%s_%i/%s",setType.Data(), set_qcd, hist.Data()));
    if(!htmp) continue;
    htmp = (TH1D*) htmp->Clone("tmp");
    // htmp->SetBit(kCanDelete);
    htmp->Scale(scale_[i]);
    if(rebinH_ > 0) htmp->Rebin(rebinH_);
    if(hMC) hMC->Add(htmp);
    else hMC = (TH1D*) htmp->Clone("qcd_tmp");
    delete htmp;
  }
  if(hMC) {hData->Add(hMC, -1.); delete hMC;}
  for(int i = 0; i < hData->GetNbinsX(); ++i) {
    if(hData->GetBinContent(i+1) < 0.)
      hData->SetBinContent(i+1,0.);
  }
  // hData->SetBit(kCanDelete);
  hData->Scale(qcd_scale_);
  
  const char* stats = (doStatsLegend_) ? Form(" #scale[0.8]{(%.2e)}", hData->Integral()
					      +hData->GetBinContent(0)+hData->GetBinContent(hData->GetNbinsX()+1)) : "";
  hData->SetTitle(Form("QCD%s",stats));
  if(fill_alpha_ < 1.) {
    hData->SetLineColorAlpha(kOrange+6,fill_alpha_);
    hData->SetFillColorAlpha(kOrange+6,fill_alpha_);
  } else {
    hData->SetLineColor(kOrange+6);
    hData->SetFillColor(kOrange+6);
  }
  return hData;
}

TH1D* DataPlotter::get_stack_uncertainty(THStack* hstack, TString hname) {
  if(!hstack || hstack->GetNhists() == 0)
    return NULL;
  TList* hlist = hstack->GetHists();
  TH1D* hlast = (TH1D*) hlist->Last();
  //clone last histogram to match the setup
  TH1D* huncertainty = (TH1D*) hlast->Clone(hname.Data());
  huncertainty->Clear(); huncertainty->Reset();
  huncertainty->SetTitle("Bkg. #pm#sigma(Stat.)");
  huncertainty->SetFillColor(kGray+1);
  huncertainty->SetLineColor(kGray+1);
  huncertainty->SetFillStyle(3001);
  huncertainty->SetMarkerSize(0.); //so no marker
  Int_t nbins = hlast->GetNbinsX();

  for(TObject* o : *hlist) {
    if(o->InheritsFrom(TH1D::Class())) {
      huncertainty->Add((TH1D*) o);
    }
  }
  return huncertainty;
}

THStack* DataPlotter::get_stack(TString hist, TString setType, Int_t set) {
  vector<TH1D*> h;
  TH1D* hQCD = (include_qcd_) ? get_qcd(hist,setType,set) : NULL;

  {
    auto o = gDirectory->Get(Form("%s",hist.Data()));
    if(o) delete o;
  }
  
  THStack* hstack = new THStack(Form("%s",hist.Data()),Form("%s",hist.Data()));
  //for combining histograms of the same process
  map<TString, int> indexes;
  vector<TString> labels; //use to preserve order of entries
  map<TString, int> colors; //index for the color array
  int n_colors = 0;
  if(debug_) 
      printf("get_stack: entry name label scale\n");
  for(UInt_t i = 0; i < data_.size(); ++i) {
    if(debug_) 
      printf("get_stack: %i %s %s %.4f\n",
	     i, names_[i].Data(), labels_[i].Data(), scale_[i]);

    if(isData_[i]) {h.push_back(NULL);continue;}
    if(isSignal_[i]) {h.push_back(NULL);continue;}
    
    h.push_back((TH1D*) data_[i]->Get(Form("%s_%i/%s",setType.Data(), set, hist.Data())));
    if(!h[i]) continue;
    auto o = gDirectory->Get("tmp");
    if(o) delete o;
    h[i] = (TH1D*) h[i]->Clone("tmp");
    h[i]->Scale(scale_[i]);
    if(rebinH_ > 0) h[i]->Rebin(rebinH_);
    int index = i;
    int i_color = n_colors;
    if(indexes.count(labels_[i])) {
      index = indexes[labels_[i]];
      i_color = colors[labels_[i]];
    } else {
      indexes.insert({labels_[i], i});
      colors.insert({labels_[i], n_colors});
      n_colors++;
      labels.push_back(labels_[i]);
    }
    TString name = labels_[index];
    if(index != (int) i)  {
      h[index]->Add(h[i]);
      delete h[i];
    } else {
      while(auto o = gDirectory->Get(Form("s_%s_%s_%i",name.Data(),hist.Data(),set))) {
	if(o) delete o;
      }
      if(fill_alpha_ < 1.) {
	h[index]->SetFillColorAlpha(background_colors_[i_color],fill_alpha_);
	h[index]->SetLineColorAlpha(background_colors_[i_color],fill_alpha_);
      } else {
	h[index]->SetFillColor(background_colors_[i_color]);
	h[index]->SetLineColor(background_colors_[i_color]);
      }
      h[index]->SetLineWidth(2);
      h[index]->SetMarkerStyle(20);
      h[index]->SetName(Form("s_%s_%s_%i",name.Data(),hist.Data(),set));    
    }
    const char* stats = (doStatsLegend_) ? Form(" #scale[0.8]{(%.2e)}", h[index]->Integral()
						+h[index]->GetBinContent(0)+h[index]->GetBinContent(h[index]->GetNbinsX()+1)) : "";
    h[index]->SetTitle(Form("%s%s", name.Data(), stats));
  }
  for(unsigned index = 0; index < labels.size(); ++index) {
    hstack->Add(h[indexes[labels[index]]]);
  }
  if(hQCD) hstack->Add(hQCD);
  // hstack->SetBit(kCanDelete);
  return hstack;
}

TCanvas* DataPlotter::plot_single_2Dhist(TString hist, TString setType, Int_t set, TString label) {

  TH2D* h = 0; //histogram

  Int_t color = kBlack;

  {
    auto o = gDirectory->Get(Form("h2d_%s_%i",hist.Data(),set));
    if(o) delete o;
  }
  
  TCanvas* c = new TCanvas(Form("h2d_%s_%i",hist.Data(),set),Form("h2D_%s_%i",hist.Data(),set), canvas_x_, canvas_y_);

  for(UInt_t i = 0; i < data_.size(); ++i) {
    if(label != labels_[i]) continue;
    
    //get histogram
    TH2D* htmp = (TH2D*) data_[i]->Get(Form("%s_%i/%s",setType.Data(), set, hist.Data()));
    if(!htmp) {printf("No hist %s in %s, continuing\n",hist.Data(), fileNames_[i].Data());continue;}
    htmp = (TH2D*) htmp->Clone("tmp");
    //scale to cross section and luminosity
    htmp->Scale(scale_[i]);
    if(rebinH_ > 1) {
      htmp->RebinX(rebinH_);
      htmp->RebinY(rebinH_);
    }
    if(h) {
      h->Add(htmp);
      delete htmp;
    }
    else h = htmp;
    
  }
  // h->SetBit(kCanDelete);
  const char* stats = (doStatsLegend_) ? Form(" #scale[0.8]{(%.2e)}", h->Integral()
					      +h->GetBinContent(0)+h->GetBinContent(h->GetNbinsX()+1)) : "";
  TH2D* data = 0;
  if(plot_data_) {
    data = get_data_2D(hist, setType, set);
    if(!data) {
      printf("Warning! Data not found!\n");
    } else {
      gStyle->SetPalette(kRainBow);
      // if(normalize_2ds_ && data->Integral() > 0. && logZ_) {
      // 	data->Scale(1./data->Integral());
      // }
      data->Scale(1./data->GetMaximum());
      data->Draw("col");
    }
    if(normalize_2ds_&&h->Integral() > 0.) {
      if(logZ_) {
	// h->Scale(2.*data->GetMaximum()/h->GetMaximum());
	data->GetZaxis()->SetRangeUser(1.e-5, 1.5);
	// h->GetZaxis()->SetRangeUser(1.e-6, 0.1);
      } else {
	Double_t mx      = h->GetMaximum();
	Double_t data_mx = data->GetMaximum();
	h->GetZaxis()->SetRangeUser(1.e-3*data_mx,data_mx*1.e2);
	data->GetZaxis()->SetRangeUser(1.e-3*data_mx,1.1*data_mx);
	// h->Scale(20.*data_mx/mx); // set to have the same maximum
      }
    }
  } else {
    gStyle->SetPalette(kRainBow);
    Double_t mx      = h->GetMaximum();
    if(logZ_)
      h->GetZaxis()->SetRangeUser(1.e-4*mx, 2.*mx);
    else 
      h->GetZaxis()->SetRangeUser(1.e-2,1.2*mx);
  }

  // if(data) data->SetBit(kCanDelete);
  TH2D* hAxis = (plot_data_ && data) ? data : h;
  h->SetTitle(Form("%s%s", label.Data(),stats));
  if(plot_data_) {
    h->SetLineColor(color);
    if(fill_alpha_ < 1.)
      h->SetMarkerColorAlpha(color,0.5);
    else 
      h->SetMarkerColor(color);
    h->SetMarkerStyle(6);
    h->SetContour(10);
    // h->RebinX(2); h->RebinY(2);
  }
  h->SetName(Form("h2D_%s_%s_%i",label.Data(),hist.Data(),set));    
  if(plot_data_ && data) h->Draw("same cont3");
  else                   h->Draw("colz");

  //get axis titles
  TString xtitle;
  TString ytitle;
  TString title;
  get_titles(hist,setType,&xtitle,&ytitle,&title);

  c->SetGrid();
  c->SetTopMargin(0.06);
  c->SetRightMargin(0.05 - (plot_data_-1)*0.08);
  c->SetLeftMargin(0.09);
  if(normalize_2ds_ == 0) c->BuildLegend(); //0.6, 0.9, 0.9, 0.45, "", "L");
  if(yMin_ <= yMax_)hAxis->GetYaxis()->SetRangeUser(yMin_,yMax_);
  if(xMin_ <= xMax_)hAxis->GetXaxis()->SetRangeUser(xMin_,xMax_);
  //draw text on plots
  draw_luminosity(true);
  draw_cms_label(true);
  hAxis->SetXTitle(xtitle.Data());
  hAxis->SetYTitle(ytitle.Data());
  hAxis->GetXaxis()->SetTitleSize(0.05);
  hAxis->GetYaxis()->SetTitleSize(0.05);
  hAxis->GetXaxis()->SetTitleOffset(0.8);
  hAxis->GetYaxis()->SetTitleOffset(0.8);
  if(plot_title_) hAxis->SetTitle (title.Data());
  else            hAxis->SetTitle (""); //no title, overwrite current with empty string
  if(logY_) c->SetLogy();
  if(logZ_) c->SetLogz();
  return c;

}

TCanvas* DataPlotter::plot_2Dhist(TString hist, TString setType, Int_t set) {

  vector<TH2D*> h; //list of histograms
  //check if QCD is defined for this set
  //  TH2D* hQCD = (include_qcd_) ? get_qcd(hist,setType,set) : NULL;

  //array of colors and fills for each label
  Int_t color[] = {kRed+1, kYellow+1,kYellow+3,kBlue+1, kRed+3, kViolet-2, kGreen-2, kOrange-9};
  //  Int_t fill[]  = {1001,3005,1001,1001,3005,1001,1001,1001};

  {
    auto o = gDirectory->Get(Form("h2d_%s_%i",hist.Data(),set));
    if(o) delete o;
  }
  
  TCanvas* c = new TCanvas(Form("h2d_%s_%i",hist.Data(),set),Form("h2D_%s_%i",hist.Data(),set), canvas_x_, canvas_y_);

  //maximum z value of histograms, for plotting
  //  double m = 0.;

  //for combining histograms of the same process
  map<TString, int> indexes;
  
  map<TString, int> colors; //map label to index for the color array
  int n_colors = 0; //number of colors used so far

  for(UInt_t i = 0; i < data_.size(); ++i) {
    //push null to not mess up indexing
    if(isData_[i]) {h.push_back(NULL);continue;}

    //get histogram
    TH2D* htmp = (TH2D*) data_[i]->Get(Form("%s_%i/%s",setType.Data(), set, hist.Data()));
    if(!htmp) {printf("No hist %s in %s, continuing\n",hist.Data(), fileNames_[i].Data());continue;}
    h.push_back((TH2D*) htmp->Clone("tmp"));
    //scale to cross section and luminosity
    h[i]->Scale(scale_[i]);

    if(rebinH_ > 1) {
      h[i]->RebinX(rebinH_);
      h[i]->RebinY(rebinH_);
    }
    //if the first, add to map, else get first of this label
    int index = i;
    int i_color = n_colors;
    if(indexes.count(labels_[i])) {
      index = indexes[labels_[i]];
      i_color = colors[labels_[i]];
    } else {
      indexes.insert({labels_[i], i});
      colors.insert({labels_[i], n_colors});
      n_colors++;
    }
    //if not first, add this to first histogram of this label
    TString name = labels_[index];
    if(index != (int)i) {
      h[index]->Add(h[i]);
      delete h[i];
    } else {
      //set plotting info
      h[index]->SetLineColor(color[i_color]);
      h[index]->SetMarkerColor(color[i_color]);
      h[index]->SetName(Form("h2D_%s_%s_%i",name.Data(),hist.Data(),set));    
    }
    const char* stats = (doStatsLegend_) ? Form(" #scale[0.8]{(%.2e)}", h[index]->Integral()
						+h[index]->GetBinContent(0)+h[index]->GetBinContent(h[index]->GetNbinsX()+1)) : "";
    h[index]->SetTitle(Form("%s%s", name.Data(), stats));
  }
  //plot each histogram, remember which is first for axis setting
  bool first = true;
  int ind = 0;
  auto it = indexes.begin();
  while(it != indexes.end()) {
    if(normalize_2ds_ && h[it->second]->Integral() > 0.) h[it->second]->Scale(1./h[it->second]->Integral());
    if(first) {
      h[it->second]->Draw("");
      ind = it->second;
    }
    else
      h[it->second]->Draw("same");
    first = false;
    it++;
  }
  //plot QCD
  //  if(h.size() == 0 && hQCD) hQCD->Draw("hist");
  //  else if(hQCD) hQCD->Draw("hist same");

  //get axis titles
  TString xtitle;
  TString ytitle;
  TString title;
  get_titles(hist,setType,&xtitle,&ytitle,&title);

  c->SetGrid();
  c->SetTopMargin(0.06);
  c->SetRightMargin(0.05);
  c->SetLeftMargin(0.087);
  c->BuildLegend();//0.6, 0.9, 0.9, 0.45, "", "L");
  auto o = c->GetPrimitive("TPave");
  if(o) {
    auto tl = (TLegend*) o;
    tl->SetDrawOption("L");
    tl->SetTextSize(legend_txt_);
    tl->SetY2NDC(legend_y2_);
    tl->SetY1NDC(legend_y1_);
    tl->SetX1NDC(((doStatsLegend_) ? legend_x1_stats_ : legend_x1_));
    tl->SetX2NDC(legend_x2_);
    tl->SetEntrySeparation(legend_sep_);
    c->Update();
  }

  TH2D* hAxis = (h.size() > 0) ? h[ind] : 0;
  if(!hAxis) return NULL;
  
  if(yMin_ <= yMax_)hAxis->GetYaxis()->SetRangeUser(yMin_,yMax_);
  if(xMin_ <= xMax_)hAxis->GetXaxis()->SetRangeUser(xMin_,xMax_);
  //draw text on plots
  draw_luminosity();
  draw_cms_label();
  hAxis->SetXTitle(xtitle.Data());
  hAxis->SetYTitle(ytitle.Data());
  hAxis->GetXaxis()->SetTitleSize(axis_font_size_);
  hAxis->GetYaxis()->SetTitleSize(axis_font_size_);
  if(plot_title_) hAxis->SetTitle (title.Data());
  else hAxis->SetTitle (""); //no title, overwrite current with empty string
  if(logY_) c->SetLogy();
  if(logZ_) c->SetLogz();
  return c;

}

TCanvas* DataPlotter::plot_hist(TString hist, TString setType, Int_t set) {

  vector<TH1D*> h; //list of histograms
  //check if QCD is defined for this set
  TH1D* hQCD = (include_qcd_) ? get_qcd(hist,setType,set) : NULL;
  if(hQCD) hQCD->SetFillStyle(0);
  // if(hQCD) hQCD->SetBit(kCanDelete);
  //array of colors and fills for each label
  Int_t bkg_color[] = {kRed+1, kRed-2, kYellow+1,kSpring-1 , kViolet-2, kGreen-2, kRed+3,kOrange-9,kBlue+1};
  Int_t bkg_fill[]  = {     0,      0,         0,         0,         0,        0,      0,        0,      0};//1001,3005,1001,1001,3005,1001,1001,1001};
  Int_t sig_color[] = {kBlue, kOrange+10, kGreen+4, kViolet-2, kYellow+3,kOrange-9,kBlue+1};
  Int_t sig_fill[]  = {0,          0,          0,         0,        0,         0,       0};//3002,3001,3003,3003,3005,3006,3003,3003};
  TLegend* leg = new TLegend(legend_x1_, legend_y1_, legend_x2_, legend_y2_);
  if(hQCD) leg->AddEntry(hQCD, hQCD->GetTitle(), "L");
  // leg->SetDrawOption("L");
  leg->SetBorderSize(0);
  leg->SetTextSize(legend_txt_);
  leg->SetX1NDC(((doStatsLegend_) ? legend_x1_stats_ : legend_x1_));
  // leg->SetEntrySeparation(legend_sep_);
  {
    auto o = gDirectory->Get(Form("h_%s_%i",hist.Data(),set));
    if(o) delete o;
  }
  TCanvas* c = new TCanvas(Form("h_%s_%i",hist.Data(),set),Form("h_%s_%i",hist.Data(),set), canvas_x_, canvas_y_);

  //maximum y value of histograms, for plotting
  double m = 0.;

  //for combining histograms of the same process
  map<TString, int> indexes;
  
  map<TString, int> colors; //map label to index for the color array
  int n_bkg_colors = 0; //number of bkg colors used so far
  int n_sig_colors = 0; //number of sig colors used so far

  for(UInt_t i = 0; i < data_.size(); ++i) {
    //push null to not mess up indexing
    if(isData_[i]) {h.push_back(NULL);continue;}

    //get histogram
    h.push_back((TH1D*) data_[i]->Get(Form("%s_%i/%s",setType.Data(), set, hist.Data())));
    if(!h[i]) {printf("No hist %s in %s, continuing\n",hist.Data(), fileNames_[i].Data());continue;}
    h[i] = (TH1D*) h[i]->Clone("htmp");
    //scale to cross section and luminosity
    h[i]->Scale(scale_[i]);
    if(rebinH_ > 0) h[i]->Rebin(rebinH_);

    //if the first, add to map, else get first of this label
    int index = i;
    int i_color = (isSignal_[i]) ? n_sig_colors : n_bkg_colors;
    if(indexes.count(labels_[i])) {
      index = indexes[labels_[i]];
      i_color = colors[labels_[i]];
    } else {
      indexes.insert({labels_[i], i});
      colors.insert({labels_[i], (isSignal_[i] ? n_sig_colors : n_bkg_colors)});
      if(isSignal_[i])
	n_sig_colors++;
      else
	n_bkg_colors++;
    }
    //if not first, add this to first histogram of this label
    TString name = labels_[index];
    if(index != (int)i) {
      h[index]->Add(h[i]);
      delete h[i];
    } else {
      //set plotting info
      {
        auto o = gDirectory->Get(Form("h_%s_%s_%i",name.Data(),hist.Data(),set));
	if(o) delete o;
      }
      if(isSignal_[i]) {
	h[index]->SetFillStyle(sig_fill[i_color]); 
	h[index]->SetFillColor(sig_color[i_color]);
	if(fill_alpha_ < 1.)
	  h[index]->SetFillColorAlpha(sig_color[i_color],fill_alpha_);
	else
	  h[index]->SetFillColor(sig_color[i_color]);
	h[index]->SetLineColor(sig_color[i_color]);
	h[index]->SetLineWidth(4);
      } else {
	h[index]->SetFillStyle(bkg_fill[i_color]); 
	if(fill_alpha_ < 1.)
	  h[index]->SetFillColorAlpha(bkg_color[i_color],fill_alpha_);
	else
	  h[index]->SetFillColor(bkg_color[i_color]);
	h[index]->SetLineColor(bkg_color[i_color]);
	h[index]->SetLineWidth(3);
      }
      h[index]->SetMarkerStyle(20);
      h[index]->SetName(Form("h_%s_%s_%i",name.Data(),hist.Data(),set));    
    }
    const char* stats = (doStatsLegend_) ? Form(" #scale[0.8]{(%.2e)}", h[index]->Integral()
						+h[index]->GetBinContent(0)+h[index]->GetBinContent(h[index]->GetNbinsX()+1)) : "";
    if(isSignal_[index])
      h[index]->SetTitle(Form("%s%s%s", name.Data(),
			      (signal_scale_ == 1. || normalize_1ds_) ? "" : Form(" (x%.1f)",signal_scale_),
			      stats));
    else
      h[index]->SetTitle(Form("%s%s", name.Data(),stats));
    if(isSignal_[i] && signal_scale_ > 1.) h[i]->Scale(signal_scale_);
    if((int) i == index) leg->AddEntry(h[index], h[index]->GetTitle(), "L");
  }
  //plot each histogram, remember which is first for axis setting
  bool first = true;
  int ind = 0;
  auto it = indexes.begin();
  while(it != indexes.end()) {
    if(h[it->second]->GetEntries() == 0) continue;
    if(first) {
      if(normalize_1ds_) h[it->second]->Scale(1./h[it->second]->Integral());
      m = max(m,h[it->second]->GetMaximum());
      h[it->second]->Draw("hist");
      ind = it->second;
      first = false;
    }
    else {
      if(normalize_1ds_) h[it->second]->Scale(1./h[it->second]->Integral());
      m = max(m,h[it->second]->GetMaximum());
      h[it->second]->Draw("same hist");
    }
    it++;
  }
  //plot QCD
  if(hQCD && normalize_1ds_) hQCD->Scale(1./hQCD->Integral());
  if(h.size() == 0 && hQCD) hQCD->Draw("hist");
  else if(hQCD) hQCD->Draw("hist same");
  if(hQCD) m = max(m,hQCD->GetMaximum());
  
  //get axis titles
  TString xtitle;
  TString ytitle;
  TString title;
  get_titles(hist,setType,&xtitle,&ytitle,&title);

  c->SetGrid();
  c->SetTopMargin(0.06);
  c->SetRightMargin(0.05);
  c->SetLeftMargin(0.087);
  // c->BuildLegend();//0.6, 0.9, 0.9, 0.45, "", "L");

  TH1D* hAxis = (h.size() > 0) ? h[ind] : hQCD;
  if(!hAxis) return NULL;
  
  if(yMin_ <= yMax_)hAxis->SetAxisRange(yMin_,yMax_,"Y");
  else              hAxis->SetAxisRange((normalize_1ds_) ? m*1.e-4 : 1.e-1, m*1.2,"Y");    
  if(xMin_ < xMax_) hAxis->SetAxisRange(xMin_,xMax_,"X");    
  //draw text on plots
  draw_luminosity();
  draw_cms_label();
  hAxis->SetXTitle(xtitle.Data());
  hAxis->GetXaxis()->SetTitleSize(axis_font_size_);
  hAxis->GetYaxis()->SetTitleSize(axis_font_size_);
  if(plot_y_title_) hAxis->SetYTitle(ytitle.Data());
  if(plot_title_) hAxis->SetTitle (title.Data());
  else hAxis->SetTitle (""); //no title, overwrite current with empty string
  if(logY_) c->SetLogy();
  // auto o = c->GetPrimitive("TPave");
  // if(o) {
  //   c->Modified();
  //   c->Update();
  //   auto tl = (TLegend*) o;
  //   tl->SetDrawOption("L");
  //   tl->SetTextSize(legend_txt_);
  //   tl->SetY2NDC(legend_y2_);
  //   tl->SetY1NDC(legend_y1_);
  //   tl->SetX1NDC(((doStatsLegend_) ? legend_x1_stats_ : legend_x1_));
  //   tl->SetX2NDC(legend_x2_);
  //   tl->SetEntrySeparation(legend_sep_);
  //   c->Modified();
  //   c->Update();
  // } else {
  //   printf("Warning! Legend not found\n");
  // }
  leg->Draw();
  return c;

}

TCanvas* DataPlotter::plot_stack(TString hist, TString setType, Int_t set) {
  //get stack and data histogram
  THStack* hstack = get_stack(hist,setType,set);
  if(!hstack || hstack->GetNhists() <= 0) {
    printf("Null or empty stack!\n");
    return NULL;
  }
  // hstack->SetBit(kCanDelete);
  TH1D* d = get_data(hist, setType, set);
  vector<TH1D*> hsignal = get_signal(hist,setType,set);
  {
    auto o = gDirectory->Get(Form("s_%s_%i",hist.Data(),set));
    if(o) delete o;
  }
  TCanvas* c = new TCanvas(Form("s_%s_%i",hist.Data(),set),Form("s_%s_%i",hist.Data(),set), canvas_x_, canvas_y_);
  //split the top into main stack and bottom into Data/MC if plotting data
  TPad *pad1, *pad2;


  if((plot_data_ && d) || (plot_data_ == 0 && data_over_mc_ < 0)) {
    pad1 = new TPad("pad1","pad1",upper_pad_x1_, upper_pad_y1_, upper_pad_x2_, upper_pad_y2_); //xL yL xH xH, (0,0) = bottom left
    pad2 = new TPad("pad2","pad2",lower_pad_x1_, lower_pad_y1_, lower_pad_x2_, lower_pad_y2_);
    pad1->SetTopMargin(upper_pad_topmargin_);
    if(data_over_mc_ < 0)
      pad2->SetTopMargin(lower_pad_sigbkg_topmargin_);
    else
      pad2->SetTopMargin(lower_pad_topmargin_);
    pad1->SetBottomMargin(upper_pad_botmargin_);
    pad2->SetBottomMargin(lower_pad_botmargin_);
    pad1->Draw();
    pad2->Draw();
  } else {
    pad1 = new TPad("pad1","pad1",0.0,0.0,1,1); //xL yL xH xH, (0,0) = bottom left
    pad1->Draw();
  }
  pad1->cd();
  //store maximum of stack/Data
  double m = 0.;

  
  //get axis titles
  TString xtitle;
  TString ytitle;
  TString title;
  get_titles(hist,setType,&xtitle,&ytitle,&title);
  if(stack_signal_) {
    for(unsigned int i = 0; i < hsignal.size(); ++i) {
      if(hsignal[i]) hstack->Add(hsignal[i]);
    }
  }
  TH1D* hstack_hist = 0;
  if(stack_as_hist_) {
    hstack_hist = (TH1D*) hstack->GetStack()->Last()->Clone("hBackground");
    hstack_hist->SetFillStyle(3002);
    hstack_hist->SetFillColor(total_background_color_);
    hstack_hist->SetLineColor(total_background_color_);
    hstack_hist->SetTitle("Background");
    hstack_hist->Draw("hist");
  } else
    //draw stack, preserving style set for each histogram
    hstack->Draw("hist noclear");

  if(!stack_signal_) {
    for(unsigned int i = 0; i < hsignal.size(); ++i) {
      if(hsignal[i] && hsignal[i]->GetEntries() > 0) {
	hsignal[i]->Draw("hist same");
	m = max(m, hsignal[i]->GetMaximum());
      }
    }
  }

  TH1D* huncertainty = get_stack_uncertainty(hstack,Form("uncertainty_%s_%i", hist.Data(), set));
  if(stack_uncertainty_) 
    huncertainty->Draw("E2 SAME");

  int ndata = 0.;
  double nmc = 0.;
  map<TString, double> nsig;
  if(d && plot_data_) {
    ndata = d->Integral() + d->GetBinContent(0)+d->GetBinContent(d->GetNbinsX()+1);
  }
  for(unsigned i = 0; i < hsignal.size(); ++i) {
    if(!doStatsLegend_ && hsignal[i]->GetEntries() > 0)
      nsig[hsignal[i]->GetName()] = (hsignal[i]->Integral() + hsignal[i]->GetBinContent(0)+hsignal[i]->GetBinContent(hsignal[i]->GetNbinsX()+1))/signal_scale_;
  }
  nmc = huncertainty->Integral() + huncertainty->GetBinContent(0)+huncertainty->GetBinContent(huncertainty->GetNbinsX()+1);

  //blind if needed
  if(blindxmin_.size() > 0 && d && plot_data_) {
    unsigned nbins = d->GetNbinsX();
    for(unsigned bin = 1; bin <= nbins; ++bin) {
      double binlow = d->GetBinLowEdge(bin);
      double binhigh = binlow + d->GetBinWidth(bin);
      if(isBlind(binlow, binhigh))
	d->SetBinContent(bin, 0.);
    }
  }
  
  //draw the data with error bars
  if(plot_data_ && d) d->Draw("E same");

  m = max(max(m,hstack->GetMaximum()), (d) ? d->GetMaximum() : 0.);

  pad1->BuildLegend();//0.6, 0.9, 0.9, 0.45, "", "L");
  pad1->SetGrid();
  pad1->Update();
  //draw text
  if(draw_statistics_) draw_data(ndata,nmc,nsig);
  draw_luminosity(plot_data_ == 0 && data_over_mc_ >= 0);
  draw_cms_label(plot_data_ == 0 && data_over_mc_ >= 0);
  
  auto o = pad1->GetPrimitive("TPave");
  if(o) {
    auto tl = (TLegend*) o;
    tl->SetDrawOption("L");
    tl->SetTextSize(legend_txt_);
    tl->SetY2NDC(legend_y2_);
    tl->SetY1NDC(legend_y1_);
    tl->SetX1NDC(((doStatsLegend_) ? legend_x1_stats_ : legend_x1_));
    tl->SetX2NDC(legend_x2_);
    tl->SetEntrySeparation(legend_sep_);
    pad1->Update();
  }

  //Make a Data/MC histogram
  if(plot_data_ && !d) {
    printf("Warning! Data histogram is Null! Skipping Data/MC plot\n");
  }
  TH1D* hDataMC = 0;
  if(plot_data_ && d && data_over_mc_ > 0)
    hDataMC = (TH1D*) d->Clone("hDataMC");
  else if(data_over_mc_ < 0) //make signal/background histograms instead
    hDataMC = (TH1D*) hstack->GetStack()->Last()->Clone("hRatio");
  TGraphErrors* hDataMCErr = 0;
  int nb = (hDataMC) ? hDataMC->GetNbinsX() : -1;
  std::vector<TH1D*> hSignalsOverMC;
  if(hDataMC && data_over_mc_ > 0) {
    hDataMC->Clear();
    hDataMC->SetName("hDataMC");
    hDataMC->SetTitle("");
    double x[nb];
    double y[nb];
    double xerr[nb];
    double yerr[nb];
    for(int i = 1; i <= nb; ++i) {
      double dataVal = 0;
      double dataErr = 0;
      dataVal = d->GetBinContent(i);
      dataErr= d->GetBinError(i);
      dataErr = sqrt(dataVal);
      double mcVal = huncertainty->GetBinContent(i);
      double mcErr = huncertainty->GetBinError(i);
      x[i-1] = hDataMC->GetBinCenter(i);
      y[i-1] = 1.;
      xerr[i-1] = huncertainty->GetBinWidth(i)/2.;
      yerr[i-1] = (mcVal > 0.) ? mcErr/mcVal : 0.;
      if(dataVal == 0 || mcVal == 0) {hDataMC->SetBinContent(i,-1); continue;}
      double err = sqrt(mcErr*mcErr + dataErr*dataErr);
      double errRatio = (dataVal/mcVal)*(dataVal/mcVal)*(dataErr*dataErr/(dataVal*dataVal));
      errRatio = sqrt(errRatio);
      double ratio = dataVal/mcVal;
      hDataMC->SetBinContent(i,ratio);
      hDataMC->SetBinError(i,errRatio); 
    }
    hDataMCErr = new TGraphErrors(nb, x, y, xerr, yerr);
    hDataMCErr->SetFillStyle(3001);
    hDataMCErr->SetFillColor(kGray+1);
  } else if(hDataMC && data_over_mc_ < 0) {
    hDataMC->SetName("hRatio");
    for(TH1D* h : hsignal)
      hSignalsOverMC.push_back((TH1D*) h->Clone(Form("%s_over_mc", h->GetName())));
    for(TH1D* h : hSignalsOverMC) {
      if(data_over_mc_ == -2) //~significance squared = sig*sig / data
	h->Multiply(h);
      h->Divide(hDataMC);
    }
  }

  
  if(plot_data_ && hDataMC) 
    hDataMC->GetXaxis()->SetTitle(xtitle.Data());
  
  else if(hstack_hist)
    hstack_hist->GetXaxis()->SetTitle(xtitle.Data());
  else
    hstack->GetXaxis()->SetTitle(xtitle.Data());
  if(plot_y_title_ && hstack_hist)
    hstack_hist->GetYaxis()->SetTitle(ytitle.Data());
  else if(plot_y_title_)
    hstack->GetYaxis()->SetTitle(ytitle.Data());

  if(hstack_hist) {
    hstack_hist->GetXaxis()->SetTitleSize(axis_font_size_);
    hstack_hist->GetYaxis()->SetTitleSize(axis_font_size_);
    if(yMin_ < yMax_) hstack_hist->GetYaxis()->SetRangeUser(yMin_,yMax_);    
    else              hstack_hist->GetYaxis()->SetRangeUser(1.e-1,(logY_) ? m*20. : m*1.2);    
    if(xMin_ < xMax_) hstack_hist->GetXaxis()->SetRangeUser(xMin_,xMax_);    
    if(plot_title_) hstack_hist->SetTitle (title.Data());
    else hstack_hist->SetTitle("");
    if(yMin_ < yMax_) {
      hstack_hist->SetMinimum(yMin_);
      hstack_hist->SetMaximum(yMax_);
    } else {
      hstack_hist->SetMinimum(1.e-1);
      hstack_hist->SetMaximum((logY_>0 ? 2.*m : 1.2*m));
    }
  } else {
    hstack->GetXaxis()->SetTitleSize(axis_font_size_);
    hstack->GetYaxis()->SetTitleSize(axis_font_size_);
    if(yMin_ < yMax_) hstack->GetYaxis()->SetRangeUser(yMin_,yMax_);    
    else              hstack->GetYaxis()->SetRangeUser(1.e-1,(logY_) ? m*20. : m*1.2);    
    if(xMin_ < xMax_) hstack->GetXaxis()->SetRangeUser(xMin_,xMax_);    
    if(plot_title_) hstack->SetTitle (title.Data());
    else hstack->SetTitle("");
    if(yMin_ < yMax_) {
      hstack->SetMinimum(yMin_);
      hstack->SetMaximum(yMax_);
    } else {
      hstack->SetMinimum(1.e-1);
      hstack->SetMaximum((logY_>0 ? 2.*m : 1.2*m));
    }
  }
  
  if(plot_data_ && xMin_ < xMax_ && hDataMC) hDataMC->GetXaxis()->SetRangeUser(xMin_,xMax_);    

  if(logY_) {
    if(plot_data_)pad1->SetLogy();
    else          c->SetLogy();
  }
  c->SetGrid();
  if(plot_data_ && hDataMC && data_over_mc_ > 0) {
    pad2->cd();
    pad2->SetGrid();
    c->SetGrid();
    hDataMC->Draw("E");
    TLine* line = new TLine((xMax_ < xMin_) ? hDataMC->GetBinCenter(1)-hDataMC->GetBinWidth(1)/2. : xMin_, 1.,
			    (xMax_ < xMin_) ? hDataMC->GetBinCenter(hDataMC->GetNbinsX())+hDataMC->GetBinWidth(1)/2. : xMax_, 1.);
    line->SetLineColor(kRed);
    line->Draw("same");
    
    hDataMC->GetYaxis()->SetTitle("Data/MC");
    hDataMC->GetXaxis()->SetTitleSize(axis_font_size_);
    hDataMC->GetXaxis()->SetTitleOffset(x_title_offset_);
    hDataMC->GetXaxis()->SetLabelSize(x_label_size_);
    hDataMC->GetYaxis()->SetTitleSize(axis_font_size_);
    hDataMC->GetYaxis()->SetTitleOffset(y_title_offset_);
    hDataMC->GetYaxis()->SetLabelSize(y_label_size_);
    m = hDataMC->GetMaximum();
    double mn = hDataMC->GetMinimum();
    mn = max(0.2*mn,5e-1);
    m = 1.2*m;
    m = min(m, 2.0);
    hDataMC->GetYaxis()->SetRangeUser(mn,m);    
    hDataMC->SetMinimum(mn);
    hDataMC->SetMaximum(m);
    //  hDataMC->GetXaxis()->SetLabelOffset(0.5);
  
    hDataMC->SetMarkerStyle(20);
    //  hDataMC->SetName("hDataMC");
    if(hDataMCErr)
      hDataMCErr->Draw("E2");
  } else if(hDataMC && data_over_mc_ < 0) {
    pad2->cd();
    pad2->SetGrid();
    c->SetGrid();
    m = 0.;
    for(unsigned index = 0; index < hSignalsOverMC.size(); ++index) {
      if(signal_scale_ > 0.) {
	hSignalsOverMC[index]->Scale(1./signal_scale_);
	if(data_over_mc_ == -2) hSignalsOverMC[index]->Scale(1./signal_scale_);
      }
      if(index == 0) hSignalsOverMC[index]->Draw("hist E1");
      else           hSignalsOverMC[index]->Draw("hist E1 same");
      m = max(m, hSignalsOverMC[index]->GetMaximum());
    }
    if(hSignalsOverMC.size() > 0) {
      hSignalsOverMC[0]->GetXaxis()->SetTitle(xtitle.Data());
      if(data_over_mc_ == -2) hSignalsOverMC[0]->GetYaxis()->SetTitle("S^{2}/B");
      else                    hSignalsOverMC[0]->GetYaxis()->SetTitle("S/B");
      hSignalsOverMC[0]->GetXaxis()->SetTitleSize(axis_font_size_);
      hSignalsOverMC[0]->GetXaxis()->SetTitleOffset(x_title_offset_);
      hSignalsOverMC[0]->GetXaxis()->SetLabelSize(x_label_size_);
      hSignalsOverMC[0]->GetYaxis()->SetTitleSize(axis_font_size_);
      hSignalsOverMC[0]->GetYaxis()->SetTitleOffset(y_title_offset_);
      hSignalsOverMC[0]->GetYaxis()->SetLabelSize(y_label_size_);
      hSignalsOverMC[0]->SetAxisRange(m/1.e4,m*5., "Y");
      hSignalsOverMC[0]->SetTitle("");
      if(xMin_ < xMax_) hSignalsOverMC[0]->GetXaxis()->SetRangeUser(xMin_,xMax_);    
      pad2->SetLogy();
    }
  }

  if(stack_as_hist_) {
    for(auto o : *hstack->GetHists()) { //clear out the un-needed histograms
      delete o;
    }
    delete hstack;
  }

  return c;

}

TCanvas* DataPlotter::plot_cdf(TString hist, TString setType, Int_t set, TString label) {

  TH1D* hCDF = 0;
  for(UInt_t i = 0; i < data_.size(); ++i) {
    if(labels_[i] == label) {
      TH1D* tmp = (TH1D*) data_[i]->Get(Form("%s_%i/%s", setType.Data(), set, hist.Data()));
      if(!tmp) continue;
      tmp = (TH1D*) tmp->Clone("tmp");
      // tmp->SetBit(kCanDelete);
      tmp->Scale(scale_[i]);
      
      if(hCDF) {
	hCDF->Add(tmp);
	delete tmp;
      }
      else {
	hCDF = tmp;
	hCDF->SetName("hCDF");
      }
    }
  }
  if(!hCDF) {
    printf("Histogram for CDF making with label %s not found!\n", label.Data());
    return NULL;
  }
  
  //Build CDF (CDF(x) = integral(pdf(x')dx', -infinity, x))
  TH1D* tmp = (TH1D*) hCDF->Clone("tmp");
  tmp->Scale(1./tmp->Integral()/tmp->GetBinWidth(1));
  const int nbins = tmp->GetNbinsX();
  for(Int_t bin = 1; bin <= nbins; ++bin)
    hCDF->SetBinContent(bin, tmp->Integral(0, bin)*tmp->GetBinWidth(1));
  hCDF->Scale(1./hCDF->GetMaximum());
  // tmp->SetBit(kCanDelete);

  int rebinH = rebinH_;
  rebinH_ = 1;
  //With cdf made, now loop through histograms and generate transforms of each
  THStack* hstack = get_stack(hist, setType, set);
  TList* hists = hstack->GetHists();
  {
    auto o = gDirectory->Get(Form("%s_%i_cdf",hist.Data(),set));
    if(o) delete o;
  }
  THStack* hcdfstack = new THStack(Form("%s_%i_cdf",hist.Data(),set),Form("%s_cdf",hist.Data()));
  vector<TH1D*> htransforms;
  int ntransbins = 50;
  double xtransmin = 0.;
  double xtransmax = 1.7;
  for(TObject* hist : *hists) {
    TH1D* htmp = (TH1D*) hist;
    while(auto o = gDirectory->Get(Form("%s_trans_%i", htmp->GetName(),set))) {
      if(o) delete o;
    }
    TH1D* htrans = new TH1D(Form("%s_trans_%i", htmp->GetName(),set), htmp->GetTitle(),
			    ntransbins, xtransmin, xtransmax);
    // htrans->SetBit(kCanDelete);
    
    for(Int_t bin = 1; bin <= htmp->GetNbinsX(); ++bin) {
      Double_t y = hCDF->GetBinContent(bin);
      htrans->Fill(y, htmp->GetBinContent(bin));
    }
    htrans->SetLineColor(htmp->GetLineColor());
    if(fill_alpha_ < 1.)
      htrans->SetFillColorAlpha(htmp->GetFillColor(),fill_alpha_);
    else
      htrans->SetFillColor(htmp->GetFillColor());
    htrans->SetFillStyle(htmp->GetFillStyle());
    htrans->SetLineWidth(htmp->GetLineWidth());
    htrans->SetMarkerStyle(htmp->GetMarkerStyle());
    hcdfstack->Add(htrans);
  }
  
  {
    auto o = gDirectory->Get("data_cdf");
    if(o) delete o;
  }
  TH1D* d = get_data(hist, setType, set);
  TH1D* data_cdf = new TH1D("data_cdf", d->GetTitle(),
			    ntransbins, xtransmin, xtransmax);
  // data_cdf->SetBit(kCanDelete);
  for(Int_t bin = 1; bin <= d->GetNbinsX(); ++bin) {
    Double_t y = hCDF->GetBinContent(bin);
    if(isBlind(y)) continue; //blinding data
    data_cdf->Fill(y, d->GetBinContent(bin));
    data_cdf->SetBinError(data_cdf->FindBin(y), sqrt(data_cdf->GetBinContent(data_cdf->FindBin(y))));
  }
  
  data_cdf->SetLineColor(d->GetLineColor());
  data_cdf->SetFillColor(d->GetFillColor());
  data_cdf->SetFillStyle(d->GetFillStyle());
  data_cdf->SetLineWidth(d->GetLineWidth());
  data_cdf->SetMarkerStyle(d->GetMarkerStyle());
  delete d;

  TCanvas* c = new TCanvas(Form("cdf_%s_%s_%i",hist.Data(),label.Data(),set),Form("cdf_%s_%s_%i",hist.Data(),label.Data(),set), canvas_x_, canvas_y_);
  TPad *pad1, *pad2;


  if(plot_data_) {
    pad1 = new TPad("pad1","pad1",upper_pad_x1_, upper_pad_y1_, upper_pad_x2_, upper_pad_y2_); //xL yL xH xH, (0,0) = bottom left
    pad2 = new TPad("pad2","pad2",lower_pad_x1_, lower_pad_y1_, lower_pad_x2_, lower_pad_y2_);
    pad1->SetTopMargin(upper_pad_topmargin_);
    pad2->SetTopMargin(lower_pad_topmargin_);
    pad1->SetBottomMargin(upper_pad_botmargin_);
    pad2->SetBottomMargin(lower_pad_botmargin_);
    pad1->Draw();
    pad2->Draw();
  } else {
    pad1 = new TPad("pad1","pad1",0.0,0.0,1,1); //xL yL xH xH, (0,0) = bottom left
    pad1->Draw();
  }
  pad1->cd();

  hcdfstack->Draw("hist noclear");
  vector<TH1D*> hsignal = get_signal(hist,setType,set);
  vector<TH1D*> hsignalcdf;
  for(TH1D* signal : hsignal) {
    auto o = gDirectory->Get(Form("%s_trans", signal->GetName()));
    if(o) delete o;
    TH1D* htrans = new TH1D(Form("%s_trans", signal->GetName()), signal->GetTitle(),
			    ntransbins, xtransmin, xtransmax);
    // htrans->SetBit(kCanDelete);

    for(Int_t bin = 1; bin <= signal->GetNbinsX(); ++bin) {
      Double_t y = hCDF->GetBinContent(bin);
      htrans->Fill(y, signal->GetBinContent(bin));
    }
    htrans->SetLineColor(signal->GetLineColor());
    if(fill_alpha_ < 1.)
      htrans->SetFillColorAlpha(signal->GetFillColor(),fill_alpha_);
    else
      htrans->SetFillColor(signal->GetFillColor());
    htrans->SetFillStyle(signal->GetFillStyle());
    htrans->SetLineWidth(signal->GetLineWidth());
    htrans->SetMarkerStyle(signal->GetMarkerStyle());
    htrans->Draw("hist same");
    hsignalcdf.push_back(htrans);
  }
  data_cdf->Draw("same");
  delete hCDF; //done with the CDF

  TH1D* hDataMC = (plot_data_) ? (TH1D*) data_cdf->Clone("hDataMC") : 0;
  // TGraphErrors* hDataMCErr = 0;
  double nmc = 0.;
  int ndata = 0;
  int nb = (data_cdf) ? data_cdf->GetNbinsX() : -1;
  if(hDataMC) {
    // hDataMC->SetBit(kCanDelete);
    hDataMC->Clear();
    TH1D* hlast = get_stack_uncertainty(hcdfstack,Form("uncertainty_cdf_%i", set));
    nmc = hlast->Integral();
    nmc += hlast->GetBinContent(0);
    nmc += hlast->GetBinContent(nb+1);
    ndata = data_cdf->Integral();
    ndata += data_cdf->GetBinContent(0);
    ndata += data_cdf->GetBinContent(nb+1);
    for(int i = 1; i <= nb; ++i) {
      double dataVal = 0;
      double dataErr = 0;
      dataVal = data_cdf->GetBinContent(i);
      dataErr = data_cdf->GetBinError(i);
      dataErr = sqrt(dataVal);
      double mcVal = hlast->GetBinContent(i);
      if(dataVal == 0 || mcVal == 0) {hDataMC->SetBinContent(i,-1); continue;}
      double ratio = dataVal/mcVal;
      double errRatio = (ratio)*(ratio)*(dataErr*dataErr/(dataVal*dataVal));
      errRatio = sqrt(errRatio);
      hDataMC->SetBinContent(i,ratio);
      hDataMC->SetBinError(i,errRatio); 
    }
  }
  
  pad1->BuildLegend();//0.6, 0.9, 0.9, 0.45, "", "L");
  pad1->SetGrid();
  pad1->Update();
  auto o = pad1->GetPrimitive("TPave");
  if(o) {
    auto tl = (TLegend*) o;
    tl->SetDrawOption("L");
    tl->SetTextSize(legend_txt_);
    tl->SetY2NDC(legend_y2_);
    tl->SetY1NDC(legend_y1_);
    tl->SetX1NDC(((doStatsLegend_) ? legend_x1_stats_ : legend_x1_));
    tl->SetX2NDC(legend_x2_);
    tl->SetEntrySeparation(legend_sep_);
    pad1->Update();
  }
  //get axis titles
  TString xtitle;
  TString ytitle;
  TString title;
  get_titles(hist,setType,&xtitle,&ytitle,&title);

  //draw text
  draw_luminosity();
  draw_cms_label();
  
  if(plot_data_ && hDataMC) hDataMC->GetXaxis()->SetTitle(Form("%s CDF", xtitle.Data()));
  else hcdfstack->GetXaxis()->SetTitle(Form("%s CDF", xtitle.Data()));
  if(plot_y_title_) hcdfstack->GetYaxis()->SetTitle(ytitle.Data());
  hcdfstack->GetXaxis()->SetTitleSize(axis_font_size_);
  hcdfstack->GetYaxis()->SetTitleSize(axis_font_size_);
  double m = max(max(m,hcdfstack->GetMaximum()), (data_cdf) ? data_cdf->GetMaximum() : 0.);

  if(yMin_ < yMax_) hcdfstack->GetYaxis()->SetRangeUser(yMin_,yMax_);    
  else              hcdfstack->GetYaxis()->SetRangeUser(1.e-1,m*1.2);    
  if(plot_data_ && xMin_ < xMax_ && hDataMC) hDataMC->GetXaxis()->SetRangeUser(xMin_,xMax_);    
  if(xMin_ < xMax_) hcdfstack->GetXaxis()->SetRangeUser(xMin_,xMax_);    

  if(plot_title_) hcdfstack->SetTitle (title.Data());
  else hcdfstack->SetTitle("");

  if(yMin_ < yMax_) {
    hcdfstack->SetMinimum(yMin_);
    hcdfstack->SetMaximum(yMax_);
  }
  else {
    hcdfstack->SetMinimum(1.e-1);
    hcdfstack->SetMaximum((logY_>0 ? 2.*m : 1.2*m));
  }
  if(logY_) {
    if(plot_data_)pad1->SetLogy();
    else          c->SetLogy();
  }
  c->SetGrid();
  if(plot_data_ && hDataMC) {
    pad2->cd();
    pad2->SetGrid();
    c->SetGrid();
    hDataMC->Draw("E");
    TLine* line = new TLine((xMax_ < xMin_) ? hDataMC->GetBinCenter(1)-hDataMC->GetBinWidth(1)/2. : xMin_, 1.,
			    (xMax_ < xMin_) ? hDataMC->GetBinCenter(hDataMC->GetNbinsX())+hDataMC->GetBinWidth(1)/2. : xMax_, 1.);
    line->SetLineColor(kRed);
    line->Draw("same");
    
    hDataMC->GetYaxis()->SetTitle("Data/MC");
    hDataMC->GetXaxis()->SetTitleSize(axis_font_size_);
    hDataMC->GetXaxis()->SetTitleOffset(x_title_offset_);
    hDataMC->GetXaxis()->SetLabelSize(x_label_size_);
    hDataMC->GetYaxis()->SetTitleSize(axis_font_size_);
    hDataMC->GetYaxis()->SetTitleOffset(y_title_offset_);
    hDataMC->GetYaxis()->SetLabelSize(y_label_size_);
    double m = hDataMC->GetMaximum();
    double mn = hDataMC->GetMinimum();
    mn = max(0.2*mn,5e-1);
    m = 1.2*m;
    m = min(m, 2.0);
    hDataMC->GetYaxis()->SetRangeUser(mn,m);    
    hDataMC->SetMinimum(mn);
    hDataMC->SetMaximum(m);
    //  hDataMC->GetXaxis()->SetLabelOffset(0.5);
  
    hDataMC->SetMarkerStyle(20);
  }

  rebinH_ = rebinH;
  return c;
}

TCanvas* DataPlotter::plot_significance(TString hist, TString setType, Int_t set, TString label,
					bool dir = true, Double_t line_val = -1., bool doVsEff = false,
					TString label1 = "", TString label2 = "") {

  TH1D* hSignal = 0;
  {
    auto o = gDirectory->Get("hSignal");
    if(o) delete o;
  }
  //get the signal histogram
  for(UInt_t i = 0; i < data_.size(); ++i) {
    if(labels_[i] == label) {
      TH1D* tmp = (TH1D*) data_[i]->Get(Form("%s_%i/%s", setType.Data(), set, hist.Data()));
      if(!tmp) continue;
      auto o = gDirectory->Get("tmp");
      if(o) delete o;
      tmp = (TH1D*) tmp->Clone("tmp");
      // tmp->SetBit(kCanDelete);
      tmp->Scale(scale_[i]);
      
      if(hSignal) {
	hSignal->Add(tmp);
	delete tmp;
      }
      else {
	hSignal = tmp;
	hSignal->SetName("hSignal");
      }
    }
  }
  
  if(!hSignal) {
    printf("Histogram for significance making with label %s not found!\n", label.Data());
    return NULL;
  }

  //don't rebin for improved statistics
  int rebinH = rebinH_;
  rebinH_ = 1;

  //take the signal histogram as a template for x-binnning
  TH1D* hEfficiency = (TH1D*) hSignal->Clone("hEfficiency");
  hEfficiency->Clear(); hEfficiency->Reset();
  //get the background stack
  THStack* hstack = get_stack(hist, setType, set);
  TH1D* hlast = (TH1D*) hstack->GetStack()->Last()->Clone("hlast");

  //clean up memory
  for(auto htmp : *hstack->GetHists())
    delete htmp;
  delete hstack;

  //parameters for limit loop
  UInt_t nbins = hSignal->GetNbinsX();
  double clsig = 1.644853627; // 95% CL value
  bool doExactLimit = true;
  
  double xs[nbins], xerrs[nbins]; //for significance graph
  double sigs[nbins], sigsErrUp[nbins], sigsErrDown[nbins];
  double init_sig = -1.;

  double p(0.05), tolerance(0.001), val(-1.);
  Significances significances(p, tolerance, useCLs_, 10);
  for(UInt_t bin = 1; bin <= nbins; ++bin) {
    xs[bin-1] = (dir) ? hSignal->GetBinLowEdge(bin) : hSignal->GetBinLowEdge(bin) + hSignal->GetBinWidth(bin);
    sigs[bin-1] = 0.;
    xerrs[bin-1] = 0.;
    double bkgerr(0.), sigerr(0.);
    double bkgval = (dir) ? hlast->IntegralAndError(bin, nbins, bkgerr) : hlast->IntegralAndError(1, bin, bkgerr);
    double sigval = (dir) ? hSignal->IntegralAndError(bin, nbins, sigerr) : hSignal->IntegralAndError(1, bin, sigerr);
    if(init_sig < 0. && sigval > 0.) init_sig = sigval;
    //plot vs signal efficiency instead
    if(doVsEff) xs[bin-1] = sigval/init_sig;
    hEfficiency->SetBinContent(bin, sigval);
    if(bkgval <= 0. || sigval <= 0.) continue;
    double significance = sigval/sqrt(bkgval)/clsig; //not really significance but ratio of signal to background fluctuation
    if(doExactLimit)  //get 95% CL by finding signal scale so poisson prob n < n_bkg for mu = n_bkg + n_sig = 0.05
      significance = significances.LimitGain(sigval, bkgval, val);
    sigs[bin-1] = significance;
    //Add MC uncertainty band
    if(limit_mc_err_range_) {
      //bkg+1 sigma
      significance = sigval/sqrt(bkgval+bkgerr)/clsig; //not really significance but ratio of signal to background fluctuation
      if(doExactLimit)  //get 95% CL by finding signal scale so poisson prob n < n_bkg for mu = n_bkg + n_sig = 0.05
	significance = significances.LimitGain(sigval, bkgval+bkgerr, val);
      sigsErrUp[bin-1] = sigs[bin-1]-significance;
      //bkg-1 sigma
      significance = sigval/sqrt(max(0.1, bkgval-bkgerr))/clsig; //not really significance but ratio of signal to background fluctuation
      if(doExactLimit)  //get 95% CL by finding signal scale so poisson prob n < n_bkg for mu = n_bkg + n_sig = 0.05
	significance = significances.LimitGain(sigval, max(0.1, bkgval-bkgerr), val);
      sigsErrDown[bin-1] = significance-sigs[bin-1];
    }
  }

  //clean up memory
  delete hlast;
  delete hSignal;
  
  TCanvas* c = new TCanvas(Form("sig_%s_%i",hist.Data(), set), Form("sig_%s_%i",hist.Data(),set), canvas_x_, canvas_y_);
  c->SetTopMargin(0.055);
  // c->SetRightMargin(0.12);
  //get axis titles
  TString xtitle;
  TString ytitle;
  TString title;
  get_titles(hist,setType,&xtitle,&ytitle,&title);

  TGraph* gSignificance = (limit_mc_err_range_) ? new TGraphAsymmErrors(nbins, xs, sigs, xerrs, xerrs, sigsErrUp, sigsErrDown) : new TGraph(nbins, xs, sigs);
  if(verbose_ > 1) {
    std::cout << "Printing up to 15 significance bins (" << nbins << " total):\n";
    for(int bin = 1; bin < min(1.*nbins, 1.*16); ++bin)
      std::cout << "Bin " << bin << ": " << "(x, +xerr, -xerr, y, +yerr, -yerr) = ("
		<< xs[bin-1] << ", " << xerrs[bin-1] << ", " << xerrs[bin-1] << ", " << sigs[bin-1]
		<< ", " << sigsErrDown[bin-1] << ", " << sigsErrUp[bin-1] <<")"
		<< std::endl;
  }
  
  gSignificance->SetName(Form("gsig_%s_%i", label.Data(),set));
  gSignificance->SetTitle(Form("; %s; Limit gain factor",
			       (doVsEff) ? "Efficiency" : (xtitle+" Cut Value").Data()));
  gSignificance->SetLineColor(kBlue);
  if(limit_mc_err_range_) gSignificance->SetFillColor(kGreen);
  gSignificance->SetLineWidth(3);
  if(limit_mc_err_range_)
    gSignificance->Draw("APL3");
  gSignificance->Draw(((limit_mc_err_range_) ? "LX" : "APL"));
  TAxis* yaxis = gSignificance->GetYaxis();
  yaxis->SetRangeUser(0., sig_plot_range_);
  yaxis->SetLabelColor(kBlue);
  yaxis->SetTitleColor(kBlue);
  yaxis->SetTitleSize(0.04);
  gSignificance->GetXaxis()->SetTitleSize(0.04);
  TLine* line;
  TLine* second_line = 0;
  if(xMax_ > xMin_) {
    gSignificance->GetXaxis()->SetRangeUser(xMin_, xMax_);
    line = new TLine(xMin_, 1., xMax_, 1.);
    if(line_val > 0.) second_line = new TLine(xMin_, line_val, xMax_, line_val);
  } else {
    line = new TLine(xs[0], 1., xs[nbins-1], 1.);
    if(line_val > 0.) second_line = new TLine(xs[0], line_val, xs[nbins-1], line_val);
  }
  if(second_line) {
    second_line->SetLineColor(kTeal);
    second_line->SetLineWidth(4);
    second_line->Draw("sames");
  }
  line->SetLineColor(kRed);
  line->SetLineWidth(4);
  line->Draw("sames");
  //draw text
  draw_luminosity(true);
  draw_cms_label(true);
  c->Update();

  if(!doVsEff) {
    //scale to put on same plot
    double rightmax = 1.2*hEfficiency->GetMaximum(); //maximum of second axis
    double scale = gPad->GetUymax()/rightmax;
    hEfficiency->SetLineColor(kGreen+2);
    hEfficiency->SetMarkerColor(kGreen+2);
    hEfficiency->SetMarkerStyle(6);
  
    hEfficiency->Scale(scale);
    hEfficiency->Draw("P same");

    //make second axis on right side of plot
    TGaxis* axis = new TGaxis(gPad->GetUxmax(), gPad->GetUymin(),
			      gPad->GetUxmax(), gPad->GetUymax(), 0, rightmax, 510, "+L");
    axis->SetTitle(Form("n_{%s}",label.Data()));
    axis->SetLabelColor(kGreen+2);
    axis->SetTitleColor(kGreen+2);
    axis->SetTitleSize(0.05);
    axis->SetTitleOffset(-0.7);
    axis->Draw();
  }
  TLegend* leg = new TLegend(0.55, (line_val > 0.) ? 0.55 : 0.6, 0.89, 0.8);
  leg->AddEntry(gSignificance, "Limit Gain", "L");
  if(!doVsEff) leg->AddEntry(hEfficiency, "N(Signal)", "L");
  leg->AddEntry(line, (label1 == "") ? "Current Limit" : label1.Data(), "L");
  if(second_line) leg->AddEntry(second_line, (label2 == "") ? "Previous Limit" : label2.Data(), "L");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->Draw();
  rebinH_ = rebinH;
  if(logY_) c->SetLogy();
  c->SetGrid();
  return c;
}
TCanvas* DataPlotter::print_stack(TString hist, TString setType, Int_t set) {
  TCanvas* c = plot_stack(hist,setType,set);
  if(!c) return c;
  TString year_dir = "";
  for(unsigned index = 0; index < years_.size(); ++index) {
    year_dir += years_[index];
    if(index != years_.size() - 1) year_dir += "_";
  }
  //FIXME: use a method to build the directory path for all printing functions
  c->Print(Form("figures/%s/%s/%s/stack_%s%s%s%s_%s_set_%i.png",folder_.Data(),selection_.Data(), year_dir.Data(),
		(setType+"_"+hist).Data(),
		(logY_ ? "_log":""),
		((plot_data_) ? "_data":""), (stack_as_hist_ ? "_totbkg" : ""),
		((data_over_mc_ < 0) ? "sigOverBkg" : "dataOverMC"),set));
  return c;
}

TCanvas* DataPlotter::print_hist(TString hist, TString setType, Int_t set) {
  TCanvas* c = plot_hist(hist,setType,set);
  cout << "plotted hist" << endl;
  if(!c) return c;
  TString year_dir = "";
  for(unsigned index = 0; index < years_.size(); ++index) {
    year_dir += years_[index];
    if(index != years_.size() - 1) year_dir += "_";
  }
  c->Print(Form("figures/%s/%s/%s/hist_%s%s%s_%s_set_%i.png",folder_.Data(),selection_.Data(), year_dir.Data(),
		(setType+"_"+hist).Data(),
		(logY_ ? "_log":""),
		((plot_data_) ? "_data":""),"dataOverMC",set));
  return c;
}

TCanvas* DataPlotter::print_2Dhist(TString hist, TString setType, Int_t set) {
  TCanvas* c = plot_2Dhist(hist,setType,set);
  cout << "plotted 2D hist" << endl;
  if(!c) return c;
  TString year_dir = "";
  for(unsigned index = 0; index < years_.size(); ++index) {
    year_dir += years_[index];
    if(index != years_.size() - 1) year_dir += "_";
  }
  c->Print(Form("figures/%s/%s/%s/hist2D_%s%s_%s_set_%i.png",folder_.Data(),selection_.Data(), year_dir.Data(),
		(setType+"_"+hist).Data(),
		((plot_data_) ? "_data":""),"dataOverMC",set));
  return c;
}

TCanvas* DataPlotter::print_single_2Dhist(TString hist, TString setType, Int_t set, TString label) {
  TCanvas* c = plot_single_2Dhist(hist,setType,set,label);
  cout << "plotted 2D hist " << label.Data() << endl;
  if(!c) return c;
  label.ReplaceAll("#",""); //for ease of use in bash
  label.ReplaceAll(" ", "");
  label.ReplaceAll("/", "");
  TString year_dir = "";
  for(unsigned index = 0; index < years_.size(); ++index) {
    year_dir += years_[index];
    if(index != years_.size() - 1) year_dir += "_";
  }
  c->Print(Form("figures/%s/%s/%s/hist2D_%s_%s%s%s_%s_set_%i.png",folder_.Data(),selection_.Data(), year_dir.Data(), label.Data(), 
		(setType+"_"+hist).Data(),
		(logZ_ ? "_log":""),
		((plot_data_) ? "_data":""),"dataOverMC",set));
  return c;
}

TCanvas* DataPlotter::print_cdf(TString hist, TString setType, Int_t set, TString label) {
  TCanvas* c = plot_cdf(hist,setType,set,label);
  if(!c) return c;
  label.ReplaceAll("#",""); //for ease of use in bash
  label.ReplaceAll(" ", "");
  label.ReplaceAll("/", "");
  TString year_dir = "";
  for(unsigned index = 0; index < years_.size(); ++index) {
    year_dir += years_[index];
    if(index != years_.size() - 1) year_dir += "_";
  }
  c->Print(Form("figures/%s/%s/%s/cdf_%s_%s%s%s_%s_set_%i.png",folder_.Data(),selection_.Data(), year_dir.Data(), label.Data(),
		(setType+"_"+hist).Data(),
		(logY_ ? "_log":""),
		((plot_data_) ? "_data":""),"dataOverMC",set));
  return c;
}

TCanvas* DataPlotter::print_significance(TString hist, TString setType, Int_t set, TString label,
					 bool dir=true, Double_t line_val = -1., bool doVsEff = false,
					 TString label1 = "", TString label2 = "") {
  TCanvas* c = plot_significance(hist,setType,set,label, dir, line_val, doVsEff, label1, label2);
  if(!c) return c;
  label.ReplaceAll("#",""); //for ease of use in bash
  label.ReplaceAll(" ", "");
  label.ReplaceAll("/", "");
  TString year_dir = "";
  for(unsigned index = 0; index < years_.size(); ++index) {
    year_dir += years_[index];
    if(index != years_.size() - 1) year_dir += "_";
  }
  c->Print(Form("figures/%s/%s/%s/sig_%s%s_%s%s_set_%i.png",folder_.Data(),selection_.Data(), year_dir.Data(),
		(doVsEff) ? "vsEff_" : "", label.Data(),
		(setType+"_"+hist).Data(),
		(logY_ ? "_log":""),set));
  return c;
}

Int_t DataPlotter::print_stacks(vector<TString> hists, vector<TString> setTypes, vector<Int_t> sets
				, vector<Double_t> xMaxs, vector<Double_t> xMins, vector<Int_t> rebins
				, vector<Double_t> signal_scales = {}, vector<Int_t> base_rebins = {}) {
  if(hists.size() != setTypes.size()) {
    printf("hist size != hist type size!\n");
    return 1;
  } 
  if(hists.size() != xMaxs.size()) {
    printf("hist size != xMaxs size!\n");
    return 2;
  } 
  if(hists.size() != xMins.size()) {
    printf("hist size != xMins size!\n");
    return 3;
  } 
  if(hists.size() != rebins.size()) {
    printf("hist size != rebins size!\n");
    return 4;
  }
  if(signal_scales.size() > 0 && signal_scales.size() != sets.size()) {
    printf("signal scales size != sets size!\n");
    return 5;
  }
  if(base_rebins.size() > 0 && base_rebins.size() != sets.size()) {
    printf("base rebins size != sets size!\n");
    return 6;
  }
  Int_t set_index = 0;
  Int_t base_rebin = 1;
  for(Int_t set : sets) {
    if(signal_scales.size() > 0) signal_scale_ = signal_scales[set_index];
    for(UInt_t i = 0; i < hists.size(); ++i) {
      TString hist(hists[i]);
      TString setType(setTypes[i]);
      xMax_ = xMaxs[i];
      xMin_ = xMins[i];
      rebinH_ = rebins[i]*base_rebin;
      if(rebinH_ == 0) rebinH_ = 1;
      TCanvas* c = print_stack(hist,setType,set);
      Int_t status = (c) ? 0 : 1;
      if(verbose_ > 0) printf("Printing Data/MC stack %s %s set %i has status %i\n",setType.Data(),hist.Data(),set,status);
      Empty_Canvas(c);
    }
    ++set_index;
  }
  return 0;
}

Int_t DataPlotter::print_hists(vector<TString> hists, vector<TString> setTypes, vector<Int_t> sets
			       , vector<Double_t> xMaxs, vector<Double_t> xMins, vector<Int_t> rebins
			       , vector<Double_t> signal_scales = {}, vector<Int_t> base_rebins = {}) {
  if(hists.size() != setTypes.size()) {
    printf("hist size != hist type size!\n");
    return 1;
  } 
  if(hists.size() != xMaxs.size()) {
    printf("hist size != xMaxs size!\n");
    return 2;
  } 
  if(hists.size() != xMins.size()) {
    printf("hist size != xMins size!\n");
    return 3;
  } 
  if(hists.size() != rebins.size()) {
    printf("hist size != rebins size!\n");
    return 4;
  }
  if(signal_scales.size() > 0 && signal_scales.size() != sets.size()) {
    printf("signal scales size != sets size!\n");
    return 5;
  }
  if(base_rebins.size() > 0 && base_rebins.size() != sets.size()) {
    printf("base rebins size != sets size!\n");
    return 6;
  }
  Int_t set_index = 0;
  Int_t base_rebin = 1;
  for(Int_t set : sets) {
    if(signal_scales.size() > 0) signal_scale_ = signal_scales[set_index];
    if(base_rebins.size() > 0) base_rebin = base_rebins[set_index];
    for(UInt_t i = 0; i < hists.size(); ++i) {
      TString hist(hists[i]);
      TString setType(setTypes[i]);
      xMax_ = xMaxs[i];
      xMin_ = xMins[i];
      rebinH_ = rebins[i]*base_rebin;
      TCanvas* c = print_hist(hist,setType,set);
      Int_t status = (c) ? 0 : 1;
      printf("Printing Data/MC hist %s %s set %i has status %i\n",setType.Data(),hist.Data(),set,status);
      Empty_Canvas(c);
    }
    ++set_index;
  }
  
  return 0;
}


Int_t DataPlotter::init_files() {

  UInt_t nFiles = fileNames_.size();
  vector<TFile*> f;
  for(UInt_t i = 0; i < nFiles; ++i) {
    if(verbose_ > 0) 
      std::cout << "Initializing dataset, filepath = " << fileNames_[i].Data()
		<< " name = " << names_[i].Data()
		<< " label = " << labels_[i].Data()
		<< " isData = " << isData_[i]
		<< " xsec = " << xsec_[i]
		<< " isSignal = " << isSignal_[i] << std::endl;
    f.push_back(TFile::Open(fileNames_[i].Data(),"READ"));
    if(f[i]) data_.push_back((TFile*) f[i]->Get("Data"));
    else {
      printf("File %s not found! Exiting\n", fileNames_[i].Data());
      return 1;
    }
  }
  for(UInt_t i = 0; i < nFiles; ++i) {
    if(isData_[i]) scale_.push_back(1.);
    else {
      // loop through the directory and add events from each event histogram (in case of hadd combined dataset)
      double nevents = 0.;
      TKey* key = 0;
      TIter nextkey(f[i]->GetListOfKeys());
      TH1F* events = 0;
      while((key = (TKey*)nextkey())) {
	TObject* obj = key->ReadObj();
	if(obj->InheritsFrom(TH1::Class())) {
	  events = (TH1F*) obj;
	  nevents += events->GetBinContent(1); //total events
	  nevents -= 2.*events->GetBinContent(10); //negative weight events
	  if(debug_ > 0) printf("%s %s: bin 1, bin 11: %.0f, %.0f\n", names_[i].Data(), events->GetName(),
				events->GetBinContent(1), events->GetBinContent(10));
	}
      }
      if(nevents <= 0.) {
	std::cout << "Error! No events in the events histogram for " << names_[i].Data() << std::endl;
	return 1;
      }
      scale_.push_back(1./(nevents)*xsec_[i]*((lums_.size() > 0) ? lums_[dataYear_[i]] : lum_));
    }
  }

  gStyle->SetTitleW(0.8f);
  gStyle->SetOptStat(0);
  if(useOpenGL_) gStyle->SetCanvasPreferGL(kTRUE);
  rnd_ = new TRandom(seed_);

  return 0;
}

Int_t DataPlotter::add_dataset(TString filepath, TString name, TString label, bool isData, double xsec, bool isSignal) {
  fileNames_.push_back(filepath);
  names_.push_back(name);
  labels_.push_back(label);
  isData_.push_back(isData);
  if(!isData)
    xsec_.push_back(xsec);
  else
    xsec_.push_back(1.);
  isSignal_.push_back(isSignal);
  int dataYear = 0;
  if(filepath.Contains("2016"))      dataYear = 2016;
  else if(filepath.Contains("2017")) dataYear = 2017;
  else if(filepath.Contains("2018")) dataYear = 2018;
  dataYear_.push_back(dataYear);
  if(verbose_ > 0) 
    std::cout << "Added dataset, filepath = " << filepath.Data()
	      << " name = " << name.Data()
	      << " label = " << label.Data()
	      << " isData = " << isData
	      << " xsec = " << xsec
	      << " isSignal = " << isSignal
	      << " year = " << dataYear
	      << std::endl;
    
  
  return 0;
}
