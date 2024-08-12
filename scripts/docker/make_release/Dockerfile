FROM ubuntu:noble
LABEL version="1.0"
LABEL description="prepares the ppa release using the make_release scripts"

ARG MCRL2_RELEASE_BRANCH="release-202307"
ARG MCRL2_VERSION="202307.0"

# Install packages needed for compilation and the ppa release building.
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y \
 build-essential \
 cmake \
 dos2unix \
 git \
 libboost-dev \
 libgl1-mesa-dev \
 libglu1-mesa-dev \
 qt6-base-dev \
 ubuntu-dev-tools \
 debhelper \
 python3-psutil \
 python3-yaml \
 file

# Copy the current source files into the root home directory
COPY ./ /root/ppa/mcrl2/

# Build the mCRL2 release.
SHELL ["/bin/bash", "-c"] 
RUN cd ~/ppa \
    && dos2unix ~/ppa/mcrl2/scripts/make_release/make_release \
    && source ~/ppa/mcrl2/scripts/make_release/make_release \
    && MCRL2_RELEASE_BRANCH=$MCRL2_RELEASE_BRANCH MCRL2_VERSION=$MCRL2_VERSION build_mcrl2_release

# Package the source package.
SHELL ["/bin/bash", "-c"] 
RUN cd ~/ppa \
    && dos2unix ~/ppa/mcrl2/scripts/make_release/make_release \
    && source ~/ppa/mcrl2/scripts/make_release/make_release \
    && MCRL2_RELEASE_BRANCH=$MCRL2_RELEASE_BRANCH MCRL2_VERSION=$MCRL2_VERSION build_source_package

# Here we have to add the gpg key into the docker image.

# 5. Test the debian packages.
#RUN ["/bin/bash", "-c", "cd ~/ppa && source mcrl2/build/make_release/make_release && MCRL2_VERSION=$MCRL2_VERSION test_ppa_package focal "]
#RUN ["/bin/bash", "-c", "cd ~/ppa && source mcrl2/build/make_release/make_release && MCRL2_VERSION=$MCRL2_VERSION test_ppa_package jammy "]
#RUN ["/bin/bash", "-c", "cd ~/ppa && source mcrl2/build/make_release/make_release && MCRL2_VERSION=$MCRL2_VERSION test_ppa_package noble "]

