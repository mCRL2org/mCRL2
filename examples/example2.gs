% This is the GenSpect example from Section 10.2 of the TU/e Computer Science
% master's thesis titled 'GenSpect Process Algebra' by Muck van Weerdenburg.

proc
  P(i:Nat, j:Nat, b:Bag(Nat)) = 
    sum(n:N, put(i,n) . P(i,j,{n} + b))) +
    sum(n:Nat, get(j,n) . (n in b) -> P(b - {n}));

  P2 = 
    hide({__pass},
      restrict({get, put, __pass},
        comm({get | put -> __pass},
	  P(i,k,emptyBag) || P(k,j,emptyBag)
        )
      )
    )

init
  P2;
