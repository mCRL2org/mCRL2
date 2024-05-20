#!/usr/bin/env python3

import subprocess
import os

from shutil import which

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

subprocess.run(['mcrl22lps', '-nfTv', 'ball_game.mcrl2', 'ball_game.lps'], check=True)
subprocess.run(['lpsuntime', '-v', 'ball_game.lps', 'ball_gameu.lps'], check=True)

lpsrealelm = which('lpsrealelm')
if lpsrealelm is not None:
    subprocess.run([lpsrealelm, '--max=11', '-v', 'ball_gameu.lps', 'ball_gamer.lps'], check=True)
    subprocess.run(['lps2lts', '-v', 'ball_gamer.lps', 'ball_game.lts'], check=True)