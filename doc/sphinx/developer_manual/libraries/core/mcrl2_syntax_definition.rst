mCRL2 Syntax Definition
=======================

This document describes the syntax of mCRL2 expressions and specifications.
We present the syntax in a rich text format.
In :ref:`sec-symbols` a translation of rich text to plain text is given, which is needed
for using the toolset.

Throughout this document, suggestive dots (:math:`\boldsymbol{\ldots}`,
:math:`\boldsymbol{\cdots}`) are used to indicate repeating patterns with one or more
occurrence. Furthermore, :math:`\boldsymbol{|}` distinguishes alternatives (not to be
mistaken with the pipe :math:`|`),
:math:`\boldsymbol{(}\mathit{pattern}\boldsymbol{)^{+}}` indicates one or more occurrences
of :math:`\mathit{pattern}`, and
:math:`\boldsymbol{(}\mathit{pattern}\boldsymbol{)^{*}}` indicates zero or more occurrences
of :math:`\mathit{pattern}`.
As opposed to real EBNF, we do not use quotes to separate the terminals from the
non-terminals.

Lexical Syntax
--------------

We define the notions of identifiers, numbers, whitespace and comments:

* An *identifier* is a string matching the pattern
  :math:`[A\text{-}Za\text{-}z\_][A\text{-}Za\text{-}z\_0\text{-}9]^*`,
  excluding the following reserved words:

  .. code-block:: none

     sort    cons    map     var     eqn     act     glob    proc    pbes    init
     struct  Bool    Pos     Nat     Int     Real    List    Set     Bag
     true    false   if      div     mod     in      lambda  forall  exists  whr     end
     delta   tau     sum     block   allow   hide    rename  comm
     val     mu      nu      delay   yaled   nil

  Identifiers are used for representing sort names :math:`b`, function names :math:`f`,
  data variable names :math:`x`, action names :math:`a`, process reference names :math:`P`,
  and propositional variable names :math:`X`.

* A *number* is a string that matches the pattern :math:`0` or :math:`[1\text{-}9][0\text{-}9]^*`.

* Spaces, tabs and newlines are treated as *whitespace*.

* A :math:`\%`-sign indicates the beginning of a *comment* that extends to the end of the
  line.


Data Specifications
-------------------

Sort expressions :math:`s`:

.. math::

   \begin{array}{lrl}
   s       & ::= & b \mid s \to s \mid \sbool \mid \spos \mid \snat \mid \sint \mid \sreal \\
           & \mid & \kwstruct\ \f{scs} \mid \cdots \mid \f{scs} \mid \mathsf{List}(s) \mid \mathsf{Set}(s) \mid \mathsf{Bag}(s) \mid (s) \\
   \f{scs} & ::= & f \mid f(\f{spj},\ldots,\f{spj}) \mid f\,{?}\,f \mid f(\f{spj},\ldots,\f{spj})\,{?}\,f \\
   \f{spj} & ::= & s \mid s \times \cdots \times s \to s \mid f{:}s \mid f{:}s \times \cdots \times s \to s
   \end{array}

Here :math:`\f{scs}` and :math:`\f{spj}` stand for the constructor and projection functions
of a structured sort. The binary operator :math:`\to` associates to the right.

Data expressions :math:`d`:

.. math::

   \begin{array}{lrl}
   d        & ::= & x \mid f \mid d(d,\ldots,d) \mid N \mid \mathsf{true} \mid \mathsf{false} \mid \mathsf{if} \mid \lnot d \mid {-}d \mid \overline{d} \mid \#d \mid d \oplus d \\
            & \mid & \el \mid [d,\ldots,d] \mid \{\,\} \mid \{d,\ldots,d\} \mid \{d{:}d,\ldots,d{:}d\} \mid \{x{:}s \mid d\} \\
            & \mid & \lambda_{\f{mvd},\ldots,\f{mvd}} d \mid \forall_{\f{mvd},\ldots,\f{mvd}} d \mid \exists_{\f{mvd},\ldots,\f{mvd}} d \mid d\ \kwwhr\ x{=}d,\ldots,x{=}d\ \kwend \\
            & \mid & (d) \\
   \f{mvd}  & ::= & x,\ldots,x{:}s \\
   \oplus   & ::= & {*} \mid {.} \mid \cap \mid {/} \mid \kwdiv \mid \kwmod \mid {+} \mid {-} \mid \cup \mid \concat \mid \snoc \mid \cons \\
            & \mid & {<} \mid {\leq} \mid {\geq} \mid {>} \mid \subset \mid \subseteq \mid \in \mid \approx \mid \not\approx \mid \land \mid \lor \mid \limp
   \end{array}

Here :math:`\f{mvd}` stands for a multiple data variable declaration, :math:`\oplus` for a
binary operator, and :math:`N` for a number. The unary operators have the highest priority,
followed by the infix operators, followed by :math:`\lambda`, :math:`\forall` and
:math:`\exists`, followed by :math:`\kwwhr\ \kwend`. The descending order of precedence of
the infix operators is:
:math:`\{{*}, {.}, \cap\}`,
:math:`\{{/}, \kwdiv, \kwmod\}`,
:math:`\{{+}, {-}, \cup\}`,
:math:`\concat`,
:math:`\snoc`,
:math:`\cons`,
:math:`\{{<}, {\leq}, {\geq}, {>}, \subset, \subseteq, \in\}`,
:math:`\{\approx, \not\approx\}`,
:math:`\{\land, \lor\}`,
:math:`\limp`.
Of these operators :math:`{*}`, :math:`{.}`, :math:`\cap`, :math:`{/}`, :math:`\kwdiv`,
:math:`\kwmod`, :math:`{+}`, :math:`{-}`, :math:`\cup` and :math:`\concat` associate to the
left and :math:`\approx`, :math:`\not\approx`, :math:`\land`, :math:`\lor` and :math:`\limp`
associate to the right.

Data specifications :math:`\f{data\_spec}`:

.. math::

   \begin{array}{lrl}
   \f{data\_spec} & ::= & \kwsort\ (\f{sd};)^{+} \\
                  & \mid & \kwcons\ (\f{mfd};)^{+} \\
                  & \mid & \kwmap\ (\f{mfd};)^{+} \\
                  & \mid & \kwvar\ (\f{mvd};)^{+}\ \kweqn\ (\f{ed};)^{+} \\
                  & \mid & \kweqn\ (\f{ed};)^{+} \\
   \f{sd}         & ::= & b \mid b = s \\
   \f{mfd}        & ::= & f,\ldots,f{:}s \\
   \f{ed}         & ::= & d = d \mid c \to d = d
   \end{array}

Here, :math:`\f{sd}` stands for sort declaration, :math:`\f{mfd}` for multiple function
declaration, :math:`\f{ed}` for equation declaration, and :math:`\f{ad}` for action
declaration.


Process Specifications
----------------------

Process expressions :math:`p`:

.. math::

   \begin{array}{lrl}
   p       & ::= & a \mid \delta \mid \tau \mid p \alt p \mid p \seq p \mid P \mid p \sync p \mid p \pmerge p \mid p \lmerge p \\
           & \mid & \allow{\set{\f{as},\ldots,\f{as}}}(p) \mid \block{\set{a,\ldots,a}}(p) \mid \hide{\set{a,\ldots,a}}(p) \mid \ren{\set{\f{ar},\ldots,\f{ar}}}(p) \mid \comm{\set{\f{ac},\ldots,\f{ac}}}(p) \\
           & \mid & a(d,\ldots,d) \mid P(d,\ldots,d) \mid P() \mid P(x{=}d,\ldots,x{=}d) \\
           & \mid & c \to p \diamond p \mid c \to p \mid \sum_{\f{mvd},\ldots,\f{mvd}} p \\
           & \mid & p\,\mathbin{@}\,t \mid t \pinit p \mid p \pbefore q \\
           & \mid & (p) \\
   \f{as}  & ::= & a \mid \cdots \mid a \\
   \f{ar}  & ::= & a \to a \\
   \f{ac}  & ::= & a \sync \f{as} \to a \mid a \sync \f{as} \to \tau \mid a \sync \f{as}
   \end{array}

Here, :math:`c` and :math:`t` stand for data expressions of sort :math:`\sbool` and
:math:`\sreal`, respectively. For technical reasons, :math:`c` and :math:`t` may not have an
infix operator, a where clause or a quantifier at the top-level (parentheses should be used
instead). :math:`\f{as}` represents an action sequence, :math:`\f{ar}` an action renaming,
and :math:`\f{ac}` an action communication. The descending order of precedence of the
operators is: :math:`\sync`, :math:`\mathbin{@}`, :math:`\seq`,
:math:`\{\pinit, \pbefore\}`, :math:`\to`, :math:`\{\pmerge, \lmerge\}`, :math:`\sum`,
:math:`\alt`. Of these operators :math:`\alt`, :math:`\pmerge`, :math:`\lmerge`, :math:`\seq`
and :math:`\sync` associate to the right.

Process specifications :math:`\f{proc\_spec}`:

.. math::

   \begin{array}{lrl}
   \f{proc\_spec}      & ::= & (\f{proc\_spec\_elt})^{*} \\
   \f{proc\_spec\_elt} & ::= & \f{data\_spec} \\
                       & \mid & \kwact\ (\f{ad};)^{+} \\
                       & \mid & \kwglob\ (\f{mvd};)^{+} \\
                       & \mid & \kwproc\ (\f{pd};)^{+} \\
                       & \mid & \kwinit\ p; \\
   \f{pd}              & ::= & P = p \mid P(\f{mvd},\ldots,\f{mvd}) = p \\
   \f{ad}              & ::= & a \mid a{:}s \times \cdots \times s
   \end{array}

Here :math:`\f{proc\_spec\_elt}` represents a process specification element, :math:`\f{pd}`
a process definition, and :math:`\f{ad}` an action declaration. Furthermore, we impose the
restriction that :math:`\f{proc\_spec}` should contain precisely one occurrence of the
keyword :math:`\kwinit`.


Mu-calculus Formulae
--------------------

Multiactions :math:`\f{ma}`:

.. math::

   \begin{array}{lrl}
   \f{ma} & ::= & \tau \mid \f{pa} \mid \cdots \mid \f{pa} \\
   \f{pa} & ::= & a \mid a(d,\ldots,d)
   \end{array}

Here, :math:`\f{pa}` represents a parameterised action.

Action formulae :math:`\alpha`:

.. math::

   \begin{array}{lrl}
   \alpha & ::= & \f{ma} \mid \alpha \mathbin{@} t \mid \mathsf{val}(c) \mid (\alpha) \\
          & \mid & \mathsf{true} \mid \mathsf{false} \mid \neg\alpha \mid \alpha \land \alpha \mid \alpha \lor \alpha \mid \alpha \limp \alpha \mid \forall_{\f{mvd},\ldots,\f{mvd}}\alpha \mid \exists_{\f{mvd},\ldots,\f{mvd}}\alpha
   \end{array}

Here, :math:`c` and :math:`t` stand for data expressions of sort :math:`\sbool` and
:math:`\sreal`, respectively. For technical reasons, :math:`t` may not have an infix
operator, a where clause or a quantifier at the top-level (parentheses should be used
instead). The descending order of precedence of the operators is:
:math:`\neg`, :math:`\mathbin{@}`, :math:`\{\land, \lor\}`, :math:`\limp`,
:math:`\{\forall, \exists\}`. Of the infix operators :math:`\mathbin{@}` associates to the
left and :math:`\land`, :math:`\lor` and :math:`\limp` associate to the right.

Regular formulae :math:`\varphi_r`:

.. math::

   \begin{array}{lrl}
   \varphi_r & ::= & \alpha \mid \epsilon \mid \varphi_r \cdot \varphi_r \mid \varphi_r + \varphi_r \mid \varphi_r^{*} \mid \varphi_r^{+} \mid (\varphi_r)
   \end{array}

The postfix operators :math:`{}^{*}` and :math:`{}^{+}` have the highest priority, followed
by :math:`\cdot`, followed by infix :math:`+`. The infix operators associate to the right.

State formulae :math:`\varphi_s`:

.. math::

   \begin{array}{lrl}
   \varphi_s & ::= & [\varphi_r]\varphi_s \mid \langle\varphi_r\rangle\varphi_s \mid \nabla(t) \mid \Delta(t) \mid \nabla \mid \Delta \mid \mathsf{val}(c) \mid (\varphi_s) \\
             & \mid & \nu X.\varphi_s \mid \mu X.\varphi_s \mid \nu X(\f{vdi},\ldots,\f{vdi}).\varphi_s \mid \mu X(\f{vdi},\ldots,\f{vdi}).\varphi_s \mid X \mid X(d,\ldots,d) \\
             & \mid & \mathsf{true} \mid \mathsf{false} \mid \neg\varphi_s \mid \varphi_s \land \varphi_s \mid \varphi_s \lor \varphi_s \mid \varphi_s \limp \varphi_s \mid \forall_{\f{mvd},\ldots,\f{mvd}}\varphi_s \mid \exists_{\f{mvd},\ldots,\f{mvd}}\varphi_s \\
   \f{vdi}   & ::= & x{:}s = d
   \end{array}

Here :math:`\f{vdi}` stands for a data variable declaration and initialisation, and :math:`c`
and :math:`t` stand for data expressions of sort :math:`\sbool` and :math:`\sreal`,
respectively. For technical reasons, :math:`t` may not have an infix operator, a where clause
or a quantifier at the top-level (parentheses should be used instead). The descending order
of precedence of the operators is:
:math:`\{\neg, {\_}[{\_}], {\_}\langle{\_}\rangle\}`,
:math:`\{\land, \lor\}`,
:math:`\limp`,
:math:`\{\forall, \exists, \mu, \nu\}`.
The infix operators :math:`\land`, :math:`\lor` and :math:`\limp` associate to the right.


PBES's
------

Parameterised boolean expressions :math:`\varphi_e`:

.. math::

   \begin{array}{lrl}
   \varphi_e & ::= & \f{pvo} \mid \mathsf{val}(c) \mid (\varphi_e) \\
             & \mid & \mathsf{true} \mid \mathsf{false} \mid \neg\varphi_e \mid \varphi_e \land \varphi_e \mid \varphi_e \lor \varphi_e \mid \varphi_e \limp \varphi_e \mid \forall_{\f{mvd},\ldots,\f{mvd}}\varphi_e \mid \exists_{\f{mvd},\ldots,\f{mvd}}\varphi_e \\
   \f{pvo}   & ::= & X \mid X(d,\ldots,d)
   \end{array}

Here :math:`\f{pvo}` stands for a propositional variable occurrence. The descending order of
operator precedence is: :math:`\neg`, :math:`\{\land, \lor\}`, :math:`\limp`,
:math:`\{\forall, \exists\}`. The infix operators :math:`\land`, :math:`\lor` and
:math:`\limp` associate to the right.

Parameterised boolean equations :math:`\f{pb\_eqn}`:

.. math::

   \begin{array}{lrl}
   \f{pb\_eqn} & ::= & \sigma\,\f{pvd} = \varphi_e \\
   \sigma      & ::= & \nu \mid \mu \\
   \f{pvd}     & ::= & X \mid X(\f{mvd},\ldots,\f{mvd})
   \end{array}

Here :math:`\sigma` stands for a fixpoint symbol, and :math:`\f{pvd}` for a propositional
variable declaration.

PBES specifications :math:`\f{pbes\_spec}`:

.. math::

   \begin{array}{lrl}
   \f{pbes\_spec}      & ::= & (\f{pbes\_spec\_elt})^{*} \\
   \f{pbes\_spec\_elt} & ::= & \f{data\_spec} \\
                       & \mid & \kwglob\ (\f{mvd};)^{+} \\
                       & \mid & \kwpbes\ (\f{pb\_eqn};)^{+} \\
                       & \mid & \kwinit\ \f{pvo};
   \end{array}

Here :math:`\f{pbes\_spec\_elt}` represents a PBES specification element. We impose the
restriction that :math:`\f{pbes\_spec}` should contain precisely one occurrence of each of
the keywords :math:`\kwpbes` and :math:`\kwinit`.


.. _sec-symbols:

Table of Symbols
----------------

In the toolset, a plain text format is used as opposed to the rich text format of the
previous sections. A mapping from rich text to plain text symbols is provided below.

.. list-table:: Mapping from rich to plain text
   :header-rows: 1
   :widths: 25 35 25

   * - Symbol
     - Rich text
     - Plain text
   * - arrow
     - :math:`\to`
     - ``->``
   * - cross
     - :math:`\times`
     - ``#``
   * - diamond
     - :math:`\diamond`
     - ``<>``
   * - standard sorts
     - :math:`\sbool, \spos, \snat, \sint, \sreal`
     - ``Bool``, ``Pos``, ``Nat``, ``Int``, ``Real``
   * - equality and inequality
     - :math:`\approx, \not\approx`
     - ``==``, ``!=``
   * - logical operators
     - :math:`\lnot, \land, \lor, \limp`
     - ``!``, ``&&``, ``||``, ``=>``
   * - relational numeric operators
     - :math:`\leq, \geq`
     - ``<=``, ``>=``
   * - relational set operators
     - :math:`\in, \subseteq, \subset`
     - ``in``, ``<=``, ``<``
   * - set operators
     - :math:`\overline{\cdot}, \cup, \cap`
     - ``!``, ``+``, ``*``
   * - list operators
     - :math:`\cons, \snoc, \concat`
     - ``|>``, ``<|``, ``++``
   * - lambda abstraction
     - :math:`\lambda_{x:s}\,d`
     - ``lambda x:s.d``
   * - universal quantification
     - :math:`\forall_{x:s}\,\varphi`
     - ``forall x:s.phi``
   * - existential quantification
     - :math:`\exists_{x:s}\,\varphi`
     - ``exists x:s.phi``
   * - deadlock
     - :math:`\delta`
     - ``delta``
   * - internal action
     - :math:`\tau`
     - ``tau``
   * - left merge
     - :math:`\lmerge`
     - ``||_``
   * - sum
     - :math:`\sum_{x:s} p`
     - ``sum x:s.p``
   * - allow
     - :math:`\allow{\set{a \mid b}}(p)`
     - ``allow({a|b},p)``
   * - block
     - :math:`\block{\set{a}}(p)`
     - ``block({a},p)``
   * - hide
     - :math:`\hide{\set{a}}(p)`
     - ``hide({a},p)``
   * - rename
     - :math:`\ren{\set{a \to b}}(p)`
     - ``rename({a -> b},p)``
   * - communication
     - :math:`\comm{\set{a \mid b \to c}}(p)`
     - ``comm({a|b -> c},p)``
   * - time
     - :math:`\mathbin{@}, \pinit, \pbefore`
     - ``@``, ``>>``, ``<<``
   * - negation of ultimate delay
     - :math:`\nabla`
     - ``yaled``
   * - ultimate delay
     - :math:`\Delta`
     - ``delay``
   * - nil
     - :math:`\epsilon`
     - ``nil``
   * - fixpoint symbol
     - :math:`\nu, \mu`
     - ``nu``, ``mu``
   * - maximal fixpoint
     - :math:`\nu X(x{:}s = d).\varphi`
     - ``nu X(x:s = d).phi``
   * - minimal fixpoint
     - :math:`\mu X(x{:}s = d).\varphi`
     - ``mu X(x:s = d).phi``
