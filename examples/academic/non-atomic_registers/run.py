#!/usr/bin/env python3

import sys
import subprocess
import os

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

model = sys.argv[1]
property = sys.argv[2]

print(f'Verifying property {property}.mcf for {model}.mcrl2')
subprocess.run(['mcrl22lps', '-v', '--timings', f'models/{model}.mcrl2', f'{model}.lps'], check=True)
subprocess.run(['lps2pbes', '-v', '--timings', '-o', 'pbes', '-f', f'properties/{property}.mcf', f'{model}.lps', f'{model}.{property}.pbes'], check=True)
subprocess.run(['pbessolvesymbolic', '-v', '--timings', '-Q0', '--memory-limit=128', '--cached', '--groups=used', '-rjittyc', '-s2', f'{model}.{property}.pbes'], check=True)