import os

os.system('mcrl22lps carpet.mcrl2 temp.lps -v')
os.system('lps2lts temp.lps temp.lts')
os.system('ltsgraph temp.lts')
