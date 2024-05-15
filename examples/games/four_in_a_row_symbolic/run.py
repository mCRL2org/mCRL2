#!/usr/bin/env python3

import subprocess
import shutil

# We apply various preprocessing steps to eliminate the Board and Row parameters
# since they are complex data types, in this case lists.
run = subprocess.run(["mcrl22lps", "-v", "-n", "-f", "four_in_a_row_symbolic.mcrl2"], stdout=subprocess.PIPE, check=True)
run = subprocess.run(["lpssuminst", "-v"], input=run.stdout, stdout=subprocess.PIPE, check=True)
run = subprocess.run(["lpsparunfold", "-sBoard", "-n6"], input=run.stdout, stdout=subprocess.PIPE, check=True)
run = subprocess.run(["lpsparunfold", "-sRow", "-n8"], input=run.stdout, stdout=subprocess.PIPE, check=True)
run = subprocess.run(["lpsrewr", "-v"], input=run.stdout, stdout=subprocess.PIPE, check=True)
subprocess.run(["lpsconstelm", "-v", "-", "temp.lps"], input=run.stdout, stdout=subprocess.PIPE, check=True)

run = subprocess.run(["lps2pbes", "-m", "-f", "red_wins.mcf", "temp.lps"], stdout=subprocess.PIPE, check=True)
run = subprocess.run(["pbesconstelm", "-v", "-", "temp.red_wins.pbes"], input=run.stdout, stdout=subprocess.PIPE, check=True)

lpsreach = shutil.which("lpsreach")
if lpsreach:
    # The variables are ordered from bottom to top, left to right on the board.
    # The first parameter indicates the current player as this seems to work
    # best. number of states = 7.02089e+13 (time = 8.61s)
    subprocess.run(
        [
            lpsreach,
            "-v",
            "--groups=simple",
            '--reorder="42 35 28 21 14 7 0 36 29 22 15 8 1 37 30 23 16 9 2 38 31 24 17 10 3 39 32 25 18 11 4 40 33 26 19 12 5 41 34 27 20 13 6"',
            "temp.lps",
        ],
        check=True,
    )

pbessolvesymbolic = shutil.which("pbessolvesymbolic")
if pbessolvesymbolic:
    # Actually solving this takes approximately 1TB of ram and a day worth of computation.
    # Number of BES equations = 2.00208e+14 (time = 273.47s)
    # finished solving (time = 296641.13s)
    subprocess.run([pbessolvesymbolic, '-v', '--groups=simple', '--reorder="36 29 22 15 8 1 37 30 23 16 9 2 38 31 24 17 10 3 39 32 25 18 11 4 40 33 26 19 12 5 41 34 27 20 13 6 42 35 28 21 14 7"', 'temp.red_wins.lps'], check=True)