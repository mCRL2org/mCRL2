FROM fedora:25
MAINTAINER Wieger Wesselink <j.w.wesselink@tue.nl>

# Packages needed for compiling the tools
RUN dnf install -y \
 boost-devel \
 cmake \
 gcc-c++ \
 mesa-libGL-devel \
 mesa-libGLU-devel \
 qt5 \
 qt5-qtbase-devel \
 subversion

# Packages needed for generating the documentation
RUN dnf install -y \
 doxygen \
 python-sphinx \
 texlive \
 texlive-scheme-medium \
 texlive-collection-mathextra

# Packages needed for testing
RUN dnf install -y \
 python-psutil \
 python-yaml 

RUN svn co https://svn.win.tue.nl/repos/MCRL2/trunk mcrl2
RUN cd mcrl2 && cmake . \
 -DCMAKE_BUILD_TYPE=RELEASE \
 -DBUILD_SHARED_LIBS=ON \
 -DMCRL2_ENABLE_DEVELOPER=ON \
 -DMCRL2_ENABLE_DEPRECATED=OFF \
 -DMCRL2_ENABLE_EXPERIMENTAL=ON \
 -DMCRL2_ENABLE_GUI_TOOLS=ON \
 -DCMAKE_INSTALL_PREFIX=`pwd`/install \
&& make -k -j8 \
&& make install -j8 \
&& make doc \
&& ctest . -j8
