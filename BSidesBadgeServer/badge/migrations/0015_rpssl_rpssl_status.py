# -*- coding: utf-8 -*-
# Generated by Django 1.9.4 on 2016-12-01 22:51
from __future__ import unicode_literals

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('badge', '0014_rpssl'),
    ]

    operations = [
        migrations.AddField(
            model_name='rpssl',
            name='rpssl_status',
            field=models.IntegerField(default=0, verbose_name='Game Status'),
        ),
    ]
