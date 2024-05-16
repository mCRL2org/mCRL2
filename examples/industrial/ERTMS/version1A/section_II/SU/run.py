#!/usr/bin/env python3

import subprocess
import os

from sys import argv

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

run = subprocess.run(['mcrl22lps', '-vnb', 'ertms-hl3.mcrl2'], stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpssumelm', '-vc'], input=run.stdout, stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpsconstelm', '-v'], input=run.stdout, stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpsparelm', '-v', '-', 'ertms-hl3.lps'], input=run.stdout, stdout=subprocess.PIPE, check=True)

if '-rjittyc' in argv:
    subprocess.run(['lps2lts', '-v', '-rjittyc', '--cached', '--timings=lps2lts_times.txt', 'ertms-hl3.lps', 'ertms-hl3.mod.aut'], check=True)

    subprocess.run(['ltsconvert', '--timings=ltsconvert_times.txt', '--tau=break,split_train,enter,leave,extend_EoA,move,connect,disconnect', '-v', '-edpbranching-bisim', 'ertms-hl3.mod.aut', 'ertms-hl3.mod.min.aut'], check=True)
    subprocess.run(['ltsconvert', '-vl', 'ertms-hl3.lps', 'ertms-hl3.mod.min.aut', 'ertms-hl3.mod.min.lts'], check=True)
    
    print('Verifying strong determinacy')
    subprocess.run(['lts2pbes', '-vf', 'strong_determinacy.mcf', 'ertms-hl3.mod.min.lts', '-l', 'ertms-hl3.lps', 'ertms-hl3.mod.min.strong_determinacy.pbes'], check=True)
    subprocess.run(['pbessolve', '-v', '-s2', '--timings=pbessolve_strong_determinacy_times.txt', 'ertms-hl3.mod.min.strong_determinacy.pbes'], check=True)

    print('Verifying termination')
    subprocess.run(['lts2pbes', '-vf', 'termination.mcf', 'ertms-hl3.mod.min.lts', '-l', 'ertms-hl3.lps', 'ertms-hl3.mod.min.termination.pbes'], check=True)
    subprocess.run(['pbessolve', '-v', '-s2', '--timings=pbessolve_termination_times.txt', 'ertms-hl3.mod.min.termination.pbes'], check=True)

    print('Verifying deterministic stabilisation')
    subprocess.run(['lts2pbes', '-vf', 'deterministic_stabilisation.mcf', 'ertms-hl3.mod.min.lts', '-l', 'ertms-hl3.lps', 'ertms-hl3.mod.min.deterministic_stabilisation.pbes'], check=True)
    subprocess.run(['pbessolve', '-v', '-s2', '--timings=pbessolve_times.txt', 'ertms-hl3.mod.min.deterministic_stabilisation.pbes'], check=True)


run = subprocess.run(['mcrl22lps', '-vnb', 'ertms-hl3.announce.mcrl2'], stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpssumelm', '-vc'], input=run.stdout, stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpsconstelm', '-v'], input=run.stdout, stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpsparelm', '-v', '-', 'ertms-hl3.announce.lps'], input=run.stdout, stdout=subprocess.PIPE, check=True)

if '-rjittyc' in argv:
    subprocess.run(['lps2lts', '-v', '-rjittyc', '--cached', '--timings=lps2lts_announce_times.txt', 'ertms-hl3.announce.lps', 'ertms-hl3.announce.mod.aut'], check=True)

    subprocess.run(['ltsconvert', '--timings=ltsconvert_announce_times.txt', '-edpbranching-bisim', 'ertms-hl3.announce.mod.aut', 'ertms-hl3.announce.mod.min.aut'], check=True)
    subprocess.run(['ltsconvert', '-vl', 'ertms-hl3.lps', 'ertms-hl3.announce.mod.min.aut', 'ertms-hl3.announce.mod.min.lts'], check=True)
    
    print('Verifying no collision')
    subprocess.run(['lts2pbes', '-vf', 'no_collision.mcf', 'ertms-hl3.announce.mod.min.lts', '-l', 'ertms-hl3.lps', 'ertms-hl3.mod.min.no_collision.pbes'], check=True)
    subprocess.run(['pbessolve', '-v', '-s2', '--timings=pbessolve_times.announce.txt', 'ertms-hl3.mod.min.no_collision.pbes'], check=True)
