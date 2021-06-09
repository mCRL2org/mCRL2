FROM ubuntu:focal
LABEL version="1.0"
LABEL description="prepares the "

ARG MCRL2_RELEASE_BRANCH="release-202006"
ARG MCRL2_VERSION="202006.0"

# 1. Install packages needed for compilation.
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
 build-essential \
 cmake \
 git \
 libboost-dev \
 libgl1-mesa-dev \
 qt5-default \
# Needed to create the debian package.
 ubuntu-dev-tools \
 debhelper \
# Needed for testing
 python-psutil \
 python-yaml

# 2. Clone the release branch of the repository.
RUN mkdir ~/packaging && cd ~/packaging && git clone -b $MCRL2_RELEASE_BRANCH git://github.com/mcrl2org/mcrl2.git mcrl2

# 3. Build the mCRL2 release.
RUN ["/bin/bash", "-c", "cd ~/packaging && source mcrl2/build/make_release/make_release && MCRL2_VERSION=$MCRL2_VERSION build_mcrl2_release"]

# 4. Package the source package.
RUN ["/bin/bash", "-c", "cd ~/packaging && source mcrl2/build/make_release/make_release && MCRL2_VERSION=$MCRL2_VERSION build_source_package"]

# 5. Test the debian packages.
RUN ["/bin/bash", "-c", "cd ~/packaging && source mcrl2/build/make_release/make_release && MCRL2_VERSION=$MCRL2_VERSION test_ppa_package bionic "]
RUN ["/bin/bash", "-c", "cd ~/packaging && source mcrl2/build/make_release/make_release && MCRL2_VERSION=$MCRL2_VERSION test_ppa_package focal "]
RUN ["/bin/bash", "-c", "cd ~/packaging && source mcrl2/build/make_release/make_release && MCRL2_VERSION=$MCRL2_VERSION test_ppa_package groovy "]

# 6. Enter the docker image, copy your ssh key to ~/.ssh/ and execute build_ppa_package for every package.