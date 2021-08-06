#include "Arduino.h"
#include "Adafruit_NeoPixel.h"
#include "ros.h"
#include "ANTS_ROS.h"
#include "BluetoothSerial.h" //adds Bluetooth support to existing ESP32

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#define IGNOREDEBUG 0 //must be set to 0 to enable fully working
#define USEBLUETOOTH 1 //must be 1 to use Blueooth for debugging
#define MAXPOWER 0.2 //MAX POWER IN %/100

Adafruit_NeoPixel LeftStrip;
Adafruit_NeoPixel RightStrip;

#define LEFTLED 25
#define RIGHTLED 26

#define LEFTCOUNT 144
#define RIGHTCOUNT 144

unsigned long timeSinceStart = 0;
unsigned long turnLeftTimer = 0;
unsigned long turnRightTimer = 0;

BluetoothSerial SerialBT;
TaskHandle_t BluetoothDataTransfer;
void BluetoothROS(void * parameter);

void drawForward(Adafruit_NeoPixel *ledStrip, int LEDCOUNT);
void drawReverse(Adafruit_NeoPixel *ledStrip, int LEDCOUNT);
void drawTurn(Adafruit_NeoPixel *ledStrip1, Adafruit_NeoPixel *ledStrip2, int LEDCOUNT);

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

    //start wi-fi and ROS node
    if (!IGNOREDEBUG) {
      LED.initNode();
      SerialBT.println("Initializing ROS topics");

      //LED subs
      LED.subscribe(LeftLEDstrip); //motor 1
      LED.subscribe(RightLEDstrip); //motor 2 -> included in motor 1
    }

    LeftStrip = Adafruit_NeoPixel(LEFTCOUNT, LEFTLED, NEO_GRB + NEO_KHZ800);
    RightStrip = Adafruit_NeoPixel(RIGHTCOUNT, RIGHTLED, NEO_GRB + NEO_KHZ800);

    LeftStrip.begin();
    RightStrip.begin();
    LeftStrip.show(); // Initialize all pixels to 'off'
    RightStrip.show();
}

// LOOP FUNCTION ====================================================================================
void loop()
{ 
  LeftStrip.setBrightness(30);
  RightStrip.setBrightness(30);
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
  leftLEDcommand = msg1.data;

  if (leftLEDcommand == "forward") {
    drawForward(&LeftStrip, LEFTCOUNT);
  } else if (leftLEDcommand == "reverse") {
    drawReverse(&LeftStrip, LEFTCOUNT);
  } else if (leftLEDcommand == "left") {
    drawTurn(&LeftStrip, &RightStrip, 144);
  } else if (leftLEDcommand == "right") {
    drawTurn(&RightStrip, &LeftStrip, 144);
  }
}

void rightLEDcontrol(const std_msgs::String& msg2) { //motor 2 data from ROS to motor control
  rightLEDcommand = msg2.data;

  if (leftLEDcommand == "forward") {
    drawForward(&LeftStrip, RIGHTCOUNT);
  } else if (leftLEDcommand == "reverse") {
    drawReverse(&LeftStrip, RIGHTCOUNT);
  } else if (leftLEDcommand == "left") {
    drawTurn(&LeftStrip, &RightStrip, 144);
  } else if (leftLEDcommand == "right") {
    drawTurn(&RightStrip, &LeftStrip, 144);
  }
}

void drawForward(Adafruit_NeoPixel *ledStrip, int LEDCOUNT) {
  for (int i = 4; i < LEDCOUNT; i++) {
    ledStrip->setPixelColor(i, 0, 255, 0);
    ledStrip->setPixelColor(i-1, 0, 255, 0);
    ledStrip->setPixelColor(i-2, 0, 255, 0);
    ledStrip->setPixelColor(i-3, 0, 255, 0);
    ledStrip->setPixelColor(i-4, 0, 255, 0);

    ledStrip->setPixelColor(i-5, 0, 0, 0);
    ledStrip->show();
  }
}

void drawReverse(Adafruit_NeoPixel *ledStrip, int LEDCOUNT) {
  for (int i = LEDCOUNT; i >= 0; i--) {
    ledStrip->setPixelColor(i, 0, 255, 0);
    ledStrip->setPixelColor(i-1, 0, 255, 0);
    ledStrip->setPixelColor(i-2, 0, 255, 0);
    ledStrip->setPixelColor(i-3, 0, 255, 0);
    ledStrip->setPixelColor(i-4, 0, 255, 0);

    ledStrip->setPixelColor(i+1, 0, 0, 0);
    ledStrip->show();
  }
}

void drawTurn(Adafruit_NeoPixel *ledStrip1, Adafruit_NeoPixel *ledStrip2, int LEDCOUNT) {
  //Strip1 is the priority one, Strip2 is the one that will be turned off
  ledStrip1->fill(255, 165, 0);
  ledStrip2->fill(0, 0, 0);
  ledStrip1->show();
  ledStrip2->show();
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