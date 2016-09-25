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
const int latchPin = 12;
const int clockPin = 14;
const int dataPin = 13;

/* Shift In  */
const int pinShcp = 15; //Clock
const int pinStcp = 0; //Latch
const int pinDataIn = 16; // Data

MDNSResponder mdns;
WiFiServer server(80);
HTTPClient http;

boolean debug = true;

char defaultSSID[32]     = "SSID";
char defaultPassword[32] = "Password";


struct WiFiSettings {
  char ssid[32];
  char password[32];
} currentWiFi;


uint8_t MAC_array[6];
char MAC_char[18];
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
  defaultConnect = wifiConnect(defaultSSID,defaultPassword,1);

  
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

void makeHTTPRequest(String URL)
{
  String hashEndPoint = "http://badges2016.andrewmohawk.com/hash.html";
  if (WiFi.status() == WL_CONNECTED) 
  {
      // Lets get the hash
      http.begin(hashEndPoint); 
      int httpCode = http.GET();
      if(httpCode > 0) 
      {
          //Serial.printf("[HTTP1] GET... code: %d\n", httpCode);
          // file found at server
          if(httpCode == HTTP_CODE_OK) 
          {
              String hashkey = http.getString();
              Serial.println("Got hash:" + hashkey);
              http.end();


              http.begin(URL); 
                    
              // start connection and send HTTP header
              int httpCode = http.GET();
              if(httpCode > 0) 
              {
                  // HTTP header has been send and Server response header has been handled
                  //Serial.printf("[HTTP] GET... code: %d\n", httpCode);
                  // file found at server
                  if(httpCode == HTTP_CODE_OK) 
                  {
                      String payload = http.getString();
                      Serial.println("Got Payload:" + payload);
                      String decoded = decodeShift(payload,hashkey);
                      Serial.println("Decryped: " + decoded);
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
      
      
      http.begin(URL); 
      
      // start connection and send HTTP header
      httpCode = http.GET();
      if(httpCode > 0) 
      {
          // HTTP header has been send and Server response header has been handled
          //Serial.printf("[HTTP] GET... code: %d\n", httpCode);
          // file found at server
          if(httpCode == HTTP_CODE_OK) 
          {
              String payload = http.getString();
              Serial.println(payload);
          }
      } 
      else 
      {
          //Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
      http.end();
  }
  else
  {
    initWiFi();
  }
}

/*
 * Main Setup for badge.
 */
void setup() {
  //Setup Serial Connection at ESP debug speed ( so we get debug information as well )
  Serial.begin(74880);
  
  //Setup EEPROM - 512 bytes!
  EEPROM.begin(512);
  
  if(debug == true)
  {
    Serial.println("Starting Badge...");
    Serial.println("[+] Connecting to default WiFi");
  }

  initWiFi(); // Initialise WiFi

  makeHTTPRequest("http://badges2016.andrewmohawk.com/");
    
  

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

}
