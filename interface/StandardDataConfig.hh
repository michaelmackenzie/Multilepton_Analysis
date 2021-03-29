#ifndef __STANDARDDATACONFIG__
#define __STANDARDDATACONFIG__
//Class to configure data and MC info for plotting
#include "interface/DataCard_t.hh"
#include "interface/CrossSections.hh"

class StandardDataConfig {
public:
  typedef DataCard_t dcard;

  static std::vector<dcard> GetCards(std::vector<dcard> &cards, TString selection, std::vector<int> years, TString options) {
    CrossSections xs;
    for(int year : years) {

      ///////////////////////
      // Add background MC //
      ///////////////////////

      //combine DY extension sample
      long num1((year == 2018) ? 1 : xs.GetGenNumber("DY50", year)), num2((year == 2018) ? 1 : xs.GetGenNumber("DY50-ext",year));
      double scale = (year == 2018) ? 1. : (num1/(num1+num2));
      //card constructor:   filepath,              name,                  label,      isData,                   xsec               ,  isSignal, year,  color,
      cards.push_back(dcard("DY50"               , "DY50"               , "Drell-Yan", false, scale*xs.GetCrossSection("DY50"         ), false, year, kRed-7   ));
      if(year != 2018) {
        cards.push_back(dcard("DY50-ext"         , "DY50-ext"           , "Drell-Yan", false, (1.-scale)*xs.GetCrossSection("DY50"    ), false, year, kRed-7   ));
      }
      cards.push_back(dcard("SingleAntiToptW"    , "SingleAntiToptW"    , "Top"      , false, xs.GetCrossSection("SingleAntiToptW"    ), false, year, kYellow-7));
      cards.push_back(dcard("SingleToptW"        , "SingleToptW"        , "Top"      , false, xs.GetCrossSection("SingleToptW"        ), false, year, kYellow-7));
      cards.push_back(dcard("ttbarToHadronic"    , "ttbarToHadronic"    , "Top"      , false, xs.GetCrossSection("ttbarToHadronic")    , false, year, kYellow-7));
      cards.push_back(dcard("ttbarToSemiLeptonic", "ttbarToSemiLeptonic", "Top"      , false, xs.GetCrossSection("ttbarToSemiLeptonic"), false, year, kYellow-7));
      cards.push_back(dcard("ttbarlnu"           , "ttbarlnu"           , "Top"      , false, xs.GetCrossSection("ttbarlnu"           ), false, year, kYellow-7));
      cards.push_back(dcard("WWW"                , "WWW"                , "W/Diboson", false, xs.GetCrossSection("WWW"                ), false, year, kViolet-9));
      cards.push_back(dcard("WZ"                 , "WZ"                 , "W/Diboson", false, xs.GetCrossSection("WZ"                 ), false, year, kViolet-9));
      cards.push_back(dcard("ZZ"                 , "ZZ"                 , "W/Diboson", false, xs.GetCrossSection("ZZ"                 ), false, year, kViolet-9));
      cards.push_back(dcard("WW"                 , "WW"                 , "W/Diboson", false, xs.GetCrossSection("WW"                 ), false, year, kViolet-9));
      //combine W+Jets extension sample
      num1  = (year == 2018) ? 1  : xs.GetGenNumber("Wlnu", year);
      num2  = (year == 2018) ? 1  : xs.GetGenNumber("Wlnu-ext", year);
      scale = (year == 2018) ? 1. : (num1/(num1+num2));
      cards.push_back(dcard("Wlnu"               , "Wlnu"               , "W/Diboson", false, scale*xs.GetCrossSection("Wlnu"         ), false, year, kViolet-9));
      if(year != 2018){
        cards.push_back(dcard("Wlnu-ext"         , "Wlnu-ext"           , "W/Diboson", false, (1.-scale)*xs.GetCrossSection("Wlnu"    ), false, year, kViolet-9));
      }

      ///////////////////
      // Add signal MC //
      ///////////////////

      if(selection == "emu") {
        cards.push_back(dcard("ZEMu"             , "ZEMu"             , "Z->e#mu"   , false, xs.GetCrossSection("ZEMu"  ), true, year, kBlue   ));
        cards.push_back(dcard("HEMu"             , "HEMu"             , "H->e#mu"   , false, xs.GetCrossSection("HEMu"  ), true, year, kGreen-1));
      } else if(selection.Contains("etau") || selection == "ee") {
        cards.push_back(dcard("ZETau"            , "ZETau"            , "Z->e#tau"  , false, xs.GetCrossSection("ZETau" ), true, year, kBlue   ));
        cards.push_back(dcard("HETau"            , "HETau"            , "H->e#tau"  , false, xs.GetCrossSection("HETau" ), true, year, kGreen-1));
      } else if(selection.Contains("mutau") || selection == "mumu") {
        cards.push_back(dcard("ZMuTau"           , "ZMuTau"           , "Z->#mu#tau", false, xs.GetCrossSection("ZMuTau"), true, year, kBlue   ));
        cards.push_back(dcard("HMuTau"           , "HMuTau"           , "H->#mu#tau", false, xs.GetCrossSection("HMuTau"), true, year, kGreen-1));
      }

      //////////////
      // Add data //
      //////////////

      if(selection != "etau"  && selection != "ee"  ) cards.push_back(dcard("SingleMu" , "SingleMu" , "Data", true , 1., false, year));
      if(selection != "mutau" && selection != "mumu") cards.push_back(dcard("SingleEle", "SingleEle", "Data", true , 1., false, year));
    }
    return cards;
  }
};

#endif
