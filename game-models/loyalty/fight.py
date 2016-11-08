import random

class Fight:
  def fight(self, clan_member_a, clan_member_b,clans):
    
    debug = False
    diff = 1
    
    if(debug == True):
      print "Start:\n A-- lvl: %s clan: %s \n B-- lvl: %s clan: %s " % (clan_member_a.clan_level,clan_member_a.clan.colour,clan_member_b.clan_level,clan_member_b.clan.colour)
    
    # if its the same, then meh, who cares.
    if clan_member_a == clan_member_b:
      return
       
    # if these two are from the same team, 1 in level*10 chance of defecting, 1 in 10 chance of improving
    if(clan_member_a.clan.colour == clan_member_b.clan.colour):
      if(debug == True):
        print "Same team! %s and %s " % (clan_member_a.clan.colour,clan_member_b.clan.colour)
      
      #1 in level*10 chance of defecting
      if(random.randint(0,(clan_member_a.clan_level * 10 )) == 1): 
        #defect one of the players to another team and drop a level
        defectClan = random.choice(clans);
        
        if(clan_member_a.clan_level > 1):
          clan_member_a.clan_level = clan_member_a.clan_level - 1
        
        if(debug == True):
          print "defecting player a from %s to %s!" % (clan_member_a.clan.colour,defectClan.colour);
        
        defectClan.add_member(clan_member_a);
      elif(random.randint(0,15) == 1):
        #improve level of one of the badges as they improve their 'loyalty'
        winner = random.choice([clan_member_a,clan_member_b])
        winner.clan_level = winner.clan_level + diff
              
     
    
    elif(clan_member_a.clan_level > clan_member_b.clan_level):
      #Player B moves over to clan from A, A loses level
      winner = clan_member_a
      winner.clan_level -=diff
      loser = clan_member_b
      #print "A Won! (%s)" % diff
    elif(clan_member_b.clan_level > clan_member_a.clan_level):
      #Player A moves over to clan from B, B loses level
      winner = clan_member_b
      winner.clan_level -=diff
      loser = clan_member_b
      
    else:
      #B and A are the same level, random pick winner and loser and move loser to winners team, winner level decreases
      winner = random.choice([clan_member_a, clan_member_b])
      loser = clan_member_b if (clan_member_a == winner) else clan_member_a
      if (winner.clan_level > 1):
        winner.clan_level -=diff
      winner.clan.add_member(loser)

      
    
    
    #print "End:\nStart:\n A-- lvl: %s clan: %s \n B-- lvl: %s clan: %s \n" % (clan_member_a.clan_level,clan_member_a.clan.colour,clan_member_b.clan_level,clan_member_b.clan.colour)
    if(debug == True):
      print "complete:\nStart:\n A-- lvl: %s clan: %s \n B-- lvl: %s clan: %s \n\n" % (winner.clan_level,winner.clan.colour,loser.clan_level,loser.clan.colour)

    #  winner.clan_level = 5
