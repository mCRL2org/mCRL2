import os

os.system('mcrl22lps -v coins.mcrl2 coins.lps')
os.system('lps2pres -v -fformula1.mcf coins.lps coins.pres')
os.system('pressolve coins.pres')
os.system('lps2pres -v -fformula2.mcf coins.lps coins.pres')
os.system('pressolve coins.pres')

