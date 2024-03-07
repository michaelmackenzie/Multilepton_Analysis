float SFBDT_weight_Zmumu(float bdt){
if (bdt<0.3) return 0.966398656323;
else if (bdt<0.7) return 0.957180214112;
else if (bdt<0.9) return 1.00179638752;
else if (bdt<1.01) return 1.05152468485;
else return 1;
}
float SFBDT_weight_Zee(float bdt){
if (bdt<0.3) return 0.986164562137;
else if (bdt<0.7) return 0.965465700814;
else if (bdt<0.9) return 1.00300052806;
else if (bdt<1.01) return 1.04430057854;
else return 1;
}
float SFBDT_weightUp_Zmumu(float bdt){
if (bdt<0.3) return 0.971449773104;
else if (bdt<0.7) return 0.959204181569;
else if (bdt<0.9) return 1.00295995185;
else if (bdt<1.01) return 1.05388359188;
else return 1;
}
float SFBDT_weightUp_Zee(float bdt){
if (bdt<0.3) return 0.991794473754;
else if (bdt<0.7) return 0.967750121622;
else if (bdt<0.9) return 1.00435943446;
else if (bdt<1.01) return 1.04729427933;
else return 1;
}
float SFBDT_weightDown_Zmumu(float bdt){
if (bdt<0.3) return 0.961347539542;
else if (bdt<0.7) return 0.955156246655;
else if (bdt<0.9) return 1.00063282318;
else if (bdt<1.01) return 1.04916577783;
else return 1;
}
float SFBDT_weightDown_Zee(float bdt){
if (bdt<0.3) return 0.980534650519;
else if (bdt<0.7) return 0.963181280006;
else if (bdt<0.9) return 1.00164162166;
else if (bdt<1.01) return 1.04130687776;
else return 1;
}
