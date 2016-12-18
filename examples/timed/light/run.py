import os

os.system('mcrl22lps -Tv light.mcrl2 light.lps')
os.system('lpsrealelm -v light.lps lightr.lps')
os.system('lps2lts -v lightr.lps light.lts')

