#!/usr/bin/env python3

import subprocess
import os

from sys import argv

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

subprocess.run(['mcrl22lps', '-v', 'ant_on_grid.mcrl2', 'ant_on_grid.lps'], check=True)

if '-rjittyc' in argv:
    subprocess.run(['lps2lts', '-vrjittyc', 'ant_on_grid.lps', 'ant_on_grid.aut'], check=True)
