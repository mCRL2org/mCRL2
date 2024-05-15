#!/usr/bin/env python3

import subprocess
import os

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

for file in os.listdir('.'):
    name, _ = os.path.splitext(file)

    subprocess.run(['mcrl22lps', file, f'{name}.lps'], check=True)