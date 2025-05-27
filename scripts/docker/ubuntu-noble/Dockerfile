FROM ubuntu:noble

# Install packages needed for compiling and testing the tools
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y \
 build-essential \
 cmake \
 git \
 libboost-dev \
 libgl1-mesa-dev \
 libglu1-mesa-dev \
 qt6-base-dev \
# Packages needed for generating the documentation
 doxygen \
 wget \ 
 python3-pip \
 python3-venv \
 sphinx-common \
 texlive \
 texlive-latex-extra \
 texlive-science \
 texlive-pstricks \
# Packages needed for creating the package
 file

# Copy the current source files into the root home directory
COPY requirements.txt /root/requirements.txt

# Create a virtual Python environment
RUN python3 -m venv ~/mcrl2-venv/

# Install the required Python packages.
RUN . ~/mcrl2-venv/bin/activate \
 && pip install --requirement /root/requirements.txt

# Copy the current source files into the root home directory
COPY ./ /root/mcrl2/

# Configure out of source build
RUN . ~/mcrl2-venv/bin/activate \
 && mkdir ~/mcrl2-build \
 && cd ~/mcrl2-build \
 && cmake . \
 -DCMAKE_BUILD_TYPE=RELEASE \
 -DBUILD_SHARED_LIBS=ON \
 -DMCRL2_ENABLE_DOCUMENTATION=ON \
 -DMCRL2_ENABLE_DEVELOPER=OFF \
 -DMCRL2_ENABLE_DEPRECATED=OFF \
 -DMCRL2_ENABLE_EXPERIMENTAL=OFF \
 -DMCRL2_ENABLE_GUI_TOOLS=ON \
 -DCMAKE_INSTALL_PREFIX=`pwd`/install \
 ~/mcrl2

# 4. Build the toolset
ARG THREADS=8
RUN cd ~/mcrl2-build && make -j${THREADS}  

# 5. Package the build
RUN cd ~/mcrl2-build && cpack -G DEB

# 6. Test the toolset; tests require the experimental tools.
RUN cd ~/mcrl2-build \
 && cmake -DMCRL2_ENABLE_EXPERIMENTAL=ON \
          -DMCRL2_ENABLE_TESTS=ON . \
 && make -j${THREADS} \
 && ctest . -j${THREADS} 

# 7. Build the documentation
RUN . ~/mcrl2-venv/bin/activate \
 && cd ~/mcrl2-build && make doxygen && make doc 