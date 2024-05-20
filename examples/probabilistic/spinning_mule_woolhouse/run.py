#!/usr/bin/env python3

import subprocess
import os

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

subprocess.run(['mcrl22lps', 'spinning_mule.mcrl2', '-v', 'temp1.lps'], check=True)
subprocess.run(['mcrl22lps', 'spinning_mule_optimized.mcrl2', '-v', 'temp2.lps'], check=True)
subprocess.run(['lps2lts', '-v', '-rjittyc', 'temp1.lps', 'temp1.lts'], check=True)
subprocess.run(['lps2lts', '-v', '-rjittyc', 'temp2.lps', 'temp2.lts'], check=True)
subprocess.run(['ltspcompare', '-epbisim', '-v', 'temp1.lts', 'temp2.lts'], check=True)

subprocess.run(['mcrl22lps', 'spinning_mule_optimized.mcrl2', '-v', 'spinning_mule_optimized.lps'], check=True)
subprocess.run(['lps2pres', '-fminimal_walking_distance.mcf', 'spinning_mule_optimized.lps', '-v', 'spinning_mule_optimized.pres'], check=True)
subprocess.run(['resrewr', '-pquantifier-inside', 'spinning_mule_optimized.pres', '|', '', 'presrewr', '-vpquantifier-one-point', '>', 'spinning_mule_optimized1.pres'], check=True)
subprocess.run(['presrewr', '-pquantifier-all', 'spinning_mule_optimized1.pres', 'spinning_mule_optimized2.pres'], check=True)
subprocess.run(['pressolve', '-v', '-rjitty', '-am', 'spinning_mule_optimized2.pres', '-p30', '--timings'], check=True)

subprocess.run(['mcrl22lps', '-n', 'spinning_mule.mcrl2', '-v', 'spinning_mule.lps'], check=True)
subprocess.run(['lps2pres', '-fminimal_walking_distance.mcf', 'spinning_mule.lps', '-v', 'spinning_mule.pres'], check=True)
subprocess.run(['presrewr', '-pquantifier-inside', 'spinning_mule.pres', '|', '', 'presrewr', '-vpquantifier-one-point', '>', 'spinning_mule1.pres'], check=True)
subprocess.run(['presrewr', '-pquantifier-all', 'spinning_mule1.pres', 'spinning_mule2.pres'], check=True)
subprocess.run(['pressolve', '-v', '-rjitty', '-am', 'spinning_mule2.pres', '-p30', '--timings'], check=True)

subprocess.run(['mcrl22lps', 'spinning_mule_woolhouse.mcrl2', '-v', 'spinning_mule_woolhouse.lps'], check=True)
subprocess.run(['lps2pres', '-fminimal_walking_distance.mcf', 'spinning_mule_woolhouse.lps', '-v', 'spinning_mule_woolhouse.pres'], check=True)
subprocess.run(['presrewr', '-pquantifier-one-point', 'spinning_mule_woolhouse.pres', 'spinning_mule_woolhouse1.pres'], check=True)
subprocess.run(['pressolve', '-v', '-rjitty', '-am', 'spinning_mule_woolhouse1.pres', '-p30'], check=True)
