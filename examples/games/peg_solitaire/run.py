import os

os.system('mcrl22lps -vnD peg_solitaire.mcrl2 peg_solitaire.lps')
os.system('lpssuminst -v peg_solitaire.lps temp1.lps')
os.system('lpsrewr temp1.lps temp2.lps')
os.system('lps2lts -vrjittyc -ftree -t -aready temp3.lps')

