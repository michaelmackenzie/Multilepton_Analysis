float SFBDT_weight_Zmumu(float bdt){
if (bdt<0.3) return 1.13040384656;
else if (bdt<0.7) return 0.997102114961;
else if (bdt<0.9) return 0.98064041578;
else if (bdt<1.01) return 1.02290654759;
else return 1;
}
float SFBDT_weight_Zee(float bdt){
if (bdt<0.3) return 1.12791280956;
else if (bdt<0.7) return 1.00460215341;
else if (bdt<0.9) return 0.983970488752;
else if (bdt<1.01) return 1.00556877834;
else return 1;
}
float SFBDT_weight_Zll(float bdt){
if (bdt<0.3) return (1.12791280956+1.13040384656)/2.;
else if (bdt<0.7) return (1.00460215341+0.997102114961)/2.;
else if (bdt<0.9) return (0.983970488752+0.98064041578)/2.;
else if (bdt<1.01) return (1.00556877834+1.02290654759)/2.;
else return 1;
}
float SFBDT_weightUp_Zmumu(float bdt){
if (bdt<0.3) return 1.13630932776;
else if (bdt<0.7) return 0.999157302142;
else if (bdt<0.9) return 0.981714850038;
else if (bdt<1.01) return 1.02504324308;
else return 1;
}
float SFBDT_weightUp_Zee(float bdt){
if (bdt<0.3) return 1.13378632375;
else if (bdt<0.7) return 1.00671949526;
else if (bdt<0.9) return 0.985110452893;
else if (bdt<1.01) return 1.0079748054;
else return 1;
}
float SFBDT_weightDown_Zmumu(float bdt){
if (bdt<0.3) return 1.12449836535;
else if (bdt<0.7) return 0.995046927779;
else if (bdt<0.9) return 0.979565981522;
else if (bdt<1.01) return 1.02076985211;
else return 1;
}
float SFBDT_weightDown_Zee(float bdt){
if (bdt<0.3) return 1.12203929538;
else if (bdt<0.7) return 1.00248481157;
else if (bdt<0.9) return 0.982830524612;
else if (bdt<1.01) return 1.00316275127;
else return 1;
}
