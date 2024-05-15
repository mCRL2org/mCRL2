#!/usr/bin/env python3

import subprocess

subprocess.run(['mcrl22lps', '-v', 'parallel.mcrl2', 'parallel.lps'], check=True)

subprocess.run(['lps2pbes', '-v', '-f', 'nodeadlock.mcf', 'parallel.lps', 'parallel.nodeadlock.pbes'], check=True)
subprocess.run(['pbes2bool', '-v', 'parallel.nodeadlock.pbes'], check=True)

# The following creates state space, and stores trace to deadlock.
subprocess.run(['lps2lts', '-v', '-Dt', 'parallel.lps', 'parallel.aut'], check=True)
