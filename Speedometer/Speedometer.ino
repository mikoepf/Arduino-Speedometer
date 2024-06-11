#include "SpeedometerClass.h" // This library was created in conjunction with this INO-file and contains all the Classes


// Joystick declaration;
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
  JOYSTICK_TYPE_JOYSTICK, 0, 0,
  false, false, false, false, false, false, 
  false, true, false, false, false);  // The Joystick will have only one Throttle-Axis
// LCD declaration and parameter definitions
LiquidCrystal_I2C lcd(0x27, 16, 2);   


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Classes and Struct Declarations /////////////////////////////////////////////////////////////////

String filter[NUMFILTERS] = {"AVR","MED","MIX"};  // Definition of the Filters / Filter names
Profile ProfileCurrent; //  Declaration of the parameter object (struct) which will store the currently utilized parameter values
Sensor Ping(0,0); // Declaration and instantiation with 0-values of a sensor object which method detects a falling edge of the sensor signal
TimeDifference DeltaTime(0,0);  // Declaration and instantiation with 0-values of an TimeDifference object, which methods are used to determin the time between the last detected falling edge and the currently detected falling edge.
Rpm Revs(ProfileCurrent); //  Declaration and instantiation with parameter-values of an Rpm object, which methods are used to apply a filter to the recorded timedifferences and to determine the revolutions per minute (rotational velocity)
AxisValue Throttle; // Declaration of an Throttle object, which method is used to calculate the axis value and send it to the computer via USB
UserInput Hmi;  // Declaration of an Hmi object, which methods are used to register the interface inputs and to print the menu and parameter values on the LCD

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

if(!Copy::LoadFromEEPROM(ProfileCurrent)) // If the EEPROM returns a value != 255 => the memory is not empty and the stored paramenter-values are copied to the current profile object
{// othervise the parameter of the default profile are copied to the currend profile.
Copy::CopyProfiles(ProfileCurrent,0,"Current");
}
// Setting the current profile as the currently active profile (shown on the LCD-Display)
Profiles::count = Profiles::pn; // The current profile is always on the last position of the profile list "Profiles::count"
Hmi.InitRot();  //  Checks the current state of rotary encoder signal befor the Loop beginn. (not really required)
Hmi.LCDprint(lcd,ProfileCurrent); // Prints the current profile parameters on the LCD
} //  END of Setup  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

void loop() { // put your main code here, to run repeatedly:

unsigned long get_time = Ping.GetTime(); // Variable to store the timestamp returned by "Ping.GetTime()" when the a falling edge is detected, othervise a "0" is stored.
if(get_time)  // Sometimes the if() statement does not receive the return-value? Othervise would do: if(Ping.GetTime())
{ // When a falling edge is detected, a sequence is initiated.

DeltaTime.CalculateTimediff(Ping.time_stamp,ProfileCurrent);  // Calculates the time between the last detected falling edge and the currently detected falling edge.

if(DeltaTime.time_difference) // Since with only the first signal the timedifference and the subsequent values can´t be calculated,
                              // the time difference will be calculated only after the second signal was received.
{// When a timedifference has been calculated, a second sequence is initiated.

Revs.FillBuffer(DeltaTime.time_difference,ProfileCurrent);  // The timedifferences are stored in a buffer.

Revs.ApplyFilter(DeltaTime.time_difference, filter, ProfileCurrent);  // According to the selected filter-parameter in the profile, the corresponding filter is applied to the previously filled buffer.

Revs.CalculateRpm();  // The rotational velocity is calculated with the filtered timedifferences.

Throttle.CalculateAxisValue(ProfileCurrent, Revs, Ping); // Transforms the measured timedifferenzes between signal-falling edges to Axisvalues.
} // END of the second sequence
Serial.println("---------------------------------------------- ");
} // END of the first sequence

Ping.SetSignalOrder();  // Stores the previously received signal value for the later comparison in the falling edge detection method. (used in "Ping.GetTime()")

Throttle.ResetAxisValue(ProfileCurrent,Revs,Ping,DeltaTime); // Resets the Axisvalue to zero if no falling edge has been detected for a prolonget time
Throttle.SetAxisValue(Joystick); // Sets/sends the calculated Axisvalues to the Joystick.

Hmi.NavigateMenu(lcd,ProfileCurrent); //  When the menubutton is pressed, the menu number "menu_count" is iterated.
Hmi.Reset(lcd,ProfileCurrent,Revs,Ping,DeltaTime);  // When the menubutton is pressed, all outputs rpm, Axisvalue, LCD-Display are reset to 0, or their default values and all respective buffers and variables are cleared/zeroized.
Hmi.ChangeParameter(lcd,ProfileCurrent,filter,Revs,Ping,DeltaTime);  // When the rotary encoder is rotaded, the parameter in the currently active menu are altered respectively.





































































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