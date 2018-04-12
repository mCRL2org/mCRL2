FROM fedora:27
MAINTAINER Wieger Wesselink <j.w.wesselink@tue.nl>

# 1. Cloning and building
# Packages needed for compiling the tools
RUN dnf install -y \
 boost-devel \
 cmake \
 gcc-c++ \
 git \
 make \
 mesa-libGL-devel \
 mesa-libGLU-devel \
 qt5 \
 qt5-qtbase-devel \
 which

# Clone the git repository in the home directory
RUN cd ~/ && git clone git://github.com/mcrl2org/mcrl2.git mcrl2

# Out of source build
RUN mkdir ~/mcrl2-build && cd ~/mcrl2-build && cmake \
 -DCMAKE_BUILD_TYPE=RELEASE \
 -DBUILD_SHARED_LIBS=ON \
 -DMCRL2_ENABLE_DEVELOPER=ON \
 -DMCRL2_ENABLE_DEPRECATED=OFF \
 -DMCRL2_ENABLE_EXPERIMENTAL=ON \
 -DMCRL2_ENABLE_GUI_TOOLS=ON \
 -DCMAKE_INSTALL_PREFIX=`pwd`/install \
 ~/mcrl2

RUN cd ~/mcrl2-build && make -k -j8 

# 2. Testing build 
# Packages needed for testing
RUN dnf install -y \
 python-psutil \
 python-yaml 

RUN cd ~/mcrl2-build && cmake -DMCRL2_ENABLE_TESTS=ON 
RUN cd ~/mcrl2-build && ctest . -j8

# 3. Building documentation
# Packages needed for generating the documentation
RUN dnf install -y \
 doxygen \
 python-sphinx \
 texlive \
 texlive-scheme-medium \
 texlive-collection-mathextra

# Re-run cmake because the documentation building can be enabled now
RUN cd ~/mcrl2-build && cmake . && make doc 

# 4. Package the build
