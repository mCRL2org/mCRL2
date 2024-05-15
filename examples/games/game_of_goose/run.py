#!/usr/bin/env python3

import subprocess
import os

from sys import argv

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

run = subprocess.run(['mcrl22lps', '-vn', 'game_of_goose.mcrl2'], stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpssuminst', '-v'], input=run.stdout, stdout=subprocess.PIPE, check=True)
subprocess.run(['lpsrewr', '-v', '-', 'game_of_goose.lps'], input=run.stdout, check=True)

subprocess.run(['lps2lts', 'game_of_goose.lps', '-v'], check=True)
