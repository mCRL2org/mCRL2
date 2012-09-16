import os
import sys
import shutil
import platform
import multiprocessing

from util import *

#
# Try to remove CMake cache. We can wait the extra few minutes if it means we don't
# get weird errors all the time.
#
try:
  os.remove(os.path.join(builddir, 'CMakeCache.txt'))
except:
  pass

#
# Configuration axis: compiler
#

compilerflags = []
if compiler == 'clang':
  cc = which('clang')
  cpp = which('clang++')
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
# Do not run long tests, unless we're doing the ubuntu-amd64 maintainer build
#
testflags = []
if not (label == 'ubuntu-amd64' and buildtype == 'Maintainer'):
  testflags += ['-DMCRL2_SKIP_LONG_TESTS=ON']

#
# If we are building the mCRL2-release job, run all tests
#
if jobname == "mCRL2-release":
  testflags += ['-DMCRL2_ENABLE_RELEASE_TEST_TARGETS=ON']

#
# Run CMake, take into account configuration axes.
#
if not os.path.exists(builddir):
  os.mkdir(builddir)
os.chdir(builddir)
cmake_command = ['cmake', \
                 srcdir, \
                 '-DCMAKE_BUILD_TYPE={0}'.format(buildtype), \
                 '-DMCRL2_STAGE_ROOTDIR={0}/stage'.format(builddir)] \
                 + compilerflags \
                 + testflags
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
if platform.system() == 'Linux':
  extraoptions =  ['-j{0}'.format(buildthreads)]
make_command = ['cmake', '--build', builddir, '--'] + extraoptions
if call('CMake --build', make_command):
  log('Build failed.')
  sys.exit(1)

#
# Test
#

newenv = {'MCRL2_COMPILEREWRITER': '{0}/stage/bin/mcrl2compilerewriter'.format(builddir)}
newenv.update(os.environ)
ctest_command = ['ctest', \
                 '-T', 'Test', \
                 '--output-on-failure', \
                 '--no-compress-output', \
                 '-j{0}'.format(buildthreads)]
ctest_result = call('CTest', ctest_command, env=newenv)
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
