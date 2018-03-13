#!/usr/bin/env python

#~ Copyright 2015 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
import os.path
import re
import sys
sys.path += [os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'python'))]

import random_state_formula_generator
from random_bes_generator import make_bes
from random_pbes_generator import make_pbes
import random_process_expression
from testing import run_pbes_test
from testcommand import YmlTest
from text_utility import write_text

MCRL2_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
MCRL2_INSTALL_DIR = os.path.join(MCRL2_ROOT, 'install', 'bin')

def ymlfile(file):
    return '{}/tests/specifications/{}.yml'.format(MCRL2_ROOT, file)

def mcrl2file(file):
    return os.path.join(MCRL2_ROOT, file)

class RandomTest(YmlTest):
    def __init__(self, name, ymlfile, settings = dict()):
        super(RandomTest, self).__init__(name, ymlfile, [], settings)

    # create input files for the random test, and add the filenames to self.inputfiles
    def create_inputfiles(self, runpath = '.'):
        raise NotImplemented

    # removes input files that are in the runpath directory
    def remove_inputfiles(self, runpath = '.'):
        for filename in self.inputfiles:
            if os.path.abspath(runpath) == os.path.abspath(os.path.dirname(filename)):
                os.remove(filename)

    def execute(self, runpath = '.'):
        self.create_inputfiles(runpath)
        super(RandomTest, self).execute(runpath)
        self.remove_inputfiles(runpath)

class ProcessTest(RandomTest):
    def __init__(self, name, ymlfile, settings = dict()):
        super(ProcessTest, self).__init__(name, ymlfile, settings)
        self.actions = ['a', 'b', 'c', 'd']
        self.process_identifiers = ['P', 'Q', 'R']
        self.process_size = 13
        self.parallel_operator_generators = random_process_expression.default_parallel_operator_generators
        self.process_expression_generators = random_process_expression.default_process_expression_generators
        self.init = None
        self.generate_process_parameters = False

    def create_inputfiles(self, runpath = '.'):
        filename = '{0}.mcrl2'.format(self.name, self.settings)
        p = random_process_expression.make_process_specification(self.parallel_operator_generators, self.process_expression_generators, self.actions, self.process_identifiers, self.process_size, self.init, self.generate_process_parameters)
        write_text(filename, str(p))
        self.inputfiles += [filename]

# generates random process with higher probability of tau transitions
class ProcessTauTest(ProcessTest):
    def __init__(self, name, testfile, settings = dict()):
        super(ProcessTauTest, self).__init__(name, testfile, settings)
        self.actions = ['a', 'b', 'c']
        self.init = 'hide({a}, allow({a, b, c}, P || Q || R))'
        self.process_expression_generators = {
                               random_process_expression.make_action: 8,
                               random_process_expression.make_delta: 1,
                               random_process_expression.make_tau: 4,
                               random_process_expression.make_process_instance: 1,
                               random_process_expression.make_sum: 0,
                               random_process_expression.make_if_then: 0,
                               random_process_expression.make_if_then_else: 0,
                               random_process_expression.make_choice: 5,
                               random_process_expression.make_seq: 5,
                               random_process_expression.make_multi_action : 1,
                             }

class AlphabetReduceTest(ProcessTest):
    def __init__(self, name, settings = dict()):
        super(AlphabetReduceTest, self).__init__(name, ymlfile('alphabet-reduce'), settings)
        self.actions = ['a', 'b', 'c', 'd', 'e']

class AlphabetComputeTest(ProcessTest):
    def __init__(self, name, settings = dict()):
        super(AlphabetComputeTest, self).__init__(name, ymlfile('alphabet-compute'), settings)

class LpsSuminstTest(ProcessTest):
    def __init__(self, name, settings = dict()):
        super(LpsSuminstTest, self).__init__(name, ymlfile('lpssuminst'), settings)

class LpsSumelmTest(ProcessTest):
    def __init__(self, name, settings = dict()):
        super(LpsSumelmTest, self).__init__(name, ymlfile('lpssumelm'), settings)

class LpsParelmTest(ProcessTest):
    def __init__(self, name, settings = dict()):
        super(LpsParelmTest, self).__init__(name, ymlfile('lpsparelm'), settings)
        self.generate_process_parameters = True

class LpsOnePointRuleRewriteTest(ProcessTest):
    def __init__(self, name, settings = dict()):
        super(LpsOnePointRuleRewriteTest, self).__init__(name, ymlfile('lpstransform'), settings)
        self.set_command_line_options('t2', ['-alps-one-point-rule-rewriter'])

class LpsConfcheckTest(ProcessTauTest):
    def __init__(self, name, confluence_type, settings = dict()):
        self.option_map = { 'commutative' : 'C',
                            'commutative-disjoint' : 'c',
                            'disjoint' : 'd',
                            'triangular' : 'T',
                            'trivial' : 'Z'
                          }
        assert confluence_type in self.option_map
        super(LpsConfcheckTest, self).__init__(name, ymlfile('lpsconfcheck'), settings)
        self.set_command_line_options('t2', ['-x' + self.option_map[confluence_type]])

class LtscompareTest(ProcessTauTest):
    def __init__(self, name, equivalence_type, settings = dict()):
        assert equivalence_type in ['bisim', 'bisim-gv', 'branching-bisim', 'branching-bisim-gv', 'dpbranching-bisim', 'dpbranching-bisim-gv', 'weak-bisim', 'dpweak-bisim', 'sim', 'ready-sim' , 'trace', 'weak-trace']
        super(LtscompareTest, self).__init__(name, ymlfile('ltscompare'), settings)
        self.set_command_line_options('t3', ['-e' + equivalence_type])
        self.set_command_line_options('t4', ['-e' + equivalence_type])

class BisimulationTest(ProcessTauTest):
    def __init__(self, name, equivalence_type, settings = dict()):
        assert equivalence_type in ['bisim', 'bisim-gv', 'branching-bisim', 'branching-bisim-gv', 'weak-bisim']
        super(BisimulationTest, self).__init__(name, ymlfile('bisimulation'), settings)
        self.set_command_line_options('t3', ['-e' + equivalence_type])
        self.set_command_line_options('t4', ['-e' + equivalence_type])
        if equivalence_type == 'branching-bisim-gv':
            self.set_command_line_options('t7', ['-bbranching-bisim'])
        elif equivalence_type in ['bisim', 'bisim-gv']:
            self.set_command_line_options('t7', ['-bstrong-bisim'])
        else:
            self.set_command_line_options('t7', ['-b' + equivalence_type])

class LpsConstelmTest(ProcessTest):
    def __init__(self, name, settings = dict()):
        super(LpsConstelmTest, self).__init__(name, ymlfile('lpsconstelm'), settings)
        self.generate_process_parameters = True

class LpsBinaryTest(ProcessTest):
    def __init__(self, name, settings = dict()):
        super(LpsBinaryTest, self).__init__(name, ymlfile('lpsbinary'), settings)
        self.generate_process_parameters = True

class Lps2pbesTest(ProcessTest):
    def __init__(self, name, settings = dict()):
        super(Lps2pbesTest, self).__init__(name, ymlfile('lps2pbes'), settings)

    def create_inputfiles(self, runpath = '.'):
        super(Lps2pbesTest, self).create_inputfiles(runpath)
        self.inputfiles.append(mcrl2file('examples/modal-formulas/nodeadlock.mcf'))

class Lts2pbesTest(ProcessTest):
    def __init__(self, name, settings = dict()):
        super(Lts2pbesTest, self).__init__(name, ymlfile('lts2pbes'), settings)

    def create_inputfiles(self, runpath = '.'):
        super(Lts2pbesTest, self).create_inputfiles(runpath)
        self.inputfiles.append(mcrl2file('examples/modal-formulas/nodeadlock.mcf'))

class PbesTest(RandomTest):
    def __init__(self, name, ymlfile, settings = dict()):
        super(PbesTest, self).__init__(name, ymlfile, settings)
        self.equation_count = 4
        self.atom_count = 4
        self.propvar_count = 3
        self.use_quantifiers = True

    def execute(self, runpath = '.'):
        p = make_pbes(self.equation_count, self.atom_count, self.propvar_count, self.use_quantifiers)
        run_pbes_test(self.name, self.ymlfile, p, self.settings)

# N.B. does not work yet due to unusable abstraction map
class PbesabsintheTest(PbesTest):
    def __init__(self, name, settings = dict()):
        super(PbesabsintheTest, self).__init__(name, ymlfile('pbesabsinthe'), settings)

# N.B. This test has been disabled, since the tool has been deprecated.
class PbesabstractTest(PbesTest):
    def __init__(self, name, settings = dict()):
        super(PbesabstractTest, self).__init__(name, ymlfile('pbesabstract'), settings)

class PbesconstelmTest(PbesTest):
    def __init__(self, name, settings = dict()):
        super(PbesconstelmTest, self).__init__(name, ymlfile('pbesconstelm'), settings)

class PbesparelmTest(PbesTest):
    def __init__(self, name, settings = dict()):
        super(PbesparelmTest, self).__init__(name, ymlfile('pbesparelm'), settings)

class PbespareqelmTest(PbesTest):
    def __init__(self, name, settings = dict()):
        super(PbespareqelmTest, self).__init__(name, ymlfile('pbespareqelm'), settings)

class PbesrewrTest(PbesTest):
    def __init__(self, name, rewriter, settings = dict()):
        super(PbesrewrTest, self).__init__(name, ymlfile('pbesrewr'), settings)
        self.set_command_line_options('t2', ['-p' + rewriter])

class PbestransformTest(PbesTest):
    def __init__(self, name, rewriter, settings = dict()):
        super(PbestransformTest, self).__init__(name, ymlfile('pbestransform'), settings)
        self.set_command_line_options('t2', ['-a' + rewriter])

class PbesinstTest(PbesTest):
    def __init__(self, name, options = ['-slazy'], settings = dict()):
        super(PbesinstTest, self).__init__(name, ymlfile('pbesinst'), settings)
        self.set_command_line_options('t2', options)

class PbessolveTest(PbesTest):
    def __init__(self, name, settings = dict()):
        super(PbessolveTest, self).__init__(name, ymlfile('pbessolve'), settings)

class PbesstategraphTest(PbesTest):
    def __init__(self, name, settings = dict()):
        super(PbesstategraphTest, self).__init__(name, ymlfile('pbesstategraph'), settings)

class Pbessolve_structure_graphTest(PbesTest):
    def __init__(self, name, settings = dict()):
        super(Pbessolve_structure_graphTest, self).__init__(name, ymlfile('pbessolve-structure-graph'), settings)

class Pbessolve_counter_exampleTest(ProcessTest):
    def __init__(self, name, settings = dict()):
        super(Pbessolve_counter_exampleTest, self).__init__(name, ymlfile('pbessolve-counter-example'), settings)

    def create_inputfiles(self, runpath = '.'):
        super(Pbessolve_counter_exampleTest, self).create_inputfiles(runpath)
        filename = '{0}.mcf'.format(self.name, self.settings)
        formula = random_state_formula_generator.make_modal_formula()
        write_text(filename, str(formula))
        self.inputfiles += [filename]

# N.B does not work due to unknown expressions (F_or)
class SymbolicExplorationTest(PbesTest):
    def __init__(self, name, settings = dict()):
        super(SymbolicExplorationTest, self).__init__(name, ymlfile('symbolic_exploration'), settings)

class BesTest(RandomTest):
    def __init__(self, name, ymlfile, settings = dict()):
        super(BesTest, self).__init__(name, ymlfile, settings)
        self.equation_count = 4
        self.term_size = 3

    def create_inputfiles(self, runpath = '.'):
        filename = '{0}.txt'.format(self.name, self.settings)
        p = make_bes(self.equation_count, self.term_size)
        write_text(filename, str(p))
        self.inputfiles += [filename]

class BessolveTest(BesTest):
    def __init__(self, name, settings = dict()):
        super(BessolveTest, self).__init__(name, ymlfile('bessolve'), settings)

available_tests = {
    'alphabet-compute'                            : lambda name, settings: AlphabetComputeTest(name, settings)                                     ,
    'alphabet-reduce'                             : lambda name, settings: AlphabetReduceTest(name, settings)                                      ,
    'lpssuminst'                                  : lambda name, settings: LpsSuminstTest(name, settings)                                          ,
    'lpssumelm'                                   : lambda name, settings: LpsSumelmTest(name, settings)                                           ,
    'lpsparelm'                                   : lambda name, settings: LpsParelmTest(name, settings)                                           ,
    'lps-quantifier-one-point'                    : lambda name, settings: LpsOnePointRuleRewriteTest(name, settings)                              ,
    'lpsconfcheck-commutative'                    : lambda name, settings: LpsConfcheckTest(name, 'commutative', settings)                         ,
    'lpsconfcheck-commutative-disjoint'           : lambda name, settings: LpsConfcheckTest(name, 'commutative-disjoint', settings)                ,
    'lpsconfcheck-disjoint'                       : lambda name, settings: LpsConfcheckTest(name, 'disjoint', settings)                            ,
    'lpsconfcheck-triangular'                     : lambda name, settings: LpsConfcheckTest(name, 'triangular', settings)                          ,
    'lpsconfcheck-trivial'                        : lambda name, settings: LpsConfcheckTest(name, 'trivial', settings)                             ,
    'lpsconstelm'                                 : lambda name, settings: LpsConstelmTest(name, settings)                                         ,
    'lpsbinary'                                   : lambda name, settings: LpsBinaryTest(name, settings)                                           ,
    'lps2pbes'                                    : lambda name, settings: Lps2pbesTest(name, settings)                                            ,
    'lts2pbes'                                    : lambda name, settings: Lts2pbesTest(name, settings)                                            ,
    'ltscompare-bisim'                            : lambda name, settings: LtscompareTest(name, 'bisim', settings)                                 ,
    'ltscompare-bisim-gv'                         : lambda name, settings: LtscompareTest(name, 'bisim-gv', settings)                              ,
    'ltscompare-branching-bisim'                  : lambda name, settings: LtscompareTest(name, 'branching-bisim', settings)                       ,
    'ltscompare-branching-bisim-gv'               : lambda name, settings: LtscompareTest(name, 'branching-bisim-gv', settings)                    ,
    'ltscompare-dpbranching-bisim'                : lambda name, settings: LtscompareTest(name, 'dpbranching-bisim', settings)                     ,
    'ltscompare-dpbranching-bisim-gv'             : lambda name, settings: LtscompareTest(name, 'dpbranching-bisim-gv', settings)                  ,
    'ltscompare-weak-bisim'                       : lambda name, settings: LtscompareTest(name, 'weak-bisim', settings)                            ,
    'ltscompare-dpweak-bisim'                     : lambda name, settings: LtscompareTest(name, 'dpweak-bisim', settings)                          ,
    'ltscompare-sim'                              : lambda name, settings: LtscompareTest(name, 'sim', settings)                                   ,
    'ltscompare-ready-sim'                        : lambda name, settings: LtscompareTest(name, 'ready-sim', settings)                             ,
    'ltscompare-trace'                            : lambda name, settings: LtscompareTest(name, 'trace', settings)                                 ,
    'ltscompare-weak-trace'                       : lambda name, settings: LtscompareTest(name, 'weak-trace', settings)                            ,
    'bisimulation-bisim'                          : lambda name, settings: BisimulationTest(name, 'bisim', settings)                               ,
    'bisimulation-bisim-gv'                       : lambda name, settings: BisimulationTest(name, 'bisim-gv', settings)                            ,
    'bisimulation-branching-bisim'                : lambda name, settings: BisimulationTest(name, 'branching-bisim', settings)                     ,
    'bisimulation-branching-bisim-gv'             : lambda name, settings: BisimulationTest(name, 'branching-bisim-gv', settings)                  ,
    'bisimulation-weak-bisim'                     : lambda name, settings: BisimulationTest(name, 'weak-bisim', settings)                          ,
    'pbesconstelm'                                : lambda name, settings: PbesconstelmTest(name, settings)                                        ,
    'pbesparelm'                                  : lambda name, settings: PbesparelmTest(name, settings)                                          ,
    'pbespareqelm'                                : lambda name, settings: PbespareqelmTest(name, settings)                                        ,
    'pbesrewr-simplify'                           : lambda name, settings: PbesrewrTest(name, 'simplify', settings)                                ,
    'pbesrewr-pfnf'                               : lambda name, settings: PbesrewrTest(name, 'pfnf', settings)                                    ,
    'pbesrewr-quantifier-all'                     : lambda name, settings: PbesrewrTest(name, 'quantifier-all', settings)                          ,
    'pbesrewr-quantifier-finite'                  : lambda name, settings: PbesrewrTest(name, 'quantifier-finite', settings)                       ,
    'pbesrewr-quantifier-inside'                  : lambda name, settings: PbesrewrTest(name, 'quantifier-inside', settings)                       ,
    'pbesrewr-quantifier-one-point'               : lambda name, settings: PbesrewrTest(name, 'quantifier-one-point', settings)                    ,
    'pbesrewr-data-rewriter'                      : lambda name, settings: PbestransformTest(name, 'pbes-data-rewriter', settings)                     ,
    'pbesrewr-simplify-rewriter'                  : lambda name, settings: PbestransformTest(name, 'pbes-simplify-rewriter', settings)                 ,
    'pbesrewr-simplify-data-rewriter'             : lambda name, settings: PbestransformTest(name, 'pbes-simplify-data-rewriter', settings)            ,
    'pbesrewr-simplify-quantifiers-rewriter'      : lambda name, settings: PbestransformTest(name, 'pbes-simplify-quantifiers-rewriter', settings)     ,
    'pbesrewr-simplify-quantifiers-data-rewriter' : lambda name, settings: PbestransformTest(name, 'pbes-simplify-quantifiers-data-rewriter', settings),
    'pbesinst-lazy'                               : lambda name, settings: PbestransformTest(name, 'pbesinst-lazy', settings),
    'pbesinst-on-the-fly'                         : lambda name, settings: PbestransformTest(name, 'pbesinst-on-the-fly', settings),
    'pbesinst-on-the-fly-with-fixed-points'       : lambda name, settings: PbestransformTest(name, 'pbesinst-on-the-fly-with-fixed-points', settings),
    'pbesinst-optimize'                           : lambda name, settings: PbestransformTest(name, 'pbesinst-optimize', settings),
    'pbesinst-lazy'                               : lambda name, settings: PbesinstTest(name, ['-slazy'], settings)                                ,
    'pbesinst-alternative_lazy'                   : lambda name, settings: PbesinstTest(name, ['-salternative-lazy'], settings)                    ,
    'pbesinst-finite'                             : lambda name, settings: PbesinstTest(name, ['-sfinite', '-f*(*:Bool)'], settings)               ,
    'pbessolve'                                   : lambda name, settings: PbessolveTest(name, settings)                                           ,
    'pbessolve-structure-graph'                   : lambda name, settings: Pbessolve_structure_graphTest(name, settings)                           ,
    'pbessolve-counter-example'                   : lambda name, settings: Pbessolve_counter_exampleTest(name, settings)                           ,
    'pbesstategraph'                              : lambda name, settings: PbesstategraphTest(name, settings)                                      ,
    'bessolve'                                    : lambda name, settings: BessolveTest(name, settings)                                            ,
}

def print_names(tests):
    print('--- available tests ---')
    for name in sorted(tests):
        print(name)

def main(tests):
    import argparse
    cmdline_parser = argparse.ArgumentParser()
    cmdline_parser.add_argument('-t', '--toolpath', dest='toolpath', help='The path where the mCRL2 tools are installed')
    cmdline_parser.add_argument('-r', '--repetitions', dest='repetitions', metavar='N', default='10', help='Perform N repetitions of each test')
    cmdline_parser.add_argument('-v', '--verbose', dest='verbose', action='store_true', help='Display additional progress messages.')
    cmdline_parser.add_argument('-k', '--keep-files', dest='keep_files', action='store_true', help='Keep the files produced by the test')
    cmdline_parser.add_argument('-n', '--names', dest='names', action='store_true', help='Print the names of the available tests')
    cmdline_parser.add_argument('-p', '--pattern', dest='pattern', metavar='P', default='.', action='store', help='Run the tests that match with pattern P')
    args = cmdline_parser.parse_args()
    if args.names:
        print_names(tests)
        return
    toolpath = args.toolpath
    if not toolpath:
        toolpath = MCRL2_INSTALL_DIR
    settings = {'toolpath': toolpath, 'verbose': args.verbose, 'cleanup_files': not args.keep_files, 'allow-non-zero-return-values': True}
    I = range(int(args.repetitions))

    testdir = 'output'
    if not os.path.exists(testdir):
        os.mkdir(testdir)
    os.chdir(testdir)

    for name in sorted(tests):
        if re.search(args.pattern, name):
            try:
                for i in I:
                    test = tests[name]('{}_{}'.format(name, i), settings)
                    test.execute_in_sandbox()
            except Exception as e:
                print(e)

if __name__ == '__main__':
    main(available_tests)
