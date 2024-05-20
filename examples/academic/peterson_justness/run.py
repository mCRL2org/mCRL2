#!/usr/bin/env python3

import subprocess
import os

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

subprocess.run(['mcrl22lps', 'mutex.mcrl2', 'mutex.lps'], check=True)
subprocess.run(['lps2pbes', '-f', 'justlive.mcf', 'mutex.lps', 'mutex.justlive.pbes'], check=True)
subprocess.run(['pbessolve', 'mutex.justlive.pbes'], check=True)
