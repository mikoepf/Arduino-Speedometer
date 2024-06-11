#include "Profiles.h" 

const unsigned short int Profiles::pn = 3;
const String Profiles::profile_names [3] = {"Default","test 1","test 2"};
const unsigned short int Profiles::min_rpms[3] = {30,30,25};
const unsigned short int Profiles::max_rpms[3] = {80,80,80};
const unsigned short int Profiles::sample_sizes[3] = {6,6,6};
const String Profiles::filter_names[3] = {"AVR","AVR","AVR"};
unsigned short int Profiles::count=0;
