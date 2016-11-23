
/*
 * 'Decrypts' a shit string...
 */
String decodeShift(String input, String key)
{
  //this is our shift
  int keyLen = key.length();
  int inputLen = input.length();
  String output = "";
  for(int i=0; i<inputLen;i++)
  {
    char thisChar = input.charAt(i);
    int thisCharInt = (int)thisChar;   
    int outCharInt = thisCharInt + keyLen;
    output = output + ((char)outCharInt);

  }
  return output;
  
}

/*
 * Creates HTTP request, first fetches the hash, then decodes for whatever URL
 */
String makeHTTPRequest(String URL)
{
  updating = true;
  //disableInterrupts()
  String decoded = "";
  URL = URL + "" + badgeName + "/";
  Serial.println("[+] About to make HTTP Request");
  if (WiFi.status() == WL_CONNECTED) 
  {
      // Lets get the hash
      http.begin(hashEndPoint + "" + badgeName + "/");
      
      int httpCode = http.GET();
      Serial.println("[+] HTTP Request completed");
      if(httpCode > 0) 
      {
          // file found at server
          if(httpCode == HTTP_CODE_OK) 
          {
              String hashkey = http.getString();
              Serial.println("[+] Got Key for decoding:" + hashkey);
              http.end();

              Serial.println("[+] Starting fetch");
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
              Serial.println("[+] Making POST");
              int httpCode = http.POST(postString);    
              Serial.println("[+] POST complete");  
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
  updating = false;
  return decoded;
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


void transmitBadge()
{
  
  if(updating == false)
  {
    Serial.print("[+] IR TX: ");
    
    Serial.println(badgeNumber,HEX);
    irsend.sendSony(badgeNumber, 32);
  }
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
      Serial.print("[*] Received Badge via IR: ");Serial.println(newBadge,HEX);
      if(newBadge == badgeNumber)
      {
        Serial.println("[!] This is me! LOL!");
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

