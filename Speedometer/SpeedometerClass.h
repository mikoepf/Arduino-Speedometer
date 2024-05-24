#pragma once  // should prevent the multiple call of the header
//#ifndef SpeedometerClass_H // should prevent the multiple call of the header
//#define SpeedometerClass_H // should prevent the multiple call of the header
//#include <Keypad.h>         // A library for using matrix style keypads with the Arduino.
#include <Joystick.h>       // A library to emulate a Joystick with buttons, switches and axis
//#include <Wire.h>           // This library allows you to communicate with I2C devices
#include <LiquidCrystal_I2C.h>  // This library allows you to communicate with I2C Liquid crystal Display

#define axis_minimum  0       // Minimumvalue of the axis-range
#define axis_maximum  1023    // Maximumvalue of the axis-range
#define pins          22      // Number of accessible Pins on the Arduino Micro Pro
#define RSETBUT       7       // Pinnumber of the Resetbutton
#define ROTBUT        6       // Pinnumber of the Rotary-EncoderButton
#define ROTARY        21      // Pinnumber of the RotationSensor


/*
extern const unsigned short int axis_minimum = 0; // Minimumvalue of the axis-range
extern const unsigned short int axis_maximum = 1023;// Maximumvalue of the axis-range
extern const unsigned short int pins = 22;    // Number of accessible Pins on the Arduino Micro Pro
extern bool Pin [21];   // An array to store the Pin-values

extern const unsigned short int RSETBUT=7; // Pinnumber of the Resetbutton
extern const unsigned short int ROTBUT=6; // Pinnumber of the Rotary-EncoderButton
extern const unsigned short int ROTARY=21; // Pinnumber of the RotationSensor
*/
struct Profile
{
  char profile_name[6];
  char filter_name[6];
  unsigned short int max_rpm;//3000/minMillis; // 3000 => (60sec * 1000ms) / 20magnets "sensors/pings per revolution"  (85)
  unsigned short int min_rpm;//3000/maxMillis; // 3000 => (60sec * 1000ms) / 20magnets "sensors/pings per revolution"  (47)
  unsigned short int sample_size;
};


class Sensor
{
  private:
bool signal_order;
  public:
unsigned long int time_stamp;
unsigned long int GetTime();
void SetSignalOrder();
Sensor() : signal_order(0), time_stamp (0) {};
Sensor(const unsigned long int t_stamp,const bool s_order): time_stamp(t_stamp),signal_order(s_order) {};
};

class TimeDifference
{
  private:
unsigned long int previous_time_stamp;
  public:
unsigned long int time_difference;
void CalculateTimediff(const unsigned long int &t_stamp);
TimeDifference(): previous_time_stamp(0),time_difference(0){};
TimeDifference(const unsigned long int p_stamp,const unsigned long int t_diff): previous_time_stamp(p_stamp),time_difference(t_diff) {};
};

class Rpm
{
  private:
unsigned short int* time_differences;
unsigned short int sample_size;
unsigned short int sample_count;
unsigned short int rpm;
char filter[6];     // Needs to be resolved !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
unsigned long int av_time_difference;
  public:
void CalculateRpm();
void ApplyFilter(const unsigned long int &time_difference);
void AverageFilter(const unsigned long int &time_difference);
void MedianFilter(const unsigned long int &time_difference);
void ResetRpm();
Rpm(const Profile & other)
{
  //filter = other.filter_name;
  sample_size = other.sample_size;
//  strcpy_s(filter, 6, other.filter_name);

for(int i = 0;i<6;++i)
{
  filter[i] = other.filter_name[i];
}


time_differences = new unsigned short int[sample_size];
for(int i = 0;i<sample_size;++i)
{
  time_differences[i] = 0;
}

};

~Rpm()
{
  delete[] time_differences;
};

};


//#endif