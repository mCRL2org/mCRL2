#!/usr/bin/env python3

import subprocess
import os

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

subprocess.run(['mcrl22lps', '-v', 'beggar_my_neighbour.mcrl2', 'beggar_my_neighbour.lps'], check=True)
subprocess.run(['lps2lts', '-v', 'beggar_my_neighbour.lps', 'beggar_my_neighbour.lts'], check=True)
subprocess.run(['lts2pbes', '-v', '-fexists_an_infinite_game.mcf', 'beggar_my_neighbour.lts', 'beggar_my_neighbour.exists_an_infinite_game.pbes'], check=True)
subprocess.run(['pbessolve', '-v', 'beggar_my_neighbour.exists_an_infinite_game.pbes'], check=True)

