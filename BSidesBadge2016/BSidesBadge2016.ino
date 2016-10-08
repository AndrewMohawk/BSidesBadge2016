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
SSD1306  display(0x3c, 5, 2);
OLEDDisplayUi ui     ( &display );

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
const int latchPin = 14;
//Pin connected to clock pin (SH_CP) of 74HC595
const int clockPin = 12;
//Pin connected to Data in (DS) of 74HC595
const int dataPin = 13;

/* Shift In  */
const int pinShcp = 15; //Clock
const int pinStcp = 0; //Latch
const int pinDataIn = 16; // Data

MDNSResponder mdns;
WiFiServer server(80);
HTTPClient http;

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
//String hashEndPoint = "http://badges2016.andrewmohawk.com/hash.html";
String hashEndPoint = "http://10.85.0.200:8000/gethash/";
String checkInEndPoint = "http://10.85.0.200:8000/checkin/";
String badgeName = "";
/*
 * Connects to Wifi Network and tries for <attempts> seconds
 */
boolean wifiConnect(char* wSSID,char* wPassword,int attempts)
{
  
  Serial.print("[+] Connecting to network:");Serial.print(wSSID);Serial.print("-- password:");Serial.print(wPassword);Serial.println(".");
  WiFi.begin(wSSID, wPassword);
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print(".");
    delay(1000);
    attempts = attempts - 1;
    if(attempts == 0)
    {
      Serial.println("FAILED.");
      return false;
    }
  }
  
  Serial.println("PASSED.");
  WiFi.macAddress(MAC_array);
  
  
 for (int i = 0; i < sizeof(MAC_array); ++i){
  badgeName = badgeName + MAC_array[i];
 }
  Serial.println("BadgeNumber:");
  Serial.println(badgeName);
  

  
  Serial.print("MAC Address:");
  for (int i = 0; i < sizeof(MAC_array); ++i){
    sprintf(MAC_char,"%s%02x:",MAC_char,MAC_array[i]);
  }

  Serial.println(MAC_char);

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  
  return true;
}

/*
 * Initialise Wifi
 * ---------------
 * This first tries the default creds for 30s, then looks to see 
 * if anything is stored in the EEPROM and then asks the user for SSID/PW
 */

void initWiFi()
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

    
  }
}

String makeHTTPRequest(String URL)
{
  String decoded = "";
  URL = URL + "" + badgeName;
  if (WiFi.status() == WL_CONNECTED) 
  {
      // Lets get the hash
      http.begin(hashEndPoint + "" + badgeName);
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
                    
              // start connection and send HTTP header
              int httpCode = http.GET();
              if(httpCode > 0) 
              {
                  if(httpCode == HTTP_CODE_OK) 
                  {
                      String payload = http.getString();
                      Serial.println("[+] Got Payload:" + payload);
                      decoded = decodeShift(payload,hashkey);
                      Serial.println("[+] Decryped: " + decoded);
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
        Serial.println("Got no response.");
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
  Serial.println("Setting " + String(whichPin) + " to High");
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

/*
 * Main Setup for badge.
 */
void setup() {

/* shift out */
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
  
  
  
  
  display.init();
  display.setFont(ArialMT_Plain_10);
  if(debug == true)
  {
    Serial.println("[+] Display Initialised");
  }

  drawProgressBar(0, 10, "Display Init..",50);
  //Setup Serial Connection at ESP debug speed ( so we get debug information as well )
  Serial.begin(74880);
  
  //Setup EEPROM - 512 bytes!
  EEPROM.begin(512);

  drawProgressBar(10, 20, "EEPROM ..",50);
  
  drawProgressBar(20, 30, "Serial ..",50);
  if(debug == true)
  {
    Serial.println("Starting Badge...");
    Serial.println("[+] Connecting to default WiFi");
  }
  
  drawProgressBar(30, 50, "Connecting to Wifi..",100);
  initWiFi(); // Initialise WiFi
  
  drawProgressBar(50, 60, "Joining network..",100);
  fetchStatus(); // fetch network status
  int fetchStatusEvent = t.every(15000, fetchStatus); // Set it to run every 15 seconds after this
  

  drawProgressBar(60, 70, "Configuring AI",75);
  drawProgressBar(70, 80, "Hacking Planet..",75);
  for(int i=0;i<=8;i++)
  {
    registerWrite(i,1);
    delay(100);
  }
  drawProgressBar(80, 90, "Starting nice game",75);
  drawProgressBar(90, 100, "...of chess...",50);
  

}

void fetchStatus()
{
  String statusURL = "http://10.85.0.241/badge/fetchStatus.php";
  String statusResult = makeHTTPRequest(checkInEndPoint);
  if(statusResult != "")
  {
    
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(statusResult);

    if (!root.success()) 
    {
        Serial.println("[!] parseObject() failed");
        return;
    }
    
    byte shift = root["shift"];
    String statusmsg          = root["statusmsg"];
    JsonArray& challengesWon    = root["challenges"];
    

    Serial.println(shift,BIN);
    //writeShift(shift);
    digitalWrite(latchPin, LOW); 
    byte curb = shift;
    Serial.println(curb,BIN);
    shiftOut(dataPin, clockPin,LSBFIRST,  curb); 
    digitalWrite(latchPin, HIGH);

    
    Serial.println(statusmsg);

    for(JsonArray::iterator it=challengesWon.begin(); it!=challengesWon.end(); ++it) 
    {
        const char* value = *it;
        Serial.print("[+] Won Challenge:");Serial.println(value);   
    }
   
    
  }
  else
  {
    Serial.println("[!] Error! invalid status!");
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
  // put your main code here, to run repeatedly:
  t.update();
}
