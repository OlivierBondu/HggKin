#! /bin/bash

bash table_splot.sh > note_splot.tex

pdflatex note_splot.tex
pdflatex note_splot.tex
pdflatex note_splot.tex

rm -v note_splot.aux
rm -v note_splot.log
#rm -v note_splot.tex

mv note_splot.pdf /afs/cern.ch/work/c/cgoudet/private/plot/.