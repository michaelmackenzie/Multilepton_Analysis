//MVA fitting mode configuration
#ifndef __MVA_DEFAULTS__
#define __MVA_DEFAULTS__

#ifndef __BEMU_DEFAULTS__
bool useDefaultBr_    = false; //use given branching fraction instead of fixed example
#ifndef __DATAPLOTTER_CLFV__
Int_t verbose_        = 1;
#endif
#endif

int embed_mode_            = 1; //Nominal: 1; Definitions: 0: use DY MC; 1: use Embedding; 2: use Embedding except DY MC in emu
int embed_muon_scale_mode_ = 1; //0: use Higgs LFV uncertainty binning; 1: use 1 bin
bool use_scale_sys_        = false; //use normalization uncertainties

#endif
