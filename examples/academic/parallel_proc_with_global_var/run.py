import os

os.system('mcrl22lps -vnf parallel_counting.mcrl2 temp.lps')
os.system('lps2pbes -v -fparallel_counting.mcf temp.lps temp.pbes')
os.system('pbes2bool -v -rjittyc -s2 temp.pbes')
os.system('rm -f temp.lps temp.pbes')


