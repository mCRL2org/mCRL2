#!/usr/bin/env python3

import subprocess
import os

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

subprocess.run(['mcrl22lps', '-v', 'goback.mcrl2', 'goback.lps'], check=True)

subprocess.run(['lps2pbes', '-v', '-f', 'nodeadlock.mcf', 'goback.lps', 'goback.nodeadlock.pbes'], check=True)
subprocess.run(['pbes2bool', '-v', 'goback.nodeadlock.pbes'], check=True)