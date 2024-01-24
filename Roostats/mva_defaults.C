//MVA fitting mode configuration
#ifndef __MVA_DEFAULTS__
#define __MVA_DEFAULTS__

#ifndef __BEMU_DEFAULTS__
bool useDefaultBr_    = false; //use given branching fraction instead of fixed example
#ifndef __DATAPLOTTER_CLFV__
Int_t verbose_        = 0;
#endif
#endif

int  ignore_sys_            = 0; //ignore systematics
int  embed_mode_            = 1; //Nominal: 1; Definitions: 0: use DY MC; 1: use Embedding; 2: use Embedding except DY MC in emu
int  embed_muon_scale_mode_ = 1; //0: use Higgs LFV uncertainty binning; 1: use 1 bin
bool use_scale_sys_         = true; //use normalization uncertainties
int  embed_bin_stats_mode_  = 1; //increase embedded histogram uncertainties to account for binomial error: 1 = emu, 2 = all
int  split_pdf_sys_         = 0; //split PDF uncertainties into 31 nuisance parameters: 0 = single pdf shape, 1 = 31 pdf shapes, 2 = 1 pdf rate
int  signal_in_fakes_cr_    = 0; //include signal in the data - MC subtraction for the fake lepton background estimates
int  apply_signal_bias_     = 1; //correct for signal included in the data-driven background estimate control region

#endif
