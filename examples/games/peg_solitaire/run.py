#!/usr/bin/env python3

from re import sub
import subprocess
import os

from sys import argv

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

subprocess.run(['mcrl22lps', '-vn', 'peg_solitaire.mcrl2', 'peg_solitaire.lps'], check=True)

run = subprocess.run(['lpssuminst', '-v', 'peg_solitaire.lps'], stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpsrewr', '-v'], input=run.stdout, stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpsparunfold', '-v', '-sBoard'], input=run.stdout, stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpsrewr', '-v'], input=run.stdout, stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpsparunfold', '', '-v', '-sRow'], input=run.stdout, stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpsrewr', '-v'], input=run.stdout, stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpsparelm', '-v'], input=run.stdout, stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpsconstelm', '-v'], input=run.stdout, stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpsrewr', '-v'], input=run.stdout, stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpsbinary'], input=run.stdout, stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpsactionrename', '-frename.ren'], input=run.stdout, stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpsrewr', '-v'], input=run.stdout, stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpsconstelm', '-', 'temp.lps'], input=run.stdout, stdout=subprocess.PIPE, check=True)

if '-rjittyc' in argv:
    subprocess.run(['lps2lts', '-vrjittyc', '-aready', 'temp.lps'], check=True)
