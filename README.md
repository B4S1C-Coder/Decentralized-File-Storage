# Decentralized File Storage Network (DFS)
DFS is a small distributed file storage network which allows the user to split and disperse their files over a network ensuring that other than the user no one else can re-construct the files. The network is based on gRPC and peer-to-peer communication, there is no middle man between the user and the storage node.

## Techstack
- [Django](https://www.djangoproject.com/)
- [Django REST Framework](https://www.django-rest-framework.org/)
- [gRPC](https://grpc.io/)
- [protobuf](https://protobuf.dev/)
- [MySQL](https://www.mysql.com/)
- [SQLite](https://sqlite.org/)

The diagram below shows a detailed working of DFS.

![Working](https://github.com/B4S1C-Coder/Decentralized-File-Storage/blob/main/DecentralizedFileStorage.jpg)

>**Note**: As of now the project is under development and the first prototype will be implemented in python and C/C++ after that everything will likely be converted to C/C++ (or any other compiled language).

## Targets for First Prototype

- **Storage service for less conscious users**: Normally the information regarding which chunks are stored on which node is kept strictly on the client machine to adhere to the main goal of the project. However, if the client wishes they can store this information on a tracking server (**this is optional**) so that when they change machines they do not have to manually import the data (previously mentioned) to the new machine.

- Create a CLI and a GUI (desktop)
