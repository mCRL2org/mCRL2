import os

os.system('mcrl22lps -vn peg_solitaire.mcrl2 peg_solitaire.lps')
os.system('lpssuminst -v peg_solitaire.lps temp1.lps')
os.system('lpsrewr -v temp1.lps temp2.lps')
os.system('lpsparunfold -v -l -sBoard temp2.lps | lpsrewr -v > temp3.lps')
os.system('lpsparunfold  -v -l -sRow temp3.lps |lpsrewr -v > temp4.lps')
os.system('lps2lts -vrjittyc -ftree -aready temp4.lps')

