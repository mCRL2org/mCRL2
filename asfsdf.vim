" Vim syntax file
" Language:   ASF+SDF
" Extension:  asf, sdf
" Maintainer: Aad Mathijssen <A.H.J.Mathijssen@tue.nl>
" Version:    1.0
" Date:       2004-10-15

syntax clear

syntax case match

set iskeyword=-,a-z,A-Z
syntax keyword asfsdfKeyword  aliases assoc avoid bracket cons context-free
syntax keyword asfsdfKeyword  constructor definition equations exports hiddens
syntax keyword asfsdfKeyword  imports LAYOUT left lexical memo module non-assoc
syntax keyword asfsdfKeyword  prefer priorities reject restrictions right sorts
syntax keyword asfsdfKeyword  start-symbols syntax traversal variables
syntax keyword asfsdfTodo     contained TODO FIXME XXX

syntax match   asfsdfSort     /[A-Z][a-zA-Z\-]\+/
syntax match   asfsdfOperator /[~*?(),+<>#={}:|\-/!]/

syntax region  asfsdfString   start=+"+  skip=+\\"+  end=+"+  end=+\\\\"+   contains=asfsdfTodo
syntax region  asfsdfString   start=+\[+ skip=+\\\]+ end=+\]+ end=+\\\\\]+  contains=asfsdfTodo
syntax region  asfsdfComment  start=/%/              end=/%/                contains=asfsdfTodo
syntax region  asfsdfComment  start=/%%/             end="$"                contains=asfsdfTodo

highlight link asfsdfKeyword  Statement
highlight link asfsdfTodo     Todo
highlight link asfsdfSort     Identifier
highlight link asfsdfOperator Operator
highlight link asfsdfString   String
highlight link asfsdfComment  Comment
