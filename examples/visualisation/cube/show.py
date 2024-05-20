#!/usr/bin/env python3

import subprocess
import os

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

subprocess.run(['mcrl22lps', 'cube.mcrl2', 'temp.lps', '-v'], check=True)
subprocess.run(['lps2lts', 'temp.lps', 'temp.lts'], check=True)
subprocess.run(['ltsgraph', 'temp.lts'], check=True)
