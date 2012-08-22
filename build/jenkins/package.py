import os
from util import *

os.chdir(builddir)

#
# Source distribution
#
if label in ["ubuntu-amd64"]:
  source_command = ['make', 'package_source']
  source_result = call('Source', source_command)
  if source_result:
    log('Make returned ' + str(source_result))

#
# Binary Package
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
