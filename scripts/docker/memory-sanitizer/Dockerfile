FROM fedora:42

# Install packages needed for compiling and testing the tools
RUN dnf install -y \
 boost-devel \
 clang \
 cmake \
 git \
 make \
 mesa-libGL-devel \
 mesa-libGLU-devel \
 qt6-qtbase-devel \
 which \
 # Packages needed for testing
 python3 \
 python3-pip

RUN cd /root/ && git clone --depth=1 --branch=llvmorg-21-init https://github.com/llvm/llvm-project

# Configure the libcxx standard library with memory sanitizer support
RUN mkdir /root/llvm-build && cd /root/llvm-build && cmake /root/llvm-project/llvm \
 -S /root/llvm-project/runtimes \
 -DCMAKE_BUILD_TYPE=Release \
 -DLLVM_ENABLE_PROJECTS="clang" \
 -DLLVM_ENABLE_RUNTIMES="libcxx;libunwind" \
 -DCMAKE_C_COMPILER=clang \
 -DCMAKE_CXX_COMPILER=clang++ \
 -DLLVM_USE_SANITIZER=MemoryWithOrigins

ARG THREADS=8
RUN cd /root/llvm-build/ && make -j${THREADS} cxx

# Copy the current source files into the root home directory
COPY requirements.txt /root/requirements.txt

# Install the required Python packages.
RUN pip install --user --requirement /root/requirements.txt

# Copy the current source files into the root home directory
COPY ./ /root/mcrl2/

# Configure out of source build
RUN mkdir /root/mcrl2-build && cd /root/mcrl2-build && cmake \
 -DCMAKE_C_COMPILER=clang \
 -DCMAKE_CXX_COMPILER=clang++ \
 -DCMAKE_CXX_FLAGS="--stdlib=libc++ -nostdinc++ -I/root/llvm-build/include/c++/v1 -L/root/llvm-build/lib -Wl,-rpath,/root/llvm-build/lib" \
 -DCMAKE_EXE_LINKER_FLAGS="--stdlib=libc++ -L/root/llvm-build/lib -Wl,-rpath,/root/llvm-build/lib" \
 -DCMAKE_BUILD_TYPE=RELEASE \
 -DBUILD_SHARED_LIBS=ON \
 -DMCRL2_ENABLE_DOCUMENTATION=ON \
 -DMCRL2_ENABLE_DEVELOPER=OFF \
 -DMCRL2_ENABLE_DEPRECATED=OFF \
 -DMCRL2_ENABLE_EXPERIMENTAL=OFF \
 -DMCRL2_ENABLE_DOC_DOXYGEN=OFF \
 -DMCRL2_ENABLE_GUI_TOOLS=ON \
 -DCMAKE_INSTALL_PREFIX=`pwd`/install \
 /root/mcrl2

# Build the toolset
RUN cd /root/mcrl2-build && make -j${THREADS} 

# Package the build
RUN cd /root/mcrl2-build && cpack -G RPM

# Test the toolset; tests require the experimental tools.
RUN cd /root/mcrl2-build \
    && cmake -DMCRL2_ENABLE_EXPERIMENTAL=ON \
             -DMCRL2_ENABLE_TESTS=ON . \
    && make -j${THREADS} \
    && ctest . -j${THREADS}
