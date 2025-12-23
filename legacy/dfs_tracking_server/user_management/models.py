from django.db import models
from django.contrib.auth.models import User
from django.core.validators import RegexValidator

# To-do: Decide whether user_avatar would be hosted through django
# or some other platform and add accordingly to the model
class UserProfile(models.Model):
    user = models.OneToOneField(User, on_delete=models.CASCADE)
    bio = models.CharField(max_length=200, default="Hey there! I'm using DFS.")
    # phone number to be stored as eg. +918-12345678901234567
    phone_number = models.CharField(max_length=22, validators=[
        RegexValidator(regex=r"^\+\d{1,4}-\d{6,20}$",
        message="Invalid format or phone number",
        code="invalid_phone_no"),
    ])
