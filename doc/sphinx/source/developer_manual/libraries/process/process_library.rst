Introduction
============

The Process library contains classes and algorithms for general processes.
The code in the Process library is contained in the namespace ``process``.

The following C++ classes are defined for the Process Library:

* :cpp:class:`process_specification    <mcrl2::process::process_specification>`
* :cpp:class:`process_equation         <mcrl2::process::process_equation>`
* :cpp:class:`process_identifier       <mcrl2::process::process_identifier>`
* :cpp:class:`rename_expression        <mcrl2::process::rename_expression>`
* :cpp:class:`communication_expression <mcrl2::process::communication_expression>`
* :cpp:class:`action_name_multiset     <mcrl2::process::action_name_multiset>`

Process expressions
-------------------
Process expressions are defined using the following grammar

.. math::

   \begin{array}{lrl}
      p & ::= &            a(e_1, \ldots, e_n)
                \: \mid \: P(e_1, \ldots, e_n)
                \: \mid \: P(d_1 = e_1, \ldots, d_n = e_n)
                \: \mid \: \delta
                \: \mid \: \tau
                \: \mid \: \sum\limits_{d_1:D_1, \ldots, d_n:D_n}p
       \\   & ~ &
                \: \mid \: \partial _{B}(p)
                \: \mid \: \tau _{I}(p)
                \: \mid \: \rho _{R}(p)
                \: \mid \: \Gamma _{C}(p)
                \: \mid \: \bigtriangledown _{V}(p)
                \: \mid \: p\ |\ p
                \: \mid \: p^{@}t
                \: \mid \: p\cdot p
       \\   & ~ &
                \: \mid \: c\rightarrow p
                \: \mid \: c\rightarrow p\diamond p
                \: \mid \: p << p
                \: \mid \: p\ ||\ p
                \: \mid \: p\ ||\_\ p
                \: \mid \: p + p,
   \end{array}

where :math:`a(e_1, \ldots, e_n)` is an action, :math:`d_i` is a variable of sort :math:`D_i`, :math:`e_i` is a data expression,
:math:`B` and :math:`I` are lists of strings, :math:`R` is a list of rename expressions, :math:`C` is a list of communication
expressions, :math:`V` is a list of action name multi sets, :math:`t` is a data expression of sort Real,
and :math:`c` is a data expression of sort Bool.

The following C++ classes are defined for process expressions:

.. table:: process expression classes

   ============================================  ===============================================================================
   Expression                                    C++ class
   ============================================  ===============================================================================
   :math:`a(e_1, \ldots, e_n)`                   :cpp:class:`lps::action                 <mcrl2::lps::action>`
   :math:`P(e_1, \ldots, e_n)`                   :cpp:class:`process_instance            <mcrl2::process::process_instance>`
   :math:`P(d_1 = e_1, \ldots, d_n = e_n)`       :cpp:class:`process_instance_assignment <mcrl2::process::process_instance_assignment>`
   :math:`\delta`                                :cpp:class:`delta                       <mcrl2::process::delta>`
   :math:`\tau`                                  :cpp:class:`tau                         <mcrl2::process::tau>`
   :math:`\sum\limits_{d:D}p`                    :cpp:class:`sum                         <mcrl2::process::sum>`
   :math:`\partial _{B}(p)`                      :cpp:class:`block                       <mcrl2::process::block>`
   :math:`\tau _{B}(p)`                          :cpp:class:`hide                        <mcrl2::process::hide>`
   :math:`\rho _{R}(p)`                          :cpp:class:`rename                      <mcrl2::process::rename>`
   :math:`\Gamma _{C}(p)`                        :cpp:class:`comm                        <mcrl2::process::comm>`
   :math:`\bigtriangledown _{V}(p)`              :cpp:class:`allow                       <mcrl2::process::allow>`
   :math:`p\ |\ p`                               :cpp:class:`sync                        <mcrl2::process::sync>`
   :math:`p^{@}t`                                :cpp:class:`at                          <mcrl2::process::at>`
   :math:`p\cdot p`                              :cpp:class:`seq                         <mcrl2::process::seq>`
   :math:`c\rightarrow p`                        :cpp:class:`if_then                     <mcrl2::process::if_then>`
   :math:`c\rightarrow p\diamond p`              :cpp:class:`if_then_else                <mcrl2::process::if_then_else>`
   :math:`p << p`                                :cpp:class:`bounded_init                <mcrl2::process::bounded_init>`
   :math:`p\ ||\ p`                              :cpp:class:`merge                       <mcrl2::process::merge>`
   :math:`p\ ||\_\ p`                            :cpp:class:`left_merge                  <mcrl2::process::left_merge>`
   :math:`p + p`                                 :cpp:class:`choice                      <mcrl2::process::choice>`
   ============================================  ===============================================================================

Algorithms on processes
-----------------------

.. table:: Algorithms on processes

   ====================================================================================   =============================================================
   algorithm                                                                              description
   ====================================================================================   =============================================================
   :cpp:class:`alphabet_reduction     <mcrl2::process::alphabet_reduction>`               Applies alphabet reduction to a process specification
   :cpp:func:`is_linear               <mcrl2::process::is_linear>`                        Determines if a process specification is linear
   :cpp:func:`rewrite                 <mcrl2::process::rewrite>`                          Applies a rewriter to a process data type
   :cpp:func:`normalize_sorts         <mcrl2::process::normalize_sorts>`                  Applies sort normalization to a process data type
   :cpp:func:`translate_user_notation <mcrl2::process::translate_user_notation>`          Applies translation of user notation to a process data type
   ====================================================================================   =============================================================

Search and Replace functions
----------------------------

.. table:: Search and Replace functions

   ===============================================================================  =============================================================================
   algorithm                                                                                                          description
   ===============================================================================  =============================================================================
   :cpp:func:`find_identifiers         <mcrl2::process::find_identifiers>`          Finds all identifiers occurring in a process data type
   :cpp:func:`find_sort_expressions    <mcrl2::process::find_sort_expressions>`     Finds all sort expressions occurring in a process  data type
   :cpp:func:`find_function_symbols    <mcrl2::process::find_function_symbols>`     Finds all function symbols occurring in a process  data type
   :cpp:func:`find_variables           <mcrl2::process::find_variables>`            Finds all variables occurring in a process  data type
   :cpp:func:`find_free_variables      <mcrl2::process::find_free_variables>`       Finds all free variables occurring in a process  data type
   :cpp:func:`replace_sort_expressions <mcrl2::process::replace_sort_expressions>`  Replaces sort expressions in a process data type
   :cpp:func:`replace_data_expressions <mcrl2::process::replace_data_expressions>`  Replaces data expressions in a process data type
   :cpp:func:`replace_variables        <mcrl2::process::replace_variables>`         Replaces variables in a process data type
   :cpp:func:`replace_free_variables   <mcrl2::process::replace_free_variables>`    Replaces free variables in a process data type
   ===============================================================================  =============================================================================
