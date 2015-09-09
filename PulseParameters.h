
# ifndef __PULSE__
# define __PULSE__

#include "TString.h"

// total number of bunches in "LHC" bunch train
const int NBXTOTAL = 2800;

// length of a waveform in 1ns steps
const int WFLENGTH  = 500;

// number of samples per hit
// const int NSAMPLES   = 10;
const int NSAMPLES   = 20;

// distance between samples in 1ns steps
// const int NFREQ      = 25;
const int NFREQ      = 12;

// position of a 1st sample inside waveform
const int IDSTART    = 180;

// CRRC shaping time in ns. It is used to calculate noise
// correlations. For QIE, set it to 1e-1
const double TAU = 43.0;

// filename with pulse shape
const TString FNAMESHAPE       = "data/EmptyFileCRRC43.root";

# endif

