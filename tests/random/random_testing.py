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
from text_utility import write_text
from testing import run_replay, run_yml_test, run_pbes_test, run_pbes_test_with_counter_example_minimization, cleanup_files

MCRL2_ROOT = os.path.join(os.path.dirname(__file__), '..', '..')
MCRL2_INSTALL_DIR = os.path.join(MCRL2_ROOT, 'stage', 'bin')

def run_alphabet_test(name, settings):
    testfile = '../specifications/alphabet.yml'
    actions = ['a', 'b', 'c', 'd']
    process_identifiers = ['P', 'Q', 'R']
    size = 10
    p = make_process_specification(generator_map, actions, process_identifiers, size)
    filename = '{0}.mcrl2'.format(name, settings)
    write_text(filename, str(p))
    inputfiles = [filename]
    result = run_yml_test(name, testfile, inputfiles, settings)
    cleanup_files(result, inputfiles, settings)

def run_lpssuminst_test(name, settings):
    testfile = '../specifications/lpssuminst.yml'
    actions = ['a', 'b', 'c', 'd']
    process_identifiers = ['P', 'Q', 'R']
    size = 10
    p = make_process_specification(generator_map, actions, process_identifiers, size)
    filename = '{0}.mcrl2'.format(name, settings)
    write_text(filename, str(p))
    inputfiles = [filename]
    result = run_yml_test(name, testfile, inputfiles, settings)
    cleanup_files(result, inputfiles, settings)

def run_lpssumelm_test(name, settings):
    testfile = '../specifications/lpssumelm.yml'
    actions = ['a', 'b', 'c', 'd']
    process_identifiers = ['P', 'Q', 'R']
    size = 10
    p = make_process_specification(generator_map, actions, process_identifiers, size)
    filename = '{0}.mcrl2'.format(name, settings)
    write_text(filename, str(p))
    inputfiles = [filename]
    result = run_yml_test(name, testfile, inputfiles, settings)
    cleanup_files(result, inputfiles, settings)

def run_lpsparelm_test(name, settings):
    testfile = '../specifications/lpsparelm.yml'
    actions = ['a', 'b', 'c', 'd']
    process_identifiers = ['P', 'Q', 'R']
    size = 10
    p = make_process_specification(generator_map, actions, process_identifiers, size)
    filename = '{0}.mcrl2'.format(name, settings)
    write_text(filename, str(p))
    inputfiles = [filename]
    result = run_yml_test(name, testfile, inputfiles, settings)
    cleanup_files(result, inputfiles, settings)

def run_lpsconstelm_test(name, settings):
    testfile = '../specifications/lpsconstelm.yml'
    actions = ['a', 'b', 'c', 'd']
    process_identifiers = ['P', 'Q', 'R']
    size = 10
    p = make_process_specification(generator_map, actions, process_identifiers, size)
    filename = '{0}.mcrl2'.format(name, settings)
    write_text(filename, str(p))
    inputfiles = [filename]
    result = run_yml_test(name, testfile, inputfiles, settings)
    cleanup_files(result, inputfiles, settings)

def run_lpsbinary_test(name, settings):
    testfile = '../specifications/lpsbinary.yml'
    actions = ['a', 'b', 'c', 'd']
    process_identifiers = ['P', 'Q', 'R']
    size = 10
    p = make_process_specification(generator_map, actions, process_identifiers, size)
    filename = '{0}.mcrl2'.format(name, settings)
    write_text(filename, str(p))
    inputfiles = [filename]
    result = run_yml_test(name, testfile, inputfiles, settings)
    cleanup_files(result, inputfiles, settings)

def run_bessolve_test(name, settings):
    testfile = '../specifications/bessolve.yml'
    filename = '{0}.txt'.format(name, settings)
    equation_count = 4
    term_size = 3
    p = make_bes(equation_count, term_size)
    write_text(filename, str(p))
    inputfiles = [filename]
    result = run_yml_test(name, testfile, inputfiles, settings)
    cleanup_files(result, inputfiles, settings)

def run_lps2pbes_test(name, settings):
    testfile = '../specifications/lps2pbes.yml'
    actions = ['a', 'b', 'c', 'd']
    process_identifiers = ['P', 'Q', 'R']
    size = 10
    p = make_process_specification(generator_map, actions, process_identifiers, size)
    filename = '{0}.mcrl2'.format(name, settings)
    write_text(filename, str(p))
    nodeadlock = os.path.join(MCRL2_ROOT, 'examples', 'modal-formulas', 'nodeadlock.mcf')
    inputfiles = [filename, nodeadlock]
    result = run_yml_test(name, testfile, inputfiles, settings)
    cleanup_files(result, [filename], settings)

# N.B. does not work yet due to unusable abstraction map
def run_pbesabsinthe_test(name, settings):
    testfile = '../specifications/pbesabsinthe.yml'
    equation_count = 2
    atom_count = 2
    propvar_count = 2
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    settings = dict()
    abstraction = '-a{0}'.format(os.path.join(os.path.dirname(__file__), 'formulas', 'abstraction.txt'))
    settings['tools'] = { 't2': { 'args': [abstraction] }, 't3': { 'args': [abstraction] } }
    run_pbes_test_with_counter_example_minimization(name, testfile, p, settings)

def run_pbesrewr_pfnf_test(name, settings):
    testfile = '../specifications/pbesrewr_pfnf.yml'
    equation_count = 3
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(name, testfile, p, settings)

def run_pbesrewr_simplify_test(name, settings):
    testfile = '../specifications/pbesrewr_simplify.yml'
    equation_count = 3
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(name, testfile, p, settings)

def run_pbesabstract_test(name, settings):
    testfile = '../specifications/pbesabstract.yml'
    equation_count = 2
    atom_count = 2
    propvar_count = 2
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(name, testfile, p, settings)

def run_pbesconstelm_test(name, settings):
    testfile = '../specifications/pbesconstelm.yml'
    equation_count = 3
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(name, testfile, p, settings)

def run_pbesparelm_test(name, settings):
    testfile = '../specifications/pbesparelm.yml'
    equation_count = 3
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(name, testfile, p, settings)

def run_pbespareqelm_test(name, settings):
    testfile = '../specifications/pbespareqelm.yml'
    equation_count = 3
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(name, testfile, p, settings)

# N.B. does not work since the generated PBES is not in BQNF format!
def run_pbesrewr_test(name, rewriter, settings):
    name = '{0}_{1}'.format(name, rewriter)
    testfile = '../specifications/pbesrewr-{0}.yml'.format(rewriter)
    equation_count = 3
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(name, testfile, p, settings)

def run_pbesinst_lazy_test(name, settings):
    testfile = '../specifications/pbesinst_lazy.yml'
    equation_count = 3
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(name, testfile, p, settings)

def run_pbesinst_finite_test(name, settings):
    testfile = '../specifications/pbesinst_finite.yml'
    equation_count = 3
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(name, testfile, p, settings)

# N.B does not work due to unknown expressions (F_or)
def run_symbolic_exploration_test(name, settings):
    testfile = '../specifications/symbolic_exploration.yml'
    equation_count = 3
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(name, testfile, p, settings)

def run_pbessolve_test(name, settings):
    testfile = '../specifications/pbessolve.yml'
    equation_count = 5
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(name, testfile, p, settings)

def run_pbesstategraph_test(name, settings):
    testfile = '../specifications/pbesstategraph.yml'
    equation_count = 4
    atom_count = 5
    propvar_count = 4
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(name, testfile, p, settings)

if __name__ == '__main__':
    settings = {'toolpath': MCRL2_INSTALL_DIR, 'verbose': False, 'cleanup_files': True }

    run_lps2pbes_test('lps2pbes', settings)
    # run_pbesabsinthe_test('pbesabsinthe', settings)
    run_pbesabstract_test('pbesabstract', settings)
    run_pbesconstelm_test('pbesconstelm', settings)
    run_pbesinst_finite_test('pbesinst_finite', settings)
    run_pbesinst_lazy_test('pbesinst_lazy', settings)
    run_pbesparelm_test('pbesparelm', settings)
    run_pbespareqelm_test('pbespareqelm', settings)
    run_pbesrewr_test('pbesrewr', 'simplify', settings)
    run_pbesrewr_test('pbesrewr', 'pfnf', settings)
    run_pbesrewr_test('pbesrewr', 'quantifier-all', settings)
    run_pbesrewr_test('pbesrewr', 'quantifier-finite', settings)
    run_pbesrewr_test('pbesrewr', 'quantifier-one-point', settings)
    # run_pbesrewr_test('pbesrewr', 'bqnf-quantifier', settings)
    run_pbessolve_test('pbessolve', settings)
    run_pbesstategraph_test('pbesstategraph', settings)

    run_alphabet_test('alphabet', settings)
    run_lpsbinary_test('lpsbinary', settings)
    run_lpsparelm_test('lpsconstelm', settings)
    run_lpsparelm_test('lpsparelm', settings)
    run_lpssumelm_test('lpssumelm', settings)
    run_lpssuminst_test('lpssuminst', settings)

    run_bessolve_test('bessolve', settings)
    # run_symbolic_exploration_test('symbolic_exploration', settings)
