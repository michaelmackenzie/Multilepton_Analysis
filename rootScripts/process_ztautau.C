
#include "ZTauTauHistMaker.cc++g"
Int_t process_ztautau() {

  const char* gridPath[] = {
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //ttbar
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //DY AMC
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //DY AMC
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //t_tw
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //tbar_tw
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //DY MadGraph
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //DY MadGraph
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //DY 1 Jet MadGraph
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //DY 1 Jet MadGraph
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //DY 2 Jet MadGraph
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //DY 2 Jet MadGraph
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //DY 3 Jet MadGraph
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //DY 3 Jet MadGraph
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //DY 4 Jet MadGraph
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //DY 4 Jet MadGraph
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //W 1 Jet
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //W 2 Jet
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //W 3 Jet
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //W 4 Jet
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //WW
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //WZ Jets to 2L2Q
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //WZ Jets to 3LNu
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //ZZ Jets to 2L2Nu
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //ZZ Jets to 2L2Q
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //ZZ Jets to 4L
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191004_110436/", //HZG gluglu
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191004_110436/", //HZG tth
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191004_110436/", //HZG vbf
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191004_110436/", //HZG W-
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191004_110436/", //HZG W+
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191004_110436/", //HZG zh
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140407/", //HTauTau gluglu
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140407/", //ZETau
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140407/", //ZMuTau
    "/eos/uscms/store/user/mmackenz/private_mc/2016/BaconAna_Output/"                , //ZMuTau NoFilter
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //Muon Data 2016 B
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //Muon Data 2016 C
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //Muon Data 2016 D
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //Muon Data 2016 E
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //Muon Data 2016 F
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //Muon Data 2016 G
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //Muon Data 2016 H
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //Electron Data 2016 B
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //Electron Data 2016 C
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //Electron Data 2016 D
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //Electron Data 2016 E
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //Electron Data 2016 F
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191118_140159/", //Electron Data 2016 G
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20191004_110436/"  //Electron Data 2016 H
    
  };
  
  const char* files[] = {"output_ttbar_inclusive.root"             ,
			 "output_DYJetsToLL_M-50_amcatnlo.root"    ,
			 "output_DYJetsToLL_M-10to50_amcatnlo.root",
			 "output_T_tW-channel.root"                ,
			 "output_Tbar_tW-channel.root"             ,
			 "output_DYJetsToLL_M-50.root"             ,
			 "output_DYJetsToLL_M-10to50.root"         ,
			 "output_DY1JetsToLL_M-50.root"            ,
			 "output_DY1JetsToLL_M-10to50.root"        ,
			 "output_DY2JetsToLL_M-50.root"            ,
			 "output_DY2JetsToLL_M-10to50.root"        ,
			 "output_DY3JetsToLL_M-50.root"            ,
			 "output_DY3JetsToLL_M-10to50.root"        ,
			 "output_DY4JetsToLL_M-50.root"            ,
			 "output_DY4JetsToLL_M-10to50.root"        ,
			 "output_W1JetsToLNu.root"                 ,
			 "output_W2JetsToLNu.root"                 ,
			 "output_W3JetsToLNu.root"                 ,
			 "output_W4JetsToLNu.root"                 ,
			 "output_WW.root"                          ,
			 "output_WZJetsTo2L2Q.root"                ,
			 "output_WZJetsTo3LNu.root"                ,
			 "output_ZZJetsTo2L2Nu.root"               ,
			 "output_ZZJetsTo2L2Q.root"                ,
			 "output_ZZJetsTo4L.root"                  ,
			 "output_hzg_gluglu.root"                  ,
			 "output_hzg_tth.root"                     ,
			 "output_hzg_vbf.root"                     ,
			 "output_hzg_wminus.root"                  ,
			 "output_hzg_wplus.root"                   ,
			 "output_hzg_zh.root"                      ,
			 "output_htautau_gluglu.root"              ,
			 "output_zetau.root"                       ,
			 "output_zmutau.root"                      ,
			 "output_zmutau_nofilter.root"             ,
			 "output_muon_2016B_v2.root"               , 
			 "output_muon_2016C.root"                  , 
			 "output_muon_2016D.root"                  , 
			 "output_muon_2016E.root"                  , 
			 "output_muon_2016F.root"                  , 
			 "output_muon_2016G.root"                  , 
			 "output_muon_2016H_v2.root"               ,
			 "output_electron_2016B_v2.root"               , 
			 "output_electron_2016C.root"                  , 
			 "output_electron_2016D.root"                  , 
			 "output_electron_2016E.root"                  , 
			 "output_electron_2016F.root"                  , 
			 "output_electron_2016G.root"                  , 
			 "output_electron_2016H_v2.root"

  };
  const int doProcess[] = {1, //ttbar
			   1, //DY AMC
			   1, //DY AMC
			   1, //t_tw
			   1, //tbar_tw
			   0, //DY MadGraph
			   0, //DY MadGraph
			   0, //DY 1 Jet MadGraph
			   0, //DY 1 Jet MadGraph
			   0, //DY 2 Jet MadGraph
			   0, //DY 2 Jet MadGraph
			   0, //DY 3 Jet MadGraph
			   0, //DY 3 Jet MadGraph
			   0, //DY 4 Jet MadGraph
			   0, //DY 4 Jet MadGraph
			   1, //W 1 Jet
			   1, //W 2 Jet
			   1, //W 3 Jet
			   1, //W 4 Jet
			   1, //WW
			   1, //WZ Jets to 2L2Q
			   1, //WZ Jets to 3LNu
			   1, //ZZ Jets to 2L2Nu
			   1, //ZZ Jets to 2L2Q
			   1, //ZZ Jets to 4L
			   0, //HZG gluglu
			   0, //HZG tth
			   0, //HZG vbf
			   0, //HZG W-
			   0, //HZG W+
			   0, //HZG zh
			   1, //HTauTau gluglu
			   1, //ZETau
			   1, //ZMuTau
			   0, //ZMuTau NoFilter
			   1, //Muon Data 2016 B
			   1, //Muon Data 2016 C
			   1, //Muon Data 2016 D
			   1, //Muon Data 2016 E
			   1, //Muon Data 2016 F
			   1, //Muon Data 2016 G
			   1, //Muon Data 2016 H
			   1, //Electron Data 2016 B
			   1, //Electron Data 2016 C
			   1, //Electron Data 2016 D
			   1, //Electron Data 2016 E
			   1, //Electron Data 2016 F
			   1, //Electron Data 2016 G
			   1  //Electron Data 2016 H
  };
  
  Double_t xsec[100];
  for(int i = 0; i < sizeof(xsec)/sizeof(*xsec); ++i)
    xsec[i] = 1.;
  
  //Taken from https://twiki.cern.ch/twiki/bin/viewauth/CMS/SummaryTable1G25ns     
  xsec[0]  =  831.76;                 //"ttbar_inclusive"         
  xsec[1]  =  6225.42; //6803.2;   //6225.42  ; //5765.4;    //"zjets_m-50_amcatnlo"     
  xsec[2]  =  18610. ; //21959.8; //18610.  ;         //"zjets_m-10to50_amcatnlo" 
  xsec[3]  =  35.85;	               //"t_tw"                    
  xsec[4]  =  35.85;	               //"tbar_tw"                 
  xsec[5]  =  6225.42; //6803.2;   //6225.42  ; //5765.4;    //"zjets_m-50_amcatnlo"     
  xsec[6]  =  18610. ; //21959.8; //18610.  ;         //"zjets_m-10to50_amcatnlo" 
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

  TStopwatch* timer = new TStopwatch();

  bool useTauFakeSF = true;
  bool writeTrees = true;
  Int_t category = 0;
  for(int i = 0; i < sizeof(files)/sizeof(*files); ++i) {
    ++category;
    if(!doProcess[i]) continue;
    const char* c = files[i];
    TFile* f = new TFile(Form("%s%s",gridPath[i],c));
    if(f == 0) {
      printf("File %s%s not found, continuing\n",gridPath[i],c);
      continue;
    }
    printf("using %s%s\n",gridPath[i],c);
    TDirectoryFile* fChannel = 0;
    TH1F* events = 0;
    TKey* key = 0;
    TIter nextkey(f->GetListOfKeys());

    //for splitting DY lepton decays
    TString cString = c;
    bool isDY = (cString.Contains("DY"));
    
    //get events histograms first
    while((key = (TKey*)nextkey())) {
      TObject* obj = key->ReadObj();
      if(obj->IsA()->InheritsFrom(TH1::Class())) events = (TH1F*)obj;
    }
    if(events == 0) {
      printf("Events Histogram in %s%s not found, continuing\n",gridPath[i],c);
      continue;
    }
    

    TIter nextkeyT(f->GetListOfKeys());
    nextkeyT.Reset();
    while((key = (TKey*)nextkeyT())) {
      TObject* obj = key->ReadObj();
      if(obj->IsA()->InheritsFrom(TDirectoryFile::Class())) fChannel = (TDirectoryFile*)obj;
      else {
	continue;
      }
      printf("Found file %s\n",fChannel->GetName());
      TTree* tree = 0;
      TH1F* eventsChannel = 0;
      TKey* key2 = 0;
      TIter nextkey2(fChannel->GetListOfKeys());
      while((key2 = (TKey*)nextkey2())) {
	TObject* obj2 = key2->ReadObj(); 
	if(obj2->IsA()->InheritsFrom(TH1::Class())) eventsChannel = (TH1F*)obj2;
	if(obj2->IsA()->InheritsFrom(TTree::Class())) tree = (TTree*)obj2;
      }
    
      if(tree == 0) {
	printf("Tree in %s%s/%s not found, continuing\n",gridPath[i],c,fChannel->GetName());
	continue;
      }
      if(eventsChannel == 0) {
	printf("Events Channel Histogram in %s%s/%s not found, continuing\n",gridPath[i],c,fChannel->GetName());
	continue;
      }
      // isDY = false;
      int nloops = (isDY) ? 2 : 1;
      for(int loop = 1; loop <= nloops; ++loop) {
	ZTauTauHistMaker* selec = new ZTauTauHistMaker();
	TString cString = c;
	selec->fFolderName = fChannel->GetName();
	if(isDY) selec->fDYType = loop;
	selec->fEventCategory = category;
	selec->fWriteTrees = writeTrees;
	selec->fUseTauFakeSF = useTauFakeSF;
	selec->fXsec = xsec[i]/events->GetBinContent(1);
	tree->Process(selec,"");
	TFile* out = new TFile(Form("ztautau_%s%s_%s.hist",fChannel->GetName(),
				    (isDY) ? Form("_%i",loop) : "", tree->GetName()),"UPDATE");
	if(out == 0) {
	  printf("Unable to find output hist file ztautau_%s%s_%s.hist, continuing\n",fChannel->GetName(),
		 (isDY) ? Form("_%i",loop) : "", tree->GetName());
	  continue;
	}
	events->Write();
	out->Write();
	delete out;
      }
    }
    delete events;
    delete f;
  }
  Double_t cpuTime = timer->CpuTime();
  Double_t realTime = timer->RealTime();
  printf("Processing time: %7.2fs CPU time %7.2fs Wall time\n",cpuTime,realTime);
  if(realTime > 600. ) printf("Processing time: %7.2fmin CPU time %7.2fmin Wall time\n",cpuTime/60.,realTime/60.);

  return 0;
}
