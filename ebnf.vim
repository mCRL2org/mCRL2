" Vim syntax file
" Language:   EBNF
" Extension:  bnf,ebnf
" Maintainer: Aad Mathijssen <A.H.J.Mathijssen@tue.nl>
" Version:    1.0
" Date:       2004-10-15

syntax clear

syntax case match

syntax keyword ebnfTodo       contained TODO FIXME XXX

syntax match   ebnfIdentifier /\a\(\a\|\d\)\+/
syntax match   ebnfOperator   /[:=|*+?()\[\]\-~]/

syntax region  ebnfString     start=+"+   end=+"+   contains=ebnfTodo
syntax region  ebnfString     start=+'+   end=+'+   contains=ebnfTodo
syntax region  ebnfRange      start=+\[+  skip=+\\\]+ end=+\]+
syntax region  ebnfComment    start=+/\*+ end=+\*/+ contains=ebnfcomment_attention,ebnfString
syntax region  ebnfComment    start=+//+  end=+$+   contains=ebnfcomment_attention,ebnfString

highlight link ebnfTodo       Todo 
highlight link ebnfIdentifier Identifier
highlight link ebnfOperator   Operator
highlight link ebnfRange      Constant
highlight link ebnfString     String
highlight link ebnfComment    Comment
