
/* JSON Parser */
#include <ArduinoJson.h>

/* ESP8266 WiFi */
#include "ESP8266WiFi.h"
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

/* Button Constants */
const int P1_Left = 0;
const int P1_Right = 2;
const int P1_Top = 1;
const int P1_Bottom = 3;

const int P2_Left = 6;
const int P2_Right = 4;
const int P2_Top = 7;
const int P2_Bottom = 5;

/* LCD */
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
#include "OLEDDisplayUi.h" // Include the UI lib
#include "images.h" // Include custom images
SSD1306  display(0x3c, 5, 2);
OLEDDisplayUi ui     ( &display );

/* noway */
#include <qrcode.h>
QRcode qrcode (&display);

/* Timer */
#include "Timer.h"
Timer t;                               //instantiate the timer object




/* Shift Out ( 74HC595 ) */
//Pin connected to latch pin (ST_CP) of 74HC595
const int latchPin = 12;
//Pin connected to clock pin (SH_CP) of 74HC595
const int clockPin = 14;
//Pin connected to Data in (DS) of 74HC595
const int dataPin = 13;

/* Shift In  */
const int pinShcp = 15; //Clock
const int pinStcp = 0; //Latch
const int pinDataIn = 16; // Data


boolean debug = true;

char defaultSSID[32]     = "EmpireRecords_Devices";
char defaultPassword[32] = "loldevices";


struct WiFiSettings {
  char ssid[32];
  char password[32];
} currentWiFi;


uint8_t MAC_array[6];
char MAC_char[18];

// Badge connect details
//String hashEndPoint = "http://badges2016.andrewmohawk.com:8000/badge/gethash/";
//String checkInEndPoint = "http://badges2016.andrewmohawk.com:8000/badge/checkin/";
String hashEndPoint = "http://10.85.0.241:8000/badge/gethash/";
String checkInEndPoint = "http://10.85.0.241:8000/badge/checkin/";

String badgeName = "";
unsigned int badgeNumber;


unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 200;    // the debounce time; increase if the output flickers

unsigned long lastPWMTime = 0;  
unsigned long PWMDelay = 10;   
byte currentShiftOut = 0;


String team = "RED";
String alias = "AndrewM";
String badgeVerifyCode = "";
int level = 1;

void msOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {
  
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(0, 0, String(level));
  
  display->drawXbm(12, 0, fullheart_width, fullheart_height, fullheart_bits);
  
  

  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_10);
  display->drawString(55, 0, alias);


  display->drawXbm(90, 0, space_width, space_height, space_bits);
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(128, 0, team);
  
}

void drawFrame1(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // draw an xbm image.
  // Please note that everything that should be transitioned
  // needs to be drawn relative to x and y
  display->drawXbm(x,y+16, tblmnt_width, tblmnt_height, tblmnt_bits);
}

void drawFrame2(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // Demonstrates the 3 included default sizes. The fonts come from SSD1306Fonts.h file
  // Besides the default fonts there will be a program to convert TrueType fonts into this format
  display->drawXbm(x,y+16, clown_width, clown_height, clown_bits);
  
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  
  display->setFont(ArialMT_Plain_10);
  display->drawString(x+32,y+16,"Challenges: 5");
    
  display->setFont(ArialMT_Plain_10);
  display->drawString(x+32,y+26,"Badge:");
  display->setFont(ArialMT_Plain_10);
  display->drawString(x+32,y+36,"2483202117");

  display->setFont(ArialMT_Plain_10);
  display->drawString(x+32,y+46,"Code: ayZsl1");
  
  
 //display->drawXbm(x+35, y+20, WiFi_Logo_width, WiFi_Logo_height-20, WiFi_Logo_bits);
 //display->setTextAlignment(TEXT_ALIGN_CENTER);
 //display->setFont(ArialMT_Plain_10);
 //display->drawString(x+60,y+15,"Local IP:" + WiFi.localIP().toString());
 //display->drawString(x+60,y+25,"BadgeNumber:" + badgeName);
 //display->drawString(x+60,y+35,"Verify Code:" + badgeVerifyCode);

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


  //qrcode.create("http://www.andrewmohawk.com/");
}

void drawFrame4(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // Demo for drawStringMaxWidth:
  // with the third parameter you can define the width after which words will be wrapped.
  // Currently only spaces and "-" are allowed for wrapping
  /*display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawStringMaxWidth(0 + x, 16 + y, 128, "9h30 9h45:        Welcome    9h45 10h00: BSides Badge  10-10.30: How To Proxy NFC Comms 10.45-11.15:  Coffee 11.15-11.45:What the DLL?");
*/
 display->setTextAlignment(TEXT_ALIGN_CENTER);
 display->setFont(ArialMT_Plain_10);
 display->drawString(x+60,y+16,"9h30-9h45");
 display->drawString(x+60,y+26,"Grant Ongers");
 display->drawStringMaxWidth(x,y,128,"Welcome and Introduction to BSides CPT");

  
}

// This array keeps function pointers to all frames
// frames are the single views that slide in
FrameCallback frames[] = { drawFrame1, drawFrame2, drawFrame3, drawFrame4 };

// how many frames are there?
int frameCount = 4;

// Overlays are statically drawn on top of a frame eg. a clock
OverlayCallback overlays[] = { msOverlay };
int overlaysCount = 1;


boolean lowPowerMode = false;
/*
 * Draw a progress bar on the screen
 */

void drawProgressBar(int from, int to, String statusMessage, int progdelay = 5 ) {
  display.clear();
  display.setFont(ArialMT_Plain_10);
  for (int countUp = from;countUp <= to;countUp++)
  {
    display.clear();
    int progress = (countUp);
    // draw the progress bar
    display.drawProgressBar(0, 32, 120, 10, progress);
  
    // draw the percentage as String
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64, 15, statusMessage + " " + String(progress) + "%");
    display.display();
    delay(progdelay);
  }
}

void writeShift(byte curb)
{
  digitalWrite(latchPin, LOW); 
  shiftOut(dataPin, clockPin,MSBFIRST,  curb); 
  digitalWrite(latchPin, HIGH);
}

void registerWrite(int whichPin, int whichState) {
// the bits you want to send
  byte bitsToSend = 0;
  //Serial.println("Setting " + String(whichPin) + " to High");
  // turn off the output so the pins don't light up
  // while you're shifting bits:
  digitalWrite(latchPin, LOW);

  // turn on the next highest bit in bitsToSend:
  bitWrite(bitsToSend, whichPin, whichState);

  // shift the bits out:
  shiftOut(dataPin, clockPin, MSBFIRST, bitsToSend);

    // turn on the output so the LEDs can light up:
  digitalWrite(latchPin, HIGH);
  //delay(100);

}

void darkness()
{
  byte shift = 0;
  //Serial.println(shift,BIN);
  setOutShift(shift);
}


void setOutShift(byte shift)
{
  digitalWrite(latchPin, LOW); 
  shiftOut(dataPin, clockPin,MSBFIRST,  shift); 
  digitalWrite(latchPin, HIGH);

}

void twirl(int numTimes = 1)
{
  for(int x=0;x<numTimes;x++)
  {
    for(int i=0;i<=8;i++)
    {
      registerWrite(i,1);
      delay(100);
    }
  }
}
/*
 * Main Setup for badge.
 */
void setup() {
  Serial.begin(74880);
/* shift out */
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
  darkness();
qrcode.init();
  

/* shift in */
pinMode(pinStcp, OUTPUT);
 pinMode(pinShcp, OUTPUT);
 pinMode(pinDataIn, INPUT);
  
  
  
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);



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

  for (int16_t i=0; i<DISPLAY_WIDTH; i+=4) {
    display.drawLine(0, 0, i, DISPLAY_HEIGHT-1);
    display.display();
    //delay(10);
  }
  for (int16_t i=0; i<DISPLAY_HEIGHT; i+=4) {
    display.drawLine(0, 0, DISPLAY_WIDTH-1, i);
    display.display();
    //delay(10);
  }
  
  // Initialising the UI will init the display too.
  ui.init();
  display.flipScreenVertically();

}


void readShift()
{


  int inputPin = 1;
  int buttonPressedVal = 1; //Depending on how buttons are wired
  digitalWrite(pinStcp, LOW);
  delayMicroseconds(20);
  digitalWrite(pinStcp, HIGH);
  byte buttonVals = 0;
  
  
  for (int i=0; i<8; i++)
  {
    digitalWrite(pinShcp,LOW);
    delayMicroseconds(20);
    inputPin = digitalRead(pinDataIn);
    if(inputPin == buttonPressedVal)
     {
      //Serial.println("[.] Button " + String(i) + " pressed!");
      buttonVals = buttonVals | (1 << i);
     }
    
    digitalWrite(pinShcp,HIGH);
  }

   
  if(buttonVals == ((1 << P1_Bottom) | (1<<P2_Bottom)))
  {
    lowPowerMode = true;
    display.displayOff();
    darkness();
    
  }
  else if(buttonVals & (1 << P1_Right))
  {
    display.displayOn();
    ui.nextFrame();
    lowPowerMode = false;
    
  }
  else if(buttonVals & (1 << P1_Left))
  {
    display.displayOn();
    ui.previousFrame();
    lowPowerMode = false;
  }
  else if(buttonVals & (1 << P1_Top))
  {
    lowPowerMode = false;
    display.displayOn();
  }
  else if(buttonVals & (1 << P1_Bottom))
  {
    lowPowerMode = false;
    display.displayOn();
  }
  else if(buttonVals & (1 << P2_Top))
  {
    lowPowerMode = false;
    display.displayOn();
    level = level + 1;
  }
  else if(buttonVals & (1 << P2_Left))
  {
    lowPowerMode = false;
    display.displayOn();
  }
  else if(buttonVals & (1 << P2_Bottom))
  {
    lowPowerMode = false;
    display.displayOn();
    level = level -1;
  }
  else if(buttonVals & (1 << P2_Right))
  {
    darkness();
    display.displayOn();
    lowPowerMode = false;
  }


  
  
  


}



void loop() {
  int remainingTimeBudget = ui.update();


  if (remainingTimeBudget > 0) {
    // You can do some work here
    // Don't do stuff if you are below your
    // time budget.
    if ((millis() - lastDebounceTime) > debounceDelay) {
      readShift();
      lastDebounceTime = millis();
    }

    if ((millis() - lastPWMTime) > PWMDelay) 
    {
      if(lowPowerMode == false)
      {
        setOutShift(currentShiftOut);
      }
      lastPWMTime = millis();
    }
    else
    {
      darkness();
    }
    
    // put your main code here, to run repeatedly:
    t.update();
    //delay(remainingTimeBudget);
  }
  

  
}
