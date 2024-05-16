#!/usr/bin/env python3

import subprocess
import os

from sys import argv

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

run = subprocess.run(['mcrl22lps', '-vo', 'magic_hexagon.mcrl2'], stdout=subprocess.PIPE, check=True)
subprocess.run(['lpssumelm', '-v', '-', 'temp.lps', '--timings'], input=run.stdout, check=True)

if '-rjittyc' in argv:
    subprocess.run(['lps2lts', '-v', '-rjittyc', 'temp.lps', 'temp.aut'], check=True)

subprocess.run(['mcrl22lps', '-v', 'magic_square.mcrl2', 'magic_square.lps'], check=True)
subprocess.run(['lps2lts', '-v', 'magic_square.lps', 'magic_square.aut'], check=True)
