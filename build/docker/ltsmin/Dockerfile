FROM fedora:38

# Install packages needed for compiling and testing the tools.
RUN dnf install -y \
 boost-devel \
 cmake \
 gcc-c++ \
 git \
 make \
 which \
# Dependencies for sylvan.
 gmp-devel \
# Dependencies for ltsmin.
 ant-lib \
 autoconf \
 automake \
 bison \
 diffutils \
 flex \
 file \
 libtool \
 libtool-ltdl-devel \
 pkgconf \
 popt-devel \
 zlib-devel

# Copy the current source files into the root home directory
COPY ./ /root/mcrl2/

# Configure out of source build.
RUN mkdir ~/mcrl2-build && cd ~/mcrl2-build && cmake \
 -DCMAKE_BUILD_TYPE=RELEASE \
 -DBUILD_SHARED_LIBS=ON \
 -DMCRL2_ENABLE_DOCUMENTATION=OFF \
 -DMCRL2_ENABLE_DEVELOPER=OFF \
 -DMCRL2_ENABLE_DEPRECATED=OFF \
 -DMCRL2_ENABLE_EXPERIMENTAL=OFF \
 -DMCRL2_ENABLE_GUI_TOOLS=OFF \
 ~/mcrl2

# Build the toolset.
ARG THREADS=8
RUN cd ~/mcrl2-build && make -j${THREADS} install

# Clone and build the sylvan.
RUN cd ~/ && git clone --depth=1 --branch=v1.5.0 https://github.com/trolando/sylvan.git sylvan

# Disabled a specific warning as error since sylvan cannot be compiled otherwise.
RUN cd ~/sylvan && cmake -DCMAKE_C_FLAGS="-Wno-error=array-parameter" -DBUILD_SHARED_LIBS=OFF . && make -j${THREADS} install

# Clone ltsmin git repository at a specific version.
RUN cd ~/ && git clone https://github.com/utwente-fmt/ltsmin.git ltsmin
RUN cd ~/ltsmin && git checkout 3499bb5afb06c5e955d0df5bb3220eb7b654f3da

# This step is necessary according to the 'Building from a Git Repository' section of the readme.
RUN cd ~/ltsmin && git submodule update --init

# Build the ltsmin toolset.
RUN cd ~/ltsmin \
    && ./ltsminreconf \
    && ./configure --disable-dependency-tracking --prefix=/root/ltsmin-build/ \
    && cd ~/ltsmin \
    && make -j${THREADS} install

# We can now copy the /root/ltsmin-build directory from the resulting image and install the mcrl2 release locally for the shared libraries and jittyc headers.