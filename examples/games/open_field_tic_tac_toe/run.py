import os

os.system('mcrl22lps -v open_field_tictactoe.mcrl2 temp.lps')
os.system('lpssuminst -v temp.lps | lpsparunfold -l -sBoard -v -n6 | lpsparunfold -l -sRow -v -n8 | lpsrewr -v > temp1.lps')
os.system('lpsconstelm -v -t temp1.lps temp2.lps')
# os.system('lps2lts -v -rjittyc temp2.lps tictactoe.aut')
# os.system('lps2lts -v -rjittyc temp2.lps ')
os.system('lps2pbes -fyellow_has_a_winning_strategy.mcf temp2.lps temp.pbes')
os.system('pbesconstelm temp.pbes -v | pbesparelm -v > temp_yellow.pbes')
os.system('lps2pbes -fred_has_a_winning_strategy.mcf temp2.lps temp.pbes')
os.system('pbesconstelm temp.pbes -v | pbesparelm -v > temp_red.pbes')
# os.system('pbes2bool -v -s2 -rjittyc  -esome temp_yellow.pbes')
os.system('pbes2bool -v -s2 -rjittyc  -esome temp_red.pbes')


