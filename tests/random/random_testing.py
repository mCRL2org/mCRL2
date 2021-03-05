#!/usr/bin/env python

# Copyright 2015 Wieger Wesselink.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
import os.path
import random
import re
import sys
import traceback
sys.path += [os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'python'))]

import random_state_formula_generator
from random_bes_generator import make_bes
from random_pbes_generator import make_pbes
import random_process_expression
from testing import YmlTest
from text_utility import write_text

MCRL2_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
MCRL2_INSTALL_DIR = os.path.join(MCRL2_ROOT, 'install', 'bin')

def ymlfile(file):
    return '{}/tests/specifications/{}.yml'.format(MCRL2_ROOT, file)

def mcrl2file(file):
    return os.path.join(MCRL2_ROOT, file)

class RandomTest(YmlTest):
    def __init__(self, name, ymlfile, settings):
        super(RandomTest, self).__init__(name, ymlfile, [], settings)

    # create input files for the random test, and add the filenames to self.inputfiles
    def create_inputfiles(self, runpath = '.'):
        raise NotImplementedError

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
    def __init__(self, name, ymlfile, settings):
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

# generates stochastic random processes
class StochasticProcessTest(ProcessTest):
    def __init__(self, name, ymlfile, settings):
        super(StochasticProcessTest, self).__init__(name, ymlfile, settings)
        self.process_expression_generators = {
                               random_process_expression.make_action          : 8,
                               random_process_expression.make_delta           : 1,
                               random_process_expression.make_tau             : 1,
                               random_process_expression.make_process_instance: 2,
                               random_process_expression.make_sum             : 2,
                               random_process_expression.make_if_then         : 2,
                               random_process_expression.make_if_then_else    : 2,
                               random_process_expression.make_choice          : 5,
                               random_process_expression.make_seq             : 5,
                               random_process_expression.make_multi_action    : 1,
                               random_process_expression.make_dist            : 3,
                            }

# generates random process with higher probability of tau transitions
class ProcessTauTest(ProcessTest):
    def __init__(self, name, testfile, settings):
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
                               random_process_expression.make_multi_action: 1,
                               random_process_expression.make_dist: 0,
                             }

class AlphabetReduceTest(ProcessTest):
    def __init__(self, name, settings):
        super(AlphabetReduceTest, self).__init__(name, ymlfile('alphabet-reduce'), settings)
        self.actions = ['a', 'b', 'c', 'd', 'e']

class LpsSuminstTest(ProcessTest):
    def __init__(self, name, settings):
        super(LpsSuminstTest, self).__init__(name, ymlfile('lpssuminst'), settings)

class LpsSumelmTest(ProcessTest):
    def __init__(self, name, settings):
        super(LpsSumelmTest, self).__init__(name, ymlfile('lpssumelm'), settings)

class LpsParelmTest(ProcessTest):
    def __init__(self, name, settings):
        super(LpsParelmTest, self).__init__(name, ymlfile('lpsparelm'), settings)
        self.generate_process_parameters = True

class LpsOnePointRuleRewriteTest(ProcessTest):
    def __init__(self, name, settings):
        super(LpsOnePointRuleRewriteTest, self).__init__(name, ymlfile('lpstransform'), settings)
        self.add_command_line_options('t2', ['-alps-one-point-rule-rewriter'])

class LpsConfcheckTest(ProcessTauTest):
    def __init__(self, name, confluence_type, settings):
        self.option_map = { 'commutative' : 'C',
                            'commutative-disjoint' : 'c',
                            'disjoint' : 'd',
                            'triangular' : 'T',
                            'trivial' : 'Z'
                          }
        assert confluence_type in self.option_map
        super(LpsConfcheckTest, self).__init__(name, ymlfile('lpsconfcheck'), settings)
        self.add_command_line_options('t2', ['-x' + self.option_map[confluence_type]])

class LtscompareTest(ProcessTauTest):
    def __init__(self, name, equivalence_type, settings):
        assert equivalence_type in ['bisim', 'bisim-gv', 'bisim-gjkw', 'branching-bisim', 'branching-bisim-gv', 'branching-bisim-gjkw', 'dpbranching-bisim', 'dpbranching-bisim-gv', 'dpbranching-bisim-gjkw', 'weak-bisim', 'dpweak-bisim', 'sim', 'ready-sim' , 'trace', 'weak-trace']
        super(LtscompareTest, self).__init__(name, ymlfile('ltscompare'), settings)
        self.add_command_line_options('t3', ['-e' + equivalence_type])
        self.add_command_line_options('t4', ['-e' + equivalence_type])

class StochasticLtscompareTest(StochasticProcessTest):
    def __init__(self, name, settings):
        super(StochasticLtscompareTest, self).__init__(name, ymlfile('stochastic-ltscompare'), settings)

class BisimulationTest(ProcessTauTest):
    def __init__(self, name, equivalence_type, settings):
        assert equivalence_type in ['bisim', 'bisim-gv', 'bisim-gjkw', 'branching-bisim', 'branching-bisim-gv', 'branching-bisim-gjkw', 'weak-bisim']
        super(BisimulationTest, self).__init__(name, ymlfile('bisimulation'), settings)
        self.add_command_line_options('t3', ['-e' + equivalence_type])
        self.add_command_line_options('t4', ['-e' + equivalence_type])
        if equivalence_type in ['branching-bisim-gv', 'branching-bisim-gjkw']:
            self.add_command_line_options('t7', ['-bbranching-bisim'])
        elif equivalence_type in ['bisim', 'bisim-gv', 'bisim-gjkw']:
            self.add_command_line_options('t7', ['-bstrong-bisim'])
        else:
            self.add_command_line_options('t7', ['-b' + equivalence_type])

class Lps2ltsAlgorithmsTest(ProcessTauTest):
    def __init__(self, name, settings):
        super(Lps2ltsAlgorithmsTest, self).__init__(name, ymlfile('lps2lts-algorithms'), settings)
        # randomly choose an algorithm
        actions = random.choice(['a', 'a,b', 'a,b,c'])
        options = [random.choice(['--deadlock', '--divergence', '--nondeterminism', '--action={}'.format(actions)])]
        options = [random.choice(['--deadlock', '--nondeterminism', '--action={}'.format(actions)])]
        if 'divergence' in options[0]:
            tau_actions = random.choice(['', '', 'b', 'b,c'])
            if tau_actions:
                options.append('--tau={}'.format(tau_actions))
        self.add_command_line_options('t2', options)
        self.add_command_line_options('t3', options)

class LpsConstelmTest(ProcessTest):
    def __init__(self, name, settings):
        super(LpsConstelmTest, self).__init__(name, ymlfile('lpsconstelm'), settings)
        self.generate_process_parameters = True

class LpsBinaryTest(ProcessTest):
    def __init__(self, name, settings):
        super(LpsBinaryTest, self).__init__(name, ymlfile('lpsbinary'), settings)
        self.generate_process_parameters = True

class LpsstategraphTest(ProcessTest):
    def __init__(self, name, settings):
        super(LpsstategraphTest, self).__init__(name, ymlfile('lpsstategraph'), settings)
        self.generate_process_parameters = True

class Lps2pbesTest(ProcessTest):
    def __init__(self, name, settings):
        super(Lps2pbesTest, self).__init__(name, ymlfile('lps2pbes'), settings)

    def create_inputfiles(self, runpath = '.'):
        super(Lps2pbesTest, self).create_inputfiles(runpath)
        self.inputfiles.append(mcrl2file('examples/modal-formulas/nodeadlock.mcf'))

class Lts2pbesTest(ProcessTest):
    def __init__(self, name, settings):
        super(Lts2pbesTest, self).__init__(name, ymlfile('lts2pbes'), settings)

    def create_inputfiles(self, runpath = '.'):
        super(Lts2pbesTest, self).create_inputfiles(runpath)
        self.inputfiles.append(mcrl2file('examples/modal-formulas/nodeadlock.mcf'))

class PbesTest(RandomTest):
    def __init__(self, name, ymlfile, settings):
        super(PbesTest, self).__init__(name, ymlfile, settings)
        self.equation_count = 4
        self.atom_count = 4
        self.propvar_count = 3
        self.use_quantifiers = True
        self.use_integers = True

    def create_inputfiles(self, runpath = '.'):
        filename = '{0}.txt'.format(self.name)
        p = make_pbes(self.equation_count, self.atom_count, self.propvar_count, self.use_quantifiers, use_integers=self.use_integers)
        write_text(filename, str(p))
        self.inputfiles += [filename]

# N.B. does not work yet due to unusable abstraction map
class PbesabsintheTest(PbesTest):
    def __init__(self, name, settings):
        super(PbesabsintheTest, self).__init__(name, ymlfile('pbesabsinthe'), settings)

# N.B. This test has been disabled, since the tool has been deprecated.
class PbesabstractTest(PbesTest):
    def __init__(self, name, settings):
        super(PbesabstractTest, self).__init__(name, ymlfile('pbesabstract'), settings)

class PbesbddsolveTest(PbesTest):
    def __init__(self, name, settings):
        super(PbesbddsolveTest, self).__init__(name, ymlfile('pbesbddsolve'), settings)
        self.use_integers = False
        self.use_quantifiers = False

class PbesconstelmTest(PbesTest):
    def __init__(self, name, settings):
        super(PbesconstelmTest, self).__init__(name, ymlfile('pbesconstelm'), settings)

class PbesparelmTest(PbesTest):
    def __init__(self, name, settings):
        super(PbesparelmTest, self).__init__(name, ymlfile('pbesparelm'), settings)

class PbespareqelmTest(PbesTest):
    def __init__(self, name, settings):
        super(PbespareqelmTest, self).__init__(name, ymlfile('pbespareqelm'), settings)

class Pbespor1Test(PbesTest):
    def __init__(self, name, settings):
        super(Pbespor1Test, self).__init__(name, ymlfile('pbespor1'), settings)

class Pbespor2Test(ProcessTest):
    def __init__(self, name, settings):
        super(Pbespor2Test, self).__init__(name, ymlfile('pbespor2'), settings)

    def create_inputfiles(self, runpath = '.'):
        super(Pbespor2Test, self).create_inputfiles(runpath)
        filename = '{0}.mcf'.format(self.name, self.settings)
        formula = random_state_formula_generator.make_modal_formula()
        write_text(filename, str(formula))
        self.inputfiles += [filename]

class PbesrewrTest(PbesTest):
    def __init__(self, name, rewriter, settings):
        super(PbesrewrTest, self).__init__(name, ymlfile('pbesrewr'), settings)
        self.add_command_line_options('t2', ['-p' + rewriter])

class PbestransformTest(PbesTest):
    def __init__(self, name, rewriter, settings):
        super(PbestransformTest, self).__init__(name, ymlfile('pbestransform'), settings)
        self.add_command_line_options('t2', ['-a' + rewriter])

class PbesinstTest(PbesTest):
    def __init__(self, name, options, settings):
        super(PbesinstTest, self).__init__(name, ymlfile('pbesinst'), settings)
        self.add_command_line_options('t2', options)

class PbespgsolveTest(PbesTest):
    def __init__(self, name, settings):
        super(PbespgsolveTest, self).__init__(name, ymlfile('pbespgsolve'), settings)

class PbesstategraphTest(PbesTest):
    def __init__(self, name, settings):
        super(PbesstategraphTest, self).__init__(name, ymlfile('pbesstategraph'), settings)

class PbessymbolicbisimTest(PbesTest):
    def __init__(self, name, settings):
        super(PbessymbolicbisimTest, self).__init__(name, ymlfile('pbessymbolicbisim'), settings)

class PbessolvesymbolicTest(PbesTest):
    def __init__(self, name, settings):
        super(PbessolvesymbolicTest, self).__init__(name, ymlfile('pbessolvesymbolic'), settings)

class Pbes2boolTest(PbesTest):
    def __init__(self, name, settings):
        super(Pbes2boolTest, self).__init__(name, ymlfile('pbessolve'), settings)

class Pbes2boolDepthFirstTest(PbesTest):
    def __init__(self, name, settings):
        super(Pbes2boolDepthFirstTest, self).__init__(name, ymlfile('pbessolve'), settings)
        self.add_command_line_options('t2', ['-zdepth-first'])
        self.add_command_line_options('t3', ['-zdepth-first'])
        self.add_command_line_options('t4', ['-zdepth-first'])
        self.add_command_line_options('t5', ['-zdepth-first'])
        self.add_command_line_options('t6', ['-zdepth-first'])
        self.add_command_line_options('t7', ['-zdepth-first'])
        self.add_command_line_options('t8', ['-zdepth-first'])

class Pbes2bool_counter_exampleTest(ProcessTest):
    def __init__(self, name, optimization, settings):
        super(Pbes2bool_counter_exampleTest, self).__init__(name, ymlfile('pbessolve-counter-example'), settings)
        if optimization in [4, 5]:
            self.add_command_line_options('t3', ['-l{}'.format(optimization), '--aggressive', '--prune-todo-list'])
        else:
            self.add_command_line_options('t3', ['-l{}'.format(optimization), '--prune-todo-list'])

    def create_inputfiles(self, runpath = '.'):
        super(Pbes2bool_counter_exampleTest, self).create_inputfiles(runpath)
        filename = '{0}.mcf'.format(self.name, self.settings)
        formula = random_state_formula_generator.make_modal_formula()
        write_text(filename, str(formula))
        self.inputfiles += [filename]

class Pbes_unify_parametersTest(PbesTest):
    def __init__(self, name, settings):
        super(Pbes_unify_parametersTest, self).__init__(name, ymlfile('pbes-unify-parameters'), settings)

class Pbes_srfTest(PbesTest):
    def __init__(self, name, settings):
        super(Pbes_srfTest, self).__init__(name, ymlfile('pbes-srf'), settings)

# N.B does not work due to unknown expressions (F_or)
class SymbolicExplorationTest(PbesTest):
    def __init__(self, name, settings):
        super(SymbolicExplorationTest, self).__init__(name, ymlfile('symbolic_exploration'), settings)

class BesTest(RandomTest):
    def __init__(self, name, ymlfile, settings):
        super(BesTest, self).__init__(name, ymlfile, settings)
        self.equation_count = 4
        self.term_size = 3

    def create_inputfiles(self, runpath = '.'):
        filename = '{0}.txt'.format(self.name, self.settings)
        p = make_bes(self.equation_count, self.term_size)
        write_text(filename, str(p))
        self.inputfiles += [filename]

class BessolveTest(BesTest):
    def __init__(self, name, settings):
        super(BessolveTest, self).__init__(name, ymlfile('bessolve'), settings)

available_tests = {
    'alphabet-reduce'                             : lambda name, settings: AlphabetReduceTest(name, settings)                                          ,
    'lpssuminst'                                  : lambda name, settings: LpsSuminstTest(name, settings)                                              ,
    'lpssumelm'                                   : lambda name, settings: LpsSumelmTest(name, settings)                                               ,
    'lpsparelm'                                   : lambda name, settings: LpsParelmTest(name, settings)                                               ,
    'lps-quantifier-one-point'                    : lambda name, settings: LpsOnePointRuleRewriteTest(name, settings)                                  ,
    'lpsconfcheck-commutative'                    : lambda name, settings: LpsConfcheckTest(name, 'commutative', settings)                             ,
    'lpsconfcheck-commutative-disjoint'           : lambda name, settings: LpsConfcheckTest(name, 'commutative-disjoint', settings)                    ,
    'lpsconfcheck-disjoint'                       : lambda name, settings: LpsConfcheckTest(name, 'disjoint', settings)                                ,
    'lpsconfcheck-triangular'                     : lambda name, settings: LpsConfcheckTest(name, 'triangular', settings)                              ,
    'lpsconfcheck-trivial'                        : lambda name, settings: LpsConfcheckTest(name, 'trivial', settings)                                 ,
    'lpsconstelm'                                 : lambda name, settings: LpsConstelmTest(name, settings)                                             ,
    'lpsbinary'                                   : lambda name, settings: LpsBinaryTest(name, settings)                                               ,
    'lps2lts-algorithms'                          : lambda name, settings: Lps2ltsAlgorithmsTest(name, settings)                                       ,
    'lps2pbes'                                    : lambda name, settings: Lps2pbesTest(name, settings)                                                ,
    'lpsstategraph'                               : lambda name, settings: LpsstategraphTest(name, settings)                                           ,
    'lts2pbes'                                    : lambda name, settings: Lts2pbesTest(name, settings)                                                ,
    'ltscompare-bisim'                            : lambda name, settings: LtscompareTest(name, 'bisim', settings)                                     ,
    'ltscompare-bisim-gv'                         : lambda name, settings: LtscompareTest(name, 'bisim-gv', settings)                                  ,
    'ltscompare-bisim-gjkw'                       : lambda name, settings: LtscompareTest(name, 'bisim-gjkw', settings)                                ,
    'ltscompare-branching-bisim'                  : lambda name, settings: LtscompareTest(name, 'branching-bisim', settings)                           ,
    'ltscompare-branching-bisim-gv'               : lambda name, settings: LtscompareTest(name, 'branching-bisim-gv', settings)                        ,
    'ltscompare-branching-bisim-gjkw'             : lambda name, settings: LtscompareTest(name, 'branching-bisim-gjkw', settings)                      ,
    'ltscompare-dpbranching-bisim'                : lambda name, settings: LtscompareTest(name, 'dpbranching-bisim', settings)                         ,
    'ltscompare-dpbranching-bisim-gv'             : lambda name, settings: LtscompareTest(name, 'dpbranching-bisim-gv', settings)                      ,
    'ltscompare-dpbranching-bisim-gjkw'           : lambda name, settings: LtscompareTest(name, 'dpbranching-bisim-gjkw', settings)                    ,
    'ltscompare-weak-bisim'                       : lambda name, settings: LtscompareTest(name, 'weak-bisim', settings)                                ,
    'ltscompare-dpweak-bisim'                     : lambda name, settings: LtscompareTest(name, 'dpweak-bisim', settings)                              ,
    'ltscompare-sim'                              : lambda name, settings: LtscompareTest(name, 'sim', settings)                                       ,
    'ltscompare-ready-sim'                        : lambda name, settings: LtscompareTest(name, 'ready-sim', settings)                                 ,
    'ltscompare-trace'                            : lambda name, settings: LtscompareTest(name, 'trace', settings)                                     ,
    'ltscompare-weak-trace'                       : lambda name, settings: LtscompareTest(name, 'weak-trace', settings)                                ,
    'bisimulation-bisim'                          : lambda name, settings: BisimulationTest(name, 'bisim', settings)                                   ,
    'bisimulation-bisim-gv'                       : lambda name, settings: BisimulationTest(name, 'bisim-gv', settings)                                ,
    'bisimulation-bisim-gjkw'                     : lambda name, settings: BisimulationTest(name, 'bisim-gjkw', settings)                              ,
    'bisimulation-branching-bisim'                : lambda name, settings: BisimulationTest(name, 'branching-bisim', settings)                         ,
    'bisimulation-branching-bisim-gv'             : lambda name, settings: BisimulationTest(name, 'branching-bisim-gv', settings)                      ,
    'bisimulation-branching-bisim-gjkw'           : lambda name, settings: BisimulationTest(name, 'branching-bisim-gjkw', settings)                    ,
    'bisimulation-weak-bisim'                     : lambda name, settings: BisimulationTest(name, 'weak-bisim', settings)                              ,
    'pbesconstelm'                                : lambda name, settings: PbesconstelmTest(name, settings)                                            ,
    'pbesparelm'                                  : lambda name, settings: PbesparelmTest(name, settings)                                              ,
    'pbespareqelm'                                : lambda name, settings: PbespareqelmTest(name, settings)                                            ,
    'pbespor2'                                    : lambda name, settings: Pbespor2Test(name, settings)                                                ,
    'pbesrewr-simplify'                           : lambda name, settings: PbesrewrTest(name, 'simplify', settings)                                    ,
    'pbesrewr-pfnf'                               : lambda name, settings: PbesrewrTest(name, 'pfnf', settings)                                        ,
    'pbesrewr-quantifier-all'                     : lambda name, settings: PbesrewrTest(name, 'quantifier-all', settings)                              ,
    'pbesrewr-quantifier-finite'                  : lambda name, settings: PbesrewrTest(name, 'quantifier-finite', settings)                           ,
    'pbesrewr-quantifier-inside'                  : lambda name, settings: PbesrewrTest(name, 'quantifier-inside', settings)                           ,
    'pbesrewr-quantifier-one-point'               : lambda name, settings: PbesrewrTest(name, 'quantifier-one-point', settings)                        ,
    'pbesrewr-data-rewriter'                      : lambda name, settings: PbestransformTest(name, 'pbes-data-rewriter', settings)                     ,
    'pbesrewr-simplify-rewriter'                  : lambda name, settings: PbestransformTest(name, 'pbes-simplify-rewriter', settings)                 ,
    'pbesrewr-simplify-data-rewriter'             : lambda name, settings: PbestransformTest(name, 'pbes-simplify-data-rewriter', settings)            ,
    'pbesrewr-simplify-quantifiers-rewriter'      : lambda name, settings: PbestransformTest(name, 'pbes-simplify-quantifiers-rewriter', settings)     ,
    'pbesrewr-simplify-quantifiers-data-rewriter' : lambda name, settings: PbestransformTest(name, 'pbes-simplify-quantifiers-data-rewriter', settings),
    'pbesinst-lazy'                               : lambda name, settings: PbesinstTest(name, ['-slazy'], settings)                                    ,
    'pbesinst-alternative_lazy'                   : lambda name, settings: PbesinstTest(name, ['-salternative-lazy'], settings)                        ,
    'pbesinst-finite'                             : lambda name, settings: PbesinstTest(name, ['-sfinite', '-f*(*:Bool)'], settings)                   ,
    'pbespgsolve'                                 : lambda name, settings: PbespgsolveTest(name, settings)                                             ,
    'pbessolve'                                   : lambda name, settings: Pbes2boolTest(name, settings)                                               ,
    'pbessolve-depth-first'                       : lambda name, settings: Pbes2boolDepthFirstTest(name, settings)                                     ,
    'pbessolve-counter-example-optimization-0'    : lambda name, settings: Pbes2bool_counter_exampleTest(name, 0, settings)                            ,
    'pbessolve-counter-example-optimization-1'    : lambda name, settings: Pbes2bool_counter_exampleTest(name, 1, settings)                            ,
    'pbessolve-counter-example-optimization-2'    : lambda name, settings: Pbes2bool_counter_exampleTest(name, 2, settings)                            ,
    'pbessolve-counter-example-optimization-3'    : lambda name, settings: Pbes2bool_counter_exampleTest(name, 3, settings)                            ,
    'pbessolve-counter-example-optimization-4'    : lambda name, settings: Pbes2bool_counter_exampleTest(name, 4, settings)                            ,
    'pbessolve-counter-example-optimization-5'    : lambda name, settings: Pbes2bool_counter_exampleTest(name, 5, settings)                            ,
    'pbessolve-counter-example-optimization-6'    : lambda name, settings: Pbes2bool_counter_exampleTest(name, 6, settings)                            ,
    'pbessolve-counter-example-optimization-7'    : lambda name, settings: Pbes2bool_counter_exampleTest(name, 7, settings)                            ,
    'pbesstategraph'                              : lambda name, settings: PbesstategraphTest(name, settings)                                          ,
    'pbes-unify-parameters'                       : lambda name, settings: Pbes_unify_parametersTest(name, settings)                                   ,
    'pbes-srf'                                    : lambda name, settings: Pbes_srfTest(name, settings)                                                ,
    # 'pbessymbolicbisim'                           : lambda name, settings: PbessymbolicbisimTest(name, settings)                                       , # excluded from the tests because of Z3 dependency
    'bessolve'                                    : lambda name, settings: BessolveTest(name, settings)                                                ,
    #'stochastic-ltscompare'                      : lambda name, settings: StochasticLtscompareTest(name, settings)                                     ,
}

# These test do not work on Windows due to dependencies.
if os.name != 'nt':
    available_tests.update({'pbessolvesymbolic' : lambda name, settings: PbessolvesymbolicTest(name, settings) })
#    available_tests.update({ 'pbesbddsolve' : lambda name, settings: PbesbddsolveTest(name, settings) })

def print_names(tests):
    for name in sorted(tests):
        print(name)

# Return all tests that match with pattern. In case of an exact match, only this exact match is returned.
def matching_tests(tests, pattern):
    matches = [name for name in sorted(tests) if re.search(pattern, name)]
    if pattern in matches:
        return [pattern]
    return matches

def main(tests):
    import argparse
    cmdline_parser = argparse.ArgumentParser()
    cmdline_parser.add_argument('-t', '--toolpath', dest='toolpath', help='The path where the mCRL2 tools are installed')
    cmdline_parser.add_argument('-r', '--repetitions', dest='repetitions', metavar='N', default='10', help='Perform N repetitions of each test')
    cmdline_parser.add_argument('-v', '--verbose', dest='verbose', action='store_true', help='Display additional progress messages.')
    cmdline_parser.add_argument('-k', '--keep-files', dest='keep_files', action='store_true', help='Keep the files produced by the test')
    cmdline_parser.add_argument('-n', '--names', dest='names', action='store_true', help='Print the names of the available tests')
    cmdline_parser.add_argument('-p', '--pattern', dest='pattern', metavar='P', default='.', action='store', help='Run the tests that match with pattern P')
    cmdline_parser.add_argument('-o', '--output', dest='output', metavar='o', action='store', help='Run the tests in the given directory')
    args = cmdline_parser.parse_args()
    if args.names:
        print_names(tests)
        return
    toolpath = args.toolpath
    if not toolpath:
        toolpath = MCRL2_INSTALL_DIR
    settings = {'toolpath': toolpath, 'verbose': args.verbose, 'cleanup_files': not args.keep_files, 'allow-non-zero-return-values': True}
    I = range(int(args.repetitions))

    if args.output:
        if not os.path.exists(args.output):
            os.mkdir(args.output)
        os.chdir(args.output)

    test_failed = False
    for name in matching_tests(tests, args.pattern):
        try:
            for i in I:
                test = tests[name]('{}_{}'.format(name, i), settings)
                test.execute_in_sandbox()
        except Exception as e:
            print('An exception occurred:', e.__class__, e)
            traceback.print_exc()
            test_failed = True

    if (test_failed):
      sys.exit(-1)

if __name__ == '__main__':
    main(available_tests)
