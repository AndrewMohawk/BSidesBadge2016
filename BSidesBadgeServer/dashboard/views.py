from django.shortcuts import render
from django.views.generic import TemplateView
from django.http import HttpResponse, JsonResponse
from badge.models import Badge, Challenges, Team, gameStatus

class badgeDashboard(TemplateView):
	template_name = "index.html"
# Create your views here.


class badgeDashboardAjax(TemplateView):
	
	template_name = "index.html"
	
	def post(self, request, *args, **kwargs):
		try:
			Type = self.request.POST.get('Type')
			
			
			
			
			
			returnObj = {}
			if(Type == "fullDay"):
				dayStats = gameStatus.objects.all();
				redList = []
				greenList = []
				blueList = []
				for timePeriod in dayStats:
					#print timePeriod
					redList.append([str(timePeriod.gamestatus_timestamp)[:19],timePeriod.gamestatus_red])
					greenList.append([str(timePeriod.gamestatus_timestamp)[:19],timePeriod.gamestatus_green])
					blueList.append([str(timePeriod.gamestatus_timestamp)[:19],timePeriod.gamestatus_blue])
					#returnObj[] = {"red": timePeriod.gamestatus_red,"blue": timePeriod.gamestatus_blue,"green": timePeriod.gamestatus_green }
				
				returnObj["fullDayData"] = [{"name":"Red Team","data":redList},{"name":"Green Team","data":greenList},{"name":"Blue Team","data":blueList}]
				
				returnObj["red"] = redList
				returnObj["blue"] = blueList
				returnObj["green"] = greenList
				
			if(Type == "statusUpdate"):
				valRed = int(self.request.POST.get('Value[red]'))
				valGreen = int(self.request.POST.get('Value[green]'))
				valBlue = int(self.request.POST.get('Value[blue]'))
				
				blueBadges = Badge.objects.filter(badge_team=Team.objects.get(team_name='blue'))
				greenBadges = Badge.objects.filter(badge_team=Team.objects.get(team_name='green'))
				redBadges = Badge.objects.filter(badge_team=Team.objects.get(team_name='red'))
				
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
	