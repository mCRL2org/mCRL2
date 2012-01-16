Process specifications
======================

*Actions*

.. dparser:: ActDecl ActSpec

*Process expressions*

.. dparser:: Action ActIdSet MultActId MultActIdList MultActIdSet RenExpr 
             RenExprList RenExprSet CommExprRhs CommExpr CommExprList CommExprSet 
             DataExprUnit IfThen ProcExpr

The non-terminal :token:`ProcExprNoif` is equal to :token:`ProcExpr`, except 
that no *if-then* or *if-then-else* productions are allowed. It recurses back 
into :token:`ProcExpr` where this non-terminal is enclosed by brackets; 
otherwise, it stays in :token:`ProcExprNoIf`.

*Process equation*
 
.. dparser:: ProcDecl ProcSpec
