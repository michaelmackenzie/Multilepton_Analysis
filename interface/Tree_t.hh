#ifndef __TREE__HH
#define __TREE__HH

namespace CLFV {

  //Tree Variables
  struct Tree_t {
    //lepton variables
    float leponept;
    float leponeptoverm;
    float leponem;
    float leponeeta;
    float leponed0;
    float leponeiso;
    float leponereliso;
    float leponeidone;
    float leponeidtwo;
    float leponeidthree;
    float leptwopt;
    float leptwoptoverm;
    float leptwom;
    float leptwoeta;
    float leptwod0;
    float leptwoiso;
    float leptworeliso;
    float leptwoidone;
    float leptwoidtwo;
    float leptwoidthree;

    //di-lepton variables
    float lepp;
    float leppt;
    float lepptoverm;
    float lepm;
    float lepmt;
    float lepeta;
    float lepdeltaeta;
    float lepdeltar;
    float lepdeltaphi;

    //extra angles
    float htdeltaphi;
    float metdeltaphi;
    float leponedeltaphi;
    float leptwodeltaphi;
    float onemetdeltaphi;
    float twometdeltaphi;

    //MET variables
    float met;
    float mtone;
    float mttwo;
    float mtlep;
    float mtoneoverm;
    float mttwooverm;
    float pxivis;
    float pxiinv;
    float ptauvisfrac;
    float mestimate;
    float mestimatetwo;

    //transformed frame variables
    //index 0: include met, lep 2 = tau; index 1: include met, lep 1 = tau; index 2: no met, lep 2 = defines x axis and z direction
    float leponeprimepx[3];
    float leptwoprimepx[3];
    float metprimepx   [3];
    float leponeprimepy[3];
    float leptwoprimepy[3];
    float metprimepy   [3];
    float leponeprimepz[3];
    float leptwoprimepz[3];
    float metprimepz   [3];
    float leponeprimee [3];
    float leptwoprimee [3];
    float metprimee    [3];

    // float leponeprimepxt;
    // float leptwoprimepxt;
    // float metprimepxt   ;
    // float leponeprimepyt;
    // float leptwoprimepyt;
    // float metprimepyt   ;
    // float leponeprimepzt;
    // float leptwoprimepzt;
    // float metprimepzt   ;
    // float leponeprimeet ;
    // float leptwoprimeet ;
    // float metprimeet    ;

    float alphaz1;
    float alphah1;
    float alpha2;
    float alpha3;
    float deltaalphaz1;
    float deltaalphaz2;
    float deltaalphah1;
    float deltaalphah2;
    float deltaalpham1;
    float deltaalpham2;

    //Event variables
    float ht;
    float htsum;
    float jetpt;
    float njets;
    float nbjets; //pt > 30
    float nbjetsm;
    float nbjetsl;
    float nbjets20; //pt > 20
    float nbjets20m;
    float nbjets20l;
    float nphotons;
    float eventweight;
    float eventweightMVA; //for MVA categories/histograms, remove training samples
    float fulleventweight; //includes cross-section and number gen
    float fulleventweightlum; //includes cross-section, number gen, and luminosity
    float eventcategory; //for identifying the process in mva trainings
    float issignal;
    float category; //selection category e.g. mutau_h, mutau_e
    float type; //background type, signal, data, Z->ll, Z->tautau, Top, WJets, or Diboson

    //large systematics we may want to reduce impact of within TMVA training
    //these are fractions to multiply to the full event weight
    float jettotaunonclosure; //event weight change without non-closure
    float zptup; //event weight change using up for z pT weights
    float zptdown; //event weight change using down for z pT weights

    //identify whether or not to use in training
    float train; //  < 0 --> testing, > 0 --> training sample
  };
}
#endif
