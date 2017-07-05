.. math::
   :nowrap:

    \renewcommand{\implies}{\mathop{\Rightarrow}}

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
   :math:`\cal{E}`                      :cpp:class:`pbes                                      <mcrl2::pbes_system::pbes>`
   :math:`\sigma X(d:D)=\varphi`        :cpp:class:`pbes_equation                             <mcrl2::pbes_system::pbes_equation>`
   :math:`\sigma`                       :cpp:class:`fixpoint_symbol                           <mcrl2::pbes_system::fixpoint_symbol>`
   :math:`X(d:D)`                       :cpp:class:`propositional_variable                    <mcrl2::pbes_system::propositional_variable>`
   :math:`\varphi`                      :cpp:class:`pbes_expression                           <mcrl2::pbes_system::pbes_expression>`
   :math:`d:D`                          :cpp:type:`data::variable_list                        <mcrl2::data::variable_list>`
   :math:`e`                            :cpp:type:`data::data_expression_list                 <mcrl2::data::data_expression_list>`
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
   :math:`c`                            :cpp:class:`data::data_expression                <mcrl2::data::data_expression>`
   :math:`\neg \varphi`                 :cpp:class:`not_                                 <mcrl2::pbes_system::not_>`
   :math:`\varphi \wedge \psi`          :cpp:class:`and_                                 <mcrl2::pbes_system::and_>`
   :math:`\varphi \vee \psi`            :cpp:class:`or_                                  <mcrl2::pbes_system::or_>`
   :math:`\varphi \implies \psi`        :cpp:class:`imp                                  <mcrl2::pbes_system::imp>`
   :math:`\forall d{:}D .\:\varphi`     :cpp:class:`forall                               <mcrl2::pbes_system::forall>`
   :math:`\exists d{:}D .\:\varphi`     :cpp:class:`exists                               <mcrl2::pbes_system::exists>`
   :math:`X(e)`                         :cpp:class:`propositional_variable_instantiation <mcrl2::pbes_system::propositional_variable_instantiation>`
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
   :cpp:func:`txt2pbes                     <mcrl2::pbes_system::txt2pbes>`                       Parses a textual description of a PBES
   :cpp:func:`lps2pbes                     <mcrl2::pbes_system::lps2pbes>`                       Generates a PBES from a linear process specification and a state formula
   :cpp:func:`constelm                     <mcrl2::pbes_system::constelm>`                       Removes constant parameters from a PBES
   :cpp:func:`parelm                       <mcrl2::pbes_system::parelm>`                         Removes unused parameters from a PBES
   :cpp:func:`pbesrewr                     <mcrl2::pbes_system::pbesrewr>`                       Rewrites the predicate formulae of a PBES
   :cpp:func:`pbesinst                     <mcrl2::pbes_system::pbesinst>`                       Transforms a PBES to a BES by instantiating predicate variables
   :cpp:func:`gauss_elimination            <mcrl2::pbes_system::gauss_elimination>`              Solves a PBES using Gauss elimination
   :cpp:func:`remove_parameters            <mcrl2::pbes_system::remove_parameters>`              Removes propositional variable parameters
   :cpp:func:`remove_unreachable_variables <mcrl2::pbes_system::remove_unreachable_variables>`   Removes equations that are not (syntactically) reachable from the initial state of a PBES
   :cpp:func:`is_bes                       <mcrl2::pbes_system::is_bes>`                         Returns true if a PBES data type is in BES form
   :cpp:func:`complement                   <mcrl2::pbes_system::complement>`                     Pushes negations as far as possible inwards towards data expressions
   :cpp:func:`normalize                    <mcrl2::pbes_system::normalize>`                      Brings a PBES expression into positive normal form, i.e. without occurrences of :math:`\neg` and :math:`\implies`
   ===========================================================================================   =================================================================================================================

Search and Replace functions
----------------------------

.. table:: Search and Replace functions

   ========================================================================================================================  =============================================================================
   algorithm                                                                                                                 description
   ========================================================================================================================  =============================================================================
   :cpp:func:`find_identifiers                            <mcrl2::pbes_system::find_identifiers>`                            Finds all identifiers occurring in a PBES data type
   :cpp:func:`find_sort_expressions                       <mcrl2::pbes_system::find_sort_expressions>`                       Finds all sort expressions occurring in a PBES  data type
   :cpp:func:`find_function_symbols                       <mcrl2::pbes_system::find_function_symbols>`                       Finds all function symbols occurring in a PBES  data type
   :cpp:func:`find_all_variables                          <mcrl2::pbes_system::find_variables>`                              Finds all variables occurring in a PBES  data type
   :cpp:func:`find_free_variables                         <mcrl2::pbes_system::find_free_variables>`                         Finds all free variables occurring in a PBES  data type
   :cpp:func:`find_propositional_variable_instantiations  <mcrl2::pbes_system::find_propositional_variable_instantiations>`  Finds all propositional variable instantiations occurring in a PBES data type
   :cpp:func:`replace_sort_expressions                    <mcrl2::pbes_system::replace_sort_expressions>`                    Replaces sort expressions in a PBES data type
   :cpp:func:`replace_data_expressions                    <mcrl2::pbes_system::replace_data_expressions>`                    Replaces data expressions in a PBES data type
   :cpp:func:`replace_variables                           <mcrl2::pbes_system::replace_variables>`                           Replaces variables in a PBES data
   :cpp:func:`replace_variables_capture_avoiding          <mcrl2::pbes_system::replace_variables_capture_avoiding>`          Replaces variables in a PBES data type, and avoids unwanted capturing
   :cpp:func:`replace_free_variables                      <mcrl2::pbes_system::replace_free_variables>`                      Replaces free variables in a PBES data type
   :cpp:func:`replace_all_variables                       <mcrl2::pbes_system::replace_all_variables>`                       Replaces all variables in a PBES data type, even in declarations
   :cpp:func:`replace_propositional_variables             <mcrl2::pbes_system::replace_propositional_variables>`             Replaces propositional variables in a PBES data type
   ========================================================================================================================  =============================================================================

Rewriters for PBES expressions
------------------------------
The following rewriters are available

.. table:: PBES expression rewriters

   ==================================================================================================  =========================================================================
   name                                                                                                description
   ==================================================================================================  =========================================================================
   :cpp:class:`bqnf_rewriter                   <mcrl2::pbes_system::bqnf_rewriter>`                    BQNF rewriter
   :cpp:class:`data2pbes_rewriter              <mcrl2::pbes_system::data2pbes_rewriter>`               Replaces data library operators to equivalent PBES library operators
   :cpp:class:`data_rewriter                   <mcrl2::pbes_system::data_rewriter>`                    Rewrites data expressions that appear as a subterm of the PBES expression
   :cpp:class:`enumerate_quantifiers_rewriter  <mcrl2::pbes_system::enumerate_quantifiers_rewriter>`   Eliminates quantifiers by enumerating quantifier variables
   :cpp:class:`one_point_rule_rewriter         <mcrl2::pbes_system::one_point_rule_rewriter>`          Applies one point rule to simplify quantifier expressions
   :cpp:class:`pfnf_rewriter                   <mcrl2::pbes_system::pfnf_rewriter>`                    Brings PBES expressions into PFNF normal form
   :cpp:class:`quantifiers_inside_rewriter     <mcrl2::pbes_system::quantifiers_inside_rewriter>`      Pushes quantifiers inside
   :cpp:class:`simplify_quantifiers_rewriter   <mcrl2::pbes_system::simplify_quantifiers_rewriter>`    Simplifies quantifier expressions
   :cpp:class:`simplify_rewriter               <mcrl2::pbes_system::simplify_rewriter>`                Simplifies logical boolean operators
   ==================================================================================================  =========================================================================
