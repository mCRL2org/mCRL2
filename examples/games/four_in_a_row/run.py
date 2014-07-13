import os

os.system('mcrl22lps -vD four_in_a_row.mcrl2 temp.lps')
os.system('lpssuminst -v temp.lps | lpsparunfold -l -sBoard -v -n6 | lpsparunfold -l -sRow -v -n8 | lpsrewr -v > temp1.lps')
os.system('lpsconstelm -v -c -rjittyc temp1.lps temp2.lps')
# The state space of four in a row is big, so the statement below will not terminate.
# Reducing the number of columns, e.g. to four instead of default 7 does lead to a result.
# In that case the there are 16M states. Solving the formulas red_wins.mcf requires 7M bes
# variables (with result false). Five columns lead to appr. 100Mstates.
os.system('lps2lts -v -rjittyc --prune temp2.lps') 
# Also pbes2bool does not tend to terminate on the full game, as it is too big.
os.system('lps2pbes -vfred_wins.mcf temp2.lps temp.pbes')
os.system('pbesrewr -pquantifier-all temp.pbes | pbesconstelm -v > temp1.pbes')
os.system('pbes2bool -rjittyc -v -s2 temp1.pbes')



