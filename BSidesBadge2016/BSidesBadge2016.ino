#include "ESP8266WiFi.h"
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <EEPROM.h>
#include "EEPROMAnything.h"

MDNSResponder mdns;
WiFiServer server(80);
boolean debug = true;

char defaultSSID[32]     = "SSID";
char defaultPassword[32] = "Password";

char* currentSSID = "";
char* currentPassword = "";

struct WiFiSettings {
  char ssid[32];
  char password[32];
} currentWiFi;



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
  return true;
}


void setup() {
  //Setup Serial Connection at ESP debug speed ( so we get debug information as well )
  Serial.begin(74880);
  
  //Setup EEPROM - 512!
  EEPROM.begin(512);
  
  if(debug == true)
  {
    Serial.println("Starting Badge...");
    Serial.println("[+] Connecting to default WiFi");
  }

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
      //currentSSID = EEPROMSSID;
      //currentPassword = EEPROMPassword;
      strcpy(currentWiFi.ssid, EEPROMSSID);
      strcpy(currentWiFi.password, EEPROMPassword);
      
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
        //currentSSID = SerialSSID;
        //currentPassword = SerialPassword;
        strcpy(currentWiFi.ssid, SerialSSID);
        strcpy(currentWiFi.password, SerialPassword);
        
        EEPROM_writeAnything(0,currentWiFi);
        
        Serial.println("Written to EEPROM!");
      }
      else
      {
        Serial.println("Failed. Please reset to try again. Deepsleep in 5 seconds...");
        delay(5000);
        //ESP.deepsleep();
      }
      
      
    }

    
  }
    
  

}

void loop() {
  // put your main code here, to run repeatedly:

}
