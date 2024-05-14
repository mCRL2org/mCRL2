#!/usr/bin/env python3

import subprocess
import os

from sys import argv

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

mcrl22lps = subprocess.run(['mcrl22lps', '-vn', 'hex.mcrl2', 'hex.lps'], stdout=subprocess.PIPE, check=True)
#lpsparunfold = subprocess.run(['lpsparunfold', '-sBoard', '-n5'], input=mcrl22lps.stdout, stdout=subprocess.PIPE, check=True)
#lpsparunfold = subprocess.run(['lpsparunfold', '-sRow', '-n5'], input=lpsparunfold.stdout, stdout=subprocess.PIPE, check=True)
#subprocess.run(['lpsrewr', '-v', '-', 'hex.lps'], input=lpsparunfold.stdout, check=True)

if '--long' in argv:
    subprocess.run(['lps2lts', '-v', '-rjittyc', 'hex.lps'], check=True)
