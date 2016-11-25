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




void msOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {
  
   display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(0, 0, String(level));
  if(updating == false)
  {
    display->drawXbm(8, 0, fullheart_width, fullheart_height, fullheart_bits);
  }
  else
  {
    display->drawXbm(8, 0, emptyheart_width, emptyheart_height, emptyheart_bits);
  }
  
  

  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_10);
  display->drawString(53, 0, alias);


  display->drawXbm(87, 0, space_width, space_height, space_bits);
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(128, 0, team);
  
}

void bsidesLogoFrame(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
    display->drawXbm(x,y+16, tblmnt_width, tblmnt_height, tblmnt_bits);
}

void playerInfoFrame(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {

  display->drawXbm(x,y+16, ship_width, ship_height, ship_bits);
  
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  
  display->setFont(ArialMT_Plain_10);
  display->drawString(x+32,y+16,"Challenges:" + String(completedChallenges));
    
  display->setFont(ArialMT_Plain_10);
  display->drawString(x+32,y+26,"Badge:");
  display->setFont(ArialMT_Plain_10);
  display->drawString(x+32,y+36,badgeName);

  display->setFont(ArialMT_Plain_10);
  display->drawString(x+32,y+45,"Code:");
  display->drawString(x+32,y+54,badgeVerifyCode);
  
}

 

void ScheduleFrame(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  if(currentScheduleItem == 0)
  {
    display->drawXbm(x+50, y+15, camera_width, camera_height, camera_bits);
    display->drawStringMaxWidth(0 + x, 40 + y, 128,currentSpeaker);
  }
  else
  {
    display->drawStringMaxWidth(0 + x, 13 + y, 128,currentSpeaker);
  }
}


void ChallengeFrame(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {

 display->drawXbm(x, y+16, challenges_width, challenges_height, challenges_bits);
 display->setTextAlignment(TEXT_ALIGN_CENTER);
 display->setFont(ArialMT_Plain_10);
 display->drawXbm(x+50, y+38, uparrow_width, uparrow_height, uparrow_bits);
 display->drawString(x+80,y+35," = Play");
 
  
 display->drawXbm(x+5, y+50, leftarrow_width, leftarrow_height, leftarrow_bits);
 if(completedChallenges == 0)
 {
  display->drawString(x+70,y+50,"None completed ");
 }
 else
 {
   display->drawString(x+70,y+50,Challenges[currentListedChallenge]);
 }
 display->drawXbm(x+120, y+50, rightarrow_width, rightarrow_height, rightarrow_bits);
  
}

void konamiCode(int delaySpeed=100)
{
  Serial.println("[+] KONAMI CODE!");
  int y = 0;
  for (int16_t x=0; x<DISPLAY_WIDTH; x+=4) {
    display.clear();
    y = random(0,15);
    display.drawXbm(x, y, skeleton_width, skeleton_height, skeleton_bits);
    display.display();
    delay(delaySpeed);
  }
}

void playAlias()
{
  Serial.println("[+] Play Alias!");
  int x = 0;
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  for (int16_t y=0; y<DISPLAY_HEIGHT; y+=2) {
    display.clear();
    x = random(0,DISPLAY_WIDTH/2);
    display.drawString(x, y, alias);
    display.display();
    delay(100);
  }
}

void playText(String thetext)
{
  Serial.println("[+] Play Alias!");
  int x = 0;
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  for (int16_t y=0; y<DISPLAY_HEIGHT; y+=2) {
    display.clear();
    x = random(0,DISPLAY_WIDTH/2);
    display.drawString(x, y, thetext);
    display.display();
    delay(100);
  }
}

void playNinja()
{
  Serial.println("[+] Challenge completed!");
  
  int y = 0;
  for (int16_t x=DISPLAY_WIDTH; x>=0; x-=5) {
    display.clear();
    y = 0;
    display.drawXbm(x, y, ninja_width, ninja_height, ninja_bits);
    display.display();
    
  }
  
  display.invertDisplay();
  delay(300);
  display.normalDisplay();
  delay(300);
  display.invertDisplay();
  delay(300);
  display.normalDisplay();
  delay(300);
}


