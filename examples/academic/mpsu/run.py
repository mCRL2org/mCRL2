import os

os.system('mcrl22lps -v mpsu.mcrl2 mpsu.lps')

os.system('lps2pbes -v -f nodeadlock.mcf mpsu.lps mpsu.nodeadlock.pbes')
os.system('pbes2bool -v mpsu.nodeadlock.pbes')

os.system('lps2pbes -v -f mpsu1.mcf mpsu.lps mpsu.mpsu1.pbes')
os.system('pbes2bool -v mpsu.mpsu1.pbes')

os.system('lps2pbes -v -f mpsu2.mcf mpsu.lps mpsu.mpsu2.pbes')
os.system('pbes2bool -v mpsu.mpsu2.pbes')

os.system('lps2pbes -v -f mpsu3.mcf mpsu.lps mpsu.mpsu3.pbes')
os.system('pbes2bool -v mpsu.mpsu3.pbes')

os.system('lps2pbes -v -f mpsu4.mcf mpsu.lps mpsu.mpsu4.pbes')
os.system('pbes2bool -v mpsu.mpsu4.pbes')

os.system('lps2pbes -v -f mpsu5.mcf mpsu.lps mpsu.mpsu5.pbes')
os.system('pbes2bool -v mpsu.mpsu5.pbes')

os.system('lps2pbes -v -f mpsu6.mcf mpsu.lps mpsu.mpsu6.pbes')
os.system('pbes2bool -v mpsu.mpsu6.pbes')

