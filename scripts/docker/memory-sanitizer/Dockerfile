FROM fedora:43

# Install packages needed for compiling and testing the tools
RUN dnf install -y \
 boost-devel \
 clang \
 cmake \
 git \
 make \
 which \
 # Packages needed for testing
 python3 \
 python3-pip \
 python3-devel \
 libxml2-devel \
 libxslt-devel

RUN cd /root/ && git clone --depth=1 --branch=llvmorg-21-init https://github.com/llvm/llvm-project

# Configure the libcxx standard library with memory sanitizer support
RUN mkdir /root/llvm-build && cd /root/llvm-build && cmake  \
 -DCMAKE_BUILD_TYPE=Release \
 -DLLVM_ENABLE_RUNTIMES="libcxx;libcxxabi;libunwind" \
 -DCMAKE_C_COMPILER=clang \
 -DCMAKE_CXX_COMPILER=clang++ \
 -DLLVM_USE_SANITIZER=MemoryWithOrigins \
 /root/llvm-project/runtimes

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
 -DCMAKE_CXX_FLAGS="-nostdinc++ -I/root/llvm-build/include/c++/v1" \
 -DCMAKE_EXE_LINKER_FLAGS="--stdlib=libc++ -L/root/llvm-build/lib" \
 -DCMAKE_BUILD_TYPE=DEBUG \
 -DBUILD_SHARED_LIBS=ON \
 -DMCRL2_ENABLE_DOCUMENTATION=OFF \
 -DMCRL2_ENABLE_DEVELOPER=ON \
 -DMCRL2_ENABLE_DEPRECATED=ON \
 -DMCRL2_ENABLE_EXPERIMENTAL=ON \
 -DMCRL2_ENABLE_DOC_DOXYGEN=OFF \
 -DMCRL2_ENABLE_GUI_TOOLS=OFF \
 -DMCRL2_ENABLE_MEMORYSANITIZER=ON \
 -DMCRL2_ENABLE_TESTS=ON \
 -DCMAKE_INSTALL_PREFIX=`pwd`/install \
 /root/mcrl2

# Build the toolset
RUN cd /root/mcrl2-build && make -j${THREADS} 

# Test the toolset
RUN ctest . -j${THREADS}
