#!/usr/bin/env python3

import subprocess
import os

from shutil import which

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

subprocess.run(['mcrl22lps', '5_paylines_game_spec.mcrl2', '5_paylines_game_spec.lps'], check=True)
subprocess.run(['mcrl22lps', '10_paylines_game_spec.mcrl2', '10_paylines_game_spec.lps'], check=True)

lps2pres = which('lps2pres')
pressolve = which('pressolve')
if lps2pres is not None and pressolve is not None:
    subprocess.run([lps2pres, '-f', 'expected_gain_5_run.mcf', '5_paylines_game_spec.lps', '5_paylines_game_spec.pres'], check=True)
    subprocess.run([pressolve, '-am', '-v', '-p30', '5_paylines_game_spec.pres'], check=True)

    subprocess.run([lps2pres, '-f', 'expected_gain_10_run.mcf', '10_paylines_game_spec.lps', 'expected_gain_10_run.pres'], check=True)
    subprocess.run([pressolve, '-am', '-v', '-p30', 'expected_gain_10_run.pres'], check=True)