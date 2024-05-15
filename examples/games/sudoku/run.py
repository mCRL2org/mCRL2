#!/usr/bin/env python3

import subprocess
import os

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

subprocess.run(['mcrl22lps', '-v', 'sudoku.mcrl2', 'sudoku.lps'], check=True)
subprocess.run(['lps2lts', '-v', 'sudoku.lps', 'sudoku.aut'], check=True)
