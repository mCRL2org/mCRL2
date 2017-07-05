import os

os.system('mcrl22lps -v brp.mcrl2 brp.lps')
os.system('lps2lts -v brp.lps brp.aut')


