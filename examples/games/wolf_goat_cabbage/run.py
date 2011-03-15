import os

os.system('mcrl22lps -v wolf_goat_cabbage.mcrl2 wolf_goat_cabbage.lps')
os.system('lps2lts -vrjittyc wolf_goat_cabbage.lps wolf_goat_cabbage.svc')

os.system('lps2pbes -v -f nodeadlock.mcf wolf_goat_cabbage.lps wolf_goat_cabbage.nodeadlock.pbes')
os.system('pbes2bool -vrjittyc -s1 wolf_goat_cabbage.nodeadlock.pbes')

