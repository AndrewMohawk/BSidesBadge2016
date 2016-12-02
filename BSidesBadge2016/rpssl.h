



void rpssl_drawPickChallenge()
{
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(30,0,">>Pick Challenger<<");
  if(rpssl_badgelist_num == 0)
  {
    display.drawString(20,14,"Searching for players...");
  }
  else
  {
    display.drawString(30,14,"P2_right to start");
  }
  int yval = 0;
  for(int i=0;i<rpssl_badgelist_num;i++)
  {
    yval = 24 + (i * 10);
    
    if(i == rpssl_badgelist_selected)
    {
      display.drawXbm(0, yval, rightarrow_width, rightarrow_height, rightarrow_bits);
    }
    display.drawString(15,yval,(String)rpssl_badgeList[i]);
  }
  
  
  
  display.display();
}


void rpssl_drawSelection()
{
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(30,0,">> R.P.S.S.L<<");
  
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  if(rpssl_mode == 0)
  {
    display.drawXbm(70, 18, rightarrow_width, rightarrow_height, rightarrow_bits);
    display.drawString(85,15,"Select");
    
    display.drawXbm(70, 35, uparrow_width, uparrow_height, uparrow_bits);
    display.drawString(85,30,"Start");
    
    display.drawXbm(65, 48, rightarrow_width, rightarrow_height, rightarrow_bits);
    display.drawXbm(75, 48, leftarrow_width, leftarrow_height, leftarrow_bits);
    display.drawString(85,45,"Quit");
  }
  
  if(rpssl_current == 0)
  {
    display.drawXbm(0, 16, rock_width, rock_height, rock_bits);
  }
  if(rpssl_current == 1)
  {
    display.drawXbm(0, 16, paper_width, paper_height, paper_bits);
  }
  if(rpssl_current == 2)
  {
    display.drawXbm(0, 16, scissors_width, scissors_height, scissors_bits);
  }
  if(rpssl_current == 3)
  {
    display.drawXbm(0, 16, spock_width, spock_height, spock_bits);
  }
  if(rpssl_current == 4)
  {
    display.drawXbm(0, 16, lizard_width, lizard_height, lizard_bits);
  }

  display.drawString(20,50,rpspl_str[rpssl_current]);
  
  display.display();
}



unsigned int rpssl_dump(decode_results *results) {
  int newBadge = results->value;
  

  

  int count = results->rawlen;
  
  if (results->decode_type == RC5 && rpssl_mode == 1) //Looking for a game
  {
    
    if(results->bits == 32)
    {
      
      if(newBadge == badgeNumber)
      {
        //Serial.println("[!] This is me! LOL!");
        return 0;
      }
      else
      {
        //Serial.print("[*] Received Badge looking for RPSSL via IR: ");Serial.println(newBadge,HEX);
        return newBadge;
      }
    }
  }


  return 0;

}

byte shift = 0;


    
  


void rpssl_showResults(String l1,String l2,String l3)
{
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(65,20,l1);
  display.drawString(65,30,l2);
  display.drawString(65,40,l3);

  display.display();

  display.invertDisplay();
  delay(300);
  display.normalDisplay();
  delay(300);
  display.invertDisplay();
  delay(300);
  display.normalDisplay();
  delay(300);
  display.invertDisplay();
  delay(300);
  display.normalDisplay();
  
  rpssl_mode = 4;
}
  



void rpssl_fetchResults()
{
  if(lowPowerMode == false)
  {
  
  display.clear();
  display.drawXbm(20,16, enterprise_width, enterprise_height, enterprise_bits);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(65,42,"Awaiting Result...");
  display.display();

  }
  
  String RPSSLEndPoint = blankEndPoint + "RPSSL/" + rpssl_challenger_badge + "/" + rpssl_my_selection + "/";
  String statusResult = makeHTTPRequest(RPSSLEndPoint);
  if(statusResult != "")
  {
    
    StaticJsonBuffer<500> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(statusResult);

    if (!root.success()) 
    {
        Serial.println("[!] parseObject() failed");
        return;
    }

    String state = root["state"].asString();

    if(state == "1")
    {
      
      rpssl_showResults(root["status1"].asString(),root["status2"].asString(),root["status3"].asString());
    }
    else if(state == "2" || state == "3")
    {
       rpssl_showResults("",root["status1"].asString(),"");
    }
    else if(state == "0")
    {
      //keep trying...
    }
    else
    {
      //wtf
    }
    
    
   
    
  }
  else
  {
    Serial.println("[!] Error! invalid status!" );
  }
  delay(random(200,5000));
}



void rpssl_main()
{
  unsigned int currTime = millis();
  if(rpssl_updateScreen == true)
  {
    if(rpssl_mode == 0)
    {
      rpssl_drawSelection();
    }
    
    if(rpssl_mode == 1)
    {
      rpssl_drawPickChallenge();
      //rpssl_drawSelection();
    }
    
    rpssl_updateScreen = false;
  }
  
  if(rpssl_mode == 0)
  {
    digitalWrite(latchPin, LOW); 
    shift = B00000000;
    shiftOut(dataPin, clockPin,MSBFIRST,  shift); 
    digitalWrite(latchPin, HIGH);
    
    
  }
  
  if (rpssl_mode == 1)
  {
    /* searching for challengers*/ 
    digitalWrite(latchPin, LOW); 
    shift = B00011111;
    shiftOut(dataPin, clockPin,MSBFIRST,  shift); 
    digitalWrite(latchPin, HIGH);

    if ((currTime - rpssl_irsendLastTime) > rpssl_irsendDelayTime) 
    {
   
        irsend.sendRC5(badgeNumber, 32); // we are looking for a game
      
      rpssl_irsendLastTime = currTime;
      delay(random(0,100));
    }
    
    if (irrecv.decode(&results)) 
    {
        unsigned int modeRX = rpssl_dump(&results);
        if(modeRX > 0)
        {
          
          bool alreadySeen = false;
          for(int z=0;z<rpssl_badgelist_num;z++)
          {
            if(rpssl_badgeList[z] == modeRX)
            {
              alreadySeen = true;
            }
          }
          
          if(alreadySeen == false)
          {
            if(rpssl_badgelist_id = rpssl_badgelist_max)
            {
              rpssl_badgelist_id = 0;
            }
            
            rpssl_badgeList[rpssl_badgelist_id] = modeRX;
            rpssl_badgelist_num++;
            rpssl_updateScreen = true;
          }
          
        }
        
        irrecv.resume(); // Receive the next value
    }
        
  }

  if (rpssl_mode == 2)
  {
    
    if ((currTime - rpssl_irsendLastTime) > rpssl_irsendDelayTime) 
    {
     
      irsend.sendRC5(badgeNumber, 32); // we are looking for a game
      
      rpssl_irsendLastTime = currTime;
      delay(random(0,100));
    }
    
    /* pinging server for result */ 
    digitalWrite(latchPin, LOW); 
    shift = B00011111;
    shiftOut(dataPin, clockPin,MSBFIRST,  shift); 
    digitalWrite(latchPin, HIGH);

    rpssl_fetchResults();

    

    
  }
  
  currTime = millis();
  if ((currTime - lastDebounceTime) > debounceDelay) 
  {
    byte buttonVals = readShiftByte();
    if(buttonVals != 0)
    {
      if (rpssl_mode == 4)
      {
        rpssl_mode = 0;
        rpssl_badgelist_num = 0;
        buttonVals = 0;
        rpssl_updateScreen = true;
      }
    }
    
    if(buttonVals == ((1 << P1_Right) | (1<<P2_Left)))
    {
      return;    
    }
    
    if(buttonVals & (1 << P1_Top))
    {
      
    }
    
    if(buttonVals & (1 << P2_Bottom))
    {
      
      if(rpssl_mode == 1)
      {

        if(rpssl_badgelist_selected < (rpssl_badgelist_num-1))
        {
          rpssl_badgelist_selected++;
        }
         rpssl_updateScreen = true;
      }
      
    }

    if(buttonVals & (1 << P2_Top))
    {
      
      if(rpssl_mode == 0)
      {
        
        rpssl_my_selection = rpssl_current;
        
        rpssl_mode = 1;
        
      }
      else if(rpssl_mode == 1)
      {
        if(rpssl_badgelist_selected > 0)
        {
          rpssl_badgelist_selected--;
        }
         
      }
      rpssl_updateScreen = true;
      
    }
    if(buttonVals & (1<<P2_Left))
    {
      if(rpssl_mode == 0)
      {
        if(rpssl_current > 0)
        {
          rpssl_current--;
        }
        else if(rpssl_current == 0)
        {
          rpssl_current = 4;
        }
      }
      rpssl_updateScreen = true;
    }

    if(buttonVals & (1<<P2_Right))
    {
      if(rpssl_mode == 0)
      {
        if(rpssl_current < 4)
        {
          rpssl_current++;
        }
        else if(rpssl_current == 4)
        {
          rpssl_current = 0;
        }
      }
      if(rpssl_mode == 1)
      {
        if(rpssl_badgelist_num > 0)
        {
          rpssl_challenger_badge = rpssl_badgeList[rpssl_badgelist_selected];
          Serial.print("picking:");Serial.print(rpssl_badgelist_selected);Serial.print("-");Serial.println(rpssl_badgeList[rpssl_badgelist_selected]);
          rpssl_mode = 2;
        }
      }
      rpssl_updateScreen = true;
    }
  }


  
  delay(50);
  rpssl_main();

  
}

