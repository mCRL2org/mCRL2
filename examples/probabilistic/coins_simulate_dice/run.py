import os

os.system('mcrl22lps -v dice.mcrl2 dice.lps')
os.system('lps2lts -v dice.lps dice.aut')
os.system('ltspbisim -epbisim dice.aut dice_reduced.aut')

