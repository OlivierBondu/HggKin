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
echo "\\title{Determination of ggh \$p_T\$ Distribution Through the Use of SPlot.}"
echo "\\begin{document}"
echo "\\maketitle"
echo "\\tableofcontents"
echo "\\clearpage"
echo "\\part{Generator Level}"
echo "\\section{Inclusive on $|cos(\\theta*)|$}"
  echo "\\begin{figure}[h!]"
  echo "\\centering"
  echo "\\begin{minipage}[c]{0.49\linewidth}"
  echo "\\includegraphics[width=8cm]{/afs/cern.ch/work/c/cgoudet/private/plot/pdf/splot_gen_unweighted_gghbkg.pdf}"
  echo "\\caption{Entrance Fit of mass and \$p_T\$ for Generated Backgound + Generated ggH events.}" 
  echo "\\end{minipage}"

  echo "\\hfill"
  
  echo "\\begin{minipage}[c]{0.49\linewidth}"
  echo "\\centering"
  echo "\\includegraphics[width=8cm]{/afs/cern.ch/work/c/cgoudet/private/plot/pdf/splot_gen_gghbkg.pdf}"
  echo "\\caption{Weighted Data from Splot and Entrance Corresponding Fit of \$p_T\$ for Generated Backgound + Generated ggH events.}" 
  echo "\\end{minipage}"
  echo "\\end{figure}"

  echo "\\begin{figure}[h!]"
  echo "\\centering"
  echo "\\includegraphics[width=12cm]{/afs/cern.ch/work/c/cgoudet/private/plot/pdf/splot_gen_ggh.pdf}"
  echo "\\caption{Extrapolated \$p_T\$ Distribution of \$gg\\rightarrow H\$ Signal.}" 
  echo "\\end{figure}"

echo

for cut in `echo "200 375 550 750"`
  do
  echo "\\clearpage"
  echo "\\section{\$cos(\\theta*)>0.${cut}\$}"
  echo "\\begin{figure}[h!]"
  echo "\\centering"
  echo "\\begin{minipage}[c]{0.49\linewidth}"
  echo "\\centering"
  echo "\\includegraphics[width=8cm]{/afs/cern.ch/work/c/cgoudet/private/plot/pdf/splot_gen_unweighted_gghbkg${cut}.pdf}"
  echo "\\caption{Entrance Fit of mass and \$p_T\$ for Generated Backgound + Generated ggH events with \$cos(\\theta*)>0.${cut}\$.}" 
  echo "\\end{minipage}"

  echo "\\hfill"
  
  echo "\\begin{minipage}[c]{0.49\linewidth}"
  echo "\\centering"
  echo "\\includegraphics[width=8cm]{/afs/cern.ch/work/c/cgoudet/private/plot/pdf/splot_gen_gghbkg${cut}.pdf}"
  echo "\\caption{Weighted Data from Splot and Entrance Corresponding Fit of \$p_T\$ for Generated Backgound + Generated ggH events with \$cos(\\theta*)>0.${cut}\$.}" 
  echo "\\end{minipage}"
  echo "\\end{figure}"

  echo "\\begin{figure}[h!]"
  echo "\\centering"
  echo "\\includegraphics[width=12cm]{/afs/cern.ch/work/c/cgoudet/private/plot/pdf/splot_gen_ggh${cut}.pdf}"
  echo "\\caption{Extrapolated \$p_T\$ Distribution of \$gg\\rightarrow H\$ Signal with the cut \$|cos(\\theta*)|>0.${cut}\$.}" 
  echo "\\end{figure}"

done

echo "\\end{document}"




exit 0
