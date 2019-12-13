#!/bin/bash

echo 'currently requires the tool pbesinst and the experimental tool pbessymbolicbisim and an SMT solver'

txt2pbes pbes_inst_tr_4.txt | pbesinst -sfinite -f"*(*:Bool)" | pbesrewr -psimplify >  pbes_inst_tr_4.pbes
pbessymbolicbisim -v pbes_inst_tr_4.pbes 
