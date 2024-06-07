#pragma once  // should prevent the multiple call of the header.

#ifndef PROFILES_H
#define PROFILES_H

#include <Arduino.h>


struct Profiles
{
  
  static const unsigned short int pn;
  static const unsigned short int min_rpms[];
  static const unsigned short int max_rpms[];
  static const unsigned short int sample_sizes[];
  static const String profile_names [];
  static const String filter_names [];
  static unsigned short int count;
};

#endif // PROFILE_H