syntax clear

syntax case match

syntax keyword mCRL_keyword sort func map var rew proc act comm init For EndFor SynchronizeDecl Synchronize EndSynchronize IncludeBool IncludeNat MakeSet MakeTuple MakeDict
syntax keyword mCRL_algebra delta tau encap hide rename sum
syntax keyword mCRL_comment_attention contained TODO FIXME XXX

syntax region mCRL_comment start=/%/ end='$' contains=mCRL_comment_attention

syntax match mCRL_identifier /[a-zA-Z0-9_]\+/
syntax match mCRL_nat /$[0-9]\+/
syntax match mCRL_operator /[.+|<>:=#]/
syntax match mCRL_operator /|_/
syntax match mCRL_operator /->/

highlight link mCRL_keyword Type
highlight link mCRL_algebra Statement
highlight link mCRL_comment Comment
highlight link mCRL_comment_attention Error
highlight link mCRL_identifier Identifier
highlight link mCRL_operator Operator
highlight link mCRL_special Special
