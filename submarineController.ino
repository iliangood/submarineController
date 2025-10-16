#include <easyEthernetLib.h>
#include <message.h>
#include <utils.h>

#include "PS2X_lib.h"

#include "axis.h"

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

  if(transmitter.init(IPAddress(192, 168, 1, 3)) != 0)
  {
    DEBUG_ERROR("transmitter init failed");
  }

}

void loop()
{
  message<64> txMsg;
  message<64> rxMsg;
  
  Axises axises;

  axises[AxisesNames::Vx] = map(ps2x.Analog(PSS_LY), 0, 255, 256, -256); // Вдоль Оси X
  axises[AxisesNames::Wz] = map(ps2x.Analog(PSS_LX), 0, 255, -256, 256); // Поворот по Оси Z
  axises[AxisesNames::Wy] = map(ps2x.Analog(PSS_RY), 0, 255, -256, 256); // Поворот по Оси Y
  axises[AxisesNames::Wx] = map(ps2x.Analog(PSS_RX), 0, 255, -256, 256); // Повотор по Оси X

  axises[AxisesNames::Vz] = ps2x.Button(PSB_PAD_UP) ? 256 : ps2x.Button(PSB_PAD_RIGHT) ? -256 : 0;
  axises[AxisesNames::Vy] = ps2x.Button(PSB_PAD_RIGHT) ? 256 : ps2x.Button(PSB_PAD_LEFT) ? -256 : 0;

  txMsg.push(axises);

  transmitter.sendData(txMsg);
  
  transmitter.receiveData(&rxMsg);

  delay(50);
}