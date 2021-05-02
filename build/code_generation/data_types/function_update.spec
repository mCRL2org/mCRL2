#using S
#using T

map @func_update <"function_update">: (S -> T) <"arg1"> # S <"arg2"> # T <"arg3"> -> (S -> T)                   internal defined_by_rewrite_rules;
% Potential new mappings, under study. 
%    @func_update_stable <"function_update_stable">: (S -> T) <"arg1"> # S <"arg2"> # T <"arg3"> -> (S -> T);
%    @func_base <"function_base">: (S->T) -> (S->T);

var x: S;
    y: S;
    v: T;
    w: T;
    f: S -> T;

eqn ==(f(x),v) -> @func_update(f,x,v) = f;
    @func_update(@func_update(f,x,w),x,v) = @func_update(f,x,v);
    >(x,y) -> @func_update(@func_update(f,y,w), x,v) = @func_update(@func_update(f,x,v),y,w);
    !=(x,y) -> @func_update(f,x,v)(y) = f(y);
    @func_update(f,x,v)(x) = v;

% Potentially new rules, still under study.
% The first rule below should only be applied if f is not preceded by a @func_update_stable, or a @func_update. 
%eqn  ==(f(x),v) -> @func_update(@func_base(f),x,v) = @func_base(f);
%     !=(f(x),v) -> @func_update(@func_base(f),x,v) = @func_update_stable(@func_base(f),x,v);
%     @func_update(@func_update_stable(f,x,w),x,v) = if(==(f(x),v),f,@func_update_stable(f,x,v));
%     <(y,x) -> @func_update(@func_update_stable(f,y,w), x,v) = @func_update_stable(@func_update(f,x,v),y,w);
%     <(x,y) -> @func_update(@func_update_stable(f,y,w), x,v) = if(==(f(x),v),@func_update_stable(f,y,w),@func_update_stable(@func_update_stable(f,y,w), x,v));
%     !=(x,y) -> @func_update_stable(f,x,v)(y) = f(y);
%     @func_update_stable(f,x,v)(x) = v;
%     @func_base(f)(x) = f(x);

