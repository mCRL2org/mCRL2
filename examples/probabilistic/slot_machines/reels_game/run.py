#!/usr/bin/env python3

import subprocess
import os

from shutil import which

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

mcrl22lps = subprocess.run(['mcrl22lps', 'reels_game_spec.mcrl2'], stdout=subprocess.PIPE, check=True)
lpssuminst = subprocess.run(['lpssuminst'], input=mcrl22lps.stdout, stdout=subprocess.PIPE, check=True)
subprocess.run(['lpsconstelm', '-', 'temp.lps'], input=lpssuminst.stdout, check=True)

lps2pres = which('lps2pres')
pressolve = which('pressolve')
presrewr = which('presrewr')
if lps2pres is not None and pressolve is not None and presrewr is not None:
    for formula in ['expected_gain_max.mcf', 'expected_gain_alt.mcf', 'expected_gain_alt1.mcf']:
        lps2pres_run = subprocess.run([lps2pres, '-f', 'expected_gain_max.mcf'], stdout=subprocess.PIPE, check=True)
        presrewr_run = subprocess.run([presrewr], input=lps2pres_run.stdout, stdout=subprocess.PIPE, check=True)
        presrewr_run = subprocess.run([presrewr, '-p', 'quantifier-one-point'], input=presrewr_run.stdout, stdout=subprocess.PIPE, check=True)
        presrewr_run = subprocess.run([presrewr, '-p', 'quantifier-all'], input=presrewr_run.stdout, stdout=subprocess.PIPE, check=True)
        presrewr_run = subprocess.run([presrewr, '-p', 'simplify', '-', 'temp.lps'], input=presrewr_run.stdout, stdout=subprocess.PIPE, check=True)

        subprocess.run([pressolve, '-am', '-rjittyc', '-v', '-p30', 'temp.lps'], check=True)
