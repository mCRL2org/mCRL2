#!/usr/bin/env python3

import subprocess

subprocess.run(['mcrl22lps', 'mutex.mcrl2', 'mutex.lps'], check=True)
subprocess.run(['lps2pbes', '-f', 'justlive.mcf', 'mutex.lps', 'mutex.justlive.pbes'], check=True)
subprocess.run(['pbessolve', 'mutex.justlive.pbes'], check=True)
