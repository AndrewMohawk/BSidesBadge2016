from django.contrib import admin

from .models import Badge, Challenges, Team, Log, gameStatus, RPSSL

admin.site.register(Badge)
admin.site.register(Challenges)
admin.site.register(Team)
admin.site.register(Log)
admin.site.register(gameStatus)
admin.site.register(RPSSL)