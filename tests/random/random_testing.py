#!/usr/bin/env python

#~ Copyright 2015 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
sys.path += [os.path.join(os.path.dirname(__file__), '..', 'python')]

from random_bes_generator import make_bes
from random_pbes_generator import make_pbes
from random_process_generator import make_process_specification, generator_map, make_action, make_delta, make_tau, \
    make_process_instance, make_sum, make_if_then, make_if_then_else, make_choice, make_seq
from text_utility import write_text
from testing import run_yml_test, run_pbes_test_with_counter_example_minimization, cleanup_files

MCRL2_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
MCRL2_INSTALL_DIR = os.path.join(MCRL2_ROOT, 'stage', 'bin')

lpsconfcheck_generator_map = {
    make_action: 8,
    make_delta: 1,
    make_tau: 1,
    make_process_instance: 1,
    make_sum: 0,
    make_if_then: 0,
    make_if_then_else: 0,
    make_choice: 5,
    make_seq: 5,
}

def run_alphabet_test(source_path, name, settings):
    testfile = '{}/tests/specifications/{}'.format(source_path, 'alphabet.yml')
    actions = ['a', 'b', 'c', 'd']
    process_identifiers = ['P', 'Q', 'R']
    size = 10
    p = make_process_specification(generator_map, actions, process_identifiers, size)
    filename = '{0}.mcrl2'.format(name, settings)
    write_text(filename, str(p))
    inputfiles = [filename]
    result = run_yml_test(name, testfile, inputfiles, settings)
    cleanup_files(result, inputfiles, settings)

def run_lpssuminst_test(source_path, name, settings):
    testfile = '{}/tests/specifications/{}'.format(source_path, 'lpssuminst.yml')
    actions = ['a', 'b', 'c', 'd']
    process_identifiers = ['P', 'Q', 'R']
    size = 10
    p = make_process_specification(generator_map, actions, process_identifiers, size, generate_process_parameters = True)
    filename = '{0}.mcrl2'.format(name, settings)
    write_text(filename, str(p))
    inputfiles = [filename]
    result = run_yml_test(name, testfile, inputfiles, settings)
    cleanup_files(result, inputfiles, settings)

def run_lpssumelm_test(source_path, name, settings):
    testfile = '{}/tests/specifications/{}'.format(source_path, 'lpssumelm.yml')
    actions = ['a', 'b', 'c', 'd']
    process_identifiers = ['P', 'Q', 'R']
    size = 10
    p = make_process_specification(generator_map, actions, process_identifiers, size, generate_process_parameters = True)
    filename = '{0}.mcrl2'.format(name, settings)
    write_text(filename, str(p))
    inputfiles = [filename]
    result = run_yml_test(name, testfile, inputfiles, settings)
    cleanup_files(result, inputfiles, settings)

def run_lpsparelm_test(source_path, name, settings):
    testfile = '{}/tests/specifications/{}'.format(source_path, 'lpsparelm.yml')
    actions = ['a', 'b', 'c', 'd']
    process_identifiers = ['P', 'Q', 'R']
    size = 10
    p = make_process_specification(generator_map, actions, process_identifiers, size, generate_process_parameters = True)
    filename = '{0}.mcrl2'.format(name, settings)
    write_text(filename, str(p))
    inputfiles = [filename]
    result = run_yml_test(name, testfile, inputfiles, settings)
    cleanup_files(result, inputfiles, settings)

def run_lpsconfcheck(source_path, name, settings, testfile):
    testfile = '{}/tests/specifications/{}'.format(source_path, testfile)
    actions = ['a', 'b', 'c']
    process_identifiers = ['P', 'Q', 'R']
    size = 13
    p = make_process_specification(lpsconfcheck_generator_map, actions, process_identifiers, size, init='hide({a}, allow({a, b, c}, P || Q || R))')
    filename = '{0}.mcrl2'.format(name, settings)
    write_text(filename, str(p))
    inputfiles = [filename]
    result = run_yml_test(name, testfile, inputfiles, settings)
    cleanup_files(result, inputfiles, settings)

def run_lpsconfcheck_c_test(source_path, name, settings):
    run_lpsconfcheck(source_path, name, settings, 'lpsconfcheck_c.yml')

def run_lpsconfcheck_capital_c_test(source_path, name, settings):
    run_lpsconfcheck(source_path, name, settings, 'lpsconfcheck_capital_c.yml')

def run_lpsconfcheck_d_test(source_path, name, settings):
    run_lpsconfcheck(source_path, name, settings, 'lpsconfcheck_d.yml')

def run_lpsconfcheck_t_test(source_path, name, settings):
    run_lpsconfcheck(source_path, name, settings, 'lpsconfcheck_t.yml')

def run_lpsconfcheck_z_test(source_path, name, settings):
    run_lpsconfcheck(source_path, name, settings, 'lpsconfcheck_z.yml')

def run_lpsconstelm_test(source_path, name, settings):
    testfile = '{}/tests/specifications/{}'.format(source_path, 'lpsconstelm.yml')
    actions = ['a', 'b', 'c', 'd']
    process_identifiers = ['P', 'Q', 'R']
    size = 13
    p = make_process_specification(generator_map, actions, process_identifiers, size, generate_process_parameters = True)
    filename = '{0}.mcrl2'.format(name, settings)
    write_text(filename, str(p))
    inputfiles = [filename]
    result = run_yml_test(name, testfile, inputfiles, settings)
    cleanup_files(result, inputfiles, settings)

def run_lpsbinary_test(source_path, name, settings):
    testfile = '{}/tests/specifications/{}'.format(source_path, 'lpsbinary.yml')
    actions = ['a', 'b', 'c', 'd']
    process_identifiers = ['P', 'Q', 'R']
    size = 10
    p = make_process_specification(generator_map, actions, process_identifiers, size, generate_process_parameters = True)
    filename = '{0}.mcrl2'.format(name, settings)
    write_text(filename, str(p))
    inputfiles = [filename]
    result = run_yml_test(name, testfile, inputfiles, settings)
    cleanup_files(result, inputfiles, settings)

def run_bessolve_test(source_path, name, settings):
    testfile = '{}/tests/specifications/{}'.format(source_path, 'bessolve.yml')
    filename = '{0}.txt'.format(name, settings)
    equation_count = 4
    term_size = 3
    p = make_bes(equation_count, term_size)
    write_text(filename, str(p))
    inputfiles = [filename]
    result = run_yml_test(name, testfile, inputfiles, settings)
    cleanup_files(result, inputfiles, settings)

def run_lps2pbes_test(source_path, name, settings):
    testfile = '{}/tests/specifications/{}'.format(source_path, 'lps2pbes.yml')
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
def run_pbesabsinthe_test(source_path, name, settings):
    testfile = '{}/tests/specifications/{}'.format(source_path, 'pbesabsinthe.yml')
    equation_count = 2
    atom_count = 2
    propvar_count = 2
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    settings = dict()
    abstraction = '-a{0}'.format(os.path.join(os.path.dirname(__file__), 'formulas', 'abstraction.txt'))
    settings['tools'] = {'t2': {'args': [abstraction]}, 't3': {'args': [abstraction]}}
    run_pbes_test_with_counter_example_minimization(name, testfile, p, settings)

def run_pbesabstract_test(source_path, name, settings):
    testfile = '{}/tests/specifications/{}'.format(source_path, 'pbesabstract.yml')
    equation_count = 2
    atom_count = 2
    propvar_count = 2
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(name, testfile, p, settings)

def run_pbesconstelm_test(source_path, name, settings):
    testfile = '{}/tests/specifications/{}'.format(source_path, 'pbesconstelm.yml')
    equation_count = 3
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(name, testfile, p, settings)

def run_pbesparelm_test(source_path, name, settings):
    testfile = '{}/tests/specifications/{}'.format(source_path, 'pbesparelm.yml')
    equation_count = 3
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(name, testfile, p, settings)

def run_pbespareqelm_test(source_path, name, settings):
    testfile = '{}/tests/specifications/{}'.format(source_path, 'pbespareqelm.yml')
    equation_count = 3
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(name, testfile, p, settings)

# N.B. does not work since the generated PBES is not in BQNF format!
def run_pbesrewr_test(source_path, name, rewriter, settings):
    name = '{0}_{1}'.format(name, rewriter)
    testfile = '{}/tests/specifications/{}'.format(source_path, 'pbesrewr-{0}.yml'.format(rewriter))
    equation_count = 3
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(name, testfile, p, settings)

def run_pbesinst_lazy_test(source_path, name, settings):
    testfile = '{}/tests/specifications/{}'.format(source_path, 'pbesinst_lazy.yml')
    equation_count = 3
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(name, testfile, p, settings)

def run_pbesinst_finite_test(source_path, name, settings):
    testfile = '{}/tests/specifications/{}'.format(source_path, 'pbesinst_finite.yml')
    equation_count = 3
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(name, testfile, p, settings)

# N.B does not work due to unknown expressions (F_or)
def run_symbolic_exploration_test(source_path, name, settings):
    testfile = '{}/tests/specifications/{}'.format(source_path, 'symbolic_exploration.yml')
    equation_count = 3
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(name, testfile, p, settings)

def run_pbessolve_test(source_path, name, settings):
    testfile = '{}/tests/specifications/{}'.format(source_path, 'pbessolve.yml')
    equation_count = 5
    atom_count = 4
    propvar_count = 3
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(name, testfile, p, settings)

def run_pbesstategraph_test(source_path, name, settings):
    testfile = '{}/tests/specifications/{}'.format(source_path, 'pbesstategraph.yml')
    equation_count = 4
    atom_count = 5
    propvar_count = 4
    use_quantifiers = True
    p = make_pbes(equation_count, atom_count, propvar_count, use_quantifiers)
    run_pbes_test_with_counter_example_minimization(name, testfile, p, settings)

if __name__ == '__main__':
    settings = {'toolpath': MCRL2_INSTALL_DIR, 'verbose': False, 'cleanup_files': False}
    testdir = 'output'
    if not os.path.exists(testdir):
        os.mkdir(testdir)
    os.chdir(testdir)
    source_path = MCRL2_ROOT

    run_lpsconfcheck_c_test(source_path, 'lpsconfcheck_c', settings)
    run_lpsconfcheck_capital_c_test(source_path, 'lpsconfcheck_capital_c', settings)
    run_lpsconfcheck_d_test(source_path, 'lpsconfcheck_d', settings)
    run_lpsconfcheck_t_test(source_path, 'lpsconfcheck_t', settings)
    run_lpsconfcheck_z_test(source_path, 'lpsconfcheck_z', settings)
    run_lps2pbes_test(source_path, 'lps2pbes', settings)
    run_pbesabstract_test(source_path, 'pbesabstract', settings)
    run_pbesconstelm_test(source_path, 'pbesconstelm', settings)
    run_pbesinst_finite_test(source_path, 'pbesinst_finite', settings)
    run_pbesinst_lazy_test(source_path, 'pbesinst_lazy', settings)
    run_pbesparelm_test(source_path, 'pbesparelm', settings)
    run_pbespareqelm_test(source_path, 'pbespareqelm', settings)
    run_pbesrewr_test(source_path, 'pbesrewr', 'simplify', settings)
    run_pbesrewr_test(source_path, 'pbesrewr', 'pfnf', settings)
    run_pbesrewr_test(source_path, 'pbesrewr', 'quantifier-all', settings)
    run_pbesrewr_test(source_path, 'pbesrewr', 'quantifier-finite', settings)
    run_pbesrewr_test(source_path, 'pbesrewr', 'quantifier-one-point', settings)
    run_pbessolve_test(source_path, 'pbessolve', settings)
    run_pbesstategraph_test(source_path, 'pbesstategraph', settings)
    run_alphabet_test(source_path, 'alphabet', settings)
    run_lpsbinary_test(source_path, 'lpsbinary', settings)
    run_lpsconstelm_test(source_path, 'lpsconstelm', settings)
    run_lpsparelm_test(source_path, 'lpsparelm', settings)
    run_lpssumelm_test(source_path, 'lpssumelm', settings)
    run_lpssuminst_test(source_path, 'lpssuminst', settings)
    run_bessolve_test(source_path, 'bessolve', settings)

    # run_pbesabsinthe_test(source_path, 'pbesabsinthe', settings)
    # run_pbesrewr_test(source_path, 'pbesrewr', 'bqnf-quantifier', settings)
    # run_symbolic_exploration_test(source_path, 'symbolic_exploration', settings)
