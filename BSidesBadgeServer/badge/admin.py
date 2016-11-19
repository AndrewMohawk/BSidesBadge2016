from django.contrib import admin

from .models import Badge, Challenges, Team, Log

admin.site.register(Badge)
admin.site.register(Challenges)
admin.site.register(Team)
admin.site.register(Log)