import os
import sys
import subprocess
import shutil
import glob

workspace = os.environ['WORKSPACE']
buildthreads = os.environ['BUILD_THREADS'] if 'BUILD_THREADS' in os.environ else 0
buildtype = os.environ['buildtype']
compiler = os.environ['compiler']
label = os.environ['label']
package = os.environ['package']
jobname = os.environ['JOB_NAME']
srcdir = "{0}/src".format(workspace)
builddir = "{0}/build".format(workspace)

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

def copy_files(src_glob, dst_folder):
  for filename in glob.iglob(src_glob):
    shutil.copy(filename, os.path.join(dst_folder, os.path.basename(filename)))
