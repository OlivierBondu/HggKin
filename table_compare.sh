#!/bin/bash

# if [[ -z ${1} ]]
# then
# 	echo "warning: no input file"
# 	exit 1
# fi
plot="/afs/cern.ch/work/c/cgoudet/private/plot/pdf/"

echo "\\documentclass[a4paper,11pt]{article}"
echo "\\usepackage[english]{babel}"
echo "\\usepackage[utf8]{inputenc}"
echo "\\usepackage[T1]{fontenc}"
echo "\\usepackage{graphicx}"
echo "\\usepackage[a4paper]{geometry}"
echo "\\geometry{hscale=0.9,vscale=0.85,centering}"
echo "\\title{Kinematical Distributions of BEH Signal from \$gg\rightarrow H\$ and VBF processes and Diphoton Background Normalized to \$19.62~fb^{-1}\$ for Generated and Reconstructed Levels.}"

echo "\\begin{document}"
echo "\\setcounter{tocdepth}{2}"
echo "\\maketitle"
echo "\\tableofcontents"


for level in `echo "gen"`
do
echo "\\clearpage"
if [[ ${level} == "gen" ]]
then echo "\\section{Generator Level}"
fi
# without cut on theta *
echo "\\subsection{Inclusive on \$|cos(\\theta*)|\$.}"
for window in `echo "0 2 3"`
  do
  if [[ ${window} == "0" ]] 
      then 
echo "\\addtocontents{toc}{\\protect\\setcounter{tocdepth}{3}}"
      echo "\\subsubsection{Inclusive on  Mass.}"
echo "\\addtocontents{toc}{\\protect\\setcounter{tocdepth}{2}}"
  else
      echo "\\clearpage" 
echo "\\addtocontents{toc}{\\protect\\setcounter{tocdepth}{3}}"
      echo "\\subsubsection{\$${window}~GeV\$ Mass Window.}"
echo "\\addtocontents{toc}{\\protect\\setcounter{tocdepth}{2}}"
  fi

echo "\\begin{figure}[!h]"
echo "\\centering"
echo "\\begin{minipage}[c]{0.49\linewidth}"
echo "\\includegraphics[width=8cm]{${plot}compare_process_pt_${level}_win${window}_norm0.pdf}"
echo "\\caption{Distribution of \$p_T\$ for ggh, vbf and Diphoton Background.}"
echo "\\end{minipage}"
echo "\hfill"
echo "\\begin{minipage}[c]{0.49\linewidth}"
echo "\\includegraphics[width=8cm]{${plot}compare_process_pt_${level}_win${window}_norm0_stack.pdf}"
echo "\\caption{Sum of Distributions of \$p_T\$ of ggh, vbf and Diphoton Background.}"
echo "\\end{minipage}"
echo "\\vfill"
echo "\\begin{minipage}[c]{0.49\linewidth}"
echo "\\includegraphics[width=8cm]{${plot}compare_process_pt_${level}_win${window}_norm0_logy.pdf}"
echo "\\caption{Distribution of \$p_T\$ for ggh, vbf and Diphoton Background on Log Scale.}"
echo "\\end{minipage}"
echo "\\hfill"
echo "\\begin{minipage}[c]{0.49\linewidth}"
echo "\\includegraphics[width=8cm]{${plot}compare_process_pt_${level}_win${window}_norm1_logy.pdf}"
echo "\\caption{Normalized \$p_T\$ Distributions of ggh, vbf and Diphoton Background on Log Scale.}"
echo "\\end{minipage}"
echo "\\end{figure}"

echo "\\clearpage"
echo "\\begin{figure}[!h]"
echo "\\centering"
echo "\\begin{minipage}[c]{0.49\linewidth}"
echo "\\includegraphics[width=8cm]{${plot}compare_process_costheta_${level}_win${window}_norm0.pdf}"
echo "\\caption{Distribution of \$|cos(\\theta*)|\$ for ggh, vbf and Diphoton Background.}"
echo "\\end{minipage}"
echo "\hfill"
echo "\\begin{minipage}[c]{0.49\linewidth}"
echo "\\includegraphics[width=8cm]{${plot}compare_process_costheta_${level}_win${window}_norm0_stack.pdf}"
echo "\\caption{Sum of Distributions of \$|cos(\\theta*)|\$ of ggh, vbf and Diphoton Background.}"
echo "\\end{minipage}"
echo "\\vfill"
echo "\\begin{minipage}[c]{0.49\linewidth}"
echo "\\includegraphics[width=8cm]{${plot}compare_process_costheta_${level}_win${window}_norm0_logy.pdf}"
echo "\\caption{Distribution of \$|cos(\\theta*)|\$ for ggh, vbf and Diphoton Background on Log Scale.}"
echo "\\end{minipage}"
echo "\\hfill"
echo "\\begin{minipage}[c]{0.49\linewidth}"
echo "\\includegraphics[width=8cm]{${plot}compare_process_costheta_${level}_win${window}_norm1_logy.pdf}"
echo "\\caption{Normalized \$|cos(\\theta*)|\$ Distributions of ggh, vbf and Diphoton Background on Log Scale.}"
echo "\\end{minipage}"
echo "\\end{figure}"

done #end window loop


window="0"

# with cut on theta *
for cut in `echo "200 375 550 750"`
  do
  echo "\\clearpage"
  echo "\\subsection{\$p_T\$ Distributions with \$|cos(\\theta*)|>0.${cut}\$}"
  for window in `echo "0 2 3"`
    do
  if [[ ${window} == "0" ]] 
      then 
      echo "\\subsubsection{Inclusive on  Mass.}"
  else
      echo "\\clearpage" 
      echo "\\subsubsection{\$${window}~GeV\$ Mass Window.}"
  fi

    echo "\\begin{figure}[!h]"
    echo "\\centering"
    echo "\\begin{minipage}[c]{0.49\linewidth}"
    echo "\\includegraphics[width=8cm]{${plot}compare_process_ptcuttheta${cut}_${level}_win${window}_norm0.pdf}"
    echo "\\caption{Distribution of \$p_T\$ for ggh, vbf and Diphoton Background.}"
    echo "\\end{minipage}"
    echo "\hfill"
    echo "\\begin{minipage}[c]{0.49\linewidth}"
    echo "\\includegraphics[width=8cm]{${plot}compare_process_ptcuttheta${cut}_${level}_win${window}_norm0_stack.pdf}"
    echo "\\caption{Sum of Distributions of \$p_T\$ of ggh, vbf and Diphoton Background.}"
    echo "\\end{minipage}"
    echo "\\vfill"
    echo "\\begin{minipage}[c]{0.49\linewidth}"
    echo "\\includegraphics[width=8cm]{${plot}compare_process_ptcuttheta${cut}_${level}_win${window}_norm0_logy.pdf}"
    echo "\\caption{Distribution of \$p_T\$ for ggh, vbf and Diphoton Background on Log Scale.}"
    echo "\\end{minipage}"
    echo "\\hfill"
    echo "\\begin{minipage}[c]{0.49\linewidth}"
    echo "\\includegraphics[width=8cm]{${plot}compare_process_ptcuttheta${cut}_${level}_win${window}_norm1_logy.pdf}"
    echo "\\caption{Normalized \$p_T\$ Distributions of ggh, vbf and Diphoton Background on Log Scale.}"
    echo "\\end{minipage}"
    echo "\\end{figure}"
    

done #end window loop
done #end cut loop

done #end level
echo "\\end{document}"




exit 0
