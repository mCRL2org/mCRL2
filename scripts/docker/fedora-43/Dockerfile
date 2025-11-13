FROM fedora:43

# Install packages needed for compiling and testing the tools
RUN dnf install -y \
 boost-devel \
 cmake \
 doxygen \
 gcc-c++ \
 git \
 make \
 mesa-libGL-devel \
 mesa-libGLU-devel \
 qt6-qtbase-devel \
 which \
 # Packages needed for testing 
 python3 \
 python3-pip \
 python3-devel \
 libxml2-devel \
 libxslt-devel \
 # Packages needed for packaging
 rpm-build \
 # Packages needed for documentation
 doxygen \
 swig \
 texlive-scheme-full \ 
 dvisvgm 

# Copy the current source files into the root home directory
COPY requirements.txt /root/requirements.txt

# Install the required Python packages.
RUN pip install --user --requirement /root/requirements.txt

# Copy the current source files into the root home directory
COPY ./ /root/mcrl2/

# Configure out of source build
RUN mkdir ~/mcrl2-build && cd ~/mcrl2-build && cmake \
 -DCMAKE_BUILD_TYPE=RELEASE \
 -DBUILD_SHARED_LIBS=ON \
 -DMCRL2_ENABLE_DOCUMENTATION=ON \
 -DMCRL2_ENABLE_DEVELOPER=OFF \
 -DMCRL2_ENABLE_DEPRECATED=OFF \
 -DMCRL2_ENABLE_EXPERIMENTAL=OFF \
 -DMCRL2_ENABLE_DOC_DOXYGEN=OFF \
 -DMCRL2_ENABLE_GUI_TOOLS=ON \
 -DCMAKE_INSTALL_PREFIX=`pwd`/install \
 ~/mcrl2

# Build the toolset
ARG THREADS=8
RUN cd ~/mcrl2-build && make -j${THREADS} 

# Package the build
RUN cd ~/mcrl2-build && cpack -G RPM

# Test the toolset; tests require the experimental tools.
RUN cd ~/mcrl2-build \
    && cmake -DMCRL2_ENABLE_EXPERIMENTAL=ON \
             -DMCRL2_ENABLE_TESTS=ON . \
    && make -j${THREADS} \
    && ctest . -j${THREADS}

# Build the documentation
RUN cd ~/mcrl2-build && make doxygen && make doc