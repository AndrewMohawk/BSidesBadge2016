
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


void readShift()
{
   int currentFrame = ui.getUiState()->currentFrame;

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
      Serial.println("[.] Button " + String(i) + " pressed!");
      buttonVals = buttonVals | (1 << i);
     }
    
    digitalWrite(pinShcp,HIGH);
  }

   
  if(buttonVals == ((1 << P1_Bottom) | (1<<P2_Bottom)))
  {
    lowPowerMode = true;
    Serial.println("[+] Turning Power Off!");
    display.displayOff();
    darkness();
    
  }
  else if(buttonVals & (1 << P1_Right))
  {
    display.displayOn();
    ui.nextFrame();
    lowPowerMode = false;
    lastAction = millis();
    
  }
  else if(buttonVals & (1 << P1_Left))
  {
    display.displayOn();
    ui.previousFrame();
    lowPowerMode = false;
    lastAction = millis();
  }
  else if(buttonVals & (1 << P1_Top))
  {
    lowPowerMode = false;
    display.displayOn();
    lastAction = millis();
  }
  else if(buttonVals & (1 << P1_Bottom))
  {
    lowPowerMode = false;
    display.displayOn();
    lastAction = millis();
  }
  else if(buttonVals & (1 << P2_Top))
  {
    lowPowerMode = false;
    display.displayOn();
    lastAction = millis();
    Serial.print("Free Memory:");
    Serial.println(ESP.getFreeHeap());
  }
  else if(buttonVals & (1 << P2_Left))
  {
    if (currentFrame == 3)
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
    
    
    lowPowerMode = false;
    display.displayOn();
    lastAction = millis();
  }
  else if(buttonVals & (1 << P2_Bottom))
  {
    lowPowerMode = false;
    display.displayOn();
    lastAction = millis();
  }
  else if(buttonVals & (1 << P2_Right))
  {
    if (currentFrame == 3)
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
    
    
    display.displayOn();
    lastAction = millis();
    lowPowerMode = false;
  }


  
  
  


}
