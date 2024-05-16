#!/usr/bin/env python3

import subprocess
import os

from shutil import which

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

subprocess.run(['mcrl22lps', '-Tv', 'simple.mcrl2', 'simple.lps'], check=True)

lpsrealelm = which('lpsrealelm')
if lpsrealelm is not None:
    subprocess.run([lpsrealelm, '-v', 'simple.lps', 'simpler.lps'], check=True)
    subprocess.run(['lps2lts', '-v', 'simpler.lps', 'simple.lts'], check=True)