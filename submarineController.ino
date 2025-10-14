#include <easyEthernetLib.h>
#include <message.h>
#include <utils.h>

#include "PS2X_lib.h"

#define PS2_DAT        13
#define PS2_CMD        11
#define PS2_SEL        10
#define PS2_CLK        12


PS2X ps2x;

uint8_t mac[6] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

DataTransmitter transmitter(mac, 80, "submarine");

void setup()
{
  Serial.begin(115200);

  int error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, false, false);

  if(error == 0)
  {
    Serial.println("Found Controller, configured successful");
    Serial.println("Try out all the buttons, X will vibrate the controller, faster as you press harder;");
    Serial.println("holding L1 or R1 will print out the analog stick values.");
    Serial.println("Go to www.billporter.info for updates and to report bugs.");
  }
  else if(error == 1)
    Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");
  else if(error == 2)
    Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");
  else if(error == 3)
    Serial.println("Controller refusing to enter Pressures mode, may not support it. ");
  
  byte type = ps2x.readType();

  if(type == 1)
    Serial.println("DualShock Controller Found");
  else
  {
    while(1)
    {
      Serial.println("Unknown Controller");
      delay(50);
    }
  }


}

void loop()
{
    
    /*  if(ps2x.Button(PSB_L3))
        Serial.println("L3 pressed");
      if(ps2x.Button(PSB_R3))
        Serial.println("R3 pressed");
      if(ps2x.Button(PSB_L2))
        Serial.println("L2 pressed");
      if(ps2x.Button(PSB_R2))
        Serial.println("R2 pressed");
      if(ps2x.Button(PSB_GREEN))
        Serial.println("Triangle pressed");*/
    
    // will be TRUE if button was JUST pressed
    if(ps2x.ButtonPressed(PSB_RED))
    Serial.println("Circle just pressed");
    
    // will be TRUE if button was JUST released
    if(ps2x.ButtonReleased(PSB_PINK))
      Serial.println("Square just released");     
    
    // will be TRUE if button was JUST pressed OR released
    if(ps2x.NewButtonState(PSB_BLUE))
      Serial.println("X just changed");    
    

    ps2x.Analog(PSS_LY);
    ps2x.Analog(PSS_LX);
    ps2x.Analog(PSS_RY);
    ps2x.Analog(PSS_RX);

  delay(50);
}