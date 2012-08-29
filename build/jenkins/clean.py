import shutil
from util import *

#
# Clean up the build directory. Can be used to ensure a completely clean build.
#
if os.path.exists(builddir):
  shutil.rmtree(builddir)

