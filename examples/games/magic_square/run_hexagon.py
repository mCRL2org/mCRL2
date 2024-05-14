#!/usr/bin/env python3

import subprocess
import os

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

mcrl22lps = subprocess.run(['mcrl22lps', '-vo', 'magic_hexagon.mcrl2'], stdout=subprocess.PIPE, check=True)
subprocess.run(['lpssumelm', '-v', '-', 'temp.lps', '--timings'], input=mcrl22lps.stdout, check=True)

subprocess.run(['lps2lts', '-v', 'temp.lps', 'temp.aut'], check=True)
