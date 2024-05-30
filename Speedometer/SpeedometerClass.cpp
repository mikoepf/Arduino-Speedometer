#include "LiquidCrystal_I2C.h"
#ifndef SPEEDOMETERCLASS_H // should prevent the multiple call of the header.
#define SPEEDOMETERCLASS_H // should prevent the multiple call of the header.

#include "SpeedometerClass.h" // This library was created in conjunction with this INO-file and contains all the Classes



////////////////////////////////// Detects the falling edge and stores the time when ist was detected int the "time_stamp".///
unsigned long int Sensor::GetTime()
{
 if (!digitalRead(TACHO) && signal_order)  // If falling edge: the current signal is low/0 and the last measurement was high/1 
  {
time_stamp = millis();  // get current timestamp (timestamp when the sensor made contact).
Serial.print("time_stamp: ");
Serial.println(time_stamp);
return time_stamp;  // Returns the timestamp at which falling edge was detected.
  }
else return 0;  // Returns 0 if no falling edge has been detected.
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////// Stores the current signal value for later comparison in the next iteration. ///////////////
void Sensor::SetSignalOrder()  
{
signal_order = digitalRead(TACHO); // 0 if contact, else 1
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////// Determines the time between the last detected falling edge and the currently detected falling edge ///
void TimeDifference::CalculateTimediff(const unsigned long int &t_stamp) 
{
  if (previous_time_stamp && previous_time_stamp < t_stamp) // If previous time stamp exists and it is < current timestamp "t_stamp"
  {
time_difference = t_stamp - previous_time_stamp;  // Calculation fo the time between the previous falling edge detection and the current falling edge detection
  }
  else 
  {
time_difference=0;  // If there is no previous falling edge detection, or the current falling edge has a smaller timestemp E.g: because of clock overflow
  }
Serial.print("current time_difference: ");
Serial.println(time_difference);
  previous_time_stamp = t_stamp;  // The time at which the current falling edge has been detected is now set as previous timestamp.
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////// Stores the current sample value in a buffer array (FIFO-princip).///////////////////
void Rpm::FillBuffer(const unsigned long int &time_difference)
{
if(sample_count < sample_size)  // If the number of samples in the buffer is less than the length of the buffer, 
                                // the current sample is attached behind the last sample in the abuffer.
{
   sample_buffer[sample_count]=time_difference; // The current sample is stored behind the last sample in the abuffer.
  ++sample_count; // increase of the sample count for the next iteration
}
else  // If the sample size is equal to the length of the buffer, the stored sample values are shifted to the left and the left
      // and the current sample value is stored at the end of the buffer. In this process, the oldest/first sample value is deleted.
{
for (int i=0;i<sample_size-1;++i) 
{ // The stored sample values are shifted to the left. In this process, the oldest/first sample value is deleted.
  sample_buffer[i] = sample_buffer[i+1];
  }
  sample_buffer[sample_size-1]=time_difference; // The current sample value is stored at the end of the buffer.
}
// Print the sample_buffer array ///////////////////////
Serial.print("sample_buffer: ");
for (int i=0;i<sample_count;++i) 
{
Serial.print(sample_buffer[i]);
Serial.print(".");
}
Serial.println("  ");
////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////// This method calculates the averaged timedifferences from the time-differences stored in the buffer.////
void Rpm::AverageFilter()
{
unsigned long int average_difference=0; // Temporary variable to store the sum of all sample values/time periods "time_difference"
for (int i=0;i<sample_count;++i)  // Builds a sum of sum of all sample valuse in the buffer.
{
average_difference += sample_buffer[i];
}

av_time_difference = average_difference / sample_count; // Calculates the average value of all sample valuse in the buffer.

Serial.print("AverageFilter av_time_difference: ");
Serial.println(av_time_difference);

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////// This method calculates the averaged timedifferences by applying the medianfilter to the buffer.////
void Rpm::MedianFilter(const unsigned long int &time_difference)
{
 
 if(sample_count<=1) // If the number of samples is <= 1, the filter is not applied and the current time_difference is taken as it is. 
  {
    av_time_difference = time_difference;
  }
  else // If the number of samples is > 1, the medianfilter is applied.
  ///////// Bubble sort to sort the sample-values in ascending order.////////////////////////
  { 
    int i, j; // Internal count variables
    unsigned long int temp_sample_value=0; // Internal buffervariable to temporarily store a sample value.
    unsigned long int sorted_array[sample_count];  // Internal array to temporarily store the sorted sample values.
    for (i = 0; i < sample_count; ++i) 
    {
      sorted_array[i] = sample_buffer[i];
    }
    for (i = 0; i < sample_count - 1; ++i) 
    {
     for (j = 0; j < sample_count - i - 1; ++j) 
     {
         if (sorted_array[j] > sorted_array[j + 1]) 
         {
           temp_sample_value=sorted_array[j+1];
           sorted_array[j + 1]=sorted_array[j];
           sorted_array[j]=temp_sample_value;
          }
      }
    }
//////////////////////////////////////////////////////////////////////////////////////////////
// Print the sorted_array //////////////////////////////
Serial.print("sorted_array: ");
for (int i=0;i<sample_count;++i) 
{
Serial.print(sorted_array[i]);
Serial.print(".");
}
Serial.println("  ");
////////////////////////////////////////////////////////
    av_time_difference = sorted_array[sample_count/2];  // Stores the center sample value form the sorted array.
                                                        // This is called the median"Filter". All the outlier are omitted, 
                                                        // since thy are either at the beginn, or at the end of the sorted array. 

Serial.print("Medianfilter av_time_difference: ");
Serial.println(av_time_difference);
  }

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////// This method calculates the averaged timedifferences by combining the medianfilter and averagefilter to the buffer.////
void Rpm::MedianAverageFilter(const unsigned long int &time_difference)
{
 // Medianfilter ///////////////////////////////////////////////////////////
 if(sample_count<=1) // If the number of samples is <= 1, the filter is not applied and the current time_difference is taken as it is. 
  {
    av_time_difference = time_difference;
  }
  else // If the number of samples is > 1, the medianfilter is applied.
  ///////// Bubble sort to sort the sample-values in ascending order.////////////////////////////////////////
  { 
    int i, j; // Internal count variables
    unsigned long int temp_sample_value=0; // Internal buffervariable to temporarily store a sample value.
    unsigned long int sorted_array[sample_count];  // Internal array to temporarily store the sorted sample values.
    for (i = 0; i < sample_count; ++i) 
    {
      sorted_array[i] = sample_buffer[i];
    }

    for (i = 0; i < sample_count - 1; ++i) {
     for (j = 0; j < sample_count - i - 1; ++j) 
     {
         if (sorted_array[j] > sorted_array[j + 1]) 
         {
           temp_sample_value=sorted_array[j+1];
           sorted_array[j + 1]=sorted_array[j];
           sorted_array[j]=temp_sample_value;
            }
        }
 
    }
////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Print the sorted_array /////////////////////////
Serial.print("median_avearage_sorted_array: ");
for (int i=0;i<sample_count;++i) 
{
Serial.print(sorted_array[i]);
Serial.print(".");
}
Serial.println("  ");
////////////////////////////////////////////////////////

av_time_difference = sorted_array[sample_count/2];// Stores the center sample value form the sorted array.
                                                  // This is called the median"Filter". All the outlier are omitted, 
                                                  // since thy are either at the beginn, or at the end of the sorted array. 

Serial.print("Medianfilter av_time_difference: ");
Serial.println(av_time_difference);

}
/// END of Medianfilter/////////////////////////////////////////////////////////////////////////////////////

// Buffer, in which the Median-filtered sample values are stored. ///////////////////////////////////////////
// Stores the current sample value in a buffer array (FIFO-princip).
if(median_sample_count < sample_size)  // If the number of samples in the buffer is less than the length of the buffer, 
                                // the current sample is attached behind the last sample in the abuffer.
{
   median_sample_buffer[median_sample_count]=av_time_difference;  // The current sample is stored behind the last sample in the abuffer.
  ++median_sample_count; // increase of the sample count for the next iteration
  
}
else  // If the sample size is equal to the length of the buffer, the stored sample values are shifted to the left and the left
      // and the current sample value is stored at the end of the buffer. In this process, the oldest/first sample value is deleted.
{
for (int i=0;i<sample_size-1;++i) 
{ // The stored sample values are shifted to the left. In this process, the oldest/first sample value is deleted.
  median_sample_buffer[i] = median_sample_buffer[i+1];
  }
  median_sample_buffer[sample_size-1]=av_time_difference; // The current sample value is stored at the end of the buffer.
}
// Print the sample_buffer array ////////////////////
Serial.print("median_sample_buffer: ");
for (int i=0;i<median_sample_count;++i) 
{
Serial.print(median_sample_buffer[i]);
Serial.print(".");
}
Serial.println("  ");
////////////////////////////////////////////////////////
// END of Buffer for Median-filtered sample values //////////////////////////////////////////////////////////

// Averagefilter is applied to the Buffer with Median-filtered sample values////////////////////////////////
unsigned long int median_average_difference=0; // Temporary variable to store the sum of all sample values/time periods "time_difference"
 for (int i=0;i<median_sample_count;++i)  // Builds a sum of sum of all sample valuse in the buffer.
{

median_average_difference += median_sample_buffer[i];
}

av_time_difference = median_average_difference / median_sample_count; // Calculates the average value of all Median-Filter sample valuse in the buffer.

Serial.print("MedianAverageFilter av_time_difference: ");
Serial.println(av_time_difference);
}
/// END of Averagefilter ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////// Applies a filter selected in the menue. /////////////////////////////////////////////////
void Rpm::ApplyFilter(const unsigned long int &time_difference)
{
 

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////// This method calculates the revolutions per minute "rpm" from the "av_time_difference".///
void Rpm::CalculateRpm()
{
 rpm = 3000 / av_time_difference; // 3000 => (60sec * 1000ms) / 20magnets "sensors/pings per revolution"
 Serial.print("rpm: ");
Serial.println(rpm);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////// This method resets the rpm to 0 and clears/zeroizes all the respective buffers and variables.////
void Rpm::ResetRpm(Sensor & sensor,TimeDifference & timedifference)
{
 sample_count = 0;
 median_sample_count = 0;
 av_time_difference = 0;
 rpm = 0;
timedifference.time_difference =0;
sensor.time_stamp =0;
timedifference.previous_time_stamp=0;

// Print values:
Serial.print("sample_count: ");
Serial.println(sample_count);
Serial.print("median_sample_count: ");
Serial.println(median_sample_count);
Serial.print("av_time_difference: ");
Serial.println(av_time_difference);
Serial.print("rpm: ");
Serial.println(rpm);
Serial.print("time_difference: ");
Serial.println(timedifference.time_difference);
Serial.print("previous_time_stamp: ");
Serial.println(timedifference.previous_time_stamp);
Serial.print("sample_buffer[");
for(int i = 0;i<sample_size;++i)  // Zeroizes all the values in the array/buffer
{
  sample_buffer[i] = 0;
  
  Serial.print(i);
}
Serial.println("] zeroized");
Serial.print("median_sample_buffer[");
for(int i = 0;i<sample_size;++i)  // Zeroizes all the values in the array/buffer
{
  median_sample_buffer[i] = 0;
  
  Serial.print(i);
}
Serial.println("] zeroized");
Serial.println("---------------------------------------------------------------------------------");


 //delete[] time_differences;
 //time_differences = new unsigned short int[sample_size];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////// Transforms the measured timedifferenzes between signal-falling edges to Axisvalues.////////////////////////////////////////////////////////////////////////////////////////
void AxisValue::CalculateAxisValue(const Profile & profile, const Rpm & rpm, Sensor &sensor)
{
  //  If the measured timedifferenzes between signal-falling edges are outside the defined min_rpm & max_rpm, the axisvalues are set to their maximum and minimum values respectively
 if(rpm.av_time_difference <= (60000/SENSORS)/profile.max_rpm)  // If the av_time_difference <= the av_time_difference at max_rpm,
                                                                // the axisvalue is set to maximum axisvalue
                                                                // 60000 => (60sec * 1000ms) / 20magnets "sensors/pings per revolution"
  {
  throttle = AXISMAXIMUM;  // the axisvalue is set to maximum value
  }// handeling the minimum range of the velocity/throttle range if the ~rpm <= min rpm ==> throttle =0;
  else if(rpm.av_time_difference >= (60000/SENSORS)/profile.min_rpm)  // If the av_time_difference >= the av_time_difference at min_rpm,
                                                                      // the axisvalue is set to minimum axisvalue
                                                                      // 60000 => (60sec * 1000ms) / 20magnets "sensors/pings per revolution"
  {
throttle = AXISMINIMUM;
   }
   else { // The av_time_difference is converted to the axisvalue
    throttle = AXISMAXIMUM *(static_cast <double>((60000/SENSORS)/static_cast <double>(rpm.av_time_difference)-profile.min_rpm)/static_cast <double>(profile.max_rpm - profile.min_rpm));
   }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////// Resets the Axisvalue to zero if no falling edge has been detected for a prolonged time.////////////////////////////////////////////////////////////////////////////////////////
void AxisValue::ResetAxisValue(const Profile & profile,Rpm & rpm,Sensor &sensor,TimeDifference & timedifference)
{
if (millis() >= (sensor.time_stamp + (2 * (60000/SENSORS)/profile.min_rpm)) && throttle != AXISMINIMUM )  // Verifies whether a falling edge has been detected within a certain time period,
                                                                                                // whilst the axisvalue is not at its minimum.
                                                                                                // Since the Axisvalue is calculate only when a falling edge has been detected, the Axisvalu would be stuck
                                                                                                // on the last calculated value, if no new falling edge has been detected. E.g: abrupt stop of the rotation.
{
  throttle = AXISMINIMUM;
  rpm.ResetRpm(sensor,timedifference);  // Zeroizes all the values relavant for the axisvalue calculation.
}
/*
Serial.print("---------------------------------Throttle: ");
Serial.println(throttle);
*/
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////// Sets/sends the calculated Axisvalues to the Joystick.///////////////////////////////////
void AxisValue::SetAxisValue(Joystick_ &Joystick )
{
Joystick.setThrottle(throttle); // Sets/sends the calculated Axisvalues to the Joystick.
}
/////////////////////////////////// When the menubutton is pressed, the menu number "menu_count" is iterated.///////////////
void UserInput:: NavigateMenu(LiquidCrystal_I2C &lcd,Profile &ProfileCurrent)
{
if(GetInput(ROTBUT))
{
  if(menu_count < static_cast<int>(UserInput::MENUSIZE-1))
  {
    ++menu_count;
  }
  else
  {
    menu_count = static_cast<int>(UserInput::STARTMENU);
  }
  LCDprint(lcd,ProfileCurrent);
  Serial.print("Switched Menu to: ");
  Serial.println(menu_count);
}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////This method resets the microcontroller to its default state //////////////////////////////
void UserInput:: Reset(LiquidCrystal_I2C &lcd,Profile &ProfileCurrent) //When the menubutton is pressed, all outputs rpm, Axisvalue, LCD-Display are reset to 0,
                        // or their default values and all respective buffers and variables are cleared/zeroized.
{
if(GetInput(RSETBUT))
{
  Serial.println("Resetting: ");
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

LCDprint(lcd,ProfileCurrent);
}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////This is the visualization of the menues and respective parameters on the LCD. ///////////
const void UserInput::LCDprint(LiquidCrystal_I2C &lcd,Profile &ProfileCurrent )
{
  lcd.clear();
  lcd.setCursor (0,0); // set cursor at line 0, char 0
  switch(menu_count)
  {
    case STARTMENU:
{ 
  lcd.print("Prof:"); 
  lcd.setCursor (6,0);
  lcd.print("_TODO_");  //  TODO lcd.print(ProfileCurrent.profile_name);
  lcd.setCursor (0,1);
  lcd.print(ProfileCurrent.min_rpm);  //  TODO
  lcd.setCursor (4,1);
  lcd.print(ProfileCurrent.max_rpm);  //  TODO
  lcd.setCursor (8,1);
  lcd.print(ProfileCurrent.filter_name);
  lcd.setCursor (13,1);
  lcd.print(ProfileCurrent.sample_size);
}         
   break;
       case PROFILE_LOAD:
{ 
  lcd.print("Load?:"); 
  lcd.setCursor (6,0);
  lcd.print("_TODO_");  //  TODO lcd.print(ProfileCurrent.profile_name);
  lcd.setCursor (0,1);
  lcd.print(ProfileCurrent.min_rpm);  //  TODO
  lcd.setCursor (4,1);
  lcd.print(ProfileCurrent.max_rpm);  //  TODO
  lcd.setCursor (8,1);
  lcd.print(ProfileCurrent.filter_name);
  lcd.setCursor (11,1);
  if(consent_load)
  {
    lcd.print("(YES)");
  }
  else
  {
    lcd.print("( NO)");
  }
}
   break;
       case MINRPM:
{ 
  lcd.print("(MIN)MAX FLT SN"); 
  lcd.setCursor (1,1);
  lcd.print(ProfileCurrent.min_rpm);
  lcd.setCursor (5,1);
  lcd.print(ProfileCurrent.max_rpm);
  lcd.setCursor (9,1);
  lcd.print(ProfileCurrent.filter_name);
  lcd.setCursor (13,1);
  lcd.print(ProfileCurrent.sample_size);
}
   break;
       case MAXRPM:
{ 
  lcd.print(" MIN(MAX)FLT SN"); 
  lcd.setCursor (1,1);
  lcd.print(ProfileCurrent.min_rpm);
  lcd.setCursor (5,1);
  lcd.print(ProfileCurrent.max_rpm);
  lcd.setCursor (9,1);
  lcd.print(ProfileCurrent.filter_name);
  lcd.setCursor (13,1);
  lcd.print(ProfileCurrent.sample_size);
}  
   break;
       case FILTER:
{ 
  lcd.print(" MIN MAX(FLT)SN"); 
  lcd.setCursor (1,1);
  lcd.print(ProfileCurrent.min_rpm);
  lcd.setCursor (5,1);
  lcd.print(ProfileCurrent.max_rpm);
  lcd.setCursor (9,1);
  lcd.print(ProfileCurrent.filter_name);
  lcd.setCursor (13,1);
  lcd.print(ProfileCurrent.sample_size);
}
   break;
       case SAMPLE_SIZE: //SAMPLE_SIZE
{ 
  lcd.print(" MIN MAX FLT(SN)"); 
  lcd.setCursor (1,1);
  lcd.print(ProfileCurrent.min_rpm);
  lcd.setCursor (5,1);
  lcd.print(ProfileCurrent.max_rpm);
  lcd.setCursor (9,1);
  lcd.print(ProfileCurrent.filter_name);
  lcd.setCursor (13,1);
  lcd.print(ProfileCurrent.sample_size);
}
   break;
       case PROFILE_SAVE:
{ 
  lcd.print("Save USER?:"); 
  lcd.setCursor (11,0);
  if(consent_save)
  {
    lcd.print("(YES)");
  }
  else
  {
    lcd.print("( NO)");
  }
  lcd.setCursor (1,1);
  lcd.print(ProfileCurrent.min_rpm);
  lcd.setCursor (5,1);
  lcd.print(ProfileCurrent.max_rpm);
  lcd.setCursor (9,1);
  lcd.print(ProfileCurrent.filter_name);
  lcd.setCursor (13,1);
  lcd.print(ProfileCurrent.sample_size);
}
   break;
      default:
{
  Serial.println("Menu Error!!!!!!!!!! ");
}
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////This method changes the parameters according to the active menue and the rotary encoder inputs.//////////////////////////
void UserInput::ChangeParameter(LiquidCrystal_I2C &lcd,Profile &ProfileCurrent)
{
  unsigned short int result = CheckRotaries();
  switch(menu_count)
  {
    case STARTMENU:
{ 
      if (consent_save)
  {
    consent_save = false;
    Serial.println("PLACEHOLDER Saving Profile: TODO");
    Serial.print("consent_save set to: ");
    Serial.println(consent_save);
    Serial.println("---------------------------------------------------");
  }

  if (result == DIR_CCW)
  { 
  Serial.println("PLACEHOLDER Profile decrease: TODO");
  Serial.println("---------------------------------------------------");
  LCDprint(lcd,ProfileCurrent);
  }
  else if(result == DIR_CW)
  {
  Serial.println("PLACEHOLDER Profile increase: TODO");
  Serial.println("---------------------------------------------------");
  LCDprint(lcd,ProfileCurrent);
  }
}
   break;
       case PROFILE_LOAD:
{ 
  if (result == DIR_CCW)
  { 
  consent_load = false;
  Serial.print("consent_load = ");
  Serial.println(consent_load);
  Serial.println("---------------------------------------------------");
  LCDprint(lcd,ProfileCurrent);
  }
  else if(result == DIR_CW)
  {
    consent_load = true;
  Serial.print("consent_load = ");
  Serial.println(consent_load);
  Serial.println("---------------------------------------------------");
  LCDprint(lcd,ProfileCurrent);
  }

}
   break;
       case MINRPM:
{ 
    if (consent_load)
  {
    consent_load = false;
    Serial.println("PLACEHOLDER Loading Profile: TODO");
    Serial.print("consent_load set to: ");
    Serial.println(consent_load);
    Serial.println("---------------------------------------------------");
  }

  if (result == DIR_CCW)
  {
    if(ProfileCurrent.min_rpm > RPMMIN)
    {
    --ProfileCurrent.min_rpm;
    Serial.print("Min_rpm:");
    Serial.println(ProfileCurrent.min_rpm);
    Serial.println("---------------------------------------------------");
    }
    else
    {
     ProfileCurrent.min_rpm = RPMMIN;
    }
    LCDprint(lcd,ProfileCurrent);
  }
  else if(result == DIR_CW)
  {
    if(ProfileCurrent.min_rpm < ProfileCurrent.max_rpm - 5)
    {
    ++ProfileCurrent.min_rpm;
    Serial.print("Min_rpm:");
    Serial.println(ProfileCurrent.min_rpm);
    Serial.println("---------------------------------------------------");
    }
    else
    {
     ProfileCurrent.min_rpm = ProfileCurrent.max_rpm - 5;
    }
    LCDprint(lcd,ProfileCurrent);
  }
}
   break;
       case MAXRPM:
{ 
  if (result == DIR_CCW)
  {
    if(ProfileCurrent.max_rpm > ProfileCurrent.min_rpm + 5)
    {
    --ProfileCurrent.max_rpm;
    Serial.print("Max_rpm:");
    Serial.println(ProfileCurrent.max_rpm);
    Serial.println("---------------------------------------------------");
    }
    else
    {
     ProfileCurrent.max_rpm = ProfileCurrent.min_rpm + 5;
    }
    LCDprint(lcd,ProfileCurrent);
  }
  else if(result == DIR_CW)
  {
    if(ProfileCurrent.max_rpm < RPMMAX)
    {
    ++ProfileCurrent.max_rpm;
    Serial.print("Max_rpm:");
    Serial.println(ProfileCurrent.max_rpm);
    Serial.println("---------------------------------------------------");
    }
    else
    {
     ProfileCurrent.max_rpm = RPMMAX;
    }
    LCDprint(lcd,ProfileCurrent);
  }
  
}  
   break;
       case FILTER:
{ 
  if (result == DIR_CCW)
  { 
  Serial.println("PLACEHOLDER Filter decrease: TODO");
  Serial.println("---------------------------------------------------");
  LCDprint(lcd,ProfileCurrent);
  }
  else if(result == DIR_CW)
  {
  Serial.println("PLACEHOLDER Filter increase: TODO");
  Serial.println("---------------------------------------------------");
  LCDprint(lcd,ProfileCurrent);
  }
}
   break;
       case SAMPLE_SIZE:
{ 
  if (result == DIR_CCW)
  {
    if(ProfileCurrent.sample_size > SAMPLEMIN )
    {
    --ProfileCurrent.sample_size;
    Serial.print("Sample_size:");
    Serial.println(ProfileCurrent.sample_size);
    Serial.println("---------------------------------------------------");
    }
    else
    {
     ProfileCurrent.sample_size = SAMPLEMIN;
    }
    LCDprint(lcd,ProfileCurrent);
  }
  else if(result == DIR_CW)
  {
    if(ProfileCurrent.sample_size < SAMPLEMAX)
    {
    ++ProfileCurrent.sample_size;
    Serial.print("Sample_size:");
    Serial.println(ProfileCurrent.sample_size);
    Serial.println("---------------------------------------------------");
    }
    else
    {
     ProfileCurrent.sample_size = SAMPLEMAX;
    }
    LCDprint(lcd,ProfileCurrent);
  }
}
   break;
       case PROFILE_SAVE:
{ 
  if (result == DIR_CCW)
  { 
    consent_save = false;
  Serial.print("consent_save = ");
  Serial.println(consent_save);
  Serial.println("---------------------------------------------------");
  LCDprint(lcd,ProfileCurrent);
  }
  else if(result == DIR_CW)
  {
    consent_save = true;
  Serial.print("consent_save = ");
  Serial.println(consent_save);
  Serial.println("---------------------------------------------------");
  LCDprint(lcd,ProfileCurrent);
  }
}
   break;

default:
{
  Serial.println("Menu Error!!!!!!!!!! ");
}
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////// Checks the current state of rotary encoder signal befor the Loop beginn.////////////////////////////////
void UserInput::InitRot()
{
  for(int i=0;i< NUMROTARIES;++i)
  {
  rot_state[1] = digitalRead(rot_pin1);
  }
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////// Reads the state of the given rotary (number).////////////////////////////////////
unsigned short int UserInput::CheckRotaries()
{
rot_state[0] = digitalRead(rot_pin1); // Reads the "current" state of the rot_pin1 

  if (rot_state[0] != rot_state[1] && (millis()-rot_delay)>ROTDELAY) // If the previous and the current state of the rot_pin1 are different, 
                                                          // that means a Pulse has occurred
  {
    if(digitalRead(rot_pin2) != rot_state[0]) // If the rot_pin2 state is different to the rot_pin1 state, 
                                                                    // that means the encoder is rotating clockwise 
    {
    rot_dir = static_cast<int>(UserInput::DIR_CW);
    rot_delay = millis();
    }
    else
    {
    rot_dir = static_cast<int>(UserInput::DIR_CCW);
    rot_delay = millis();
    }
  }
  else
  {
   // Serial.println("UserInput::NONE");
  rot_dir = static_cast<int>(UserInput::DIR_NONE);
  }
 rot_state[1] = rot_state[0];// Updates the previous state of the outputA with the current state
 return rot_dir;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////// Reads the signals on the given pin (number).////////////////////////////////////////
bool UserInput::GetInput(const unsigned short int i)
{
  bool result;
  if (!digitalRead(i) && !pin_state[i] && (millis()-button_delay)>BUTTONDELAY) // If the previous and the current state of the rot_pin1 are different, 
                                            // that means a Pulse has occurred (rising )
  {
  pin_state[i] = 1;
  result = true;
  button_delay = millis();
  }
  else if(digitalRead(i) && pin_state[i])
  {
  pin_state[i] = 0;
  result = false;
  button_delay = millis();
  }
  else
  {
  result = false;
  }
  return result;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////






#endif