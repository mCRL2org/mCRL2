#!/usr/bin/env python

# ~ Copyright 2011-2015 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
sys.path += [os.path.join(os.path.dirname(__file__), '..', 'python')]

MCRL2_ROOT = os.path.join(os.path.dirname(__file__), '..', '..')

from random_bes_generator import make_bes
from random_pbes_generator import make_pbes, CounterExampleMinimizer
from random_process_generator import make_process_specification, generator_map
from replay import run_replay
from mcrl2_tools import parse_command_line, remove_temporary_files
from text_utility import write_text

def run_test(testfile, inputfiles):
    reporterrors = True
    settings = dict()
    settings['toolpath'] = '../../stage/bin'
    settings['verbose'] = False
    settings['verbose'] = True
    result, msg = run_replay(testfile, inputfiles, reporterrors, settings)
    print ', '.join(inputfiles), result, msg
    return result

def run_pbes_test(testfile, p, name):
    filename = '{0}.txt'.format(name)
    with open(filename, 'w') as f:
        f.write(str(p))
    inputfiles = [filename]
    run_test(testfile, inputfiles)

def run_pbes_test_with_counter_example_minimization(testfile, p, name):
    result = run_pbes_test(testfile, p, name)
    if result == False:
        m = CounterExampleMinimizer(p, lambda x: run_pbes_test(testfile, x, name + '_minimize'), name)
        m.minimize()
        raise RuntimeError('Test {0} failed'.format(name))

def run_alphabet_test(name):
    testfile = 'tests/alphabet.yml'
    actions = ['a', 'b', 'c', 'd']
    process_identifiers = ['P', 'Q', 'R']
    size = 10
    p = make_process_specification(generator_map, actions, process_identifiers, size)
    filename = '{0}.mcrl2'.format(name)
    write_text(filename, str(p))
    inputfiles = [filename]
    run_test(testfile, inputfiles)

def run_bessolve_test(name):
    testfile = 'tests/bessolve.yml'
    filename = '{0}.txt'.format(name)
    equation_count = 4
    term_size = 3
    p = make_bes(equation_count, term_size)
    write_text(filename, str(p))
    inputfiles = [filename]
    run_test(testfile, inputfiles)

# N.B. does not work yet due to input files problem!
def run_lps2pbes_deadlock_test(name):
    testfile = 'tests/lps2pbes_deadlock.yml'
    actions = ['a', 'b', 'c', 'd']
    process_identifiers = ['P', 'Q', 'R']
    size = 10
    p = make_process_specification(generator_map, actions, process_identifiers, size)
    filename = '{0}.mcrl2'.format(name)
    write_text(filename, str(p))
    inputfiles = [filename, os.path.join(MCRL2_ROOT, 'examples', 'modal-formulas', 'nodeadlock.mcf')]
    run_test(testfile, inputfiles)

# N.B. does not work yet due to empty abstraction map problem!
def run_pbesabsinthe_test(name):
    testfile = 'tests/pbesabsinthe.yml'
    equation_count = 2
    atom_count = 2
    propvar_count = 2
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(testfile, p, name)

def run_pbesrewr_pfnf_test(name):
    testfile = 'tests/pbesrewr_pfnf.yml'
    equation_count = 3
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(testfile, p, name)

def run_pbesrewr_simplify_test(name):
    testfile = 'tests/pbesrewr_simplify.yml'
    equation_count = 3
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(testfile, p, name)

def run_pbesabstract_test(name):
    testfile = 'tests/pbesabstract.yml'
    equation_count = 2
    atom_count = 2
    propvar_count = 2
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(testfile, p, name)

def run_pbesconstelm_test(name):
    testfile = 'tests/pbesconstelm.yml'
    equation_count = 3
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(testfile, p, name)

def run_pbesparelm_test(name):
    testfile = 'tests/pbesparelm.yml'
    equation_count = 3
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(testfile, p, name)

def run_pbespareqelm_test(name):
    testfile = 'tests/pbespareqelm.yml'
    equation_count = 3
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(testfile, p, name)

# N.B. does not work since the generated PBES is not in BQNF format!
def run_pbesrewr_test(name, rewriter):
    name = '{0}_{1}'.format(name, rewriter)
    testfile = 'tests/pbesrewr-{0}.yml'.format(rewriter)
    equation_count = 3
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(testfile, p, name)

def run_pbesinst_lazy_test(name):
    testfile = 'tests/pbesinst_lazy.yml'
    equation_count = 3
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(testfile, p, name)

def run_pbesinst_finite_test(name):
    testfile = 'tests/pbesinst_finite.yml'
    equation_count = 3
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(testfile, p, name)

# N.B does not work due to unknown expressions (F_or)
def run_symbolic_exploration_test(name):
    testfile = 'tests/symbolic_exploration.yml'
    equation_count = 3
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(testfile, p, name)

def run_pbessolve_test(name):
    testfile = 'tests/pbessolve.yml'
    equation_count = 5
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(testfile, p, name)

def run_pbesstategraph_test(name):
    testfile = 'tests/pbesstategraph.yml'
    equation_count = 4
    atom_count = 5
    propvar_count = 4
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(testfile, p, name)

if __name__ == '__main__':
    run_lps2pbes_deadlock_test('lps2pbes_deadlock')
    raw_input('ha')

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
    run_pbessolve_test('pbessolve')
    run_pbesstategraph_test('pbesstategraph')

    run_alphabet_test('alphabet')

    run_bessolve_test('bessolve')

    #run_symbolic_exploration_test('symbolic_exploration')
    #run_pbesabsinthe_test('pbesabsinthe')
    #run_lps2pbes_deadlock_test('lps2pbes_deadlock')
    #run_pbesrewr_test('pbesrewr', 'bqnf-quantifier')
