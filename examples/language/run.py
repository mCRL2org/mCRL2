#!/usr/bin/env python3

import subprocess
import os

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

for file in os.listdir('.'):
    if 'mcrl2' in file:
        name, _ = os.path.splitext(file)

        print(f'linearising {file}')
        subprocess.run(['mcrl22lps', file, f'{name}.lps'], check=True)