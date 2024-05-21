#!/usr/bin/env python3

import subprocess
import os

from shutil import which

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

subprocess.run(['mcrl22lps', '3slot_hold_spec.mcrl2', '3slot_hold_spec.lps'], check=True)

lps2pres = which('lps2pres')
pressolve = which('pressolve')
if lps2pres is not None and pressolve is not None:
    subprocess.run([lps2pres, '-f', 'expected_probability_average.mcf', '3slot_hold_spec.lps', 'expected_probability_average.pres'], check=True)
    subprocess.run([pressolve, '-am', '-v', '-p30', 'expected_probability_average.pres'], check=True)

    subprocess.run([lps2pres, '-f', 'expected_probability_max.mcf', '3slot_hold_spec.lps', 'expected_probability_max.pres'], check=True)
    subprocess.run([pressolve, '-am', '-v', '-p30', 'expected_probability_max.pres'], check=True)

    subprocess.run([lps2pres, '-f', 'expected_probability_min.mcf', '3slot_hold_spec.lps', 'expected_probability_min.pres'], check=True)
    subprocess.run([pressolve, '-am', '-v', '-p30', 'expected_probability_max.pres'], check=True)
