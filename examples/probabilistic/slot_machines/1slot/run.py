#!/usr/bin/env python3

import subprocess
import os

from shutil import which

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

subprocess.run(['mcrl22lps', '1slot_spec.mcrl2', '1slot_spec.lps'], check=True)

lps2pres = which('lps2pres')
pressolve = which('pressolve')
if lps2pres is not None and pressolve is not None:
    subprocess.run([lps2pres, '-f', 'expected_gain_1_run.mcf', '1slot_spec.lps', 'fexpected_gain_1_run.pres'], check=True)
    subprocess.run([pressolve, '-am', '-v', '-p30', 'fexpected_gain_1_run.pres'], check=True)

    subprocess.run([lps2pres, '-f', 'expected_gain_long_run.mcf', '1slot_spec.lps', 'expected_gain_long_run.pres'], check=True)
    subprocess.run([pressolve, '-am', '-v', '-p10', 'expected_gain_long_run.pres'], check=True)

    subprocess.run([lps2pres, '-f', 'expected_gain_long_run.mcf', '1slot_spec.lps', 'expected_time_to_star.pres'], check=True)
    subprocess.run([pressolve, '-ag', '-v', 'expected_time_to_star.pres'], check=True)
