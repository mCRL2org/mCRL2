#!/usr/bin/env python
import os

def run(mcffile, lpssfile):
    pbesfile = '{}.{}.pbes'.format(lpsfile[:-4], mcffile[:-4])
    cmd1 = 'lps2pbes -f {} {} {}'.format(mcffile, lpsfile, pbesfile)
    cmd2 = 'pbes2bool {}'.format(pbesfile)
    os.system(cmd1)
    os.system(cmd2)

mcrl2file = '11073.mcrl2'
lpsfile = '11073.lps'
os.system('mcrl22lps {} {}'.format(mcrl2file, lpsfile))
run('data_can_be_communicated.mcf', lpsfile)
run('infinite_data_communication_is_possible.mcf', lpsfile)
run('nodeadlock.mcf', lpsfile)
run('no_inconsistent_operating_states.mcf', lpsfile)
run('no_successful_transmission_in_inconsistent_operating_states.mcf', lpsfile)
