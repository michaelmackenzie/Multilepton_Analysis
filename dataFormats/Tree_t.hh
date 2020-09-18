#ifndef __TREE__HH
#define __TREE__HH

//Tree Variables
struct Tree_t {
  //lepton variables
  float leponept;
  float leponem;
  float leponeeta;
  float leponed0;
  float leponeiso;
  float leponeidone;
  float leponeidtwo;
  float leponeidthree;
  float leptwopt;
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
  float lepm;
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
  float pxivis;
  float pxiinv;
  float ptauvisfrac;
  float mestimate;
  float mestimatetwo;
    
  //Event variables
  float ht;
  float htsum;
  float jetpt;
  float njets;
  float nbjets; //pt > 30
  float nbjetsm;
  float nbjetsl;
  float nbjetstot25; //pt > 25
  float nbjetstot25m;
  float nbjetstot25l;
  float nbjetstot20; //pt > 20
  float nbjetstot20m;
  float nbjetstot20l;
  float nphotons;
  float eventweight;
  float fulleventweight; //includes cross-section and number gen
  float eventcategory; //for identifying the process in mva trainings

  //identify to use in training
  float train; //  < 0 --> testing, > 0 --> training sample
};
#endif
