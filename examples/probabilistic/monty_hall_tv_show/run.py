import os

os.system('mcrl22lps -v monty_hall.mcrl2 monty_hall.lps')
os.system('lps2lts -v monty_hall.lps monty_hall.aut')


