% This is based on the GenSpect example from Section 10.1 of the TU/e Computer
% Science master's thesis titled 'GenSpect Process Algebra' by Muck van
% Weerdenburg.

act
  get_i, put_k, get_l, put_j: Nat;
  get, put: Nat;

proc 
  Sqr_ik =
    sum n:Nat. get_i(n) | put_k(n*n) . Sqr_ik;

  Sqr_lj =
    sum n:Nat. get_l(n) | put_j(n*n) . Sqr_lj;

  P_kl(b:Bag(Nat)) =
    sum n:Nat. put_k(i,n) . P_kl(b + {n:1}) +
    sum n:Nat. (n in b) -> get_l(n) . P_kl(b - {n:1});

  DSqr_ij =
    hide({put,get},
      allow({get_i,put,get,put_j},
        comm({put_k|put_k -> put, get_l|get_l -> get},
           Sqr_ik || P_kl({}) || Sqr_lj
        )
      )
    );

init
  DSqr_ij;
