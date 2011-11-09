import os

os.system('mcrl22lps -v four_in_a_row.mcrl2 four_in_a_row.lps')
# The state space of four in a row is big, so the statement below will not terminate.
os.system('lps2lts -vrjittyc four_in_a_row.lps')


