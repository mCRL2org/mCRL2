#!/usr/bin/env python3

import subprocess
import os

from sys import argv

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

for spec in [
    '3_Ideal_trace.expanded.mcrl2',
    '3_Mute_follower.expanded.mcrl2',
    '3_Mute_leader.expanded.mcrl2',
    '3_Regular.expanded.mcrl2',
    '3_Regular.expanded.mcrl2',
]:
    name, _ = os.path.splitext(spec)

    subprocess.run(['mcrl22lps', spec, f'{name}.lps'], check=True)    

    for formula in [
        'eventually_comm.mcf',
        'eventually_startup.mcf',
        'nodeadlock.mcf'
    ]:
        formula_name, _ = os.path.splitext(formula)

        print(f'verifying {spec} and property {formula}')

        run = subprocess.run(['lps2pbes', '-f', 'mucalc/eventually_comm.mcf', f'{name}.lps'], stdout=subprocess.PIPE, check=True)
        run = subprocess.run(['pbesconstelm'], input=run.stdout, stdout=subprocess.PIPE, check=True)
        run = subprocess.run(['pbesparelm'], input=run.stdout, stdout=subprocess.PIPE, check=True)
        subprocess.run(['pbesrewr', '-', f'{formula_name}.pbes'], input=run.stdout, check=True)
        
        if '-rjittyc' in argv:
            subprocess.run(['pbessolve', '-v', f'{formula_name}.pbes'], check=True)