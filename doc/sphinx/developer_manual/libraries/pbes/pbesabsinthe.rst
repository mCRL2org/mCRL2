PBES Abstraction
================

*Wieger Wesselink, Tim Willemse*

Absinthe
--------

The Absinthe algorithm takes as input a PBES :math:`p`, a substitution on function
symbols :math:`\sigma_F:\mathcal{F}\rightarrow\mathcal{F}`, and a substitution on
sorts :math:`\sigma_S:\mathcal{S}\rightarrow\mathcal{S}`, where :math:`\mathcal{F}`
is the set of function symbols and :math:`\mathcal{S}` is the set of sorts. For
each :math:`s\in domain(\sigma_S)` there is a corresponding *abstraction function*
:math:`h_s`.

Definitions
~~~~~~~~~~~

We define :math:`FunctionSymbols(p)` as the set of function symbols that occur in
the PBES :math:`p`. For a sort :math:`s` we define :math:`ContainerConstructors(s)`
as the set of constructor functions of the sorts :math:`List(s)`, :math:`Set(s)` and
:math:`Bag(s)`. This includes for example the functions :math:`[]{:}List(s)` and
:math:`{\rhd}{:}s\times List(s)\rightarrow List(s)`.

Algorithm
~~~~~~~~~

The algorithm first extends and modifies the substitution :math:`\sigma_F`. Then a
transformation to :math:`p` is applied.

Step 1
^^^^^^

The algorithm first extends the substitution :math:`\sigma_F`. Let

.. math::

   A = \left( FunctionSymbols(p) \cup \bigcup_{s\in domain(\sigma_S)} ContainerConstructors(s) \right)
       \setminus domain(\sigma_F).

For each function symbol :math:`f_1\in A` the substitution :math:`\sigma_F` is
extended with :math:`\sigma_F(f_1):=f_2`, where :math:`f_2` is obtained from
:math:`f_1` using

.. math::

   \begin{array}{lcl}
   f_1{:}s_b & \mapsto & f_2{:}s_b^{\sigma_S} \\
   f_1{:}C(s) & \mapsto & f_2{:}C(s)^{\sigma_S} \\
   f_1{:}s_1\times\cdots\times s_n\rightarrow s & \mapsto &
     f_2{:}s_1^{\sigma_S}\times\cdots\times s_n^{\sigma_S}\rightarrow Set(s^{\sigma_S}),
   \end{array}

where :math:`s_b` is a basic sort, :math:`s_1\times\cdots\times s_n\rightarrow s`
is a function sort, and :math:`C(s)` is a container sort, and where :math:`f_2` is
a fresh identifier.

.. remark::

   The sort of :math:`f_1` may not contain any element of :math:`domain(\sigma_S)`
   as a subsort.

.. remark::

   If :math:`f_1` is a function update, then the domain of the updated function may
   not contain any element of :math:`domain(\sigma_S)` as a subsort.

For each of the function symbols :math:`f\in FunctionSymbols(p)\setminus domain(\sigma_F)`
a corresponding equation is introduced:

.. math::

   \begin{array}{lcl}
   f_2 & = & \left\{\begin{array}{ll}
     h_{s_b}(f_1) & \text{if } s_b\in domain(\sigma_S) \\
     f^{\sigma_S} & \text{otherwise}
   \end{array}\right. \\[8pt]
   f_2 & = & f_1^{\sigma_S} \\[8pt]
   f_2(x) & = & \left\{\begin{array}{ll}
     \{h_s(f_1(x))\} & \text{if } TargetSort(s)\in domain(\sigma_S) \\
     \{f_1(x)\} & \text{otherwise,}
   \end{array}\right.
   \end{array}

where :math:`x{:}s_1^{\sigma_S}\times\cdots\times s_n^{\sigma_S}\rightarrow s^{\sigma_S}`,
and where :math:`TargetSort` is recursively defined as

.. math::

   \begin{array}{lcl}
   TargetSort(s_b) & = & s_b \\
   TargetSort(C(s)) & = & C(s) \\
   TargetSort(s_1\times\cdots\times s_n\rightarrow s) & = & TargetSort(s).
   \end{array}

Step 2
^^^^^^

After this the substitution :math:`\sigma_F` is transformed. Each
:math:`(f_1,f_2)\in\sigma_F` is replaced by :math:`(f_1,f_3)`, where :math:`f_3`
is obtained from :math:`f_2` as follows:

.. math::

   \begin{array}{lcl}
   f_2{:}s_b & \mapsto & f_3{:}Set(s_b) \\
   f_2{:}C(s) & \mapsto & f_3{:}Set(C(s)) \\
   f_2{:}s_1\times\cdots\times s_n\rightarrow s & \mapsto &
     f_3{:}Set(s_1)\times\cdots\times Set(s_n)\rightarrow s,
   \end{array}

where :math:`f_3` is a fresh name.

For each pair :math:`(f_2,f_3)` a corresponding equation is generated:

.. math::

   \begin{array}{lcl}
   f_3 & = & \{f_2\} \\[8pt]
   f_3 & = & \{f_2\} \\[8pt]
   f_3(X) & = & \bigl\{ y{:}s \mid \exists_{x{:}s_1\times\cdots\times s_n\rightarrow s}.\;
                 x\in X \wedge y\in f_2(x) \bigr\},
   \end{array}

where :math:`X{:}Set(s_1)\times\cdots\times Set(s_n)\rightarrow s`.

Step 3
^^^^^^

The PBES :math:`p` is transformed using the transformations :math:`T` and :math:`U`,
defined recursively as:

.. math::

   \begin{array}{lcl}
   T\!\left((\sigma_1 X_1(d_1{:}D_1)=\varphi_1)\cdots(\sigma_n X_n(d_n{:}D_n)=\varphi_n)\right)
   & = &
   \begin{array}[t]{l}
   (\sigma_1 \widehat{X_1}(d_1{:}D_1^{\sigma_S})=T(\varphi_1))\cdots \\
   (\sigma_n \widehat{X_n}(d_n{:}D_n^{\sigma_S})=T(\varphi_n))
   \end{array} \\[4pt]
   T(\lnot\varphi) & = & \lnot T(\varphi) \\
   T(\varphi\oplus\psi) & = & T(\varphi)\oplus T(\psi) \\
   T(X_i(e)) & = & \left\{\begin{array}{ll}
     \bigvee_{d\in T(e)}\widehat{X_i}(d) & \text{if it is an under-approximation} \\[2pt]
     \bigwedge_{d\in T(e)}\widehat{X_i}(d) & \text{if it is an over-approximation}
   \end{array}\right. \\[8pt]
   T(\forall_{d{:}D}.\varphi) & = & \forall_{d{:}D^{\sigma_S}}.T(\varphi) \\
   T(\exists_{d{:}D}.\varphi) & = & \exists_{d{:}D^{\sigma_S}}.T(\varphi) \\
   T(b) & = & \left\{\begin{array}{ll}
     false\in U(b) & \text{if it is an under-approximation} \\[2pt]
     true\in U(b) & \text{if it is an over-approximation}
   \end{array}\right. \\[8pt]
   U(v) & = & \{v^{\sigma_S}\} \\
   U(f) & = & \sigma_F(f) \\
   U(c) & = & \{h_s(c)\} \quad\text{if } c{:}s \text{ is a ground term and } s\in domain(\sigma_S) \\
   U(f(x)) & = & U(f)(U(x)) \quad\text{(N.B.\ The general case } U(y(x)) \text{ is not supported!)} \\
   U(\lambda_{d{:}D}.x) & = & \{v{:}s^{\sigma_S} \mid v=U(x)\}, \text{ where } x{:}s \\
   U(\forall_{d{:}D}.x) & = & \text{?} \\
   U(\exists_{d{:}D}.x) & = & \text{?} \\
   U(x\ \mathbf{whr}\ y{:}=z) & = & \text{?}
   \end{array}

where :math:`\oplus\in\{\wedge,\vee,\Rightarrow\}`. Note that :math:`T` operates on
PBES expressions, and :math:`U` operates on data expressions.

PBES abstraction
----------------

Let :math:`\mathcal{E}=(\sigma_1 X_1(d_{X_1}{:}D_{X_1})=\varphi_{X_1})\cdots(\sigma_n X_n(d_{X_n}{:}D_{X_n})=\varphi_{X_n})`
be a PBES, and let :math:`V_i` be a subset of the parameters in :math:`d_{X_i}` for
:math:`i=1\cdots n`. Let :math:`e\in\{true,false\}` be a data expression. Then we
define the algorithm :math:`\mathsf{abstract}` as follows:

.. math::

   \begin{array}{lll}
   \mathsf{abstract}(d,V,e) & = & \left\{\begin{array}{ll}
     d & \text{if } \mathsf{freevar}(d)\cap V=\emptyset \\
     e & \text{otherwise}
   \end{array}\right. \\[8pt]
   \mathsf{abstract}(\varphi\oplus\psi,V,e) & = &
     \mathsf{abstract}(\varphi,V,e)\oplus\mathsf{abstract}(\psi,V,e) \\[4pt]
   \mathsf{abstract}(\mathsf{Q}_{d_1{:}D_1,\ldots,d_m{:}D_m}.\varphi,V,e) & = &
     \mathsf{Q}_{d_1{:}D_1,\ldots,d_m{:}D_m}.\mathsf{abstract}(\varphi,V\setminus\{d_1{:}D_1,\ldots,d_m{:}D_m\},e) \\[4pt]
   \mathsf{abstract}(\sigma X(d_1{:}D_1,\ldots,d_m{:}D_m)=\varphi,V,e) & = &
     \sigma X(d_1{:}D_1,\ldots,d_m{:}D_m)=\mathsf{abstract}(\varphi,V,e) \\[8pt]
   \mathsf{abstract}(\mathcal{E},[V_1,\ldots,V_n],e) & = &
     \begin{array}[t]{l}
     (\sigma_1 X_1(d_{X_1}{:}D_{X_1})=\mathsf{abstract}(\varphi_{X_1},V_1,e)) \\
     \quad\cdots \\
     (\sigma_n X_n(d_{X_n}{:}D_{X_n})=\mathsf{abstract}(\varphi_{X_n},V_n,e))
     \end{array}
   \end{array}

with :math:`d` a data expression, :math:`\oplus\in\{\wedge,\vee,\Rightarrow\}`,
:math:`\mathsf{Q}\in\{\forall,\exists\}` and
:math:`V\subset\{d_1{:}D_1,\ldots,d_m{:}D_m\}`.

Motivation
~~~~~~~~~~

The motivation for this algorithm is that if the solution of
:math:`\mathsf{abstract}(\mathcal{E},[V_1,\ldots,V_n],false)` is :math:`true`, this
implies that the solution of :math:`\mathcal{E}` is :math:`true` as well, and if the
solution of :math:`\mathsf{abstract}(\mathcal{E},[V_1,\ldots,V_n],true)` is
:math:`false`, this implies that the solution of :math:`\mathcal{E}` is :math:`false`.
