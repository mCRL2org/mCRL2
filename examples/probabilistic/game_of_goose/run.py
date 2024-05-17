#!/usr/bin/env python3

import subprocess
import os

from sys import argv

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

subprocess.run(['mcrl22lps', '-v', 'game_of_goose_stochastic.mcrl2', 'game_of_goose_stochastic.lps'], check=True)

if '-rjittyc' in argv:
    subprocess.run(['lps2lts', '-vrjittyc', '--cached', 'game_of_goose_stochastic.lps', 'game_of_goose_stochastic.lts'], check=True)
