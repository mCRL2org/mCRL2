#!/usr/bin/env python3

import sys
import subprocess
import os

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

if len(sys.argv) < 3:
    print('Usage: run.py <model> <property>')
    sys.exit(0)

model = sys.argv[1]
prop = sys.argv[2]

print(f'Verifying property {prop}.mcf for {model}.mcrl2')
subprocess.run(['mcrl22lps', '-v', '--timings', f'models/{model}.mcrl2', f'{model}.lps'], check=True)
subprocess.run(['lps2pbes', '-v', '--timings', '-o', 'pbes', '-f', f'properties/{prop}.mcf', f'{model}.lps', f'{model}.{prop}.pbes'], check=True)
subprocess.run(['pbessolvesymbolic', '-v', '--timings', '-Q0', '--memory-limit=128', '--cached', '--groups=used', '-rjittyc', '-s2', f'{model}.{prop}.pbes'], check=True)