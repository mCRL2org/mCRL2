#!/usr/bin/env python

#~ Copyright 2015 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
import os.path
import sys
sys.path += [os.path.join(os.path.dirname(__file__), '..', 'python')]

from random_bes_generator import make_bes
from random_pbes_generator import make_pbes
from random_process_generator import make_process_specification, generator_map, make_action, make_delta, make_tau, \
    make_process_instance, make_sum, make_if_then, make_if_then_else, make_choice, make_seq
from testing import run_pbes_test_with_counter_example_minimization
from testcommand import YmlTest
from text_utility import write_text

MCRL2_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
MCRL2_INSTALL_DIR = os.path.join(MCRL2_ROOT, 'stage', 'bin')

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
        self.generator_map = generator_map
        self.actions = ['a', 'b', 'c', 'd']
        self.process_identifiers = ['P', 'Q', 'R']
        self.process_size = 13
        self.init = None
        self.generate_process_parameters = False

    def create_inputfiles(self, runpath = '.'):
        filename = '{0}.mcrl2'.format(self.name, self.settings)
        p = make_process_specification(self.generator_map, self.actions, self.process_identifiers, self.process_size, init = self.init, generate_process_parameters = self.generate_process_parameters)
        write_text(filename, str(p))
        self.inputfiles += [filename]

# generates random process with higher probability of tau transitions
class ProcessTauTest(ProcessTest):
    def __init__(self, name, testfile, settings = dict()):
        super(ProcessTauTest, self).__init__(name, testfile, settings)
        self.actions = ['a', 'b', 'c']
        self.init = 'hide({a}, allow({a, b, c}, P || Q || R))'
        self.generator_map = {
                               make_action: 8,
                               make_delta: 1,
                               make_tau: 4,
                               make_process_instance: 1,
                               make_sum: 0,
                               make_if_then: 0,
                               make_if_then_else: 0,
                               make_choice: 5,
                               make_seq: 5,
                             }

class AlphabetTest(ProcessTest):
    def __init__(self, name, settings = dict()):
        super(AlphabetTest, self).__init__(name, ymlfile('alphabet'), settings)

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

class LpsConfcheckTest(ProcessTauTest):
    def __init__(self, name, confluence_type, settings = dict()):
        assert confluence_type in 'cdCTZ'
        super(LpsConfcheckTest, self).__init__(name, ymlfile('lpsconfcheck'), settings)
        self.set_command_line_options('t2', ['-x' + confluence_type])

class LtscompareTest(ProcessTauTest):
    def __init__(self, name, equivalence_type, settings = dict()):
        assert equivalence_type in ['bisim', 'branching-bisim', 'dpbranching-bisim', 'weak-bisim', 'dpweak-bisim', 'sim', 'trace', 'weak-trace']
        super(LtscompareTest, self).__init__(name + '_' + equivalence_type, ymlfile('ltscompare'), settings)
        self.set_command_line_options('t3', ['-e' + equivalence_type])
        self.set_command_line_options('t4', ['-e' + equivalence_type])

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

class PbesTest(RandomTest):
    def __init__(self, name, ymlfile, settings = dict()):
        super(PbesTest, self).__init__(name, ymlfile, settings)
        self.equation_count = 4
        self.atom_count = 4
        self.propvar_count = 3
        self.use_quantifiers = True

    def execute(self, runpath = '.'):
        p = make_pbes(self.equation_count, self.atom_count, self.propvar_count, self.use_quantifiers)
        run_pbes_test_with_counter_example_minimization(self.name, self.ymlfile, p, self.settings)

# N.B. does not work yet due to unusable abstraction map
class PbesabsintheTest(PbesTest):
    def __init__(self, name, settings = dict()):
        super(PbesabsintheTest, self).__init__(name, ymlfile('pbesabsinthe'), settings)

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
        super(PbesrewrTest, self).__init__(name + '_' + rewriter, ymlfile('pbesrewr'), settings)
        self.set_command_line_options('t2', ['-p' + rewriter])

class Pbesinst_lazyTest(PbesTest):
    def __init__(self, name, settings = dict()):
        super(Pbesinst_lazyTest, self).__init__(name, ymlfile('pbesinst_lazy'), settings)

class Pbesinst_finiteTest(PbesTest):
    def __init__(self, name, settings = dict()):
        super(Pbesinst_finiteTest, self).__init__(name, ymlfile('pbesinst_finite'), settings)

class PbessolveTest(PbesTest):
    def __init__(self, name, settings = dict()):
        super(PbessolveTest, self).__init__(name, ymlfile('pbessolve'), settings)

class PbesstategraphTest(PbesTest):
    def __init__(self, name, settings = dict()):
        super(PbesstategraphTest, self).__init__(name, ymlfile('pbesstategraph'), settings)

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

if __name__ == '__main__':
    settings = {'toolpath': MCRL2_INSTALL_DIR, 'verbose': False, 'cleanup_files': False}
    testdir = 'output'
    if not os.path.exists(testdir):
        os.mkdir(testdir)
    os.chdir(testdir)

    AlphabetTest('alphabet', settings).execute_in_sandbox()
    LpsSuminstTest('lpssuminst', settings).execute_in_sandbox()
    LpsSumelmTest('lpssumelm', settings).execute_in_sandbox()
    LpsParelmTest('lpsparelm', settings).execute_in_sandbox()
    LpsConfcheckTest('lpsconfcheck_c', 'c', settings).execute_in_sandbox()
    LpsConfcheckTest('lpsconfcheck_capital_c', 'C', settings).execute_in_sandbox()
    LpsConfcheckTest('lpsconfcheck_d', 'd', settings).execute_in_sandbox()
    LpsConfcheckTest('lpsconfcheck_t', 'T', settings).execute_in_sandbox()
    LpsConfcheckTest('lpsconfcheck_z', 'Z', settings).execute_in_sandbox()
    LpsConstelmTest('lpsconstelm', settings).execute_in_sandbox()
    LpsBinaryTest('lpsbinary', settings).execute_in_sandbox()
    Lps2pbesTest('lps2pbes', settings).execute_in_sandbox()
    LtscompareTest('ltscompare_bisim'            , 'bisim'            , settings).execute_in_sandbox()
    LtscompareTest('ltscompare_branching_bisim'  , 'branching-bisim'  , settings).execute_in_sandbox()
    LtscompareTest('ltscompare_dpbranching_bisim', 'dpbranching-bisim', settings).execute_in_sandbox()
    LtscompareTest('ltscompare_weak_bisim'       , 'weak-bisim'       , settings).execute_in_sandbox()
    LtscompareTest('ltscompare_dpweak_bisim'     , 'dpweak-bisim'     , settings).execute_in_sandbox()
    LtscompareTest('ltscompare_sim'              , 'sim'              , settings).execute_in_sandbox()
    LtscompareTest('ltscompare_trace'            , 'trace'            , settings).execute_in_sandbox()
    LtscompareTest('ltscompare_weak_trace'       , 'weak-trace'       , settings).execute_in_sandbox()
    PbesabstractTest('pbesabstract', settings).execute_in_sandbox()
    PbesconstelmTest('pbesconstelm', settings).execute_in_sandbox()
    PbesparelmTest('pbesparelm', settings).execute_in_sandbox()
    PbespareqelmTest('pbespareqelm', settings).execute_in_sandbox()
    PbesrewrTest('pbesrewr', 'simplify', settings).execute_in_sandbox()
    PbesrewrTest('pbesrewr', 'pfnf', settings).execute_in_sandbox()
    PbesrewrTest('pbesrewr', 'quantifier-all', settings).execute_in_sandbox()
    PbesrewrTest('pbesrewr', 'quantifier-finite', settings).execute_in_sandbox()
    PbesrewrTest('pbesrewr', 'quantifier-one-point', settings).execute_in_sandbox()
    Pbesinst_lazyTest('pbesinst_lazy', settings).execute_in_sandbox()
    Pbesinst_finiteTest('pbesinst_finite', settings).execute_in_sandbox()
    PbessolveTest('pbessolve', settings).execute_in_sandbox()
    PbesstategraphTest('pbesstategraph', settings).execute_in_sandbox()
    BessolveTest('bessolve', settings).execute_in_sandbox()

    # SymbolicExplorationTest('symbolic_exploration', settings).execute_in_sandbox()
    # PbesrewrTest('pbesrewr', 'bqnf-quantifier', settings).execute_in_sandbox()
    # PbesabstractTest('pbesabsinthe', settings).execute_in_sandbox()
