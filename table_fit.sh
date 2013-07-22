#!/bin/bash

if [[ -z ${5} ]]
then
	echo "warning: not enough input file"
	exit 1
fi
file_ggh=${1}
file_vbf=${2}
file_bkg=${3}
file_bkg2=${4}
file_bkg3=${5}


echo "\\documentclass[a4paper,11pt]{article}"
echo "\\usepackage[english]{babel}"
echo "\\usepackage[utf8]{inputenc}"
echo "\\usepackage[T1]{fontenc}"
echo "\\usepackage{graphicx}"
echo "\\usepackage[a4paper]{geometry}"
echo "\\geometry{hscale=0.9,vscale=0.85,centering}"
echo "\\begin{document}"
echo "\\begin{center}"

echo "\\section*{\$\\chi^2$ of different fitting fonctions of the \$p_t\$ for generated background, ggh and vbf  diphoton systems with cuts over \$cos(\\theta*)\$.}" 

echo ""
echo "Fitting distributions are product of main function (land, logn, tsallis) with a polynome (pol0,1,2).\\\\
land = landau distribution \\\\
logn = logarithm normal distribution\\\\
tsallis = \$\\frac{(n-1)(n-2)}{nT(nT+(n-2)m)}*p_t*(1+\\frac{\\sqrt{m^2+p_t^2}-m}{nT})^n\$\\\\
poli = polynom with a degree i\\\\"



#Create functions names for head of tabular
func=`echo "process"`
for function in `echo "land logn tsallis"`
  do
  for pol in `seq 0 2`
    do 
    func="${func} & ${function}pol${pol}"
  done
done 

#Fill no_cut tabular
echo "\\begin{figure}[!h]"
echo "\\caption{No cut on \$cos(\\theta*)\$}"
echo "\\begin{tabular}{|c|c|c|c|c|c|c|c|c|c|}"
echo "\\hline"
echo "${func} \\\\"
echo "\\hline"
for function in `echo "land logn tsallis"`
  do
  for pol in `seq 0 2`
    do
    name=`echo "ggh${function}pol${pol}"`
    chi2=`grep ${name} ${file_ggh} | grep -v "cuttheta*"   | awk '{print $2}'`
#      echo "${name} ${chi2}"
    chi2list_ggh="${chi2list_ggh} &  ${chi2}"
    
    name=`echo "vbf${function}pol${pol}"`
    chi2=`grep ${name} ${file_vbf} | grep -v "cuttheta*"   | awk '{print $2}'`
    chi2list_vbf="${chi2list_vbf} &  ${chi2}"

    name=`echo "bkg${function}pol${pol}"`
    chi2=`grep ${name} ${file_bkg} | grep -v "cuttheta*"   | awk '{print $2}'`
    chi2list_bkg="${chi2list_bkg} &  ${chi2}"

    name=`echo "bkg2${function}pol${pol}"`
    chi2=`grep ${name} ${file_bkg2} | grep -v "cuttheta*"   | awk '{print $2}'`
    chi2list_bkg2="${chi2list_bkg2} &  ${chi2}"

    name=`echo "bkg3${function}pol${pol}"`
    chi2=`grep ${name} ${file_bkg3} | grep -v "cuttheta*"   | awk '{print $2}'`
    chi2list_bkg3="${chi2list_bkg3} &  ${chi2}"

  done # end loop pol
done # end loop function
echo "ggh ${chi2list_ggh} \\\\"
echo "vbf ${chi2list_vbf} \\\\"
echo "bkg ${chi2list_bkg} \\\\"
echo "bkg2 ${chi2list_bkg2} \\\\"
echo "bkg3 ${chi2list_bkg3} \\\\"
chi2list_ggh=""
chi2list_vbf=""
chi2list_bkg=""
chi2list_bkg2=""
chi2list_bkg3=""

echo "\\hline"
echo "\\end{tabular}"
echo "\\newline"
echo "\\\\ "
echo "\\begin{minipage}{0.33\\linewidth}"
echo "\\includegraphics[width=6cm]{/afs/cern.ch/work/c/cgoudet/private/plot/pdf/fit_gghtsallispol2.pdf}"
echo "\\end{minipage}"
echo "\\begin{minipage}{0.33\\linewidth}"
echo "\\includegraphics[width=6cm]{/afs/cern.ch/work/c/cgoudet/private/plot/pdf/fit_vbftsallispol1.pdf}"
echo "\\end{minipage}"
echo "\\begin{minipage}{0.33\\linewidth}"
echo "\\includegraphics[width=6cm]{/afs/cern.ch/work/c/cgoudet/private/plot/pdf/fit_bkgtsallispol2.pdf}"
echo "\\end{minipage}"

echo "\\end{figure}"



echo "${bestname}  ${bestchi2}"



# liste tous les gghlandpol0 | garde la colonne 1 | coupe la 2e partie
cutlist=`grep gghlandpol0 ${file_ggh} | grep cuttheta | awk '{print $1}' | cut -d _ -f 2 | cut -d a -f 2` 

#Fill tabular for cuts
for cut in `echo ${cutlist}`
  do
  if [[ "${cut}" = "200" ]];
      then echo "\\clearpage"
elif [[ "${cut}" = "550" ]];
then echo "\\clearpage"
  fi
  echo "\\begin{figure}[!h]"
  echo "\\caption{Cut \$cos(\\theta*)>0.${cut}\$}"
  echo "\\begin{tabular}{|c|c|c|c|c|c|c|c|c|c|}"
  echo "\\hline"
  echo "${func} \\\\"
  echo "\\hline"
    for function in `echo "land logn tsallis"`
      do
      for pol in `seq 0 2`
	do
	name=`echo "ggh${function}pol${pol}_cuttheta${cut}"`
	chi2=`grep ${name} ${file_ggh} | awk '{print $2}'`
	chi2list_ggh="${chi2list_ggh} &  ${chi2}"

	name=`echo "vbf${function}pol${pol}_cuttheta${cut}"`
	chi2=`grep ${name} ${file_vbf} | awk '{print $2}'`
	chi2list_vbf="${chi2list_vbf} &  ${chi2}"

	name=`echo "bkg${function}pol${pol}_cuttheta${cut}"`
	chi2=`grep ${name} ${file_bkg} | awk '{print $2}'`
	chi2list_bkg="${chi2list_bkg} &  ${chi2}"

	name=`echo "bkg2${function}pol${pol}_cuttheta${cut}"`
	chi2=`grep ${name} ${file_bkg2} | awk '{print $2}'`
	chi2list_bkg2="${chi2list_bkg2} &  ${chi2}"

	name=`echo "bkg3${function}pol${pol}_cuttheta${cut}"`
	chi2=`grep ${name} ${file_bkg3} | awk '{print $2}'`
	chi2list_bkg3="${chi2list_bkg3} &  ${chi2}"


      done # end loop pol
    done # end loop function
    echo "ggh  ${chi2list_ggh} \\\\"
    echo "vbf  ${chi2list_vbf} \\\\"
    echo "bkg  ${chi2list_bkg} \\\\"
    echo "bkg2  ${chi2list_bkg2} \\\\"
    echo "bkg3  ${chi2list_bkg3} \\\\"
chi2list_ggh=""
chi2list_vbf=""
chi2list_bkg=""
chi2list_bkg2=""
chi2list_bkg3=""


    echo "\\hline"
    echo "\\end{tabular}"
    echo "\\begin{minipage}{0.33\\linewidth}"
    echo "\\includegraphics[width=6cm]{/afs/cern.ch/work/c/cgoudet/private/plot/pdf/fitcuttheta${cut}_gghtsallispol2.pdf}"
    echo "\\end{minipage}"
    echo "\\begin{minipage}{0.33\\linewidth}"
    echo "\\includegraphics[width=6cm]{/afs/cern.ch/work/c/cgoudet/private/plot/pdf/fitcuttheta${cut}_vbftsallispol1.pdf}"
    echo "\\end{minipage}"
    echo "\\begin{minipage}{0.33\\linewidth}"
    echo "\\includegraphics[width=6cm]{/afs/cern.ch/work/c/cgoudet/private/plot/pdf/fitcuttheta${cut}_bkgtsallispol2.pdf}"
    echo "\\end{minipage}"

echo "\\end{figure}"
done # end loop cut


echo "\\end{center}"
echo "\\end{document}"



#for process in `echo "ggh vbf bkg"`
#do
#	for function in `echo "land logn tallis"`
#	do
#		for pol in `seq 0 2`
#		do
#			for cut in `echo ${cutlist}`
#			do
#				name=`echo "${process}${function}pol${pol}_${cut}"`
#				chi2=`grep ${name} ${file} |awk '{print $2}'`
#				echo ${name} ${chi2}
#				break
#			done
#		done
#	done
#	break
#done




exit 0
