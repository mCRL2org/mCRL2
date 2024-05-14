#!/usr/bin/env python3

import subprocess
import os

from sys import argv

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

mcrl22lps = subprocess.run(['mcrl22lps', '-vn', 'game_of_goose.mcrl2'], stdout=subprocess.PIPE, check=True)
lpssuminst = subprocess.run(['lpssuminst', '-v'], input=mcrl22lps.stdout, stdout=subprocess.PIPE, check=True)
subprocess.run(['lpsrewr', '-v', '-', 'game_of_goose.lps'], input=lpssuminst.stdout, check=True)

if '--long' in argv:
    subprocess.run(['lps2lts', 'game_of_goose.lps', '-v'], check=True)
