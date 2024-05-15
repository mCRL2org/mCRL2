#!/usr/bin/env python3

import subprocess
import os

from sys import argv

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

mcrl22lps = subprocess.run(['mcrl22lps', '-v', 'knights.mcrl2'], stdout=subprocess.PIPE, check=True)
lpssuminst = subprocess.run(['lpssuminst'], input=mcrl22lps.stdout, stdout=subprocess.PIPE, check=True)
lpsparunfold = subprocess.run(['lpsparunfold', '-n1', '-sBoard'], input=lpssuminst.stdout, stdout=subprocess.PIPE, check=True)
lpsparunfold = subprocess.run(['lpsparunfold', '-n5', '-sRow', '-', 'knights.lps'], input=lpsparunfold.stdout, check=True)

if "-rjittyc" in argv:
    # There are 35144889 states at 19 levels. The action ready
    # is found at the end. There are 36 moves necessary to
    # exchange the positions of the black and white horses.
    subprocess.run(['lps2lts', '-rjittyc', '-v', '-aready', 'knights.lps'], check=True)