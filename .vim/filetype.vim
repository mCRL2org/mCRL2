" local filetype file
if exists("did_load_filetypes")
  finish
endif


augroup filetypedetect
  au! BufRead,BufNewFile *.bnf          setfiletype ebnf
  au! BufRead,BufNewFile *.ebnf         setfiletype ebnf
  au! BufRead,BufNewFile *.asf          setfiletype asfsdf
  au! BufRead,BufNewFile *.sdf          setfiletype asfsdf
  au! BufRead,BufNewFile *.mcrl         setfiletype mcrl
  au! BufRead,BufNewFile *.gs           setfiletype genspect
augroup END
