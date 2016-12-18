import os

os.system('mcrl22lps -v airplane_ticket.mcrl2 airplane_ticket.lps')
os.system('lps2lts -v airplane_ticket.lps airplane_ticket.aut')


