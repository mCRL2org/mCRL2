#!/usr/bin/env python3

import subprocess

subprocess.run(['mcrl22lps', '-vnf', 'parallel_counting.mcrl2', 'temp.lps'], check=True)
subprocess.run(['lps2pbes', '-v', '-fparallel_counting.mcf', 'temp.lps', 'temp.pbes'], check=True)
subprocess.run(['pbes2bool', '-v', '-rjittyc', '-s2', 'temp.pbes'], check=True)
