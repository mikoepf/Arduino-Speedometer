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
// LCD declaration and parameter definitions
LiquidCrystal_I2C lcd(0x27, 16, 2);   

// Testvariables declaration

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Classes and Struct Declarations /////////////////////////////////////////////////////////////////
String filter[3] = {"AVR","MED","MIX"};
const Profile ProfileDefault={"STD","MIX", 85, 25, 6};
Profile ProfileCurrent;
Sensor Ping(0,0);
TimeDifference DeltaTime(0,0);
Rpm Revs(ProfileDefault);
AxisValue Throttle;
UserInput Hmi;

//  END of Declarations  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

void setup() {// The setup code is run only once, no Declarations can be performed here!
  

// Initialisation of the serial Monitor
  Serial.begin(9600);
  Serial.println("Serialprint initialised");
  Serial.println(__LINE__);  //  Print the current code-Line on the serial Monitor

// Initialisation of the Joystick
  Joystick.begin();
  Joystick.setThrottleRange(AXISMINIMUM,AXISMAXIMUM);

// Initialisation of the LCd
 	lcd.begin();  // initialize the LCD, 
	lcd.backlight();  // Turn on the backlight and print a message.
  lcd.clear();
  lcd.setCursor (0,0); // set cursor at char 0, line 0
  lcd.print("First line");  
  lcd.setCursor (0,1); // set cursor at char 0, line 1
  lcd.print("Second line");

// Pulling up the built in PIN-pullups:
  for (int i=0;i< PINS;++i)
{
 pinMode(i,INPUT_PULLUP);
}

// Initialisation of the Current Profile
ProfileCurrent = ProfileDefault;
Hmi.InitRot();
Hmi.LCDprint(lcd,ProfileCurrent);
} //  END of Setup  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

void loop() { // put your main code here, to run repeatedly:

if(Ping.GetTime())
{

DeltaTime.CalculateTimediff(Ping.time_stamp);

if(DeltaTime.time_difference) // Since with only the first signal the timedifference and the subsequent values can´t be calculated,
                              // the subsequent will be calculated only after the second signal was received.
{
Revs.FillBuffer(DeltaTime.time_difference,ProfileCurrent);
Revs.MedianAverageFilter(DeltaTime.time_difference,ProfileCurrent);
Revs.CalculateRpm();
Throttle.CalculateAxisValue(ProfileCurrent, Revs, Ping); // Transforms the measured timedifferenzes between signal-falling edges to Axisvalues.
}
Serial.println(filter[1]);
Serial.println("---------------------------------------------- ");
}
Ping.SetSignalOrder();

Throttle.ResetAxisValue(ProfileCurrent,Revs,Ping,DeltaTime); // Resets the Axisvalue to zero if no falling edge has been detected for a prolonget time
Throttle.SetAxisValue(Joystick); // Sets/sends the calculated Axisvalues to the Joystick.

Hmi.NavigateMenu(lcd,ProfileCurrent); //  When the menubutton is pressed, the menu number "menu_count" is iterated.
Hmi.Reset(lcd,ProfileCurrent,ProfileDefault,Revs,Ping,DeltaTime);  // When the menubutton is pressed, all outputs rpm, Axisvalue, LCD-Display are reset to 0, or their default values and all respective buffers and variables are cleared/zeroized.
Hmi.ChangeParameter(lcd,ProfileCurrent);  // When the rotary encoder is rotaded, the parameter in the currently active menu are altered respectively.
//Hmi.PrintString(filter[0]);




































































if (!digitalRead(RSETBUT))
{

Serial.println("!!!!!!!!!!! reset button pressed !!!!!!!!!!!!!! ");
Revs.ResetRpm(Ping,DeltaTime);
}


//Revs.ApplyFilter(DeltaTime.time_difference);
//Revs.FillBuffer(DeltaTime.time_difference);
//Revs.AverageFilter();

//Revs.FillBuffer(DeltaTime.time_difference);
//Revs.MedianFilter(DeltaTime.time_difference);
/* Das geht !!!!!!!!!!!!!
unsigned long int median_average_difference=0;
++median_average_difference;
*/

// Das geht nicht?????!!!!!!
//Profile testprofile;


//Revs.FillBuffer(Revs.av_time_difference);
//Revs.AverageFilter();




}//  END of Loop, start of definitions  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

/////////////////////////////////////////////////////////////// FUnction to check the Buttons ///////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////// FUnction to check the Buttons //////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////// FUnction to check the Buttons //////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////// FUnction to check the Buttons //////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////// FUnction to check the Buttons //////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////// FUnction to check the Buttons //////////////////////////////////////////////////////////////////////////


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! TEST_FUNCTIONS !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



//////////////////////////////////////////////////////////////// Test-FUnction to check the Buttons //////////////////////////////////////////////////////////////////////////
/*
void CheckButtons(void)
{
 for (int i=0;i<PINS;++i)
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
*/
//////////////////////////////////////////////////////////////// Test-FUnction to PrintValues //////////////////////////////////////////////////////////////////////////
/*
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
*/