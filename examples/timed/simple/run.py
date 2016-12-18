import os

os.system('mcrl22lps -Tv simple.mcrl2 simple.lps')
os.system('lpsrealelm -v simple.lps simpler.lps')
os.system('lps2lts -v simpler.lps simple.lts')

