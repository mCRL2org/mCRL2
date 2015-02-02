#!/usr/bin/env python

#~ Copyright 2014 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
sys.path += [os.path.join(os.path.dirname(__file__), '..', 'python')]
from replay import run_replay

MCRL2_ROOT = '../../'

def run_ticket(testfile, inputfiles, msg = '', verbose = False, reporterrors = True, toolpath = MCRL2_ROOT + 'stage/bin'):
    settings = dict()
    settings['verbose'] = verbose
    settings['toolpath'] = toolpath
    print(msg)
    run_replay(testfile, inputfiles, reporterrors, settings)

run_ticket(MCRL2_ROOT + 'tests/specifications/mcrl22lps.yml', ['tickets/283/1.mcrl2'], 'ticket 325')
run_ticket(MCRL2_ROOT + 'tests/specifications/alphabet.yml', ['tickets/283/1.mcrl2'], 'ticket 283')
run_ticket(MCRL2_ROOT + 'tests/specifications/alphabet.yml', ['tickets/352/1.mcrl2'], 'ticket 352')
run_ticket(MCRL2_ROOT + 'tests/specifications/mcrl22lps.yml', ['tickets/397/1.mcrl2'], 'ticket 397')
run_ticket(MCRL2_ROOT + 'tests/specifications/lps2pbes.yml', ['tickets/1090/form.mcf', 'tickets/1090/spec.mcrl2'], 'ticket 1090')
run_ticket(MCRL2_ROOT + 'tests/specifications/alphabet.yml', ['tickets/1093/1.mcrl2'], 'ticket 1093')
run_ticket(MCRL2_ROOT + 'tests/specifications/txt2lps.yml', ['tickets/1122/1.mcrl2'], 'ticket 1122')
run_ticket(MCRL2_ROOT + 'tests/specifications/mcrl22lps.yml', ['tickets/1127/1.mcrl2'], 'ticket 1127')
run_ticket(MCRL2_ROOT + 'tests/specifications/pbesrewr-onepoint.yml', ['tickets/1143/1.txt'], 'ticket 1143')
run_ticket(MCRL2_ROOT + 'tests/specifications/lpsbisim2pbes.yml', ['tickets/1144/test1.txt', 'tickets/1144/test2.txt'], 'ticket 1144')
run_ticket(MCRL2_ROOT + 'tests/specifications/countstates.yml', [MCRL2_ROOT + 'examples/academic/abp/abp.mcrl2'], 'ticket 1167')
run_ticket(MCRL2_ROOT + 'tests/specifications/lpsbinary.yml', [MCRL2_ROOT + 'examples/academic/cabp/cabp.mcrl2'], 'ticket 1234')
run_ticket(MCRL2_ROOT + 'tests/specifications/alphabet.yml', ['tickets/1241/1.mcrl2'], 'ticket 1241')
run_ticket(MCRL2_ROOT + 'tests/specifications/alphabet.yml', ['tickets/1247/1.mcrl2'], 'ticket 1247')
run_ticket(MCRL2_ROOT + 'tests/specifications/alphabet.yml', ['tickets/1249/1.mcrl2'], 'ticket 1249')
