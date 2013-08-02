#!/bin/bash

# if [[ -z ${1} ]]
# then
# 	echo "warning: no input file"
# 	exit 1
# fi
plot="/afs/cern.ch/work/c/cgoudet/private/plot/pdf/"
#Begining of latex document
echo "\\documentclass[a4paper,11pt]{article}"
echo "\\usepackage[english]{babel}"
echo "\\usepackage[utf8]{inputenc}"
echo "\\usepackage[T1]{fontenc}"
echo "\\usepackage{graphicx}"
echo "\\usepackage[a4paper]{geometry}"
echo "\\geometry{hscale=0.9,vscale=0.85,centering}"
echo "\\title{Kinematical Distributions of BEH Signal from \$gg\rightarrow H\$ and VBF processes and Diphoton Background Normalized to \$19.62~fb^{-1}\$ for Generated and Reconstructed Levels.}"

echo "\\begin{document}"
echo "\\maketitle"
echo "\\tableofcontents"
echo "\\clearpage"
################################################################################################
################################################################################################
################################################################################################
echo "\\part{Generator Level}"
echo "\\section{Distribution on \$p_T\$.}"
for window in `echo "0 2 3"`
  do
  if [[ ${window} == "0" ]] 
      then 
      echo "\\subsection{Inclusive on  Mass.}"
  else
      echo "\\clearpage" 
      echo "\\subsection{\$${window}~GeV\$ Mass Window.}"
  fi

  echo "\\begin{figure}[!h]"
  echo "\\begin{minipage}[c]{0.49\linewidth}"
  echo "\\centering"
  echo "\\includegraphics[width=\linewidth]{${plot}compare_process_pt_gen_win${window}_norm0.pdf}"
  echo "\\caption{Distribution of \$p_T\$ for ggh, vbf and Diphoton Background for Generator.}"
  echo "\\end{minipage}"
  echo "\hfill"
  echo "\\begin{minipage}[c]{0.49\linewidth}"
  echo "\\centering"
  echo "\\includegraphics[width=\linewidth]{${plot}compare_process_pt_gen_win${window}_norm0_stack.pdf}"
  echo "\\caption{Sum of Distributions of \$p_T\$ of ggh, vbf and Diphoton Background for Generator.}"
  echo "\\end{minipage}"
  echo "\\vfill"
  echo "\\begin{minipage}[c]{0.49\linewidth}"
  echo "\\centering"
  echo "\\includegraphics[width=\linewidth]{${plot}compare_process_pt_gen_win${window}_norm0_logy.pdf}"
  echo "\\caption{Distribution of \$p_T\$ for ggh, vbf and Diphoton Background on Log Scale for Generator.}"
  echo "\\end{minipage}"
  echo "\\hfill"
  echo "\\begin{minipage}[c]{0.49\linewidth}"
  echo "\\centering"
  echo "\\includegraphics[width=\linewidth]{${plot}compare_process_pt_gen_win${window}_norm1_logy.pdf}"
  echo "\\caption{Normalized \$p_T\$ Distributions of ggh, vbf and Diphoton Background on Log Scale for Generator.}"
  echo "\\end{minipage}"
  echo "\\end{figure}"

  echo "\\clearpage"
  echo "\\begin{figure}[!h]"
  echo "\\centering"
  echo "\\begin{minipage}[c]{0.49\linewidth}"
  echo "\\centering"
  echo "\\includegraphics[width=\linewidth]{${plot}compare_process_costheta_gen_win${window}_norm0.pdf}"
  echo "\\caption{Distribution of \$|cos(\\theta*)|\$ for ggh, vbf and Diphoton Background for Generator.}"
  echo "\\end{minipage}"
  echo "\hfill"
  echo "\\begin{minipage}[c]{0.49\linewidth}"
  echo "\\centering"
  echo "\\includegraphics[width=\linewidth]{${plot}compare_process_costheta_gen_win${window}_norm0_stack.pdf}"
  echo "\\caption{Sum of Distributions of \$|cos(\\theta*)|\$ of ggh, vbf and Diphoton Background for Generator.}"
  echo "\\end{minipage}"
  echo "\\vfill"
  echo "\\begin{minipage}[c]{0.49\linewidth}"
  echo "\\centering"
  echo "\\includegraphics[width=\linewidth]{${plot}compare_process_costheta_gen_win${window}_norm0_logy.pdf}"
  echo "\\caption{Distribution of \$|cos(\\theta*)|\$ for ggh, vbf and Diphoton Background on Log Scale for Generator.}"
  echo "\\end{minipage}"
  echo "\\hfill"
  echo "\\begin{minipage}[c]{0.49\linewidth}"
  echo "\\centering"
  echo "\\includegraphics[width=\linewidth]{${plot}compare_process_costheta_gen_win${window}_norm1_logy.pdf}"
  echo "\\caption{Normalized \$|cos(\\theta*)|\$ Distributions of ggh, vbf and Diphoton Background on Log Scale for Generator.}"
  echo "\\end{minipage}"
  echo "\\end{figure}"

done #end window loop




# with cut on theta *
for cut in `echo "200 375 550 750"`
  do
  echo "\\clearpage"
  echo "\\section{\$p_T\$ Distributions with \$|cos(\\theta*)|>0.${cut}\$}"
  for window in `echo "0 2 3"`
    do
    if [[ ${window} == "0" ]] 
	then 
	echo "\\subsection{Inclusive on  Mass.}"
    else
	echo "\\clearpage" 
	echo "\\subsection{\$${window}~GeV\$ Mass Window.}"
    fi

    echo "\\begin{figure}[!h]"

    echo "\\begin{minipage}[c]{0.49\linewidth}"
    echo "\\centering"
    echo "\\includegraphics[width=\linewidth]{${plot}compare_process_ptcuttheta${cut}_gen_win${window}_norm0.pdf}"
    echo "\\caption{Distribution of \$p_T\$ for ggh, vbf and Diphoton Background for Generator with \$|cos(\\theta*)|>0.${cut}\$.}"
    echo "\\end{minipage}"
    echo "\hfill"
    echo "\\begin{minipage}[c]{0.49\linewidth}"
    echo "\\centering"
    echo "\\includegraphics[width=\linewidth]{${plot}compare_process_ptcuttheta${cut}_gen_win${window}_norm0_stack.pdf}"
    echo "\\caption{Sum of Distributions of \$p_T\$ of ggh, vbf and Diphoton Background for Generator with \$|cos(\\theta*)|>0.${cut}\$.}"
    echo "\\end{minipage}"
    echo "\\vfill"
    echo "\\begin{minipage}[c]{0.49\linewidth}"
    echo "\\centering"
    echo "\\includegraphics[width=\linewidth]{${plot}compare_process_ptcuttheta${cut}_gen_win${window}_norm0_logy.pdf}"
    echo "\\caption{Distribution of \$p_T\$ for ggh, vbf and Diphoton Background on Log Scale for Generator with \$|cos(\\theta*)|>0.${cut}\$.}"
    echo "\\end{minipage}"
    echo "\\hfill"
    echo "\\begin{minipage}[c]{0.49\linewidth}"
    echo "\\centering"
    echo "\\includegraphics[width=\linewidth]{${plot}compare_process_ptcuttheta${cut}_gen_win${window}_norm1_logy.pdf}"
    echo "\\caption{Normalized \$p_T\$ Distributions of ggh, vbf and Diphoton Background on Log Scale for Generator with \$|cos(\\theta*)|>0.${cut}\$.}"
    echo "\\end{minipage}"
    echo "\\end{figure}"
    

  done #end window loop
done #end cut loop

###########################################################################################################
#######################################  RECO ####################################################################
###########################################################################################################
echo "\\clearpage"
echo "\\part{Reconstructed Level}"
echo "\\section{Distribution on Mass}"

for categ in {-1..3}
  do
  if [[ ${categ} = "-1" ]];
      then
      buffcateg=""
      echo "\\subsection{No Category.}"
  else
      echo "\\clearpage"
      buffcateg="_categ${categ}"
      echo "\\subsection{Category ${categ}.}"
  fi 
  
  echo "\\begin{figure}[!h]"
  echo "\\centering"
  echo "\\includegraphics[width=0.6\\linewidth]{/afs/cern.ch/work/c/cgoudet/private/plot/pdf/compare_process_mass${buffcateg}_reco_win0_norm0_stack.pdf}"
  echo "\\caption{Expected Mass Distribution.}"
  echo "\\end{figure}"
  echo "\\vfill"
  echo "\\begin{figure}[!h]"
  echo "\\centering"
  echo "\\includegraphics[width=0.6\\linewidth]{/afs/cern.ch/work/c/cgoudet/private/plot/pdf/compare_process_mass${buffcateg}_reco_win0_norm1.pdf}"
  echo "\\caption{Expected Normalized Mass Distribution.}"
  echo "\\end{figure}"

done #endloopcateg
categ=""
echo "\\clearpage"
echo "\\section{Distribution on \$p_T\$.}"
echo ${categ}

for categ in {-1..3}
  do
  if [[ ${categ} = "-1" ]];
      then 
      buffcateg=""
      echo "\\subsection{No Category.}"
  else  
      echo "\\clearpage"
      buffcateg="_categ${categ}"
      echo "\\subsection{Category ${categ}}"
  fi
  
  for window in `echo "0 2 3"`
    do
    if [[ ${window} = "0" ]];
	then
	echo "\\subsubsection{Inclusive on  Mass.}"
    else
	echo "\\clearpage" 
	echo "\\subsubsection{\$${window}~GeV\$ Mass Window.}"
    fi
    
    
    echo "\\begin{figure}[!h]"
    echo "\\begin{minipage}[c]{0.49\linewidth}"
    echo "\\centering"
    echo "\\includegraphics[width=\linewidth]{${plot}compare_process_pt${buffcateg}_reco_win${window}_norm0.pdf}"
    echo "\\caption{Distribution of \$p_T\$ for ggh, vbf and Diphoton Background for Reconstructed Category ${categ}.}"
    echo "\\end{minipage}"
    echo "\hfill"
    echo "\\begin{minipage}[c]{0.49\linewidth}"
    echo "\\centering"
    echo "\\includegraphics[width=\linewidth]{${plot}compare_process_pt${buffcateg}_reco_win${window}_norm0_stack.pdf}"
    echo "\\caption{Sum of Distributions of \$p_T\$ of ggh, vbf and Diphoton Background for Reconstructed Category ${categ}.}"
    echo "\\end{minipage}"
    echo "\\vfill"
    echo "\\begin{minipage}[c]{0.49\linewidth}"
    echo "\\centering"
    echo "\\includegraphics[width=\linewidth]{${plot}compare_process_pt${buffcateg}_reco_win${window}_norm0_logy.pdf}"
    echo "\\caption{Distribution of \$p_T\$ for ggh, vbf and Diphoton Background on Log Scale  for Reconstructed Category ${categ}.}"
    echo "\\end{minipage}"
    echo "\\hfill"
    echo "\\begin{minipage}[c]{0.49\linewidth}"
    echo "\\centering"
    echo "\\includegraphics[width=\linewidth]{${plot}compare_process_pt${buffcateg}_reco_win${window}_norm1_logy.pdf}"
    echo "\\caption{Normalized \$p_T\$ Distributions of ggh, vbf and Diphoton Background on Log Scale  for Reconstructed Category ${categ}.}"
    echo "\\end{minipage}"
    echo "\\end{figure}"

    echo "\\clearpage"
    echo "\\begin{figure}[!h]"
    echo "\\centering"
    echo "\\begin{minipage}[c]{0.49\linewidth}"
    echo "\\centering"
    echo "\\includegraphics[width=\linewidth]{${plot}compare_process_costheta${buffcateg}_reco_win${window}_norm0.pdf}"
    echo "\\caption{Distribution of \$|cos(\\theta*)|\$ for ggh, vbf and Diphoton Background for Reconstructed Category ${categ}.}"
    echo "\\end{minipage}"
    echo "\hfill"
    echo "\\begin{minipage}[c]{0.49\linewidth}"
    echo "\\centering"
    echo "\\includegraphics[width=\linewidth]{${plot}compare_process_costheta${buffcateg}_reco_win${window}_norm0_stack.pdf}"
    echo "\\caption{Sum of Distributions of \$|cos(\\theta*)|\$ of ggh, vbf and Diphoton Background for Reconstructed Category ${categ}.}"
    echo "\\end{minipage}"
    echo "\\vfill"
    echo "\\begin{minipage}[c]{0.49\linewidth}"
    echo "\\centering"
    echo "\\includegraphics[width=\linewidth]{${plot}compare_process_costheta${buffcateg}_reco_win${window}_norm0_logy.pdf}"
    echo "\\caption{Distribution of \$|cos(\\theta*)|\$ for ggh, vbf and Diphoton Background on Log Scale for Reconstructed Category ${categ}.}"
    echo "\\end{minipage}"
    echo "\\hfill"
    echo "\\begin{minipage}[c]{0.49\linewidth}"
    echo "\\centering"
    echo "\\includegraphics[width=\linewidth]{${plot}compare_process_costheta${buffcateg}_reco_win${window}_norm1_logy.pdf}"
    echo "\\caption{Normalized \$|cos(\\theta*)|\$ Distributions of ggh, vbf and Diphoton Background on Log Scale for Reconstructed Category ${categ}.}"
    echo "\\end{minipage}"
    echo "\\end{figure}"

  done #end window loop
done #categ

window="0"
categ=""

# with cut on theta *
for cut in `echo " 200 375 550 750"`
  do
  echo "\\clearpage"
  echo "\\section{\$p_T\$ Distributions with \$|cos(\\theta*)|>0.${cut}\$}"
  for categ in `seq 0 4`
    do

    if [[ ${categ} == "4" ]]
	then 
	buffcateg=""
	echo "\\subsection{No  Categories.}"
    else buffcateg="_categ${categ}"
	echo "\\clearpage"	
	echo "\\subsection{Categorie ${categ}}"
    fi
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
      
      echo "\\begin{minipage}[c]{0.49\linewidth}"
      echo "\\centering"
      echo "\\includegraphics[width=\linewidth]{${plot}compare_process_ptcuttheta${cut}${buffcateg}_reco_win${window}_norm0.pdf}"
      echo "\\caption{Distribution of \$p_T\$ for ggh, vbf and Diphoton Background for Reconstructed Category ${categ} with \$|cos(\\theta*)|>0.${cut}\$.}"
      echo "\\end{minipage}"
      echo "\hfill"
      echo "\\begin{minipage}[c]{0.49\linewidth}"
      echo "\\centering"
      echo "\\includegraphics[width=\linewidth]{${plot}compare_process_ptcuttheta${cut}${buffcateg}_reco_win${window}_norm0_stack.pdf}"
      echo "\\caption{Sum of Distributions of \$p_T\$ of ggh, vbf and Diphoton Background for Reconstructed Category ${categ} with \$|cos(\\theta*)|>0.${cut}\$.}"
      echo "\\end{minipage}"
      echo "\\vfill"
      echo "\\begin{minipage}[c]{0.49\linewidth}"
      echo "\\centering"
      echo "\\includegraphics[width=\linewidth]{${plot}compare_process_ptcuttheta${cut}${buffcateg}_reco_win${window}_norm0_logy.pdf}"
      echo "\\caption{Distribution of \$p_T\$ for ggh, vbf and Diphoton Background on Log Scale for Reconstructed Category ${categ} with \$|cos(\\theta*)|>0.${cut}\$.}"
      echo "\\end{minipage}"
      echo "\\hfill"
      echo "\\begin{minipage}[c]{0.49\linewidth}"
      echo "\\centering"
      echo "\\includegraphics[width=\linewidth]{${plot}compare_process_ptcuttheta${cut}${buffcateg}_reco_win${window}_norm1_logy.pdf}"
      echo "\\caption{Normalized \$p_T\$ Distributions of ggh, vbf and Diphoton Background on Log Scale for Reconstructed Category ${categ} with \$|cos(\\theta*)|>0.${cut}\$.}"
      echo "\\end{minipage}"
      echo "\\end{figure}"
      
      
    done #end window loop
  done #end categ
done #end cut loop



echo "\\end{document}"




exit 0
