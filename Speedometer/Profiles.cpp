#include "Profiles.h" 

const unsigned short int Profiles::pn = 5;
const String Profiles::profile_names [5] = {"Default","Test1","Test2","Test3","Test4"};
const unsigned short int Profiles::min_rpms[5] = {30,15,35,19,24};
const unsigned short int Profiles::max_rpms[5] = {81,95,80,99,84};
const unsigned short int Profiles::sample_sizes[5] = {6,9,5,6,4};
const String Profiles::filter_names[5] = {"AVR","MED","MIX","AVR","MIX"};
unsigned short int Profiles::count=0;
