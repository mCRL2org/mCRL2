#!/usr/bin/env python3

import subprocess
import os

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

subprocess.run(['mcrl22lps', '-vnD', 'rubiks_cube.mcrl2', 'rubiks_cube.lps'], check=True)
