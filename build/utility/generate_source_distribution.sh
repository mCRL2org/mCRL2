#!/bin/sh

# Purpose of this script is an easy and reliable way of creating a source
# distribution from a checked out copy of the SVN repository. Special care was
# taken to include the following behaviour
#
#  - the script does not need to do a `make distclean' or similar, but can run
#    on a non-clean source tree while a build is in progress, without interfering
#    with this build. This ensures that a daily build process can be performed
#    using the same source tree.
#  - the distribution is tagged with a revision number that is extracted out of
#    the currently checked-out source tree
#  - build tools specific for the SVN source tree can be left out of a source
#    distribution
#
# Finally take note that this script is not meant to be portable.

if [[ $# != 2 ]]; then
  echo "Usage: $0 <SVN-trunk> [ <target-directory> ]";
  exit 0;
fi

SOURCE_BASE="$1"
TARGET_BASE="$2"

# Assumes that the current working directory contains the trunk directory as checked out from the svn repository

if [[ ! -d ${SOURCE_BASE} ]]; then
  echo "Error:";
  echo "";
  echo " The first argument should be a directory containing";
  echo " the trunk directory of a checked out svn repository.";
  echo "";
  exit 0;
fi
if [[ ! -d ${TARGET_BASE} ]]; then
  echo "Error: <target-directory> must exist and be a directory!";
fi

source_files="mcrl2/articles \
              mcrl2/3rd-party \
              mcrl2/boost-build.jam \
              mcrl2/conventions \
              mcrl2/documentation \
              mcrl2/COPYING \
              mcrl2/INSTALL \
              mcrl2/README \
              mcrl2/configure \
              mcrl2/Jamroot.jam \
              mcrl2/build \
              mcrl2/Makefile \
              mcrl2/examples \
              mcrl2/tests \
              mcrl2/specs \
              mcrl2/.vim \
              mcrl2/src"

svn_update="$(svn update --non-interactive ${SOURCE_BASE} 2>&1 | tail -n 1)"

# Obtain revision number from checked out source tree
revision=$(expr "$(/usr/bin/svnversion ${SOURCE_BASE})" : "\([0-9]*\)")

# Determine archive name
archive_name="${TARGET_BASE}/mcrl2-${revision}.tar"

# The following works based on assumption that the source tree is configured
if (! test -e "${archive_name}.bz2"); then
  # Generate tarball for source distribution
  ln -sf ${SOURCE_BASE} mcrl2

  # Find binaries and other files with no extension that should be filtered out
  find -L mcrl2 -mindepth 2 -type f \
       -not -name '*.*' \
       -not -name 'PkgInfo' \
       -not -name 'Makefile' \
       -not -name 'doxyfile' \
       -not -name 'Jamfile' \
       -not -wholename '*/examples/*' \
       -not -wholename '*/build*' \
       -not -wholename '*/3rd-party*' > excludes

  # Find directories to filter out
  find -L mcrl2 -type d \
       -name 'html' \
       -or -name 'latex' \
       -or -wholename '*build/bin' \
       -or -wholename '*mcrl2/build/utility' \
       -or -wholename '*3rd-party/boost/tools/jam/bootstrap' >> excludes

  # Append revision number to template configuration file
  echo "REVISION        = \"${revision}\" ;" >> ${SOURCE_BASE}/build/config.jam.in

  tar cf ${archive_name} \
           --exclude '.svn' \
           --exclude '*.ps' \
           --exclude '*.dvi' \
           --exclude '*.log' \
           --exclude '*.pdf' \
           --exclude '*.d' \
           --exclude '*.dpp' \
           --exclude '*.o' \
           --exclude '*.a' \
           --exclude '*.so' \
           --exclude-from excludes ${source_files}

  svn revert ${SOURCE_BASE}/build/config.jam.in

  unlink mcrl2

  bzip2 -f ${archive_name}

  unlink excludes
fi

# Return archive base name
echo $(basename ${archive_name}.bz2)
