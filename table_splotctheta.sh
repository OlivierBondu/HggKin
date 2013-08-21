#!/bin/bash

# if [[ -z ${1} ]]
# then
# 	echo "warning: no input file"
# 	exit 1
# fi

echo "\\documentclass[a4paper,11pt]{article}"
echo "\\usepackage[english]{babel}"
echo "\\usepackage[utf8]{inputenc}"
echo "\\usepackage[T1]{fontenc}"
echo "\\usepackage{graphicx}"
echo "\\usepackage[a4paper]{geometry}"
echo "\\geometry{hscale=0.9,vscale=0.85,centering}"
echo "\\title{Determination of ggh \$|cos(\\theta*)|\$ Distribution Through the Use of SPlot.}"
echo "\\begin{document}"
echo "\\maketitle"
echo "\\tableofcontents"
echo "\\clearpage"


echo "\\part{Reconstructed Level}"

for categ in `seq -1 3`
  do
  if [[ ${categ} = "-1" ]];
      then
      buffcateg=""
      echo "\\section{No Category}"
  else 
      echo "\\clearpage"
      echo "\\section{Category ${categ}}"
      buffcateg="_categ${categ}"
  fi

  echo "\\begin{figure}[h!]"
  echo "\\centering"
  echo "\\begin{minipage}[c]{0.49\linewidth}"
  echo "\\includegraphics[width=8cm]{/afs/cern.ch/work/c/cgoudet/private/plot/pdf/SPlotInput_ctheta${buffcateg}_recoggh.pdf}"
  echo "\\caption{Mass discriminant distribution for Reconstructed Backgound + Reconstructed ggH events.}" 
  echo "\\end{minipage}"

  echo "\\hfill"
  
  echo "\\begin{minipage}[c]{0.49\linewidth}"
  echo "\\centering"
  echo "\\includegraphics[width=8cm]{/afs/cern.ch/work/c/cgoudet/private/plot/pdf/splot_ctheta${buffcateg}_recogghbkg.pdf}"
  echo "\\caption{Weighted Data from Splot and Simulation of \$|cos(\\theta*)|\$ for Reconstructed Backgound + Reconstructed ggH events.}" 
  echo "\\end{minipage}"
  echo "\\end{figure}"

  echo "\\begin{figure}[h!]"
  echo "\\centering"
  echo "\\includegraphics[width=12cm]{/afs/cern.ch/work/c/cgoudet/private/plot/pdf/splot_ctheta${buffcateg}_recoggh.pdf}"
  echo "\\caption{Extracted \$|cos(\\theta*)|\$ Distribution of \$gg\\rightarrow H\$ Signal.}" 
  echo "\\end{figure}"



done #end categ
echo "\\end{document}"
exit 0