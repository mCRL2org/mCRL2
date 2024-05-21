#!/usr/bin/env python3

import subprocess
import os

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

subprocess.run(['mcrl22lps', '-v', '-o', 'cellular_automata.mcrl2', 'cellular_automata.lps'], check=True)
subprocess.run(['lps2lts', 'cellular_automata.lps', 'cellular_automata.lts'])