import os

os.system('mcrl22lps -vD snake.mcrl2 snake.lps')
os.system('lps2lts -vrjittyc snake.lps snake.aut')

os.system('lps2pbes -v -f nodeadlock.mcf snake.lps snake.nodeadlock.pbes')
os.system('pbes2bool -vrjittyc -s1 snake.nodeadlock.pbes')

os.system('lps2pbes -v -f white_can_win.mcf snake.lps snake.white_can_win.pbes')
os.system('pbes2bool -vrjittyc -s1 snake.white_can_win.pbes')

os.system('lps2pbes -v -f black_can_win.mcf snake.lps snake.black_can_win.pbes')
os.system('pbes2bool -vrjittyc -s1 snake.black_can_win.pbes')

os.system('lps2pbes -v -f eventually_white_or_black_wins.mcf snake.lps snake.eventually_white_or_black_wins.pbes')
os.system('pbes2bool -vrjittyc -s1 snake.eventually_white_or_black_wins.pbes')
