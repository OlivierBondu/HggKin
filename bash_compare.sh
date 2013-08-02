#! /bin/bash

bash table_compare.sh > note_compare.tex
pdflatex note_compare.tex
pdflatex note_compare.tex
pdflatex note_compare.tex
rm -v note_compare.aux
rm -v note_compare.log
rm -v note_compare.tex
rm -v note_compare.toc

mv note_compare.pdf /afs/cern.ch/work/c/cgoudet/private/plot/.