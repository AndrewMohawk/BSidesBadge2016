

void pong_drawBall()
{
  if(pong_ball_x < 4)
  {
    pong_ball_x = 4;
  }

  if(pong_ball_x > 125)
  {
    pong_ball_x = 125;
  }
  
  //calc movement
  if(pong_ball_x <= 4)
  {
    //if((pong_p1place+pong_paddle_size) >= pong_ball_y)
    if(pong_ball_y >= pong_p1place && pong_ball_y <= pong_p1place + pong_paddle_size)
    {
      pong_ball_right = true;
      pong_ballmovement = pong_ballmovement * pong_speed_multiplyer;
      pong_ball_x = 4;
    }
    else
    {
      pong_p2score++;
      pong_ball_x = 5;
      pong_ball_y = random(5,45);
      pong_ball_right = true;
      pong_ballmovement = pong_startmovement;
      delay(400);
    }
  }

  if(pong_ball_x >= 125)
  {
    if(pong_ball_y >= pong_p2place && pong_ball_y <= pong_p2place + pong_paddle_size)
    {
      pong_ball_right = false;
      pong_ballmovement = pong_ballmovement * pong_speed_multiplyer;
      pong_ball_x = 125;
    }
    else
    {
      pong_p1score++;
      pong_ball_x = 123;
      pong_ball_y = random(5,45);
      pong_ball_right = false;
      pong_ballmovement = pong_startmovement;
      delay(400);
    }
  }
  

  if(pong_ball_y <= 0)
  {
    pong_ball_up = false;
  }
  
  if(pong_ball_y >= 64)
  {
    pong_ball_up = true;
  }

  if(pong_ball_up == true)
  {
    pong_ball_y = pong_ball_y - pong_ballmovement;
  }
  else
  {
    pong_ball_y = pong_ball_y + pong_ballmovement;
  }

  if(pong_ball_right == true)
  {
    pong_ball_x = pong_ball_x + pong_ballmovement;
  }
  else
  {
    pong_ball_x = pong_ball_x - pong_ballmovement;
  }
  
  display.fillRect(pong_ball_x,pong_ball_y,pong_ball_size,pong_ball_size);
  
}

void pong_drawPlayerPlayers()
{
  //player1
  display.fillRect(0,pong_p1place,3,pong_paddle_size);

  //player1
  display.fillRect(125,pong_p2place,3,pong_paddle_size);

  display.display();
}

void pong_drawPongField()
{
  
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.fillRect(62,0,4,64);
  display.setFont(ArialMT_Plain_10);
  
  display.drawString(50,0,(String)pong_p1score);
  display.drawString(70,0,(String)pong_p2score);
  
}



void pong_runPong()
{
  currTime = millis();
    
  if ((currTime - pong_time) > pong_delay) 
  {
    
    pong_drawPongField();
    pong_drawPlayerPlayers();
    pong_drawBall();
    
    pong_time = millis();

    

    display.display();
    
  
  }

  currTime = millis();
    
  if ((currTime - lastDebounceTime) > debounceDelay) 
  {
    byte buttonVals = readShiftByte();
    //buttonVals = 0;
    if(buttonVals == ((1 << P1_Right) | (1<<P2_Left)))
    {
      return;    
    }
    
    if(buttonVals & (1 << P1_Top))
    {
      if(pong_p1place > pong_paddle_movespeed)
      {
        pong_p1place=pong_p1place - pong_paddle_movespeed;
      }
    }
    if(buttonVals & (1 << P1_Bottom))
    {
      if(pong_p1place < 64 - pong_paddle_movespeed - pong_paddle_size)
      {
        pong_p1place=pong_p1place + pong_paddle_movespeed;
      }
      else
      {
        pong_p1place = 64 - pong_paddle_size;
      }
    }
    if(buttonVals & (1 << P2_Top))
    {
      if(pong_p2place > pong_paddle_movespeed)
      {
        pong_p2place=pong_p2place - pong_paddle_movespeed;
      }
      else
      {
        pong_p2place = 0;
      }
    }
    if(buttonVals & (1 << P2_Bottom))
    {
      if(pong_p2place < 64 - pong_paddle_movespeed - pong_paddle_size)
      {
        pong_p2place=pong_p2place + pong_paddle_movespeed;
      }
      else
      {
        pong_p2place = 64 - pong_paddle_size;
      }
    }
  }


if(pong_p1score == 5)
    {
      konamiCode(50);
      //playText("Player 1 Wins!");
      pong_p1score = 0;
      pong_p2score = 0;
      return;
    }
    if(pong_p2score == 5)
    {
      konamiCode(50);
      pong_p1score = 0;
      pong_p2score = 0;
      //playText("Player 2 Wins!");
      return;
    }


  pong_runPong();
}
