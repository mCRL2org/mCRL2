" Vim syntax file
" Language:    EBNF
" Extension:   bnf,ebnf
" Maintainer:  Aad Mathijssen <A.H.J.Mathijssen@tue.nl>
" Last Change: 1.0

syntax clear

syntax case match

syntax keyword ebnfTodo       contained TODO FIXME XXX

syntax match   ebnfIdentifier /\a\(\a\|\d\)*/
syntax match   ebnfOperator   /[:=|*+?()\[\]\-~]/

syntax region  ebnfString     start=+"+   end=+"+   contains=ebnfTodo
syntax region  ebnfString     start=+'+   end=+'+   contains=ebnfTodo
syntax region  ebnfRange      start=+\[+  skip=+\\\]+ end=+\]+
syntax region  ebnfComment    start=+/\*+ end=+\*/+ contains=ebnfTodo,ebnfString
syntax region  ebnfComment    start=+//+  end=+$+   contains=ebnfTodo,ebnfString

highlight link ebnfTodo       Todo 
highlight link ebnfIdentifier Identifier
highlight link ebnfOperator   Operator
highlight link ebnfRange      Constant
highlight link ebnfString     String
highlight link ebnfComment    Comment
