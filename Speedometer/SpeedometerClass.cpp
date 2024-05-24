#include "SpeedometerClass.h" // This library was created in conjunction with this INO-file and contains all the Classes




unsigned long int Sensor::GetTime()
{
 if (!digitalRead(ROTARY) && signal_order)  // If falling edge: the current signal is low/0 and the last measurement was high/1 
  {
time_stamp = millis();  // get current timestamp (when the sensor made contact)
Serial.print("time_stamp: ");
Serial.println(time_stamp);
return time_stamp;
  }
else return 0;
}




void Sensor::SetSignalOrder()  // stores the current signal value for later comparison
{
signal_order = digitalRead(ROTARY); // 0 if contact, else 1
}

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
Serial.print("time_difference: ");
Serial.println(time_difference);
  previous_time_stamp = t_stamp;
}

void Rpm::CalculateRpm()
{
 rpm = 3000 / av_time_difference; // 3000 => (60sec * 1000ms) / 20magnets "sensors/pings per revolution"
 Serial.print("rpm: ");
Serial.println(rpm);
}

void Rpm::ApplyFilter(const unsigned long int &time_difference)
{
 
if(sample_count < sample_size)
{
  time_differences[sample_count]=time_difference;
  ++sample_count;
}
else 
{
for (int i=0;i<sample_size-1;++i) 
{
   //Throttle_[CountLimit_ - i -1] = Throttle_[CountLimit_ - i];
   time_differences[sample_size-1 - i] = time_differences[sample_size -2 - i];
  }
  time_differences[0]=time_difference;
}
Serial.print("time_differences: ");
for (int i=0;i<sample_count;++i) 
{
Serial.print(time_differences[i]);
Serial.print(".");
}
Serial.println("  ");

for (int i=0;i<sample_count;++i) 
av_time_difference += time_differences[i];

av_time_difference /= sample_count;

Serial.print("av_time_difference: ");
Serial.println(av_time_difference);

}

void Rpm::ResetRpm()
{
 sample_count = 0;
 av_time_difference = 0;
 rpm = 0;
for(int i = 0;i<sample_size;++i)
{
  time_differences[i] = 0;
}

 //delete[] time_differences;
 //time_differences = new unsigned short int[sample_size];
}