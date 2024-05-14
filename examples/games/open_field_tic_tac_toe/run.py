#!/usr/bin/env python3

import subprocess
import os

from sys import argv

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

mcrl22lps = subprocess.run(['mcrl22lps', '-v', 'open_field_tictactoe.mcrl2'], stdout=subprocess.PIPE, check=True)
lpssuminst = subprocess.run(['lpssuminst', '-v'], input=mcrl22lps.stdout, stdout=subprocess.PIPE, check=True)
lpsparunfold = subprocess.run(['lpsparunfold', '-sBoard', '-n6'], input=lpssuminst.stdout, stdout=subprocess.PIPE, check=True)
lpsparunfold = subprocess.run(['lpsparunfold', '-sRow', '-n8'], input=lpsparunfold.stdout, stdout=subprocess.PIPE, check=True)
lpsrewr = subprocess.run(['lpsrewr', '-v'], input=lpsparunfold.stdout, stdout=subprocess.PIPE, check=True)
subprocess.run(['lpsconstelm', '-v', '-t', '-', 'temp.lps'],  input=lpsrewr.stdout, check=True)

subprocess.run(['lps2lts', '-v', '-rjittyc', 'temp.lps', 'tictactoe.aut'], check=True)

lps2pbes = subprocess.run(['lps2pbes', '-fyellow_has_a_winning_strategy.mcf', 'temp.lps'], stdout=subprocess.PIPE, check=True)
pbesconstelm = subprocess.run(['pbesconstelm', 'temp.pbes', '-v'], input=lps2pbes.stdout, stdout=subprocess.PIPE, check=True)
pbesparelm = subprocess.run(['pbesparelm', '-v'], input=pbesconstelm.stdout, stdout=subprocess.PIPE, check=True)
subprocess.run(['pbesrewr', '-pquantifier-all', '-', 'temp_yellow.pbes'], input=pbesparelm.stdout, check=True)

lps2pbes = subprocess.run(['lps2pbes', '-fred_has_a_winning_strategy.mcf', 'temp.lps'], stdout=subprocess.PIPE, check=True)
pbesconstelm = subprocess.run(['pbesconstelm', 'temp.pbes', '-v'], input=lps2pbes.stdout, stdout=subprocess.PIPE, check=True)
pbesparelm = subprocess.run(['pbesparelm', '-v'], input=pbesconstelm.stdout, stdout=subprocess.PIPE, check=True)
subprocess.run(['pbesrewr', '-pquantifier-all', '-', 'temp_red.pbes'], input=pbesparelm.stdout, check=True)

if "-rjittyc" in argv:
    subprocess.run(['pbes2bool', '-v', '-s2', '-rjittyc', '', '-esome', 'temp_yellow.pbes'], check=True)
    subprocess.run(['pbes2bool', '-v', '-s2', '-rjittyc', '', '-esome', 'temp_red.pbes'], check=True)