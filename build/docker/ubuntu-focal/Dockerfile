FROM ubuntu:focal

# 1. Install packages needed for compiling and testing the tools
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y \
 build-essential \
 cmake \
 git \
 libboost-dev \
 libgl1-mesa-dev \
 qt5-default \
# Packages needed for packaging
# Packages needed for testing
 python3-psutil \
 python3-yaml \
# Packages needed for generating the documentation
 doxygen \
 python3-pip \
 sphinx-common \
 swig \
 texlive \
 texlive-latex-extra \
 texlive-science \
 xsltproc

# This package is not available in the ubuntu repository
RUN pip install --user dparser

# 2. Clone the git repository in the home directory
RUN cd ~/ && git clone -b release-202106 git://github.com/mcrl2org/mcrl2.git mcrl2

# 3. Configure out of source build
RUN mkdir ~/mcrl2-build && cd ~/mcrl2-build && cmake . \
 -DCMAKE_BUILD_TYPE=RELEASE \
 -DBUILD_SHARED_LIBS=ON \
 -DMCRL2_ENABLE_DOCUMENTATION=ON \
 -DMCRL2_ENABLE_DEVELOPER=OFF \
 -DMCRL2_ENABLE_DEPRECATED=OFF \
 -DMCRL2_ENABLE_EXPERIMENTAL=OFF \
 -DMCRL2_ENABLE_GUI_TOOLS=ON \
 -DMCRL2_PACKAGE_RELEASE=ON \
 -DCMAKE_INSTALL_PREFIX=`pwd`/install \
 ~/mcrl2

# 4. Build the toolset
RUN cd ~/mcrl2-build && make -j8 

# 5. Package the build
RUN cd ~/mcrl2-build && cpack -G DEB

# 6. Test the toolset; tests require the experimental tools.
RUN cd ~/mcrl2-build \
    && cmake -DMCRL2_ENABLE_EXPERIMENTAL=ON \
             -DMCRL2_ENABLE_TESTS=ON . \
    && make -j8 \
    && ctest . -j8

# 7. Build the documentation
RUN cd ~/mcrl2-build && make doc 