#!/usr/bin/env python3

import subprocess

subprocess.run(['mcrl22lps', '-v', 'scheduler.mcrl2', 'scheduler.lps'], check=True)

subprocess.run(['lps2pbes', '-v', '-f', 'nodeadlock.mcf', 'scheduler.lps', 'scheduler.nodeadlock.pbes'], check=True)
subprocess.run(['pbes2bool', '-v', 'scheduler.nodeadlock.pbes'], check=True)

subprocess.run(['lps2pbes', '-v', '-f', 'infinitely_often_enabled_then_infinitely_often_taken_a.mcf', 'scheduler.lps', 'scheduler.infinitely_often_enabled_then_infinitely_often_taken_a.pbes'], check=True)

subprocess.run(['pbes2bool', '-v', '-rjittyc', 'scheduler.infinitely_often_enabled_then_infinitely_often_taken_a.pbes'], check=True)
