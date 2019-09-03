
#include "MultileptonNTupleMaker.cc++g"
Int_t process_multileptonNT(int doOrig = 0, int doPtStudyFiles = 0) {


  const char* gridPath[] = {
    //selection with lower trailing pt and no required btag
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190812_092228/", //ttbar
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190304_151029/", //zjets M > 50
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190304_151029/", //zjets 10 < M < 50
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190812_092228/", //single t
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190812_092228/", //single tbar
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190620_122923/", //zjets M > 50
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190620_122923/", //zjets 10 < M < 50
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190812_092228/", //z1jets M > 50
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190812_092228/", //z1jets 10 < M < 50
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190812_092228/", //z2jets M > 50
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190812_092228/", //z2jets 10 < M < 50
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190812_092228/", //z3jets M > 50
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190812_092228/", //z3jets 10 < M < 50
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190812_092228/", //z4jets M > 50
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190812_092228/", //z4jets 10 < M < 50
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190812_092228/", //2016B_v2 Data
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190812_092228/", //signal samples
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190812_092228/", 
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190812_092228/", 
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190812_092228/", 
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190812_092228/"}; 

  //From original selection
  const char* gridPathOrig[] = {
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190816_110742/", //ttbar
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190809_090013/", //zjets M > 50 //not here
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190809_090013/", //zjets 10 < M < 50 //not here
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190816_110742/", //single t
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190816_110742/", //single tbar
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190809_090013/", //zjets M > 50
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190809_090013/", //zjets 10 < M < 50
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190816_110742/", //z1jets M > 50
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190816_110742/", //z1jets 10 < M < 50
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190816_110742/", //z2jets M > 50
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190816_110742/", //z2jets 10 < M < 50
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190816_110742/", //z3jets M > 50
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190816_110742/", //z3jets 10 < M < 50
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190816_110742/", //z4jets M > 50
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190816_110742/", //z4jets 10 < M < 50
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190816_110742/", //2016B_v2 Data
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190816_110742/", //signal samples
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190816_110742/", 
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190816_110742/", 
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190816_110742/", 
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190816_110742/"}; 

  // Original files used
    // "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190304_151029/", //ttbar
    // "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190304_151029/", //zjets M > 50
    // "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190304_151029/", //zjets 10 < M < 50
    // "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190306_083442/", //single t
    // "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190306_083442/", //single tbar
    // "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190313_122403/", //zjets M > 50
    // "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190313_122403/", //zjets 10 < M < 50
    // "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190313_122403/", //z1jets M > 50
    // "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190313_122403/", //z1jets 10 < M < 50
    // "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190313_122403/", //z2jets M > 50
    // "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190313_122403/", //z2jets 10 < M < 50
    // "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190313_122403/", //z3jets M > 50
    // "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190313_122403/", //z3jets 10 < M < 50
    // "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190313_122403/", //z4jets M > 50
    // "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190313_122403/", //z4jets 10 < M < 50
    // "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190307_101513/", //2016B_v2 Data
    // "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190307_163959/", //signal samples
    // "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190307_163959/", 
    // "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190307_163959/", 
    // "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190307_163959/", 
    // "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190307_163959/"}; 

  const char* gridPathPtStudy[] = {
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190318_185111/", //ttbar
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190319_080712/", //single t
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190319_080712/", //single tbar
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190318_185111/", //zjets M > 50
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190318_185111/", //zjets 10 < M < 50
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190318_185111/", //z1jets M > 50
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190318_185111/", //z1jets 10 < M < 50
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190318_185111/", //z2jets M > 50
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190318_185111/", //z2jets 10 < M < 50
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190318_185111/", //z3jets M > 50
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190318_185111/", //z3jets 10 < M < 50
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190318_185111/", //z4jets M > 50
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190318_185111/", //z4jets 10 < M < 50
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190307_101513/", //2016B_v2 Data
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190319_080712/", //signal samples
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190319_080712/", 
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190319_080712/", 
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190319_080712/", 
    "/eos/uscms/store/user/mmackenz/batch_output/mumu_2016_20190319_080712/"}; 

  const char* files[] = {"output_ttbar_inclusive.root"        ,
			 "output_DYJetsToLL_M-50_alt.root"    ,
			 "output_DYJetsToLL_M-10to50_alt.root",
			 "output_T_tW-channel.root"           ,
			 "output_Tbar_tW-channel.root"        ,
			 "output_DYJetsToLL_M-50.root"        ,
			 "output_DYJetsToLL_M-10to50.root"    ,
			 "output_DY1JetsToLL_M-50.root"       ,
			 "output_DY1JetsToLL_M-10to50.root"   ,
			 "output_DY2JetsToLL_M-50.root"       ,
			 "output_DY2JetsToLL_M-10to50.root"   ,
			 "output_DY3JetsToLL_M-50.root"       ,
			 "output_DY3JetsToLL_M-10to50.root"   ,
			 "output_DY4JetsToLL_M-50.root"       ,
			 "output_DY4JetsToLL_M-10to50.root"   ,
			 "output_muon_2016B_v2.root"          ,
			 "output_BB2bbdimu.root"              ,
			 "output_FCNC_MX10.root"              ,
			 "output_FCNC_MX30.root"              ,
			 "output_FCNC_MX60.root"              ,
			 "output_btag_B2bX_t.root"            };

  const char* filesPtStudy[] = {"output_ttbar_inclusive.root"        ,
				"output_T_tW-channel_1.root"           ,
				"output_Tbar_tW-channel_1.root"        ,
				"output_DYJetsToLL_M-50.root"        ,
				"output_DYJetsToLL_M-10to50.root"    ,
				"output_DY1JetsToLL_M-50.root"       ,
				"output_DY1JetsToLL_M-10to50.root"   ,
				"output_DY2JetsToLL_M-50.root"       ,
				"output_DY2JetsToLL_M-10to50.root"   ,
				"output_DY3JetsToLL_M-50.root"       ,
				"output_DY3JetsToLL_M-10to50.root"   ,
				"output_DY4JetsToLL_M-50.root"       ,
				"output_DY4JetsToLL_M-10to50.root"   ,
				"output_muon_2016B_v2.root"          ,
				"output_BB2bbdimu_1.root"              ,
				"output_FCNC_MX10_1.root"              ,
				"output_FCNC_MX30_1.root"              ,
				"output_FCNC_MX60_1.root"              ,
				"output_btag_B2bX_t.root"            };
  const int doProcess[] = {1, //ttbar
			   0, //DY AMC
			   0, //DY AMC
			   1, //t_tw
			   1, //tbar_tw
			   0, //DY MadGraph
			   0, //DY MadGraph
			   1, //DY 1 Jet MadGraph
			   1, //DY 1 Jet MadGraph
			   1, //DY 2 Jet MadGraph
			   1, //DY 2 Jet MadGraph
			   1, //DY 3 Jet MadGraph
			   1, //DY 3 Jet MadGraph
			   1, //DY 4 Jet MadGraph
			   1, //DY 4 Jet MadGraph
			   1, //2016B2
			   1, //bb2bb
			   1, //fcnc 10
			   1, //fcnc 30
			   1, //fcnc 60
			   1}; //b2bx_t
  
  for(int i = 0; i < sizeof(files)/sizeof(*files); ++i) {
    if(doPtStudyFiles == 0 && !doProcess[i]) continue;
    if(i >= sizeof(filesPtStudy)/sizeof(*filesPtStudy)
       && doPtStudyFiles > 0) break;
    const char* c = (doPtStudyFiles == 0) ? files[i] : filesPtStudy[i];
    TFile* f = new TFile(Form("%s%s",
			      (doPtStudyFiles == 0) ? ((doOrig == 0) ? gridPath[i] : gridPathOrig[i]) : gridPathPtStudy[i],c));
    if(f == 0) {
      printf("File %s%s not found, continuing\n",
	     (doPtStudyFiles == 0) ? ((doOrig == 0) ? gridPath[i] : gridPathOrig[i]) : gridPathPtStudy[i],c);
      continue;
    }
    printf("using %s%s\n",(doPtStudyFiles == 0) ? ((doOrig == 0) ? gridPath[i] : gridPathOrig[i]) : gridPathPtStudy[i],c);
    TTree* tree = 0;
    TH1F* events = 0;
    TKey* key = 0;
    TIter nextkey(f->GetListOfKeys());
    while((key = (TKey*)nextkey())) {
      TObject* obj = key->ReadObj();
      if(obj->IsA()->InheritsFrom(TTree::Class())) tree = (TTree*)obj;
      if(obj->IsA()->InheritsFrom(TH1::Class())) events = (TH1F*)obj;
    }
    if(tree == 0) {
      printf("Tree in %s%s not found, continuing\n",((doOrig == 0) ? gridPath[i] : gridPathOrig[i]),c);
      continue;
    }
    if(events == 0) {
      printf("Events Histogram in %s%s not found, continuing\n",((doOrig == 0) ? gridPath[i] : gridPathOrig[i]),c);
      continue;
    }

    MultileptonNTupleMaker* selec = new MultileptonNTupleMaker();
    if(doOrig > 0) selec->fEnd = "_orig";
    tree->Process(selec,"");
    TFile* out = new TFile(Form("%s%s.tree",tree->GetName(), (doOrig == 0) ? "" : "_orig"),"UPDATE");
    if(out == 0) {
      printf("Unable to find output tree file %s.tree, continuing\n",tree->GetName());
      continue;
    }
    events->Write();
    out->Write();
    
    if(doPtStudyFiles > 0 ) {
      int res = rename(Form("%s.tree",tree->GetName()),Form("%s.ptstudy_tree",tree->GetName()));
      if(res == 0) printf("Renamed file to %s.ptstudy_tree\n",tree->GetName());
      else         printf("Failed to rename file to %s.ptstudy_tree\n",tree->GetName());
    }

    delete tree;
    delete events;
    delete out;
    delete f;
  }
  return 0;
}
