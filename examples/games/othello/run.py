import os

os.system('mcrl22lps -vDo othello.mcrl2 othello.lps')
os.system('lpssuminst -v -rjittyc othello.lps temp0.lps')
os.system('lpsparunfold -lv -n4 -sBoard temp0.lps temp1.lps')
os.system('lpsparunfold -lv -n4 -sRow temp1.lps temp2.lps')
os.system('lpsconstelm -c -t -rjittyc -v temp2.lps temp3.lps')
os.system('lps2lts -v temp3.lps')

os.system('lps2pbes -v -f nodeadlock.mcf temp3.lps temp3.nodeadlock.pbes')
os.system('pbes2bool -vrjittyc temp3.nodeadlock.pbes')

os.system('lps2pbes -v -f white_wins_always.mcf temp3.lps temp3.white_wins_always.pbes')
os.system('pbes2bool -vrjittyc temp3.white_wins_always.pbes')

os.system('lps2pbes -v -f red_wins_always.mcf temp3.lps temp3.red_wins_always.pbes')
os.system('pbes2bool -vrjittyc temp3.red_wins_always.pbes')

os.system('lps2pbes -v -f white_can_win.mcf temp3.lps temp3.white_can_win.pbes')
os.system('pbes2bool -vrjittyc temp3.white_can_win.pbes')

os.system('lps2pbes -v -f red_can_win.mcf temp3.lps temp3.red_can_win.pbes')
os.system('pbes2bool -vrjittyc temp3.red_can_win.pbes')

os.system('lps2pbes -v -f exists_draw.mcf temp3.lps temp3.exists_draw.pbes')
os.system('pbes2bool -vrjittyc temp3.exists_draw.pbes')
