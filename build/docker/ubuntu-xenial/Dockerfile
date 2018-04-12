FROM ubuntu:xenial
MAINTAINER Wieger Wesselink <j.w.wesselink@tue.nl>

# 1. Clone and build
# Packages needed for compiling the tools
RUN apt-get update && apt-get install -y \
 build-essential \
 cmake \
 git \
 libboost-dev \
 libgl1-mesa-dev \
 qt5-default

RUN cd ~/ && git clone git://github.com/mcrl2org/mcrl2.git mcrl2
RUN mkdir ~/mcrl2-build && cd ~/mcrl2-build && cmake . \
 -DCMAKE_BUILD_TYPE=RELEASE \
 -DBUILD_SHARED_LIBS=ON \
 -DMCRL2_ENABLE_DEVELOPER=ON \
 -DMCRL2_ENABLE_DEPRECATED=OFF \
 -DMCRL2_ENABLE_EXPERIMENTAL=ON \
 -DMCRL2_ENABLE_GUI_TOOLS=ON \
 -DCMAKE_INSTALL_PREFIX=`pwd`/install \
 ~/mcrl2
RUN cd ~/mcrl2-build && make -k -j8 

# 2. Test the build
# Packages needed for testing
RUN apt-get install -y \
 python-psutil \
 python-yaml
RUN cd ~/mcrl2-build && cmake -DMCRL2_ENABLE_TESTS=ON .
RUN cd ~/mcrl2-build && ctest . -j8

# 3. Build the documentation
# Packages needed for generating the documentation
RUN apt-get install -y \
 doxygen \
 python-sphinx \
 texlive \
 texlive-latex-extra \
 texlive-math-extra \
 texlive-science \
 xsltproc
RUN cd ~/mcrl2-build && make doc

# 4. Package the build
