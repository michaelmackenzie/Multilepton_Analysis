// Script to read in CLFV Analyzer ntuples and output histogrammed files
#include "../rootScripts/CLFVHistMaker.cc+g"

int process_ntuples() {
  //list of files to process
  std::vector<TString> file_names = {"output_zemu.root"
				, "output_hemu.root"
				, "output_htautau_gluglu.root"
				, "output_T_tW-channel.root"
				, "output_Tbar_tW-channel.root"};

  for(TString file_name : file_names) {
    std::cout << "Beginning file " << file_name.Data() << std::endl;
    //open the file
    TFile* f = TFile::Open(file_name.Data(), "READ");
    if(!f) continue; //skip if not found
    TDirectoryFile* fChannel = 0; //data selection channel
    TH1F* events = 0; //histogram that stores the generation values
    TKey* key = 0; //key in the file
    TIter nextkey(f->GetListOfKeys()); //iterator through file keys
    //get events histograms first
    while((key = (TKey*)nextkey())) {
      TObject* obj = key->ReadObj();
      if(obj->IsA()->InheritsFrom(TH1::Class())) events = (TH1F*)obj;
    }
    //check the histogram is found
    if(events == 0) {
      printf("Events Histogram in %s not found, continuing\n",file_name.Data());
      continue;
    }
    //now iterate again, but looking at each directory
    nextkey.Reset();
    while((key = (TKey*)nextkey())) {
      TObject* obj = key->ReadObj();
      if(obj->IsA()->InheritsFrom(TDirectoryFile::Class())) fChannel = (TDirectoryFile*)obj;
      else { //skip if not a directory
	continue;
      }
      printf("Found file %s\n",fChannel->GetName());
      //look through the directory for the TTree
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
	  //take the latest tree iteration from the file in case of copies (most entries)
	  if(tree) tree = (tmptree->GetEntriesFast() > tree->GetEntriesFast()) ? tmptree : tree;
	  else tree = tmptree;
	}
      }
      //check that these are found
      if(tree == 0) {
	printf("Tree in %s/%s not found, continuing\n",file_name.Data(),fChannel->GetName());
	continue;
      }
      if(eventsChannel == 0) {
	printf("Events Channel Histogram in %s/%s not found, continuing\n",file_name.Data(),fChannel->GetName());
	continue;
      }

      //run the histogramming selector over the tree
      CLFVHistMaker* selec = new CLFVHistMaker();
      //store some information first
      selec->fFolderName = fChannel->GetName();
      selec->fXsec = 1.; //ignore for now
      tree->Process(selec);
	//open back up the file
	TFile* out = new TFile(Form("clfv_%s_%s.hist",fChannel->GetName(), tree->GetName()),"UPDATE");
	if(out == 0) {
	  printf("Unable to find output hist file clfv_%s_%s.hist, continuing\n",fChannel->GetName(), tree->GetName());
	  continue;
	}
	//add the events histogram to the output
	events->Write();
	out->Write();
	delete out;
    }
    delete events;
    delete f;
  }
  return 0;
}
