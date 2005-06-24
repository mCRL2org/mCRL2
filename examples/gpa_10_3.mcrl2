% This is based on the GenSpect example from Section 10.3 of the TU/e Computer
% Science master's thesis titled 'GenSpect Process Algebra' by Muck van
% Weerdenburg.

act
  r_dup, s_dup1, s_dup2, r_inc, s_inc, r_mul1, r_mul2, s_mul: Int;

proc
  Dup = sum x:Int. r_dup(x) | s_dup1(x) | s_dup2(x) . Dup;

  Inc = sum x:Int. r_inc(x) | s_inc(x+1) . Inc;

  Mul = sum x,y:Int. r_mul1(x) | r_mul2(y) | s_mul(x*y) . Mul;

  Dim = allow({r_dup | s_mul},
          comm({s_dup1 | r_mul1, s_dup2 | r_inc, s_inc | r_mul2},
            Dup || Inc || Mul
          )
        );

init
  Dim;
