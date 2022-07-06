#include "interface/CrossSections.hh"

using namespace CLFV;

CrossSections::CrossSections(int useUL, int ZMode) {
  //cross sections by MC dataset
  double zll = (useUL > 0) ? 6435. : /*2075.14/0.0337*/ 6077.22; //UL: AMC@NLO = 6435 MadGraph = 5321
  if(ZMode == 1 && useUL == 0) zll = 4963.0; //2016 Legacy AMC@NLO (?)

  const double br_ll = 0.0337; //branching ratio of Z->ll
  const double zxs = zll/(3.*br_ll);
  const double higgs = (48.61+3.766+0.5071+1.358+0.880);
  values_["DY50"                    ][2016] = zll    ;
  values_["DY50"                    ][2017] = (ZMode == 1) ? 6435. : zll;
  values_["DY50"                    ][2018] = (ZMode == 1) ? 6435. : zll;
  values_["DY10to50"                ][2016] =  21658. ; //18810. ;
  values_["DY10to50"                ][2017] =  21658. ; //15890. ;
  values_["DY10to50"                ][2018] =  21658. ; //15890. ;
  values_["Z"                       ][2016] =  zxs    ;
  values_["Br_Zll"                  ][2016] =  br_ll  ;
  values_["H"                       ][2016] =  higgs  ;
  values_["SingleAntiToptW"         ][2016] =   35.85 ; //34.91  ;
  values_["SingleToptW"             ][2016] =   35.85 ; //34.91  ;
  values_["SingleAntiToptChannel"   ][2016] =   80.95 ;
  values_["SingleToptChannel"       ][2016] =  136.02 ;
  values_["WWW"                     ][2016] =  0.2086 ;
  values_["WWZ"                     ][2016] =  0.1651 ;
  values_["WZ"                      ][2016] =  47.13  ; //27.6   ;
  values_["ZZ"                      ][2016] =  16.523 ; //12.14  ;
  values_["WW"                      ][2016] =  12.178 ; //WW->2l2nu
  values_["WWLNuQQ"                 ][2016] =  43.53  ;
  values_["EWKWminus"               ][2016] =  23.24  ;
  values_["EWKWplus"                ][2016] =  29.59  ;
  values_["EWKZ-M50"                ][2016] =   4.321 ;
  values_["WGamma"                  ][2016] =    464.4;
  values_["Wlnu"                    ][2016] =  61526.7; //52850.0;
  values_["Wlnu-1J"                 ][2016] =  11823.6; //11805.6; //taken from H->tau+tau
  values_["Wlnu-2J"                 ][2016] =   3907.2; //3891.0;
  values_["Wlnu-3J"                 ][2016] =   1136.3; //1153.2;
  values_["Wlnu-4J"                 ][2016] =    616.0; //60.67;
  values_["Wlnu-1J"                 ][2017] =   9386.3; //9370.5; //taken from H->tau+tau
  values_["Wlnu-2J"                 ][2017] =   3166.3; //3170.9;
  values_["Wlnu-3J"                 ][2017] =   1131.2; //1132.5;
  values_["Wlnu-4J"                 ][2017] =    623.5; //631.5;
  values_["Wlnu-1J"                 ][2018] =   9341.2; //9328.1; //taken from H->tau+tau
  values_["Wlnu-2J"                 ][2018] =   3196.0; //3181.5;
  values_["Wlnu-3J"                 ][2018] =   1110.9; //1116.2;
  values_["Wlnu-4J"                 ][2018] =    625.3; // 629.3;
  values_["ttbarToSemiLeptonic"     ][2016] =  365.35 ;
  values_["ttbarlnu"                ][2016] =  88.29  ; //87.31  ;
  values_["ttbarToHadronic"         ][2016] =  377.96 ;
  values_["SingleEle"               ][2016] =  1.     ;
  values_["SingleMu"                ][2016] =  1.     ;
  values_["QCDEMEnrich15to20"       ][2016] = 1327000.;
  values_["QCDEMEnrich20to30"       ][2016] = 4897000.;
  values_["QCDEMEnrich30to50"       ][2016] = 6379000.;
  values_["QCDEMEnrich50to80"       ][2016] = 1984000.;
  values_["QCDEMEnrich80to120"      ][2016] =  366500.;
  values_["QCDEMEnrich120to170"     ][2016] =   66490.;
  values_["QCDEMEnrich170to300"     ][2016] =   16480.;
  values_["QCDEMEnrich300toInf"     ][2016] =    1097.;
  values_["QCDDoubleEMEnrich30to40" ][2016] =   22180.;
  values_["QCDDoubleEMEnrich30toInf"][2016] =  247000.;
  values_["QCDDoubleEMEnrich40toInf"][2016] =  113100.;

  //////////////////////////////////////////////////////////////////////////////////
  //Scale factors for embedding due to inefficiencies in processing

  //2016
  values_["Embed-EMu-B"][2016] = 1.0500;
  values_["Embed-EMu-C"][2016] = 1.0384;
  values_["Embed-EMu-D"][2016] = 1.0000;
  values_["Embed-EMu-E"][2016] = 1.1195;
  values_["Embed-EMu-F"][2016] = 1.1512;
  values_["Embed-EMu-G"][2016] = 1.0869;
  values_["Embed-EMu-H"][2016] = 1.1368;

  values_["Embed-ETau-B"][2016] = 1.0630;
  values_["Embed-ETau-C"][2016] = 1.0001;
  values_["Embed-ETau-D"][2016] = 1.0002;
  values_["Embed-ETau-E"][2016] = 1.1017;
  values_["Embed-ETau-F"][2016] = 1.1401;
  values_["Embed-ETau-G"][2016] = 1.1246;
  values_["Embed-ETau-H"][2016] = 1.1758;

  values_["Embed-MuTau-B"][2016] = 1.0833;
  values_["Embed-MuTau-C"][2016] = 1.2368;
  values_["Embed-MuTau-D"][2016] = 1.0171;
  values_["Embed-MuTau-E"][2016] = 1.1940;
  values_["Embed-MuTau-F"][2016] = 1.1564;
  values_["Embed-MuTau-G"][2016] = 1.1467;
  values_["Embed-MuTau-H"][2016] = 1.1323;

  values_["Embed-MuMu-B"][2016]  = 1.0657;
  values_["Embed-MuMu-C"][2016]  = 1.0195;
  values_["Embed-MuMu-D"][2016]  = 1.0178;
  values_["Embed-MuMu-E"][2016]  = 1.0573;
  values_["Embed-MuMu-F"][2016]  = 1.1227;
  values_["Embed-MuMu-G"][2016]  = 1.0557;
  values_["Embed-MuMu-H"][2016]  = 1.0611;

  values_["Embed-ElEl-B"][2016]  = 1.0280;
  values_["Embed-ElEl-C"][2016]  = 1.0329;
  values_["Embed-ElEl-D"][2016]  = 1.0198;
  values_["Embed-ElEl-E"][2016]  = 1.0517;
  values_["Embed-ElEl-F"][2016]  = 1.0390;
  values_["Embed-ElEl-G"][2016]  = 1.1005;
  values_["Embed-ElEl-H"][2016]  = 1.0498;

  // *** Total 2016 ElMu : Total Mini / Nano = 29697000/27318414 = 1.0870
  // *** Total 2016 ElTau: Total Mini / Nano = 26477000/24049075 = 1.1009
  // *** Total 2016 MuTau: Total Mini / Nano = 26367000/23397800 = 1.1269
  // *** Total 2016 MuMu : Total Mini / Nano = 38448000/36381476 = 1.0568
  // *** Total 2016 ElEl : Total Mini / Nano = 34637000/32933414 = 1.0517

  //2017
  values_["Embed-EMu-B"][2017] = 2.0244;
  values_["Embed-EMu-C"][2017] = 4.5268;
  values_["Embed-EMu-D"][2017] = 1.1387;
  values_["Embed-EMu-E"][2017] = 2.6201;
  values_["Embed-EMu-F"][2017] = 6.0306;

  values_["Embed-ETau-B"][2017] = 2.4139;
  values_["Embed-ETau-C"][2017] = 3.2928;
  values_["Embed-ETau-D"][2017] = 1.0589;
  values_["Embed-ETau-E"][2017] = 2.4626;
  values_["Embed-ETau-F"][2017] = 4.1141;

  values_["Embed-MuTau-B"][2017] = 2.2020;
  values_["Embed-MuTau-C"][2017] = 5.0035;
  values_["Embed-MuTau-D"][2017] = 1.1480;
  values_["Embed-MuTau-E"][2017] = 2.7259;
  values_["Embed-MuTau-F"][2017] = 5.6159;

  values_["Embed-MuMu-B"][2017]  = 1.9039;
  values_["Embed-MuMu-C"][2017]  = 3.2283;
  values_["Embed-MuMu-D"][2017]  = 1.3284;
  values_["Embed-MuMu-E"][2017]  = 2.8647;
  values_["Embed-MuMu-F"][2017]  = 6.8001;

  values_["Embed-ElEl-B"][2017]  = 1.5825;
  values_["Embed-ElEl-C"][2017]  = 2.7471;
  values_["Embed-ElEl-D"][2017]  = 1.1859;
  values_["Embed-ElEl-E"][2017]  = 2.5760;
  values_["Embed-ElEl-F"][2017]  = 5.4018;


  // *** Total 2017 ElMu  Mini / Nano = 37713920/12833285 = 2.9387
  // *** Total 2017 ElTau Mini / Nano = 32788982/12741595 = 2.5733
  // *** Total 2017 MuTau Mini / Nano = 34683979/11470240 = 3.0238
  // *** Total 2017 MuMu  Mini / Nano = 64519124/21647343 = 2.9804
  // *** Total 2017 ElEl  Mini / Nano = 45398261/17827258 = 2.5465

  //2018
  values_["Embed-EMu-A"][2018] = 1.0832;
  values_["Embed-EMu-B"][2018] = 1.0118;
  values_["Embed-EMu-C"][2018] = 1.1415;
  values_["Embed-EMu-D"][2018] = 4.6364;

  values_["Embed-ETau-A"][2018] = 1.1062;
  values_["Embed-ETau-B"][2018] = 1.2357;
  values_["Embed-ETau-C"][2018] = 1.1306;
  values_["Embed-ETau-D"][2018] = 2.4541;

  values_["Embed-MuTau-A"][2018] = 1.0764;
  values_["Embed-MuTau-B"][2018] = 1.0591;
  values_["Embed-MuTau-C"][2018] = 1.0022;
  values_["Embed-MuTau-D"][2018] = 2.7302;

  values_["Embed-MuMu-A"][2018]  = 1.1174;
  values_["Embed-MuMu-B"][2018]  = 1.2995;
  values_["Embed-MuMu-C"][2018]  = 1.0580;
  values_["Embed-MuMu-D"][2018]  = 7.5245;

  values_["Embed-ElEl-A"][2018]  = 1.2749;
  values_["Embed-ElEl-B"][2018]  = 1.0317;
  values_["Embed-ElEl-C"][2018]  = 1.0161;
  values_["Embed-ElEl-D"][2018]  = 2.8777;

  // --- Total 2018 ElMu  Mini / Nano = 55484067/30962759 = 1.7919
  // --- Total 2018 ElTau Mini / Nano = 48434386/30515756 = 1.5871
  // --- Total 2018 MuTau Mini / Nano = 48428591/31170318 = 1.5536
  // --- Total 2018 MuMu  Mini / Nano = 78733782/39175846 = 2.0097
  // --- Total 2018 ElEl  Mini / Nano = 59254929/35680605 = 1.6594

  ////////////////////////////////////////////////////////////
  // Signal example branching fractions

  //normalized to example branching fractions
  values_["ZEMu"  ][2016] = zxs*5e-7;   //PDG: 7.5e-7 ATLAS: 7.5e-7 CMS: 7.3e-7
  values_["ZETau" ][2016] = zxs*5e-6; //PDG: 9.8e-6 ATLAS: 8.1e-6 (13 TeV)
  values_["ZMuTau"][2016] = zxs*5e-6; //PDG: 1.2e-5 ATLAS: 6.3e-6 (13 TeV) 6.1e-6 (13+8 TeV)
  values_["HEMu"  ][2016] = higgs*1.e-4; //PDG: 3.5e-4 CMS: 5.6e-5 ATLAS: 5.9e-5
  values_["HETau" ][2016] = higgs*1.e-3; //PDG: 6.1e-3 CMS: 1.9e-3
  values_["HMuTau"][2016] = higgs*1.e-3; //PDG: 2.5e-3 CMS: 1.6e-3

  //data SS qcd estimates
  //give fraction of SS --> OS or j --> tau weighted (Data - MC) / Data for full Run-II
  values_["QCD_mutau"][2016] = 0.918;
  values_["QCD_etau" ][2016] = 0.935;
  values_["QCD_emu"  ][2016] = 0.342;




  ////////////////////////////////////////////////////////////
  // Fraction of negative events

  neg_frac_["DY50-amc"           ][2016] = 0.1367 ;
  neg_frac_["SingleToptW"        ][2016] = 0.003708;
  neg_frac_["SingleAntiToptW"    ][2016] = 0.00369;
  neg_frac_["ttbarToHadronic"    ][2016] = 0.00406;
  neg_frac_["ttbarToSemiLeptonic"][2016] = 0.003979;
  neg_frac_["ttbarlnu"           ][2016] = 0.00402;
  neg_frac_["WWW"                ][2016] = 0.06054;
  neg_frac_["WGamma"             ][2016] = 0.1799;

  neg_frac_["DY50-amc"           ][2018] = 0.163  ;
  neg_frac_["SingleToptW"        ][2018] = 0.003758;
  neg_frac_["SingleAntiToptW"    ][2018] = 0.03002;
  neg_frac_["ttbarToHadronic"    ][2018] = 0.00388;
  neg_frac_["ttbarToSemiLeptonic"][2018] = 0.003887;
  neg_frac_["ttbarlnu"           ][2018] = 0.00379;
  neg_frac_["WW "                ][2018] = 0.001916;
  neg_frac_["WWLNuQQ"            ][2018] = 0.001895;
  neg_frac_["WWW"                ][2018] = 0.06197;
  neg_frac_["WGamma"             ][2018] = 2.986e-05;

  ///////////////////////
  // generated numbers //
  ///////////////////////

  //Retrieved from DAS, can access on command line via:
  //$> das_client -query="dataset <das name> | grep dataset.nevents "
  numbers_[2016]["Wlnu"    ] =  29514020;
  numbers_[2016]["Wlnu-ext"] =  57402435;
  numbers_[2017]["Wlnu"    ] =  33073306;
  numbers_[2017]["Wlnu-ext"] =  44627200;
  numbers_[2016]["DY50"    ] =  49748967;
  numbers_[2016]["DY50-ext"] =  93007332;
  numbers_[2017]["DY50"    ] =  25757729;
  numbers_[2017]["DY50-ext"] = 186217773;
  numbers_[2018]["DY50"    ] = 100194597;

  //Embedding generation numbers (NANO, not MINI)

  //2016
  numbers_[2016]["Embed-EMu-B"] = 4508465;
  numbers_[2016]["Embed-EMu-C"] = 1962513;
  numbers_[2016]["Embed-EMu-D"] = 3324740;
  numbers_[2016]["Embed-EMu-E"] = 2726153;
  numbers_[2016]["Embed-EMu-F"] = 1988278;
  numbers_[2016]["Embed-EMu-G"] = 6047364;
  numbers_[2016]["Embed-EMu-H"] = 6760901;

  numbers_[2016]["Embed-ETau-B"] = 3966702;
  numbers_[2016]["Embed-ETau-C"] = 1817674;
  numbers_[2016]["Embed-ETau-D"] = 2966135;
  numbers_[2016]["Embed-ETau-E"] = 2466026;
  numbers_[2016]["Embed-ETau-F"] = 1785747;
  numbers_[2016]["Embed-ETau-G"] = 5216000;
  numbers_[2016]["Embed-ETau-H"] = 5830791;

  numbers_[2016]["Embed-MuTau-B"] = 3906285;
  numbers_[2016]["Embed-MuTau-C"] = 1374478;
  numbers_[2016]["Embed-MuTau-D"] = 2915000;
  numbers_[2016]["Embed-MuTau-E"] = 2274690;
  numbers_[2016]["Embed-MuTau-F"] = 1760599;
  numbers_[2016]["Embed-MuTau-G"] = 5114478;
  numbers_[2016]["Embed-MuTau-H"] = 6052270;

  //2017
  numbers_[2017]["Embed-EMu-B"] = 2012635;
  numbers_[2017]["Embed-EMu-C"] = 2068587;
  numbers_[2017]["Embed-EMu-D"] = 3533197;
  numbers_[2017]["Embed-EMu-E"] = 3290180;
  numbers_[2017]["Embed-EMu-F"] = 1928686;

  numbers_[2017]["Embed-ETau-B"] = 1532529;
  numbers_[2017]["Embed-ETau-C"] = 2457960;
  numbers_[2017]["Embed-ETau-D"] = 3278083;
  numbers_[2017]["Embed-ETau-E"] = 3023027;
  numbers_[2017]["Embed-ETau-F"] = 2449996;

  numbers_[2017]["Embed-MuTau-B"] = 1755514;
  numbers_[2017]["Embed-MuTau-C"] = 1714208;
  numbers_[2017]["Embed-MuTau-D"] = 3206086;
  numbers_[2017]["Embed-MuTau-E"] = 2894406;
  numbers_[2017]["Embed-MuTau-F"] = 1900026;

  //2018
  numbers_[2018]["Embed-EMu-A"] = 12744399;
  numbers_[2018]["Embed-EMu-B"] = 6603834 ;
  numbers_[2018]["Embed-EMu-C"] = 5394411 ;
  numbers_[2018]["Embed-EMu-D"] = 6220115 ;

  numbers_[2018]["Embed-ETau-A"] = 10607520;
  numbers_[2018]["Embed-ETau-B"] = 4769019 ;
  numbers_[2018]["Embed-ETau-C"] = 4795833 ;
  numbers_[2018]["Embed-ETau-D"] = 10343384;

  numbers_[2018]["Embed-MuTau-A"] = 10902486;
  numbers_[2018]["Embed-MuTau-B"] = 5563063 ;
  numbers_[2018]["Embed-MuTau-C"] = 5408863 ;
  numbers_[2018]["Embed-MuTau-D"] = 9295906 ;

  //Signal gen numbers
  numbers_[2016]["ZEMu"    ] =  40000;
  numbers_[2017]["ZEMu"    ] =  80000;
  numbers_[2018]["ZEMu"    ] =  40000;
  numbers_[2016]["ZMuTau"  ] =  40000;
  numbers_[2017]["ZMuTau"  ] =  80000;
  numbers_[2018]["ZMuTau"  ] =  40000;
  numbers_[2016]["ZETau"   ] =  40000;
  numbers_[2017]["ZETau"   ] =  80000;
  numbers_[2018]["ZETau"   ] =  40000;
  numbers_[2016]["HEMu"    ] =  40000;
  numbers_[2017]["HEMu"    ] =  80000;
  numbers_[2018]["HEMu"    ] =  40000;
  numbers_[2016]["HMuTau"  ] =  40000;
  numbers_[2017]["HMuTau"  ] =  80000;
  numbers_[2018]["HMuTau"  ] =  40000;
  numbers_[2016]["HETau"   ] =  40000;
  numbers_[2017]["HETau"   ] =  80000;
  numbers_[2018]["HETau"   ] =  40000;

  //Luminosity by year
  lum_[2016] = 36.33e3;
  lum_[2017] = 41.48e3;
  lum_[2018] = 59.83e3;

}
