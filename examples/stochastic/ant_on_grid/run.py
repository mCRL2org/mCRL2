import os

os.system('mcrl22lps -v ant_on_grid.mcrl2 ant_on_grid.lps')
os.system('lps2lts -vrjittyc ant_on_grid.lps ant_on_grid.aut')

