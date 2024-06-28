from knox.views import LoginView as KnoxLoginView
from knox.auth import TokenAuthentication
from rest_framework.authentication import BasicAuthentication
from rest_framework import generics
from rest_framework import permissions
from rest_framework import status
from rest_framework.views import APIView
from rest_framework.response import Response
from django.contrib.auth.models import User
from django.contrib.auth.hashers import make_password
from .models import UserProfile
from .serializers import (
    UserSerializer, UserProfileSerializer
)

# TO-DO:
# Add functionality for changing password
# 1) Send OTP to user on their registered email
# 2) If OTP is correct allow the user to change their password

# Overriding the authentication classes of knox login view as this view will be
# used for obtaining the access token (that would be used for further requests).
# Hence, the client can initially only provide username and password.
class LoginView(KnoxLoginView):
    authentication_classes = [BasicAuthentication]

class CreateUserView(generics.CreateAPIView):
    queryset = User.objects.all()
    serializer_class = UserSerializer

class GetUserProfileView(APIView):
    authentication_classes = [TokenAuthentication,]
    permission_classes = [permissions.IsAuthenticated,]

    def get(self, request, format=None):
        user_profile = UserProfile.objects.filter(user_id=self.request.user.id).first()

        if user_profile:
            user_profile_serializer = UserProfileSerializer(user_profile)
            return Response(user_profile_serializer.data, status=status.HTTP_200_OK)

        return Response({"detail": "profile not found"}, status=status.HTTP_404_NOT_FOUND)

class UpdateUserProfileView(generics.UpdateAPIView):
    authentication_classes = [TokenAuthentication,]
    permission_classes = [permissions.IsAuthenticated,]
    serializer_class = UserProfileSerializer

    def get_object(self):
        user_profile = UserProfile.objects.filter(user_id=self.request.user.id).first()

        if user_profile:
            return user_profile

        return Response({"detail": "profile not found"}, status=status.HTTP_404_NOT_FOUND)
