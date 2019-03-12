import os

os.system('mcrl22lps -v tictactoe.mcrl2 tictactoe.lps')
# os.system('lps2lts -v -rjittyc tictactoe.lps tictactoe.lts')
os.system('lts2pbes -v -fhas_player_cross_a_winning_strategy.mcf tictactoe.lts tictactoe.winning_strategy.pbes')
os.system('lts2pbes -vc -fone_wrong_move.mcf tictactoe.lts tictactoe.wrong_move.pbes')
os.system('pbessolve -v tictactoe.winning_strategy.pbes')
os.system('pbessolve -v -f tictactoe.lts tictactoe.wrong_move.pbes')


