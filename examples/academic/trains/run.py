#!/usr/bin/env python3

import subprocess

subprocess.run(['mcrl22lps', '-v', 'trains.mcrl2', 'trains.lps'], check=True)

subprocess.run(['lps2pbes', '-v', '-f', 'nodeadlock.mcf', 'trains.lps', 'trains.nodeadlock.pbes'], check=True)
subprocess.run(['pbes2bool', '-v', 'trains.nodeadlock.pbes'], check=True)

subprocess.run(['lps2pbes', '-v', '-f', 'infinitely_often_enabled_then_infinitely_often_taken_enter.mcf', 'trains.lps', 'trains.infinitely_often_enabled_then_infinitely_often_taken_enter.pbes'], check=True)
subprocess.run(['pbes2bool', '-v', 'trains.infinitely_often_enabled_then_infinitely_often_taken_enter.pbes'], check=True)

subprocess.run(['lps2pbes', '-v', '-f', 'mutual_exclusion.mcf', 'trains.lps', 'trains.mutual_exclusion.pbes'], check=True)
subprocess.run(['pbes2bool', '-v', 'trains.mutual_exclusion.pbes'], check=True)
