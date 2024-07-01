#include "Profiles.h" 

const unsigned short int Profiles::pn = 6;
const String Profiles::profile_names [6] = {"Default","test 1","test 2","DCS","DCS v2","ACCv2"};
const unsigned short int Profiles::min_rpms[6] = {30,30,25,30,10,5};
const unsigned short int Profiles::max_rpms[6] = {80,80,80,85,150,80};
const unsigned short int Profiles::sample_sizes[6] = {6,6,6,5,5,6};
const String Profiles::filter_names[6] = {"AVR","AVR","AVR","AVR","MED","AVR"};
unsigned short int Profiles::count=0;
