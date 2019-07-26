Double_t scale[20];
Double_t sScale[10];
Double_t scalePt[20];
Double_t sScalePt[10];
Double_t sOrig[2] = {69.29,65.14};
Double_t sOrigPt[2] = {150.9,146.7};
TFile*   data[20];
TFile*   sData[10];
TFile*   dataPt[20];
TFile*   sDataPt[10];
Int_t    noPtOM_ = 0;
Int_t    noPt_   = 1;
Int_t    bkgds_[20]; //indicates which backgrounds to use
Int_t    doPtStudy_ = 1;


TH1F* plot_cuts(Int_t set, Double_t signalSteps = 0.02) {

  const char* cutNames[] = {"leppt","lepptoverm","lepeta","onept","twopt","onereliso","tworeliso",
			    "oneiso","twoiso","met","jetslepdelphi","jetlepdelphi","bjetlepdelphi",
			    "jetslepdelr","jetlepdelr","bjetlepdelr"};
  const char* cutSets[] = {"event","event","event","lep","lep","lep","lep",
			   "lep","lep","event","event","event","event",
			   "event","event","event"};
  Int_t nCuts = sizeof(cutNames)/sizeof(*cutNames);
  TH1F* cutValues = new TH1F("cutValues","cutValues",nCuts*2,0,nCuts*2.0);
  double m = 0.; //to find best cut
  int cut =  -1; //cut num
  int cutDir = 0; //cut direction
  double cutVal = 0; //cut value
  Double_t denom = (doPtStudy_ == 0) ? sOrig[set-20] : sOrigPt[set-20];
  Double_t signalLoss = 0.;
  for(int i = 0; i < nCuts; ++i) {
    // printf("Testing Cut %s in set %i\n",cutNames[i], set);
    TH1F* h[2] = {0,0};
    TH1F* hTemp;
    if(noPtOM_ && (strcmp("lepptoverm",cutNames[i]) == 0)) continue;
    if(noPt_ && (strcmp("leppt",cutNames[i]) == 0)) continue;
    //get background histograms
    for(int j = 0; j < 20; ++j) {
      if(bkgds_[j] == 0) continue;
      TFile* f = (doPtStudy_ == 0) ? (TFile*) data[j]->Get(Form("%s_%i",cutSets[i],set)) :
	(TFile*) dataPt[j]->Get(Form("%s_%i",cutSets[i],set));
      // printf("Opening %s_%i\n",cutSets[i], set);
      if(!f) continue;
      hTemp = (TH1F*) f->Get(cutNames[i]);
      // printf("Pulling %s\n",cutNames[i]);
      if(!hTemp) continue;
      hTemp->Scale((doPtStudy_ == 0) ? scale[j] : scalePt[j]);
      // printf("Scaling by %f\n",scale[j]);
      if(j == 0 || !h[0]) h[0] = hTemp;
      else h[0]->Add(hTemp);
    }
    if(!h[0]) continue;
    //get signal histograms
    for(int j = 0; j < 1; ++j) {
      TFile* f = (doPtStudy_ == 0) ? (TFile*) sData[j]->Get(Form("%s_%i",cutSets[i],set)) :
	(TFile*) sDataPt[j]->Get(Form("%s_%i",cutSets[i],set));
      if(!f) continue;
      hTemp = (TH1F*) f->Get(cutNames[i]);
      if(!hTemp) continue;
      hTemp->Scale((doPtStudy_ == 0) ? sScale[j] : sScalePt[j]);
      if(!h[1]) h[1] = hTemp;
      else h[1]->Add(hTemp);
    }
    if(i==0 || (noPt_ > 0 && i==1) ) printf("***Integral of Hists Before Cuts: %.3e (Bkgd) %.3e (Signal)\n",h[0]->Integral(),h[1]->Integral());
    Int_t nBins = h[1]->GetNbinsX();
    Int_t bin = -1;
    for(int j = 1; j <= nBins; ++j) {
      if(h[1]->Integral(1,j)/denom > signalSteps) {
	signalLoss = h[1]->Integral(1,j);
	bin = j;
	break;
      }
    }
    double cutFrac = 0.;
    if(bin>0) {
      double signal = h[1]->Integral(1,bin);
      double bkgr   = h[0]->Integral(1,bin);
      cutFrac = bkgr/signal;//sqrt(signal+bkgr)/signal;
    }
    if(cutFrac > m) {
      m = cutFrac;
      cut = i;
      cutDir = 1;
      cutVal = h[1]->GetBinLowEdge(bin) + h[1]->GetBinWidth(bin);
    }
    cutValues->Fill(2*i,cutFrac);
    cutValues->GetXaxis()->SetBinLabel(2*i+1,Form("%s_-",cutNames[i]));
    if(bin < 0) printf("No Bin Found for Cut %s in set %i\n",cutNames[i], set);
    else printf("%s from below = %.2e frac = %.2e loss = %.2e / %.2e ",
		cutNames[i],h[1]->GetBinLowEdge(bin)+h[1]->GetBinWidth(bin),
		cutFrac, signalLoss,denom);
    bin = -1;
    for(int j = nBins; j > 0; --j) {
      if(h[1]->Integral(j,nBins)/denom > signalSteps) {
	signalLoss = h[1]->Integral(j,nBins);
	bin = j;
	break;
      }
    }
    cutFrac = 0.;
    if(bin>0) {
      double signal = h[1]->Integral(bin,nBins);
      double bkgr   = h[0]->Integral(bin,nBins);
      cutFrac = bkgr/signal;//sqrt(signal+bkgr)/signal;
    }

    if(cutFrac > m) {
      m = cutFrac;
      cut = i;
      cutDir = -1;
      cutVal = h[0]->GetBinLowEdge(bin);
    }
    if(bin < 0) printf("No Bin Found for Cut %s in set %i\n",cutNames[i], set);
    else printf("from above = %.2e frac = %.2e loss = %.2e / %.2e\n",
		h[0]->GetBinLowEdge(bin),
		cutFrac, signalLoss,denom);

    cutValues->Fill(2*i+1,cutFrac);
    cutValues->GetXaxis()->SetBinLabel(2*i+2,Form("%s_+",cutNames[i]));
  }
  printf("***Final cut with fraction %.2e is %s %s %.2e\n",m,cutNames[cut],(cutDir>0)?">":"<",cutVal);
  return cutValues;
}

Int_t find_cuts() {
  printf("Beginning Find Cuts\n");

  const char* names[] = {
    			 "t_tw"                   ,
                         "tbar_tw"                ,
			 "ttbar_inclusive"        ,
			 "zjets_m-50_alt"         ,
			 "zjets_m-10to50_alt"     ,
			 "zjets_m-50"             , //inclusive?
			 "zjets_m-10to50"         , //inclusive?
			 "z1jets_m-50"            ,
			 "z1jets_m-10to50"        ,
			 "z2jets_m-50"            ,
			 "z2jets_m-10to50"        ,
			 "z3jets_m-50"            ,
			 "z3jets_m-10to50"        ,
			 "z4jets_m-50"            ,
			 "z4jets_m-10to50"        };
  
  const char* files[] = {Form("tree_%s.hist",names[0]),
			 Form("tree_%s.hist",names[1]),
			 Form("tree_%s.hist",names[2]),
			 Form("tree_%s.hist",names[3]),
			 Form("tree_%s.hist",names[4]),
			 Form("tree_%s.hist",names[5]),
			 Form("tree_%s.hist",names[6]),
			 Form("tree_%s.hist",names[7]),
			 Form("tree_%s.hist",names[8]),
			 Form("tree_%s.hist",names[9]),
			 Form("tree_%s.hist",names[10]),
			 Form("tree_%s.hist",names[11]),
			 Form("tree_%s.hist",names[12]),
			 Form("tree_%s.hist",names[13]),
			 Form("tree_%s.hist",names[14])};

  const char* filesPtStudy[] = {Form("tree_%s.ptstudy_hist",names[0]),
				Form("tree_%s.ptstudy_hist",names[1]),
				Form("tree_%s.ptstudy_hist",names[2]),
				Form("tree_%s.ptstudy_hist",names[3]),
				Form("tree_%s.ptstudy_hist",names[4]),
				Form("tree_%s.ptstudy_hist",names[5]),
				Form("tree_%s.ptstudy_hist",names[6]),
				Form("tree_%s.ptstudy_hist",names[7]),
				Form("tree_%s.ptstudy_hist",names[8]),
				Form("tree_%s.ptstudy_hist",names[9]),
				Form("tree_%s.ptstudy_hist",names[10]),
				Form("tree_%s.ptstudy_hist",names[11]),
				Form("tree_%s.ptstudy_hist",names[12]),
				Form("tree_%s.ptstudy_hist",names[13]),
				Form("tree_%s.ptstudy_hist",names[14])};

  const Double_t xsec[] = {
			   35.6,
			   35.6,
			   831.8,
			   6803.2,
			   21959.8,
			   6803.2,
			   21959.8,
			   1198.9,
			   855.5,
			   390.6,
			   466.1,
			   113.3,
			   114.5,
			   60.2,
			   36.4,}; //taken from CMS AN-16-458

  const Double_t sXsec[] = {.1,.1,.1,.1,.1}; //just for a reference
  // const char* dNames[] = {
  // 			 "muon_2016B"                 };
  
  // const char* dFiles[] = {Form("tree_%s.hist",dNames[0])};

  // const Double_t dLum[] = {
  // 			   5.3e3}; //taken from CMS AN-16-458

  const Double_t lum    = 36.4e3; //Doesn't Matter Here
  

  const char* sNames[] = { //Signal
      			 "b2bx_t"                     ,
    			 "bb2bbdimu"                  ,
    			 "fcnc_mx10"                  ,
    			 "fcnc_mx30"                  ,
    			 "fcnc_mx60"                  };

  
  const char* sFiles[] = {Form("tree_%s.hist",sNames[0]),
			  Form("tree_%s.hist",sNames[1]),
			  Form("tree_%s.hist",sNames[2]),
			  Form("tree_%s.hist",sNames[3]),
			  Form("tree_%s.hist",sNames[4])};

  const char* sFilesPt[] = {Form("tree_%s.ptstudy_hist",sNames[0]),
			    Form("tree_%s.ptstudy_hist",sNames[1]),
			    Form("tree_%s.ptstudy_hist",sNames[2]),
			    Form("tree_%s.ptstudy_hist",sNames[3]),
			    Form("tree_%s.ptstudy_hist",sNames[4])};

  printf("Opening Background Files\n");
  TFile* f[] = {new TFile(files[0]),new TFile(files[1]),new TFile(files[2]),
		new TFile(files[3]),new TFile(files[4]),new TFile(files[5])
		,new TFile(files[6]),new TFile(files[7]),new TFile(files[8])
		,new TFile(files[9]),new TFile(files[10]),new TFile(files[11])
		,new TFile(files[12]),new TFile(files[13]),new TFile(files[14])};

  TFile* fPt[] = {new TFile(filesPtStudy[0]), new TFile(filesPtStudy[1]), new TFile(filesPtStudy[2]),
		  new TFile(filesPtStudy[3]), new TFile(filesPtStudy[4]), new TFile(filesPtStudy[5]),
		  new TFile(filesPtStudy[6]), new TFile(filesPtStudy[7]), new TFile(filesPtStudy[8]),
		  new TFile(filesPtStudy[9]), new TFile(filesPtStudy[10]),new TFile(filesPtStudy[11]),
		  new TFile(filesPtStudy[12]),new TFile(filesPtStudy[13]),new TFile(filesPtStudy[14])};
  
  printf("Opening Signal Files\n");
  TFile* sF[] = {new TFile(sFiles[0]),new TFile(sFiles[1]),new TFile(sFiles[2]),
		 new TFile(sFiles[3]),new TFile(sFiles[4])};

  TFile* sFPt[] = {new TFile(sFilesPt[0]),new TFile(sFilesPt[1]),new TFile(sFilesPt[2]),
		   new TFile(sFilesPt[3]),new TFile(sFilesPt[4])};


  for(int i = 0; i < sizeof(bkgds_)/sizeof(*bkgds_); ++i) bkgds_[i]=0;
  bkgds_[0]  = 1; //t
  bkgds_[1]  = 1; //tbar
  bkgds_[2]  = 1; //ttbar
  bkgds_[3]  = 0; //amc z
  bkgds_[4]  = 0; //amc z
  bkgds_[5]  = 0; //madgraph z
  bkgds_[6]  = 0; //madgraph z
  bkgds_[7]  = 1; //madgraph z + 1 jet
  bkgds_[8]  = 1; //madgraph z + 1 jet
  bkgds_[9]  = 1; //madgraph z + 2 jets
  bkgds_[10] = 1; //madgraph z + 2 jets
  bkgds_[11] = 1; //madgraph z + 3 jets
  bkgds_[12] = 1; //madgraph z + 3 jets
  bkgds_[13] = 1; //madgraph z + 4 jets
  bkgds_[14] = 1; //madgraph z + 4 jets

  Int_t nF  = sizeof(f) /sizeof(*f);
  Int_t nsF = sizeof(sF)/sizeof(*sF);
  for(int i = 0; i < nF; ++i) {
    data[i]  = (TFile*) f[i]->Get("Data");
  }
  for(int i = 0; i < nF; ++i) {
    dataPt[i]  = (bkgds_[i] > 0) ? (TFile*) fPt[i]->Get("Data") : 0;
  }
  for(int i = 0; i < nsF; ++i) {
    sData[i]  = (TFile*) sF[i]->Get("Data");
  }
  for(int i = 0; i < nsF; ++i) {
    sDataPt[i]  = (TFile*) sFPt[i]->Get("Data");
  }

  TH1F* evts[nF];
  for(int i = 0; i < nF; ++i) {
    evts[i] = (TH1F*)  f[i]->Get(Form("TotalEvents_%s",names[i]));
    scale[i]  = 1./(evts[i]->GetBinContent(1)-2*evts[i]->GetBinContent(11))*xsec[i]*lum;
  }

  TH1F* evtsPt[nF];
  for(int i = 0; i < nF; ++i) {
    evtsPt[i] = 0;
    if(bkgds_[i] == 0) continue;
    evtsPt[i] = (TH1F*)  fPt[i]->Get(Form("TotalEvents_%s",names[i]));
    scalePt[i]  = 1./(evtsPt[i]->GetBinContent(1)-2*evtsPt[i]->GetBinContent(11))*xsec[i]*lum;
  }


  TH1F* sEvts[nsF];
  for(int i = 0; i < nsF; ++i) {
    sEvts[i] = (TH1F*)  sF[i]->Get(Form("TotalEvents_%s",sNames[i]));
    sScale[i]  = 1./(sEvts[i]->GetBinContent(1)-2*sEvts[i]->GetBinContent(11))*sXsec[i]*lum;
  }
  
  TH1F* sEvtsPt[nsF];
  for(int i = 0; i < nsF; ++i) {
    sEvtsPt[i]   = (TH1F*)  sFPt[i]->Get(Form("TotalEvents_%s",sNames[i]));
    sScalePt[i]  = 1./(sEvtsPt[i]->GetBinContent(1)-2*sEvtsPt[i]->GetBinContent(11))*sXsec[i]*lum;
  }


  gStyle->SetTitleW(0.8f);
  //  return print_stacks(folders,hists,setTypes,sets, xMaxs, xMins,rebins);
  gStyle->SetOptStat(0);
  TH1F* h = plot_cuts(20);
  h->Draw("HIST");
  new TCanvas();
  TH1F* h2 = plot_cuts(21);
  h2->Draw("HIST");
  return 0;
}
