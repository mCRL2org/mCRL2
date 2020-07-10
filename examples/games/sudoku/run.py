import os

os.system('mcrl22lps -v sudoku.mcrl2 sudoku.lps')
os.system('lps2lts -vrjitty sudoku.lps sudoku.aut')

