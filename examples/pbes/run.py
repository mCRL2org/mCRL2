#!/usr/bin/env python3

import subprocess
import os


from shutil import which

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

pbessymbolicbisim = which('pbessymbolicbisim')

if pbessymbolicbisim is not None and which('Z3') is not None:
    for file in ['pbes_inst_tr_1.txt', 'pbes_inst_tr_2.txt', 'pbes_inst_tr_3.txt', 'pbes_inst_tr_4.txt', 'pbes_inst_tr_5.txt']:
        name, _ = os.path.splitext(file)

        run = subprocess.run(['txt2pbes', file], stdout=subprocess.PIPE, check=True)
        if file == 'pbes_inst_tr_4.txt':
            run = subprocess.run(['pbesinst', '-sfinite', '-f"*(*:Bool)"'], input=run.stdout, stdout=subprocess.PIPE, check=True)
            run = subprocess.run(['pbesrewr', '-psimplify'], input=run.stdout, stdout=subprocess.PIPE, check=True)

        subprocess.run([pbessymbolicbisim], input=run.stdout, check=True)

else:
    print('Requires the experimental tool pbessymbolicbisim and an SMT solver')

for file in ['datatypes.txt', 'nonmonotonic.txt', 'true.txt']:
    run = subprocess.run(['txt2pbes', file], stdout=subprocess.PIPE, check=True)