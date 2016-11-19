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
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
#include "OLEDDisplayUi.h" // Include the UI lib
#include "images.h" // Include custom images
SSD1306  display(0x3c, 5, 2);
OLEDDisplayUi ui     ( &display );

/* Button Constants */
const int P1_Left = 0;
const int P1_Right = 2;
const int P1_Top = 1;
const int P1_Bottom = 3;

const int P2_Left = 6;
const int P2_Right = 4;
const int P2_Top = 7;
const int P2_Bottom = 5;

bool lowPowerMode = false;

/* Timer */
#include "Timer.h"
Timer t;                               //instantiate the timer object

/*Infrared */
#include <IRremoteESP8266.h>
const int TX_PIN = 10;
IRsend irsend(TX_PIN); //an IR led is connected to GPIO pin 0

int RECV_PIN = 4; 
IRrecv irrecv(RECV_PIN);
decode_results results;


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

unsigned int badgeList[10];
int numBadges = 0;

MDNSResponder mdns;
//WiFiServer server(80);
HTTPClient http;

boolean debug = true;

char defaultSSID[32]     = "EmpireRecords_Devices";
char defaultPassword[32] = "loldevices";
//char defaultSSID[32]     = "AndrewMohawkGlobal";
//char defaultPassword[32] = "0126672737";

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
String alias = "";
String badgeVerifyCode = "";
int level = 1;

void msOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(0, 0, String(level) + "/5");

  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_10);
  display->drawString(64, 0, alias);

  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(128, 0, team);
}

void drawFrame2(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // draw an xbm image.
  // Please note that everything that should be transitioned
  // needs to be drawn relative to x and y
  display->drawXbm(x+1,y+1, tm_width, tm_height, tm_bits);
}

void drawFrame1(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // Demonstrates the 3 included default sizes. The fonts come from SSD1306Fonts.h file
  // Besides the default fonts there will be a program to convert TrueType fonts into this format


 //display->drawXbm(x+35, y+20, WiFi_Logo_width, WiFi_Logo_height-20, WiFi_Logo_bits);
 //display->setTextAlignment(TEXT_ALIGN_CENTER);
 //display->setFont(ArialMT_Plain_10);
 display->drawString(x+60,y+15,"Local IP:" + WiFi.localIP().toString());
 display->drawString(x+60,y+25,"BadgeNumber:" + badgeName);
 display->drawString(x+60,y+35,"Verify Code:" + badgeVerifyCode);

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

/*
 * Connects to Wifi Network and tries for <attempts> seconds
 */
boolean wifiConnect(char* wSSID,char* wPassword,int attempts)
{
  WiFi.mode(WIFI_STA) ;
  Serial.printf("Wi-Fi mode set to WIFI_STA %s\n", WiFi.mode(WIFI_STA) ? "" : "Failed!");

  //delay(1000);
  Serial.print("[+] Connecting to network:");Serial.print(wSSID);Serial.print("-- password:");Serial.print(wPassword);Serial.println(".");
  WiFi.begin(wSSID, wPassword);
  int statNum = 0;
  
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print(".");
    delay(500);
    ESP.wdtFeed();
    if(attempts % 2 == 0)
    {
      if(statNum == 5)
      {
        statNum = 0;
      }
      
      statNum = statNum + 1;
      
      int baseColor = 128;
      writeShift(baseColor + pow(statNum,2));
      
    }
    attempts = attempts - 1;
    if(attempts == 0)
    {
      Serial.println("FAILED.");
      Serial.printf("Connection status: %d\n", WiFi.status());
      WiFi.printDiag(Serial);
      return false;
    }
    
  }
  Serial.println("PASSED.");
  
  //Dont even ask about this fuckshow. -- seriously, >_<
  badgeName = WiFi.macAddress();
  //Serial.println("starting:" + badgeName);
  badgeName.replace(":","");
  badgeName = badgeName.substring(4);
  //Serial.println("using:" + badgeName);
  char charBuf[50];
  int str_len = badgeName.length() + 1; 
  badgeName.toCharArray(charBuf, str_len);
  //Serial.println("using2:");Serial.println(charBuf);
  badgeNumber = strtoul(charBuf, NULL, 16);
  badgeName = String(badgeNumber);
  //Serial.println("ended with:" + badgeName);
  
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Badge Number: ");
  Serial.println(badgeName);
  
  return true;
}

/*
 * Initialise Wifi
 * ---------------
 * This first tries the default creds for 30s, then looks to see 
 * if anything is stored in the EEPROM and then asks the user for SSID/PW
 */

void initWiFi(boolean boot = false)
{
  
  // Try connect to the default WiFi SSID and Password
  boolean defaultConnect;
  
  defaultConnect = wifiConnect(defaultSSID,defaultPassword,30);

  
  if(defaultConnect == true)
  {
    strcpy(currentWiFi.ssid, defaultSSID);
    strcpy(currentWiFi.password, defaultPassword);
  }
  else
  {
    // Default SSID/PW doesnt work, lets try from the EEPROM
    char* EEPROMSSID;
    char* EEPROMPassword;   
    boolean EEPROMConnect = false;
    
    EEPROM_readAnything(0, currentWiFi);
    EEPROMConnect = wifiConnect(currentWiFi.ssid,currentWiFi.password,10);
    

    if(EEPROMConnect == true)
    {
      //Serial.println("Connected1!");
      //strcpy(currentWiFi.ssid, EEPROMSSID);
      //strcpy(currentWiFi.password, EEPROMPassword); 
      //Serial.println("Connected2!");
    }
    else
    {
      if(boot == true)
      {
      Serial.println("Cannot connect to valid WiFi network, please provide one.");
      
      char SerialSSID[32] = "";
      char SerialPassword[32] = "";
      String str;

      Serial.println("Please enter SSID:");
      while(Serial.available() == 0) {}
      if(Serial.available() > 0)
      {
          str = Serial.readStringUntil('\n');
      }
      
      str.toCharArray(SerialSSID,str.length());
      

      str = "";
      Serial.println("Please enter Password:");
      while(Serial.available() == 0) {}
      if(Serial.available() > 0)
      {
          str = Serial.readStringUntil('\n');
      }
      str.toCharArray(SerialPassword,str.length());
      
      
      boolean SerialConnect;
      SerialConnect = wifiConnect(SerialSSID,SerialPassword,10);

      if(SerialConnect == true)
      {
        strcpy(currentWiFi.ssid, SerialSSID);
        strcpy(currentWiFi.password, SerialPassword);
        
        EEPROM_writeAnything(0,currentWiFi);
        
        Serial.println("Written to EEPROM!");
      }
      else
      {
        Serial.println("Failed. Please reset to try again. ");
        delay(5000);
      }
      
      }
      else
      {
        initWiFi();
      }
    }

    
  }
}

String makeHTTPRequest(String URL)
{
  String decoded = "";
  URL = URL + "" + badgeName + "/";
  if (WiFi.status() == WL_CONNECTED) 
  {
      // Lets get the hash
      http.begin(hashEndPoint + "" + badgeName + "/");
      
      int httpCode = http.GET();
      if(httpCode > 0) 
      {
          // file found at server
          if(httpCode == HTTP_CODE_OK) 
          {
              String hashkey = http.getString();
              Serial.println("[+] Got Key for decoding:" + hashkey);
              http.end();


              http.begin(URL); 
              http.addHeader("Content-Type", "application/x-www-form-urlencoded");
              String postString = "seen=[";
              for (int i = 0; i < numBadges; i++)
              {
                //Serial.println("POST Badge:" + badgeList[i]);
                postString += '"';
                postString = postString + (String)badgeList[i];
                postString += '"';
                if(i < numBadges -1)
                {
                  postString += ",";
                }
              }
              postString += "]";
              int httpCode = http.POST(postString);      
              // start connection and send HTTP header
              //int httpCode = http.GET();
              if(httpCode > 0) 
              {
                  if(httpCode == HTTP_CODE_OK) 
                  {
                      String payload = http.getString();
                      Serial.println("[+] Got Payload:" + payload);
                      decoded = decodeShift(payload,hashkey);
                      Serial.println("[+] Decrypted: " + decoded);

                      numBadges = 0;
                  }
              } 
              else 
              {
                  //Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
              }
              http.end();
              
          }
      }
      else
      {
        Serial.println("[!] Got no response.");
      }
      
      
  }
  else
  {
    initWiFi();
  }

  return decoded;
}

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

/* shift in */
pinMode(pinStcp, OUTPUT);
 pinMode(pinShcp, OUTPUT);
 pinMode(pinDataIn, INPUT);
  
  
  
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);


initWiFi(true); // Initialise WiFi
  drawProgressBar(0, 10, "Display Init..",50);
  //Setup Serial Connection at ESP debug speed ( so we get debug information as well )
 
  
  //Setup EEPROM - 512 bytes!
  EEPROM.begin(512);

  drawProgressBar(10, 20, "EEPROM ..",50);

  
  drawProgressBar(20, 30, "Serial ..",50);
  Serial.setDebugOutput(true);

  
  drawProgressBar(30, 50, "Connecting to Wifi..",100);
  

  darkness();
  twirl();
  
  drawProgressBar(50, 60, "Joining network..",100);
  fetchStatus(); // fetch network status
  int fetchStatusEvent = t.every(15000, fetchStatus); // Set it to run every 15 seconds after this
  

  drawProgressBar(60, 70, "Configuring AI/IR",75);
  irrecv.enableIRIn(); // Start the receiver
  irsend.begin();
  int tickEvent2 = t.every(5550, transmitBadge);
  
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
  }
  else if(buttonVals & (1 << P2_Right))
  {
    darkness();
    display.displayOn();
    lowPowerMode = false;
  }


  
  
  


}



void transmitBadge()
{
  
  
  Serial.print("[+] IR TX: ");
  
  Serial.println(badgeNumber,HEX);
  irsend.sendSony(badgeNumber, 32);
 
}


void dump(decode_results *results) {
  int newBadge = results->value;
  //Serial.println("[+] IR RX");

  Serial.print("[*] IR RX: ");
  Serial.println(newBadge,HEX);

  int count = results->rawlen;
  
  if (results->decode_type == SONY) 
  {
    
    if(results->bits == 32)
    {
      int newBadge = results->value;
     // char charBuf[50];
      //String(newBadge).toCharArray(charBuf, 50);
      Serial.print("[*] Decoded Badge: ");Serial.println(newBadge,HEX);
      if(newBadge == badgeNumber)
      {
        Serial.println("[*] This is me! LOL!");
        return;
      }
      
      bool seenBadge = false;
      int i = 0;
      
      for (i = 0; i < numBadges; i++)
      {
        if(badgeList[i] == newBadge)
        {
          seenBadge = true;
        }
      }

      if(seenBadge == false)
      {
        
        int badgePos = numBadges;  
        if(badgePos > 6)
        {
          badgePos = 0;
        }
        badgeList[badgePos] = newBadge;
        numBadges++;
        Serial.println("[*] Adding as new Badge.");
        Serial.print("[*] List count at:");Serial.println(numBadges);
        
      }
      else
      {
       Serial.println("[*]  Already seen this badge.");
      }
  
    }
   
   
    
  }

}

void fetchStatus()
{
  
  String statusResult = makeHTTPRequest(checkInEndPoint);
  if(statusResult != "")
  {
    
    StaticJsonBuffer<500> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(statusResult);

    if (!root.success()) 
    {
        Serial.println("[!] parseObject() failed");
        return;
    }
    darkness();
    
    //String statusmsg          = root["statusmsg"];
    level = root["level"];
    team = root["team"].asString();
    alias = root["alias"].asString();
    badgeVerifyCode = root["verify"].asString();
    
    
    JsonArray& challengesWon    = root["challenges"];
    
    
    //twirl();
    if(lowPowerMode == false)
    {
      byte shift = root["shift"];
      Serial.println(shift,BIN);
      digitalWrite(latchPin, LOW); 
      currentShiftOut = shift;
      shiftOut(dataPin, clockPin,MSBFIRST,  shift); 
      digitalWrite(latchPin, HIGH);
    }
    else
    {
      Serial.println("Not updating.");
    }
    
    //Serial.println(statusmsg);

    for(JsonArray::iterator it=challengesWon.begin(); it!=challengesWon.end(); ++it) 
    {
        const char* value = *it;
        Serial.print("[+] Won Challenge:");Serial.println(value);   
    }
   
    
  }
  else
  {
    Serial.println("[!] Error! invalid status!" );
  }
  
  
}


String decodeShift(String input, String key)
{
  //this is our shift
  int keyLen = key.length();
  int inputLen = input.length();
  Serial.println("Decoding...");
  String output = "";
  for(int i=0; i<inputLen;i++)
  {
    char thisChar = input.charAt(i);
    //Serial.print("Char:");Serial.print(thisChar);
    int thisCharInt = (int)thisChar;
    //Serial.print(" Int:");Serial.print(thisCharInt);
    
    int outCharInt = thisCharInt + keyLen;
    //Serial.print(" OutInt:");Serial.print(outCharInt);
    output = output + ((char)outCharInt);
    //Serial.print(" OutChar:");Serial.print((char)outCharInt);Serial.println("!");
  }
  return output;
  //return (String)"This isnt it";
  
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
  
 if (irrecv.decode(&results)) {
    dump(&results);
    irrecv.resume(); // Receive the next value
  }
  
}
