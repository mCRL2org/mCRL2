A Telephone Book
----------------

**Contribution of this section**

#. use of functions with updates,
#. modelling considerations,
#. realistic verification process and its problems.

**New tools**:
none.

In this section we describe the model of a telephone book in
mCRL2. This example is based on the Phone Book example in [CORSS95]_.
We base our model on the following requirements:

#. A phone book shall store phone numbers.
#. It shall be possible to add and delete entries from a phone book.
#. It shall be possible to retrieve a phone number given a name.

By looking at these requirements, we identify the following entities:

* phone book
* phone number
* name

We start by giving abstract types for phone numbers and names; their concrete form
will be given later. For the phone book we decide that it is a mapping of names
to numbers. This gives us the following specification of sorts.

.. code-block:: mcrl2

  sort Name;
       PhoneNumber;
       PhoneBook = Name -> PhoneNumber;

As a user, you need to be aware that the types as given here, are predetermined.
This means that they cannot be extended on the fly. As a consequence, all names
and phone numbers that can ever be added to the phone book must be known upfront.

If we again look at the requirements, our phone book must support the following
operations:

* *addPhone* Adds a phone number for a name.
* *delPhone* Deletes a phone number corresponding to a name.
* *findPhone* Finds the phone number corresponding to a name.

These operations will be the *actions* of our process. We need to decide
on the parameters that the actions are going to take. We assume that our process
will support a single phone book, i.e. the process itself model a phone book. It
is then natural to model ``addPhone`` with parameters ``Name`` and ``PhoneNumber``,
``delPhone`` with a ``Name``, and ``findPhone`` with a ``Name``.
This gives rise to the following action specification.

.. code-block:: mcrl2

  act  addPhone: Name # Number;
       delPhone: Name;
       findPhone: Name;

We now need to take care that not every number is in every phone book. In order
to describe a phone book as a total function, we introduce a special phone number,
``p0``, to indicate that a name has no associated phone number.

.. code-block:: mcrl2

  map  p0: PhoneNumber;

Given this decision, we can specify the empty phone book as the phone book that
maps every name to ``n0``.

.. code-block:: mcrl2

  lambda n: Name . p0;

In modelling the empty phone book we use lambda abstraction. In this expression
``lambda n: Name`` says that we are defining a function that takes arguments of
type ``Name``, and for each name produces ``p0`` as a result. As ``p0`` is of
type ``PhoneNumber``, ``lambda n: Name . p0`` describes a function of type
``Name ->PhoneNumber``, which is by definition equal to ``PhoneBook``.

Given a function ``b`` of type ``PhoneBook``, a name ``n`` and a phone number
``p``, we can set the value of ``n`` in ``b`` to ``p`` using the expression
``b[n -> p]``. This has as property that, for all names ``m != n``, ``b[n ->
p](m) = b(m)``, and ``b[n -> p](n) = p``.

Using the above ingredients, we can model a simple phone book using the
following specification, that is also available as
:download:`phonebook1.mcrl2 <files/phonebook1.mcrl2>`.

.. literalinclude:: files/phonebook1.mcrl2
   :language: mcrl2

.. admonition:: Exercise

  There are some obvious flaws in the phone book that we have specified. Can you
  find and explain them?

In the previous specification, the "special" phone number ``p0`` can be assigned
to a name freely. Furthermore, a ``findPhone`` action can be performed, but the
actual phone number is never reported.

.. admonition:: Exercise

   Fix these issues in the above specification.

Preventing the assignment on ``p0`` to a name can easily prevented by guarding
the ``addPhone`` action with ``p != p0``. Fixing the second issue requires some
more thought. There are two possible ways around fixing the problem. We can
either assume that reporting of the result is immediate, and add the resulting
phone number as a parameter to the ``findPhone`` action, or we can assume that
querying for a phone number is asynchronous, and my take time, and split the
query into the action initiating the query (``findPhone``) and an action
reporting the result, e.g. ``reportPhone``.

The first approach is suitable when, e.g., modelling a phone book that is a library
in a synchronous program in, say, C or Java. In that case indeed the program
pointer of the calling program will not change before the result has been returned,
making a model in which reporting the result a faithful representation of reality.

If we are, e.g., modelling a phone book that is a web service, where a client
performs a request, and in the meantime may do other kinds of actions like sending
requests to other web services, the previous approach provides too coarse an
abstraction. In this case it is more accurate to use the second approach, in
which performing the query and obtaining the result are truely decoupled.

The following specification (available as :download:`phonebook2a.mcrl2 <files/phonebook2a.mcrl2>`)
gives the first variation, in which the result is
obtained instantaneously. Watch the extra parameter to ``addPhone``.

.. literalinclude:: files/phonebook2a.mcrl2
   :language: mcrl2

The second variation is given by the following specification (available as
:download:`phonebook2b.mcrl2 <files/phonebook2b.mcrl2>`).

.. literalinclude:: files/phonebook2b.mcrl2
   :language: mcrl2

In the rest of this tutorial we will stick to the specification with asynchronous
reporting.

In complex specifications, it can be convenient to introduce additional functions,
with descriptive names, that take care of the modifications of parameters that
is done in the process. As a bonus this usually makes it easier to change the
data structures used in a specification.

.. admonition:: Exercise

   Modify the specification in :download:`phonebook2b.mcrl2 <files/phonebook2b.mcrl2>`
   by adding functions ``emptybook, add_phone, del_phone`` and ``find_phone`` with
   the following signatures.

   .. code-block:: mcrl2

      map  emptybook: PhoneBook;
           add_phone: PhoneBook # Name # PhoneNumber -> PhoneBook;
           del_phone: PhoneBook # Name -> PhoneBook;
           find_phone: PhoneBook # Name -> PhoneNumber;

A solution to the above exercise is given by the specification in
:download:`phonebook3.mcrl2 <files/phonebook3.mcrl2>`.

It should be noted that, instead of using a function of names to phone numbers,
we could also have modelled the phone book using a set of pairs of names and
phone numbers. A model using sets is likely to become complicated in this case.

.. admonition:: Exercise

  Modify the specification in :download:`phonebook3.mcrl2 <files/phonebook3.mcrl2>`
  such that it uses a set of pairs of names and
  phone numbers instead of function from names to phone numbers to store the phone
  numbers internally.

A possible solution to the above exercise is given in
:download:`phonebook4.mcrl2 <files/phonebook4.mcrl2>`.
Note that the function ``find_phone`` cannot be implemented using sets, because no concrete
elements can be taken from the set. Therefore, the functionality of ``find_phone``
is modelled using the ``sum`` operator on a process level.

In the rest of this chapter we stick to the model in which functions occur
directly in the specification (:download:`phonebook2b.mcrl2 <files/phonebook2b.mcrl2>`).
We are going to check whether our model makes sense. A suitable property for our
specification is: "if a name ``n`` with phone number ``p`` is added to the phone
book, and a lookup of name ``n`` is performed, then phone number ``p`` should be
reported, provided that in the meantime no other phone number has been added for
name ``n``, and the phone number for name ``n`` has not been deleted''.
We can write this in ASCII notation as follows:

.. code-block:: mcrl2

  forall n: Name, p,r: PhoneNumber .
    [true* . addPhone(n,p) .
       !(delPhone(n) || exists q: PhoneNumber . addPhone(n, q))* .
       findPhone(n) .
       !(delPhone(n) || exists q: PhoneNumber . addPhone(n, q))* .
       reportPhone(n, r)] val(p == r)

We can try checking this property using the following command::

  $ mcrl22lps -D phonebook2b.mcrl2 | lps2pbes -f phonebook1.mcf | pbes2bool

The tools fail to verify this requirement, and give throw the following error::

  error: Cannot find a term of sort Name

The tool is telling you that it wants to find some representative term of sort
``Name``, but is not able to do so. This indeed makes sense as we have not
given a specification of names and phone numbers yet. We can try to verify this
requirement for a specification in which we have some fixed set of names and
numbers, as is given in the following specification (also available as
:download:`phonebook5.mcrl2 <files/phonebook5.mcrl2>`).

.. literalinclude:: files/phonebook5.mcrl2
   :language: mcrl2

The specification is now easily checked using the following sequence of
commands::

  $ mcrl22lps -D phonebook5.mcrl2 | lps2pbes -f phonebook.mcf | pbes2bool

.. admonition:: Exercise

  Verify whether the following property holds for
  :download:`phonebook5.mcrl2 <files/phonebook5.mcrl2>`.
  "if a name ``n`` with phone number ``p`` is added to the phone book,
  and a lookup of name ``n`` is performed, then phone number ``p`` should be
  reported, provided that in the meantime the phone number for name
  ``n`` has not been deleted". You first need to formalise this property as a 
  :math:`\mu`-calculus formula, and then verify whether it holds. Explain the outcome
  of the verification.

We see that this verification fails, because ``addPhone`` allows you to add
a phone number for a person that already has a phone number. If a new phone
number is added for such a person, the original phone number is overwritten.

.. admonition:: Exercise

  Modify the specification in :download:`phonebook5.mcrl2
  <files/phonebook5.mcrl2>` such that ``addPhone(n,p)`` can only be executed if no
  phone number for name ``n`` is known. Furthermore, extend the specification with
  and action ``changePhone`` with the following signature

  .. code-block:: mcrl2

     changePhone: Name # PhoneNumber

  such that ``changePhone(n,p)`` can only be executed if ``n`` already has
  a phone number, and that afterwards the phone number of ``n`` has been updated to
  ``p``.

.. admonition:: Exercise

  Verify whether your new specification satisfies the property you formulated
  before. Explain the outcome.

.. admonition:: Exercise

  If the verification in the previous exercise failed, think about the influence
  of the ``changePhone`` action on the validity of the property you are trying
  to check. Describe the changed property in natural language, give the modal
  :math:`\mu`-calculus formula, and do the verification.

