# This dockerfile builds the docker image for IFEM CI builds

FROM ubuntu:24.04

WORKDIR /tmp/ifem
ADD . .

# Make sure we have updated URLs to packages etc.
RUN apt-get update -y

# Make sure we have updated packages
RUN apt-get upgrade -y

# Packages needed for add-apt-repository
RUN apt-get install -y software-properties-common wget apt-transport-https openssh-client

# Add apt-repo for the IFEM packages
RUN apt-add-repository ppa:ifem/ppa

# Install system packages
RUN apt-get install -y cmake g++ gfortran git pkg-config ninja-build \
                       libarpack2-dev libsuperlu-dev libtinyxml2-dev catch2 \
                       libgotools-core-dev libgotools-trivariate-dev libboost-dev \
                       libhdf5-serial-dev libcereal-dev

# Install SAM
RUN /tmp/ifem/build_sam.sh

# Build IFEM and the application
RUN /tmp/ifem/build_ifem.sh master master master

# Create and mount the working directory
RUN mkdir /work
WORKDIR /work
VOLUME /work
