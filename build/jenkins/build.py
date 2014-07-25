import os
import sys
import shutil
import platform
import multiprocessing

from util import *

#
# Print some info about the build
#
print "Building job {0} with label {1}, using compiler {2} and buildtype {3}".format(jobname, label, compiler, buildtype)

#
# Configuration axis: compiler
#
compilerflags = []
if compiler == 'clang':
  cc = which('clang')
  cpp = which('clang++')
elif compiler == 'gcc-oldest':
  cc = which('gcc-4.4')
  cpp = which('g++-4.4')
elif compiler == 'gcc-latest':
  cc = which('gcc-4.7')
  cpp = which('g++-4.7')
if compiler != 'default':
  if cc and cpp:
    compilerflags = ['-DCMAKE_C_COMPILER=' + cc, '-DCMAKE_CXX_COMPILER=' + cpp]
  else:
    log('Compiler not found: ' + compiler)
    sys.exit(1)

#
# Include experimental and deprecated tools when not building a redistributable
# package.
#
targetflags = []
if package not in ['nightly', 'official-release']:
  targetflags += ['-DMCRL2_ENABLE_EXPERIMENTAL=ON', '-DMCRL2_ENABLE_DEPRECATED=ON']
#
# Package for a proper mCRL2 release
#
packageflags = []
if package.startswith('official-release'):
  packageflags += ['-DMCRL2_PACKAGE_RELEASE=ON']

if label.startswith('windows'):
  packageflags += ['-DMSVC10_REDIST_DIR:PATH=C:\Program Files\Common Files\VC\Redist']
  
if label.startswith('macosx') and package in ['nightly', 'official-release']:
  packageflags += ['-DMCRL2_OSX_PACKAGE=ON'] # Needed for packaging relevant Boost headers (for compiling rewriter)

#
# Enable random tests for all builds.
#
testflags = ['-DMCRL2_ENABLE_RANDOM_TEST_TARGETS=ON']

#
# Do not run long tests if we're doing the ubuntu-amd64 clang maintainer 
# build (these time out because profiling is on in Clang).
#
if (label == 'ubuntu-amd64' and buildtype == 'Maintainer' and compiler == 'clang'):
  testflags += ['-DMCRL2_SKIP_LONG_TESTS=ON']

#
# For Windows, explicitly tell CMake which generator to use to avoid trouble
# with x86/x64 incompatibilities
#
generator = []
if label == 'windows-amd64':
  generator += ['-GNMake Makefiles']
elif label == 'windows-x86':
  generator += ['-GNMake Makefiles']

#
# If we are building the mCRL2-release job, run all tests
# Note that jobname is something like, i.e. including buildtype etc.
# mCRL2-release/buildtype=Maintainer,compiler=default,label=ubuntu-amd64
#
if jobname.split('/')[0].lower().find("release") <> -1:
  testflags += ['-DMCRL2_ENABLE_RELEASE_TEST_TARGETS=ON']

#
# Run CMake, take into account configuration axes.
#
if not os.path.exists(builddir):
  os.mkdir(builddir)
os.chdir(builddir)

# Using a stage dir breaks packaging for MacOSX. Ideally, we'd have a stage
# dir though, for easy debugging when something goes wrong during testing.
# To do so, re-add the following option:
#
# '-DMCRL2_STAGE_ROOTDIR={0}/stage'.format(builddir)

cmake_command = ['cmake', 
                 srcdir, 
                 '-DCMAKE_BUILD_TYPE={0}'.format(buildtype)] \
                + targetflags \
                + compilerflags \
                + testflags \
                + packageflags \
                + generator
if call('CMake', cmake_command):
  log('CMake failed.')
  sys.exit(1)

#
# Set appropriate number of threads to build/test with: the maximum amount for VMs,
# or 4 threads for physical machines (mcrl2build1/mcrl2build2).
#

if not buildthreads:
  buildthreads = multiprocessing.cpu_count()

#
# Build
#

extraoptions = []
if label in ["windows-x86", "windows-amd64"]:
  extraoptions = ['--config', buildtype]
else:
  extraoptions =  ['--', '-j{0}'.format(buildthreads)]
make_command = ['cmake', '--build', builddir] + extraoptions
if call('CMake --build', make_command):
  log('Build failed.')
  sys.exit(1)

#
# If we're only building documentation, then this is all we need to do here.
#
if package == 'official-release-doc':
  sys.exit(0)

#
# Test
#

ctest_command = ['ctest', \
                 # '-T', 'Test', \  # This seems to do something odd to the test dependencies
                 '--output-on-failure', \
                 '--no-compress-output', \
                 '-j{0}'.format(buildthreads)]
if label in ["windows-x86", "windows-amd64"]:
  ctest_command += ['--build-config', buildtype]
env = {}
env.update(os.environ)
env['MCRL2_COMPILEREWRITER'] = os.path.abspath(os.path.join('.', 'mcrl2compilerewriter_ctest'))
ctest_result = call('CTest', ctest_command, env=env)
if ctest_result:
  log('CTest returned ' + str(ctest_result))

#
# Copy test output to ${builddir}/ctest.xml, so the xUnit plugin can find
# it and transform it into a test report.
#

tagfile = '{0}/Testing/TAG'.format(builddir)
if os.path.exists(tagfile):
  dirname = open(tagfile).readline().strip()
  shutil.copyfile('{0}/Testing/{1}/Test.xml'.format(builddir, dirname), 
                  '{0}/ctest.xml'.format(builddir))
                  
os.chdir(workspace)
