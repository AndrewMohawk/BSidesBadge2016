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

void drawFrame1(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
    display->drawXbm(x,y+16, tblmnt_width, tblmnt_height, tblmnt_bits);
}

void drawFrame2(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {

  display->drawXbm(x,y+16, ship_width, ship_height, ship_bits);
  
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  
  display->setFont(ArialMT_Plain_10);
  display->drawString(x+32,y+16,"Challenges: 0");
    
  display->setFont(ArialMT_Plain_10);
  display->drawString(x+32,y+26,"Badge:");
  display->setFont(ArialMT_Plain_10);
  display->drawString(x+32,y+36,badgeName);

  display->setFont(ArialMT_Plain_10);
  display->drawString(x+32,y+46,"Code:");
  display->drawString(x+32,y+53,badgeVerifyCode);
  
}

 
void drawFrame3(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
 display->drawXbm(x, y+15, skull_width, skull_height, skull_bits);
 display->setTextAlignment(TEXT_ALIGN_RIGHT);
 display->setFont(ArialMT_Plain_10);
 display->drawString(x+85,y+15,"Challenge One");
 display->drawXbm(x+95, y+15, emptyheart_width, emptyheart_height, emptyheart_bits);

 display->drawXbm(x, y+30, skull_width, skull_height, skull_bits);
 display->drawString(x+85,y+30,"Challenge Two");
 display->drawXbm(x+95, y+30, halfheart_width, halfheart_height, halfheart_bits);

 display->drawXbm(x, y+45, skull_width, skull_height, skull_bits);
 display->drawString(x+95,y+45,"Challenge Three");
 display->drawXbm(x+95, y+45, fullheart_width, fullheart_height, fullheart_bits);

}

void drawFrame4(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawStringMaxWidth(0 + x, 13 + y, 128,currentSpeaker);
}


