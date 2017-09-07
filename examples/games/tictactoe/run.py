import os

os.system('mcrl22lps -v tictactoe.mcrl2 tictactoe.lps')
os.system('lps2lts -v -rjittyc tictactoe.lps tictactoe.lts')
os.system('lps2pbes -vm -fhas_player_cross_a_winning_strategy.mcf tictactoe.lps tictactoe.pbes')
os.system('pbes2bool -v -rjittyc tictactoe.pbes')


