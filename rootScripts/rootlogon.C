//------------------------------------------------------------------------------
//  rootlogon.C: a sample ROOT logon macro allowing use of ROOT script 
//               compiler in CDF RunII environment. The name of this macro file
//               is defined by the .rootrc file
//------------------------------------------------------------------------------
{
                                // the line below tells rootcling where to look for 
				// the include files

  gInterpreter->AddIncludePath("./");
  gInterpreter->AddIncludePath("../rootScripts");
  gInterpreter->AddIncludePath("./include");
  // gInterpreter->AddIncludePath(gSystem->Getenv("CLHEP_INC"));
  gInterpreter->AddIncludePath(Form("%s/include",gSystem->Getenv("ROOTSYS")));
  TString hostname = gSystem->Getenv("HOSTNAME");
//-----------------------------------------------------------------------------
// load in ROOT physics vectors and event generator libraries
//-----------------------------------------------------------------------------
  // gSystem->Load("$ROOTSYS/lib/libEG.so");
  // //  gSystem->Load("$ROOTSYS/lib/libPhysics.so");
  // gSystem->Load("$ROOTSYS/lib/libMinuit.so");
  // gSystem->Load("$ROOTSYS/lib/libFumili.so");
  //  gSystem->Load("$ROOTSYS/lib/libTree.so");
  //  gSystem->Load("$ROOTSYS/lib/libRuby.so");
//-----------------------------------------------------------------------------
//  check batch mode
//-----------------------------------------------------------------------------
  // const char* opt ;
  // int batch_mode = 0;

  // int nargs = gApplication->Argc();

  // for (int i=1; i<nargs; i++) {
  //   opt  = gApplication->Argv(i);
  //   if (strcmp(opt,"-b") == 0) {
  //     batch_mode = 1;
  //     break;
  //   }
  // }

  // printf("   batch_mode = %i\n",batch_mode);
//-----------------------------------------------------------------------------
// always need libStntuple_loop, but the other 2 libs should be loaded in 
// only if we're running bare root
//-----------------------------------------------------------------------------
  const char* exec_name = gApplication->Argv(0);
 
  // printf(" nargs = %2i exec_name = %s\n",nargs, exec_name);

  if (exec_name) {
    if (strcmp(exec_name,"root.exe") == 0) {
					// print overflows/underflows in the stat box
      gStyle->SetOptStat(11111111);
					// print fit results in the stat box
      gStyle->SetOptFit(1110);
      TArrow::SetDefaultArrowSize(0.015);
    // }
    // else if (strstr(exec_name,"mu2e.NNN") != 0) {
    //   //      gSystem->Load("$MU2E_BASE_RELEASE/lib/libmurat_obj.so");
    //   gSystem->Load("$MU2E_BASE_RELEASE/lib/libStntuple_val.so");
    //   //      gSystem->Load("$MU2E_BASE_RELEASE/lib/libmurat_plot.so");
    // }

      //-----------------------------------------------------------------------------
      // report the process ID which simplifies debugging
      //-----------------------------------------------------------------------------
      printf("process ID: %i\n",gSystem->GetPid());
      TAuthenticate::SetGlobalUser(gSystem->Getenv("USER"));
      gInterpreter->ProcessLine(".! ps | grep root");
      printf("Loading AsciiPlotter, ZTauTau_X_Makers, Fitter, DataPlotter, and CutsetTrainer\n");
      TString cmssw = gSystem->Getenv("CMSSW_BASE");
      TString path = (hostname.Contains("cmslpc")) ? "/src/BLT/BLTAnalysis/" : gSystem->Getenv("PWD");
      if(!hostname.Contains("cmslpc")) path += "/../";
      // gSystem->Load((cmssw + path + "AsciiPlotter/AsciiPlotter_cc.so").Data());
      gSystem->Load((cmssw + path + "rootScripts/ZTauTauHistMaker_cc.so").Data());
     // gSystem->Load((cmssw + path + "rootScripts/Fitter_cc.so").Data());
      gSystem->Load((cmssw + path + "rootScripts/DataPlotter_cc.so").Data());
     // gSystem->Load((cmssw + path + "CutsetTraining/CutsetTrainer_cc.so").Data());

      // cout << "Loading SVFit libraries" << endl;
     // gSystem->Load("libTauAnalysisClassicSVfit.so");
     // gSystem->Load("libTauAnalysisSVfitTF.so");
    }
  }
//-----------------------------------------------------------------------------
//  databases
//-----------------------------------------------------------------------------
  if(hostname.Contains("cmslpc") && TString(gSystem->Getenv("PWD")).Contains("BLTAnalysis/")) { //only load if on LPC and not in nano AOD area
    cout << "Loading Bacon data formats." << endl;
    gSystem->Load("libBaconAnaDataFormats.so"); 
  }
}


