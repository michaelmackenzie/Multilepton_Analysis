//MVA fitting mode configuration
#ifndef __BEMU_DEFAULTS__
#define __BEMU_DEFAULTS__

#ifndef __MVA_DEFAULTS__
bool useDefaultBr_    = false; //use given branching fraction instead of fixed example
#ifndef __DATAPLOTTER_CLFV__
Int_t verbose_        = 0;
#endif

#endif
int use_same_flavor_  = false;
bool blindData_     = true;
int zemu_signal_mode_ = 1; //Z->e+mu resonance shape: 0: CB+Gaussian; 1: DSCB

#endif
