import os
import string
from path import *

SPEC = "pbes/spec1.mcrl2"

for file in path('pbes').files('*.form'):
    print '---', file, '---'
    print 'formula:', string.strip(file.text())
    print ''

    arg = "dist%sgenerate_lpe2pbes_result %s %s %s" % (os.sep, SPEC, file, "timed")
    #print 'result (timed)'
    os.system(arg)
    print ''

    arg = "dist%sgenerate_lpe2pbes_result %s %s %s" % (os.sep, SPEC, file, "untimed")
    #print 'result (untimed)'
    os.system(arg)
    print ''
