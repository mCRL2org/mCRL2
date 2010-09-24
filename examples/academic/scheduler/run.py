import os

os.system('mcrl22lps -v scheduler.mcrl2 scheduler.lps')

os.system('lps2pbes -v -f nodeadlock.mcf scheduler.lps scheduler.nodeadlock.pbes')
os.system('pbes2bool -v scheduler.nodeadlock.pbes')

os.system('lps2pbes -v -f infinitely_often_enabled_then_infinitely_often_taken_a.mcf scheduler.lps scheduler.infinitely_often_enabled_then_infinitely_often_taken_a.pbes')
# Heavily used expansion of universal quantifiers
os.system('pbes2bool -v -rjittyc scheduler.infinitely_often_enabled_then_infinitely_often_taken_a.pbes')

