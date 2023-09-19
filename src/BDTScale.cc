#include "interface/BDTScale.hh"
using namespace CLFV;

//----------------------------------------------------------------------------------------
float BDTScale::Weight(float score, int year, float& up, float& down) {
  up = 1.f; down = 1.f;
  if(!(year == 2016 || year == 2017 || year == 2018)) {
    printf("BDTScale::%s: Error! Unknown year %i\n", __func__, year);
    return 1.f;
  }

  //Check for an uninitialized score
  if(score < -1.f) return 1.f;

  float ele, ele_up, ele_down, mu, mu_up, mu_down;
  if(year == 2016) {
    ele      = electron_2016_weight     (score);
    ele_up   = electron_2016_weight_up  (score);
    ele_down = electron_2016_weight_down(score);
    mu       =     muon_2016_weight     (score);
    mu_up    =     muon_2016_weight_up  (score);
    mu_down  =     muon_2016_weight_down(score);
  } else if(year == 2017) {
    ele      = electron_2017_weight     (score);
    ele_up   = electron_2017_weight_up  (score);
    ele_down = electron_2017_weight_down(score);
    mu       =     muon_2017_weight     (score);
    mu_up    =     muon_2017_weight_up  (score);
    mu_down  =     muon_2017_weight_down(score);
  } else {
    ele      = electron_2018_weight     (score);
    ele_up   = electron_2018_weight_up  (score);
    ele_down = electron_2018_weight_down(score);
    mu       =     muon_2018_weight     (score);
    mu_up    =     muon_2018_weight_up  (score);
    mu_down  =     muon_2018_weight_down(score);
  }

  const float corr = (ele + mu) / 2.f;
  up   = std::max(ele_up  , mu_up  );
  down = std::min(ele_down, mu_down);
  return corr;
}

//----------------------------------------------------------------------------------------
float BDTScale::muon_2018_weight(float score){
  if      (score<0.30f) return 1.13040384656;
  else if (score<0.70f) return 0.997102114961;
  else if (score<0.90f) return 0.98064041578;
  else if (score<1.01f) return 1.02290654759;
  return 1.f;
}
float BDTScale::muon_2018_weight_up(float score){
  if      (score<0.30f) return 1.13630932776;
  else if (score<0.70f) return 0.999157302142;
  else if (score<0.90f) return 0.981714850038;
  else if (score<1.01f) return 1.02504324308;
  return 1.f;
}
float BDTScale::muon_2018_weight_down(float score){
  if      (score<0.30f) return 1.12449836535;
  else if (score<0.70f) return 0.995046927779;
  else if (score<0.90f) return 0.979565981522;
  else if (score<1.01f) return 1.02076985211;
  return 1.f;
}

//----------------------------------------------------------------------------------------
float BDTScale::muon_2016_weight(float score){
  if      (score<0.30f) return 0.98; //0.857319368881;
  else if (score<0.70f) return 0.975; //0.934838741361;
  else if (score<0.90f) return 0.995; //1.00941545225;
  else if (score<1.01f) return 1.03; //1.08027597966;
  return 1.f;
}
float BDTScale::muon_2016_weight_up(float score){
  if      (score<0.30f) return 0.862626829026;
  else if (score<0.70f) return 0.937149845238;
  else if (score<0.90f) return 1.01076565413;
  else if (score<1.01f) return 1.08304469131;
  return 1.f;
}
float BDTScale::muon_2016_weight_down(float score){
  if      (score<0.30f) return 0.852011908737;
  else if (score<0.70f) return 0.932527637484;
  else if (score<0.90f) return 1.00806525037;
  else if (score<1.01f) return 1.07750726801;
  return 1.f;
}

//----------------------------------------------------------------------------------------
float BDTScale::muon_2017_weight(float score){
  if      (score<0.30f) return 1.205; //0.966398656323;
  else if (score<0.70f) return 1.025; //0.957180214112;
  else if (score<0.90f) return 0.975; //1.00179638752;
  else if (score<1.01f) return 0.97; //1.05152468485;
  return 1.f;
}
float BDTScale::muon_2017_weight_up(float score){
  if      (score<0.30f) return 0.971449773104;
  else if (score<0.70f) return 0.959204181569;
  else if (score<0.90f) return 1.00295995185;
  else if (score<1.01f) return 1.05388359188;
  return 1.f;
}
float BDTScale::muon_2017_weight_down(float score){
  if      (score<0.30f) return 0.961347539542;
  else if (score<0.70f) return 0.955156246655;
  else if (score<0.90f) return 1.00063282318;
  else if (score<1.01f) return 1.04916577783;
  return 1.f;
}


//----------------------------------------------------------------------------------------
float BDTScale::electron_2018_weight(float score){
  if      (score<0.30f) return 1.12791280956;
  else if (score<0.70f) return 1.00460215341;
  else if (score<0.90f) return 0.983970488752;
  else if (score<1.01f) return 1.00556877834;
  return 1.f;
}
float BDTScale::electron_2018_weight_up(float score){
  if      (score<0.30f) return 1.13378632375;
  else if (score<0.70f) return 1.00671949526;
  else if (score<0.90f) return 0.985110452893;
  else if (score<1.01f) return 1.0079748054;
  return 1.f;
}
float BDTScale::electron_2018_weight_down(float score){
  if      (score<0.30f) return 1.12203929538;
  else if (score<0.70f) return 1.00248481157;
  else if (score<0.90f) return 0.982830524612;
  else if (score<1.01f) return 1.00316275127;
  return 1.f;
}


//----------------------------------------------------------------------------------------
float BDTScale::electron_2016_weight(float score){
  if      (score<0.30f) return 1.03; //0.870700870122;
  else if (score<0.70f) return 0.99; //0.938575286732;
  else if (score<0.90f) return 0.995; //1.01425625952;
  else if (score<1.01f) return 1.01; //1.06927338134;
  return 1.f;
}
float BDTScale::electron_2016_weight_up(float score){
  if      (score<0.30f) return 0.876233508214;
  else if (score<0.70f) return 0.94097108298;
  else if (score<0.90f) return 1.01567282937;
  else if (score<1.01f) return 1.07225261139;
  return 1.f;
}
float BDTScale::electron_2016_weight_down(float score){
  if      (score<0.30f) return 0.86516823203;
  else if (score<0.70f) return 0.936179490484;
  else if (score<0.90f) return 1.01283968968;
  else if (score<1.01f) return 1.0662941513;
  return 1.f;
}

//----------------------------------------------------------------------------------------
float BDTScale::electron_2017_weight(float score){
  if      (score<0.30f) return 1.19; //0.986164562137;
  else if (score<0.70f) return 1.02; //0.965465700814;
  else if (score<0.90f) return 0.975; //1.00300052806;
  else if (score<1.01f) return 0.96; //1.04430057854;
  return 1.f;
}
float BDTScale::electron_2017_weight_up(float score){
  if      (score<0.30f) return 0.991794473754;
  else if (score<0.70f) return 0.967750121622;
  else if (score<0.90f) return 1.00435943446;
  else if (score<1.01f) return 1.04729427933;
  return 1.f;
}
float BDTScale::electron_2017_weight_down(float score){
  if      (score<0.30f) return 0.980534650519;
  else if (score<0.70f) return 0.963181280006;
  else if (score<0.90f) return 1.00164162166;
  else if (score<1.01f) return 1.04130687776;
  return 1.f;
}
