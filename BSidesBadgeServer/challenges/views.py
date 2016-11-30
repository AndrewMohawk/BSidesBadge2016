from django.shortcuts import render

# Create your views here.
from django.shortcuts import render
from django.views.generic import TemplateView
from django.http import HttpResponse, JsonResponse
from badge.models import Challenges

class challengeDashboard(TemplateView):
    template_name = "challengeList.html"
    def get(self, request, *args, **kwargs):
        return JsonResponse({});

class badgeDashboardAjax(TemplateView):
    template_name = "index.html"
    def get(self, request, *args, **kwargs):
        return JsonResponse({});
    