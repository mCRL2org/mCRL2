#!/usr/bin/env python3

import subprocess
import os

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

subprocess.run(['mcrl22lps', '-v', 'prime.mcrl2', 'prime.lps'], check=True)
subprocess.run(['lps2lts', '-v', '-Q0', 'prime.lps', 'prime.aut'], check=True)

