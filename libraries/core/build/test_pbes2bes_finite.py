#~ Copyright 2010, 2011 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

from optparse import OptionParser
from path import *
from random_pbes_generator import *
from mcrl2_tools import *

def test_pbes2bes_finite(p, filename):
    txtfile = filename + '.txt'
    path(txtfile).write_text('%s' % p)
    pbesfile1 = filename + '.pbes'
    pbesfile2 = filename + 'a.pbes'
    run_txt2pbes(txtfile, pbesfile1)
    if run_pbes2bes(pbesfile1, pbesfile2, strategy = 'finite', selection = '*(*:Bool)'):
        answer1 = run_pbes2bool(pbesfile1)
        answer2 = run_pbes2bool(pbesfile2)
        print filename, answer1, answer2   
        if answer1 == None or answer2 == None:
          return True
        return answer1 == answer2
    return True

def main():
    usage = "usage: %prog [options]"
    parser = OptionParser(usage)
    parser.add_option("-t", "--tooldir", dest="tooldir", help="the mCRL2 tools directory")
    parser.add_option("-i", "--iterations", type="int", dest="iterations", default="100", help="the number of tests that is performed")
    parser.add_option("-v", "--verbose", action="store_true", dest="verbose", help="print verbose output")
    parser.add_option("-l", "--leave-files", action="store_true", dest="leave_files", help="leave temporary files")
    (options, args) = parser.parse_args()

    set_mcrl2_tooldir(options.tooldir)
    mcrl2_tool_options.verbose = options.verbose

    try:
        equation_count = 2
        atom_count = 2
        propvar_count = 2
        use_quantifiers = True
        
        for i in range(options.iterations):
            filename = 'pbes2bes_finite_%d' % i
            p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
            if not test_pbes2bes_finite(p, filename):
                m = CounterExampleMinimizer(p, lambda x: test_pbes2bes(x, filename + '_minimize'), 'pbes2bes_finite')
                m.minimize()
                print 'Test %s.txt failed' % filename
                exit(1)
    finally:
        if not options.leave_files:
            remove_temporary_files()

if __name__ == '__main__':
    main()
