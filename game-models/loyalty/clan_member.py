class ClanMember:
  def __init__(self, name, clan, clan_level = 0):
    self.name = name
    self.clan = None

    clan.add_member(self)

    self.clan = clan
    self.clan_level = clan_level


  def __repr__(clan_member):
    return "ClanMember: clan={0},name={1},level={2}".format(clan_member.clan.colour, clan_member.name, clan_member.clan_level)

  def __str__():
    return "ClanMember: clan={0},name={1},level={2}".format(clan_member.clan.colour, clan_member.name, clan_member.clan_level)

