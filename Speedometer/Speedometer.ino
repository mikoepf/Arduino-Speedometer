//#include <Keypad.h>         // A library for using matrix style keypads with the Arduino.
//#include <Joystick.h>       // A library to emulate a Joystick with buttons, switches and axis
//#include <Wire.h>           // This library allows you to communicate with I2C devices
//#include <LiquidCrystal_I2C.h>  // This library allows you to communicate with I2C Liquid crystal Display
#include "SpeedometerClass.h" // This library was created in conjunction with this INO-file and contains all the Classes

// Joystick declaration;

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
  JOYSTICK_TYPE_JOYSTICK, 0, 0,
  false, false, false, false, false, false, 
  false, true, false, false, false);  // The Joystick will have only one Throttle-Axis

LiquidCrystal_I2C lcd(0x27, 16, 2);   // LCD parameter definitions
bool Pin [21];   // An array to store the Pin-values ==> for test purposes

// Testvariables declaration
void SetJoystick();
double Throttle=0;

//////////////////////////////////////

// Classes and Struct Declarations /////////////////////////////////////////////////////////////////


const Profile ProfileDefault={"STD","AVR", 85, 25, 6};
Profile ProfileCurrent;
Sensor Ping(0,0);
TimeDifference DeltaTime(0,0);
Rpm Revs(ProfileDefault);


//  END of Declarations  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

void setup() {
  // The setup code is run only once, no Declarations can be performed here!

  // Initialisation of the serial Monitor
 Serial.begin(9600);
 Serial.println("Serialprint initialised");
 Serial.println(__LINE__);  //  Print the current code-Line on the serial Monitor

 // Initialisation of the Joystick
      Joystick.begin();
      Joystick.setThrottleRange(axis_minimum,axis_maximum);

 // Initialisation of the LCd
 	lcd.begin();  // initialize the LCD, 
	lcd.backlight();  // Turn on the backlight and print a message.
  lcd.clear();
  lcd.setCursor (0,0); // set cursor at char 0, line 0
  lcd.print("First line");  
  lcd.setCursor (0,1); // set cursor at char 0, line 1
  lcd.print("Second line");

  // Pulling up the built in PIN-pullups:
  for (int i=0;i< pins;++i)
{
 pinMode(i,INPUT_PULLUP);
}

// Initialisation of the Current Profile

ProfileCurrent = ProfileDefault;


} //  END of Setup  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

void loop() {
  // put your main code here, to run repeatedly:



if(Ping.GetTime())
{

DeltaTime.CalculateTimediff(Ping.time_stamp);


//Revs.ApplyFilter(DeltaTime.time_difference);


if(DeltaTime.time_difference) // Since with only the first signal the timedifference and the subsequent values can´t be calculated,
                              // the subsequent will be calculated only after the second signal was received.
{

//Revs.FillBuffer(DeltaTime.time_difference);
//Revs.AverageFilter();

//Revs.FillBuffer(DeltaTime.time_difference);
//Revs.MedianFilter(DeltaTime.time_difference);

Revs.FillBuffer(DeltaTime.time_difference);
Revs.MedianAverageFilter(DeltaTime.time_difference);

/* Das geht !!!!!!!!!!!!!
unsigned long int median_average_difference=0;
++median_average_difference;
*/

// Das geht nicht?????!!!!!!
//Profile testprofile;


//Revs.FillBuffer(Revs.av_time_difference);
//Revs.AverageFilter();
Revs.CalculateRpm();

}
Serial.println("---------------------------------------------- ");
}
Ping.SetSignalOrder();

SetJoystick(ProfileCurrent, Revs, Ping);

if (!digitalRead(RSETBUT))
{

Serial.println("!!!!!!!!!!! reset button pressed !!!!!!!!!!!!!! ");
Revs.ResetRpm(Ping,DeltaTime);
}

}

//  END of Loop, start of definitions  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

/////////////////////////////////////////////////////////////// FUnction to check the Buttons ///////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////// FUnction to check the Buttons //////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////// FUnction to check the Buttons //////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////// FUnction to check the Buttons //////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////// FUnction to check the Buttons //////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////// FUnction to check the Buttons //////////////////////////////////////////////////////////////////////////


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! TEST_FUNCTIONS !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!




//////////////////////////////////////////////////////////////// FUnction to check the Joystick_print //////////////////////////////////////////////////////////////////////////

void SetJoystick(const Profile & profile, Rpm & rpm, const Sensor &sensor)
{
  
 if(rpm.av_time_difference <= 3000/profile.max_rpm)
  {
  Throttle = axis_maximum;

   }// handeling the minimum range of the velocity/throttle range if the ~rpm <= min rpm ==> throttle =0;
   else if(rpm.av_time_difference >= 3000/profile.min_rpm)
   {
Throttle = 0;
   }
   else {
    Throttle = axis_maximum *(static_cast <double>(3000/static_cast <double>(rpm.av_time_difference)-profile.min_rpm)/static_cast <double>(profile.max_rpm - profile.min_rpm));
    //Throttle = axis_maximum * (100 - (rpm.av_time_difference - (3000/profile.max_rpm))*100 / ((3000/profile.min_rpm) - (3000/profile.max_rpm))) / 100;
   }
   
if (millis() >= (sensor.time_stamp + (2 * 3000/profile.min_rpm)) && Throttle != 0 )
{
  Throttle = 0;
}

Joystick.setThrottle(Throttle);
/*
Serial.print("---------------------------------Throttle: ");
Serial.println(Throttle);
Serial.print("---------------------------------sensor.time_stamp + = ");
Serial.println((2 * 3000/profile.min_rpm));
*/
}



//////////////////////////////////////////////////////////////// FUnction to check the Buttons //////////////////////////////////////////////////////////////////////////





//////////////////////////////////////////////////////////////// Test-FUnction to check the Buttons //////////////////////////////////////////////////////////////////////////

void CheckButtons(void)
{
 for (int i=0;i<pins;++i)
{

  if (!digitalRead(i) && Pin[i] == 0)
  {
  
  Serial.print("Pin#: ");
  Serial.print(i);
  Serial.print(" has been pressed... see line ");
  Serial.println(__LINE__);  //  Print the current code-Line on the serial Monitor

  lcd.clear();
  lcd.setCursor (0,0); // set cursor at char 0, line 0
  lcd.print("Pin#: ");  
  lcd.setCursor (9,0); // set cursor at char 9, line 0
  lcd.print(i);
  lcd.setCursor (0,1); // set cursor at char 0, line 1
  lcd.print("has been pressed.");    

  PrintValues();

  Pin[i]=1;
}
else if(digitalRead(i) != 0 && Pin[i]==1)
{
  Pin[i]=0;
}
}
}

//////////////////////////////////////////////////////////////// Test-FUnction to PrintValues //////////////////////////////////////////////////////////////////////////

void PrintValues(void)
{
  Serial.print("ProfileDefault.profile_name: ");
  Serial.println(ProfileDefault.profile_name);
  Serial.print("ProfileDefault.filter_name: ");
  Serial.println(ProfileDefault.filter_name);
  Serial.print("ProfileDefault.min_rpm: ");
  Serial.println(ProfileDefault.min_rpm);
  Serial.print("ProfileDefault.max_rpm: ");
  Serial.println(ProfileDefault.max_rpm);

  Serial.print("ProfileCurrent.profile_name: ");
  Serial.println(ProfileCurrent.profile_name);
  Serial.print("ProfileCurrent.filter_name: ");
  Serial.println(ProfileCurrent.filter_name);
  Serial.print("ProfileCurrent.min_rpm: ");
  Serial.println(ProfileCurrent.min_rpm);
  Serial.print("ProfileCurrent.max_rpm: ");
  Serial.println(ProfileCurrent.max_rpm);
}