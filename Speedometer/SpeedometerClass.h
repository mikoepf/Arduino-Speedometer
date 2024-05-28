#pragma once  // should prevent the multiple call of the header.
//#ifndef SpeedometerClass_H // should prevent the multiple call of the header.
//#define SpeedometerClass_H // should prevent the multiple call of the header.
//#include <Keypad.h>         // A library for using matrix style keypads with the Arduino.
#include <Joystick.h>       // A library to emulate a Joystick with buttons, switches and axis.
//#include <Wire.h>           // This library allows you to communicate with I2C devices.
#include <LiquidCrystal_I2C.h>  // This library allows you to communicate with I2C Liquid crystal Display.

#define axis_minimum  0       // Minimumvalue of the axis-range.
#define axis_maximum  1023    // Maximumvalue of the axis-range.
#define pins          22      // Number of accessible Pins on the Arduino Micro Pro.
#define RSETBUT       7       // Pinnumber of the Resetbutton.
#define ROTBUT        6       // Pinnumber of the Rotary-EncoderButton.
#define ROTARY        21      // Pinnumber of the RotationSensor.


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
unsigned long int time_stamp; // The time when the falling edge (the current signal is low/0 and the last measurement was high/1).
                              // was detected, is stored here.
unsigned long int GetTime();  // Detects the falling edge and stores the time when ist was detected int the "time_stamp".
void SetSignalOrder();  // Stores the current signal value for later comparison in the next iteration.
Sensor() : signal_order(0), time_stamp (0) {};
Sensor(const unsigned long int t_stamp,const bool s_order): time_stamp(t_stamp),signal_order(s_order) {};
};

class TimeDifference  // Determines the time between the last detected falling edge and the currently detected falling edge.
{
  private:

  public:
unsigned long int previous_time_stamp;  // The time, when the last falling edge was detected, is stored here.
unsigned long int time_difference;  // The time between the last detected falling edge and the currently detected falling edge is stored here.
void CalculateTimediff(const unsigned long int &t_stamp); // Determines the time between the last detected falling edge and the currently detected falling edge.
TimeDifference(): previous_time_stamp(0),time_difference(0){};
TimeDifference(const unsigned long int p_stamp,const unsigned long int t_diff): previous_time_stamp(p_stamp),time_difference(t_diff) {};
};

class Rpm
{
  private:
unsigned long int* sample_buffer; // This is a pointer to a dynammicly allocated buffer with the length of "sample_size".
                                      // , where the time_differences are stored (FIFO-princip).
unsigned long int* median_sample_buffer; // This is a pointer to a dynammicly allocated buffer with the length of "sample_size".
                                      // , where the time_differences are stored (FIFO-princip).                                      
unsigned short int sample_size; // Determines the size of the "sample_buffer" buffer.
unsigned short int sample_count;  // Dtores the number of currently availeable samples as long as the buffer is not full(sample_count<sample_size). 
unsigned short int median_sample_count;  // Dtores the number of currently availeable samples as long as the buffer is not full(sample_count<sample_size). 
unsigned short int rpm; // Stores the calculated revolutions per minute "rpm".
char filter[6];     // Needs to be resolved !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  public:
unsigned long int av_time_difference; // stores the averaged timedifferences which are calculated/filtered from the time-differences stored in the buffer.
void FillBuffer(const unsigned long int &time_difference);  // Stores the current sample value in a buffer array (FIFO-princip).
void CalculateRpm();  // This method calculates the revolutions per minute "rpm" from the "av_time_difference".
void ApplyFilter(const unsigned long int &time_difference); // Applies a filter selected in the menue.
void AverageFilter(); // This method calculates the averaged timedifferences from the time-differences stored in the buffer
                                                              // by building a sum of all sample-values stored in the buffer and dividing it by the number of samples.
void MedianFilter(const unsigned long int &time_difference);  // This method calculates the averaged timedifferences from the time-differences stored in the buffer
                                                              // by sorting the sample-values in ascednig order and taking the sample-values in the center (sample_count/2)
void MedianAverageFilter(const unsigned long int &time_difference);  // This method calculates the averaged timedifferences from the time-differences stored in the buffer
                                                              // by combining the medianfilter and averagefilter to the buffer.
void ResetRpm(Sensor & sensor,TimeDifference & timedifference);  // This method resets the rpm to 0 and clears/zeroizes all the respective buffers and variables.
Rpm(const Profile & other)
{
  //filter = other.filter_name;
  sample_size = other.sample_size;
//  strcpy_s(filter, 6, other.filter_name);
sample_count = median_sample_count=0;
for(int i = 0;i<6;++i)
{
  filter[i] = other.filter_name[i];
}


sample_buffer = new unsigned long int[sample_size];
for(int i = 0;i<sample_size;++i)
{
  sample_buffer[i] = 0;
}

median_sample_buffer = new unsigned long int[sample_size];
for(int i = 0;i<sample_size;++i)
{
  median_sample_buffer[i] = 0;
}

};

~Rpm()
{
  delete[] sample_buffer;
  delete[] median_sample_buffer;
};

};


//#endif