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
	'''
	def get(self, request, *args, **kwargs):
		Type = self.request.GET.get('object')
		
		returnObj = {}
		
		
		
		
		
		if(Type == "entity"):
			entities = Entity.objects.all()
			for e in entities:
				returnObj[e.entity_name] = e.entity_name
		elif(Type == "prefilter"):
			entities = Filter.objects.filter(filter_type='Pre')
			for e in entities:
				returnObj[e.id] = e.filter_name
		elif(Type == "postfilter"):
			entities = Filter.objects.filter(filter_type='Post')
			for e in entities:
				returnObj[e.id] = e.filter_name
		elif(Type == "datasource"):
			entities = Datasource.objects.all()
			for e in entities:
				returnObj[e.id] = e.datasource_name
		elif(Type == "settings"):
			entities = Setting.objects.all()
			for e in entities:
				returnObj[e.id] = e.setting_name
		elif(Type == "seeds"):
			entities = Seed.objects.all()
			for e in entities:
				returnObj[e.id] = e.seed_name
		return JsonResponse(returnObj)
	
	def post(self, request, *args, **kwargs):
		ID = self.request.POST.get('ID')
		Type = self.request.POST.get('Type')
		val = self.request.POST.get('name')
		
		thisTransform = Transform.objects.get(pk=ID)
		if(thisTransform):
			if(Type == "Name"):
				thisTransform.transform_name = val
			elif(Type == "Query"):
				thisTransform.transform_query = val
			elif(Type == "debug"):
				if(val == "1"):
					thisTransform.transform_debug = True
				else:
					thisTransform.transform_debug = False
			elif(Type == "Mapping"):
				thisTransform.transform_mapping = val
			elif(Type == "InputEntity"):
				thisTransform.transform_input_entity = val
			elif(Type == "ds"):
				thisTransform.transform_datasource = val
			elif(Type == "Pre"):
				SelectedFilter = None
				val = int(val)
				if(val > -1):
					SelectedFilter = Filter.objects.get(pk=val)
				thisTransform.transform_pre = SelectedFilter
			elif(Type == "Post"):
				SelectedFilter = None
				val = int(val)
				if(val > -1):
					SelectedFilter = Filter.objects.get(pk=val)
				thisTransform.transform_post = SelectedFilter
			elif(Type == "Settings"):
				settingInts = self.request.POST.getlist("name[]")
				thisTransform.transform_settings = settingInts
				
			elif(Type == "Seeds"):
				seedInts = self.request.POST.getlist("name[]")
				seedsContainingThisTransform =  Seed.objects.filter(seed_transforms=thisTransform)
				for s in seedsContainingThisTransform:
					s.seed_transforms.remove(thisTransform)

				for sI in seedInts:
					seed = Seed.objects.get(pk=sI)
					seed.seed_transforms.add(thisTransform)
					
				
			else:
				print "unknown: %s" % (Type)
		else:
			print "NO TRANSFORM FOUND"
		thisTransform.save()	
		return HttpResponse(val)
		#return self.form_valid(form, **kwargs)
		'''