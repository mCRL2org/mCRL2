import os

os.system('mcrl22lps -v self_stabilisation.mcrl2 self_stabilisation.lps')
os.system('lps2lts -v self_stabilisation.lps self.aut')


