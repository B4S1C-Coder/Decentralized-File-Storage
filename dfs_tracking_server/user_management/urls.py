from django.urls import path
from knox import views as knox_views
from . import views

urlpatterns = [
    path('create/', views.CreateUserView.as_view(), name="createuser"),
    path('login/', views.LoginView.as_view(), name="knox_login"),
    path('logout/', knox_views.LogoutView.as_view(), name="knox_logout"),
    path('info/', views.GetUserProfileView.as_view(), name="getuserinfo"),
    path('update/', views.UpdateUserProfileView.as_view(), name="updateuserinfo"),
]