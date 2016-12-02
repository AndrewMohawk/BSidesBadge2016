

String encryptionTypeStr(uint8_t authmode) {
    switch(authmode) {
        case ENC_TYPE_NONE:
            return "NONE";
        case ENC_TYPE_WEP:
            return "WEP";
        case ENC_TYPE_TKIP:
            return "TKIP WPA";
        case ENC_TYPE_CCMP:
            return "CCMP WPA2";
        case ENC_TYPE_AUTO:
            return "AUTO";
        default:
            return "?";
    }
}

String wifiScanner_EncryptionType(int thisType) {
  // read the encryption type and print out the name:
  switch (thisType) {
    case ENC_TYPE_WEP:
      return "WE";
      break;
    case ENC_TYPE_TKIP:
      return "WP";
      break;
    case ENC_TYPE_CCMP:
      return "W2";
      break;
    case ENC_TYPE_NONE:
      return "OP";
      break;
    case ENC_TYPE_AUTO:
      return "AU";
      break;
    default:
      return "??";
  }
}


void wifiScanner_performScan()
{
  
  display.clear();
  display.drawString(30,0,">> Scanning <<");
  display.display();

  
  wifiScanner_networksFound = WiFi.scanNetworks(false,false);
  
  if(wifiScanner_networksFound > 30)
  {
    wifiScanner_networksFound = 30;
  }
  int loops = 0;

  if (wifiScanner_networksFound > 0)
  {
   
    int indices[wifiScanner_networksFound];
    for (int i = 0; i < wifiScanner_networksFound; i++) {
      indices[i] = i;
    }


    for (int i = 0; i < wifiScanner_networksFound; i++) 
    {
      for (int j = i + 1; j < wifiScanner_networksFound; j++) 
      {
        if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i])) 
        {
          std::swap(indices[i], indices[j]);  
          
        }
      }
    }
    

    display.clear();
    display.drawString(10,0,">>" + (String)(wifiScanner_networksFound) + " Networks Found <<");
    display.display();
    
    Serial.println("00: (RSSI)[BSSID][hidden] SSID [channel] [encryption]");
    for (int i = 0; i < wifiScanner_networksFound; ++i)
    {
      String ssid  = (String)WiFi.SSID(indices[i]);
      if(ssid.length() > 15)
      {
        ssid = ssid.substring(0,15) + "...";
      }
      String network = "[" + (String)WiFi.RSSI(indices[i]) + "]" + "[" + (String) wifiScanner_EncryptionType(WiFi.encryptionType(indices[i])) + "]" + ssid;
      wifiScanner_networks[i] = network;
      
      Serial.printf("%02d", i + 1);
      Serial.print(":");

      Serial.print(" (");
      Serial.print(WiFi.RSSI(indices[i]));
      Serial.print(")");

      Serial.print(" [");
      Serial.print(WiFi.BSSIDstr(indices[i]));
      Serial.print("]");

      Serial.print(" [");
      Serial.print((String) WiFi.isHidden(indices[i]));
      Serial.print("]");

      Serial.print(" " + WiFi.SSID(indices[i]));
      // Serial.print((WiFi.encryptionType(indices[i]) == ENC_TYPE_NONE)?" ":"*");

      Serial.print(" [");
      Serial.printf("%02d",(int)WiFi.channel(indices[i]));
      Serial.print("]");

      Serial.print(" [");
      Serial.print((String) encryptionTypeStr(WiFi.encryptionType(indices[i])));
      Serial.print("]");
      Serial.println("");
    }
    
     
     
  }





  
}


void wifiScanner_displayNetworks()
{
 
  display.clear();
  display.drawString(0,0,">>  " + (String)(wifiScanner_networksFound) + " Networks Found <<");
  
  int top = wifiScanner_currentTop + wifiScanner_numNetworksShown;
  if(top > wifiScanner_networksFound)
  {
    top  = wifiScanner_networksFound;
  }
  int ypos = 13;
  for (int i = wifiScanner_currentTop; i < top; ++i)
  {
      display.drawString(0,ypos,wifiScanner_networks[i]);    
      ypos = ypos + wifiScanner_networksScreen_y;
  }
   display.display();
  
  
}





void wifiScanner_startScanner()
{
  
  if(wifiScanner_scanfornetworks == true)
  {
    wifiScanner_performScan();
    wifiScanner_scanfornetworks = false;
    wifiScanner_displayNetworks();
  }

  
  currTime = millis();
  if ((currTime - lastDebounceTime) > debounceDelay) 
  {
    byte buttonVals = readShiftByte();
    
    if(buttonVals == ((1 << P1_Right) | (1<<P2_Left)))
    {
      return;    
    }
    
    if(buttonVals & (1 << P1_Top))
    {
      wifiScanner_currentTop = 0;
      wifiScanner_scanfornetworks = true;
    }
    
    if(buttonVals & (1 << P2_Bottom))
    {
      
      if(wifiScanner_currentTop < wifiScanner_networksFound - wifiScanner_numNetworksShown)
      {
         wifiScanner_currentTop++;
      }
      wifiScanner_displayNetworks();
      
    }

    if(buttonVals & (1 << P2_Top))
    {
      
      if(wifiScanner_currentTop > 0)
      {
         wifiScanner_currentTop--;
      }
      wifiScanner_displayNetworks();
      
    }
    
  }

  // Wait a bit before scanning again
  delay(200);
  wifiScanner_startScanner();
  
  
}

