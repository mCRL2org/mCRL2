import os
import subprocess
import multiprocessing

workspace = os.environ['WORKSPACE']
buildthreads = os.environ['BUILD_THREADS'] if 'BUILD_THREADS' in os.environ else 0
buildtype = os.environ['buildtype']
compiler = os.environ['compiler']
label = os.environ['label']

#
# Configuration axis: compiler
#

compilerflags = []
if compiler == 'clang':
  compilerflags = ['-DCMAKE_C_COMPILER=clang', '-DCMAKE_CXX_COMPILER=clang++']
elif compiler == 'gcc-latest':
  compilerflags = ['-DCMAKE_C_COMPILER=gcc-4.7', '-DCMAKE_CXX_COMPILER=g++-4.7']

#
# Do not run long tests, unless we're doing the ubuntu-amd64 maintainer build
#
testflags = []
if not (label == 'ubuntu-amd64' and buildtype == 'Maintainer'):
  testflags = ['-DCMAKE_CXX_FLAGS=-DMCRL2_SKIP_LONG_TESTS']

#
# Run CMake, take into account configuration axes.
#
if subprocess.call(['cmake', 
                    '-DCMAKE_BUILD_TYPE={0}'.format(buildtype),
                    '-DMCRL2_STAGE_ROOTDIR={0}/stage'.format(workspace)] 
                    + compilerflags
                    + testflags):
  print 'CMake failed.'
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

if subprocess.check(['make', '-j{0}'.format(buildthreads)]):
  print 'Build failed.'
  sys.exit(1)

#
# Test
#

newenv = {'MCRL2_COMPILEREWRITER': '{0}/stage/bin/mcrl2compilerewriter'.format(workspace)}
newenv.update(os.environ)
ctest = subprocess.call(['ctest', '-T', 'Test', '--output-on-failure', '--no-compress-output', '-j{0}'.format(buildthreads)], env=newenv)
if ctest:
  print 'CTest returned {0}'.format(ctest)

#
# Copy test output to ${WORKSPACE}/ctest.xml, so the xUnit plugin can find
# it and transform it into a test report.
#

tagfile = '{0}/Testing/TAG'.format(workspace)
if os.path.exists(tagfile):
  dirname = open(tagfile).read()
  shutil.copyfile('{0}/Testing/{1}/Test.xml'.format(workspace, dirname), 
                  '{0}/ctest.xml'.format(workspace))
