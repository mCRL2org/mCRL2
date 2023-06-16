#!/bin/sh

for file in *.tex; do
    pdflatex $file
    filename=$(basename -- "$file"); 
    base="${filename%.*}";
    inkscape.com --pdf-poppler --export-plain-svg --export-type="svg" "$base.pdf"
done