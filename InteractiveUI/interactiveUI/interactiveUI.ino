
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


/*Infrared */
#include <IRremoteESP8266.h>
const int TX_PIN = 10;
IRsend irsend(TX_PIN); //an IR led is connected to GPIO pin 0

int RECV_PIN = 4; 
IRrecv irrecv(RECV_PIN);
decode_results results;

/* LCD */
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
#include "OLEDDisplayUi.h" // Include the UI lib
#include "images.h" // Include custom images
SSD1306  display(0x3c, 5, 2);
OLEDDisplayUi ui     ( &display );



/* Timer */
#include "Timer.h"
Timer t;                               //instantiate the timer object

unsigned long lastAction = 0;
unsigned long lastActionTimeout = 30000;



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
unsigned long debounceDelay = 150;    // the debounce time; increase if the output flickers

unsigned long lastPWMTime = 0;  
unsigned long PWMDelay = 10;   
byte currentShiftOut = 0;
unsigned long currTime = 0;

String team = "RED";
String alias = "AndrewM";
String badgeVerifyCode = "";
int level = 1;

String lastButtons = "";


#include <pgmspace.h>


//Speaker information
PROGMEM const char string_intro[]  = "      BSIDES Schedule      Use right pad to navigate.";  
PROGMEM const char string_0[]      = "          08h00-09h15:         Coffee and Registration";   
PROGMEM const char string_1[]      = "          09h15-09h30:       Grant Ongers - Welcome to BSides";
PROGMEM const char string_2[]      = "          09h30-09h45:       Andrew MacPherson & Mike Davis - The BSides Badge";
PROGMEM const char string_3[]      = "          10h00-10h30:        Neil Roebert Mi->NFC->TM: How to proxy NFC comms using Android";
PROGMEM const char string_4[]      = "          10h45-11h00:       Coffee Break";
PROGMEM const char string_5[]      = "          11h15-11h45:       Chris Le Roy: What the DLL?";
PROGMEM const char string_6[]      = "          12h00-12h45:       Lunch";
PROGMEM const char string_7[]      = "          13h00-13h30:       Ion Todd: Password Securit for humans";
PROGMEM const char string_8[]      = "          13h45-15h15:       Robert Len: (In)Outsider Trading - Hacking stocks using public information";
PROGMEM const char string_9[]      = "          14h30-14h45:       Coffee Break";
PROGMEM const char string_10[]     = "          15h00-15h30:       Charl van der Walt: Love triangles in cyberspace. A tale about trust in 5 chapters";
PROGMEM const char string_11[]     = "          15h45-16h00:       Thomas Underhay & Darryn Cull: SensePost XRDP Tool";
PROGMEM const char string_12[]     = "          16h15-16h30:       BSides CPT 2016 Challenge";
PROGMEM const char string_13[]     = "          16h45-17h00:       Closing";
const char * const BSidesSchedule [] PROGMEM = {string_intro,string_0,string_1,string_2,string_3,string_4,string_5,string_6,string_7,string_8,string_9,string_10,string_11,string_12,string_13};

//number of speakers
int numScheduleItems = 13;
int currentScheduleItem = 0;
char currentSpeaker[120] = {0};





void msOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {
  
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(0, 0, String(level));
  
  display->drawXbm(8, 0, fullheart_width, fullheart_height, fullheart_bits);
  
  

  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_10);
  display->drawString(45, 0, alias);


  display->drawXbm(90, 0, space_width, space_height, space_bits);
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(128, 0, team);
  
}
void drawFrame1(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  if(currentScheduleItem == 0)
  {
    display->drawXbm(x+50, y+15, camera_width, camera_height, camera_bits);
    display->drawStringMaxWidth(0 + x, 40 + y, 128,currentSpeaker);
  }
  else
  {
    display->drawStringMaxWidth(0 + x, 13 + y, 128,currentSpeaker);
  }
}

void drawFrame2(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // Demonstrates the 3 included default sizes. The fonts come from SSD1306Fonts.h file
  // Besides the default fonts there will be a program to convert TrueType fonts into this format
  display->drawXbm(x,y+14, ship_width, ship_height, ship_bits);
  
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  
  display->setFont(ArialMT_Plain_10);
  display->drawString(x+32,y+13,"Challenges: 0");
    
  display->setFont(ArialMT_Plain_10);
  display->drawString(x+32,y+23,"Badge:");
  display->setFont(ArialMT_Plain_10);
  display->drawString(x+32,y+33,badgeName);

  display->setFont(ArialMT_Plain_10);
  display->drawString(x+32,y+43,"Code:");
  display->drawString(x+32,y+53,"aN1oHs");
   
  
  
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


String Challenges[10];

//myChallenges[0] = "Konami"
//myChallenges[1] = "Alias"

int completedChallenges = 0;
int currentListedChallenge = 0;

void ChallengeFrame(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {

 display->drawXbm(x, y+16, challenges_width, challenges_height, challenges_bits);
 display->setTextAlignment(TEXT_ALIGN_CENTER);
 display->setFont(ArialMT_Plain_10);
 display->drawXbm(x+50, y+38, uparrow_width, uparrow_height, uparrow_bits);
 display->drawString(x+80,y+35," = Play");
 
  
 display->drawXbm(x+5, y+50, leftarrow_width, leftarrow_height, leftarrow_bits);
 if(completedChallenges == 0)
 {
  display->drawString(x+70,y+50,"None completed ");
 }
 else
 {
   display->drawString(x+70,y+50,Challenges[currentListedChallenge]);
 }
 display->drawXbm(x+120, y+50, rightarrow_width, rightarrow_height, rightarrow_bits);
  
}

// This array keeps function pointers to all frames
// frames are the single views that slide in
FrameCallback frames[] = { drawFrame1, drawFrame2, drawFrame3, ChallengeFrame };

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
 //char buffer[20] = {0};
















static const char PROGMEM Line1[] = {"String 1"};
static const char PROGMEM Line2[] = {"String 2"};
static const char PROGMEM Line3[] = {"String 3"};

const char * const Lines [] PROGMEM = { Line1, Line2, Line3 };
  
void setup() {
  //strcpy_P(buffer, (char*) pgm_read_dword(&(Lines[0])));
  lastAction = millis();
  Serial.begin(74880);
  //currentSpeaker
  strcpy_P(currentSpeaker, (char*) pgm_read_dword(&(BSidesSchedule[currentScheduleItem])));
  //Serial.println(currentSpeaker);
  
    
  //Serial.print("buffer=");
  //Serial.println(currentSpeaker);
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

  irrecv.enableIRIn(); // Start the receiver
  irsend.begin();
  //int tickEvent2 = t.every(5550, transmitBadge);

}


void readShift()
{

  int currentFrame = ui.getUiState()->currentFrame;
  int inputPin = 1;
  int buttonPressedVal = 1; //Depending on how buttons are wired
  digitalWrite(pinStcp, LOW);
  delayMicroseconds(20);
  digitalWrite(pinStcp, HIGH);
  byte buttonVals = 0;
  String thisBut = "";
  
  for (int i=0; i<8; i++)
  {
    digitalWrite(pinShcp,LOW);
    delayMicroseconds(20);
    inputPin = digitalRead(pinDataIn);
    if(inputPin == buttonPressedVal)
     {
      Serial.println("[.] Button " + String(i) + " pressed!");
      buttonVals = buttonVals | (1 << i);
     }
    
    digitalWrite(pinShcp,HIGH);
  }
  if(buttonVals != 0)
  {
    registerWrite(0,1);
  }
   
  if(buttonVals == ((1 << P1_Bottom) | (1<<P2_Bottom)))
  {
    Serial.println("setting low power!!!");
    lowPowerMode = true;
    display.displayOff();
    darkness();
    
  }
  else if(buttonVals & (1 << P1_Right))
  {
    thisBut = "R";
    display.displayOn();
    ui.nextFrame();
    lowPowerMode = false;
    lastAction = millis();
    
  }
  else if(buttonVals & (1 << P1_Left))
  {
    thisBut = "L";
    display.displayOn();
    ui.previousFrame();
    lowPowerMode = false;
    lastAction = millis();
  }
  else if(buttonVals & (1 << P1_Top))
  {
    thisBut = "U";
    lowPowerMode = false;
    display.displayOn();
    lastAction = millis();
  }
  else if(buttonVals & (1 << P1_Bottom))
  {
    thisBut = "D";

    lowPowerMode = false;
    display.displayOn();
    lastAction = millis();
  }
  else if(buttonVals & (1 << P2_Top))
  {
    thisBut = "B";
    lowPowerMode = false;
    display.displayOn();
    lastAction = millis();
    
    if (currentFrame == 3)
    {
      if(Challenges[currentListedChallenge] == "Konami")
      {
        konamiCode();
      }
      else if(Challenges[currentListedChallenge] == "Alias")
      {
        playAlias();
      }
    }
    Serial.println(ESP.getFreeHeap());
    
  }
  else if(buttonVals & (1 << P2_Left))
  {
    
    thisBut = "A";
    lowPowerMode = false;
    display.displayOn();
    lastAction = millis();
    if (currentFrame == 0)
    {
      if(currentScheduleItem < 1)
      {
        currentScheduleItem = numScheduleItems;
      }
      else
      {
        currentScheduleItem--;
      }
      strcpy_P(currentSpeaker, (char*) pgm_read_dword(&(BSidesSchedule[currentScheduleItem])));
    }

    if (currentFrame == 3)
    {
      if(currentListedChallenge < 1)
      {
        currentListedChallenge = completedChallenges - 1;
      }
      else
      {
        currentListedChallenge--;
      }
      
    }


    
  }
  else if(buttonVals & (1 << P2_Bottom))
  {
    thisBut = "C";
    lowPowerMode = false;
    display.displayOn();
    lastAction = millis();
    playNinja();
  }
  else if(buttonVals & (1 << P2_Right))
  {
    thisBut = "D";
    darkness();
    display.displayOn();
    lastAction = millis();
    lowPowerMode = false;
    if (currentFrame == 0)
    {
      if(currentScheduleItem > numScheduleItems)
      {
        currentScheduleItem = 0;
      }
      else
      {
        currentScheduleItem++;
      }
      strcpy_P(currentSpeaker, (char*) pgm_read_dword(&(BSidesSchedule[currentScheduleItem])));
    }
    if (currentFrame == 3)
    {
      if(currentListedChallenge > completedChallenges-2)
      {
        currentListedChallenge = 0;
      }
      else
      {
        currentListedChallenge++;
      }
      
    }


    
    Serial.println(ESP.getFreeHeap());
  }


  if (thisBut != "")
    {
      lastButtons = lastButtons + thisBut;
      if(lastButtons.length() > 10)
      {
        lastButtons = lastButtons.substring(1);
      }
    }
    //Serial.println(lastButtons + "!");
    if(lastButtons == "UUDDLRLRAB")
    {
      //konamiCode();
      if(addChallenge("Konami"))
      {
        playNinja();
      }
      lastButtons = "";
    }



  if(buttonVals != 0)
  {
    registerWrite(0,0);
  }
  
  
  


}

boolean addChallenge(String c_name)
{
  boolean found = false;
  for(int i=0;i<completedChallenges;i++)
  {
    if(Challenges[i] == c_name)
    {
      found == true;
    }
  }

  if(found == false)
  {
    
    Challenges[completedChallenges] = c_name;
    completedChallenges++;
    currentListedChallenge = 0;
    return true;
  }
  else
  {
    return false;
  }
}

void konamiCode()
{
  Serial.println("[+] KONAMI CODE!");
  int y = 0;
  for (int16_t x=0; x<DISPLAY_WIDTH; x+=4) {
    display.clear();
    y = random(0,15);
    display.drawXbm(x, y, skeleton_width, skeleton_height, skeleton_bits);
    display.display();
    delay(100);
  }
}

void playAlias()
{
  Serial.println("[+] Play Alias!");
  int x = 0;
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  for (int16_t y=0; y<DISPLAY_HEIGHT; y+=2) {
    display.clear();
    x = random(0,DISPLAY_WIDTH/2);
    display.drawString(x, y, alias);
    display.display();
    delay(100);
  }
}

void playNinja()
{
  Serial.println("[+] Challenge completed!");
  
  int y = 0;
  for (int16_t x=DISPLAY_WIDTH; x>=0; x-=5) {
    display.clear();
    y = 0;
    display.drawXbm(x, y, ninja_width, ninja_height, ninja_bits);
    display.display();
    
  }
  
  display.invertDisplay();
  delay(300);
  display.normalDisplay();
  delay(300);
  display.invertDisplay();
  delay(300);
  display.normalDisplay();
  delay(300);
}



void loop() {
  int remainingTimeBudget = 0;
  if(lowPowerMode == false)
  {
    
    remainingTimeBudget = ui.update();
  }
  else
  {
    //Serial.println("low power?");
    t.update();
  }


  if (remainingTimeBudget > 0 || lowPowerMode == true) {
    // You can do some work here
    // Don't do stuff if you are below your
    // time budget.

    currTime = millis();
    
    if ((currTime - lastDebounceTime) > debounceDelay) {
      readShift();
      lastDebounceTime = currTime;
    }
    
    currTime = millis();
    if(currTime - lastAction > lastActionTimeout)
    {
      /*Serial.println("Oh noes, nothing happened!");
      Serial.println(currTime - lastAction);
      Serial.println(currTime);
      Serial.println(lastAction);*/
      if(lowPowerMode == false)
      {
      display.clear();
      display.drawXbm(0, 16, sleepingpanda_width, sleepingpanda_height, sleepingpanda_bits);
      display.display();
      lowPowerMode = true;
      setOutShift(0);
      
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

    if (irrecv.decode(&results)) 
    {
      //dump(&results);
      irrecv.resume(); // Receive the next value
    }
   
  }
  

  
}
