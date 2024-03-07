int check_batch_file(const char* file) {
  TFile* f = TFile::Open(file, "READ");
  if(!f) return 1;
  if(!f->Get("mutau") ) return 2;
  if(!f->Get("etau")  ) return 3;
  if(!f->Get("emu")   ) return 4;
  if(!f->Get("mumu")  ) return 5;
  if(!f->Get("ee")    ) return 6;
  if(!f->Get("Runs")  ) return 7;
  if(!f->Get("events")) return 8;
  return 0;
}
