.. math::
   :nowrap:

Parameterised Boolean Equation Systems
======================================

This section gives an overview of the classes and algorithms on parameterised
boolean equation systems (PBESs). All classes and algorithms of the PBES Library reside
in the namespace ``pbes_system``.

PBES equation systems
---------------------

A Parameterised Boolean Equation System :math:`\cal{E}` is a sequence of fixpoint equations, where
each equation has the following form:

.. math::

   \sigma X(d:D)=\varphi,

with :math:`\sigma \in \{\mu, \nu\}` *a fixpoint symbol*, :math:`X(d:D)` a *predicate variable*,
and :math:`\varphi` a *predicate formula*.

The following C++ classes are defined for PBESs:

.. table:: PBES classes

   ===================================  ====================================================================================================================
   Expression                           C++ class
   ===================================  ====================================================================================================================
   :math:`\cal{E}`                      :mcrl2:`pbes                                      <mcrl2::pbes_system::pbes>`
   :math:`\sigma X(d:D)=\varphi`        :mcrl2:`pbes_equation                             <mcrl2::pbes_system::pbes_equation>`
   :math:`\sigma`                       :mcrl2:`fixpoint_symbol                           <mcrl2::pbes_system::fixpoint_symbol>`
   :math:`X(d:D)`                       :mcrl2:`propositional_variable                    <mcrl2::pbes_system::propositional_variable>`
   :math:`\varphi`                      :mcrl2:`pbes_expression                           <mcrl2::pbes_system::pbes_expression>`
   :math:`d:D`                          :mcrl2:`data::variable_list                       <mcrl2::data::variable_list>`
   :math:`e`                            :mcrl2:`data::data_expression_list                <mcrl2::data::data_expression_list>`
   ===================================  ====================================================================================================================

PBES expressions
----------------
PBES expressions (or predicate formulae) are defined using the following
grammar

.. math::

   \begin{array}{lrl}
   \varphi & ::= &  c
                \: \mid \: \neg \varphi
                \: \mid \: \varphi \wedge \varphi
                \: \mid \: \varphi \vee \varphi
                \: \mid \: \varphi \implies \varphi
                \: \mid \: \forall d{:}D .\:\varphi
                \: \mid \: \exists d{:}D .\:\varphi
                \: \mid \: X(e),
   \end{array}

where :math:`c` is a data term of sort Bool, :math:`X(e)` is a parameterised propositional variable,
:math:`d` is a variable of sort :math:`D` and :math:`e` is a data expression.

.. note::

   Both :math:`d{:}D` and :math:`e` can be multivariate, meaning they are shorthand for
   :math:`d_1:D_1, \cdots, d_n:D_n` and :math:`e_1, \cdots, e_n` respectively.

.. note::

   The operators :math:`\neg` and :math:`\implies` are usually not defined
   in the theory about PBESs. For practical reasons they are supported in the
   implementation.

The following C++ classes are defined for PBES expressions:

.. table:: PBES expression classes

   ===================================  ============================================================================================================
   Expression                           C++ class
   ===================================  ============================================================================================================
   :math:`c`                            :mcrl2:`data::data_expression                <mcrl2::data::data_expression>`
   :math:`\neg \varphi`                 :mcrl2:`not_                                 <mcrl2::pbes_system::not_>`
   :math:`\varphi \wedge \psi`          :mcrl2:`and_                                 <mcrl2::pbes_system::and_>`
   :math:`\varphi \vee \psi`            :mcrl2:`or_                                  <mcrl2::pbes_system::or_>`
   :math:`\varphi \implies \psi`        :mcrl2:`imp                                  <mcrl2::pbes_system::imp>`
   :math:`\forall d{:}D .\:\varphi`     :mcrl2:`forall                               <mcrl2::pbes_system::forall>`
   :math:`\exists d{:}D .\:\varphi`     :mcrl2:`exists                               <mcrl2::pbes_system::exists>`
   :math:`X(e)`                         :mcrl2:`propositional_variable_instantiation <mcrl2::pbes_system::propositional_variable_instantiation>`
   ===================================  ============================================================================================================

.. note::

   PBES expressions must be *monotonous*: every occurrence of a propositional
   variable should be in a scope such that the number of :math:`\neg` operators plus the
   number of left-hand sides of the :math:`\implies` operator is even.

.. note::

   Some of the class names of the operations have a trailing underscore character.
   This is only the case when the name itself (like ``and`` or ``not``) is a reserved
   C++ keyword.

Algorithms
==========
This section gives an overview of the algorithms that are available for PBESs.

Algorithms on PBESs
-------------------

.. table:: Selected algorithms on PBES data types

   ===========================================================================================   =================================================================================================================
   algorithm                                                                                     description
   ===========================================================================================   =================================================================================================================
   :mcrl2:`txt2pbes                     <mcrl2::pbes_system::txt2pbes>`                            Parses a textual description of a PBES
   :mcrl2:`lps2pbes                     <mcrl2::pbes_system::lps2pbes>`                            Generates a PBES from a linear process specification and a state formula
   :mcrl2:`constelm                     <mcrl2::pbes_system::constelm>`                            Removes constant parameters from a PBES
   :mcrl2:`parelm                       <mcrl2::pbes_system::parelm>`                              Removes unused parameters from a PBES
   :mcrl2:`pbesrewr                     <mcrl2::pbes_system::pbesrewr>`                            Rewrites the predicate formulae of a PBES
   :mcrl2:`pbesinst                     <mcrl2::pbes_system::pbesinst>`                            Transforms a PBES to a BES by instantiating predicate variables
   :mcrl2:`gauss_elimination            <mcrl2::pbes_system::gauss_elimination>`                   Solves a PBES using Gauss elimination
   :mcrl2:`remove_parameters            <mcrl2::pbes_system::remove_parameters>`                   Removes propositional variable parameters
   :mcrl2:`remove_unreachable_variables <mcrl2::pbes_system::remove_unreachable_variables>`        Removes equations that are not (syntactically) reachable from the initial state of a PBES
   :mcrl2:`is_bes                       <mcrl2::pbes_system::is_bes>`                              Returns true if a PBES data type is in BES form
   :mcrl2:`complement                   <mcrl2::pbes_system::complement>`                          Pushes negations as far as possible inwards towards data expressions
   :mcrl2:`normalize                    <mcrl2::pbes_system::normalize>`                           Brings a PBES expression into positive normal form, i.e. without occurrences of :math:`\neg` and :math:`\implies`
   ===========================================================================================   =================================================================================================================

Search and Replace functions
----------------------------

.. table:: Search and Replace functions

   ========================================================================================================================  =============================================================================
   algorithm                                                                                                                 description
   ========================================================================================================================  =============================================================================
   :mcrl2:`find_identifiers                            <mcrl2::pbes_system::find_identifiers>`                                Finds all identifiers occurring in a PBES data type
   :mcrl2:`find_sort_expressions                       <mcrl2::pbes_system::find_sort_expressions>`                           Finds all sort expressions occurring in a PBES  data type
   :mcrl2:`find_function_symbols                       <mcrl2::pbes_system::find_function_symbols>`                           Finds all function symbols occurring in a PBES  data type
   :mcrl2:`find_all_variables                          <mcrl2::pbes_system::find_variables>`                                  Finds all variables occurring in a PBES  data type
   :mcrl2:`find_free_variables                         <mcrl2::pbes_system::find_free_variables>`                             Finds all free variables occurring in a PBES  data type
   :mcrl2:`find_propositional_variable_instantiations  <mcrl2::pbes_system::find_propositional_variable_instantiations>`      Finds all propositional variable instantiations occurring in a PBES data type
   :mcrl2:`replace_sort_expressions                    <mcrl2::pbes_system::replace_sort_expressions>`                        Replaces sort expressions in a PBES data type
   :mcrl2:`replace_data_expressions                    <mcrl2::pbes_system::replace_data_expressions>`                        Replaces data expressions in a PBES data type
   :mcrl2:`replace_variables                           <mcrl2::pbes_system::replace_variables>`                               Replaces variables in a PBES data
   :mcrl2:`replace_variables_capture_avoiding          <mcrl2::pbes_system::replace_variables_capture_avoiding>`              Replaces variables in a PBES data type, and avoids unwanted capturing
   :mcrl2:`replace_free_variables                      <mcrl2::pbes_system::replace_free_variables>`                          Replaces free variables in a PBES data type
   :mcrl2:`replace_all_variables                       <mcrl2::pbes_system::replace_all_variables>`                           Replaces all variables in a PBES data type, even in declarations
   :mcrl2:`replace_propositional_variables             <mcrl2::pbes_system::replace_propositional_variables>`                 Replaces propositional variables in a PBES data type
   ========================================================================================================================  =============================================================================

Rewriters for PBES expressions
------------------------------
The following rewriters are available

.. table:: PBES expression rewriters

   ==================================================================================================  =========================================================================
   name                                                                                                description
   ==================================================================================================  =========================================================================
   :mcrl2:`bqnf_rewriter                   <mcrl2::pbes_system::bqnf_rewriter>`                        BQNF rewriter
   :mcrl2:`data2pbes_rewriter              <mcrl2::pbes_system::data2pbes_rewriter>`                   Replaces data library operators to equivalent PBES library operators
   :mcrl2:`data_rewriter                   <mcrl2::pbes_system::data_rewriter>`                        Rewrites data expressions that appear as a subterm of the PBES expression
   :mcrl2:`enumerate_quantifiers_rewriter  <mcrl2::pbes_system::enumerate_quantifiers_rewriter>`       Eliminates quantifiers by enumerating quantifier variables
   :mcrl2:`one_point_rule_rewriter         <mcrl2::pbes_system::one_point_rule_rewriter>`              Applies one point rule to simplify quantifier expressions
   :mcrl2:`pfnf_rewriter                   <mcrl2::pbes_system::pfnf_rewriter>`                        Brings PBES expressions into PFNF normal form
   :mcrl2:`quantifiers_inside_rewriter     <mcrl2::pbes_system::quantifiers_inside_rewriter>`          Pushes quantifiers inside
   :mcrl2:`simplify_quantifiers_rewriter   <mcrl2::pbes_system::simplify_quantifiers_rewriter>`        Simplifies quantifier expressions
   :mcrl2:`simplify_rewriter               <mcrl2::pbes_system::simplify_rewriter>`                    Simplifies logical boolean operators
   ==================================================================================================  =========================================================================


Standard recursive form
------------------------

The standard recursive form (SRF) is a normal form for PBESs whose structure is similar to that of a linear process specification (LPS).
A PBES :math:`\mathcal{E}` is in standard recursive form (SRF) iff for all :math:`\sigma_i X_i(d:D) = \varphi \in \mathcal{E}`, where
:math:`\varphi` is either disjunctive or conjunctive, i.e., the equation for :math:`X_i` has the shape:

.. math::

  \sigma_i X_i(d:D) = \bigvee\limits_{j \in J_i} \exists e_j: E_j . f_{ij}(d,e_j) \land X_{g_{ij}}(h_{ij}(d, e_j))

or

.. math::

  \sigma_i X_i(d:D) = \bigwedge\limits_{j \in J_i} \forall e_j: E_j . f_{ij}(d,e_j) \implies X_{g_{ij}}(h_{ij}(d, e_j)),

where :math:`d = (d_1, \ldots, d_m)`. Below the transformation rules for converting a PBES to standard recursive form are given.

.. math::
   :nowrap:

   \begin{alignat*}{3}
   & R_{\lor,\sigma}(f, V)                       && = &&\; \langle \exists d \ap \singletonD \ldotp f \land X_\TRUE(\singletonelem), \emptyset \rangle\\
   & R_{\lor,\sigma}(X(e), V)                    && = &&\; \langle \exists d \ap \singletonD \ldotp \mathit{true} \land X(e), \emptyset \rangle\\
   & R_{\lor,\sigma}(\varphi \land f, V)         && = &&\; \langle \bigvee_{i \in I^\varphi} \exists {W_i} \ldotp f \land f_i \land X_i(g_i), \PBES^\varphi \rangle \\
   & ~                                           && ~ &&\; \quad \text{where } \langle \bigvee_{i \in I^\varphi} \exists {W_i} \ldotp f_i \land X_i(g_i), \PBES^\varphi \rangle = R_{\lor,\sigma}(\varphi, V)\\
   & R_{\lor,\sigma}(f \land \varphi, V)         && = &&\; \langle \bigvee_{i \in I^\varphi} \exists {W_i} \ldotp f \land f_i \land X_i(g_i), \PBES^\varphi \rangle \\
   & ~                                           && ~ &&\; \quad \text{where } \langle \bigvee_{i \in I^\varphi} \exists {W_i} \ldotp f_i \land X_i(g_i), \PBES^\varphi \rangle = R_{\lor,\sigma}(\varphi, V)\\
   & R_{\lor,\sigma}(\varphi \land \psi, V)      && = &&\; \langle \exists d \ap \singletonD \ldotp \mathit{true} \land \tilde{X}(V), (\sigma\tilde{X}(V) = \varphi \land \psi) \rangle \\
   & ~                                           && ~ &&\; \quad \text{where } \langle \bigvee_{i \in I^\varphi} \exists {W_i} \ldotp f_i \land X_i(g_i), \PBES^\varphi \rangle = R_{\lor,\sigma}(\varphi, V)\\
   & ~                                           && ~ &&\; \quad \phantom{\text{where }} \langle \bigvee_{i \in I^\psi} \exists {W_i} \ldotp f_i \land X_i(g_i), \PBES^\psi \rangle = R_{\lor,\sigma}(\psi, V)\\
   & R_{\lor,\sigma}(\varphi \lor \psi, V)       && = &&\; \langle \bigvee_{i \in I^\varphi \cup I^\psi} \exists {W_i} \ldotp f_i \land X_i(g_i), \PBES^\varphi\PBES^\psi \rangle\\
   & R_{\lor,\sigma}(\forall W\ldotp f, V)       && = &&\; \langle \exists d \ap \singletonD \ldotp (\forall W\ldotp f) \land X_\TRUE(\singletonelem), \emptyset \rangle\\
   & R_{\lor,\sigma}(\forall W\ldotp \varphi, V) && = &&\; \langle \exists d \ap \singletonD \ldotp \mathit{true} \land \tilde{X}(V), (\sigma\tilde{X}(V) = \forall W \ldotp \varphi) \rangle\\
   & R_{\lor,\sigma}(\exists W\ldotp \varphi, V)    && = &&\; \langle \bigvee_{i \in I^\varphi} \exists {W \cup W_i} \ldotp f_i \land X_i(g_i), \PBES^\varphi \rangle\\
   & ~                                           && ~ &&\; \quad \text{where } \langle \bigvee_{i \in I^\varphi} \exists {W_i} \ldotp f_i \land X_i(g_i), \PBES^\varphi \rangle = R_{\lor,\sigma}(\varphi, V \cup W)
   \end{alignat*}

Where :math:`R_\land` is defined dually to :math:`R_\lor`.

.. math::
   :nowrap:

   \begin{alignat*}{3}
   & R_{\land,\sigma}(f, V)                       && = &&\; \langle \forall d \ap \singletonD \ldotp \neg f \Rightarrow X_\FALSE(\singletonelem), \emptyset \rangle\\
   & R_{\land,\sigma}(X(e), V)                    && = &&\; \langle \forall d \ap \singletonD \ldotp \mathit{true} \Rightarrow X(e), \emptyset \rangle\\
   & R_{\land,\sigma}(\varphi \land \psi, V)      && = &&\; \langle \bigwedge_{i \in I^\varphi \cup I^\psi} \forall {W_i} \ldotp f_i \Rightarrow X_i(g_i), \PBES^\varphi\PBES^\psi \rangle\\
   & R_{\land,\sigma}(\varphi \lor f, V)          && = &&\; \langle \bigwedge_{i \in I^\varphi} \forall {W_i} \ldotp (\neg f \land f_i) \Rightarrow X_i(g_i), \PBES^\varphi \rangle \\
   & ~                                            && ~ &&\; \quad \text{where } \langle \bigwedge_{i \in I^\varphi} \forall {W_i} \ldotp f_i \Rightarrow X_i(g_i), \PBES^\varphi \rangle = R_{\land,\sigma}(\varphi, V)\\
   & R_{\land,\sigma}(f \lor \varphi, V)          && = &&\; \langle \bigwedge_{i \in I^\varphi} \forall {W_i} \ldotp (\neg f \land f_i) \Rightarrow X_i(g_i), \PBES^\varphi \rangle \\
   & ~                                            && ~ &&\; \quad \text{where } \langle \bigwedge_{i \in I^\varphi} \forall {W_i} \ldotp f_i \Rightarrow X_i(g_i), \PBES^\varphi \rangle = R_{\land,\sigma}(\varphi, V)\\
   & R_{\land,\sigma}(\varphi \lor \psi, V)       && = &&\; \langle \forall d \ap \singletonD \ldotp \mathit{true} \Rightarrow \tilde{X}(V), (\sigma\tilde{X}(V) = \varphi \lor \psi) \rangle\\
   & ~                                            && ~ &&\; \quad \text{where } \langle \bigwedge_{i \in I^\varphi} \forall {W_i} \ldotp f_i \Rightarrow X_i(g_i), \PBES^\varphi \rangle = R_{\land,\sigma}(\varphi, V)\\
   & ~                                            && ~ &&\; \quad \phantom{\text{where }} \langle \bigwedge_{i \in I^\psi} \forall {W_i} \ldotp f_i \Rightarrow X_i(g_i), \PBES^\psi \rangle = R_{\land,\sigma}(\psi, V)\\
   & R_{\land,\sigma}(\forall W\ldotp \varphi, V)    && = &&\; \langle \bigwedge_{i \in I^\varphi} \forall {W \cup W_i} \ldotp f_i \Rightarrow X_i(g_i), \PBES^\varphi \rangle\\
   & ~                                            && ~ &&\; \quad \text{where } \langle \bigwedge_{i \in I^\varphi} \forall {W_i} \ldotp f_i \Rightarrow X_i(g_i), \PBES^\varphi \rangle = R_{\land,\sigma}(\varphi, V \cup W)\\
   & R_{\land,\sigma}(\exists W\ldotp f, V)       && = &&\; \langle \forall d \ap \singletonD \ldotp (\exists W\ldotp f) \Rightarrow X_\FALSE(\singletonelem), \emptyset \rangle\\
   & R_{\land,\sigma}(\exists W\ldotp \varphi, V) && = &&\; \langle \forall d \ap \singletonD \ldotp \mathit{true} \Rightarrow \tilde{X}(V), (\sigma\tilde{X}(V) = \exists W \ldotp \varphi) \rangle
   \end{alignat*}

For `pbessolvesymbolic` we have introduced a so-called pre-SRF form where
all :math:`f_{ij}` can contain propositional variables related to identifying
counter example information, these are called `Zpos` and `Zneg` by convention.
We refer to these as pseudo-simple formulas.
In the transformation from PBES to pre-SRF we have the rules above, where all
`f` are pseudo-simple formulas instead of simple formulas, and in addition to that we
introduce the following rules (subsuming the corresponding original rules):

.. math::
   :nowrap:

   \begin{align*}
   & R_{\lor,\sigma}((\bigvee_{i \in I} f_i) \lor (\bigvee_{j \in J} \varphi_j), V)       && = &&\; \langle
   (\bigvee_{i \in I} f_i \land X_{\mathit{true}}) \lor \bigvee_{k \in \bigcup_{j \in J} I^{\varphi_j}} \exists {W_k} \ldotp (\lnot \bigvee_{i \in I} f_i) \land f_k \land
   X_k(g_k), \PBES^{\varphi_{1}}\cdots \PBES^{\varphi_{|J|}} \rangle\\
   \end{align*}

where :math:`f_i` are simple formulas, and dually for :math:`R_{\land,\sigma}`:

.. math::
   :nowrap:


   \begin{align*}
   R_{\land,\sigma}((\bigwedge_{i \in I} f_i) \land (\bigwedge_{j \in J} \varphi_j), V)      & =  \langle   \left(
   \bigwedge_{i \in I} (\lnot f_i \implies X_{\mathit{false}}) \right) \land \bigwedge_{k \in \bigcup_{j \in J} I^{\varphi_j}} \forall {W_k} \ldotp (\bigwedge_{i \in I} f_i ) \land f_k
   \implies X_k(g_k), \PBES^{\varphi_{1}}\cdots \PBES^{\varphi_{|J|}} \rangle\\
   \end{align*}

Note the latter two rules are introduced to ensure that counterexamples that are
computed are a subset of the original state space.
