from django.shortcuts import render
from django.views.generic import TemplateView
from django.views.generic.edit import CreateView, UpdateView, DeleteView,FormView
# Create your views here.
from badge.models import *

from django.conf import settings
from django.http import HttpResponse
from django.http import JsonResponse
from badge.forms import AliasForm, ChallengeForm
import json
import random
from django.contrib import messages 
from django.contrib.messages.views import SuccessMessageMixin
from badge.models import Badge, Challenges

import base64
from Crypto.Cipher import AES
from Crypto import Random

BS = 16
pad = lambda s: s + (BS - len(s) % BS) * chr(BS - len(s) % BS) 
unpad = lambda s : s[:-ord(s[len(s)-1:])]

class AESCipher:
	def __init__( self, key ):
		#key = base64.b64encode(key)
		if(len(key) > 32):
			key = key[:32]
		else:
			key = key.zfill(32)
		
		self.key = key

	
	
	def encrypt( self, raw ):
		raw = pad(raw)
		iv = Random.new().read( AES.block_size )
		cipher = AES.new( self.key, AES.MODE_CBC, iv )
		return base64.b64encode( iv + cipher.encrypt( raw ) ) 
	
	def decrypt( self, enc ):
		enc = base64.b64decode(enc)
		iv = enc[:16]
		cipher = AES.new(self.key, AES.MODE_CBC, iv )
		return unpad(cipher.decrypt( enc[16:] ))


class badgeAddChallenge(FormView):
	template_name = "addChallenge.html"
	form_class = ChallengeForm
	model = Challenges
	
	
	
	def get_context_data(self, **kwargs):
		context = super(badgeAddChallenge, self).get_context_data(**kwargs)
		context["Challenges"] = Challenges.objects.all()
		return context
	
	def get(self, request, *args, **kwargs):
		context = self.get_context_data(**kwargs)
		badgeNum = self.request.GET.get("badgeNum")
		results = "";
		if(badgeNum):
			allChallenges = Challenges.objects.all()
			for challenge in allChallenges:
				challenge_pw = challenge.challenge_pass
				key = badgeNum + challenge_pw
				result = challenge.challenge_valid
				t = AESCipher(key)
				encryptedString = t.encrypt(result)
				winString = "Challenge %s: Encrypted '%s' with '%s' + '%s' to get '%s'.<br/>" % (challenge.challenge_name,result, badgeNum,challenge_pw, encryptedString)
				results  = results + winString
			return JsonResponse({'result':results})
		
		return super(badgeAddChallenge, self).render_to_response(context)
	def post(self, request, *args, **kwargs):
		context = self.get_context_data(**kwargs)
		badgeNum = self.request.POST.get("badge_number")
		badgeHash = self.request.POST.get("challenge_hash")
		
		
		
		thisBadge = False
		try:
			thisBadge = Badge.objects.get(badge_id = badgeNum)
			if settings.DEBUG:
				print "[+] Badge %s Found!" % (badgeNum)
		except Badge.DoesNotExist:
			if settings.DEBUG:
				print "[!] Could not find badge for adding challenge %s -- exiting!" % (badgeNum)
				
		if(thisBadge != False):
			allChallenges = Challenges.objects.all()
			for challenge in allChallenges:
				challenge_pw = challenge.challenge_pass
				key = badgeNum + challenge_pw
				t = AESCipher(key)
				decrypted = False
				try:
					decrypted = t.decrypt(badgeHash)
					print decrypted
				except Exception, e:
					messages.success(self.request, "Invalid decryption: %s" % (e))
					
				if(decrypted == challenge.challenge_valid):
					messages.success(self.request, "Successfully updated badge '%s' and added challenge '%s'" % (badgeNum,challenge.challenge_name))
					thisBadge.badge_challenges.add(challenge)
				else:
					messages.error(self.request,"Decoded string '%s' for challenge %s to did not produce correct key" % (badgeHash,challenge.challenge_name))
			#thisBadge.badge_nick = badgeAlias
			#thisBadge.save()
			#messages.success(self.request, "Successfully updated badge '%s' to have alias '%s'" % (badgeNum,badgeAlias))
		else:
			messages.warning(self.request, "Could not find badge with ID of '%s'! Please check and try again" % (badgeNum))
		
		return super(badgeAddChallenge, self).render_to_response(context)

class badgeAddAlias(FormView):
	template_name = "addAlias.html"
	form_class = AliasForm
	model = Badge
	
	def post(self, request, *args, **kwargs):
		context = self.get_context_data(**kwargs)
		badgeNum = self.request.POST.get("badge_number")
		badgeAlias = self.request.POST.get("badge_alias")
		badgeVerify = self.request.POST.get("badge_verify")
		thisBadge = False
		try:
			thisBadge = Badge.objects.get(badge_id = badgeNum,badge_verify = badgeVerify)
			if settings.DEBUG:
				print "[+] Badge %s Found!" % (badgeNum)
		except Badge.DoesNotExist:
			if settings.DEBUG:
				print "[!] Could not find badge %s -- exiting!" % (badgeNum)
				
		if(thisBadge != False):
			thisBadge.badge_nick = badgeAlias
			thisBadge.save()
			messages.success(self.request, "Successfully updated badge '%s' to have alias '%s'" % (badgeNum,badgeAlias))
		else:
			messages.warning(self.request, "Could not find badge with ID of '%s' and a Verification code of '%s' please check and try again" % (badgeNum,badgeVerify))
		
		return super(badgeAddAlias, self).render_to_response(context)
		#return HttpResponse(context)

class badgeGetHash(TemplateView):
	template_name = "checkin.enc"
	def get(self, request, *args, **kwargs):
		context = self.get_context_data(**kwargs)
		badgeID = context["badgeID"]
		#return super(RunnerProcess, self).render_to_response(context)
		try:
			thisBadge = Badge.objects.get(badge_id = badgeID)
			if settings.DEBUG:
				print "[+] Badge %s Found!" % (badgeID)
		except Badge.DoesNotExist:
			if settings.DEBUG:
				print "[!] Could not find badge %s -- creating" % (badgeID)
			thisBadge = Badge();
			badgeTeam = randint(1,3);
			if settings.DEBUG:
				print "[!] Random team picked as %s / 3 " % badgeTeam
			thisBadge = Badge.objects.create(badge_id = badgeID,badge_level = 1,badge_nick = None,badge_salt = "Andrew",badge_team_id = badgeTeam)
		
		return HttpResponse(thisBadge.badge_salt)

	
	
class badgeCheckin(TemplateView):
	def badgeFight(self,b1,b2):
		
		#not the same -- this shouldnt happen but whatever
		if(b1 == b2):
			return False
		
		
		
		diff = 1
		print "\n\nB1(%s)*%s*[%s] vs B2(%s)*%s*[%s]" % (b1.badge_id,b1.badge_team,b1.badge_level,b2.badge_id,b2.badge_team,b2.badge_level)
		
		if(b1.badge_team == b2.badge_team):
			if settings.DEBUG:
				print "[!] Same team for fight! "
			if(random.randint(0,(b1.badge_level * 10 )) == 1):
				currentTeam = b1.badge_team
				newTeam = Team.objects.get(pk=randint(1,3))
				b1.badge_team = newTeam
				if(b1.badge_level > 1):
					b1.badge_level = b1.badge_level - 1
				if settings.DEBUG:
					print "Both (%s) and (%s) the same,Defecting player A(%s) from %s to %s!" % (b1.badge_id,b2.badge_id,b1.badge_id,currentTeam,newTeam);
				b1.save()
				
			elif(random.randint(0,15) == 1):
				winner = random.choice([b1, b2])
				winner.badge_level = winner.badge_level + diff
				if settings.DEBUG:
					print "Both (%s) and (%s) the same, improving %s!" % (b1.badge_id,b2.badge_id,winner.badge_id);
				winner.save()
			else:
				if settings.DEBUG:
					print "Both (%s) and (%s) the same, no one defected or improved!" % (b1.badge_id,b2.badge_id)
		
		elif(b1.badge_level > b2.badge_level):
			if settings.DEBUG:
				print "(%s) has a higher level than (%s) -- (%s) Wins!" % (b1.badge_id,b2.badge_id,b1.badge_id)	
			b2.badge_team = b1.badge_team
			b1.badge_level = b1.badge_level - diff
			b1.normaliseLevel()
			b2.save()
			b1.save()
			
		elif(b1.badge_level < b2.badge_level):
			if settings.DEBUG:
				print "(%s) has a higher level than (%s) -- (%s) Wins!" % (b2.badge_id,b1.badge_id,b2.badge_id)	
			b1.badge_team = b2.badge_team
			b2.badge_level = b2.badge_level - diff
			b2.normaliseLevel()
			b2.save()
			b1.save()
		else:
			#diff teams, same level
			loser = random.choice([b1, b2])
			winner = b1 if loser == b2 else b2
			if(winner.badge_level > 1):
				winner.badge_level = winner.badge_level - diff;
			loser.badge_team = winner.badge_team
			if settings.DEBUG:
				print "Both (%s) and (%s) the same, %s converts %s!" % (b1.badge_id,b2.badge_id,winner.badge_id,loser.badge_id);
			winner.save()
				
		print "\n\nB1(%s)*%s*[%s] vs B2(%s)*%s*[%s]" % (b1.badge_id,b1.badge_team,b1.badge_level,b2.badge_id,b2.badge_team,b2.badge_level)

	template_name = "checkin.enc"
	def post(self, request, *args, **kwargs):
		context = self.get_context_data(**kwargs)
		#context = {}
		
		seenBadges = json.loads(self.request.POST.get("seen"))
		for badgeName in seenBadges:
			try:
				seenBadge = Badge.objects.get(badge_id = badgeName)
				self.badgeFight(context["currentBadge"],seenBadge)
				if settings.DEBUG:
					print "[+] Badge %s Found!" % (badgeName)
			except Badge.DoesNotExist:
				if settings.DEBUG:
					print "[!] Error! %s has not been seen before! Not saving!" % (badgeName)

		
		#return super(RunnerProcess, self).render_to_response(context)
		return HttpResponse(context["jsonResponse"])
	
	def get(self, request, *args, **kwargs):
		context = self.get_context_data(**kwargs)
		#context = {}
		#context["jsonResponse"] = "asd"
		
		#return super(RunnerProcess, self).render_to_response(context)
		return HttpResponse(context["jsonResponse"])


	def cryptMessage(self,message,key):
		encodedMessage = ""
		shiftLen = len(key)
		#print "!!!%s!!" % (message)
		for x in message:
			encodedMessage = encodedMessage +  chr(ord(x) - shiftLen)
		return encodedMessage

	def get_context_data(self, **kwargs):
		context = super(badgeCheckin, self).get_context_data(**kwargs)
		context["jsonResponse"] = "asd"
		
		
		badgeID = context["badgeID"]
		thisBadge = False
		
		try:
			thisBadge = Badge.objects.get(badge_id = badgeID)
			if settings.DEBUG:
				print "[+] Badge %s Found!" % (badgeID)
		except Badge.DoesNotExist:
			if settings.DEBUG:
				print "[!] Could not find badge %s -- creating" % (badgeID)
			
			blueBadges = Badge.objects.filter(badge_team=Team.objects.get(team_name='blueTeam'))
			greenBadges = Badge.objects.filter(badge_team=Team.objects.get(team_name='greenTeam'))
			redBadges = Badge.objects.filter(badge_team=Team.objects.get(team_name='redTeam'))
			
			badgeList = [blueBadges,greenBadges,redBadges]
			mostMembers = max(badgeList, key=len)
			selectList = [Team.objects.get(team_name='greenTeam'),Team.objects.get(team_name='redTeam'),Team.objects.get(team_name='blueTeam')]
			if(len(mostMembers) > 1):
				if (mostMembers[0].badge_team.team_name == "blueTeam"):
					selectList = [Team.objects.get(team_name='greenTeam'),Team.objects.get(team_name='redTeam')]
				if (mostMembers[0].badge_team.team_name == "greenTeam"):
					selectList = [Team.objects.get(team_name='blueTeam'),Team.objects.get(team_name='redTeam')]
				else:
					selectList = [Team.objects.get(team_name='greenTeam'),Team.objects.get(team_name='blueTeam')]
				
			thisBadge = Badge.objects.create(badge_id = badgeID,badge_level = 1,badge_nick = None,badge_salt = "Andrew",badge_team = random.choice(selectList))
			thisBadge.save()
			#thisBadge = Badge.create(badgeID)
			#thisBadge = Badge().createBadge(badgeID)
			#thisBadge = Badge.createBadge(badgeID);
		
		print thisBadge
		
		teamInt = thisBadge.badge_team.team_binary
		
		lvl = pow(2,(thisBadge.badge_level)) -1
		shiftVal = teamInt + lvl
		#{"shift":254,"status":"noob","challenges":[1,3,5,7,2]}
		jsonResponse = {}
		jsonResponse["shift"] = shiftVal
		jsonResponse["status"] = thisBadge.badge_status

		
		if(thisBadge.badge_challenges == None):
			jsonResponse["challenges"] = list([])
		else:
			jsonResponse["challenges"] = list(thisBadge.badge_challenges.all())
		
		unencrypted = json.dumps(jsonResponse)
		crypted = self.cryptMessage(unencrypted,thisBadge.badge_salt)
		context["jsonResponse"] = crypted
		
		
		thisBadge.badge_salt = random.choice(['andrew', 'godie', 'lolcakes', 'noyou', 'bitches', 'fire'])
		thisBadge.save()
		
		context["currentBadge"] = thisBadge
		return context
