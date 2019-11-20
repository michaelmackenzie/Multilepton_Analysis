
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
			   1, //ZMuTau NoFilter
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
			   0  //Electron Data 2016 H
  };

  for(int i = 0; i < sizeof(files)/sizeof(*files); ++i) {
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
  return 0;
}
