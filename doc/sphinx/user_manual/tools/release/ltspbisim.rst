.. index:: ltspbisim

.. _tool-ltspbisim:

ltspbisim
=========

The tool ``ltspbisim`` reduces a transition system modulo strong probabilistic bisimulation.
The supported formats are either .aut, .lts and .fsm.

There are implemented algorithms. A fast algorithm by Groote, Rivera-Verduzco and de Vink
and a classical and much slower algorithm by Baier, Engelen and Majster-Cederbaum. The last
algorithms is primarily available for comparison purposes.

It is expected that in the future more probabilistic equivalences will be supported.

.. mcrl2_manual:: ltspbisim
