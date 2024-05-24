#!/usr/bin/env python3

import subprocess
import os

from sys import argv
from shutil import which

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

run = subprocess.run(['mcrl22lps', 'reels_game_spec.mcrl2'], stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpssuminst'], input=run.stdout, stdout=subprocess.PIPE, check=True)
subprocess.run(['lpsconstelm', '-', 'temp.lps'], input=run.stdout, check=True)

lps2pres = which('lps2pres')
pressolve = which('pressolve')
presrewr = which('presrewr')
if lps2pres is not None and pressolve is not None and presrewr is not None:
    for formula in ['expected_gain_max_alt.mcf']:
        run = subprocess.run([lps2pres, '-f', formula, 'temp.lps'], stdout=subprocess.PIPE, check=True)
        run = subprocess.run([presrewr], input=run.stdout, stdout=subprocess.PIPE, check=True)
        run = subprocess.run([presrewr, '-p', 'quantifier-one-point'], input=run.stdout, stdout=subprocess.PIPE, check=True)
        run = subprocess.run([presrewr, '-p', 'quantifier-all'], input=run.stdout, stdout=subprocess.PIPE, check=True)
        run = subprocess.run([presrewr, '-p', 'simplify', '-', 'temp.pres'], input=run.stdout, check=True)

        if '-rjittyc' in argv:
            subprocess.run([pressolve, '-am', '-rjittyc', '-v', '-p30', 'temp.pres'], check=True)
