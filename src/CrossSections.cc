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

  values_["Embed-EE-B"][2016]  = 1.0280;
  values_["Embed-EE-C"][2016]  = 1.0329;
  values_["Embed-EE-D"][2016]  = 1.0198;
  values_["Embed-EE-E"][2016]  = 1.0517;
  values_["Embed-EE-F"][2016]  = 1.0390;
  values_["Embed-EE-G"][2016]  = 1.1005;
  values_["Embed-EE-H"][2016]  = 1.0498;

  // *** Total 2016 ElMu : Total Mini / Nano = 29697000/27318414 = 1.0870
  // *** Total 2016 ElTau: Total Mini / Nano = 26477000/24049075 = 1.1009
  // *** Total 2016 MuTau: Total Mini / Nano = 26367000/23397800 = 1.1269
  // *** Total 2016 MuMu : Total Mini / Nano = 38448000/36381476 = 1.0568
  // *** Total 2016 ElEl : Total Mini / Nano = 34637000/32933414 = 1.0517

  //2017
  values_["Embed-EMu-B"][2017] =  4.7939 /  4.6903; //current: V3 lumi; V2 N(events): 1.0269; //V1: 2.0244;
  values_["Embed-EMu-C"][2017] =  9.6312 /  7.5853; //current: V3 lumi; V2 N(events): 1.7036; //V1: 4.5268;
  values_["Embed-EMu-D"][2017] =  4.2477 /  4.1056; //current: V3 lumi; V2 N(events): 1.3926; //V1: 1.1387;
  values_["Embed-EMu-E"][2017] =  9.3133 /  9.1364; //current: V3 lumi; V2 N(events): 2.9324; //V1: 2.6201;
  values_["Embed-EMu-F"][2017] = 13.5390 / 11.6842; //current: V3 lumi; V2 N(events): 2.1986; //V1: 6.0306;

  values_["Embed-ETau-B"][2017] =  4.7939 /  4.5193; //current: V3 lumi; V2 N(events): 1.0761; //V1: 2.4139;
  values_["Embed-ETau-C"][2017] =  9.6312 /  8.9456; //current: V3 lumi; V2 N(events): 1.5662; //V1: 3.2928;
  values_["Embed-ETau-D"][2017] =  4.2477 /  4.1807; //current: V3 lumi; V2 N(events): 1.1924; //V1: 1.0589;
  values_["Embed-ETau-E"][2017] =  9.3136 /  9.2119; //current: V3 lumi; V2 N(events): 2.1550; //V1: 2.4626;
  values_["Embed-ETau-F"][2017] = 13.5390 / 12.7366; //current: V3 lumi; V2 N(events): 1.7933; //V1: 4.1141;

  values_["Embed-MuTau-B"][2017] =  4.7939 /  4.7175; //current: V3 lumi; V2 N(events): 1.0210; //V1: 2.2020;
  values_["Embed-MuTau-C"][2017] =  9.6312 /  7.9460; //current: V3 lumi; V2 N(events): 1.8336; //V1: 5.0035;
  values_["Embed-MuTau-D"][2017] =  4.2477 /  4.1262; //current: V3 lumi; V2 N(events): 1.1342; //V1: 1.1480;
  values_["Embed-MuTau-E"][2017] =  9.3136 /  9.1610; //current: V3 lumi; V2 N(events): 2.0557; //V1: 2.7259;
  values_["Embed-MuTau-F"][2017] = 13.5390 / 11.9807; //current: V3 lumi; V2 N(events): 1.5287; //V1: 5.6159;

  values_["Embed-MuMu-B"][2017]  =  4.7939 /  2.7802; //current: V3 lumi; V2 N(events): 1.9039; //V1: 1.9039;
  values_["Embed-MuMu-C"][2017]  =  9.6312 /  6.1608; //current: V3 lumi; V2 N(events): 1.5831; //V1: 3.2283;
  values_["Embed-MuMu-D"][2017]  =  4.2477 /  3.6894; //current: V3 lumi; V2 N(events): 1.1873; //V1: 1.3284;
  values_["Embed-MuMu-E"][2017]  =  9.3136 /  9.0347; //current: V3 lumi; V2 N(events): 1.3037; //V1: 2.8647;
  values_["Embed-MuMu-F"][2017]  = 13.5375 /  3.8110; //current: V3 lumi; V2 N(events): 6.8001; //V1: 6.8001;

  values_["Embed-EE-B"][2017]  =  4.7939 /  4.4775; //current: V3 lumi; V2 N(events): 1.0799; //V1: 1.5825;
  values_["Embed-EE-C"][2017]  =  9.6307 /  7.6495; //current: V3 lumi; V2 N(events): 1.2635; //V1: 2.7471;
  values_["Embed-EE-D"][2017]  =  4.2477 /  4.0178; //current: V3 lumi; V2 N(events): 1.1826; //V1: 1.1859;
  values_["Embed-EE-E"][2017]  =  9.3136 /  9.1016; //current: V3 lumi; V2 N(events): 1.3515; //V1: 2.5760;
  values_["Embed-EE-F"][2017]  = 13.5382 / 10.9579; //current: V3 lumi; V2 N(events): 2.4550; //V1: 5.4018;

  // V1:
  // *** Total 2017 ElMu  Mini / Nano = 37713920/12833285 = 2.9387
  // *** Total 2017 ElTau Mini / Nano = 32788982/12741595 = 2.5733
  // *** Total 2017 MuTau Mini / Nano = 34683979/11470240 = 3.0238
  // *** Total 2017 MuMu  Mini / Nano = 64519124/21647343 = 2.9804
  // *** Total 2017 ElEl  Mini / Nano = 45398261/17827258 = 2.5465

  // V2:
  // Run 2017B Final State ElMu: Mini / Nano =  4074443/3967571 = 1.0269
  // Run 2017C Final State ElMu: Mini / Nano =  9364234/5496437 = 1.7036
  // Run 2017D Final State ElMu: Mini / Nano =  4023470/2889168 = 1.3926 //V1 is higher
  // Run 2017E Final State ElMu: Mini / Nano =  8620632/2939770 = 2.9324 //V1 is higher
  // Run 2017F Final State ElMu: Mini / Nano = 11631141/5290113 = 2.1986
  // --- Total 2017 ElMu Mini / Nano = 37713920/20583059 = 1.8322
  // Run 2017B Final State ElTau: Mini / Nano =  3699519/3437675 = 1.0761
  // Run 2017C Final State ElTau: Mini / Nano =  8093814/5167654 = 1.5662
  // Run 2017D Final State ElTau: Mini / Nano =  3471384/2911094 = 1.1924 //V1 is higher
  // Run 2017E Final State ElTau: Mini / Nano =  7444593/3454565 = 2.1550
  // Run 2017F Final State ElTau: Mini / Nano = 10079672/5620450 = 1.7933
  // --- Total 2017 ElTau Mini / Nano = 32788982/20591438 = 1.5923
  // Run 2017B Final State MuTau: Mini / Nano =  3865704/3786021 = 1.0210
  // Run 2017C Final State MuTau: Mini / Nano =  8577076/4677691 = 1.8336
  // Run 2017D Final State MuTau: Mini / Nano =  3680715/3245024 = 1.1342
  // Run 2017E Final State MuTau: Mini / Nano =  7890071/3838042 = 2.0557
  // Run 2017F Final State MuTau: Mini / Nano = 10670413/6979700 = 1.5287
  // --- Total 2017 MuTau Mini / Nano = 34683979/22526478 = 1.5396
  // Run 2017B Final State MuMu: Mini / Nano =  5582581/ 2932147 = 1.9039
  // Run 2017C Final State MuMu: Mini / Nano = 16438354/10383013 = 1.5831
  // Run 2017D Final State MuMu: Mini / Nano =  7116260/ 5993575 = 1.1873
  // Run 2017E Final State MuMu: Mini / Nano = 15164855/11631502 = 1.3037
  // Run 2017F Final State MuMu: Mini / Nano = 20217074/ 2973048 = 6.8001
  // --- Total 2017 MuMu Mini / Nano = 64519124/33913285 = 1.9024
  // Run 2017B Final State ElEl: Mini / Nano =  4547506/4210771 = 1.0799
  // Run 2017C Final State ElEl: Mini / Nano = 11379296/9005580 = 1.2635
  // Run 2017D Final State ElEl: Mini / Nano =  4902310/4145049 = 1.1826
  // Run 2017E Final State ElEl: Mini / Nano = 10486226/7758876 = 1.3515
  // Run 2017F Final State ElEl: Mini / Nano = 14082923/5736202 = 2.4550
  // --- Total 2017 ElEl Mini / Nano = 45398261/30856478 = 1.4712

  //2018
  values_["Embed-EMu-A"][2018] = 1.0721; //V1: 1.0832; //V1 lumi: 1.0667;
  values_["Embed-EMu-B"][2018] = 1.0746; //V1: 1.0118; //V1 lumi: 1.0112;
  values_["Embed-EMu-C"][2018] = 1.0965; //V1: 1.1415; //V1 lumi: 1.1261;
  values_["Embed-EMu-D"][2018] = 1.2694; //V1: 4.6364; //V1 lumi: 3.9906;

  values_["Embed-ETau-A"][2018] = 1.0253; //V1: 1.1062;
  values_["Embed-ETau-B"][2018] = 1.0936; //V1: 1.2357;
  values_["Embed-ETau-C"][2018] = 1.0488; //V1: 1.1306;
  values_["Embed-ETau-D"][2018] = 1.2458; //V1: 2.4541;

  values_["Embed-MuTau-A"][2018] = 1.0608; //V1: 1.0764;
  values_["Embed-MuTau-B"][2018] = 1.0862; //V1: 1.0591;
  values_["Embed-MuTau-C"][2018] = 1.0572; //V1: 1.0022;
  values_["Embed-MuTau-D"][2018] = 1.2727; //V1: 2.7302;

  values_["Embed-MuMu-A"][2018]  = 1.0645; //V1: 1.1174;
  values_["Embed-MuMu-B"][2018]  = 1.0655; //V1: 1.2995;
  values_["Embed-MuMu-C"][2018]  = 1.0580; //V1: 1.0580;
  values_["Embed-MuMu-D"][2018]  = 1.3875; //V1: 7.5245;

  values_["Embed-EE-A"][2018]  = 1.0555; //V1: 1.2749;
  values_["Embed-EE-B"][2018]  = 1.0752; //V1: 1.0317;
  values_["Embed-EE-C"][2018]  = 1.0595; //V1: 1.0161;
  values_["Embed-EE-D"][2018]  = 1.4193; //V1: 2.8777;

  // V1:
  // --- Total 2018 ElMu  Mini / Nano = 55484067/30962759 = 1.7919; lumi ratio = 1.7473
  // --- Total 2018 ElTau Mini / Nano = 48434386/30515756 = 1.5871
  // --- Total 2018 MuTau Mini / Nano = 48428591/31170318 = 1.5536
  // --- Total 2018 MuMu  Mini / Nano = 78733782/39175846 = 2.0097
  // --- Total 2018 ElEl  Mini / Nano = 59254929/35680605 = 1.6594

  // V2/V3 numbers as of 10/20/22
  // Run 2018A Final State ElMu: Mini / Nano = 13805200/12876101 = 1.0721
  // Run 2018B Final State ElMu: Mini / Nano = 6681860/6217664 = 1.0746
  // Run 2018C Final State ElMu: Mini / Nano = 6157928/5615620 = 1.0965
  // Run 2018D Final State ElMu: Mini / Nano = 28839079/22717555 = 1.2694
  // --- Total 2018 ElMu Mini / Nano = 55484067/47426940 = 1.1698
  // Run 2018A Final State ElTau: Mini / Nano = 11735088/11444847 = 1.0253
  // Run 2018B Final State ElTau: Mini / Nano = 5893097/5388352 = 1.0936
  // Run 2018C Final State ElTau: Mini / Nano = 5422423/5170023 = 1.0488
  // Run 2018D Final State ElTau: Mini / Nano = 25383778/20374938 = 1.2458
  // --- Total 2018 ElTau Mini / Nano = 48434386/42378160 = 1.1429
  // Run 2018A Final State MuTau: Mini / Nano = 11735808/11062612 = 1.0608
  // Run 2018B Final State MuTau: Mini / Nano = 5892212/5424379 = 1.0862
  // Run 2018C Final State MuTau: Mini / Nano = 5420887/5127190 = 1.0572
  // Run 2018D Final State MuTau: Mini / Nano = 25379684/19940861 = 1.2727
  // --- Total 2018 MuTau Mini / Nano = 48428591/41555042 = 1.1654
  // Run 2018A Final State MuMu: Mini / Nano = 20773432/19513759 = 1.0645
  // Run 2018B Final State MuMu: Mini / Nano = 9306339/8733948 = 1.0655
  // Run 2018C Final State MuMu: Mini / Nano = 8567883/8097542 = 1.0580
  // Run 2018D Final State MuMu: Mini / Nano = 40086128/28889303 = 1.3875
  // --- Total 2018 MuMu Mini / Nano = 78733782/65234552 = 1.2069
  // Run 2018A Final State ElEl: Mini / Nano = 14964725/14176978 = 1.0555
  // Run 2018B Final State ElEl: Mini / Nano = 7100628/6603513 = 1.0752
  // Run 2018C Final State ElEl: Mini / Nano = 6542211/6174469 = 1.0595
  // Run 2018D Final State ElEl: Mini / Nano = 30647365/21593013 = 1.4193
  // --- Total 2018 ElEl Mini / Nano = 59254929/48547973 = 1.2205

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

  neg_frac_["DY50-ext"           ][2017] = 0.1611 ;

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

  /////////////////////////////////////////////////
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

  numbers_[2016]["Embed-MuMu-B"] = 5705922;
  numbers_[2016]["Embed-MuMu-C"] = 2580632;
  numbers_[2016]["Embed-MuMu-D"] = 4225463;
  numbers_[2016]["Embed-MuMu-E"] = 3749169;
  numbers_[2016]["Embed-MuMu-F"] = 2650658;
  numbers_[2016]["Embed-MuMu-G"] = 8068896;
  numbers_[2016]["Embed-MuMu-H"] = 9400736;

  numbers_[2016]["Embed-EE-B"] = 5348889;
  numbers_[2016]["Embed-EE-C"] = 2300273;
  numbers_[2016]["Embed-EE-D"] = 3804598;
  numbers_[2016]["Embed-EE-E"] = 3396152;
  numbers_[2016]["Embed-EE-F"] = 2580135;
  numbers_[2016]["Embed-EE-G"] = 6961897;
  numbers_[2016]["Embed-EE-H"] = 8541470;

  //2017
  numbers_[2017]["Embed-EMu-B"] = 3967571; //V2: 3967571; //V1: 2012635;
  numbers_[2017]["Embed-EMu-C"] = 6353673; //V2: 5496437; //V1: 2068587;
  numbers_[2017]["Embed-EMu-D"] = 3633999; //V2: 2889168; //V1: 3533197;
  numbers_[2017]["Embed-EMu-E"] = 7931888; //V2: 2939770; //V1: 3290180;
  numbers_[2017]["Embed-EMu-F"] = 7621097; //V2: 5290113; //V1: 1928686;

  numbers_[2017]["Embed-ETau-B"] = 3437675; //V2: 3437675; //V1: 1532529;
  numbers_[2017]["Embed-ETau-C"] = 6797815; //V2: 5167654; //V1: 2457960;
  numbers_[2017]["Embed-ETau-D"] = 3258607; //V2: 2911094; //V1: 3278083;
  numbers_[2017]["Embed-ETau-E"] = 6992531; //V2: 3454565; //V1: 3023027;
  numbers_[2017]["Embed-ETau-F"] = 7986753; //V2: 5620450; //V1: 2449996;

  numbers_[2017]["Embed-MuTau-B"] = 3786021; //V2: 3786021; //V1: 1755514;
  numbers_[2017]["Embed-MuTau-C"] = 6168667; //V2: 4677691; //V1: 1714208;
  numbers_[2017]["Embed-MuTau-D"] = 3364291; //V2: 3245024; //V1: 3206086;
  numbers_[2017]["Embed-MuTau-E"] = 7307243; //V2: 3838042; //V1: 2894406;
  numbers_[2017]["Embed-MuTau-F"] = 7352399; //V2: 6979700; //V1: 1900026;

  numbers_[2017]["Embed-MuMu-B"] =  2932147; //V2:  2932147; //V1: 2932147;
  numbers_[2017]["Embed-MuMu-C"] = 10383013; //V2: 10383013; //V1: 5091845;
  numbers_[2017]["Embed-MuMu-D"] =  5993575; //V2:  5993575; //V1: 5356676;
  numbers_[2017]["Embed-MuMu-E"] = 13513003; //V2: 11631502; //V1: 5293627;
  numbers_[2017]["Embed-MuMu-F"] =  2973048; //V2:  2973048; //V1: 2973048;

  numbers_[2017]["Embed-EE-B"] = 4210771; //V2: 4210771; //V1: 2873517;
  numbers_[2017]["Embed-EE-C"] = 9005580; //V2: 9005580; //V1: 4142193;
  numbers_[2017]["Embed-EE-D"] = 4246906; //V2: 4145049; //V1: 4133793;
  numbers_[2017]["Embed-EE-E"] = 9531727; //V2: 7758876; //V1: 4070691;
  numbers_[2017]["Embed-EE-F"] = 8148666; //V2: 5736202; //V1: 2607064;

  //2018
  numbers_[2018]["Embed-EMu-A"] = 12876101; //V1: 12744399;
  numbers_[2018]["Embed-EMu-B"] =  6217664; //V1: 6603834 ;
  numbers_[2018]["Embed-EMu-C"] =  5615620; //V1: 5394411 ;
  numbers_[2018]["Embed-EMu-D"] = 22717555; //V1: 6220115 ;

  numbers_[2018]["Embed-ETau-A"] = 11444847; //V1: 10607520;
  numbers_[2018]["Embed-ETau-B"] =  5388352; //V1: 4769019 ;
  numbers_[2018]["Embed-ETau-C"] =  5170023; //V1: 4795833 ;
  numbers_[2018]["Embed-ETau-D"] = 20374938; //V1: 10343384;

  numbers_[2018]["Embed-MuTau-A"] = 11062612; //V1: 10902486;
  numbers_[2018]["Embed-MuTau-B"] =  5424379; //V1: 5563063 ;
  numbers_[2018]["Embed-MuTau-C"] =  5127190; //V1: 5408863 ;
  numbers_[2018]["Embed-MuTau-D"] = 19940861; //V1: 9295906 ;

  numbers_[2018]["Embed-MuMu-A"] = 19513759; //V1: 18589871;
  numbers_[2018]["Embed-MuMu-B"] =  8733948; //V1: 7161023 ;
  numbers_[2018]["Embed-MuMu-C"] =  8097542; //V1: 8097542 ;
  numbers_[2018]["Embed-MuMu-D"] = 28889303; //V1: 5327410 ;

  numbers_[2018]["Embed-EE-A"] = 14176978; //V1: 11737292;
  numbers_[2018]["Embed-EE-B"] =  6603513; //V1: 6881988 ;
  numbers_[2018]["Embed-EE-C"] =  6174469; //V1: 6438308 ;
  numbers_[2018]["Embed-EE-D"] = 21593013; //V1: 10649806;

  //Signal gen numbers
  numbers_[2016]["ZEMu"     ] =   40000;
  numbers_[2017]["ZEMu"     ] =   80000;
  numbers_[2018]["ZEMu"     ] =   40000;
  numbers_[2016]["ZMuTau"   ] =   40000;
  numbers_[2017]["ZMuTau"   ] =   80000;
  numbers_[2018]["ZMuTau"   ] =   40000;
  numbers_[2016]["ZETau"    ] =   40000;
  numbers_[2017]["ZETau"    ] =   80000;
  numbers_[2018]["ZETau"    ] =   40000;
  numbers_[2016]["ZEMu-v2"  ] =  180400;
  numbers_[2016]["ZETau-v2" ] =  163000;
  numbers_[2016]["ZMuTau-v2"] =  178800;

  numbers_[2016]["HEMu"     ] =   40000;
  numbers_[2017]["HEMu"     ] =   80000;
  numbers_[2018]["HEMu"     ] =   40000;
  numbers_[2016]["HMuTau"   ] =   40000;
  numbers_[2017]["HMuTau"   ] =   80000;
  numbers_[2018]["HMuTau"   ] =   40000;
  numbers_[2016]["HETau"    ] =   40000;
  numbers_[2017]["HETau"    ] =   80000;
  numbers_[2018]["HETau"    ] =   40000;

  //Luminosity by year
  lum_[2016] = 36.33e3;
  lum_[2017] = 41.48e3;
  lum_[2018] = 59.83e3;

}
