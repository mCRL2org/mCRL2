#!/usr/bin/env python3

import subprocess
import os

from shutil import which

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

subprocess.run(['mcrl22lps', '-v', 'coins.mcrl2', 'coins.lps'], check=True)

lps2pres = which('lps2pres')
pressolve = which('pressolve')
if lps2pres is not None and pressolve is not None:
    
    subprocess.run([lps2pres, '-v', '-fformula1.mcf', 'coins.lps', 'coins.pres'], check=True)
    subprocess.run([pressolve, 'coins.pres'], check=True)
    subprocess.run([lps2pres, '-v', '-fformula2.mcf', 'coins.lps', 'coins.pres'], check=True)
    subprocess.run([pressolve, 'coins.pres'], check=True)