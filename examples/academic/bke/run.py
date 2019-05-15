import os

os.system('mcrl22lps -v bke.mcrl2 bke.lps')
os.system('lps2pbes -v -f secret_not_leaked.mcf bke.lps bke.secret_not_leaked.pbes')
os.system('pbessolve -v bke.secret_not_leaked.pbes')
