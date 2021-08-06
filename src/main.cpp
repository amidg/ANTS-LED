#include "Arduino.h"
#include "Adafruit_NeoPixel.h"
#include "BluetoothSerial.h" //adds Bluetooth support to existing ESP32

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#define IGNOREDEBUG 0 //must be set to 0 to enable fully working
#define USEBLUETOOTH 1 //must be 1 to use Blueooth for debugging
#define MAXPOWER 0.2 //MAX POWER IN %/100

unsigned long timeSinceStart = 0;

BluetoothSerial SerialBT;
TaskHandle_t BluetoothDataTransfer;
void BluetoothROS(void * parameter);

//WI-FI DEFINITIONS: ============================================================================
const char* ssid     = "autobot_F07B";
const char* password = "mse2021cap";
// IPAddress ip(192, 168, 1, 3); //reserved when used with router
IPAddress server(25,2,117,165);
const uint16_t serverPort = 11411;

//MAIN FUNCTION ===============================================================================]
void setup()
{ 
    timeSinceStart = millis();
    if (USEBLUETOOTH) { 
      SerialBT.begin("ANTS_DCU"); 

      xTaskCreatePinnedToCore(
        BluetoothROS,              /* Function to implement the task */
        "Transfer ROS data over Bluetooth", /* Name of the task */
        10000,                              /* Stack size in words */
        NULL,                               /* Task input parameter */
        0,                                  /* Priority of the task */
        &BluetoothDataTransfer,                             /* Task handle. */
        0);                                 /* Core where the task should run */

      if (SerialBT.available()) {
        SerialBT.println("Connected");
      }
    }

    pinMode(GIGAVACENABLE, OUTPUT); //gigavac control relay

    //start wi-fi and ROS node
    if (!IGNOREDEBUG) {
      LED.initNode();
      SerialBT.println("Initializing ROS topics");

      //LED subs
      LED.subscribe(LeftLEDstrip); //motor 1
      LED.subscribe(RightLEDstrip); //motor 2 -> included in motor 1
    }

}

// LOOP FUNCTION ====================================================================================
void loop()
{
  if (!IGNOREDEBUG) {
    while(true) {
      //check the ROS condition first. If not connected and timeSinceStart is huge -> restart
      if ( !LED.connected() && (millis() - timeSinceStart >= 60000) ) {
        //reset the hardware
        ESP.restart();
        timeSinceStart = millis();
      }

      LED.spinOnce();
      delayMicroseconds(20);
    }
  } 
}

/*
  ROS FUNCTIONS BELOW -> DO NOT TOUCH UNLESS NOT WORKING
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////
// ADDITIONAL FUNCTIONS ================================================================================
void leftLEDcontrol(const std_msgs::String& msg1) { //motor 1 data from ROS to motor control
  
}

void rightLEDcontrol(const std_msgs::String& msg2) { //motor 2 data from ROS to motor control
  
}

void BluetoothROS(void * parameter) {
  while(1) {
    if (USEBLUETOOTH) { 
      //publish bluetooth
      SerialBT.print("ROS node status: "); SerialBT.println(LED.connected());
      SerialBT.print("Right LED status: "); SerialBT.println(leftLEDcommand);
      SerialBT.print("Left LED status: "); SerialBT.println(rightLEDcommand);
      SerialBT.printf("\r\n RAM left %d (bytes)", (ESP.getFreeHeap()));
      SerialBT.println("------------------------------------");
      SerialBT.flush();
      vTaskDelay(10);
    }
  }
}