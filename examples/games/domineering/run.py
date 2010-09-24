import os

os.system('mcrl22lps -vD domineering.mcrl2 domineering.lps')
os.system('lps2lts -vrjittyc domineering.lps domineering.aut')

os.system('lps2pbes -v -f nodeadlock.mcf domineering.lps domineering.nodeadlock.pbes')
os.system('pbes2bool -vrjittyc -s1 domineering.nodeadlock.pbes')

os.system('lps2pbes -v -f player1_can_win.mcf domineering.lps domineering.player1_can_win.pbes')
os.system('pbes2bool -vrjittyc -s1 domineering.player1_can_win.pbes')

os.system('lps2pbes -v -f player2_can_win.mcf domineering.lps domineering.player2_can_win.pbes')
os.system('pbes2bool -vrjittyc -s1 domineering.player2_can_win.pbes')

os.system('lps2pbes -v -f eventually_player1_or_player2_wins.mcf domineering.lps domineering.eventually_player1_or_player2_wins.pbes')
os.system('pbes2bool -vrjittyc -s1 domineering.eventually_player1_or_player2_wins.pbes')
