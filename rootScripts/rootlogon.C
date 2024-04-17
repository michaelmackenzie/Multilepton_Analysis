//------------------------------------------------------------------------------
//  rootlogon.C: a sample ROOT logon macro allowing use of ROOT script
//               compiler in CDF RunII environment. The name of this macro file
//               is defined by the .rootrc file
//------------------------------------------------------------------------------
{
  // the line below tells rootcling where to look for
  // the include files

  gInterpreter->AddIncludePath("./");
  gInterpreter->AddIncludePath("./include");
  gInterpreter->AddIncludePath(Form("%s/include",gSystem->Getenv("ROOTSYS")));
  TString hostname = gSystem->Getenv("HOSTNAME");

  const char* exec_name = gApplication->Argv(0);

  if (exec_name) {
    if (strcmp(exec_name,"root.exe") == 0) {
      gInterpreter->AddIncludePath("../rootScripts");
      gInterpreter->AddIncludePath("../interface");
      gInterpreter->AddIncludePath("../scale_factors");
      gInterpreter->AddIncludePath("../weights");
                                        // print overflows/underflows in the stat box
      gStyle->SetOptStat(11111111);
                                        // print fit results in the stat box
      gStyle->SetOptFit(1110);
      TArrow::SetDefaultArrowSize(0.015);

      //-----------------------------------------------------------------------------
      // report the process ID which simplifies debugging
      //-----------------------------------------------------------------------------
      printf("process ID: %i\n",gSystem->GetPid());
      TAuthenticate::SetGlobalUser(gSystem->Getenv("USER"));
      gInterpreter->ProcessLine(".! ps | grep root");

      //-----------------------------------------------------------------------------
      // Load XGBoost and CLFVAnalysis libraries
      //-----------------------------------------------------------------------------

      TString cmssw = gSystem->Getenv("CMSSW_BASE");

      printf("Loading xgboost.so!\n");
      if(cmssw.Contains("CMSSW_10_")) {
        gSystem->Load(Form("/cvmfs/cms.cern.ch/%s/external/py2-xgboost/0.80-ikaegh/lib/python2.7/site-packages/xgboost/lib/libxgboost.so",
                           gSystem->Getenv("SCRAM_ARCH")));
      } else if(cmssw.Contains("CMSSW_11_")) {
        gSystem->Load(Form("/cvmfs/cms.cern.ch/%s/external/py2-xgboost/0.82-llifpc/lib/python2.7/site-packages/xgboost/lib/libxgboost.so",
                           gSystem->Getenv("SCRAM_ARCH")));
      } else {
        cout << "Can't identify an XGBoost library path for CMSSW release in path " << cmssw.Data() << endl;
      }

      printf("Loading lib/libCLFVAnalysis.so!\n");
      TString path = (hostname.Contains("cms")) ? "src/CLFVAnalysis/lib/" : gSystem->Getenv("PWD");
      if(TString(gSystem->Getenv("PWD")).Contains("CLFVAnalysis_dev")) {
        path = "src/CLFVAnalysis_dev/lib/";
      }
      if(!hostname.Contains("cms")) {
        path += "/../lib/";
      } else {
        gSystem->AddDynamicPath(cmssw.Data());
        gSystem->AddDynamicPath(gSystem->Getenv("PWD"));
      }
      int status = gSystem->Load((path + "libCLFVAnalysis.so").Data());
      if(status < 0)
        status = gSystem->Load("../lib/libCLFVAnalysis.so");
      if(status < 0)
        status = gSystem->Load("../../src/CLFVAnalysis/lib/libCLFVAnalysis.so");
      if(status < 0)
        status = gSystem->Load("src/CLFVAnalysis/lib/libCLFVAnalysis.so");
      if(status < 0)
        status = gSystem->Load("libCLFVAnalysis.so");

    }
  }
}
