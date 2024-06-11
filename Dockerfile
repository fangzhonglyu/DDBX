# Dependency setups

FROM ubuntu:20.04
ARG DEBIAN_FRONTEND=noninteractive
ARG DEPS="software-properties-common build-essential cmake make git curl freeglut3-dev libcurl4-openssl-dev libglew-dev libxext-dev libssl-dev python3 python3-pip"
ARG PYDEPS="shortuuid pillow PyYAML"

RUN apt-get update && apt-get install -y ${DEPS} \
    && rm -rf /var/lib/apt/lists/*

RUN add-apt-repository -y ppa:ubuntu-toolchain-r/test && apt-get update && apt-get install -y g++-11 \
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 100 --slave /usr/bin/g++ g++ /usr/bin/g++-11

RUN pip3 install ${PYDEPS}

# Copying project files
WORKDIR /app
COPY . /app

WORKDIR /app/CUGL-Server/scripts
RUN python3 release.py

WORKDIR /app/CUGL-Server/
RUN rm -rf external

WORKDIR /app/CUGL-Server/release/CUGL

RUN python3 /app/CUGL-Server/release/cugl /app/DDBX/Client
WORKDIR /app/DDBX/Client/build/cmake

RUN mkdir build

WORKDIR /app/DDBX/Client/build/cmake/build

RUN cmake ..

RUN make

# # Command to run the application

CMD ["./install/DDBXServer.exe"]