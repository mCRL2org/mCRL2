#!/usr/bin/env python3

import subprocess
import os

from shutil import which

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

subprocess.run(['mcrl22lps', 'sultan_of_persia.mcrl2', 'sultan_of_persia.lps'], check=True)

lps2pres = which('lps2pres')
pressolve = which('pressolve')
if lps2pres is not None and pressolve is not None:
    subprocess.run([lps2pres, '-f', 'best_spouse.mcf', 'sultan_of_persia.lps', 'best_spouse.pres'], check=True)
    subprocess.run([pressolve, 'best_spouse.pres'], check=True)