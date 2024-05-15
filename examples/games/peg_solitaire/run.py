#!/usr/bin/env python3

import subprocess
import os

from sys import argv

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

subprocess.run(['mcrl22lps', '-vn', 'peg_solitaire.mcrl2', 'peg_solitaire.lps'], check=True)

run = subprocess.run(['lpssuminst', '-v', 'peg_solitaire.lps'], stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpsrewr', '-v'], input=run.stdout, stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpsparunfold', '-v', '-l', '-sBoard'], input=run.stdout, stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpsrewr', '-v'], input=run.stdout, stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpsparunfold', '', '-v', '-l', '-sRow'], input=run.stdout, stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpsrewr', '-v'], input=run.stdout, stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpsparelm', '-v', 'temp4.lps', '|', 'lpsconstelm', '-v'], input=run.stdout, stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpsrewr', '-v'], input=run.stdout, stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpsbinary', 'temp5.lps', '|', 'lpsactionrename', '-frename.ren'], input=run.stdout, stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpsrewr', '-v'], input=run.stdout, stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpsconstelm', '-', 'temp.lps'], input=run.stdout, stdout=subprocess.PIPE, check=True)

if '-rjittyc' in argv:
    subprocess.run(['lps2lts', '-vrjittyc', '-aready', 'temp.lps'], check=True)
