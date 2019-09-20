

DataPlotter* dataplotter_;
TString selection_ = "mutau";

Int_t init_dataplotter() {

  dataplotter_ = new DataPlotter();
  dataplotter_->selection_ = selection_;
  //dataset names
  TString names[50];
  TString labels[50];
  names[0]  = "zjets_m-50_amcatnlo"     ;labels[0]  = "Z+Jets"                  ;
  names[1]  = "zjets_m-10to50_amcatnlo" ;labels[1]  = "Z+Jets"                  ;
  names[2]  = "ttbar_inclusive"         ;labels[2]  = "Top"                     ;
  names[3]  = "t_tw"                    ;labels[3]  = "Top"                     ;
  names[4]  = "tbar_tw"                 ;labels[4]  = "Top"                     ;
  names[5]  = "zjets_m-50"              ;labels[5]  = "Z+Jets"                  ;
  names[6]  = "zjets_m-10to50"          ;labels[6]  = "Z+Jets"                  ;
  names[7]  = "z1jets_m-50"             ;labels[7]  = "Z+Jets"                  ;
  names[8]  = "z1jets_m-10to50"         ;labels[8]  = "Z+Jets"                  ;
  names[9]  = "z2jets_m-50"             ;labels[9]  = "Z+Jets"                  ;
  names[10] = "z2jets_m-10to50"         ;labels[10] = "Z+Jets"                  ;
  names[11] = "z3jets_m-50"             ;labels[11] = "Z+Jets"                  ;
  names[12] = "z3jets_m-10to50"         ;labels[12] = "Z+Jets"                  ;
  names[13] = "z4jets_m-50"             ;labels[13] = "Z+Jets"                  ;
  names[14] = "z4jets_m-10to50"         ;labels[14] = "Z+Jets"                  ;
  names[15] = "w1jets"                  ;labels[15] = "W+Jets"                  ;
  names[16] = "w2jets"                  ;labels[16] = "W+Jets"                  ;
  names[17] = "w3jets"                  ;labels[17] = "W+Jets"                  ;
  names[18] = "w4jets"                  ;labels[18] = "W+Jets"                  ;
  names[19] = "ww"                      ;labels[19] = "Diboson"                 ;
  names[20] = "wz_2l2q"                 ;labels[20] = "Diboson"                 ;
  names[21] = "wz_3lnu"                 ;labels[21] = "Diboson"                 ;
  names[22] = "zz_2l2nu"                ;labels[22] = "Diboson"                 ;
  names[23] = "zz_2l2q"                 ;labels[23] = "Diboson"                 ;
  names[24] = "zz_4l"                   ;labels[24] = "Diboson"                 ;
  names[25] = "hzg_gluglu"              ;labels[25] = "H->Zg"                   ;
  names[26] = "hzg_tth"                 ;labels[26] = "H->Zg"                   ;
  names[27] = "hzg_vbf"                 ;labels[27] = "H->Zg"                   ;
  names[28] = "hzg_wminus"              ;labels[28] = "H->Zg"                   ;
  names[29] = "hzg_wplus"               ;labels[29] = "H->Zg"                   ;
  names[30] = "hzg_zh"                  ;labels[30] = "H->Zg"                   ;
  names[31] = "htautau_gluglu"          ;labels[31] = "H->tau tau"              ;

  Double_t xsec[50];
  //Taken from https://twiki.cern.ch/twiki/bin/viewauth/CMS/SummaryTable1G25ns     
  xsec[0]  =  6225.42; //6803.2;   //6225.42  ; //5765.4;    //"zjets_m-50_amcatnlo"     
  xsec[1]  =  18610. ; //21959.8; //18610.  ;         //"zjets_m-10to50_amcatnlo" 
  xsec[2]  =  831.76;                 //"ttbar_inclusive"         
  xsec[3]  =  35.85;	               //"t_tw"                    
  xsec[4]  =  35.85;	               //"tbar_tw"                 
  xsec[5]  =  6803.2;	               //"zjets_m-50"              
  xsec[6]  =  21959.8;	               //"zjets_m-10to50"          
  xsec[7]  =  1198.9;	               //"z1jets_m-50"             
  xsec[8]  =  855.5;		       //"z1jets_m-10to50"         
  xsec[9]  =  390.6;		       //"z2jets_m-50"             
  xsec[10] =  466.1;		       //"z2jets_m-10to50"         
  xsec[11] =  113.3;		       //"z3jets_m-50"             
  xsec[12] =  114.5;		       //"z3jets_m-10to50"         
  xsec[13] =  60.2;		       //"z4jets_m-50"             
  xsec[14] =  36.4;		       //"z4jets_m-10to50"         
  xsec[15] =  9493.;		       //"w1jets"                  
  xsec[16] =  3120.;		       //"w2jets"                  
  xsec[17] =  942.3;		       //"w3jets"                  
  xsec[18] =  524.1;		       //"w4jets"                  
  xsec[19] =  12.178;	               //"ww"                      
  xsec[20] =  5.595;		       //"wz_2l2q"                 
  xsec[21] =  4.42965;	               //"wz_3lnu"                 
  xsec[22] =  0.564;		       //"zz_2l2nu"                
  xsec[23] =  3.22;		       //"zz_2l2q"                 
  xsec[24] =  1.212;		       //"zz_4l"
  //Higgs branching ratios: https://twiki.cern.ch/twiki/bin/view/LHCPhysics/CERNYellowReportPageBR2014
  //Higgs production xsecs: https://twiki.cern.ch/twiki/bin/view/LHCPhysics/CERNYellowReportPageAt1314TeV2014
  //Z decay to leptons fraction: http://pdg.lbl.gov/2012/listings/rpp2012-list-z-boson.pdf
  // --> Br(Z->ll) * Br(h->Zg) * xsec(X -> h)
  xsec[25] =    3.*3.3658/100.*1.54e-3* 43.92;	       //"hzg_gluglu"              
  xsec[26] =    3.*3.3658/100.*1.54e-3* 0.5085;	       //"hzg_tth"                 
  xsec[27] =    3.*3.3658/100.*1.54e-3* 3.748;	       //"hzg_vbf"                 
  xsec[28] =    3.*3.3658/100.*1.54e-3* 1.380/2.;	       //"hzg_wminus"              
  xsec[29] =    3.*3.3658/100.*1.54e-3* 1.380/2.;	       //"hzg_wplus"               
  xsec[30] =    3.*3.3658/100.*1.54e-3* 0.8696;	       //"hzg_zh"                  
  xsec[31] =                   6.32e-2* 43.92;	       //"htautau_gluglu"                  

  int process[50];
  for(int i = 0; i < sizeof(process)/sizeof(*process); ++i)
    process[i]=0;

  //flags to include the set or not
  process[0]  = 1; //"zjets_m-50_amcatnlo"     
  process[1]  = 1; //"zjets_m-10to50_amcatnlo" 
  process[2]  = 1; //"ttbar_inclusive"         
  process[3]  = 1; //"t_tw"                    
  process[4]  = 1; //"tbar_tw"                 
  process[5]  = 0; //"zjets_m-50"              
  process[6]  = 0; //"zjets_m-10to50"          
  process[7]  = 0; //"z1jets_m-50"             
  process[8]  = 0; //"z1jets_m-10to50"         
  process[9]  = 0; //"z2jets_m-50"             
  process[10] = 0; //"z2jets_m-10to50"         
  process[11] = 0; //"z3jets_m-50"             
  process[12] = 0; //"z3jets_m-10to50"         
  process[13] = 0; //"z4jets_m-50"             
  process[14] = 0; //"z4jets_m-10to50"         
  process[15] = 1; //"w1jets"                  
  process[16] = 1; //"w2jets"                  
  process[17] = 1; //"w3jets"                  
  process[18] = 1; //"w4jets"                  
  process[19] = 1; //"ww"                      
  process[20] = 1; //"wz_2l2q"                 
  process[21] = 1; //"wz_3lnu"                 
  process[22] = 1; //"zz_2l2nu"                
  process[23] = 1; //"zz_2l2q"                 
  process[24] = 1; //"zz_4l"                   
  process[25] = 1; //"hzg_gluglu"              
  process[26] = 1; //"hzg_tth"                 
  process[27] = 1; //"hzg_vbf"                 
  process[28] = 0; //"hzg_wminus"              
  process[29] = 0; //"hzg_wplus"               
  process[30] = 1; //"hzg_zh"                  
  process[31] = 1; //"htautau_gluglu"                  

  vector<TString> files;
  for(int i = 0; i < sizeof(process)/sizeof(*process); ++i) {
    if(process[i])
      files.push_back(Form("ztautau/ztautau_%s_bltTree_%s.hist",selection_.Data(),names[i].Data()));
    else
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

  for(int i = 0; i < files.size(); ++i)
    if(files[i] != "") dataplotter_->add_dataset(files[i], names[i], labels[i], 0, xsec[i]);

  for(int i = 0; i < dFiles.size(); ++i)
    if(dFiles[i] != "") dataplotter_->add_dataset(dFiles[i], dNames[i], "Data", 1, xsec[i]);

  return dataplotter_->init_files();
}
