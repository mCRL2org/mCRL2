echo 'checking whether IU included in SU'
ltscompare -vpweak-trace-ac --tau=change,continue,ptd_continue,ptd_stable ertms-hl3.mod.aut ../SU/ertms-hl3.mod.aut
echo 'checking whether SU included in IU'
ltscompare -vpweak-trace-ac --tau=change,continue,ptd_continue,ptd_stable ../SU/ertms-hl3.mod.aut ertms-hl3.mod.aut 
