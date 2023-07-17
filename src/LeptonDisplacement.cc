#include "interface/LeptonDisplacement.hh"

using namespace CLFV;

//-------------------------------------------------------------------------------------------------
float LeptonDisplacement::Weight(float pt, float eta, bool is_muon, int year) {

  if(is_muon) {
    if     (year == 2016) return muon_2016_weight(pt, eta);
    else if(year == 2017) return muon_2017_weight(pt, eta);
    else if(year == 2018) return muon_2018_weight(pt, eta);
  } else {
    if     (year == 2016) return ele_2016_weight(pt, eta);
    else if(year == 2017) return ele_2017_weight(pt, eta);
    else if(year == 2018) return ele_2018_weight(pt, eta);
  }
  return 1.f;
}

//-------------------------------------------------------------------------------------------------
float LeptonDisplacement::muon_2016_weight(float pt, float eta) {
  eta = std::fabs(eta);
  if (pt<20) {
    if      (eta<0.9) return 1.007245952381961;
    else if (eta<1.2) return 1.0152752105344538;
    else if (eta<2.1) return 1.0429056578563722;
    else if (eta<2.4) return 1.0238905395413864;
  }
  else if (pt<25) {
    if      (eta<0.9) return 1.007013801658364;
    else if (eta<1.2) return 1.0144905550703371;
    else if (eta<2.1) return 1.0405706042685858;
    else if (eta<2.4) return 1.0357401089328553;
  }
  else if (pt<30) {
    if      (eta<0.9) return 1.0057256997834059;
    else if (eta<1.2) return 1.0126138932132556;
    else if (eta<2.1) return 1.03858424020429;
    else if (eta<2.4) return 1.0297552743540397;
  }
  else if (pt<40) {
    if      (eta<0.9) return 1.0046444796860163;
    else if (eta<1.2) return 1.0105316958554562;
    else if (eta<2.1) return 1.0354004091727864;
    else if (eta<2.4) return 1.0281195947684851;
  }
  else if (pt<50) {
    if      (eta<0.9) return 1.004185605349551;
    else if (eta<1.2) return 1.0102819039334332;
    else if (eta<2.1) return 1.0329790724374552;
    else if (eta<2.4) return 1.0279185013757524;
  }
  else if (pt<60) {
    if      (eta<0.9) return 1.006450877291955;
    else if (eta<1.2) return 1.0103621095587967;
    else if (eta<2.1) return 1.041263061693557;
    else if (eta<2.4) return 1.0341848780776526;
  }
  else if (pt<120) {
    if      (eta<0.9) return 1.0061381774475169;
    else if (eta<1.2) return 1.0126886390773273;
    else if (eta<2.1) return 1.0416952046305792;
    else if (eta<2.4) return 1.047350133817064;
  }
  else{
    if      (eta<0.9) return 1.0061381774475169;
    else if (eta<1.2) return 1.0126886390773273;
    else if (eta<2.1) return 1.0416952046305792;
    else if (eta<2.4) return 1.047350133817064;
  }

  return 1.f;
}

//-------------------------------------------------------------------------------------------------
float LeptonDisplacement::muon_2017_weight(float pt, float eta) {
  eta = std::fabs(eta);
  if (pt<20) {
    if      (eta<0.9) return 1.0032744384716148;
    else if (eta<1.2) return 1.0176835256762577;
    else if (eta<2.1) return 1.0463521167703105;
    else if (eta<2.4) return 1.0511699550656486;
  }
  else if (pt<25) {
    if      (eta<0.9) return 1.0032471169862256;
    else if (eta<1.2) return 1.0172786606277622;
    else if (eta<2.1) return 1.0438445425612293;
    else if (eta<2.4) return 1.0458350649846448;
  }
  else if (pt<30) {
    if      (eta<0.9) return 1.003097223861614;
    else if (eta<1.2) return 1.015378843072007;
    else if (eta<2.1) return 1.040933382247251;
    else if (eta<2.4) return 1.0450302261303748;
  }
  else if (pt<40) {
    if      (eta<0.9) return 1.003025512115391;
    else if (eta<1.2) return 1.0130511324888254;
    else if (eta<2.1) return 1.0374673327011983;
    else if (eta<2.4) return 1.0467167372039234;
  }
  else if (pt<50) {
    if      (eta<0.9) return 1.0024302147634518;
    else if (eta<1.2) return 1.012957802220184;
    else if (eta<2.1) return 1.0350514278194936;
    else if (eta<2.4) return 1.046802609466245;
  }
  else if (pt<60) {
    if      (eta<0.9) return 1.0033029128083284;
    else if (eta<1.2) return 1.0149563165951727;
    else if (eta<2.1) return 1.0425012651692298;
    else if (eta<2.4) return 1.0591449195189644;
  }
  else if (pt<120) {
    if      (eta<0.9) return 1.003475656865238;
    else if (eta<1.2) return 1.015022472749192;
    else if (eta<2.1) return 1.0447813289763992;
    else if (eta<2.4) return 1.057274070421095;
  }
  else{
    if      (eta<0.9) return 1.003475656865238;
    else if (eta<1.2) return 1.015022472749192;
    else if (eta<2.1) return 1.0447813289763992;
    else if (eta<2.4) return 1.057274070421095;
  }

  return 1.f;
}

//-------------------------------------------------------------------------------------------------
float LeptonDisplacement::muon_2018_weight(float pt, float eta) {
  eta = std::fabs(eta);
  if (pt<20) {
    if      (eta<0.9) return 0.9867555209929925;
    else if (eta<1.2) return 0.9945124144934455;
    else if (eta<2.1) return 0.9764128295064733;
    else if (eta<2.4) return 0.9663003281573023;
  }
  else if (pt<25) {
    if      (eta<0.9) return 0.9888478706149042;
    else if (eta<1.2) return 0.9947982529243221;
    else if (eta<2.1) return 0.9776111204955913;
    else if (eta<2.4) return 0.9620656287202801;
  }
  else if (pt<30) {
    if      (eta<0.9) return 0.9909278337639718;
    else if (eta<1.2) return 0.9952062529915819;
    else if (eta<2.1) return 0.9789473515096732;
    else if (eta<2.4) return 0.9622569121327984;
  }
  else if (pt<40) {
    if      (eta<0.9) return 0.9930960617733771;
    else if (eta<1.2) return 0.9931222119960154;
    else if (eta<2.1) return 0.9794998724397164;
    else if (eta<2.4) return 0.9657282004656631;
  }
  else if (pt<50) {
    if      (eta<0.9) return 0.9944211156155504;
    else if (eta<1.2) return 0.9941746251717741;
    else if (eta<2.1) return 0.9815112152854633;
    else if (eta<2.4) return 0.9651603694706341;
  }
  else if (pt<60) {
    if      (eta<0.9) return 0.9944252462323832;
    else if (eta<1.2) return 0.9947057709493733;
    else if (eta<2.1) return 0.9843497713620966;
    else if (eta<2.4) return 0.9615976137858482;
  }
  else if (pt<120) {
    if      (eta<0.9) return 0.9932653494488309;
    else if (eta<1.2) return 0.9921069914989961;
    else if (eta<2.1) return 0.9822681161321692;
    else if (eta<2.4) return 0.9697324948819696;
  }
  else{
    if      (eta<0.9) return 0.9932653494488309;
    else if (eta<1.2) return 0.9921069914989961;
    else if (eta<2.1) return 0.9822681161321692;
    else if (eta<2.4) return 0.9697324948819696;
  }

  return 1.f;
}

//-------------------------------------------------------------------------------------------------
float LeptonDisplacement::ele_2016_weight(float pt, float eta) {
  eta = std::fabs(eta);
  if (pt<20.f) {
    if      (eta<0.9f) return 1.0012169538580753;
    else if (eta<1.2f) return 1.0072984708257093;
    else if (eta<2.1f) return 1.0158168584514358;
    else if (eta<2.4f) return 1.0205268982657425;
  }
  else if (pt<25.f) {
    if      (eta<0.9f) return 1.0015651746674599;
    else if (eta<1.2f) return 1.0080849356522397;
    else if (eta<2.1f) return 1.0203241060552166;
    else if (eta<2.4f) return 1.024180326307058;
  }
  else if (pt<30.f) {
    if      (eta<0.9f) return 1.002362732619455;
    else if (eta<1.2f) return 1.0068273405571717;
    else if (eta<2.1f) return 1.0225930289685148;
    else if (eta<2.4f) return 1.0296093062406535;
  }
  else if (pt<40.f) {
    if      (eta<0.9f) return 1.0026985804470119;
    else if (eta<1.2f) return 1.0065704988587074;
    else if (eta<2.1f) return 1.0200697982370464;
    else if (eta<2.4f) return 1.0266887502973603;
  }
  else if (pt<50.f) {
    if      (eta<0.9f) return 1.002322413055388;
    else if (eta<1.2f) return 1.0062760411082818;
    else if (eta<2.1f) return 1.01809497742362;
    else if (eta<2.4f) return 1.0221390468773424;
  }
  else if (pt<60.f) {
    if      (eta<0.9f) return 1.002402960024834;
    else if (eta<1.2f) return 1.0086562046117191;
    else if (eta<2.1f) return 1.0182356320144834;
    else if (eta<2.4f) return 1.02934577263736;
  }
  else if (pt<120.f) {
    if      (eta<0.9f) return 1.0045188700971632;
    else if (eta<1.2f) return 1.002390784570645;
    else if (eta<2.1f) return 1.0242679065072413;
    else if (eta<2.4f) return 1.040694663682647;
  }
  else{
    if      (eta<0.9f) return 1.0045188700971632;
    else if (eta<1.2f) return 1.002390784570645;
    else if (eta<2.1f) return 1.0242679065072413;
    else if (eta<2.4f) return 1.040694663682647;
  }

  return 1.f;
}

//-------------------------------------------------------------------------------------------------
float LeptonDisplacement::ele_2017_weight(float pt, float eta) {
  eta = std::fabs(eta);
  if (pt<20.f) {
    if      (eta<0.9f) return 0.9988893455602329;
    else if (eta<1.2f) return 1.0057040943250564;
    else if (eta<2.1f) return 1.056739080924623;
    else if (eta<2.4f) return 1.0995986653574221;
  }
  else if (pt<25.f) {
    if      (eta<0.9f) return 0.9987598491529857;
    else if (eta<1.2f) return 1.0054967926603338;
    else if (eta<2.1f) return 1.057336945798204;
    else if (eta<2.4f) return 1.0993939571662867;
  }
  else if (pt<30.f) {
    if      (eta<0.9f) return 0.9987455648891811;
    else if (eta<1.2f) return 1.0040498336887265;
    else if (eta<2.1f) return 1.0538364548658787;
    else if (eta<2.4f) return 1.098945657165436;
  }
  else if (pt<40.f) {
    if      (eta<0.9f) return 0.9981378600265592;
    else if (eta<1.2f) return 1.0039908734517373;
    else if (eta<2.1f) return 1.0484665069999657;
    else if (eta<2.4f) return 1.09299517192016;
  }
  else if (pt<50.f) {
    if      (eta<0.9f) return 0.9977861121360901;
    else if (eta<1.2f) return 1.0041548480661942;
    else if (eta<2.1f) return 1.045126749674646;
    else if (eta<2.4f) return 1.092457452205239;
  }
  else if (pt<60.f) {
    if      (eta<0.9f) return 1.0008205628394309;
    else if (eta<1.2f) return 1.0085077779843543;
    else if (eta<2.1f) return 1.0523484230901692;
    else if (eta<2.4f) return 1.115244950871352;
  }
  else if (pt<120.f) {
    if      (eta<0.9f) return 0.9995740958395252;
    else if (eta<1.2f) return 1.006960503942512;
    else if (eta<2.1f) return 1.0507049008984217;
    else if (eta<2.4f) return 1.0957501154571723;
  }
  else{
    if      (eta<0.9f) return 0.9995740958395252;
    else if (eta<1.2f) return 1.006960503942512;
    else if (eta<2.1f) return 1.0507049008984217;
    else if (eta<2.4f) return 1.0957501154571723;
  }

  return 1.f;
}

//-------------------------------------------------------------------------------------------------
float LeptonDisplacement::ele_2018_weight(float pt, float eta) {
  eta = std::fabs(eta);
  if (pt<20) {
    if      (eta<0.9) return 0.980167441320488;
    else if (eta<1.2) return 0.9866995291570142;
    else if (eta<2.1) return 0.9464680016430284;
    else if (eta<2.4) return 0.951225244645303;
  }
  else if (pt<25) {
    if      (eta<0.9) return 0.981563884568672;
    else if (eta<1.2) return 0.9798763695187448;
    else if (eta<2.1) return 0.9588351490872673;
    else if (eta<2.4) return 0.9499269229246946;
  }
  else if (pt<30) {
    if      (eta<0.9) return 0.9836047599205916;
    else if (eta<1.2) return 0.9838932864665453;
    else if (eta<2.1) return 0.9582082445846748;
    else if (eta<2.4) return 0.9291029889307483;
  }
  else if (pt<40) {
    if      (eta<0.9) return 0.9833700072639457;
    else if (eta<1.2) return 0.9841071647187609;
    else if (eta<2.1) return 0.9555196587714792;
    else if (eta<2.4) return 0.9092451784416635;
  }
  else if (pt<50) {
    if      (eta<0.9) return 0.9826268435541179;
    else if (eta<1.2) return 0.9821148660266474;
    else if (eta<2.1) return 0.9579968058802665;
    else if (eta<2.4) return 0.905929267443813;
  }
  else if (pt<60) {
    if      (eta<0.9) return 0.981206489642565;
    else if (eta<1.2) return 0.9693216476210899;
    else if (eta<2.1) return 0.9509366500608969;
    else if (eta<2.4) return 0.9633576916788759;
  }
  else if (pt<120) {
    if      (eta<0.9) return 0.9836407390679724;
    else if (eta<1.2) return 0.9818525618081972;
    else if (eta<2.1) return 0.951617947886801;
    else if (eta<2.4) return 0.901331581677272;
  }
  else{
    if      (eta<0.9) return 0.9836407390679724;
    else if (eta<1.2) return 0.9818525618081972;
    else if (eta<2.1) return 0.951617947886801;
    else if (eta<2.4) return 0.901331581677272;
  }

  return 1.f;
}
