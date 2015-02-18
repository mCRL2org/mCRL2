#using S
#using T

map @func_update <"function_update">: (S -> T) <"arg1"> # S <"arg2"> # T <"arg3"> -> (S -> T);

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

