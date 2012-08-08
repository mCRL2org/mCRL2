import os
import shutil
import platform
import subprocess
import multiprocessing

workspace = os.environ['WORKSPACE']
buildthreads = os.environ['BUILD_THREADS'] if 'BUILD_THREADS' in os.environ else 0
buildtype = os.environ['buildtype']
compiler = os.environ['compiler']
label = os.environ['label']

def log(*lines):
  print '\n##\n## ' + '\n## '.join(lines) + '\n##\n'
  sys.stdout.flush()

def call(name, command, **kwargs):
  log('Running ' + name, ' '.join(command))
  return subprocess.call(command, **kwargs)

def which(name, flags=os.X_OK):
    """Search PATH for executable files with the given name.
   
    On newer versions of MS-Windows, the PATHEXT environment variable will be
    set to the list of file extensions for files considered executable. This
    will normally include things like ".EXE". This fuction will also find files
    with the given name ending with any of these extensions.

    On MS-Windows the only flag that has any meaning is os.F_OK. Any other
    flags will be ignored.
   
    @type name: C{str}
    @param name: The name for which to search.
   
    @type flags: C{int}
    @param flags: Arguments to L{os.access}.
   
    @rtype: C{list}
    @param: A list of the full paths to files found, in the
    order in which they were found.
    """
    result = None # []
    exts = filter(None, os.environ.get('PATHEXT', '').split(os.pathsep))
    path = os.environ.get('PATH', None)
    if path is None:
      return None # []
    for p in os.environ.get('PATH', '').split(os.pathsep):
      p = os.path.join(p, name)
      if os.access(p, flags):
        return p # result.append(p)
      for e in exts:
        pext = p + e
        if os.access(pext, flags):
          return pext # result.append(pext)
    return result

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
  testflags += ['-DCMAKE_CXX_FLAGS=-DMCRL2_SKIP_LONG_TESTS']

#
# For the time being, do not build the GUI tools on Windows
# TODO: Enable these tools
#
if (label == 'windows-amd64' or label == 'windows-x86'):
  compilerflags += ['-DMCRL2_ENABLE_GUI_TOOLS=OFF']

#
# Run CMake, take into account configuration axes.
#
cmake_command = ['cmake', \
                 '-DCMAKE_BUILD_TYPE={0}'.format(buildtype), \
                 '-DMCRL2_STAGE_ROOTDIR={0}/stage'.format(workspace)] \
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
make_command = ['cmake', '--build', workspace, '--'] + extraoptions
if call('CMake --build', make_command):
  log('Build failed.')
  sys.exit(1)

#
# Test
#

newenv = {'MCRL2_COMPILEREWRITER': '{0}/stage/bin/mcrl2compilerewriter'.format(workspace)}
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
# Copy test output to ${WORKSPACE}/ctest.xml, so the xUnit plugin can find
# it and transform it into a test report.
#

tagfile = '{0}/Testing/TAG'.format(workspace)
if os.path.exists(tagfile):
  dirname = open(tagfile).readline().strip()
  shutil.copyfile('{0}/Testing/{1}/Test.xml'.format(workspace, dirname), 
                  '{0}/ctest.xml'.format(workspace))
