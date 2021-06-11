FROM fedora:34

#  Install packages needed for compiling and testing the tools.
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

# Clone mcrl2 in the home directory.
RUN cd ~/ && git clone --depth=1 -b release-202106 git://github.com/mcrl2org/mcrl2.git mcrl2

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
RUN cd ~/mcrl2-build && make -j8 install

# Clone and build the sylvan.
RUN cd ~/ && git clone --depth=1 --branch=v1.4.1 https://github.com/trolando/sylvan.git sylvan

# Disabled a specific warning as error since sylvan cannot be compiled otherwise.
RUN cd ~/sylvan && cmake -DCMAKE_C_FLAGS="-Wno-error=array-parameter" -DBUILD_SHARED_LIBS=OFF . && make -j8 install

# Clone ltsmin git repository.
RUN cd ~/ && git clone -b v3.0.2 https://github.com/utwente-fmt/ltsmin.git ltsmin
RUN cd ~/ltsmin && git checkout fea250ecc03ac456f26e272f44b8035eae968b9f

# This step is necessary according to the 'Building from a Git Repository' section of the readme.
RUN cd ~/ltsmin && git submodule update --init

# Fix issues with the ltl2ba submodule
RUN cd ~/ltsmin/ltl2ba && git checkout ea33bb2091a34f6af8dba8e26978fecb969aa93b

# Fix a compilation error in ltsmin.
COPY ltsmin.patch /root/ltsmin/
RUN cd ~/ltsmin && git apply ltsmin.patch

# Build the ltsmin toolset.
RUN cd ~/ltsmin && ./ltsminreconf && ./configure --disable-dependency-tracking --prefix=/root/ltsmin-build/ && cd ~/ltsmin && make -j8 install

# We can now copy the /root/ltsmin-build directory from the resulting image and install the mcrl2 release locally for the shared libraries and jittyc headers.