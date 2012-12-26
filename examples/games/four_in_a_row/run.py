import os

os.system('mcrl22lps -vD four_in_a_row.mcrl2 temp.lps')
os.system('lpssuminst -v temp.lps | lpsparunfold -l -sBoard -v -n5 | lpsparunfold -l -sRow -v -n7 | lpsrewr -v > temp1.lps')
os.system('lpsconstelm -v -c -rjittyc temp1.lps temp2.lps')
# The state space of four in a row is big, so the statement below will not terminate.
os.system('lps2lts -v -ftree -rjittyc --cached --prune temp2.lps') 


