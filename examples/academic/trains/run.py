import os

os.system('mcrl22lps -v trains.mcrl2 trains.lps')

os.system('lps2pbes -v -f nodeadlock.mcf trains.lps trains.nodeadlock.pbes')
os.system('pbes2bool -v trains.nodeadlock.pbes')

os.system('lps2pbes -v -f infinitely_often_enabled_then_infinitely_often_taken_enter.mcf trains.lps trains.infinitely_often_enabled_then_infinitely_often_taken_enter.pbes')
os.system('pbes2bool -v trains.infinitely_often_enabled_then_infinitely_often_taken_enter.pbes')

os.system('lps2pbes -v -f mutual_exclusion.mcf trains.lps trains.mutual_exclusion.pbes')
os.system('pbes2bool -v trains.mutual_exclusion.pbes')

