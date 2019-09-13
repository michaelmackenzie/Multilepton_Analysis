
#include "ZTauTauHistMaker.cc++g"
Int_t process_ztautau() {

  const char* gridPath[] = {
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190909_153240/", //ttbar
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190909_153240/", //DY AMC
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190909_153240/", //DY AMC
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190909_153240/", //t_tw
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190909_153240/", //tbar_tw
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190909_153240/", //DY MadGraph
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190909_153240/", //DY MadGraph
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190909_153240/", //DY 1 Jet MadGraph
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190909_153240/", //DY 1 Jet MadGraph
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190909_153240/", //DY 2 Jet MadGraph
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190909_153240/", //DY 2 Jet MadGraph
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190909_153240/", //DY 3 Jet MadGraph
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190909_153240/", //DY 3 Jet MadGraph
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190909_153240/", //DY 4 Jet MadGraph
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190909_153240/", //DY 4 Jet MadGraph
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190909_153240/", //W 1 Jet
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190909_153240/", //W 2 Jet
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190909_153240/", //W 3 Jet
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190909_153240/", //W 4 Jet
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190909_153240/", //WW
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190909_153240/", //WZ Jets to 2L2Q
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190909_153240/", //WZ Jets to 3LNu
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190909_153240/", //ZZ Jets to 2L2Nu
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190909_153240/", //ZZ Jets to 2L2Q
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190909_153240/", //ZZ Jets to 4L
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190909_153240/", //HZG gluglu
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190909_153240/", //HZG tth
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190909_153240/", //HZG vbf
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190909_153240/", //HZG W-
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190909_153240/", //HZG W+
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190909_153240/", //HZG zh
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190910_144942/", //Data 2016 B
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190910_144942/", //Data 2016 C
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190910_144942/", //Data 2016 D
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190910_144942/", //Data 2016 E
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190910_144942/", //Data 2016 F
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190910_144942/", //Data 2016 G
    "/eos/uscms/store/user/mmackenz/batch_output/single_lepton_2016_20190910_144942/"  //Data 2016 H
    
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
			 "output_muon_2016B_v2.root"               , 
			 "output_muon_2016C.root"                  , 
			 "output_muon_2016D.root"                  , 
			 "output_muon_2016E.root"                  , 
			 "output_muon_2016F.root"                  , 
			 "output_muon_2016G.root"                  , 
			 "output_muon_2016H_v2.root"

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
			   1, //HZG gluglu
			   1, //HZG tth
			   1, //HZG vbf
			   1, //HZG W-
			   1, //HZG W+
			   1, //HZG zh
			   1, //Data 2016 B
			   1, //Data 2016 C
			   1, //Data 2016 D
			   1, //Data 2016 E
			   1, //Data 2016 F
			   1, //Data 2016 G
			   1  //Data 2016 H
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

      ZTauTauHistMaker* selec = new ZTauTauHistMaker();
      selec->fFolderName = fChannel->GetName();
      tree->Process(selec,"");
      TFile* out = new TFile(Form("ztautau_%s_%s.hist",fChannel->GetName(),tree->GetName()),"UPDATE");
      if(out == 0) {
	printf("Unable to find output hist file ztautau_%s_%s.hist, continuing\n",fChannel->GetName(),tree->GetName());
	continue;
      }
      events->Write();
      out->Write();
      delete out;
    }
    delete events;
    delete f;
  }
  return 0;
}
