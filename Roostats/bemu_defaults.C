//MVA fitting mode configuration
#ifndef __BEMU_DEFAULTS__
#define __BEMU_DEFAULTS__

#ifndef __MVA_DEFAULTS__
bool useDefaultBr_    = false; //use given branching fraction instead of fixed example
#ifndef __DATAPLOTTER_CLFV__
Int_t verbose_        = 0;
#endif

#endif
bool blind_data_      = true; //blind the data histogram in the signal region
int bemu_embed_mode_  = 1; //0: nominal running (MC is DY); 1: use Embedded tautau (ignores overlap from ttbar --> tautau --> emu)
int zmumu_model_      = 1; //model Z->mumu explicitly in the search
int zmumu_pdf_sys_    = 1; //include Z->mumu PDF parameter uncertainties
#endif
