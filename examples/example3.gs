% This is the GenSpect example from Section 10.3 of
% the Master's thesis by Muck van Weerdenburg

proc 

Dup=sum(x:Real,_r_dup(x)|_s_dup1(x)|_s_dup2(x)).Dup

Inc=sum(x:Real,_r_inc(x)|_s_inc(x+1)).Inc

Mul=sum(x:Real,sum(y:Real,_r_dup(x)|_r_inc(y)|_s_mult(x*y))).Mul

Dim=restrict({_r_dup|_s_mul},
	comm({_s_dup1|_r_mul,_s_dup2|_r_inc,_s_inc|_r_mul},
		Dup||Mul||Dim
	)
    )

init Dim
