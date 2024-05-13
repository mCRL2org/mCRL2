#!/usr/bin/env python3

import subprocess

mcrl22lps = subprocess.run(['mcrl22lps', '-vDn', 'four_in_a_row.mcrl2'], stdout=subprocess.PIPE, check=True)

# We use sum instantiation and parunfold to split the list of lists for the board into individual simple parameters.
lpssumsinst =  subprocess.run(['lpssuminst', '-v'], input=mcrl22lps.stdout, stdout=subprocess.PIPE, check=True)
lpsparunfold = subprocess.run(['lpsparunfold', '-sBoard', '-n5'], input=lpssumsinst.stdout, stdout=subprocess.PIPE, check=True)
lpsparunfold = subprocess.run(['lpsparunfold', '-sRow', '-n5'], input=lpsparunfold.stdout, stdout=subprocess.PIPE, check=True)
lpsrewr = subprocess.run(['lpsrewr', '-v'], input=lpsparunfold.stdout, stdout=subprocess.PIPE, check=True)
subprocess.run(['lpsconstelm', '-v', '-', 'temp.lps'], input=lpsrewr.stdout, check=True)

# The state space of the original 7x6 four in a row is so big that explicit
# state space exploration will not terminate. Reducing the number of columns,
# e.g. to four instead of default 7 does lead to a result. In that case the
# there are 16M states. Solving the formulas red_wins.mcf requires 7M bes
# variables (with result false). Five columns lead to appr. 100Mstates. See
# four_in_a_row_symbolic for an approach using the symbolic tools that are able
# to solve the full game on sufficient hardware (approx 1TB of memory).
#subprocess.run(['lps2lts', '-v', 'temp.lps'], check=True)

# Also pbes2bool does not tend to terminate on the full game, as it is too big.
# os.system('lps2pbes -vfred_wins.mcf temp2.lps temp.pbes')
# os.system('pbesrewr -pquantifier-all temp.pbes | pbesconstelm -v | pbesrewr -v > temp1.pbes')
# os.system('pbes2bool -rjittyc -v -s2 -zdepth-first temp1.pbes ')#



