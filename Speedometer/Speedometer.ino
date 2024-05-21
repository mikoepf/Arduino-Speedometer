//#include <Keypad.h>         // A library for using matrix style keypads with the Arduino.
#include <Joystick.h>       // A library to emulate a Joystick with buttons, switches and axis
//#include <Wire.h>           // This library allows you to communicate with I2C devices
#include <LiquidCrystal_I2C.h>  // This library allows you to communicate with I2C Liquid crystal Display

 

LiquidCrystal_I2C lcd(0x27, 16, 2);   // LCD parameter definitions
int pins = 22; 
int Pin [22];

//  END of Declarations  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

void setup() {
  // The setup code is run only once

  // Initialisation of the serial Monitor
 Serial.begin(9600);
 Serial.println("Serialprint initialised");
 Serial.println(__LINE__);  //  Print the current code-Line on the serial Monitor

 // Initialisation of the LCd
 	lcd.begin();  // initialize the LCD, 
	lcd.backlight();  // Turn on the backlight and print a message.
  lcd.clear();
  lcd.setCursor (0,0); // set cursor at char 0, line 0
  lcd.print("First line");  
  lcd.setCursor (0,1); // set cursor at char 0, line 1
  lcd.print("Second line");

  // Pulling up the built in PIN-pullup:
  for (int i=0;i< pins;++i)
{
 pinMode(i,INPUT_PULLUP);
}

} //  END of Setup  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

void loop() {
  // put your main code here, to run repeatedly:

  CheckButtons();

}

//  END of Loop, start of definitions  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

////////////////////////////////////////////////////////////////////////////////////////// FUnction to check the Buttons //////////////////////////////////

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

  Pin[i]=1;
}
else if(digitalRead(i) != 0 && Pin[i]==1)
{
  Pin[i]=0;
}
}
}

