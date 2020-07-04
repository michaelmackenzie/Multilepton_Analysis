
#include "ZTauTauHistMaker.cc+g"
Int_t process_ztautau() {

  TString hostname = gSystem->Getenv("HOSTNAME");
  TString gridPath = (hostname.Contains("cmslpc")) ? "root://cmseos.fnal.gov//store/user/mmackenz/ztautau_trees/"
    : "/mnt/data/mmackenz/ztautau_trees/";

  
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
			 "output_WJetsToLNu.root"                  ,
			 "output_WJetsToLNu_ext1.root"             ,
			 "output_WJetsToLNu_ext2.root"             ,
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
			 "output_zemu.root"                        ,
			 "output_hetau.root"                       ,
			 "output_hmutau.root"                      ,
			 "output_hemu.root"                        ,
			 "output_qcd_ht50to100.root"               ,
			 "output_qcd_ht100to200.root"              ,
			 "output_qcd_ht200to300.root"              ,
			 "output_qcd_ht300to500.root"              ,
			 "output_qcd_ht500to700.root"              ,
			 "output_qcd_ht700to1000.root"             ,
			 "output_qcd_ht1000to1500.root"            ,
			 "output_qcd_ht1500to2000.root"            ,
			 "output_qcd_ht2000toinf.root"             ,
			 "output_muon_2016B_v2.root"               , 
			 "output_muon_2016C.root"                  , 
			 "output_muon_2016D.root"                  , 
			 "output_muon_2016E.root"                  , 
			 "output_muon_2016F.root"                  , 
			 "output_muon_2016G.root"                  , 
			 "output_muon_2016H_v2.root"               ,
			 "output_electron_2016B_v2.root"           , 
			 "output_electron_2016C.root"              , 
			 "output_electron_2016D.root"              , 
			 "output_electron_2016E.root"              , 
			 "output_electron_2016F.root"              , 
			 "output_electron_2016G.root"              , 
			 "output_electron_2016H_v2.root"

  };
  const int doProcess[] = {1, //ttbar
			   1, //DY AMC
			   1, //DY AMC
			   1, //t_tw
			   1, //tbar_tw
			   0, //DY inclusive M > 50 MadGraph
			   0, //DY inclusive 10 < M < 50 MadGraph
			   0, //DY 1 Jet MadGraph
			   0, //DY 1 Jet MadGraph
			   0, //DY 2 Jet MadGraph
			   0, //DY 2 Jet MadGraph
			   0, //DY 3 Jet MadGraph
			   0, //DY 3 Jet MadGraph
			   0, //DY 4 Jet MadGraph
			   0, //DY 4 Jet MadGraph
			   1, //W 1 Jet MadGraph
			   1, //W 2 Jet MadGraph
			   1, //W 3 Jet MadGraph
			   1, //W 4 Jet MadGraph
			   0, //WJets amcnlo
			   0, //WJets amcnlo ext 1
			   0, //WJets amcnlo ext 2
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
			   1, //HTauTau gluglu
			   1, //ZETau
			   1, //ZMuTau
			   1, //ZEMu
			   1, //HETau
			   1, //HMuTau
			   1, //HEMu
			   1, //qcd ht   50 to  100
			   1, //qcd ht  100 to  200
			   1, //qcd ht  200 to  300
			   1, //qcd ht  300 to  500
			   1, //qcd ht  500 to  700
			   1, //qcd ht  700 to 1000
			   1, //qcd ht 1000 to 1500
			   1, //qcd ht 1500 to 2000
			   1, //qcd ht 2000 to inf
			   1, //Muon Data 2016B
			   1, //Muon Data 2016C
			   1, //Muon Data 2016D
			   1, //Muon Data 2016E
			   1, //Muon Data 2016F
			   1, //Muon Data 2016G
			   1, //Muon Data 2016H
			   1, //Electron Data 2016B
			   1, //Electron Data 2016C
			   1, //Electron Data 2016D
			   1, //Electron Data 2016E
			   1, //Electron Data 2016F
			   1, //Electron Data 2016G
			   1  //Electron Data 2016H
  };
  
  Double_t xsec[100];
  for(int i = 0; i < sizeof(xsec)/sizeof(*xsec); ++i)
    xsec[i] = 1.; //no scaling if not given
  
  //Taken from https://twiki.cern.ch/twiki/bin/viewauth/CMS/SummaryTable1G25ns     
  xsec[0]  =  831.76;                 //"ttbar_inclusive"         
  xsec[1]  =  6225.42;                //"zjets_m-50_amcatnlo"     
  xsec[2]  =  18610. ;                //"zjets_m-10to50_amcatnlo" 
  xsec[3]  =  35.85;	              //"t_tw"                    
  xsec[4]  =  35.85;	              //"tbar_tw"                 
  xsec[5]  =  6225.42;                //"zjets_m-50_amcatnlo"     
  xsec[6]  =  18610. ;                //"zjets_m-10to50_amcatnlo" 
  xsec[7]  =  1198.9;	               //"z1jets_m-50"             
  xsec[8] =  855.5;		       //"z1jets_m-10to50"         
  xsec[9] =  390.6;		       //"z2jets_m-50"             
  xsec[10] =  466.1;		       //"z2jets_m-10to50"         
  xsec[11] =  113.3;		       //"z3jets_m-50"             
  xsec[12] =  114.5;		       //"z3jets_m-10to50"         
  xsec[13] =  60.2;		       //"z4jets_m-50"             
  xsec[14] =  36.4;		       //"z4jets_m-10to50"         
  xsec[15] =  11486.53;		       //"w1jets"                  
  xsec[16] =  3775.2; 		       //"w2jets"                  
  xsec[17] =  1139.82; 		       //"w3jets"                  
  xsec[18] =  655.82; 		       //"w4jets"                  
  xsec[19] =  61526.7; 		       //"wjets"    https://twiki.cern.ch/twiki/bin/viewauth/CMS/SummaryTable1G25ns#W_jets    
  xsec[20] =  61526.7; 		       //"wjets"    https://twiki.cern.ch/twiki/bin/viewauth/CMS/SummaryTable1G25ns#W_jets    
  xsec[21] =  61526.7; 		       //"wjets"    https://twiki.cern.ch/twiki/bin/viewauth/CMS/SummaryTable1G25ns#W_jets    
  xsec[22] =  12.178;	               //"ww"                      
  xsec[23] =  5.595;		       //"wz_2l2q"                 
  xsec[24] =  4.42965;	               //"wz_3lnu"                 
  xsec[25] =  0.564;		       //"zz_2l2nu"                
  xsec[26] =  3.22;		       //"zz_2l2q"                 
  xsec[27] =  1.212;		       //"zz_4l"
  //Higgs branching ratios: https://twiki.cern.ch/twiki/bin/view/LHCPhysics/CERNYellowReportPageBR2014
  //Higgs production xsecs: https://twiki.cern.ch/twiki/bin/view/LHCPhysics/CERNHLHE2019
  //Z decay to leptons fraction: http://pdg.lbl.gov/2012/listings/rpp2012-list-z-boson.pdf
  // --> Br(Z->ll) * Br(h->Zg) * xsec(X -> h)
  xsec[28] =    3.*3.3658/100.*1.54e-3* 48.61;	       //"hzg_gluglu"              
  xsec[29] =    3.*3.3658/100.*1.54e-3* 0.5071;	       //"hzg_tth"                 
  xsec[30] =    3.*3.3658/100.*1.54e-3* 3.766;	       //"hzg_vbf"                 
  xsec[31] =    3.*3.3658/100.*1.54e-3* 0.527;         //"hzg_wminus"              
  xsec[32] =    3.*3.3658/100.*1.54e-3* 0.831;         //"hzg_wplus"               
  xsec[33] =    3.*3.3658/100.*1.54e-3* 0.880;	       //"hzg_zh"                  
  xsec[34] =                   6.32e-2* 43.92;	       //"htautau_gluglu"                  
  xsec[35] = ((6225.42+18610.)/(3.*3.3658e-2))*9.8e-6*161497./(2.e3*498); //zetau  z->ll / br(ll) * br(etau, CL=95) *N(accepted)/N(Gen) http://pdg.lbl.gov/2018/listings/rpp2018-list-z-boson.pdf
  xsec[36] = ((6225.42+18610.)/(3.*3.3658e-2))*1.2e-5*152959./(2.e3*497); //zmutau z->ll / br(ll) * br(mutau, CL=95)*N(accepted)/N(Gen) http://pdg.lbl.gov/2018/listings/rpp2018-list-z-boson.pdf
  xsec[37] = ((6225.42+18610.)/(3.*3.3658e-2))*7.3e-7*186670./(2.e3*596); //zemu   z->ll / br(ll) * br(emu, CL=95)  *N(accepted)/N(Gen) http://pdg.lbl.gov/2018/listings/rpp2018-list-z-boson.pdf
  xsec[38] = (48.61+3.766+0.5071+1.358+0.880)*6.1e-3*418794./(487.*1e3);  //hetau  xsec(higgs,glu+vbf)*br(etau, CL=95) *N(accepted)/N(Gen) http://pdg.lbl.gov/2019/listings/rpp2019-list-higgs-boson.pdf
  xsec[39] = (48.61+3.766+0.5071+1.358+0.880)*2.5e-3*388243./(453.*1e3);  //hmutau xsec(higgs,glu+vbf)*br(mutau, CL=95)*N(accepted)/N(Gen) http://pdg.lbl.gov/2019/listings/rpp2019-list-higgs-boson.pdf
  xsec[40] = (48.61+3.766+0.5071+1.358+0.880)*3.5e-4*34429./(88.*500);    //hemu   xsec(higgs,glu+vbf)*br(emu, CL=95)  *N(accepted)/N(Gen) http://pdg.lbl.gov/2019/listings/rpp2019-list-higgs-boson.pdf

  TStopwatch* timer = new TStopwatch();

  Int_t useTauFakeSF = 1; //1 = use given scale factors, 2 = override them with local ones
  bool writeTrees = true;
  TString onlyChannel = "llg_study";
  vector<TString> skipChannels = {"mumu", "ee", "all", "jets"};
  bool reProcessMVAs = false;
  Int_t removeZPtWeights = 0;
  float signalTrainFraction = 0.7;
  float backgroundTrainFraction = 0.3;
  cout << "--- Fake Tau SF mode: " << useTauFakeSF
       << ", Write Trees mode: " << writeTrees
       << ", Remove z pt weights: " << removeZPtWeights
       << ", training fractions: signal = " << signalTrainFraction
       << ", background = " << backgroundTrainFraction
       << endl;
  if(skipChannels.size() > 0) {
    cout << "--- Skipping channels: ";
    for(TString channel : skipChannels)
      cout << channel.Data() << ", ";
    cout << endl;
  }
  if(onlyChannel != "")
    cout << "--- WARNING! Only processing " << onlyChannel.Data() << " channel!\n";
  
  Int_t category = 0;
  for(int i = 0; i < sizeof(files)/sizeof(*files); ++i) {
    ++category; // could have just used i + 1?
    cout << "Loop " << i << ", category " << category
	 << ", file " << files[i] << ", xsec " << xsec[i]
	 << ", doProcess " << doProcess[i] << endl;
    if(!doProcess[i]) continue;
    const char* c = files[i];
    TFile* f = TFile::Open((gridPath+c).Data(),"READ");
    if(f == 0) {
      printf("File %s not found, continuing\n",(gridPath + c).Data());
      continue;
    }
    printf("using %s\n",(gridPath+c).Data());
    TDirectoryFile* fChannel = 0;
    TH1F* events = 0;
    TKey* key = 0;
    TIter nextkey(f->GetListOfKeys());

    //for splitting DY lepton decays
    TString cString = c;
    bool isDY = (cString.Contains("DY"));
    bool isSignal = cString.Contains("mutau") || cString.Contains("etau") || cString.Contains("emu");
    //for avoiding double counting data events
    bool isElectronData = cString.Contains("output_electron_");
    bool isMuonData = cString.Contains("output_muon_");
    
    //get events histograms first
    while((key = (TKey*)nextkey())) {
      TObject* obj = key->ReadObj();
      if(obj->IsA()->InheritsFrom(TH1::Class())) events = (TH1F*)obj;
    }
    if(events == 0) {
      printf("Events Histogram in %s not found, continuing\n",(gridPath+c).Data());
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
      TString currentChannel = fChannel->GetName();
      //check if only suppose to do 1 channel, and if this is that channel
      if(onlyChannel != "") {
	if(onlyChannel != currentChannel) { cout << "Continuing!\n"; continue;}
	else cout << "Found correct channel --> processing!\n";
      }
      //check if this channel is part of the skip list
      if(skipChannels.size() > 0) {
	bool skip = false;
	for(TString channel : skipChannels) {
	  if(channel == currentChannel) {cout << "Skipping channel!\n"; skip=true;}
	}
	if(skip) continue;
      }
      //skip if data on a channel without that trigger used
      if(isElectronData && (currentChannel == "mutau" || currentChannel == "mumu")) {
	cout << "Electron data on muon only channel, continuing!\n"; continue;
      }
      if(isMuonData && (currentChannel == "etau" || currentChannel == "ee")) {
	cout << "Muon data on electron only channel, continuing!\n"; continue;
      }
      TTree* tree = 0;
      TH1F* eventsChannel = 0;
      TKey* key2 = 0;
      TIter nextkey2(fChannel->GetListOfKeys());
      //get events tree and events counting histogram
      while((key2 = (TKey*)nextkey2())) {
	TObject* obj2 = key2->ReadObj(); 
	if(obj2->IsA()->InheritsFrom(TH1::Class())) eventsChannel = (TH1F*)obj2;
	if(obj2->IsA()->InheritsFrom(TTree::Class())) {
	  auto tmptree = (TTree*)obj2;
	  //take the latest tree iteration from the file in case of copies
	  if(tree) tree = (tmptree->GetEntriesFast() > tree->GetEntriesFast()) ? tmptree : tree;
	  else tree = tmptree;
	}
      }
      //check that these are found
      if(tree == 0) {
	printf("Tree in %s/%s not found, continuing\n",(gridPath+c).Data(),fChannel->GetName());
	continue;
      }
      if(eventsChannel == 0) {
	printf("Events Channel Histogram in %s/%s not found, continuing\n",(gridPath+c).Data(),fChannel->GetName());
	continue;
      }
      // if Drell-Yan, loop through it twice, doing tautau then ee/mumu
      int nloops = (isDY) ? 2 : 1;
      for(int loop = 1; loop <= nloops; ++loop) {
	ZTauTauHistMaker* selec = new ZTauTauHistMaker(); //selector
	TString cString = c;
	selec->fFolderName = fChannel->GetName();
	if(isDY) selec->fDYType = loop; //if Drell-Yan, tell the selector which loop we're on
	//skip electron data events with both triggers for e+mu channel, to not double count
	selec->fIsData = 2*isMuonData + isElectronData; 
	selec->fSkipDoubleTrigger = (isElectronData && (selec->fFolderName == "emu" || selec->fFolderName == "llg_study"));	
	//store a label for this dataset
	selec->fEventCategory = category;
	selec->fWriteTrees = selec->fIsData == 0 && writeTrees; //don't write trees for data
	selec->fUseTauFakeSF = useTauFakeSF; //whether or not to use fake tau weights from analyzer/locally re-defined
	selec->fXsec = xsec[i]/(events->GetBinContent(1) - 2.*events->GetBinContent(10)); //for writing trees with correct normalization
	selec->fRemoveZPtWeights = removeZPtWeights; //whether or not to re-weight Z pT
	selec->fFractionMVA = (isSignal) ? signalTrainFraction : backgroundTrainFraction; //fraction of events to use for MVA training
	if(isMuonData || isElectronData) selec->fFractionMVA = 0.; //don't split off data for training
	selec->fReprocessMVAs = reProcessMVAs; //whether to evaluate the MVAs or use defined ones from the trees
	
	tree->Process(selec,""); //run the selector over the tree

	//open back up the file
	TFile* out = new TFile(Form("ztautau_%s%s_%s.hist",fChannel->GetName(), 
				    (isDY) ? Form("_%i",loop) : "", tree->GetName()),"UPDATE");
	if(out == 0) {
	  printf("Unable to find output hist file ztautau_%s%s_%s.hist, continuing\n",fChannel->GetName(),
		 (isDY) ? Form("_%i",loop) : "", tree->GetName());
	  continue;
	}
	//add the events histogram to the output
	events->Write();
	out->Write();
	delete out;
      }
    }
    delete events;
    delete f;
  }
  //report the time spent histogramming
  Double_t cpuTime = timer->CpuTime();
  Double_t realTime = timer->RealTime();
  printf("Processing time: %7.2fs CPU time %7.2fs Wall time\n",cpuTime,realTime);
  if(realTime > 600. ) printf("Processing time: %7.2fmin CPU time %7.2fmin Wall time\n",cpuTime/60.,realTime/60.);

  return 0;
}
