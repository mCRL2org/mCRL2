#!/usr/bin/env python

# ~ Copyright 2011-2015 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
sys.path += [os.path.join(os.path.dirname(__file__), '..', 'python')]

from random_bes_generator import make_bes
from random_pbes_generator import make_pbes, CounterExampleMinimizer
from random_process_generator import make_process_specification, generator_map
from replay import run_replay
from text_utility import write_text
from testing import run_test, run_pbes_test, run_pbes_test_with_counter_example_minimization

MCRL2_ROOT = os.path.join(os.path.dirname(__file__), '..', '..')

def run_alphabet_test(name):
    testfile = '../specifications/alphabet.yml'
    actions = ['a', 'b', 'c', 'd']
    process_identifiers = ['P', 'Q', 'R']
    size = 10
    p = make_process_specification(generator_map, actions, process_identifiers, size)
    filename = '{0}.mcrl2'.format(name)
    write_text(filename, str(p))
    inputfiles = [filename]
    run_test(testfile, inputfiles)

def run_lpssuminst_test(name):
    testfile = '../specifications/lpssuminst.yml'
    actions = ['a', 'b', 'c', 'd']
    process_identifiers = ['P', 'Q', 'R']
    size = 10
    p = make_process_specification(generator_map, actions, process_identifiers, size)
    filename = '{0}.mcrl2'.format(name)
    write_text(filename, str(p))
    inputfiles = [filename]
    run_test(testfile, inputfiles)

def run_lpssumelm_test(name):
    testfile = '../specifications/lpssumelm.yml'
    actions = ['a', 'b', 'c', 'd']
    process_identifiers = ['P', 'Q', 'R']
    size = 10
    p = make_process_specification(generator_map, actions, process_identifiers, size)
    filename = '{0}.mcrl2'.format(name)
    write_text(filename, str(p))
    inputfiles = [filename]
    run_test(testfile, inputfiles)

def run_lpsparelm_test(name):
    testfile = '../specifications/lpsparelm.yml'
    actions = ['a', 'b', 'c', 'd']
    process_identifiers = ['P', 'Q', 'R']
    size = 10
    p = make_process_specification(generator_map, actions, process_identifiers, size)
    filename = '{0}.mcrl2'.format(name)
    write_text(filename, str(p))
    inputfiles = [filename]
    run_test(testfile, inputfiles)

def run_lpsconstelm_test(name):
    testfile = '../specifications/lpsconstelm.yml'
    actions = ['a', 'b', 'c', 'd']
    process_identifiers = ['P', 'Q', 'R']
    size = 10
    p = make_process_specification(generator_map, actions, process_identifiers, size)
    filename = '{0}.mcrl2'.format(name)
    write_text(filename, str(p))
    inputfiles = [filename]
    run_test(testfile, inputfiles)

def run_lpsbinary_test(name):
    testfile = '../specifications/lpsbinary.yml'
    actions = ['a', 'b', 'c', 'd']
    process_identifiers = ['P', 'Q', 'R']
    size = 10
    p = make_process_specification(generator_map, actions, process_identifiers, size)
    filename = '{0}.mcrl2'.format(name)
    write_text(filename, str(p))
    inputfiles = [filename]
    run_test(testfile, inputfiles)

def run_bessolve_test(name):
    testfile = '../specifications/bessolve.yml'
    filename = '{0}.txt'.format(name)
    equation_count = 4
    term_size = 3
    p = make_bes(equation_count, term_size)
    write_text(filename, str(p))
    inputfiles = [filename]
    run_test(testfile, inputfiles)

# TODO: find a more elegant way to pass the formula file to lps2pbes
def run_lps2pbes_deadlock_test(name):
    testfile = '../specifications/lps2pbes_deadlock.yml'
    actions = ['a', 'b', 'c', 'd']
    process_identifiers = ['P', 'Q', 'R']
    size = 10
    p = make_process_specification(generator_map, actions, process_identifiers, size)
    filename = '{0}.mcrl2'.format(name)
    write_text(filename, str(p))
    nodeadlock = '-f{0}'.format(os.path.join(MCRL2_ROOT, 'examples', 'modal-formulas', 'nodeadlock.mcf'))
    settings = dict()
    settings['tools'] = { 't2': { 'args': [nodeadlock] } }
    inputfiles = [filename]
    run_test(testfile, inputfiles, settings)

# N.B. does not work yet due to unusable abstraction map
def run_pbesabsinthe_test(name):
    testfile = '../specifications/pbesabsinthe.yml'
    equation_count = 2
    atom_count = 2
    propvar_count = 2
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    settings = dict()
    abstraction = '-a{0}'.format(os.path.join(os.path.dirname(__file__), 'formulas', 'abstraction.txt'))
    settings['tools'] = { 't2': { 'args': [abstraction] }, 't3': { 'args': [abstraction] } }
    run_pbes_test_with_counter_example_minimization(testfile, p, name, settings)

def run_pbesrewr_pfnf_test(name):
    testfile = '../specifications/pbesrewr_pfnf.yml'
    equation_count = 3
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(testfile, p, name)

def run_pbesrewr_simplify_test(name):
    testfile = '../specifications/pbesrewr_simplify.yml'
    equation_count = 3
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(testfile, p, name)

def run_pbesabstract_test(name):
    testfile = '../specifications/pbesabstract.yml'
    equation_count = 2
    atom_count = 2
    propvar_count = 2
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(testfile, p, name)

def run_pbesconstelm_test(name):
    testfile = '../specifications/pbesconstelm.yml'
    equation_count = 3
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(testfile, p, name)

def run_pbesparelm_test(name):
    testfile = '../specifications/pbesparelm.yml'
    equation_count = 3
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(testfile, p, name)

def run_pbespareqelm_test(name):
    testfile = '../specifications/pbespareqelm.yml'
    equation_count = 3
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(testfile, p, name)

# N.B. does not work since the generated PBES is not in BQNF format!
def run_pbesrewr_test(name, rewriter):
    name = '{0}_{1}'.format(name, rewriter)
    testfile = '../specifications/pbesrewr-{0}.yml'.format(rewriter)
    equation_count = 3
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(testfile, p, name)

def run_pbesinst_lazy_test(name):
    testfile = '../specifications/pbesinst_lazy.yml'
    equation_count = 3
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(testfile, p, name)

def run_pbesinst_finite_test(name):
    testfile = '../specifications/pbesinst_finite.yml'
    equation_count = 3
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(testfile, p, name)

# N.B does not work due to unknown expressions (F_or)
def run_symbolic_exploration_test(name):
    testfile = '../specifications/symbolic_exploration.yml'
    equation_count = 3
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(testfile, p, name)

def run_pbessolve_test(name):
    testfile = '../specifications/pbessolve.yml'
    equation_count = 5
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(testfile, p, name)

def run_pbesstategraph_test(name):
    testfile = '../specifications/pbesstategraph.yml'
    equation_count = 4
    atom_count = 5
    propvar_count = 4
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(testfile, p, name)

if __name__ == '__main__':
    run_lps2pbes_deadlock_test('lps2pbes_deadlock')
    # run_pbesabsinthe_test('pbesabsinthe')
    run_pbesabstract_test('pbesabstract')
    run_pbesconstelm_test('pbesconstelm')
    run_pbesinst_finite_test('pbesinst_finite')
    run_pbesinst_lazy_test('pbesinst_lazy')
    run_pbesparelm_test('pbesparelm')
    run_pbespareqelm_test('pbespareqelm')
    run_pbesrewr_test('pbesrewr', 'simplify')
    run_pbesrewr_test('pbesrewr', 'pfnf')
    run_pbesrewr_test('pbesrewr', 'quantifier-all')
    run_pbesrewr_test('pbesrewr', 'quantifier-finite')
    run_pbesrewr_test('pbesrewr', 'quantifier-one-point')
    # run_pbesrewr_test('pbesrewr', 'bqnf-quantifier')
    run_pbessolve_test('pbessolve')
    run_pbesstategraph_test('pbesstategraph')

    run_alphabet_test('alphabet')
    run_lpsbinary_test('lpsbinary')
    run_lpsparelm_test('lpsconstelm')
    run_lpsparelm_test('lpsparelm')
    run_lpssumelm_test('lpssumelm')
    run_lpssuminst_test('lpssuminst')

    run_bessolve_test('bessolve')

    # run_symbolic_exploration_test('symbolic_exploration')
