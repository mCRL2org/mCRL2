% This is based on the GenSpect example from Section 10.2 of the TU/e Computer
% Science master's thesis titled 'GenSpect Process Algebra' by Muck van
% Weerdenburg.

act
  put_i, get_k: Nat;
  put_k, get_j: Nat;
  pass_k: Nat;

proc
  P_ik(b:Bag(Nat)) = 
    sum n:Nat. put_i(n) . P_ik(b + {n:1}) +
    sum n:Nat. (n in b) -> get_k(n) . P_ik(b - {n:1});

  P_kj(b:Bag(Nat)) = 
    sum n:Nat. put_k(n) . P_kj(b + {n:1}) +
    sum n:Nat. (n in b) -> get_j(n) . P_kj(b - {n:1});

  P2 = 
    hide({pass_k},
      allow({put_i, pass_k, get_j},
        comm({get_k | put_k -> pass_k},
	  P_ik({}) || P_kj({})
        )
      )
    );

init
  P2;
