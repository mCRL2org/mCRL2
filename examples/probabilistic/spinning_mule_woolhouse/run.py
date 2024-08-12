#!/usr/bin/env python3

import subprocess
import os

from shutil import which
from sys import argv

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

print('Linearising and comparing module probabilistic bisimulation...')
subprocess.run(['mcrl22lps', '-v', 'spinning_mule.mcrl2', 'spinning_mule.lps'], check=True)
subprocess.run(['mcrl22lps', '-v', 'spinning_mule_optimized.mcrl2', 'spinning_mule_optimized.lps'], check=True)

if '-rjittyc' in argv:
    subprocess.run(['lps2lts', '-v', '-rjittyc', 'spinning_mule.lps', 'spinning_mule.lts'], check=True)
    subprocess.run(['lps2lts', '-v', '-rjittyc', 'spinning_mule_optimized.lps', 'spinning_mule_optimized.lts'], check=True)
    subprocess.run(['ltspcompare', '-epbisim', '-v', 'spinning_mule.lts', 'spinning_mule_optimized.lts'], check=True)
else:
    print('-rjittyc is required to generate the state spaces for checking probabilistic bisimulation')

lps2pres = which('lps2pres')
pressolve = which('pressolve')
presrewr = which('presrewr')

if lps2pres and presrewr and pressolve:
    print('Checking minimal_walking_distance.mcf for spinning_mule_optimised.mcrl2')
    run = subprocess.run([lps2pres, '-fminimal_walking_distance.mcf', 'spinning_mule_optimized.lps'], stdout=subprocess.PIPE, check=True)
    run = subprocess.run([presrewr, '-pquantifier-inside'], input=run.stdout, stdout=subprocess.PIPE, check=True)
    run = subprocess.run([presrewr, '-pquantifier-one-point'], input=run.stdout, stdout=subprocess.PIPE, check=True)
    subprocess.run([presrewr, '-pquantifier-all', '-', 'spinning_mule_optimized.pres'], input=run.stdout, check=True)

    subprocess.run([pressolve, '-v', '-am', 'spinning_mule_optimized.pres', '-p30', '--timings'], check=True)

    print('Checking minimal_walking_distance.mcf for spinning_mule.mcrl2')
    run = subprocess.run([lps2pres, '-fminimal_walking_distance.mcf', 'spinning_mule.lps'], stdout=subprocess.PIPE, check=True)
    run = subprocess.run([presrewr, '-pquantifier-inside'], input=run.stdout, stdout=subprocess.PIPE, check=True)
    run = subprocess.run([presrewr, '-pquantifier-one-point'], input=run.stdout, stdout=subprocess.PIPE, check=True)
    subprocess.run([presrewr, '-pquantifier-all', '-', 'spinning_mule.pres'], input=run.stdout, check=True)

    subprocess.run([pressolve, '-v', '-am', 'spinning_mule.pres', '-p30', '--timings'], check=True)

    print('Checking minimal_walking_distance.mcf for spinning_mule_woolhouse.mcrl2')
    subprocess.run(['mcrl22lps', 'spinning_mule_woolhouse.mcrl2', '-v', 'spinning_mule_woolhouse.lps'], check=True)
    subprocess.run([lps2pres, '-fminimal_walking_distance.mcf', 'spinning_mule_woolhouse.lps', '-v', 'spinning_mule_woolhouse.pres'], check=True)
    subprocess.run([presrewr, '-pquantifier-one-point', 'spinning_mule_woolhouse.pres', 'spinning_mule_woolhouse1.pres'], check=True)
    subprocess.run([pressolve, '-v', '-rjitty', '-am', 'spinning_mule_woolhouse1.pres', '-p30'], check=True)
