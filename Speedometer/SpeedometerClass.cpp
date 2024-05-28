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
  if (previous_time_stamp && previous_time_stamp < t_stamp)
  {
time_difference = t_stamp - previous_time_stamp;
  }
  else 
  {
time_difference=0;
  }
Serial.print("current time_difference: ");
Serial.println(time_difference);
  previous_time_stamp = t_stamp;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////// Stores the current sample value in a buffer array (FIFO-princip).///////////////////
void Rpm::FillBuffer(const unsigned long int &time_difference)
{
if(sample_count < sample_size)  // If the number of samples in the buffer is less than the length of the buffer, 
                                // the current sample is attached behind the last sample in the abuffer.
{
  
   sample_buffer[sample_count]=time_difference;
  ++sample_count;
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
// Print the sample_buffer array ////////////////////
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
unsigned long int average_difference=0;
for (int i=0;i<sample_count;++i)
{
average_difference += sample_buffer[i];
}

av_time_difference = average_difference / sample_count;

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
    // Print the sorted_array ////////////////////
Serial.print("sorted_array: ");
for (int i=0;i<sample_count;++i) 
{
Serial.print(sorted_array[i]);
Serial.print(".");
}
Serial.println("  ");
////////////////////////////////////////////////////////

    av_time_difference = sorted_array[sample_count/2];

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
  ///////// Bubble sort to sort the sample-values in ascending order.////////////////////////
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
    // Print the sorted_array ////////////////////
Serial.print("median_avearage_sorted_array: ");
for (int i=0;i<sample_count;++i) 
{
Serial.print(sorted_array[i]);
Serial.print(".");
}
Serial.println("  ");
////////////////////////////////////////////////////////

    av_time_difference = sorted_array[sample_count/2];
Serial.print("Medianfilter av_time_difference: ");
Serial.println(av_time_difference);

}
////////////////////////////////////////////////////////////

// Stores the current sample value in a buffer array (FIFO-princip).
if(median_sample_count < sample_size)  // If the number of samples in the buffer is less than the length of the buffer, 
                                // the current sample is attached behind the last sample in the abuffer.
{
  
   median_sample_buffer[median_sample_count]=av_time_difference;
  ++median_sample_count;
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

// Averagefilter ////////////////////////////////////////
unsigned long int median_average_difference=0;
 for (int i=0;i<median_sample_count;++i)
{

median_average_difference += median_sample_buffer[i];
}

av_time_difference = median_average_difference / median_sample_count;

Serial.print("MedianAverageFilter av_time_difference: ");
Serial.println(av_time_difference);
}
///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////// Applies a filter selected in the menue. /////////////////////////////////////////////////
void Rpm::ApplyFilter(const unsigned long int &time_difference)
{
 
if(sample_count < sample_size)
{
  sample_buffer[sample_count]=time_difference;
  ++sample_count;
}
else 
{
for (int i=0;i<sample_size-1;++i) 
{
   //Throttle_[CountLimit_ - i -1] = Throttle_[CountLimit_ - i];
   sample_buffer[sample_size-1 - i] = sample_buffer[sample_size -2 - i];
  }
  sample_buffer[0]=time_difference;
}
Serial.print("sample_buffer: ");
for (int i=0;i<sample_count;++i) 
{
Serial.print(sample_buffer[i]);
Serial.print(".");
}
Serial.println("  ");

for (int i=0;i<sample_count;++i) 
av_time_difference += sample_buffer[i];

av_time_difference /= sample_count;

Serial.print("av_time_difference: ");
Serial.println(av_time_difference);

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
for(int i = 0;i<sample_size;++i)
{
  sample_buffer[i] = 0;
  
  Serial.print(i);
}
Serial.println("] zeroized");
 Serial.print("median_sample_buffer[");
for(int i = 0;i<sample_size;++i)
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




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////







