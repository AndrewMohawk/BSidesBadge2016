from django.shortcuts import render
from django.views.generic import TemplateView
# Create your views here.
from badge.models import *
from django.conf import settings
from django.http import HttpResponse
from django.http import JsonResponse
import json

class badgeGetHash(TemplateView):
	#no
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
			Badge.createBadge(thisBadge,badgeID);
		
		return HttpResponse(thisBadge.badge_salt)
	
	
class badgeCheckin(TemplateView):

	template_name = "checkin.enc"
	def get(self, request, *args, **kwargs):
		context = self.get_context_data(**kwargs)
		
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
		badgeID = context["badgeID"]
		thisBadge = False
		
		try:
			thisBadge = Badge.objects.get(badge_id = badgeID)
			if settings.DEBUG:
				print "[+] Badge %s Found!" % (badgeID)
		except Badge.DoesNotExist:
			if settings.DEBUG:
				print "[!] Could not find badge %s -- creating" % (badgeID)
			thisBadge = Badge();
			Badge.createBadge(thisBadge,badgeID);
		
		#print thisBadge
		teamInt = thisBadge.badge_team.team_binary
		lvl = pow(2,(thisBadge.badge_level)) -1
		shiftVal = teamInt + lvl
		#{"shift":254,"status":"noob","challenges":[1,3,5,7,2]}
		jsonResponse = {}
		jsonResponse["shift"] = shiftVal
		#jsonResponse["shift"] = 250
		jsonResponse["status"] = thisBadge.badge_status
		if(thisBadge.badge_challenges == None):
			jsonResponse["challenges"] = []
		else:
			jsonResponse["challenges"] = thisBadge.badge_challenges
		
		
		unencrypted = json.dumps(jsonResponse)
		crypted = self.cryptMessage(unencrypted,thisBadge.badge_salt)
		context["jsonResponse"] = crypted
		#context["jsonResponse"] = '[\]^_`'
		#context["jsonResponse"] = JsonResponse(jsonResponse)
		print jsonResponse
		
		
		#lvlStr = "{:05b}".format(lvl)
		#print shiftBin
		
		return context
