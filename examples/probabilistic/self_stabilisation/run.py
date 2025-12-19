#!/usr/bin/env python3

import subprocess
import os

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

subprocess.run(['mcrl22lps', '-v', 'self_stabilisation.mcrl2', 'self_stabilisation.lps'], check=True)
subprocess.run(['lps2pres', '-v', '-m', '-f', 'stable.mcf',  'self_stabilisation.lps', 'self_stabilisation_stable.pres'], check=True)
subprocess.run(['pressolve', '-am', '-v', '-p30', 'self_stabilisation_stable.pres'], check=True)

