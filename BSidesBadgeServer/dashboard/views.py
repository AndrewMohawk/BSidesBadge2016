from django.shortcuts import render
from django.views.generic import TemplateView
from django.http import HttpResponse, JsonResponse
from badge.models import Badge, Challenges, Team

class badgeDashboard(TemplateView):
	template_name = "index.html"
# Create your views here.


class badgeDashboardAjax(TemplateView):
	
	template_name = "index.html"
	
	def post(self, request, *args, **kwargs):
		try:
			Type = self.request.POST.get('Type')
			valRed = int(self.request.POST.get('Value[red]'))
			valGreen = int(self.request.POST.get('Value[green]'))
			valBlue = int(self.request.POST.get('Value[blue]'))
			
			
			
			returnObj = {}
			if(Type == "statusUpdate"):
				blueBadges = Badge.objects.filter(badge_team=Team.objects.get(team_name='blueTeam'))
				greenBadges = Badge.objects.filter(badge_team=Team.objects.get(team_name='greenTeam'))
				redBadges = Badge.objects.filter(badge_team=Team.objects.get(team_name='redTeam'))
				
				currentRed = len(redBadges)
				currentGreen = len(greenBadges)
				currentBlue = len(blueBadges)
				
				#print "current: %s %s %s" % (valRed,valGreen,valBlue)
				#print "new: %s %s %s" % (currentRed, currentGreen, currentBlue)
				
				
				
				if(currentBlue != valBlue or currentGreen != valGreen or currentRed != valRed):
					returnObj['red'] = len(redBadges)
					returnObj['green'] = len(greenBadges)
					returnObj['blue'] = len(blueBadges)
					#returnObj['blue'] = 19;
					returnObj['status'] = 'update'
					#print "difference!"
				#else:
					#print "No Update."
			
			return JsonResponse(returnObj);
		except Exception, e:
			print "Err: %s" % e
			return JsonResponse({});
	