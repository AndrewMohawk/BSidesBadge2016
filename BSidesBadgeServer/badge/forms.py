from django.db import models
from django.forms import ModelForm
from django import forms
from .models import Badge

class AliasForm(ModelForm):
	badge_number = forms.CharField(widget=forms.TextInput(attrs={'size': '20'}),required=True)
	badge_alias = forms.CharField(widget=forms.TextInput(attrs={'size': '20'}),required=True)
	badge_verify = forms.CharField(widget=forms.TextInput(attrs={'size': '20'}),required=True)
	class Meta:
		model = Badge
		fields = ( 'badge_number', 'badge_alias','badge_verify')
		
class ChallengeForm(ModelForm):
	badge_number = forms.CharField(widget=forms.TextInput(attrs={'size': '20'}),required=True)
	challenge_hash = forms.CharField(widget=forms.TextInput(attrs={'size': '20'}),required=True)
	
	class Meta:
		model = Badge
		fields = ( 'badge_number', 'challenge_hash')
	


