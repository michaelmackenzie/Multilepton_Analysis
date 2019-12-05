

DataPlotter* dataplotter_;
TString selection_ = "mutau";

Int_t init_dataplotter() {

  dataplotter_ = new DataPlotter();
  dataplotter_->selection_ = selection_;
  dataplotter_->folder_ = "clfv_zdecays";
  if(selection_ == "mutau")
    dataplotter_->qcd_scale_ = 1.059;
  else if(selection_ == "etau")
    dataplotter_->qcd_scale_ = 1.157;
    
  //dataset names
  TString names [50]; //for event histogram
  TString fnames[50]; //for getting file (usually the same)
  TString labels[50]; //for combining categories
  bool    signal[50]; //for drawing separately
  bool    isDY  [50]; 
  
  names[0]  = "zjets_m-50_amcatnlo"     ;fnames[0]  = "1_bltTree_zjets_m-50_amcatnlo"     ;labels[0]  = "Z->#tau#tau"   ; signal[0]  = false  ; isDY[0]  = true   ;
  names[1]  = "zjets_m-50_amcatnlo"     ;fnames[1]  = "2_bltTree_zjets_m-50_amcatnlo"     ;labels[1]  = "Z->ee/#mu#mu"  ; signal[1]  = false  ; isDY[1]  = true   ;
  names[2]  = "zjets_m-10to50_amcatnlo" ;fnames[2]  = "1_bltTree_zjets_m-10to50_amcatnlo" ;labels[2]  = "Z->#tau#tau"   ; signal[2]  = false  ; isDY[2]  = true   ;
  names[3]  = "zjets_m-10to50_amcatnlo" ;fnames[3]  = "2_bltTree_zjets_m-10to50_amcatnlo" ;labels[3]  = "Z->ee/#mu#mu"  ; signal[3]  = false  ; isDY[3]  = true   ;
  names[4]  = "ttbar_inclusive"         ;fnames[4]  = "bltTree_ttbar_inclusive"           ;labels[4]  = "Top"           ; signal[4]  = false  ; isDY[4]  = false  ;
  names[5]  = "t_tw"                    ;fnames[5]  = "bltTree_t_tw"                      ;labels[5]  = "Top"           ; signal[5]  = false  ; isDY[5]  = false  ;
  names[6]  = "tbar_tw"                 ;fnames[6]  = "bltTree_tbar_tw"                   ;labels[6]  = "Top"           ; signal[6]  = false  ; isDY[6]  = false  ;
  names[7]  = "zjets_m-50"              ;fnames[7]  = "bltTree_zjets_m-50"                ;labels[7]  = "Z+Jets"        ; signal[7]  = false  ; isDY[7]  = true   ;
  names[8]  = "zjets_m-10to50"          ;fnames[8]  = "bltTree_zjets_m-10to50"            ;labels[8]  = "Z+Jets"        ; signal[8]  = false  ; isDY[8]  = true   ;
  names[9]  = "z1jets_m-50"             ;fnames[9]  = "bltTree_z1jets_m-50"               ;labels[9]  = "Z+Jets"        ; signal[9]  = false  ; isDY[9]  = true   ;
  names[10] = "z1jets_m-10to50"         ;fnames[10] = "bltTree_z1jets_m-10to50"           ;labels[10] = "Z+Jets"        ; signal[10] = false  ; isDY[10] = true   ;
  names[11] = "z2jets_m-50"             ;fnames[11] = "bltTree_z2jets_m-50"               ;labels[11] = "Z+Jets"        ; signal[11] = false  ; isDY[11] = true   ;
  names[12] = "z2jets_m-10to50"         ;fnames[12] = "bltTree_z2jets_m-10to50"           ;labels[12] = "Z+Jets"        ; signal[12] = false  ; isDY[12] = true   ;
  names[13] = "z3jets_m-50"             ;fnames[13] = "bltTree_z3jets_m-50"               ;labels[13] = "Z+Jets"        ; signal[13] = false  ; isDY[13] = true   ;
  names[14] = "z3jets_m-10to50"         ;fnames[14] = "bltTree_z3jets_m-10to50"           ;labels[14] = "Z+Jets"        ; signal[14] = false  ; isDY[14] = true   ;
  names[15] = "z4jets_m-50"             ;fnames[15] = "bltTree_z4jets_m-50"               ;labels[15] = "Z+Jets"        ; signal[15] = false  ; isDY[15] = true   ;
  names[16] = "z4jets_m-10to50"         ;fnames[16] = "bltTree_z4jets_m-10to50"           ;labels[16] = "Z+Jets"        ; signal[16] = false  ; isDY[16] = true   ;
  names[17] = "w1jets"                  ;fnames[17] = "bltTree_w1jets"                    ;labels[17] = "W+Jets"        ; signal[17] = false  ; isDY[17] = false  ;
  names[18] = "w2jets"                  ;fnames[18] = "bltTree_w2jets"                    ;labels[18] = "W+Jets"        ; signal[18] = false  ; isDY[18] = false  ;
  names[19] = "w3jets"                  ;fnames[19] = "bltTree_w3jets"                    ;labels[19] = "W+Jets"        ; signal[19] = false  ; isDY[19] = false  ;
  names[20] = "w4jets"                  ;fnames[20] = "bltTree_w4jets"                    ;labels[20] = "W+Jets"        ; signal[20] = false  ; isDY[20] = false  ;
  names[21] = "ww"                      ;fnames[21] = "bltTree_ww"                        ;labels[21] = "Diboson"       ; signal[21] = false  ; isDY[21] = false  ;
  names[22] = "wz_2l2q"                 ;fnames[22] = "bltTree_wz_2l2q"                   ;labels[22] = "Diboson"       ; signal[22] = false  ; isDY[22] = false  ;
  names[23] = "wz_3lnu"                 ;fnames[23] = "bltTree_wz_3lnu"                   ;labels[23] = "Diboson"       ; signal[23] = false  ; isDY[23] = false  ;
  names[24] = "zz_2l2nu"                ;fnames[24] = "bltTree_zz_2l2nu"                  ;labels[24] = "Diboson"       ; signal[24] = false  ; isDY[24] = false  ;
  names[25] = "zz_2l2q"                 ;fnames[25] = "bltTree_zz_2l2q"                   ;labels[25] = "Diboson"       ; signal[25] = false  ; isDY[25] = false  ;
  names[26] = "zz_4l"                   ;fnames[26] = "bltTree_zz_4l"                     ;labels[26] = "Diboson"       ; signal[26] = false  ; isDY[26] = false  ;
  names[27] = "hzg_gluglu"              ;fnames[27] = "bltTree_hzg_gluglu"                ;labels[27] = "H->Zg"         ; signal[27] = true   ; isDY[27] = false  ;
  names[28] = "hzg_tth"                 ;fnames[28] = "bltTree_hzg_tth"                   ;labels[28] = "H->Zg"         ; signal[28] = true   ; isDY[28] = false  ;
  names[29] = "hzg_vbf"                 ;fnames[29] = "bltTree_hzg_vbf"                   ;labels[29] = "H->Zg"         ; signal[29] = true   ; isDY[29] = false  ;
  names[30] = "hzg_wminus"              ;fnames[30] = "bltTree_hzg_wminus"                ;labels[30] = "H->Zg"         ; signal[30] = true   ; isDY[30] = false  ;
  names[31] = "hzg_wplus"               ;fnames[31] = "bltTree_hzg_wplus"                 ;labels[31] = "H->Zg"         ; signal[31] = true   ; isDY[31] = false  ;
  names[32] = "hzg_zh"                  ;fnames[32] = "bltTree_hzg_zh"                    ;labels[32] = "H->Zg"         ; signal[32] = true   ; isDY[32] = false  ;
  names[33] = "htautau_gluglu"          ;fnames[33] = "bltTree_htautau_gluglu"            ;labels[33] = "H->#tau#tau"   ; signal[33] = true   ; isDY[33] = false  ;
  names[34] = "zetau"                   ;fnames[34] = "bltTree_zetau"                     ;labels[34] = "Z->e#tau"      ; signal[34] = true   ; isDY[34] = false  ;
  names[35] = "zmutau"                  ;fnames[35] = "bltTree_zmutau"                    ;labels[35] = "Z->#mu#tau"    ; signal[35] = true   ; isDY[35] = false  ;

  Double_t xsec[50];
  //Taken from https://twiki.cern.ch/twiki/bin/viewauth/CMS/SummaryTable1G25ns     
  xsec[0]  =  6225.42; //6803.2;   //6225.42  ; //5765.4;    //"zjets_m-50_amcatnlo"     
  xsec[1]  =  6225.42; //6803.2;   //6225.42  ; //5765.4;    //"zjets_m-50_amcatnlo"     
  xsec[2]  =  18610. ; //21959.8; //18610.  ;         //"zjets_m-10to50_amcatnlo" 
  xsec[3]  =  18610. ; //21959.8; //18610.  ;         //"zjets_m-10to50_amcatnlo" 
  xsec[4]  =  831.76;                 //"ttbar_inclusive"         
  xsec[5]  =  35.85;	               //"t_tw"                    
  xsec[6]  =  35.85;	               //"tbar_tw"                 
  xsec[7]  =  6803.2;	               //"zjets_m-50"              
  xsec[8]  =  21959.8;	               //"zjets_m-10to50"          
  xsec[9]  =  1198.9;	               //"z1jets_m-50"             
  xsec[10] =  855.5;		       //"z1jets_m-10to50"         
  xsec[11] =  390.6;		       //"z2jets_m-50"             
  xsec[12] =  466.1;		       //"z2jets_m-10to50"         
  xsec[13] =  113.3;		       //"z3jets_m-50"             
  xsec[14] =  114.5;		       //"z3jets_m-10to50"         
  xsec[15] =  60.2;		       //"z4jets_m-50"             
  xsec[16] =  36.4;		       //"z4jets_m-10to50"         
  xsec[17] =  11486.53; //9493.;		       //"w1jets"                  
  xsec[18] =  3775.2; //3120.;		       //"w2jets"                  
  xsec[19] =  1139.82; //942.3;		       //"w3jets"                  
  xsec[20] =  655.82; //524.1;		       //"w4jets"                  
  xsec[21] =  12.178;	               //"ww"                      
  xsec[22] =  5.595;		       //"wz_2l2q"                 
  xsec[23] =  4.42965;	               //"wz_3lnu"                 
  xsec[24] =  0.564;		       //"zz_2l2nu"                
  xsec[25] =  3.22;		       //"zz_2l2q"                 
  xsec[26] =  1.212;		       //"zz_4l"
  //Higgs branching ratios: https://twiki.cern.ch/twiki/bin/view/LHCPhysics/CERNYellowReportPageBR2014
  //Higgs production xsecs: https://twiki.cern.ch/twiki/bin/view/LHCPhysics/CERNYellowReportPageAt1314TeV2014
  //Z decay to leptons fraction: http://pdg.lbl.gov/2012/listings/rpp2012-list-z-boson.pdf
  // --> Br(Z->ll) * Br(h->Zg) * xsec(X -> h)
  xsec[27] =    3.*3.3658/100.*1.54e-3* 43.92;	       //"hzg_gluglu"              
  xsec[28] =    3.*3.3658/100.*1.54e-3* 0.5085;	       //"hzg_tth"                 
  xsec[29] =    3.*3.3658/100.*1.54e-3* 3.748;	       //"hzg_vbf"                 
  xsec[30] =    3.*3.3658/100.*1.54e-3* 1.380/2.;	       //"hzg_wminus"              
  xsec[31] =    3.*3.3658/100.*1.54e-3* 1.380/2.;	       //"hzg_wplus"               
  xsec[32] =    3.*3.3658/100.*1.54e-3* 0.8696;	       //"hzg_zh"                  
  xsec[33] =                   6.32e-2* 43.92;	       //"htautau_gluglu"                  
  xsec[34] =    ((6225.42+18610.)/(3.*3.3658e-2))*9.8e-6*100./2000.; //zetau z->ll / br(ll) * br(etau, CL=95) *N(accepted)/N(Gen) http://pdg.lbl.gov/2018/listings/rpp2018-list-z-boson.pdf
  xsec[35] =    ((6225.42+18610.)/(3.*3.3658e-2))*1.2e-5*135./2000.; //zmutau z->ll/ br(ll) * br(mutau, CL=95)*N(accepted)/N(Gen) http://pdg.lbl.gov/2018/listings/rpp2018-list-z-boson.pdf

  int process[50];
  for(int i = 0; i < sizeof(process)/sizeof(*process); ++i)
    process[i]=0;

  //flags to include the set or not
  process[0]  = 1; //"zjets_m-50_amcatnlo"     
  process[1]  = 1; //"zjets_m-50_amcatnlo"     
  process[2]  = 1; //"zjets_m-10to50_amcatnlo" 
  process[3]  = 1; //"zjets_m-10to50_amcatnlo" 
  process[4]  = 1; //"ttbar_inclusive"         
  process[5]  = 1; //"t_tw"                    
  process[6]  = 1; //"tbar_tw"                 
  process[7]  = 0; //"zjets_m-50"              
  process[8]  = 0; //"zjets_m-10to50"          
  process[9]  = 0; //"z1jets_m-50"             
  process[10] = 0; //"z1jets_m-10to50"         
  process[11] = 0; //"z2jets_m-50"             
  process[12] = 0; //"z2jets_m-10to50"         
  process[13] = 0; //"z3jets_m-50"             
  process[14] = 0; //"z3jets_m-10to50"         
  process[15] = 0; //"z4jets_m-50"             
  process[16] = 0; //"z4jets_m-10to50"         
  process[17] = 1; //"w1jets"                  
  process[18] = 1; //"w2jets"                  
  process[19] = 1; //"w3jets"                  
  process[20] = 1; //"w4jets"                  
  process[21] = 1; //"ww"                      
  process[22] = 1; //"wz_2l2q"                 
  process[23] = 1; //"wz_3lnu"                 
  process[24] = 1; //"zz_2l2nu"                
  process[25] = 1; //"zz_2l2q"                 
  process[26] = 1; //"zz_4l"                   
  process[27] = 0; //"hzg_gluglu"              
  process[28] = 0; //"hzg_tth"                 
  process[29] = 0; //"hzg_vbf"                 
  process[30] = 0; //"hzg_wminus"              
  process[31] = 0; //"hzg_wplus"               
  process[32] = 0; //"hzg_zh"                  
  process[33] = 1; //"htautau_gluglu"                  
  process[34] = (selection_ == "etau" ) ? 1 : 0; //"zetau"
  process[35] = (selection_ == "mutau") ? 1 : 0; //"zmutau"

  vector<TString> files;
  for(int i = 0; i < sizeof(process)/sizeof(*process); ++i) {
    if(process[i]) {
      files.push_back(Form("ztautau/ztautau_%s_%s.hist",selection_.Data(),fnames[i].Data()));
    } else
      files.push_back(Form(""));
  }

  const char* dNamesMu[] = {
    "muon_2016B",
    "muon_2016C",
    "muon_2016D",
    "muon_2016E",
    "muon_2016F",
    "muon_2016G",
    "muon_2016H",
  };
  
  const char* dNamesE[] = {
    "electron_2016B",
    "electron_2016C",
    "electron_2016D",
    "electron_2016E",
    "electron_2016F",
    "electron_2016G",
    "electron_2016H"
  };
  
  // const char* dFiles[] = {Form("tree_%s.hist",dNames[0])};
  vector<TString> dFiles;
  vector<TString> dNames;
  for(int i = 0; i < sizeof(dNamesMu)/sizeof(*dNamesMu); ++i) {
    if(selection_ == "etau" ) {
      dNames.push_back(dNamesE[i]);
      dFiles.push_back(Form("ztautau/ztautau_%s_bltTree_%s.hist",selection_.Data(),dNamesE[i]));
    } else if(selection_ == "mutau") {
      dNames.push_back(dNamesMu[i]);
      dFiles.push_back(Form("ztautau/ztautau_%s_bltTree_%s.hist",selection_.Data(),dNamesMu[i]));
    }
    
  }
  
  const Double_t dLum[] = {
    5.75e3  ,
    2.573e3  ,
    4.242e3  ,
    4.025e3  ,
    3.105e3  ,
    7.576e3  ,
    8.651e3  
                          };//, (36.4e3-5.3e3)}; //taken from CMS AN-16-458

  double lum    = accumulate(begin(dLum), end(dLum), 0, plus<double>());; //pb^-1
  dataplotter_->set_luminosity(lum);

  for(int i = 0; i < files.size(); ++i) {
    if(files[i] != "") dataplotter_->add_dataset(files[i], names[i], labels[i], false, xsec[i], signal[i]);
  }
  
  for(int i = 0; i < dFiles.size(); ++i)
    if(dFiles[i] != "") dataplotter_->add_dataset(dFiles[i], dNames[i], "Data", true, 1., false);

  return dataplotter_->init_files();
}
