/* JSON Parser */
#include <ArduinoJson.h>

/* ESP8266 WiFi */
#include "ESP8266WiFi.h"
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

/* EEPROM */
#include <EEPROM.h>
#include "EEPROMAnything.h"

/* LCD */
#include "SSD1306.h" // Screen Library
#include "OLEDDisplayUi.h" // Include the UI lib
#include "images.h" // Include custom images
SSD1306  display(0x3c, 5, 2);
OLEDDisplayUi ui     ( &display );

/* Timer */
#include "Timer.h"
Timer t;                               //instantiate the timer object
//Timer t2;

/*Infrared */
#include <IRremoteESP8266.h>
const int TX_PIN = 10;
IRsend irsend(TX_PIN); //an IR led is connected to GPIO pin 0

int RECV_PIN = 4; 
IRrecv irrecv(RECV_PIN);
decode_results results;



/* Button Constants */
const int P1_Left = 0;
const int P1_Right = 2;
const int P1_Top = 1;
const int P1_Bottom = 3;

const int P2_Left = 6;
const int P2_Right = 4;
const int P2_Top = 7;
const int P2_Bottom = 5;



/* Shift Out ( 74HC595 ) */
const int latchPin = 12; //Pin connected to latch pin (ST_CP) of 74HC595
const int clockPin = 14; //Pin connected to clock pin (SH_CP) of 74HC595
const int dataPin = 13; //Pin connected to Data in (DS) of 74HC595

/* Shift In  */
const int pinShcp = 15; //Clock
const int pinStcp = 0; //Latch
const int pinDataIn = 16; // Data

/* List of last 5 seen badges and the amount we have seen since last update */
unsigned int badgeList[5] = {1234567890,1234567891,1234567892,1234567893,1234567894};
int numBadges = 0;

/* Our HTTP client */
HTTPClient http;

/* Default WiFi SSID details */
char defaultSSID[32]     = "highway";
char defaultPassword[32] = "dangerzone";
//char defaultSSID[32]     = "AndrewMohawkGlobal";
//char defaultPassword[32] = "0126672737";

/* WiFi struct for EEPROM */
struct WiFiSettings {
  char ssid[32];
  char password[32];
} currentWiFi;


/* Endpoints for badges */
String hashEndPoint = "http://badges2016.andrewmohawk.com:8000/badge/gethash/"; 
String checkInEndPoint = "http://badges2016.andrewmohawk.com:8000/badge/checkin/";
//String hashEndPoint = "http://10.85.0.243:8000/badge/gethash/";
//String checkInEndPoint = "http://10.85.0.243:8000/badge/checkin/";

/* Badge Name and Number */
String badgeName = "";
unsigned int badgeNumber;

/* UI functions for debouncing, last keypressed for 'sleep', lowPowerMode and fake PWM */

unsigned long lastDebounceTime = 0; 
unsigned long debounceDelay = 150;  

unsigned long lastAction = 0;
unsigned long lastActionTimeout = 300000; //Go to panda in 5 minutes.

bool lowPowerMode = false;
byte currentShiftOut = 0;

/* Not current used */
/*
unsigned long lastPWMTime = 0;  
unsigned long PWMDelay = 10;   
*/

unsigned long currTime = 0;


/* UI Functional variables */
String team = "None!";
String alias = "No Alias";
String badgeVerifyCode = "NoCode";
int level = 1;


String Challenges[10];
int completedChallenges = 0;
int currentListedChallenge = 0;
String lastButtons = "";


//Speaker information


#include <pgmspace.h>

PROGMEM const char string_intro[]  = "      BSIDES Schedule      Use right pad to navigate.";  
PROGMEM const char string_0[]      = "          08h00-09h30:         Registration and Tea/Coffee";   
PROGMEM const char string_1[]      = "          09h30-09h45:       Grant Ongers - Opening and Welcome";
PROGMEM const char string_2[]      = "          09h45-10h00:       Andrew MacPherson & Mike Davis - The BSides Badge";
PROGMEM const char string_3[]      = "          10h00-10h45:        Neil Roebert Mi->NFC->TM: How to proxy NFC comms using Android";
PROGMEM const char string_4[]      = "          10h45-11h15:       Coffee Break";
PROGMEM const char string_5[]      = "          11h15-12h00:       Chris Le Roy: What the DLL?";
PROGMEM const char string_6[]      = "          12h00-13h00:       Lunch";
PROGMEM const char string_7[]      = "          13h00-13h45:       Charl van der Walt: Love triangles in cyberspace. A tale about trust in 5 chapters";
PROGMEM const char string_8[]      = "          13h45-14h30:       Robert Len: (In)Outsider Trading - Hacking stocks using public information";
PROGMEM const char string_9[]      = "          14h30-15h00:       Coffee Break";
PROGMEM const char string_10[]     = "          15h00-15h45:       Ion Todd: Password Securit for humans";
PROGMEM const char string_11[]     = "          15h45-16h15:       Thomas Underhay & Darryn Cull: SensePost XRDP Tool";
PROGMEM const char string_12[]     = "          16h15-16h30:       Michael Rodger: Opening the Black Box – Software Security from a Hardware Perspective";
PROGMEM const char string_13[]     = "          16h45-17h00:       Closing";
const char * const BSidesSchedule [] PROGMEM = {string_intro,string_0,string_1,string_2,string_3,string_4,string_5,string_6,string_7,string_8,string_9,string_10,string_11,string_12,string_13};

//number of speakers
int numScheduleItems = 13;
int currentScheduleItem = 0;
char currentSpeaker[120] = {0};



// This array keeps function pointers to all frames


//So we dont do IR when we are updating... or the badge CRASHES... right?
bool updating = false;


/* Pong Stuff */
int pong_p1score = 0;
int pong_p2score = 0;
unsigned int pong_delay = 150;
unsigned int pong_time = 0;
int pong_p1place = 0;
int pong_p2place = 0;
int pong_ball_x = 5;
int pong_ball_y = 5;
int pong_paddle_size = 10;
int pong_ballmovement = 4;
int pong_startmovement = pong_ballmovement;
int pong_ball_size = 2;
int pong_paddle_movespeed = 5;
int pong_speed_multiplyer = 1.2;
bool pong_ball_up = false;
bool pong_ball_right = true;


String badgeVersion = "0.45";
String badgeGitHash = "6f8d58907096dd2a66f06d04c2525db89583c83f";

/* Helpers */
#include "general.h" // general functions
#include "screen.h" // Screen drawing functions

#include "ShiftRegisters.h" // input/output registers

#include "WiFi.h" // WiFi connections
#include "communication.h" // Communications To/From server





FrameCallback frames[] = { bsidesLogoFrame, playerInfoFrame, ScheduleFrame, ChallengeFrame,AboutFrame};

// how many frames are there?
int frameCount = 5;

// Overlays are statically drawn on top of a frame eg. a clock
OverlayCallback overlays[] = { msOverlay };
int overlaysCount = 1;




/*
 * Main Setup for badge.
 */
void setup() {
  Serial.begin(74880);
  initWiFi(true); // Initialise WiFi -- this MUST happen before screen.
  
  strcpy_P(currentSpeaker, (char*) pgm_read_dword(&(BSidesSchedule[currentScheduleItem])));
  
/* shift out */
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
  darkness();

/* shift in */
pinMode(pinStcp, OUTPUT);
 pinMode(pinShcp, OUTPUT);
 pinMode(pinDataIn, INPUT);
  
  
   
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);


  for (int16_t i=DISPLAY_HEIGHT; i>0; i-=4) {
    display.drawLine(0, i, DISPLAY_WIDTH, i);
    display.display();
    delay(50);
  }

  display.clear();
  display.drawXbm(0, 0, monerologo_width, monerologo_height, monerologo_bits);
  display.display();
  delay(450);
  

  
  drawProgressBar(0, 25, "Initialising WiFi..",10);
 
 
  
  //Setup EEPROM - 512 bytes!
  EEPROM.begin(512);

  drawProgressBar(25, 30, "EEPROM ..",50);

  
  drawProgressBar(30, 40, "Serial ..",50);
  Serial.setDebugOutput(true);

  
  

  
  
  drawProgressBar(40, 60, "Joining network..",100);
  fetchStatus(); // fetch network status
  darkness();
  twirl();
  int fetchStatusEvent = t.every(25000, fetchStatus); // Set it to run every 15 seconds after this
  

  drawProgressBar(60, 70, "Configuring AI/IR",75);
  
  
  drawProgressBar(70, 80, "Hacking Planet..",75);
  
  
  drawProgressBar(80, 90, "How about a nice game",75);
  drawProgressBar(90, 100, "...of chess...",50);




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
    delay(10);
  }
  for (int16_t i=0; i<DISPLAY_HEIGHT; i+=4) {
    display.drawLine(0, 0, DISPLAY_WIDTH-1, i);
    display.display();
    delay(10);
  }
  // Initialising the UI will init the display too.
  ui.init();
  display.flipScreenVertically();

  irrecv.enableIRIn(); // Start the receiver
  irsend.begin();
  int tickEvent2 = t.every(5550, transmitBadge);
  
  
  lastAction = millis();

  

}











void loop() {
  
  int remainingTimeBudget = 0;
  
  if(lowPowerMode == false)
  {
    remainingTimeBudget = ui.update();
  }
  else
  {
    t.update();
    //t2.update();
  }


  if (remainingTimeBudget > 0 || lowPowerMode == true) {
    // You can do some work here
    // Don't do stuff if you are below your
    // time budget.
    if(updating == false)
    {
      if (irrecv.decode(&results)) 
      {
        dump(&results);
        if(updating == false) // it should still not be updating...
        {
          irrecv.resume(); // Receive the next value
        }
          
      }
    }

    currTime = millis();
    
    if ((currTime - lastDebounceTime) > debounceDelay) {
      readShift();
      lastDebounceTime = currTime;
    }
    
    currTime = millis(); // Dont ask why we need to do this again............ I mean wtf!
    
    if(currTime - lastAction > lastActionTimeout)
    {
      if(lowPowerMode == false)
      {
          display.displayOn();
          display.clear();
          display.drawXbm(0, 16, smallerpanda_width, smallerpanda_height, smallerpanda_bits);
          display.setTextAlignment(TEXT_ALIGN_LEFT);
          display.drawString(70,30,"Alias:");
          display.drawString(70,40,alias);
          
          display.display();
          lowPowerMode = true;
          setOutShift(0);
          Serial.println("[+] Panda Mode!");
      }
      

    }
    /*
      
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
    */
    
    // put your main code here, to run repeatedly:
    t.update();
    
   
    
    
    
  }
  
 
  
}
