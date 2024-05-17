#!/usr/bin/env python3

import subprocess
import os

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

subprocess.run(['mcrl22lps', '-v', 'bke.mcrl2', 'bke.lps'], check=True)
subprocess.run(['lps2pbes', '-v', '-f', 'secret_not_leaked.mcf', 'bke.lps', 'bke.secret_not_leaked.pbes'], check=True)
subprocess.run(['pbessolve', '-v', 'bke.secret_not_leaked.pbes'], check=True)
