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
  echo "\\includegraphics[width=8cm]{/afs/cern.ch/work/c/cgoudet/private/plot/pdf/SPlotInput_pt_genggh.pdf}"
  echo "\\caption{Mass discriminant ditribution for Generated Backgound + Generated ggH events.}" 
  echo "\\end{minipage}"

  echo "\\hfill"
  
  echo "\\begin{minipage}[c]{0.49\linewidth}"
  echo "\\centering"
  echo "\\includegraphics[width=8cm]{/afs/cern.ch/work/c/cgoudet/private/plot/pdf/splot_pt_gengghbkg.pdf}"
  echo "\\caption{Weighted Data from Splot and Entrance Corresponding Fit of \$p_T\$ for Generated Backgound + Generated ggH events.}" 
  echo "\\end{minipage}"
  echo "\\end{figure}"

  echo "\\begin{figure}[h!]"
  echo "\\centering"
  echo "\\includegraphics[width=12cm]{/afs/cern.ch/work/c/cgoudet/private/plot/pdf/splot_pt_genggh.pdf}"
  echo "\\caption{Extrapolated \$p_T\$ Distribution of \$gg\\rightarrow H\$ Signal.}" 
  echo "\\end{figure}"

echo

for cut in `echo "200 375 550 750"`
  do
  echo "\\clearpage"
  echo "\\subsection{\$cos(\\theta*)>0.${cut}\$}"
  echo "\\begin{figure}[h!]"
  echo "\\centering"
  echo "\\begin{minipage}[c]{0.49\linewidth}"
  echo "\\centering"
  echo "\\includegraphics[width=8cm]{/afs/cern.ch/work/c/cgoudet/private/plot/pdf/SPlotInput_pt${cut}_genggh.pdf}"
  echo "\\caption{Mass discriminant distribution for Generated Backgound + Generated ggH events with \$cos(\\theta*)>0.${cut}\$.}" 
  echo "\\end{minipage}"

  echo "\\hfill"
  
  echo "\\begin{minipage}[c]{0.49\linewidth}"
  echo "\\centering"
  echo "\\includegraphics[width=8cm]{/afs/cern.ch/work/c/cgoudet/private/plot/pdf/splot_pt${cut}_gengghbkg.pdf}"
  echo "\\caption{Weighted Data from Splot and Entrance Corresponding Fit of \$p_T\$ for Generated Backgound + Generated ggH events with \$cos(\\theta*)>0.${cut}\$.}" 
  echo "\\end{minipage}"
  echo "\\end{figure}"

  echo "\\begin{figure}[h!]"
  echo "\\centering"
  echo "\\includegraphics[width=12cm]{/afs/cern.ch/work/c/cgoudet/private/plot/pdf/splot_pt${cut}_genggh.pdf}"
  echo "\\caption{Extrapolated \$p_T\$ Distribution of \$gg\\rightarrow H\$ Signal with the cut \$|cos(\\theta*)|>0.${cut}\$.}" 
  echo "\\end{figure}"

done


#-------------------------------------------------------------------------------------&
#-------------------------------------------------------------------------------------&
#-------------------------------------------------------------------------------------&
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

  echo "\\subsection{Inclusive on $|cos(\\theta*)|$}"
  echo "\\begin{figure}[h!]"
  echo "\\centering"
  echo "\\begin{minipage}[c]{0.49\linewidth}"
  echo "\\includegraphics[width=8cm]{/afs/cern.ch/work/c/cgoudet/private/plot/pdf/SPlotInput_pt${buffcateg}_recoggh.pdf}"
  echo "\\caption{Mass discriminant distribution for Reconstructed Backgound + Reconstructed ggH events.}" 
  echo "\\end{minipage}"

  echo "\\hfill"
  
  echo "\\begin{minipage}[c]{0.49\linewidth}"
  echo "\\centering"
  echo "\\includegraphics[width=8cm]{/afs/cern.ch/work/c/cgoudet/private/plot/pdf/splot_pt${buffcateg}_recogghbkg.pdf}"
  echo "\\caption{Weighted Data from Splot and Entrance Corresponding Fit of \$p_T\$ for Reconstructed Backgound + Reconstructed ggH events.}" 
  echo "\\end{minipage}"
  echo "\\end{figure}"

  echo "\\begin{figure}[h!]"
  echo "\\centering"
  echo "\\includegraphics[width=12cm]{/afs/cern.ch/work/c/cgoudet/private/plot/pdf/splot_pt${buffcateg}_recoggh.pdf}"
  echo "\\caption{Extrapolated \$p_T\$ Distribution of \$gg\\rightarrow H\$ Signal.}" 
  echo "\\end{figure}"

  for cut in `echo "200 375 550 750"`
  do
    echo "\\clearpage"
  echo "\\subsection{\$cos(\\theta*)>0.${cut}\$}"
  echo "\\begin{figure}[h!]"
  echo "\\centering"
  echo "\\begin{minipage}[c]{0.49\linewidth}"
  echo "\\centering"
  echo "\\includegraphics[width=8cm]{/afs/cern.ch/work/c/cgoudet/private/plot/pdf/SPlotInput_pt${cut}${buffcateg}_recoggh.pdf}"
  echo "\\caption{Mass discriminant distribution for Reconstructed Backgound + Reconstructed ggH events with \$cos(\\theta*)>0.${cut}\$.}" 
  echo "\\end{minipage}"

  echo "\\hfill"
  
  echo "\\begin{minipage}[c]{0.49\linewidth}"
  echo "\\centering"
  echo "\\includegraphics[width=8cm]{/afs/cern.ch/work/c/cgoudet/private/plot/pdf/splot_pt${cut}${buffcateg}_recogghbkg.pdf}"
  echo "\\caption{Weighted Data from Splot and Entrance Corresponding Fit of \$p_T\$ for Reconstructed Backgound + Reconstructed ggH events with \$cos(\\theta*)>0.${cut}\$.}" 
  echo "\\end{minipage}"
  echo "\\end{figure}"

  echo "\\begin{figure}[h!]"
  echo "\\centering"
  echo "\\includegraphics[width=12cm]{/afs/cern.ch/work/c/cgoudet/private/plot/pdf/splot_pt${cut}${buffcateg}_recoggh.pdf}"
  echo "\\caption{Extrapolated \$p_T\$ Reconstructed Distribution of \$gg\\rightarrow H\$ Signal with the cut \$|cos(\\theta*)|>0.${cut}\$.}" 
  echo "\\end{figure}"

done


done #end categ
echo "\\end{document}"
exit 0