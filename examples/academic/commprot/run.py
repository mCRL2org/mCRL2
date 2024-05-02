#!/usr/bin/env python3

import subprocess

# The generated state space is huge, well over 10^8 states. Explicit verification of the state
# space is therefore not possible. One can use highway search, using e.g. --todo-max=1000,
# searching with 10000 lanes through the state space for deadlocks. The total number of 
# states to be explored is limited to 100000 in this example. 
subprocess.run(['mcrl22lps', '-v', 'commprot.mcrl2', 'commprot.lps'], shell=True, check=True)
subprocess.run(['lps2lts', '-D', '--strategy=highway', '--cached', '--todo-max=1000', '-l100000', '-v', 'commprot.lps'], shell=True, check=True)
# Checking the properties below does not work on an ordinary computer, given 
# the size of the state space. 
# os.system('lps2pbes -v -fnodeadlock.mcf commprot.lps commprot.nodeadlock.pbes')
# os.system('pbes2bool -v -s2 -rjittyc commprot.nodeadlock.pbes')