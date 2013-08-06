#!/bin/bash
hgg=`pwd`
data="/afs/cern.ch/work/c/cgoudet/private/data/"
tmp_files=""
cd ${data}
for process in `echo "ggh vbf bkg"`
  do
  tmp="result*${process}*gen.txt"
  tmp="`ls ${tmp}`"
  tmp_files="${tmp_files} ${tmp}"
done
echo ${tmp_files}
for categ in `seq 0 4`
  do 
  for process in `echo "ggh vbf bkg"`
    do
    tmp="result*${process}*reco.txt"
    if [[ ${categ} = "0" ]] 
	then tmp=`ls ${tmp} | grep -v "categ"`
    else tmp=`ls ${tmp} | grep "categ${categ}"`
    fi
    tmp_files="${tmp_files} ${tmp}"
  done
done

files=""
for fil in `echo ${tmp_files}`
  do 
  files="${files} ${data}${fil}"
done

cd ${hgg}
echo "starting writting tex"
bash table_fit.sh ${files} 1> note_fit.tex 2>error.txt
echo "tex written"
 pdflatex note_fit.tex
 pdflatex note_fit.tex

 rm note_fit.log note_fit.aux note_fit.toc 
 #rm note_fit.tex
 mv note_fit.pdf /afs/cern.ch/work/c/cgoudet/private/plot/.
