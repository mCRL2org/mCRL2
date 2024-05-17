#!/usr/bin/env python3

import subprocess
import os

from shutil import which

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

subprocess.run(['mcrl22lps', '-Tv', 'light.mcrl2', 'light.lps'], check=True)

lpsrealelm = which('lpsrealelm')
if lpsrealelm is not None:
    subprocess.run(['lpsrealelm', '-v', 'light.lps', 'lightr.lps'], check=True)
    subprocess.run(['lps2lts', '-v', 'lightr.lps', 'light.lts'], check=True)