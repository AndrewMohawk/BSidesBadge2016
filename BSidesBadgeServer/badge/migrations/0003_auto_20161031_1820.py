# -*- coding: utf-8 -*-
# Generated by Django 1.9.4 on 2016-10-31 18:20
from __future__ import unicode_literals

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('badge', '0002_auto_20161031_1802'),
    ]

    operations = [
        migrations.AlterField(
            model_name='badge',
            name='badge_challenges',
            field=models.ManyToManyField(to='badge.Challenges'),
        ),
    ]
