#!/usr/bin/env python3

import subprocess
import os

from shutil import which

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

model = 'MLV'
run = subprocess.run(['mcrl22lps', '-n', f'{model}.mcrl2'], stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpsconstelm'], input=run.stdout, stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpsparelm', '-', f'{model}.lps'], input=run.stdout, check=True)

lpsreach = which('lpsreach')
pbessolvesymbolic = which('pbessolvesymbolic')

if lpsreach is not None and pbessolvesymbolic is not None:
    subprocess.run([lpsreach, '-v', '--saturation', '--cached', '--chaining', '--groups=simple', f'{model}.lps'], check=True)

    for mcfpath in os.listdir('properties/'):
        if '.mcf' in mcfpath:
            prop, _ = os.path.splitext(mcfpath)

            print('=================')
            print(f' REQUIREMENT {prop}')
            print('=================')

            subprocess.run(['lps2pbes', '-v', '-f', f'properties/{mcfpath}', f'{model}.lps', f'{model}.{prop}.pbes'], check=True)
            subprocess.run([pbessolvesymbolic, '-v', '-c', '--saturation', '--cached', '--chaining', '--groups=simple', f'{model}.{prop}.pbes'], check=True)

            print('\n\n')
