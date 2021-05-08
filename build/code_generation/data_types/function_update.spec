#using S
#using T

#include bool.spec

map @func_update <"function_update">: (S -> T) <"arg1"> # S <"arg2"> # T <"arg3"> -> (S -> T)                   external defined_by_rewrite_rules;
    @func_update_stable <"function_update_stable">: (S -> T) <"arg1"> # S <"arg2"> # T <"arg3"> -> (S -> T)     internal defined_by_rewrite_rules;
    @is_not_an_update <"is_not_a_function_update">: (S->T) <"arg1"> -> Bool                                     internal defined_by_code;
% The function below evaluates a condition, and if it is not true, else is always chosen. 
    @if_always_else <"if_always_else">: Bool <"arg1"> # (S -> T) <"arg2"> # (S -> T) <"arg3"> -> (S -> T)       internal defined_by_code;

var x: S;
    y: S;
    v: T;
    w: T;
    f: S -> T;

% The first rule below should only be applied if f is not preceded by a @func_update_stable, or a @func_update. 
eqn  @is_not_an_update(f) -> @func_update(f,x,v) = @if_always_else(==(f(x),v),f,@func_update_stable(f,x,v));
     @func_update(@func_update_stable(f,x,w),x,v) = @if_always_else(==(f(x),v),f,@func_update_stable(f,x,v));
     <(y,x) -> @func_update(@func_update_stable(f,y,w), x,v) = @func_update_stable(@func_update(f,x,v),y,w);
     <(x,y) -> @func_update(@func_update_stable(f,y,w), x,v) = 
                     @if_always_else(==(f(x),v),
                                     @func_update_stable(f,y,w),
                                     @func_update_stable(@func_update_stable(f,y,w), x,v));
     !=(x,y) -> @func_update_stable(f,x,v)(y) = f(y);
     @func_update_stable(f,x,v)(x) = v;
     !=(x,y) -> @func_update(f,x,v)(y) = f(y);
     @func_update(f,x,v)(x) = v;

% Below are the old rules. Although theoretically elegant, they require exponential time, and therefore are very slow. 
% map @func_update <"function_update">: (S -> T) <"arg1"> # S <"arg2"> # T <"arg3"> -> (S -> T)                   external defined_by_rewrite_rules;
% var x: S;
%     y: S;
%     v: T;
%     w: T;
%     f: S -> T;
% eqn ==(f(x),v) -> @func_update(f,x,v) = f;
%     @func_update(@func_update(f,x,w),x,v) = @func_update(f,x,v);
%     >(x,y) -> @func_update(@func_update(f,y,w), x,v) = @func_update(@func_update(f,x,v),y,w);
%     !=(x,y) -> @func_update(f,x,v)(y) = f(y);
%     @func_update(f,x,v)(x) = v;

