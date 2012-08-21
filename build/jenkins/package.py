import platform
from util import *

os.chdir(builddir)

#
# Package
#
cpack_options = []
if label in ["fedora-amd64"]:
  cpack_options = ["-G", "RPM"]
elif label in ["ubuntu-x86", "ubuntu-amd64"]:
  cpack_options = ["-G", "DEB"]
elif label in ["windows-x86", "windows-amd64"]:
  cpack_options = ["-G", "NSIS"]

cpack_command = ['cpack'] + cpack_options
cpack_result = call('CPack', cpack_command)
if cpack_result:
  log('CPack returned ' + str(cpack_result))

os.chdir(workspace)
