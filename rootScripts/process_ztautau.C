
//Data formats
#include "../dataFormats/SlimObject_t.hh+g"
#include "../dataFormats/SlimElectron_t.hh+g"
#include "../dataFormats/SlimMuon_t.hh+g"
#include "../dataFormats/SlimTau_t.hh+g"
#include "../dataFormats/SlimJet_t.hh+g"
#include "../dataFormats/SlimPhoton_t.hh+g"
#include "../dataFormats/Tree_t.hh+g"

//Utilities
#include "../utils/TrkQualInit.cc+g"

//Histogrammer
#include "ZTauTauHistMaker.cc+g"

struct datacard_t {
  bool process_;
  double xsec_;
  TString fname_;
  datacard_t(bool process, double xsec, TString fname) :
    process_(process), xsec_(xsec), fname_(fname) {}
};
  
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
			   0, //qcd ht   50 to  100
			   0, //qcd ht  100 to  200
			   0, //qcd ht  200 to  300
			   0, //qcd ht  300 to  500
			   0, //qcd ht  500 to  700
			   0, //qcd ht  700 to 1000
			   0, //qcd ht 1000 to 1500
			   0, //qcd ht 1500 to 2000
			   0, //qcd ht 2000 to inf
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
  xsec[3]  =  35.85;                  //"t_tw"                    
  xsec[4]  =  35.85;                  //"tbar_tw"                 
  xsec[5]  =  6225.42;                //"zjets_m-50_amcatnlo"     
  xsec[6]  =  18610. ;                //"zjets_m-10to50_amcatnlo" 
  xsec[7]  =  1198.9;                 //"z1jets_m-50"             
  xsec[8] =  855.5;                   //"z1jets_m-10to50"         
  xsec[9] =  390.6;                   //"z2jets_m-50"             
  xsec[10] =  466.1;                  //"z2jets_m-10to50"         
  xsec[11] =  113.3;                  //"z3jets_m-50"             
  xsec[12] =  114.5;                  //"z3jets_m-10to50"         
  xsec[13] =  60.2;                   //"z4jets_m-50"             
  xsec[14] =  36.4;                   //"z4jets_m-10to50"         
  xsec[15] =  11486.53;               //"w1jets"                  
  xsec[16] =  3775.2;                 //"w2jets"                  
  xsec[17] =  1139.82;                //"w3jets"                  
  xsec[18] =  655.82;                 //"w4jets"                  
  xsec[19] =  61526.7;                        //"wjets"    https://twiki.cern.ch/twiki/bin/viewauth/CMS/SummaryTable1G25ns#W_jets    
  xsec[20] =  61526.7;                        //"wjets"    https://twiki.cern.ch/twiki/bin/viewauth/CMS/SummaryTable1G25ns#W_jets    
  xsec[21] =  61526.7;                        //"wjets"    https://twiki.cern.ch/twiki/bin/viewauth/CMS/SummaryTable1G25ns#W_jets    
  xsec[22] =  12.178;                 //"ww"                      
  xsec[23] =  5.595;                  //"wz_2l2q"                 
  xsec[24] =  4.42965;                //"wz_3lnu"                 
  xsec[25] =  0.564;                  //"zz_2l2nu"                
  xsec[26] =  3.22;                   //"zz_2l2q"                 
  xsec[27] =  1.212;                  //"zz_4l"
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

  /******************************/
  /* Define NANO AOD processing */
  /******************************/
  //"/uscms/home/mmackenz/nobackup/ZEMu/CMSSW_10_2_18/src/StandardModel/CLFVAnalysis/rootScripts/";
  TString nanoaod_path = gridPath;
  nanoaod_path.ReplaceAll("ztautau_trees", "ztautau_nanoaod_test_trees"); //use the same path, but replace the search directory

  vector<datacard_t> nanocards;
  //2016
  nanocards.push_back(datacard_t(true , 365.34                                  , "clfv_2016_ttbarToSemiLeptonic.tree"     )); //1
  nanocards.push_back(datacard_t(true , 88.29                                   , "clfv_2016_ttbarlnu.tree"                )); //2
  nanocards.push_back(datacard_t(true , 6225.42                                 , "clfv_2016_DY50.tree"                    )); //3
  nanocards.push_back(datacard_t(true , 34.91                                   , "clfv_2016_SingleAntiToptW.tree"         )); //4
  nanocards.push_back(datacard_t(true , 34.91                                   , "clfv_2016_SingleToptW.tree"             )); //5
  nanocards.push_back(datacard_t(true , 52850.0                                 , "clfv_2016_Wlnu.tree"                    )); //6
  nanocards.push_back(datacard_t(true , 12.178                                  , "clfv_2016_WW.tree"                      )); //7
  nanocards.push_back(datacard_t(true , 27.6                                    , "clfv_2016_WZ.tree"                      )); //8
  nanocards.push_back(datacard_t(true , ((6225.42)/(3.*3.3658e-2))*9.8e-6       , "clfv_2016_ZETau.tree"                   )); //9
  nanocards.push_back(datacard_t(true , ((6225.42)/(3.*3.3658e-2))*1.2e-5       , "clfv_2016_ZMuTau.tree"                  )); //10
  nanocards.push_back(datacard_t(true , 2075.14/0.0337*7.3e-7                   , "clfv_2016_ZEMu.tree"                    )); //11
  nanocards.push_back(datacard_t(true , (48.61+3.766+0.5071+1.358+0.880)*6.1e-3 , "clfv_2016_HETau.tree"                   )); //12
  nanocards.push_back(datacard_t(true , (48.61+3.766+0.5071+1.358+0.880)*2.5e-3 , "clfv_2016_HMuTau.tree"                  )); //13
  nanocards.push_back(datacard_t(true , (48.61+3.766+0.5071+1.358+0.880)*3.5e-4 , "clfv_2016_HEMu.tree"                    )); //14
  nanocards.push_back(datacard_t(true , 1.                                      , "clfv_2016_SingleMu.tree"                )); //15
  nanocards.push_back(datacard_t(true , 1.                                      , "clfv_2016_SingleEle.tree"               )); //16
  nanocards.push_back(datacard_t(true , 12.14                                   , "clfv_2016_ZZ.tree"                      )); //17
  nanocards.push_back(datacard_t(true , 0.2086                                  , "clfv_2016_WWW.tree"                     )); //18
  nanocards.push_back(datacard_t(true , 22180.                                  , "clfv_2016_QCDDoubleEMEnrich30to40.tree" )); //19
  nanocards.push_back(datacard_t(true , 247000.                                 , "clfv_2016_QCDDoubleEMEnrich30toInf.tree")); //20
  nanocards.push_back(datacard_t(true , 113100.                                 , "clfv_2016_QCDDoubleEMEnrich40toInf.tree")); //21
  //2017
  nanocards.push_back(datacard_t(false, 365.34                                  , "clfv_2017_ttbarToSemiLeptonic.tree")); //
  nanocards.push_back(datacard_t(false, 88.29                                   , "clfv_2017_ttbarlnu.tree"           )); //
  nanocards.push_back(datacard_t(false, 6225.42                                 , "clfv_2017_DY50.tree"               )); //
  nanocards.push_back(datacard_t(false, 34.91                                   , "clfv_2017_SingleAntiToptW.tree"    )); //
  nanocards.push_back(datacard_t(false, 34.91                                   , "clfv_2017_SingleToptW.tree"        )); //
  nanocards.push_back(datacard_t(false, 52850.0                                 , "clfv_2017_Wlnu.tree"               )); //
  nanocards.push_back(datacard_t(false, 12.178                                  , "clfv_2017_WW.tree"                 )); //
  nanocards.push_back(datacard_t(false, 27.6                                    , "clfv_2017_WZ.tree"                 )); //
  nanocards.push_back(datacard_t(false, 2075.14/0.0337*7.3e-7                   , "clfv_2017_Signal.tree"             )); //
  nanocards.push_back(datacard_t(false, 1.                                      , "clfv_2017_SingleMu.tree"           )); //
  nanocards.push_back(datacard_t(false, 1.                                      , "clfv_2017_SingleEle.tree"          )); //
  //2018
  nanocards.push_back(datacard_t(false, 365.34                                  , "clfv_2018_ttbarToSemiLeptonic.tree")); //
  nanocards.push_back(datacard_t(false, 88.29                                   , "clfv_2018_ttbarlnu.tree"           )); //
  nanocards.push_back(datacard_t(false, 6225.42                                 , "clfv_2018_DY50.tree"               )); //
  nanocards.push_back(datacard_t(false, 34.91                                   , "clfv_2018_SingleAntiToptW.tree"    )); //
  nanocards.push_back(datacard_t(false, 34.91                                   , "clfv_2018_SingleToptW.tree"        )); //
  nanocards.push_back(datacard_t(false, 52850.0                                 , "clfv_2018_Wlnu.tree"               )); //
  nanocards.push_back(datacard_t(false, 12.178                                  , "clfv_2018_WW.tree"                 )); //
  nanocards.push_back(datacard_t(false, 27.6                                    , "clfv_2018_WZ.tree"                 )); //
  nanocards.push_back(datacard_t(false, 2075.14/0.0337*7.3e-7                   , "clfv_2018_Signal.tree"             )); //
  nanocards.push_back(datacard_t(false, 1.                                      , "clfv_2018_SingleMu.tree"           )); //
  nanocards.push_back(datacard_t(false, 1.                                      , "clfv_2018_SingleEle.tree"          )); //


  TStopwatch* timer = new TStopwatch();

  Int_t useNanoAods  = -1; // 1 = use, 0 = don't use, -1 = only use
  Int_t useTauFakeSF = 1; //1 = use given scale factors, 2 = override them with local ones
  bool writeTrees = true;
  TString onlyChannel = "";
  vector<TString> skipChannels = {"mumu", "ee", "all", "jets", "llg_study"};

  bool reProcessMVAs = false;
  Int_t removeZPtWeights = 0;
  float signalTrainFraction = 0.7;
  float backgroundTrainFraction = 0.3;
  cout << "--- Fake Tau SF mode: " << useTauFakeSF
       << ", Write Trees mode: " << writeTrees
       << ", Remove z pt weights: " << removeZPtWeights
       << ", training fractions: signal = " << signalTrainFraction
       << ", background = " << backgroundTrainFraction
       << endl << "--- Use NanoAOD = " << useNanoAods << endl;
  if(skipChannels.size() > 0) {
    cout << "--- Skipping channels: ";
    for(TString channel : skipChannels)
      cout << channel.Data() << ", ";
    cout << endl;
  }
  if(onlyChannel != "")
    cout << "--- WARNING! Only processing " << onlyChannel.Data() << " channel!\n";

  //loop over data formats, 0 = bltTrees from ZTauTauAnalyzer, 1 = trees from converting Nano AODS
  for(int dataform = 0; dataform < 2; ++dataform) {
    if(useNanoAods < 0 && dataform == 0) continue;
    if(useNanoAods == 0 && dataform != 0) break;
    Int_t category = 0;
    bool isnano = (dataform == 1);
    unsigned nfiles = (!isnano) ? sizeof(files)/sizeof(*files) : nanocards.size();
    for(unsigned i = 0; i < nfiles; ++i) {
      ++category; // could have just used i + 1?
      cout << "Loop " << i << ", category " << category
	   << ", file " << ((!isnano) ? files[i] : nanocards[i].fname_.Data())
	   << ", xsec " << ((!isnano) ? xsec[i] : nanocards[i].xsec_)
	   << ", doProcess " << ((!isnano) ? doProcess[i] : nanocards[i].process_) << endl;
      if((isnano && !nanocards[i].process_) || (!isnano && !doProcess[i])) continue;
      const char* c = (!isnano) ? files[i] : nanocards[i].fname_.Data();
      TString filepath = (!isnano) ? gridPath + c : nanoaod_path + c;
      TFile* f = TFile::Open((filepath).Data(),"READ");
      if(f == 0) {
	printf("File %s not found, continuing\n",(filepath).Data());
	continue;
      }
      printf("using %s\n",(filepath).Data());
      TDirectoryFile* fChannel = 0;
      TH1F* events = 0;
      TKey* key = 0;
      TIter nextkey(f->GetListOfKeys());

    //for splitting DY lepton decays
      TString cString = c;
      bool isDY = !isnano && (cString.Contains("DY"));
      bool isSignal = cString.Contains("mutau") || cString.Contains("etau") || cString.Contains("emu") || cString.Contains("Signal");
    //for avoiding double counting data events
      bool isElectronData = cString.Contains("output_electron_") || cString.Contains("SingleEle");
      bool isMuonData = cString.Contains("output_muon_") || cString.Contains("SingleMu");

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
	printf("Events Channel Histogram in %s/%s not found, continuing processing without it\n",(gridPath+c).Data(),fChannel->GetName());
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
	selec->fXsec = (isnano) ? nanocards[i].xsec_ : xsec[i];
	selec->fXsec /= (events->GetBinContent(1) - 2.*events->GetBinContent(10)); //for writing trees with correct normalization
	selec->fRemoveZPtWeights = removeZPtWeights; //whether or not to re-weight Z pT
	selec->fFractionMVA = (isSignal) ? signalTrainFraction : backgroundTrainFraction; //fraction of events to use for MVA training
	if(isMuonData || isElectronData) selec->fFractionMVA = 0.; //don't split off data for training
	selec->fReprocessMVAs = reProcessMVAs; //whether to evaluate the MVAs or use defined ones from the trees
	selec->fIsNano = isnano;

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
    } //end file loop
  } //end data format loop
  //report the time spent histogramming
  Double_t cpuTime = timer->CpuTime();
  Double_t realTime = timer->RealTime();
  printf("Processing time: %7.2fs CPU time %7.2fs Wall time\n",cpuTime,realTime);
  if(realTime > 600. ) printf("Processing time: %7.2fmin CPU time %7.2fmin Wall time\n",cpuTime/60.,realTime/60.);

  return 0;
}
