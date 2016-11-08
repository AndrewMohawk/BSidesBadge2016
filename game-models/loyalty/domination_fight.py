import random

from fight import Fight

class DominationFight(Fight):
  def fight(self, clan_member_a, clan_member_b):
    if clan_member_a == clan_member_b:
      return

    winner = random.choice([clan_member_a, clan_member_b])
    loser = clan_member_b if (clan_member_a == winner) else clan_member_a

    winner.clan_level += loser.clan_level
    loser.clan_level -= loser.clan_level

    if loser.clan_level <= 0:
      winner.clan.add_member(loser)

    if winner.clan_level > 5:
      winner.clan_level = 5

