#!/usr/bin/env python3

import subprocess
import os

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

subprocess.run(['mcrl22lps -vo othello.mcrl2 othello.lps'], check=True)

lpssuminst = subprocess.run(['lpssuminst', '-v', '-rjitty', 'othello.lps'], stdout=subprocess.PIPE, check=True)
lpsparunfold = subprocess.run(['lpsparunfold', '-n6', '-lv', '-sBoard'], input=lpssuminst.stdout, stdout=subprocess.PIPE, check=True)
lpsrewr = subprocess.run(['lpsrewr', '-v'], input=lpsparunfold.stdout, stdout=subprocess.PIPE, check=True)
lpsconstelm = subprocess.run(['lpsconstelm', '-c', '-v'], input=lpsrewr.stdout, stdout=subprocess.PIPE, check=True)
lpsrewr = subprocess.run(['lpsrewr', '-v'], input=lpsconstelm.stdout, stdout=subprocess.PIPE, check=True)
lpsparelm = subprocess.run(['lpsparunfold', '-n6', '-lv', '-sRow'], input=lpsrewr.stdout, stdout=subprocess.PIPE, check=True)


subprocess.run(['lpsrewr', '-v', '-rjitty', 'temp3.lps', 'temp4.lps'],', 'check=True)
subprocess.run(['lpsconstelm', '-c', '-v', '-rjitty', 'temp4.lps', 'temp4a.lps'],', 'check=True)
subprocess.run(['lpsrewr', '-v', '-rjitty', 'temp4a.lps', 'temp5.lps'],', 'check=True)
subprocess.run(['lps2lts', '-v', '-rjitty', 'temp5.lps'],', 'check=True)



print('----------------------------------------------'], check=True)
print('Red can always win, i.e. red has a winning strategy.'], check=True)
print(], check=True)'---------------------------------------------- '], check=True)
subprocess.run(['lps2pbes -v -fred_wins_always.mcf temp5.lps othello.pbes'], check=True)
subprocess.run(['pbes2bool -v -rjitty othello.pbes'], check=True)
subprocess.run(['echo ----------------------------------------------'], check=True)
subprocess.run(['echo White can always win, i.e. white has a winning strategy.'], check=True)
subprocess.run(['echo ----------------------------------------------'], check=True)
subprocess.run(['lps2pbes -v -fwhite_wins_always.mcf temp5.lps othello.pbes'], check=True)
subprocess.run(['pbes2bool -v -rjitty othello.pbes'], check=True)
subprocess.run(['echo ----------------------------------------------'], check=True)
subprocess.run(['echo Nodeadlock.'], check=True)
subprocess.run(['echo ----------------------------------------------'], check=True)
subprocess.run(['lps2pbes -v -fnodeadlock.mcf temp5.lps othello.pbes'], check=True)
subprocess.run(['pbes2bool -v -rjitty othello.pbes'], check=True)
subprocess.run(['echo ----------------------------------------------'], check=True)
subprocess.run(['echo There exists a draw.'], check=True)
subprocess.run(['echo ----------------------------------------------'], check=True)
subprocess.run(['lps2pbes -v -fexists_draw.mcf temp5.lps othello.pbes'], check=True)
subprocess.run(['pbes2bool -v -rjitty othello.pbes'], check=True)
subprocess.run(['echo ----------------------------------------------'], check=True)
subprocess.run(['echo White can win.'], check=True)
subprocess.run(['echo ----------------------------------------------'], check=True)
subprocess.run(['lps2pbes -v -fwhite_can_win.mcf temp5.lps othello.pbes'], check=True)
subprocess.run(['pbes2bool -v -rjitty othello.pbes'], check=True)
subprocess.run(['echo ----------------------------------------------'], check=True)
subprocess.run(['echo Red can win.'], check=True)
subprocess.run(['echo ----------------------------------------------'], check=True)
subprocess.run(['lps2pbes -v -fred_can_win.mcf temp5.lps othello.pbes'], check=True)
subprocess.run(['pbes2bool -v -rjitty othello.pbes'], check=True)
subprocess.run(['echo ----------------------------------------------'], check=True)
subprocess.run(['echo ----------------------------------------------'], check=True)

