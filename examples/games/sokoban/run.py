#!/usr/bin/env python3

import subprocess
import os

from sys import argv

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

run = subprocess.run(['mcrl22lps', 'sokoban.mcrl2', '-m'], stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpssuminst', '-v'], input=run.stdout, stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpsparunfold', '-s', 'Board', '-n', '8'], input=run.stdout, stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpsconstelm', '-vt'], input=run.stdout, stdout=subprocess.PIPE, check=True)
run =  subprocess.run(['lpsparunfold', '-s', 'Row', '-n', '11'], input=run.stdout, stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpsconstelm', '-vt', '-', 'temp.lps'],  input=run.stdout, stdout=subprocess.PIPE, check=True)

if '-rjittyc' in argv:
    subprocess.run(['lps2lts', '-vrjittyc', '-awin', '-t1', '-shighway', '--todo-max=100000', 'temp.lps'], check=True)
