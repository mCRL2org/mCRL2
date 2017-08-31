import os

os.system('mcrl22lps -vn game_of_goose.mcrl2 temp.lps')
os.system('lpssuminst -v temp.lps temp1.lps')
os.system('lpsrewr -v temp1.lps temp2.lps')
os.system('lps2lts temp2.lps -v')
