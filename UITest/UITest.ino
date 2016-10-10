
#include <Arduino.h>

#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
// Include the UI lib
#include "OLEDDisplayUi.h"

// Include custom images
#include "images.h"

// Initialize the OLED display using Wire library
SSD1306  display(0x3c, 5, 2);

OLEDDisplayUi ui     ( &display );

String team = "RED";
int level = 1;


/* Wifi Stuff */
#include <ESP8266WiFi.h>
const char* ssid     = "EmpireRecords_Devices";
const char* password = "loldevices";
//const char* ssid     = "Hypn-3G";
//const char* password = "1lnu5z1lnu5z";
//const char* ssid     = "FREEWIFI Woodstock Lounge";
//const char* password = "";

const char* host = "192.168.12.106";

/* Shift In Stuff */
const int pinShcp = 15; //Clock
const int pinStcp = 0; //Latch
const int pinDataIn = 16; // Data

/* Shift Out Stuff */
//Pin connected to latch pin (ST_CP) of 74HC595
const int latchPin = 12;
//Pin connected to clock pin (SH_CP) of 74HC595
const int clockPin = 14;
////Pin connected to Data in (DS) of 74HC595
const int dataPin = 13;

void msOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(0, 0, "LVL:" + String(level) + "/5");

  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(128, 0, team);
}


void drawFrame1(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // draw an xbm image.
  // Please note that everything that should be transitioned
  // needs to be drawn relative to x and y

  display->drawXbm(x+1,y+1, tm_width, tm_height, tm_bits);
}

void drawFrame2(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // Demonstrates the 3 included default sizes. The fonts come from SSD1306Fonts.h file
  // Besides the default fonts there will be a program to convert TrueType fonts into this format


 display->drawXbm(x+35, y, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
 display->setTextAlignment(TEXT_ALIGN_CENTER);
 display->setFont(ArialMT_Plain_10);
 display->drawString(x+60,y+40,WiFi.localIP().toString());


}


void drawFrame3(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // Demonstrates the 3 included default sizes. The fonts come from SSD1306Fonts.h file
  // Besides the default fonts there will be a program to convert TrueType fonts into this format


 display->drawXbm(x, y+15, skull_width, skull_height, skull_bits);
 display->setTextAlignment(TEXT_ALIGN_RIGHT);
 display->setFont(ArialMT_Plain_10);
 display->drawString(x+85,y+15,"Challenge One");
 display->drawXbm(x+95, y+15, emptyheart_width, emptyheart_height, emptyheart_bits);

 display->drawXbm(x, y+30, skull_width, skull_height, skull_bits);
 display->drawString(x+85,y+30,"Challenge Two");
 display->drawXbm(x+95, y+30, halfheart_width, halfheart_height, halfheart_bits);

 display->drawXbm(x, y+45, skull_width, skull_height, skull_bits);
 display->drawString(x+95,y+45,"Challenge Three");
 display->drawXbm(x+95, y+45, fullheart_width, fullheart_height, fullheart_bits);

}
// This array keeps function pointers to all frames
// frames are the single views that slide in
FrameCallback frames[] = { drawFrame1, drawFrame2, drawFrame3 };

// how many frames are there?
int frameCount = 3;

// Overlays are statically drawn on top of a frame eg. a clock
OverlayCallback overlays[] = { msOverlay };
int overlaysCount = 1;
void checkWifi()
{
  while (WiFi.status() != WL_CONNECTED) 
  {
   delay(500);

 }
}

void setup() {
  Serial.begin(74880);
  Serial.println("Shift Init");
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  for(int i=0;i<8;i++)
  {
    registerWrite(i, HIGH);
    delay(200);
  }
  registerWrite(8, LOW);

  Serial.println();
  Serial.println();

  WiFi.begin(ssid, password);

 

 pinMode(pinStcp, OUTPUT);
 pinMode(pinShcp, OUTPUT);
 pinMode(pinDataIn, INPUT);

  // The ESP is capable of rendering 60fps in 80Mhz mode
  // but that won't give you much time for anything else
  // run it in 160Mhz mode or just set it to 30 fps
  ui.setTargetFPS(30);

  // Customize the active and inactive symbol
  //ui.setActiveSymbol(activeSymbol);
  //ui.setInactiveSymbol(inactiveSymbol);
 ui.disableAllIndicators();
  
  // You can change this to
  // TOP, LEFT, BOTTOM, RIGHT
  ui.setIndicatorPosition(BOTTOM);

  // Defines where the first frame is located in the bar.
  ui.setIndicatorDirection(LEFT_RIGHT);

  // You can change the transition that is used
  // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_UP, SLIDE_DOWN
  ui.setFrameAnimation(SLIDE_LEFT);

  // Add frames
  ui.setFrames(frames, frameCount);

  // Add overlays
  ui.setOverlays(overlays, overlaysCount);

  //We dont want autoplay
  ui.disableAutoTransition();
  display.init();

  for (int16_t i=0; i<DISPLAY_WIDTH; i+=4) {
    display.drawLine(0, 0, i, DISPLAY_HEIGHT-1);
    display.display();
    delay(10);
  }
  for (int16_t i=0; i<DISPLAY_HEIGHT; i+=4) {
    display.drawLine(0, 0, DISPLAY_WIDTH-1, i);
    display.display();
    delay(10);
  }
  // Initialising the UI will init the display too.
  ui.init();
  delay(1000);

  display.flipScreenVertically();

}


void readShift()
{
  int inputPin = 0;

  digitalWrite(pinStcp, LOW);
  delayMicroseconds(20);
  digitalWrite(pinStcp, HIGH);


  inputPin = digitalRead(pinDataIn);
  Serial.print(inputPin,BIN);

  if(inputPin == 0)
  {
    ui.previousFrame();

  }


   for(int i=0; i < 7; i++) {

     digitalWrite(pinShcp, HIGH);
     //delayMicroseconds(0.2);
     delay(10);
     inputPin = digitalRead(pinDataIn);
     Serial.print(inputPin,BIN);
     if(inputPin == 0 && i == 0)
     {
       ui.nextFrame();
     }
     else if (inputPin == 0 && i == 1)
     {
      level = level + 1;
      team = "green";
     }
     else if (inputPin == 0 && i == 2)
     {
      for(int i=0;i<8;i++)
      {
        registerWrite(i, HIGH);
        delay(200);
      }
    registerWrite(8, LOW);
     }
     //digitalWrite(pinDs, inputPin);
  //    digitalWrite(pinDs, HIGH);

     digitalWrite(pinShcp, LOW);
     //digitalWrite(myClockPin, 1);
     //delay(10);
   }
   Serial.println("!");
}

void loop() {
  int remainingTimeBudget = ui.update();


  if (remainingTimeBudget > 0) {
    // You can do some work here
    // Don't do stuff if you are below your
    // time budget.
    readShift();
    //delay(remainingTimeBudget);
  }

  //delay(100);
}
void intDisplay(int numberToDisplay)
  {
      digitalWrite(latchPin, LOW);
      // shift out the bits:
      shiftOut(dataPin, clockPin, MSBFIRST, numberToDisplay);  
  
      //take the latch pin high so the LEDs will light up:
      digitalWrite(latchPin, HIGH);
    }
void registerWrite(int whichPin, int whichState) {
// the bits you want to send
  byte bitsToSend = 0;

  // turn off the output so the pins don't light up
  // while you're shifting bits:
  digitalWrite(latchPin, LOW);

  // turn on the next highest bit in bitsToSend:
  bitWrite(bitsToSend, whichPin, whichState);

  // shift the bits out:
  shiftOut(dataPin, clockPin, MSBFIRST, bitsToSend);

    // turn on the output so the LEDs can light up:
  digitalWrite(latchPin, HIGH);
  //delay(250);

}
