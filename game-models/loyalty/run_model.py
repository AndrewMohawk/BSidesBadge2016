import random
import sys
from clan import Clan
from clan_member import ClanMember
from fight import Fight
from domination_fight import DominationFight

clan_red = Clan("red")
clan_green = Clan("green")
clan_blue = Clan("blue")

clans = [ clan_red, clan_green, clan_blue ]

num_players = 102
all_players = []

for x in range(0, num_players/3):
  red_clan_member = ClanMember("player_red_" + str(x), clan_red)
  green_clan_member = ClanMember("player_green_" + str(x), clan_green)
  blue_clan_member = ClanMember("player_blue_" + str(x), clan_blue)

  all_players.append(red_clan_member)
  all_players.append(green_clan_member)
  all_players.append(blue_clan_member)

#print clan_red
#print clan_green
#print clan_blue

f=Fight()
df=DominationFight()

print "round,red_count,green_count,blue_count"
for round_count in range(0, 10000):
  for x in range(0, num_players):
    player_a = random.choice(all_players)
    player_b = random.choice(all_players)

    #if round_count % 300:
    f.fight(player_a, player_b,[clan_red,clan_blue,clan_green])
    #else:
    #  df.fight(player_a, player_b)

    if(clan_blue.size() == num_players):
      print "Clan blue won.";
      print str(round_count) + "," + str(clan_red.size()) + "," + str(clan_green.size()) + "," + str(clan_blue.size())
      raw_input("Press Enter to continue...")
    if(clan_red.size() == num_players):
      print "Clan red won.";
      print str(round_count) + "," + str(clan_red.size()) + "," + str(clan_green.size()) + "," + str(clan_blue.size())
      raw_input("Press Enter to continue...")
    if(clan_green.size() == num_players):
      print "Clan green won.";
      print str(round_count) + "," + str(clan_red.size()) + "," + str(clan_green.size()) + "," + str(clan_blue.size())
      raw_input("Press Enter to continue...")
      
  if round_count % 300 == 0:
    print str(round_count) + "," + str(clan_red.size()) + "," + str(clan_green.size()) + "," + str(clan_blue.size())
    #raw_input("Press Enter to continue...")
  # defections
#  defector = random.choice(all_players)
#  defector_new_clan = random.choice(clans)
#  defector_new_clan.add_member(defector)

#print clan_red
#print clan_green
#print clan_blue
