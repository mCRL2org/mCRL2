" Vim syntax file
" Language:   GenSpect
" Extension:  gs
" Maintainer: Aad Mathijssen <A.H.J.Mathijssen@tue.nl>
" Date:       08-11-2004

syntax clear

syntax case match

syntax keyword gsSpecKW     sort cons map var eqn act proc init struct
syntax keyword gsProcCKW    delta tau
syntax keyword gsProcKW     sum restrict allow hide rename comm
syntax keyword gsSortCKW    Bool Pos Nat Int
syntax keyword gsSortKW     List Set Bag
syntax keyword gsDataCKW    true false
syntax keyword gsDataKW     whr end lambda forall exists div mod in
syntax keyword gsTodo       contained TODO FIXME XXX

syntax region gsComment     start=/%/ end='$' contains=gsTodo

syntax match gsIdentifier   /[a-zA-Z_][a-zA-Z0-9_]*/
syntax match gsOperator     /[.+|&<>:;=#@(){}\[\],!*?\\\-]/
syntax match gsOperator     /||_/
syntax match gsOperator     /->/
syntax match gsNumber       /0/
syntax match gsNumber       /\-\?[1-9][0-9]*/

highlight link gsSpecKW     Special 
highlight link gsProcCKW    Constant 
highlight link gsProcKW     Operator
highlight link gsSortCKW    Type 
highlight link gsSortKW     Type 
highlight link gsDataCKW    Constant 
highlight link gsDataKW     Operator
highlight link gsTodo       Todo 
highlight link gsComment    Comment
highlight link gsIdentifier Identifier
highlight link gsOperator   Operator 
highlight link gsNumber     Number
