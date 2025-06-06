FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive

# Install build tools and other libs and packages
RUN apt-get update && apt-get install -y \
  build-essential cmake git curl wget unzip pkg-config autoconf automake \
  libtool libssl-dev libsodium-dev python3 python3-pip nodejs npm clang zlib1g-dev sqlite3 libsqlite3-dev \
  && apt-get clean && rm -rf /var/lib/apt/lists/*

# Install gRPC v1.66.0 from source
WORKDIR /opt
RUN git clone --recurse-submodules -b v1.66.0 --depth 1 --shallow-submodules https://github.com/grpc/grpc.git && \
  cd grpc && mkdir -p cmake/build && \
  cd cmake/build && cmake ../.. && make -j$(nproc) && make install

ENV DFSN_GRPC_CPP_PLUGIN=/opt/grpc/cmake/build/grpc_cpp_plugin

RUN npm install -g pnpm@latest

# Mount the codebase
WORKDIR /workspace
CMD [ "bash" ]