from __future__ import unicode_literals
from random import randint
from django.db import models
# Create your models here.
class Team(models.Model):
	team_name = models.CharField("Team Name",unique = True,max_length=200)
	team_binary = models.IntegerField("Team LED Int",unique = True)
	def __unicode__(self):
		return u'{0}'.format(self.team_name)

class Challenges(models.Model):
	challenge_name = models.CharField("Challenge Name",unique = True,max_length=200)
	challenge_description = models.TextField("Challenge Description")
	challenge_pass = models.CharField("Challenge Password",unique = True,max_length=200)
	challenge_valid = models.CharField("Challenge Valid Code",default="complete",max_length=200)
	def __unicode__(self):
		return u'{0}'.format(self.challenge_name)



class Badge(models.Model):
	badge_lastseen = models.DateTimeField("Badge Lastseen", null = True)
	badge_id = models.CharField("Badge ID",unique = True,max_length=200)
	badge_challenges = models.ManyToManyField(Challenges, blank=True)
	badge_level = models.IntegerField("Badge Level")
	badge_nick = models.CharField("Badge Alias",unique = False,max_length=200,blank=True, null=True)
	badge_verify = models.CharField("Badge Verify Code",unique = False, null = True,max_length=200)
	badge_salt = models.CharField("Badge Salt",max_length=200)
	badge_team = models.ForeignKey(Team, db_index=True,related_name="team")
	badge_status = models.CharField("Badge Status",max_length=200,default="noob")
	badge_badgesSeen = models.ManyToManyField("Badge", blank=True)
	def normaliseLevel(self):
		if(self.badge_level <= 0):
			self.badge_level = 1
		if(self.badge_level >= 6):
			self.badge_level = 5
	def __unicode__(self):
		return u'Badge [ id: %s , challenges: %s , status: %s, level: %s , nick: %s , salt: %s , team: %s ]' % (self.badge_id,self.badge_challenges,self.badge_status,self.badge_level,self.badge_nick,self.badge_salt,self.badge_team)

class Log(models.Model):
	log_timestamp = models.DateTimeField("Log TimeStamp", auto_now_add=True)
	log_badgeOne = models.ForeignKey(Badge,related_name="badgeOne")
	log_badgeTwo = models.ForeignKey(Badge,null = True,related_name="badgeTwo")
	log_type = models.CharField("Log Type", unique = False, max_length=200)
	log_description = models.CharField("Description",unique = False, max_length = 500)
	def __unicode__(self):
		return u'Log [ %s ___ %s]' % ( self.log_timestamp, self.log_description )
		#return u'Log [ %s %s -- (%s && %s) : %s]' % (self.log_timestamp,self.log_type,self.log_badgeOne,self.log_badgeTwo,self.log_description)
