#!/usr/bin/env python3

import subprocess
import os

from sys import argv

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

# Create LPS from mCRL2 specification
subprocess.run(['mcrl22lps', 'quoridor.mcrl2', 'quoridor.lps'], check=True)

# Optimize LPS
run = subprocess.run(['lpssuminst', '-sBool', 'quoridor.lps'], stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpsparunfold', '-n5', '-sPosition'], input=run.stdout, stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpsrewr'], input=run.stdout, stdout=subprocess.PIPE, check=True)
run = subprocess.run(['lpsconstelm'], input=run.stdout, stdout=subprocess.PIPE, check=True)
subprocess.run(['lpsrewr', '-', 'quoridor-1.lps'], input=run.stdout, check=True)

# Transform LPS into a LTS
subprocess.run(['lps2lts', '--cached', '-v', 'quoridor-1.lps', 'quoridor.lts'], check=True)

# Reduce the LTS modulo bisimulation
subprocess.run(['ltsconvert', '-v', '-ebisim', 'quoridor.lts', 'quoridor-1.lts'], check=True)

# Verify whether player 1 has a winning strategy
run = subprocess.run(['lts2pbes', '-c', '-p', '-f', 'properties/winning_strategy_player_1.mcf', 'quoridor-1.lts'], stdout=subprocess.PIPE, check=True)
subprocess.run(['pbessolve', '-s1', '--file=quoridor-1.lts', '--evidence-file=quoridor-player-1-evidence.lts'], input=run.stdout, check=True)

# Verify whether player 2 has a winning strategy
run = subprocess.run(['lts2pbes', '-c', '-p', '-f', 'properties/winning_strategy_player_2.mcf', 'quoridor-1.lts'], stdout=subprocess.PIPE, check=True)
subprocess.run(['pbessolve', '-s1', '--file=quoridor-1.lts', '--evidence-file=quoridor-player-2-evidence.lts'], input=run.stdout, check=True)
