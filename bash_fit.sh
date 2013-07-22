#!/bin/bash
data="/afs/cern.ch/work/c/cgoudet/private/data/"
bash table_fit.sh "${data}result_fit_ggh.txt" "${data}result_fit_vbf.txt" "${data}result_fit_bkg.txt" "${data}result_fit_bkg2.txt" "${data}result_fit_bkg3.txt" > note_fit.tex

pdflatex note_fit.tex

rm note_fit.log note_fit.aux note_fit.tex
mv note_fit.pdf /afs/cern.ch/work/c/cgoudet/private/plot/.
