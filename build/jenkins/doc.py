import os
from util import *

os.chdir(builddir)

#
# Documentation
#
if label == 'ubuntu-amd64' and buildtype == 'Release':
  doc_command = ['make', 'doc']
  doc_result = call('Make', doc_command]
  
  if doc_result:
    log('Make returned ' + str(doc_result))

os.chdir(workspace)
