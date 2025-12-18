#!/usr/bin/env python3

import subprocess
import os

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

subprocess.run(['mcrl22lps', '-v', 'brp.mcrl2', 'brp.lps'], check=True)
# Property p1
print("Property 1")
subprocess.run(['lps2pres', '-f', 'p1.mcf',  'brp.lps', 'brp_p1.pres'], check=True)
subprocess.run(['pressolve', '-am', '-v', '-p30', 'brp_p1.pres'], check=True)
# Property p2
print("Property 2")
subprocess.run(['lps2pres', '-f', 'p2.mcf',  'brp.lps', 'brp_p2.pres'], check=True)
subprocess.run(['pressolve', '-am', '-v', '-p30', 'brp_p2.pres'], check=True)
