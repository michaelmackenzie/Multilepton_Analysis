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

    //associated track variables (for tau info)
    float trkpt;
    float trketa;
    float trkphi;
    float leptrkm;
    float leptrkdeltam;
    float trkptoverpt;

    //tau info
    float taudecaymode;
    float taugenflavor;

    //di-lepton variables
    float lepp;
    float leppt;
    float lepptoverm;
    float lepm;
    float lepmt;
    float lepeta;
    float lepphi;
    float lepdeltaeta;
    float lepdeltar;
    float lepdeltaphi;
    float ptdiff;
    float ptdiffoverm;
    float ptratio;
    float pttrailoverlead;

    //extra angles
    float htdeltaphi;
    float metdeltaphi;
    float leponedeltaphi;
    float leptwodeltaphi;
    float onemetdeltaphi;
    float twometdeltaphi;

    //MET variables
    float met;
    float metsignificance;
    float mtone;
    float mttwo;
    float mtlead;
    float mttrail;
    float mtdiff;
    float mtlep;
    float mtoneoverm;
    float mttwooverm;
    float mtlepoverm;
    float pzetavis; //(l1 + l2) dot zeta
    float pzetainv; //(MET) dot zeta
    float pzetaall; //(l1 + l2 + MET) dot zeta
    float dzeta;
    float ptauvisfrac;
    float mestimate;
    float mestimatetwo;
    float mestimatethree;
    float mbalance;
    float mbalancetwo;
    float mestimatefour;
    float mestimate_cut_1;
    float mestimate_cut_2;
    float mestimate_avg_1;
    float mestimate_avg_2;
    float mestimateoverm;
    float mestimatetwooverm;

    //neutrino variables
    float p_nu_col_1;
    float p_nu_col_2;
    float p_nu_bal_1;
    float p_nu_bal_2;
    float p_nu_avg_1;
    float p_nu_avg_2;

    float metdotl1;
    float metdotl2;
    float met_u1;
    float met_u2;

    //MET along gen-level Z info
    float metdotzl1;
    float metdotzl2;

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
    float beta1;
    float beta2;
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
    float nbjets;
    float nphotons;
    float eventweight;
    float eventweightMVA; //for MVA categories/histograms, remove training samples
    float fulleventweight; //includes cross-section and number gen
    float fulleventweightlum; //includes cross-section, number gen, and luminosity
    float trainfraction; //to properly normalize events when comparing testing/training
    float eventcategory; //for identifying the process in mva trainings
    float issignal;
    float category; //selection category e.g. mutau_h, mutau_e
    float type; //background type, signal, data, Z->ll, Z->tautau, Top, WJets, or Diboson
    float year;

    int   zdecaymode; //unknown; 1: ee; 2: mumu; 3: tau_e tau_e; 4: tau_mu tau_mu; 5: tau_e tau_mu; 6: tau_e tau_h; 7: tau_mu tau_h; 8: tau_h tau_h

    //large systematics we may want to reduce impact of within TMVA training
    //these are fractions to multiply to the full event weight
    float jettotaustatup  [4]; //event weight varying j-->tau statistical uncertainty
    float jettotaustatdown[4];
    float jettotaunoncl   [4]; //event weight change without non-closure
    float jettotaubias    [4]; //event weight change without bias
    float jettotaucomp       ; //event weight change with composition variation
    float zptsys; //event weight change using up for z pT weights
    float jetantimusys;
    float jetantielesys;
    float qcdstat; //QCD SS --> OS systematics
    float qcdnc;
    float qcdbias;
    float btagsys; //b-tag ID
    float lumisys; //luminosity

    float leponeid1sys;
    float leponeid2sys;
    float leptwoid1sys;
    float leptwoid2sys;
    float leponeid1bin;
    float leptwoid1bin;

    //identify whether or not to use in training
    float train; //  < 0 --> testing, > 0 --> training sample
  };
}
#endif
