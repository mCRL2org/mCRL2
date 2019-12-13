#!/bin/bash

echo 'currently requires the experimental tool pbessymbolicbisim and an SMT solver'

txt2pbes pbes_inst_tr_5.txt pbes_inst_tr_5.pbes
pbessymbolicbisim -v pbes_inst_tr_5.pbes 
