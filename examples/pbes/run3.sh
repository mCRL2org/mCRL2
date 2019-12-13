#!/bin/bash

echo 'currently requires the experimental tool pbessymbolicbisim and an SMT solver'

txt2pbes pbes_inst_tr_3.txt  pbes_inst_tr_3.pbes
pbessymbolicbisim -v pbes_inst_tr_3.pbes 
