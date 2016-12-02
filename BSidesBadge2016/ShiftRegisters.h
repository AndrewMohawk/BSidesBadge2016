

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

void setOutShift(byte shift)
{
  digitalWrite(latchPin, LOW); 
  shiftOut(dataPin, clockPin,MSBFIRST,  shift); 
  digitalWrite(latchPin, HIGH);

}

void darkness()
{
  byte shift = 0;
  //Serial.println(shift,BIN);
  setOutShift(shift);
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

byte readShiftByte()
{
  currTime = millis();
    
  if ((currTime - lastDebounceTime) > debounceDelay) 
  {
    
    lastDebounceTime = currTime;
    
    int inputPin = 1;
    byte buttonVals = 0;
    int buttonPressedVal = 1; //Depending on how buttons are wired
    
    digitalWrite(pinStcp, LOW);
    delayMicroseconds(20);
    digitalWrite(pinStcp, HIGH);
    
    
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
    return buttonVals;
  }
  return 0;
}

#include "pong.h" // Pong
#include "warbadging.h" // WarBadging


void readShift()
{
   int currentFrame = ui.getUiState()->currentFrame;
  
  byte buttonVals = readShiftByte();
  String thisBut = "";

   
  if(buttonVals == ((1 << P1_Bottom) | (1<<P2_Bottom)))
  {
    lowPowerMode = true;
    display.clear();
    display.drawXbm(0, 0, charging_width, charging_height, charging_bits);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(65,32,"Press Reset to start badge...");
    display.display();
    darkness();
    Serial.println("[+] Turning ESP Off!");
    ESP.deepSleep(999999999*999999999U, WAKE_NO_RFCAL);
    
    
    
    
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
    if(currentFrame == 4)
    {
      if(aliasSet == true)
      {
        startRPSSL = true;
      }
    }
    if (currentFrame == 3)
    {
      if(Challenges[currentListedChallenge] == "Skeleton" || Challenges[currentListedChallenge] == "HappyCorp")
      {
        konamiCode();
      }
      else if(Challenges[currentListedChallenge] == "Alias")
      {
        playAlias();
      }
      
      else if(Challenges[currentListedChallenge] == "Pong" || Challenges[currentListedChallenge] == "RetroRom")
      {
        pong_time = millis();
        pong_runPong();
      }
      else if(Challenges[currentListedChallenge] == "SecretSquirrel" || Challenges[currentListedChallenge] == "Warbadge")
      {
        display.clear();
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.drawString(20,0,">> WarBadging! <<");
        display.drawString(0,15,"P1_Up - Scan for WiFi");
        display.drawString(0,25,"P2_UP/Down - Move list");
        display.drawString(0,35,"P1_Right + P2_Left - Quit.");
        display.drawString(80,50,"-AM");
        display.display();
        wifiScanner_startScanner();
      }
      else if(completedChallenges > 0)
      {
        playText(Challenges[currentListedChallenge]);
      }
    }
    
    lastAction = millis();
    Serial.print("Free Memory:");
    Serial.println(ESP.getFreeHeap());
    
  }
  else if(buttonVals & (1 << P2_Left))
  {
    thisBut = "A";
    if (currentFrame == 2)
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
    
    
    lowPowerMode = false;
    display.displayOn();
    lastAction = millis();
  }
  else if(buttonVals & (1 << P2_Bottom))
  {
    thisBut = "C";
    lowPowerMode = false;
    display.displayOn();
    lastAction = millis();
  }
  else if(buttonVals & (1 << P2_Right))
  {
    thisBut = "D";
    if (currentFrame == 2)
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
    
    
    display.displayOn();
    lastAction = millis();
    lowPowerMode = false;
  }



if (thisBut != "")
{
  lastButtons = lastButtons + thisBut;
  if(lastButtons.length() > 10)
  {
    lastButtons = lastButtons.substring(1);
  }
}

if(lastButtons.substring(0,8) == "UUDDLRLR" || lastButtons.substring(1,9) == "UUDDLRLR" || lastButtons.substring(2,10) == "UUDDLRLR")
{
  //because singe hates konami.
  playNinja();

  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(20,0,">> Secret Message! <<");
  display.drawString(20,15,"There's a wifi");
  display.drawString(20,25,"net you can't");
  display.drawString(20,35,"see. It's hidden");
  display.drawString(20,45,"not easy.@JP_14c");
  display.display();
  delay(6000);
  lastButtons = "";
}


/*if(lastButtons == "UUDDLRLRAB")
{
  
  if(addChallenge("Konami"))
  {
    playNinja();
    delay(200);
    konamiCode();
  }
  lastButtons = "";
}
*/
if(lastButtons == "LRLRLRLRLR")
{
  if(addChallenge("Pong"))
  {
    playNinja();
  }
  lastButtons = "";
}

if(lastButtons == "ULDRBACDUU")
{
  if(addChallenge("Warbadge"))
  {
    playNinja();
  }
  lastButtons = "";
}
if(lastButtons == "BDCARDLUCC")
{
  if(addChallenge("Skeleton"))
  {
    playNinja();
  }
  lastButtons = "";
}

  
  
  


}

