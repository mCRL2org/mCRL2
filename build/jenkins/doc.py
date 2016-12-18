import os
import sys
from util import *

#
# Documentation
#
if label == 'ubuntu-amd64' and buildtype == 'Release':
  os.chdir(builddir)
  doc_command = ['make', 'doc']
  doc_result = call('Make', doc_command)
  
  if doc_result:
    log('Make returned ' + str(doc_result))
  os.chdir(workspace)
  sys.exit(doc_result)
