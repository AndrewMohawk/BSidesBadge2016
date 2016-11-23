
/*
 * Connects to Wifi Network and tries for <attempts> seconds
 */
boolean wifiConnect(char* wSSID,char* wPassword,int attempts)
{
  updating = true;
  WiFi.mode(WIFI_STA) ;
  //Serial.printf("Wi-Fi mode set to WIFI_STA %s\n", WiFi.mode(WIFI_STA) ? "" : "Failed!");
  //Serial.print("[+] Connecting to network:");Serial.print(wSSID);Serial.print("-- password:");Serial.print(wPassword);Serial.println(".");
  WiFi.begin(wSSID, wPassword);
  int statNum = 0;
  
  while (WiFi.status() != WL_CONNECTED) 
  {
    //Serial.print(".");
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
      setOutShift(baseColor + pow(statNum,2));
    }
    attempts = attempts - 1;
    if(attempts == 0)
    {
      Serial.println("FAILED.");
      Serial.printf("Connection status: %d\n", WiFi.status());
      //WiFi.printDiag(Serial);
      return false;
    }
    
  }
  //Serial.println("PASSED."); 
  
  //Dont even ask about this fuckshow. -- seriously, >_<
  badgeName = WiFi.macAddress();
  badgeName.replace(":","");
  badgeName = badgeName.substring(4);
  char charBuf[50];
  int str_len = badgeName.length() + 1; 
  badgeName.toCharArray(charBuf, str_len);
  badgeNumber = strtoul(charBuf, NULL, 16);
  badgeName = String(badgeNumber);
  
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Badge Number: ");
  Serial.println(badgeName);
  updating = false;
  return true;
}

/*
 * Initialise Wifi
 * ---------------
 * This first tries the default creds for 30s, then looks to see 
 * if anything is stored in the EEPROM for 20 
 * If this is the boot time it will then asks the user for SSID/PW ( and store to EEPROM )
 */

void initWiFi(boolean boot = false)
{
  updating = true;
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
    EEPROMConnect = wifiConnect(currentWiFi.ssid,currentWiFi.password,20);
    

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
  updating = false;
}
