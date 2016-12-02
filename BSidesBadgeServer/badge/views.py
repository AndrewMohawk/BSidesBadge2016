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
from django.utils.crypto import get_random_string
from datetime import datetime
from django.forms.models import model_to_dict
from django.db.models.fields.related import ManyToManyField

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


def cryptMessage(message,key):
		encodedMessage = ""
		shiftLen = len(key)
		#print "!!!%s!!" % (message)
		for x in message:
			encodedMessage = encodedMessage +  chr(ord(x) - shiftLen)
		return encodedMessage	




class RPSSL_api(TemplateView):
	template_name = "api_badgeDetails"
	

	#def get(self, request, *args, **kwargs):
	#	return self.post(request, args, kwargs)
		
	def post(self, request, *args, **kwargs):
		context = self.get_context_data(**kwargs)
		badgeID = context["badgeID"]
		challengerBadgeID = context["challengerBadge"]
		selection = int(context["selection"])
		try:
			
			thisBadge = Badge.objects.get(badge_id = badgeID)
			if(thisBadge):
				print thisBadge
			challengerBadge = Badge.objects.get(badge_id = challengerBadgeID)
			
			thisGame = None
			#if settings.DEBUG:
				#print "[+] Badge %s Found!" % (thisBadge)
				#print "[+] Challenger Badge %s Found!" % (challengerBadge)
			try:
				P1Game = RPSSL.objects.get(rpssl_badgeOne = thisBadge,rpssl_badgeTwo = challengerBadge,rpssl_badgesConfirmed__lt=2)
				print "[+] We know this game: %s" % (P1Game)
				P1Game.rpssl_badgesConfirmed = 1
				if(P1Game.rpssl_status == 1 or P1Game.rpssl_status == 2):
					P1Game.rpssl_badgesConfirmed = 2
				if(P1Game.rpssl_status == 0):
					#check it hasnt expired
					timediff = datetime.now() - P1Game.rpssl_timestamp
					secondsSince = timediff.seconds
					print "[+] This game started %s seconds ago" % (secondsSince)
					if(secondsSince > 45):
						#this games timed out.
						print "[!!] Timing out game!"
						P1Game.rpssl_status = 3
				P1Game.save()
				thisGame = P1Game
				#return JsonResponse(model_to_dict(P1Game))
				
			except (RPSSL.DoesNotExist):
				try:
					P2Game = RPSSL.objects.get(rpssl_badgeTwo = thisBadge,rpssl_badgeOne = challengerBadge,rpssl_badgesConfirmed__lt=2)
					P2Game.rpssl_pick2 = selection
					
					print "[+] Complete this game: %s" % (P2Game)
					
					'''
					0: rock
					1: paper
					2: scissors
					3: spock
					4: lizard
					'''
					p1 = P2Game.rpssl_pick1
					p2 = P2Game.rpssl_pick2
					
					if(p1 == 0): #rock
						if(p2 == 2 or p2 == 4): #scissors or lizard
							P2Game.rpssl_status = 1 #p1 wins
						elif(p2 == 0):
							P2Game.rpssl_status = 3 #draw
						else:
							P2Game.rpssl_status = 2 #p2 wins
							
					elif(p1 == 1): #paper
						if(p2 == 0 or p2 == 3): #rock or spock
							P2Game.rpssl_status = 1 #p1 wins
						elif(p2 == 1):
							P2Game.rpssl_status = 3 #draw
						else:
							P2Game.rpssl_status = 2 #p2 wins
					
					elif(p1 == 2): #scissors
						if(p2 == 1 or p2 == 4): #lizard or paper
							P2Game.rpssl_status = 1 #p1 wins
						elif(p2 == 2):
							P2Game.rpssl_status = 3 #draw
						else:
							P2Game.rpssl_status = 2 #p2 wins
					
					elif(p1 == 3): #spock
						if(p2 == 0 or p2 == 2): #rock or scissors
							P2Game.rpssl_status = 1 #p1 wins
						elif(p2 == 3):
							P2Game.rpssl_status = 3 #draw	
						else:
							P2Game.rpssl_status = 2 #p2 wins
					
					elif(p1 == 4): #lizard
						if(p2 == 1 or p2 == 3): #paper or spock
							P2Game.rpssl_status = 1 #p1 wins
						elif(p2 == 4):
							P2Game.rpssl_status = 3 #draw
						else:
							P2Game.rpssl_status = 2 #p2 wins
					
					#rpssl_badgesConfirmed = 2;
					P2Game.save();
					thisGame = P2Game
					#return JsonResponse(model_to_dict(P2Game))
				except (RPSSL.DoesNotExist):
					
					x = RPSSL.objects.create(rpssl_badgeOne = thisBadge,rpssl_badgeTwo = challengerBadge,rpssl_status = 0,rpssl_pick1 = selection,rpssl_badgesConfirmed = 1)
					print "[+] Created this game: %s" % (x)
					#return JsonResponse(model_to_dict(x))
					thisGame = x
			
		except Badge.DoesNotExist:
			if settings.DEBUG:
				print "[!] Could not find badge %s AND/OR %s!" % (badgeID, challengerBadgeID)
		
		##lets do the return
		try:
			jsonObj = {}
			if(thisGame.rpssl_status == 0):
				jsonObj["statusMSG"] = "Awaiting Results...";
				jsonObj["state"] = 0
			if(thisGame.rpssl_status == 1 ):
				jsonObj["state"] = 1
				if(thisBadge == thisGame.rpssl_badgeOne):
					jsonObj["status1"] = "YOU WIN"
					jsonObj["status2"] = thisGame.rpssl_badgeOne.badge_nick
					jsonObj["status3"] = "LOSES!"
				else:
					jsonObj["status1"] = "YOU LOSE"
					jsonObj["status2"] = thisGame.rpssl_badgeTwo.badge_nick
					jsonObj["status3"] = "WINS!"
			if(thisGame.rpssl_status == 2 ):
				jsonObj["state"] = 1
				if(thisBadge == thisGame.rpssl_badgeTwo):
					jsonObj["status1"] = "YOU WIN"
					jsonObj["status2"] = thisGame.rpssl_badgeOne.badge_nick
					jsonObj["status3"] = "LOSES!"
				else:
					jsonObj["status1"] = "YOU LOSE"
					jsonObj["status2"] = thisGame.rpssl_badgeTwo.badge_nick
					jsonObj["status3"] = "WINS!"
			if(thisGame.rpssl_status == 3 ):
				jsonObj["state"] = 1
				if(thisBadge == thisGame.rpssl_badgeTwo):
					jsonObj["status1"] = "YOU DRAW WITH"
					jsonObj["status2"] = thisGame.rpssl_badgeOne.badge_nick
					jsonObj["status3"] = ""
				else:
					jsonObj["status1"] = "YOU DRAW WITH"
					jsonObj["status2"] = thisGame.rpssl_badgeTwo.badge_nick
					jsonObj["status3"] = ""
					
			if(thisGame.rpssl_status == 4 ):
				jsonObj["state"] = 2
				jsonObj["status1"] = ":( Timed out."
			
			unencrypted = json.dumps(jsonObj)
			#print unencrypted
			crypted = cryptMessage(unencrypted,thisBadge.badge_salt)
			
			
			return HttpResponse(crypted)
		except Exception,e:
			unencrypted = json.dumps({"state":3,"status1":"Server Error"})
			crypted = cryptMessage(unencrypted,thisBadge.badge_salt)
			return HttpResponse(crypted)

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
					
				except Exception, e:
					messages.success(self.request, "Invalid decryption: %s" % (e))
				
				
				if(decrypted == challenge.challenge_valid):
				
					messages.success(self.request, "Successfully updated badge '%s' and added challenge '%s'" % (badgeNum,challenge.challenge_name))
					thisBadge.badge_challenges.add(challenge)
				else:
					messages.error(self.request,"Decoded string '%s' for challenge %s to did not produce correct key" % (badgeHash,challenge.challenge_name))
			
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
			randomCode = get_random_string(length=6)
			thisBadge.badge_verify = randomCode
			thisBadge.badge_challenges.add(Challenges.objects.get(challenge_name="Alias"))
			thisBadge.save()
			messages.success(self.request, "Successfully updated badge '%s' to have alias '%s'" % (badgeNum,badgeAlias))
		else:
			messages.warning(self.request, "Could not find badge with ID of '%s' and a Verification code of '%s' please check and try again" % (badgeNum,badgeVerify))
		
		return super(badgeAddAlias, self).render_to_response(context)
		#return HttpResponse(context)




class getBadgeDetails(TemplateView):
	template_name = "api_badgeDetails"
	
	

	def to_dict(self,instance):
		opts = instance._meta
		data = {}
		for f in opts.concrete_fields + opts.many_to_many:
			if isinstance(f, ManyToManyField):
				print "Found Many to Many %s" % f
				if instance.pk is None:
					data[f.name] = []
				else:
					
					if(str(f) == "badge.Badge.badge_badgesSeen"):
						#print f.value_from_object(instance).values_list('badge_id', flat=True)
						data[f.name] = list(f.value_from_object(instance).values_list('badge_id', flat=True))
					#elif(str(f) == "badge.Badge.badge_challenges"):
					#	data[f.name] = list(f.value_from_object(instance).values_list('challenge_id', flat=True))
					else:
						#print "no.";
						data[f.name] = list(f.value_from_object(instance).values_list('pk', flat=True))
			else:
				data[f.name] = f.value_from_object(instance)
		return data
	
	
	def get(self, request, *args, **kwargs):
		context = self.get_context_data(**kwargs)
		badgeID = context["badgeID"]
		
		try:
			thisBadge = Badge.objects.get(badge_id = badgeID)		
			if settings.DEBUG:
				print "[+] Badge %s Found!" % (badgeID)
			#data = serializers.serialize("json", [thisBadge,])
			#context["json"] = data
			
			
			my_dict = self.to_dict(thisBadge)
			print my_dict
			
		except Badge.DoesNotExist:
			if settings.DEBUG:
				print "[!] Could not find badge %s -- not giving hash!" % (badgeID)
			return HttpResponse("noyouarenotarealbadge")
		
		return JsonResponse(  my_dict )


class saveCurrentTeamStandings(TemplateView):
	def get(self, request, *args, **kwargs):
		template_name = "api_badgeDetails"
		blueBadges = Badge.objects.filter(badge_team=Team.objects.get(team_name='blue'))
		greenBadges = Badge.objects.filter(badge_team=Team.objects.get(team_name='green'))
		redBadges = Badge.objects.filter(badge_team=Team.objects.get(team_name='red'))

		currentStatus = gameStatus.objects.create(gamestatus_timestamp = datetime.now(),gamestatus_red = len(redBadges),gamestatus_blue = len(blueBadges),gamestatus_green = len(greenBadges));
		print currentStatus;
		currentStatus.save();
		return JsonResponse(  {"status" :"saved"} )

class getAllBadges(TemplateView):
	template_name = "api_badgeDetails"
	
	def to_dict(self,instance):
		opts = instance._meta
		data = {}
		for f in opts.concrete_fields + opts.many_to_many:
			if isinstance(f, ManyToManyField):
				print "Found Many to Many %s" % f
				if instance.pk is None:
					data[f.name] = []
				else:
					
					if(str(f) == "badge.Badge.badge_badgesSeen"):
						#print f.value_from_object(instance).values_list('badge_id', flat=True)
						data[f.name] = list(f.value_from_object(instance).values_list('badge_id', flat=True))
					#elif(str(f) == "badge.Badge.badge_challenges"):
					#	data[f.name] = list(f.value_from_object(instance).values_list('challenge_id', flat=True))
					else:
						#print "no.";
						data[f.name] = list(f.value_from_object(instance).values_list('pk', flat=True))
			else:
				data[f.name] = f.value_from_object(instance)
		return data
	
	def get(self, request, *args, **kwargs):
		context = self.get_context_data(**kwargs)
		badges = Badge.objects.all()
		badgeDict = []
		for badge in badges:
			#badgeDict.append({"tesT":"test"})
			badgeDict.append(self.to_dict(badge))
		#data = serializers.serialize("json", Badge.objects.all())
		
		return JsonResponse(  badgeDict, safe=False )

class badgeGetHash(TemplateView):
	template_name = "checkin.enc"
	def get(self, request, *args, **kwargs):
		context = self.get_context_data(**kwargs)
		badgeID = context["badgeID"]
		
		try:
			thisBadge = Badge.objects.get(badge_id = badgeID)
			thisBadge.badge_lastseen = datetime.now()
			thisBadge.save()
			
			if settings.DEBUG:
				print "[+] Badge %s Found!" % (badgeID)
			
			logDescrip = "%s ( %s ) fetched hash." % (thisBadge.badge_id, thisBadge.badge_nick);
			logEntry = Log(log_timestamp = datetime.now(),log_badgeOne = thisBadge,log_type="FetchHash",log_description = logDescrip)
			#logEntry.save()
		except Badge.DoesNotExist:
			if settings.DEBUG:
				print "[!] Could not find badge %s -- not giving hash!" % (badgeID)
			return HttpResponse("noyouarenotarealbadge")
		
		return HttpResponse(thisBadge.badge_salt)

	
	
class badgeCheckin(TemplateView):
	def badgeFight(self,b1,b2):
		
		#not the same -- this shouldnt happen but whatever
		if(b1 == b2):
			return False
		logDescrip = "";
		logType = "Fight"
		
		
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
				logDescrip = "%s (%s) and %s (%s) are on the same team. defecting player A(%s) from %s to %s!" % (b1.badge_id,b1.badge_nick,b2.badge_id,b2.badge_nick,b1.badge_id,currentTeam,newTeam);
				logType = "Fight/Defect";
				b1.save()
				
			elif(random.randint(1,15) == 1):
				winner = random.choice([b1, b2])
				winner.badge_level = winner.badge_level + diff
				if settings.DEBUG:
					print "Both (%s) and (%s) the same, improving %s!" % (b1.badge_id,b2.badge_id,winner.badge_id);
				if(b1.badge_nick == None):
					b1.badge_nick = "";
				if(b2.badge_nick == None):
					b2.badge_nick = "";
				logDescrip = "%s (%s) and %s (%s) are on the same team. improving %s (%s)!" % (b1.badge_id,b1.badge_nick,b2.badge_id,b2.badge_nick,winner.badge_id,winner.badge_nick);
				logType = "Fight/Improve";
				if(winner.badge_level > 5):
					winner.badge_level = 5;
					if settings.DEBUG:
						print "Oops! %s (%s) already at maximum level!" % (winner.badge_id,winner.badge_nick);
				else:
					winner.save()
			else:
				if settings.DEBUG:
					print "Both (%s) and (%s) the same, no one defected or improved!" % (b1.badge_id,b2.badge_id)
		
		elif(b1.badge_level > b2.badge_level):
			if settings.DEBUG:
				print "(%s) has a higher level than (%s) -- (%s) Wins!" % (b1.badge_id,b2.badge_id,b1.badge_id)
			
			logDescrip = "%s (%s) has a higher level than %s (%s) -- %s (%s) wins!" % (b1.badge_id,b1.badge_nick,b2.badge_id,b2.badge_nick,b1.badge_id,b1.badge_nick);
			logType = "Fight/Convert";
			
			
			b2.badge_team = b1.badge_team
			b1.badge_level = b1.badge_level - diff
			b1.normaliseLevel()
			b2.save()
			b1.save()
			
		elif(b1.badge_level < b2.badge_level):
			if settings.DEBUG:
				print "(%s) has a higher level than (%s) -- (%s) Wins!" % (b2.badge_id,b1.badge_id,b2.badge_id)
			
			logDescrip = "%s (%s) has a higher level than %s (%s) -- %s (%s) wins!" % (b2.badge_id,b2.badge_nick,b1.badge_id,b1.badge_nick,b2.badge_id,b2.badge_nick);
			logType = "Fight/Convert";
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
				
			logDescrip = "%s (%s) and %s (%s) are the same -- %s (%s) converts %s (%s) wins!" % (b2.badge_id,b2.badge_nick,b1.badge_id,b1.badge_nick,winner.badge_id,winner.badge_nick,loser.badge_id,loser.badge_nick);
			logType = "Fight/Convert";
			
			loser.save()
				
		print "\n\nB1(%s)*%s*[%s] vs B2(%s)*%s*[%s]" % (b1.badge_id,b1.badge_team,b1.badge_level,b2.badge_id,b2.badge_team,b2.badge_level)
		
		logEntry = Log(log_timestamp = datetime.now(),log_badgeOne = b1,log_badgeTwo=b2,log_type="Fight",log_description = logDescrip)
		#logEntry.save()

	template_name = "checkin.enc"
	def post(self, request, *args, **kwargs):
		context = self.get_context_data(**kwargs)
		#context = {}
		
		seenBadges = json.loads(self.request.POST.get("seen"))
		
		thisBadge = context["currentBadge"]
		logDescrip = "%s ( %s ) checked in via POST and has seen these badges: %s." % (thisBadge.badge_id, thisBadge.badge_nick, ', '.join(seenBadges));
		logEntry = Log(log_timestamp = datetime.now(),log_badgeOne = thisBadge,log_type="Checkin",log_description = logDescrip)
		#logEntry.save()
		
		
		for badgeName in seenBadges:
			try:
				seenBadge = Badge.objects.get(badge_id = badgeName)
				self.badgeFight(context["currentBadge"],seenBadge)
				if settings.DEBUG:
					print "[+] Badge %s found in seen list!" % (badgeName)
					thisBadge.badge_badgesSeen.add(seenBadge)
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
		thisBadge = context["currentBadge"]
		logDescrip = "%s ( %s ) checked in via GET." % (thisBadge.badge_id, thisBadge.badge_nick);
		logEntry = Log(log_timestamp = datetime.now(),log_badgeOne = thisBadge,log_type="Checkin",log_description = logDescrip)
		#logEntry.save()
		return HttpResponse(context["jsonResponse"])


	

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
			
			blueBadges = Badge.objects.filter(badge_team=Team.objects.get(team_name='blue'))
			greenBadges = Badge.objects.filter(badge_team=Team.objects.get(team_name='green'))
			redBadges = Badge.objects.filter(badge_team=Team.objects.get(team_name='red'))
			
			badgeList = [blueBadges,greenBadges,redBadges]
			mostMembers = max(badgeList, key=len)
			selectList = [Team.objects.get(team_name='green'),Team.objects.get(team_name='red'),Team.objects.get(team_name='blue')]
			if(len(mostMembers) > 1):
				if (mostMembers[0].badge_team.team_name == "blue"):
					selectList = [Team.objects.get(team_name='green'),Team.objects.get(team_name='red')]
				if (mostMembers[0].badge_team.team_name == "green"):
					selectList = [Team.objects.get(team_name='blue'),Team.objects.get(team_name='red')]
				else:
					selectList = [Team.objects.get(team_name='green'),Team.objects.get(team_name='blue')]
			randomCode = get_random_string(length=6)
			thisBadge = Badge.objects.create(badge_id = badgeID,badge_level = 1,badge_nick = badgeID,badge_salt = "Andrew",badge_team = random.choice(selectList),badge_verify = randomCode,badge_message="WELCOME TO         BSIDES!")
			thisBadge.save()

		
		print thisBadge
		
		teamInt = thisBadge.badge_team.team_binary
		
		#lvl = pow(2,(thisBadge.badge_level)) -1
		#because the shift register is wired diff:
		lvl = 2; # level 1;
		if(thisBadge.badge_level == 2):
			lvl = 6;
		if(thisBadge.badge_level == 3):
			lvl = 14;
		if(thisBadge.badge_level == 4):
			lvl = 30;
		if(thisBadge.badge_level == 5):
			lvl = 31;
		
		shiftVal = teamInt + lvl
		
		jsonResponse = {}
		
		jsonResponse["shift"] = shiftVal
		jsonResponse["status"] = thisBadge.badge_status
		jsonResponse["message"] = thisBadge.badge_message
		if(thisBadge.badge_message != ""):
			thisBadge.badge_message = ""
			thisBadge.save()
		jsonResponse["level"] = thisBadge.badge_level
		jsonResponse["team"] = thisBadge.badge_team.team_name
		jsonResponse["verify"] = thisBadge.badge_verify
		
		if(thisBadge.badge_nick == None):
			jsonResponse["alias"] = "-_-"
		else:
			jsonResponse["alias"] = thisBadge.badge_nick
		
		
		if(thisBadge.badge_challenges == None):
			jsonResponse["challenges"] = list([])
		else:
			jsonResponse["challenges"] = map( str, thisBadge.badge_challenges.all())
		
		unencrypted = json.dumps(jsonResponse)
		#print unencrypted
		crypted = cryptMessage(unencrypted,thisBadge.badge_salt)
		context["jsonResponse"] = crypted
		
		
		thisBadge.badge_salt = random.choice(['andrew', 'godie', 'lolcakes', 'noyou', 'bitches', 'fire'])
		thisBadge.save()
		
		context["currentBadge"] = thisBadge
		return context
