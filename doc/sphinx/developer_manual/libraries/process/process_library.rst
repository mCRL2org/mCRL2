Introduction
============

The Process library contains classes and algorithms for general processes.
The code in the Process library is contained in the namespace ``process``.

The following C++ classes are defined for the Process Library:

* :mcrl2:`process_specification    <mcrl2::process::process_specification>`
* :mcrl2:`process_equation         <mcrl2::process::process_equation>`
* :mcrl2:`process_identifier       <mcrl2::process::process_identifier>`
* :mcrl2:`rename_expression        <mcrl2::process::rename_expression>`
* :mcrl2:`communication_expression <mcrl2::process::communication_expression>`
* :mcrl2:`action_name_multiset     <mcrl2::process::action_name_multiset>`

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
   :math:`a(e_1, \ldots, e_n)`                   :mcrl2:`action                      <mcrl2::process::action>`
   :math:`P(e_1, \ldots, e_n)`                   :mcrl2:`process_instance            <mcrl2::process::process_instance>`
   :math:`P(d_1 = e_1, \ldots, d_n = e_n)`       :mcrl2:`process_instance_assignment <mcrl2::process::process_instance_assignment>`
   :math:`\delta`                                :mcrl2:`delta                       <mcrl2::process::delta>`
   :math:`\tau`                                  :mcrl2:`tau                         <mcrl2::process::tau>`
   :math:`\sum\limits_{d:D}p`                    :mcrl2:`sum                         <mcrl2::process::sum>`
   :math:`\partial _{B}(p)`                      :mcrl2:`block                       <mcrl2::process::block>`
   :math:`\tau _{B}(p)`                          :mcrl2:`hide                        <mcrl2::process::hide>`
   :math:`\rho _{R}(p)`                          :mcrl2:`rename                      <mcrl2::process::rename>`
   :math:`\Gamma _{C}(p)`                        :mcrl2:`comm                        <mcrl2::process::comm>`
   :math:`\bigtriangledown _{V}(p)`              :mcrl2:`allow                       <mcrl2::process::allow>`
   :math:`p\ |\ p`                               :mcrl2:`sync                        <mcrl2::process::sync>`
   :math:`p^{@}t`                                :mcrl2:`at                          <mcrl2::process::at>`
   :math:`p\cdot p`                              :mcrl2:`seq                         <mcrl2::process::seq>`
   :math:`c\rightarrow p`                        :mcrl2:`if_then                     <mcrl2::process::if_then>`
   :math:`c\rightarrow p\diamond p`              :mcrl2:`if_then_else                <mcrl2::process::if_then_else>`
   :math:`p << p`                                :mcrl2:`bounded_init                <mcrl2::process::bounded_init>`
   :math:`p\ ||\ p`                              :mcrl2:`merge                       <mcrl2::process::merge>`
   :math:`p\ ||\_\ p`                            :mcrl2:`left_merge                  <mcrl2::process::left_merge>`
   :math:`p + p`                                 :mcrl2:`choice                      <mcrl2::process::choice>`
   ============================================  ===============================================================================

Algorithms on processes
-----------------------

.. table:: Selected algorithms on processes

   ===============================================================================================  =========================================================================================
   algorithm                                                                                        description
   ===============================================================================================  =========================================================================================
   :mcrl2:`alphabet_reduce                  <mcrl2::process::alphabet_reduce>`                        Applies alphabet reduction to a process specification
   :mcrl2:`eliminate_single_usage_equations <mcrl2::process::eliminate_single_usage_equations>`       Eliminates equations that are used only once, using substitution
   :mcrl2:`eliminate_trivial_equations      <mcrl2::process::eliminate_trivial_equations>`            Eliminates trivial equations, that have a process instance as the right hand side
   :mcrl2:`eliminate_trivial_sums           <mcrl2::process::eliminate_trivial_sums>`                 Eliminates trivial equations, that have a sum of process instances as the right hand side
   :mcrl2:`is_guarded                       <mcrl2::process::is_guarded>`                             Checks if a process expression is guarded
   :mcrl2:`is_linear                        <mcrl2::process::is_linear>`                              Determines if a process specification is linear
   :mcrl2:`normalize_sorts                  <mcrl2::process::normalize_sorts>`                        Applies sort normalization to a process data type
   :mcrl2:`remove_duplicate_equations       <mcrl2::process::remove_duplicate_equations>`             Removes duplicate equations from a process specification, using a bisimulation algorithm
   :mcrl2:`rewrite                          <mcrl2::process::rewrite>`                                Applies a rewriter to a process data type
   :mcrl2:`translate_user_notation          <mcrl2::process::translate_user_notation>`                Applies translation of user notation to a process data type
   ===============================================================================================  =========================================================================================

Search and Replace functions
----------------------------

.. table:: Search and Replace functions

   ===================================================================================================  =============================================================================
   algorithm                                                                                                          description
   ===================================================================================================  =============================================================================
   :mcrl2:`find_identifiers                   <mcrl2::process::find_identifiers>`                        Finds all identifiers occurring in a process data type
   :mcrl2:`find_sort_expressions              <mcrl2::process::find_sort_expressions>`                   Finds all sort expressions occurring in a process  data type
   :mcrl2:`find_function_symbols              <mcrl2::process::find_function_symbols>`                   Finds all function symbols occurring in a process  data type
   :mcrl2:`find_all_variables                 <mcrl2::process::find_variables>`                          Finds all variables occurring in a process  data type
   :mcrl2:`find_free_variables                <mcrl2::process::find_free_variables>`                     Finds all free variables occurring in a process  data type
   :mcrl2:`replace_sort_expressions           <mcrl2::process::replace_sort_expressions>`                Replaces sort expressions in a process data type
   :mcrl2:`replace_data_expressions           <mcrl2::process::replace_data_expressions>`                Replaces data expressions in a process data type
   :mcrl2:`replace_variables                  <mcrl2::process::replace_variables>`                       Replaces variables in a process data
   :mcrl2:`replace_variables_capture_avoiding <mcrl2::process::replace_variables_capture_avoiding>`      Replaces variables in a process data type, and avoids unwanted capturing
   :mcrl2:`replace_free_variables             <mcrl2::process::replace_free_variables>`                  Replaces free variables in a process data type
   :mcrl2:`replace_all_variables              <mcrl2::process::replace_all_variables>`                   Replaces all variables in a process data type, even in declarations
   ===================================================================================================  =============================================================================
