float SFBDT_weight_Zmumu(float bdt){
if (bdt<0.3) return 0.857319368881;
else if (bdt<0.7) return 0.934838741361;
else if (bdt<0.9) return 1.00941545225;
else if (bdt<1.01) return 1.08027597966;
else return 1;
}
float SFBDT_weight_Zee(float bdt){
if (bdt<0.3) return 0.870700870122;
else if (bdt<0.7) return 0.938575286732;
else if (bdt<0.9) return 1.01425625952;
else if (bdt<1.01) return 1.06927338134;
else return 1;
}
float SFBDT_weightUp_Zmumu(float bdt){
if (bdt<0.3) return 0.862626829026;
else if (bdt<0.7) return 0.937149845238;
else if (bdt<0.9) return 1.01076565413;
else if (bdt<1.01) return 1.08304469131;
else return 1;
}
float SFBDT_weightUp_Zee(float bdt){
if (bdt<0.3) return 0.876233508214;
else if (bdt<0.7) return 0.94097108298;
else if (bdt<0.9) return 1.01567282937;
else if (bdt<1.01) return 1.07225261139;
else return 1;
}
float SFBDT_weightDown_Zmumu(float bdt){
if (bdt<0.3) return 0.852011908737;
else if (bdt<0.7) return 0.932527637484;
else if (bdt<0.9) return 1.00806525037;
else if (bdt<1.01) return 1.07750726801;
else return 1;
}
float SFBDT_weightDown_Zee(float bdt){
if (bdt<0.3) return 0.86516823203;
else if (bdt<0.7) return 0.936179490484;
else if (bdt<0.9) return 1.01283968968;
else if (bdt<1.01) return 1.0662941513;
else return 1;
}
