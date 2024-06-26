#pragma once  // should prevent the multiple call of the header.

#ifndef SPEEDOMETERCLASS_H // should prevent the multiple call of the header.
#define SPEEDOMETERCLASS_H // should prevent the multiple call of the header.

#include <Arduino.h>
#include <EEPROM.h>
#include <Joystick.h>       // A library to emulate a Joystick with buttons, switches and axis.
#include <LiquidCrystal_I2C.h>  // This library allows you to communicate with I2C Liquid crystal Display.
#include "Profiles.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////.
#define AXISMINIMUM   0       // Minimumvalue of the axis-range.
#define AXISMAXIMUM   1023    // Maximumvalue of the axis-range.
#define NUMROTARIES   1       // Amount of rotary encoders attached to the Microkontroller
#define NUMBUTTONS    3       // Amount of buttons attached to the Microkontroller
#define PINS          22      // Number of accessible Pins on the Microkontroller.
#define RSETBUT       7       // Pinnumber of the Resetbutton.
#define ROTBUT        6       // Pinnumber of the Rotary-Encoder Button.
#define TACHO         21      // Pinnumber of the RotationSensor.
#define SENSORS       20      // Amount of magnets placed on the pulley. => So many signal falling edges are detected in one revolution
#define ROTARYMENU    0       // Which Rotary is used for the menu navigation
#define ROTMENU1      5       // Pinnumber of the Resetbutton.
#define ROTMENU2      4       // Pinnumber of the Rotary-Encoder Button.
#define ROTDELAY      100     // The delay in "milli seconds" between the rotary encoder input requests to avoid signal bouncing
#define BUTTONDELAY   500     // The delay in "milli seconds" between the button input requests to avoid signal bouncing
#define RPMMIN        5       // The lowest rpm threshold 
#define RPMMAX        200     // The highest rpm threshold 
#define NUMFILTERS    3       // Amount of hardcoded filters
#define SAMPLEMIN     1       // The kowes sample threshold: At least this number of samples has to be stored in the buffer
#define SAMPLEMAX     20      // The highest sample threshold: Maximal so many samples can be stored in the buffer

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Stores the currently utilized parameter values
struct Profile
{
  String profile_name;
  unsigned short int min_rpm;// 3000/max_time_difference; // 3000 => (60sec * 1000ms) / 20magnets "sensors/pings per revolution"  (47)
  unsigned short int max_rpm;// 3000/min_time_difference; // 3000 => (60sec * 1000ms) / 20magnets "sensors/pings per revolution"  (85)
  String filter_name;
  unsigned short int sample_size;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  A class, which methods detect a falling edge of the sensor signal
class Sensor
{
  private:
bool signal_order;
  public:
unsigned long int time_stamp; // The time when the falling edge (the current signal is low/0 and the last measurement was high/1).
                              // was detected, is stored here.
unsigned long int GetTime();  // Detects the falling edge and stores the time when it was detected int the "time_stamp".
void SetSignalOrder();  // Stores the current signal value for later comparison in the next iteration.
Sensor() : signal_order(0), time_stamp (0) {};
Sensor(const unsigned long int t_stamp,const bool s_order): time_stamp(t_stamp),signal_order(s_order) {};
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Determines the time between the last detected falling edge and the currently detected falling edge.
class TimeDifference  
{
  private:

  public:
unsigned long int previous_time_stamp;  // The time, when the last falling edge was detected, is stored here.
unsigned long int time_difference;  // The time between the last detected falling edge and the currently detected falling edge is stored here.
//void CalculateTimediff(const unsigned long int &t_stamp,const Profile & profile,const Rpm & rpm);/*, const Rpm &rpm);*/ // Determines the time between the last detected falling edge and the currently detected falling edge.
void CalculateTimediff(const unsigned long int &t_stamp,const Profile & profile);/*, const Rpm &rpm);*/ // Determines the time between the last detected falling edge and the currently detected falling edge.
TimeDifference(): previous_time_stamp(0),time_difference(0){};
TimeDifference(const unsigned long int p_stamp,const unsigned long int t_diff): previous_time_stamp(p_stamp),time_difference(t_diff) {};
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A class, which methods are used to apply a filter to the recorded timedifferences and to determine the revolutions per minute (rotational velocity)
class Rpm
{
  private:
unsigned long int* sample_buffer; // This is a pointer to a dynammicly allocated buffer with the length of "sample_size".
                                      // , where the time_differences are stored (FIFO-princip).
unsigned long int* median_sample_buffer; // This is a pointer to a dynammicly allocated buffer with the length of "sample_size".
                                      // , where the time_differences are stored (FIFO-princip).                                      
unsigned short int sample_count;  // Stores the number of currently availeable samples as long as the buffer is not full(sample_count<sample_size). 
unsigned short int median_sample_count;  // Dtores the number of currently availeable samples as long as the buffer is not full(sample_count<sample_size). 
unsigned short int rpm; // Stores the calculated revolutions per minute "rpm".

  public:
unsigned long int av_time_difference; // stores the averaged timedifferences which are calculated/filtered from the time-differences stored in the buffer.
void FillBuffer(const unsigned long int &time_difference,Profile &ProfileCurrent);  // Stores the current sample value in a buffer array (FIFO-princip).
void CalculateRpm();  // This method calculates the revolutions per minute "rpm" from the "av_time_difference".
void ApplyFilter(const unsigned long int &time_difference,String filter[],Profile &ProfileCurrent); // Applies a filter selected in the menue.
void AverageFilter(); // This method calculates the averaged timedifferences from the time-differences stored in the buffer
                                                              // by building a sum of all sample-values stored in the buffer and dividing it by the number of samples.
void MedianFilter(const unsigned long int &time_difference);  // This method calculates the averaged timedifferences from the time-differences stored in the buffer
                                                              // by sorting the sample-values in ascednig order and taking the sample-values in the center (sample_count/2)
void MedianAverageFilter(const unsigned long int &time_difference,Profile &ProfileCurrent);  // This method calculates the averaged timedifferences from the time-differences stored in the buffer
                                                              // by combining the medianfilter and averagefilter to the buffer.
void ResetRpm(Sensor & sensor,TimeDifference & timedifference);  // This method resets the rpm to 0 and clears/zeroizes all the respective buffers and variables.

Rpm(const Profile & other)  // Constructor: Initialices the argument values according to the reference profile
{

sample_count = median_sample_count=0;

sample_buffer = new unsigned long int[SAMPLEMAX];
for(int i = 0;i<SAMPLEMAX;++i)
{
  sample_buffer[i] = 0;
}
median_sample_buffer = new unsigned long int[SAMPLEMAX];
for(int i = 0;i<SAMPLEMAX;++i)
{
  median_sample_buffer[i] = 0;
}
};
//   The idea was to destroy the class object and instatiate a new one with new parameters during the reset... => not possible for now.
~Rpm()  // Destructor: was meant to free the dynamic arrays. Since the Arduino does not allow to exit the main loop during the execution and subsequently initialice new instances, this destructor is never used. => maybe a solution can be found later...
{
  delete[] sample_buffer;
  delete[] median_sample_buffer;
};

};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  A class, which methods are used to calculate the axis value and send it to the computer via USB
class AxisValue
{
  private:
double throttle;  // The Axisvalue of the Joystick is stored here. It is called "Throttle" since the Joistick-Axis is defined as throttle-axis
  public:
void CalculateAxisValue(const Profile & profile, const Rpm & rpm, Sensor &sensor); // Transforms the measured timedifferenzes between signal-falling edges to Axisvalues.
void SetAxisValue(Joystick_ &Joystick ); // Sets/sends the calculated Axisvalues to the Joystick.
void ResetAxisValue(const Profile & profile,Rpm & rpm,Sensor &sensor,TimeDifference & timedifference); // Resets the Axisvalue to zero if no falling edge has been detected for a prolonged time
                                                                                            // Since the Axisvalue is calculate only when a falling edge has been detected, the Axisvalu would be stuck
                                                                                            // on the last calculated value, if no new falling edge has been detected. E.g: abrupt stop of the rotation.
AxisValue() : throttle(0){};
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A class, which methods are used to register the interface inputs and to print the menu and parameter values on the LCD
class UserInput
{
  private:
bool consent_load;  // For query whether the selected Profile should be loaded => true = YES, false = NO
bool consent_save;  // For query whether the current Profile should be saved => true = YES, false = NO
bool* pin_state;  // Array to store the pin states (not utilized)
unsigned long int rot_delay;  // Saves the timestamp for rotary encoder delay
unsigned long int button_delay; // Saves the timestamp for button delay
unsigned short int rot_pin1;  // Stores the pin adress of the rotary encoder (has two pins) 
unsigned short int rot_pin2;  // Stores the pin adress of the rotary encoder (has two pins) 
unsigned short int rot_dir; // Stores the rotation direction/state of the rotary encoder.
volatile bool rot_state[2];  // Stores the current, or last state of rotary encoder signal.
public:
unsigned short int menu_count;  // For menu-iterations
enum EnumRotState {DIR_NONE,DIR_CW,DIR_CCW};  // Represents the rotation direction/states of the rotary encoder.
enum EnumMenu {STARTMENU,PROFILE_LOAD,MINRPM,MAXRPM,FILTER,SAMPLE_SIZE,PROFILE_SAVE,MENUSIZE};  // Represents the different menus.
bool GetInput(const unsigned short int i);  // Reads the signals on the given pin (number).
unsigned short int CheckRotaries(); // Reads the state of the given rotary (number).
void ChangeParameter(LiquidCrystal_I2C &lcd,Profile &ProfileCurrent,String filter[],Rpm & rpm,Sensor & sensor,TimeDifference & timedifference); // When the rotary encoder is rotaded, 
                                                                                                      //the parameter in the currently active menu are altered respectively.
void InitRot(); // Synchronizes the rotary encoder states at the begin of the programm.
void Reset(LiquidCrystal_I2C &lcd,Profile &ProfileCurrent,Rpm & rpm,Sensor & sensor,TimeDifference & timedifference); // Resets the Microcontroller
void NavigateMenu(LiquidCrystal_I2C &lcd,Profile &ProfileCurrent);  // Iterates the menus if the rotary encoder button is pressed.
const void LCDprint(LiquidCrystal_I2C &lcd,Profile &ProfileCurrent);  // Prints the parameter & settings on the LCD Display

UserInput() // Constructor with parameter initialization
{ 
pin_state = new bool [PINS];  // not utilized
for(int i = 0;i<PINS;++i)
{
  pin_state[i] = 0;
}
consent_load = 0;
consent_save = 0;
rot_delay = 0;
button_delay = 0;
rot_pin1 = ROTMENU1;
rot_pin2 = ROTMENU2;
rot_dir = 0;
rot_state[2] = (false,false);
menu_count = 0;
};
//   The idea was to destroy the class object and instatiate a new one with new parameters during the reset... => not possible for now.
~UserInput()  // Destructor: was meant to free the dynamic arrays. Since the Arduino does not allow to exit the main loop during the execution and subsequently initialice new instances, this destructor is never used. => maybe a solution can be found later...
{
delete[] pin_state;
};

};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A class, which methods are used to copy and store profiles
class Copy
{
  private:

  public:
static int eeAddress;
static void CopyProfiles(Profile & profile); // Copies the currently selected (count) Profile from the Profiles(Templates) to the referenced profile.
static void CopyProfiles(Profile & profile,const unsigned short int & count); // Copies the referenced (count) Profile from the Profiles(Templates) to the referenced profile.
static void CopyProfiles(Profile & profile,const unsigned short int & count,const String & profile_name); // Copies the referenced (count) Profile from the Profiles(Templates) to the referenced profile.
static void CopyToEEPROM(Profile & profile); // Copies the Profile Stored in EEPROM to the referenced profile.
static bool LoadFromEEPROM(Profile & profile); // Loads the referenced profile to the EEPROM.

};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif