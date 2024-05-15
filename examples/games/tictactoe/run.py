#!/usr/bin/env python3

import subprocess
import os

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

subprocess.run(['mcrl22lps', '-v', 'tictactoe.mcrl2', 'tictactoe.lps'], check=True)
subprocess.run(['lps2lts', '-v', 'tictactoe.lps', 'tictactoe.lts'], check=True)
subprocess.run(['lts2pbes', '-v', '-fhas_player_cross_a_winning_strategy.mcf', 'tictactoe.lts', 'tictactoe.winning_strategy.pbes'], check=True)
subprocess.run(['lts2pbes', '-vc', '-fone_wrong_move.mcf', 'tictactoe.lts', 'tictactoe.wrong_move.pbes'], check=True)
subprocess.run(['pbessolve', '-v', 'tictactoe.winning_strategy.pbes'], check=True)
subprocess.run(['pbessolve', '-v', '-f', 'tictactoe.lts', 'tictactoe.wrong_move.pbes'], check=True)

