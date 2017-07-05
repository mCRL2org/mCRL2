import os

os.system('mcrl22lps -v tictactoe.mcrl2 tictactoe.lps')
os.system('lps2lts -v -rjittyc tictactoe.lps tictactoe.fsm')


