#include "SpeedometerClass.h" // This library was created in conjunction with this INO-file and contains all the Classes



////////////////////////////////// Detects the falling edge and stores the time when ist was detected int the "time_stamp".///
unsigned long int Sensor::GetTime()
{
 if (!digitalRead(ROTARY) && signal_order)  // If falling edge: the current signal is low/0 and the last measurement was high/1 
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
signal_order = digitalRead(ROTARY); // 0 if contact, else 1
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
    // Print the sorted_array ////////////////////
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
Serial.print("sample_count: ");
Serial.println(sample_count);
 median_sample_count = 0;
Serial.print("median_sample_count: ");
Serial.println(median_sample_count);
 av_time_difference = 0;
 Serial.print("av_time_difference: ");
Serial.println(av_time_difference);
 rpm = 0;
 Serial.print("rpm: ");
Serial.println(rpm);

timedifference.time_difference =0;
 Serial.print("time_difference: ");
Serial.println(timedifference.time_difference);

sensor.time_stamp =0;

timedifference.previous_time_stamp=0;
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
  throttle = axis_maximum;  // the axisvalue is set to maximum value
  }// handeling the minimum range of the velocity/throttle range if the ~rpm <= min rpm ==> throttle =0;
  else if(rpm.av_time_difference >= (60000/SENSORS)/profile.min_rpm)  // If the av_time_difference >= the av_time_difference at min_rpm,
                                                                      // the axisvalue is set to minimum axisvalue
                                                                      // 60000 => (60sec * 1000ms) / 20magnets "sensors/pings per revolution"
  {
throttle = axis_minimum;
   }
   else { // The av_time_difference is converted to the axisvalue
    throttle = axis_maximum *(static_cast <double>((60000/SENSORS)/static_cast <double>(rpm.av_time_difference)-profile.min_rpm)/static_cast <double>(profile.max_rpm - profile.min_rpm));
   }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////// Resets the Axisvalue to zero if no falling edge has been detected for a prolonged time.////////////////////////////////////////////////////////////////////////////////////////
void AxisValue::ResetAxisValue(const Profile & profile,Rpm & rpm,Sensor &sensor,TimeDifference & timedifference)
{
if (millis() >= (sensor.time_stamp + (2 * (60000/SENSORS)/profile.min_rpm)) && throttle != axis_minimum )  // Verifies whether a falling edge has been detected within a certain time period,
                                                                                                // whilst the axisvalue is not at its minimum.
                                                                                                // Since the Axisvalue is calculate only when a falling edge has been detected, the Axisvalu would be stuck
                                                                                                // on the last calculated value, if no new falling edge has been detected. E.g: abrupt stop of the rotation.
{
  throttle = axis_minimum;
  rpm.ResetRpm(sensor,timedifference);  // Zeroizes all the values relavant for the axisvalue calculation.
}
/*
Serial.print("---------------------------------Throttle: ");
Serial.println(throttle);
*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////// Sets/sends the calculated Axisvalues to the Joystick./////////////////////////////////////////////////////////////////////////////////////////
void AxisValue::SetAxisValue(Joystick_ &Joystick )
{
Joystick.setThrottle(throttle); // Sets/sends the calculated Axisvalues to the Joystick.
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////






