<p align="center">
  <img src=".docs/dfsn_logo.png" width="250" alt="Decentralized File Storage Logo" />
</p>

<h1 align="center">Decentralized File Storage Network (dFSN)</h1>

dFSN is a small distributed file storage network which allows the user to split and disperse their files over a network ensuring that other than the user no one else can re-construct the files. The network utilises gRPC for transmission and sodium for file encryption. The network is peer-to-peer eliminating any middle man between the user and the storage nodes.

>**Note**: The project includes vendor headers and files. Please see the associated [licenses](.licenses).

## Network Architecture
The network has two interoperable variants: Organization Hosted and Open.

The network can be used as a secure storage medium especially for archival storage for eg. blueprints, customer documents (PII), etc. that are not accessed frequently but are extremely sensitive. For such use cases, tracking servers allow the organization to have a more greater control over where the chunks are being stored, the process of actually creating the chunks and reconstructing the file can only be done by the uploading user.

![Organization Hosted](.docs/DecentralizedFileStorage.jpg)
<p align="center">Organization Hosted dFSN</p>

The second variant is the *true* decentralized file storage network where only the uploading user decides where and how the chunks are stored. Each storage node cluster follow a *db-cluster* like topology where a leader node is responsible for handling incoming nodes, replicating and dispersing them across the cluster and following a robust failover procedure (*under development*).

![New Architecture](.docs/DecentralizedFileStorage2.0.jpg)
<p align="center">Open dFSN</p>

### Work Flow
The diagram describes the overall workflow from the user client's perspective.
![Work Flow](.docs/WorkFlow.jpg)

There are broadly three steps *after splitting the file* into encypted chunks:
- Choose the clusters where the chunks are going to be stored
- Request each cluster to allocate space for your chunks with appropriate replication
- Upload the chunks to the specific nodes as told by the respective clusters.

## Technology Stack
- C/C++
- gRPC
- Sodium
- protobuf
- SQLite
- Angular

### Ideology behind the Network
As illustrated in the diagram, the user client would decide the distribution of encypted chunks and the storage nodes which the client would send these chunks to would be decided by the tracking server.

The *tracking server* will now be a separate process running only in the leader storage node. In each *storage custer* there would be a leader node that in addition to being a storage node would also serve as the tracking server and allocating storage across the enitre cluster. For every cluster, there will be a pre-defined *chain of command* and in case of the leader node going down the next highest node would automatically assume leadership. There is also one and only one *Heap Service* per cluster, this service maintains an in memory fibonacci heap (which as of now is used for allocation) and the entire chain of command. The leader node also maintains a copy of the fibonacci heap, in case of a Heap Service failure, the cluster can still operate in a *degraded mode*, all operations other than failover are possible in degraded mode.

>**Note**: Unless the specific client that dispersed the chunks gets compromised. Since only that specific client knows on which nodes the chunks exist.

- From a commercial standpoint, if we were to present this as a decentralized Google Drive or Dropbox of some sort then we can store the dispersion information (see above note) in a backup service and charge users for that service. Ofcourse that service would be optional and you can avoid using it by simplying managing that information yourself.

If this were commercial how would we get the storage nodes?
- Businesses which have their own hosting infrastructure generally have spare storage space which they don't really use but still have to bear the expense of maintaining it. Such players can devote a set (guranteed for use by dFSN) amount of storage to the network, have their own tracking server and charge users / incentivise them to use their nodes.

- Since this is a decentralized network nodes can go down and may never come back up since they are not under our direct control. To mitigate this each chunk will obviously be on more than one node for redundancy purposes, and the users can have an option to also disperse some or all of their chunks to one or more of the above mentioned privately owned chunks which would be guranteed to be up.


## Setup
The docker image only provides you the environment to build and run the C++ portions of the project. Since the actual codebase is mounted from outside the container, NodeJS must be installed locally. While installing packages inside the container is possible, it may lead to overriding of `node_modules` folder when you mount the codebase. Additionally, the image uses `ubuntu:24.04` as the base image, you can experiment and use a lighter base image (in that case make sure to exclude your `infra/dev.Dockerfile` in your `.git/info/exclude`. This is only if you wish to contribute to the project).

### Setup (Docker)
1. Clone the repository:
```bash
git clone https://github.com/B4S1C-Coder/Decentralized-File-Storage.git && cd 'Decentralized-File-Storage'
```

2. Build the docker image:
```bash
docker build -f infra/dev.Dockerfile -t dfs-dev .
```

3. Run the development container:
```bash
docker run -it --rm -v "$(pwd)":/workspace -w /workspace dfs-dev
```

4. Build the project (ensure you are in the container shell):
```bash
./scripts/clean-build.sh --no-env
```

5. Install npm packages (recommended to install outside the container, locally):
```bash
npm i && cd demo-registry && npm i && cd ..
```

### Setup (local)
1. Setup **gRPC (1.66.0)** for C/C++ by following the guide on [grpc.io](https://grpc.io/docs/languages/cpp/quickstart/)

2. Setup libsodium (you can install it via apt package manager if on Debian based distros): [Sodium docs](https://doc.libsodium.org/installation)

3. Clone the repository:
```bash
git clone https://github.com/B4S1C-Coder/Decentralized-File-Storage.git && cd 'Decentralized-File-Storage'
```

4. Create a `.env` file from the given example. Populate the environment variables in the `.env`, each variable has a description explaining what it does:
```bash
cp .env.example .env
```

5. Install npm packages:
```bash
npm i && cd demo-registry && npm i && cd ..
```

6. Give execute permissions to scripts:
```bash
chmod +x ./scripts/*.sh
```

7. Last but not the least build the project:
 - In the project root run:
 ```bash
 ./scripts/clean-build.sh
 ```
 - The build should progress smoothly provided you have set-up gRPC and Sodium correctly as well as set the variables in `.env` correctly.
 
>**Note**: This project is being developed and tested on `WSL2 Ubuntu 24.04.2 LTS` with `g++ (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0`. It is recommended that you use Linux or WSL.

## Running the Project
The following applies to running the `demo-*` projects only:

1. After you've built the project, first start the demo registry:
```bash
cd demo-registry && node registry.js
```

2. Start the demo server (from `build/demo-server`):
```bash
./demo_server localhost localhost 50051
```

3. Start the demo client (from `build/demo-client`):
```bash
./demo_client --transmit ../../res/data.txt 5
```

>**Note**: Don't forget to expose the ports of the container if interacting from outside the container.