#ifndef __TREE__HH
#define __TREE__HH

//Tree Variables
struct Tree_t {
  //lepton variables
  float leponept;
  float leponeptoverm;
  float leponem;
  float leponeeta;
  float leponed0;
  float leponeiso;
  float leponeidone;
  float leponeidtwo;
  float leponeidthree;
  float leptwopt;
  float leptwoptoverm;
  float leptwom;
  float leptwoeta;
  float leptwod0;
  float leptwoiso;
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
  
  //identify to use in training
  float train; //  < 0 --> testing, > 0 --> training sample
};
#endif
