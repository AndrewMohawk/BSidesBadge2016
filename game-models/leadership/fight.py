import random

class Fight:
  def fight(self, clan_member_a, clan_member_b,clans):
    if clan_member_a == clan_member_b:
      return
    
    debug = False
    diff = 1
    
    if(debug == True):
      print "Start:\n A-- lvl: %s clan: %s \n B-- lvl: %s clan: %s " % (clan_member_a.clan_level,clan_member_a.clan.colour,clan_member_b.clan_level,clan_member_b.clan.colour)
    
    if(clan_member_a.clan.colour == clan_member_b.clan.colour):
      if(debug == True):
        print "Same team! %s and %s " % (clan_member_a.clan.colour,clan_member_b.clan.colour)
      if(random.randint(0,(clan_member_a.clan_level * 10 )) == 1):
        
          
        for c in clans:
          if(c.colour == clan_member_a.clan.colour):
            clans.remove(c)
            
        defectClan = random.choice(clans);
        if(debug == True):
          print "defecting player a from %s to %s!" % (clan_member_a.clan.colour,defectClan.colour);
        
        defectClan.add_member(clan_member_a);

        
      else:
        diff = -1
    
    
    
    
    
    
    
    if(clan_member_a.clan_level > clan_member_b.clan_level):
      winner = clan_member_a
      winner.clan_level -=diff
      loser = clan_member_b
      #print "A Won! (%s)" % diff
    elif(clan_member_b.clan_level > clan_member_a.clan_level):
      winner = clan_member_b
      winner.clan_level -=diff
      loser = clan_member_b
      #print "B Won! (%s)" % diff
    else:
      winner = random.choice([clan_member_a, clan_member_b])
      loser = clan_member_b if (clan_member_a == winner) else clan_member_a
      
      if(clan_member_a.clan.colour == clan_member_b.clan.colour):
        winner.clan_level -=diff
     # print "Rand! (%s)" % diff
    
    if(winner.clan_level == 0):
      winner.clan_level = 1
    if(loser.clan_level == 0):
      loser.clan_level = 1
      
    if(winner.clan_level > 5):
      winner.clan_level = 5
    if(loser.clan_level > 5):
      loser.clan_level = 5
      
    winner.clan.add_member(loser)
    
    #print "End:\nStart:\n A-- lvl: %s clan: %s \n B-- lvl: %s clan: %s \n" % (clan_member_a.clan_level,clan_member_a.clan.colour,clan_member_b.clan_level,clan_member_b.clan.colour)
    if(debug == True):
      print "complete:\nStart:\n A-- lvl: %s clan: %s \n B-- lvl: %s clan: %s \n\n" % (winner.clan_level,winner.clan.colour,loser.clan_level,loser.clan.colour)
    
    
    #winner.clan_level += 1
    #loser.clan_level -= 1
    
    #if loser.clan_level <= 0:
    #  winner.clan.add_member(loser)
      
    #if winner.clan_level > 5:
    #  winner.clan_level = 5
