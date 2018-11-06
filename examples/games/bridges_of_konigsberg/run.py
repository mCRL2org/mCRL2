import os

os.system('mcrl22lps bridges_of_konigsberg.mcrl2 bridges_of_konigsberg.lps -v')
os.system('lps2pbes -v -feuler_cycle.mcf bridges_of_konigsberg.lps bridges_of_konigsberg.pbes')
os.system('pbes2bool -v bridges_of_konigsberg.pbes')
