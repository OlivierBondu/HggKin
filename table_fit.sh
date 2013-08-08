#!/bin/bash

if [[ -z ${5} ]]
then
	echo "warning: not enough input file"
	exit 1
fi
file_ggh=${1}
file_vbf=${2}
file_bkg=${5}
file_bkg2=${3}
file_bkg3=${4}


echo "\\documentclass[a4paper,11pt]{article}"
echo "\\usepackage[english]{babel}"
echo "\\usepackage[utf8]{inputenc}"
echo "\\usepackage[T1]{fontenc}"
echo "\\usepackage{graphicx}"
echo "\\usepackage[a4paper]{geometry}"
echo "\\geometry{hscale=0.9,vscale=0.85,centering}"
echo "\\title{\$\\chi^2\$ of different fitting fonctions of the \$p_t\$ for expected  background, ggh and vbf  diphoton systems at generator and reconstructed level.}" 
echo "\\begin{document}"
echo "\\maketitle"
echo "Fitting distributions are product of main function (land, logn, tsallis) with a polynome (pol0,1,2).\\\\
land = landau distribution \\\\
logn = logarithm normal distribution\\\\
tsallis = \$\\frac{(n-1)(n-2)}{nT(nT+(n-2)m)}*p_t*(1+\\frac{\\sqrt{m^2+p_t^2}-m}{nT})^n\$\\\\
poli = polynom with a degree i\\\\"
echo "\\tableofcontents"
echo "\\clearpage"

############################################################################
#############################  GEN  ###############################################
############################################################################
#Create functions names for head of tabular
func=`echo "process"`
for function in `echo "land logn tsallis"`
  do
  for pol in `seq 0 2`
    do 
    func="${func} & ${function}pol${pol}"
  done
done 

# liste tous les gghlandpol0 | garde la colonne 1 | coupe la 2e partie
cutlist=`grep gghlandpol0 ${file_ggh} | grep cuttheta | awk '{print $1}' | cut -d _ -f 2 | cut -d a -f 2` 

#Fill tabular for cuts
echo "\\part{Generator Level}"
for cut in `echo "0 ${cutlist}"`
  do
  if [[ ("${cut}" = "375")  ||  ("${cut}" = "750") ]];
      then echo "\\clearpage"
  fi
  if [[ "${cut}" = "0" ]];
      then echo "\\section{Inclusive on \$|cos(\\theta*)|\$}"
  else echo "\\section{\$|cos(\\theta*)|>0.${cut}\$}"
  fi
  echo "\\begin{figure}[!h]"
  
  if [[ "${cut}" = "0" ]];
      then   echo "\\caption{\$\\chi^2\$ values for ggh, vbg and background events inclusive on \$|cos(\\theta*)|\$}"
      cuttheta=""
      grepvar=" | grep -v \"cuttheta\" " 
  else   echo "\\caption{\$\\chi^2\$ values for ggh, vbg and background events with \$cos(\\theta*)>0.${cut}\$}"
      cuttheta="_cuttheta${cut}"
      grepvar=""

  fi

  echo "\\begin{tabular}{|c|c|c|c|c|c|c|c|c|c|}"
  echo "\\hline"
  echo "${func} \\\\"
  echo "\\hline"
  for function in `echo "land logn tsallis"`
    do
    for pol in `seq 0 2`
      do
      if [[ "${cut}" = "0" ]];      
	  then name=`echo "ggh${function}pol${pol}${cuttheta}"`
	  chi2=`grep ${name} ${file_ggh} | grep -v "cuttheta" | awk '{print $2}'`
	  chi2list_ggh="${chi2list_ggh} &  ${chi2}"
	  name=`echo "vbf${function}pol${pol}${cuttheta}"`
	  chi2=`grep ${name} ${file_vbf} | grep -v "cuttheta" | awk '{print $2}'`
	  chi2list_vbf="${chi2list_vbf} &  ${chi2}"
	  
	  name=`echo "bkg${function}pol${pol}${cuttheta}"`
	  chi2=`grep ${name} ${file_bkg} | grep -v "cuttheta" | awk '{print $2}'`
	  chi2list_bkg="${chi2list_bkg} &  ${chi2}"
	  
	  name=`echo "bkg2${function}pol${pol}${cuttheta}"`
	  chi2=`grep ${name} ${file_bkg2} | grep -v "cuttheta" | awk '{print $2}'`
	  chi2list_bkg2="${chi2list_bkg2} &  ${chi2}"
	  
	  name=`echo "bkg3${function}pol${pol}${cuttheta}"`
	  chi2=`grep ${name} ${file_bkg3} | grep -v "cuttheta" | awk '{print $2}'`
	  chi2list_bkg3="${chi2list_bkg3} &  ${chi2}"


      else
	  name=`echo "ggh${function}pol${pol}${cuttheta}"`
	  chi2=`grep ${name} ${file_ggh} | awk '{print $2}'`
	  chi2list_ggh="${chi2list_ggh} &  ${chi2}"
	  
	  name=`echo "vbf${function}pol${pol}${cuttheta}"`
	  chi2=`grep ${name} ${file_vbf} | awk '{print $2}'`
	  chi2list_vbf="${chi2list_vbf} &  ${chi2}"
	  
	  name=`echo "bkg${function}pol${pol}${cuttheta}"`
	  chi2=`grep ${name} ${file_bkg} | awk '{print $2}'`
	  chi2list_bkg="${chi2list_bkg} &  ${chi2}"
	  
	  name=`echo "bkg2${function}pol${pol}${cuttheta}"`
	  chi2=`grep ${name} ${file_bkg2} | awk '{print $2}'`
	  chi2list_bkg2="${chi2list_bkg2} &  ${chi2}"
	  
	  name=`echo "bkg3${function}pol${pol}${cuttheta}"`
	  chi2=`grep ${name} ${file_bkg3} | awk '{print $2}'`
	  chi2list_bkg3="${chi2list_bkg3} &  ${chi2}"
	  
      fi      
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
    echo "\\includegraphics[width=\\linewidth]{/afs/cern.ch/work/c/cgoudet/private/plot/pdf/fit_gghtsallispol2${cuttheta}_gen.pdf}"
    echo "\\end{minipage}"
    echo "\\begin{minipage}{0.33\\linewidth}"
    echo "\\includegraphics[width=\\linewidth]{/afs/cern.ch/work/c/cgoudet/private/plot/pdf/fit_vbftsallispol1${cuttheta}_gen.pdf}"
    echo "\\end{minipage}"
    echo "\\begin{minipage}{0.33\\linewidth}"
    echo "\\includegraphics[width=\\linewidth]{/afs/cern.ch/work/c/cgoudet/private/plot/pdf/fit_bkgtsallispol2${cuttheta}_gen.pdf}"
    echo "\\end{minipage}"

echo "\\end{figure}"
done # end loop cut

############################################################################
#############################  RECO  ###############################################
############################################################################
for int in `seq 1 5` 
  do shift 
done

# liste tous les gghlandpol0 | garde la colonne 1 | coupe la 2e partie
cutlist=`grep gghlandpol0 ${file_ggh} | grep cuttheta | awk '{print $1}' | cut -d _ -f 2 | cut -d a -f 2` 
buffer=""
#Fill tabular for cuts
echo "\\clearpage"
echo "\\part{Reconstructed Level}"
echo "\\section*{Categories}"
echo "Category 0 : Both Photons Unconverted and in the Barrel\\\\"
echo "Category 1 : Both Photons in the Barrel, at Least One Converted\\\\"
echo "Category 2 : Both Photons Unconverted, at Least One in the Endcap\\\\"
echo "Category 3 : At Least One Photon Converted, at Least One in the Endcap\\\\"


for categ in `seq -1 3`
  do
file_ggh=$3
file_ggh2=$1
file_ggh3=$2
file_vbf=$6
file_vbf2=$4
file_vbf3=$5
file_bkg=$9
file_bkg2=$7
file_bkg3=$8

  if [[ ${categ} = "-1" ]];
      then 
      echo "\\section{Inclusive on Categories}"
      buffcat=""    
  else
      echo "\\clearpage"
      echo "\\section{Category ${categ}}"
      buffcat="_categ${categ}"
  fi
  for cut in `echo "0 ${cutlist}"`
    do
    if [[ "${cut}" = "0" ]];
	then 
	echo "\\subsection{Inclusive on \$|cos(\\theta*)|\$}"
	buffer="${buffer} cuttheta"
    else echo "\\clearpage"
	echo "\\subsection{\$|cos(\\theta*)|>0.${cut}\$}"
    fi
    echo "\\begin{figure}[!h]"
    
    if [[ "${cut}" = "0" ]];
	then   echo "\\caption{\$\\chi^2\$ values for ggh, vbg and background events inclusive on \$|cos(\\theta*)|\$}"
			
			cuttheta=""
			grepvar=" | grep -v \"cuttheta\" " 
    else   echo "\\caption{\$\\chi^2\$ values for ggh, vbg and background events with \$cos(\\theta*)>0.${cut}\$}"
	cuttheta="_cuttheta${cut}"
	grepvar=""
    fi
    
    echo "\\begin{tabular}{|c|c|c|c|c|c|c|c|c|c|}"
    echo "\\hline"
    echo "${func} \\\\"
    echo "\\hline"
    for function in `echo "land logn tsallis"`
      do
      for pol in `seq 0 2`
	do
	if [[ (${cut} = "0") && (${categ} = "-1") ]];
	    then
	    name=`echo "ggh${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_ggh} | grep -v "cuttheta" | grep -v "categ" | awk '{print $2}'`
	    chi2list_ggh="${chi2list_ggh} &  ${chi2}"
	    name=`echo "ggh2${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_ggh2} | grep -v "cuttheta" | grep -v "categ" | awk '{print $2}'`
	    chi2list_ggh2="${chi2list_ggh2} &  ${chi2}"
	    name=`echo "ggh3${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_ggh3} | grep -v "cuttheta" | grep -v "categ" | awk '{print $2}'`
	    chi2list_ggh3="${chi2list_ggh3} &  ${chi2}"
	
	
	    name=`echo "vbf${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_vbf} | grep -v "cuttheta" | grep -v "categ" | awk '{print $2}'`
	    chi2list_vbf="${chi2list_vbf} &  ${chi2}"
	    name=`echo "vbf2${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_vbf2} | grep -v "cuttheta" | grep -v "categ" | awk '{print $2}'`
	    chi2list_vbf2="${chi2list_vbf2} &  ${chi2}"
	    name=`echo "vbf3${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_vbf3} | grep -v "cuttheta" | grep -v "categ" | awk '{print $2}'`
	    chi2list_vbf3="${chi2list_vbf3} &  ${chi2}"

	    name=`echo "bkg${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_bkg} | grep -v "cuttheta" | grep -v "categ" | awk '{print $2}'`
	    chi2list_bkg="${chi2list_bkg} &  ${chi2}"
	    name=`echo "bkg2${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_bkg2} | grep -v "cuttheta" | grep -v "categ" | awk '{print $2}'`
	    chi2list_bkg2="${chi2list_bkg2} &  ${chi2}"
	    name=`echo "bkg3${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_bkg3} | grep -v "cuttheta" | grep -v "categ" | awk '{print $2}'`
	    chi2list_bkg3="${chi2list_bkg3} &  ${chi2}"
	
	elif [[ ${cut} = "0" ]];
	    then
	    name=`echo "ggh${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_ggh} | grep -v "cuttheta" | awk '{print $2}'`
	    chi2list_ggh="${chi2list_ggh} &  ${chi2}"
	    name=`echo "ggh2${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_ggh2} | grep -v "cuttheta"  | awk '{print $2}'`
	    chi2list_ggh2="${chi2list_ggh2} &  ${chi2}"
	    name=`echo "ggh3${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_ggh3} | grep -v "cuttheta"  | awk '{print $2}'`
	    chi2list_ggh3="${chi2list_ggh3} &  ${chi2}"
	
	
	    name=`echo "vbf${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_vbf} | grep -v "cuttheta"  | awk '{print $2}'`
	    chi2list_vbf="${chi2list_vbf} &  ${chi2}"
	    name=`echo "vbf2${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_vbf2} | grep -v "cuttheta"  | awk '{print $2}'`
	    chi2list_vbf2="${chi2list_vbf2} &  ${chi2}"
	    name=`echo "vbf3${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_vbf3} | grep -v "cuttheta"  | awk '{print $2}'`
	    chi2list_vbf3="${chi2list_vbf3} &  ${chi2}"

	    name=`echo "bkg${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_bkg} | grep -v "cuttheta"  | awk '{print $2}'`
	    chi2list_bkg="${chi2list_bkg} &  ${chi2}"
	    name=`echo "bkg2${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_bkg2} | grep -v "cuttheta"  | awk '{print $2}'`
	    chi2list_bkg2="${chi2list_bkg2} &  ${chi2}"
	    name=`echo "bkg3${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_bkg3} | grep -v "cuttheta"  | awk '{print $2}'`
	    chi2list_bkg3="${chi2list_bkg3} &  ${chi2}"
	
  elif [[ ${categ} = "-1" ]];
	    then
	    name=`echo "ggh${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_ggh}  | grep -v "categ" | awk '{print $2}'`
	    chi2list_ggh="${chi2list_ggh} &  ${chi2}"
	    name=`echo "ggh2${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_ggh2}  | grep -v "categ" | awk '{print $2}'`
	    chi2list_ggh2="${chi2list_ggh2} &  ${chi2}"
	    name=`echo "ggh3${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_ggh3}  | grep -v "categ" | awk '{print $2}'`
	    chi2list_ggh3="${chi2list_ggh3} &  ${chi2}"
	
	
	    name=`echo "vbf${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_vbf}  | grep -v "categ" | awk '{print $2}'`
	    chi2list_vbf="${chi2list_vbf} &  ${chi2}"
	    name=`echo "vbf2${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_vbf2}  | grep -v "categ" | awk '{print $2}'`
	    chi2list_vbf2="${chi2list_vbf2} &  ${chi2}"
	    name=`echo "vbf3${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_vbf3}  | grep -v "categ" | awk '{print $2}'`
	    chi2list_vbf3="${chi2list_vbf3} &  ${chi2}"

	    name=`echo "bkg${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_bkg}  | grep -v "categ" | awk '{print $2}'`
	    chi2list_bkg="${chi2list_bkg} &  ${chi2}"
	    name=`echo "bkg2${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_bkg2}  | grep -v "categ" | awk '{print $2}'`
	    chi2list_bkg2="${chi2list_bkg2} &  ${chi2}"
	    name=`echo "bkg3${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_bkg3}  | grep -v "categ" | awk '{print $2}'`
	    chi2list_bkg3="${chi2list_bkg3} &  ${chi2}"


	else
	    name=`echo "ggh${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_ggh}   | awk '{print $2}'`
	    chi2list_ggh="${chi2list_ggh} &  ${chi2}"
	    name=`echo "ggh2${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_ggh2}   | awk '{print $2}'`
	    chi2list_ggh2="${chi2list_ggh2} &  ${chi2}"
	    name=`echo "ggh3${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_ggh3}   | awk '{print $2}'`
	    chi2list_ggh3="${chi2list_ggh3} &  ${chi2}"
	
	
	    name=`echo "vbf${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_vbf}   | awk '{print $2}'`
	    chi2list_vbf="${chi2list_vbf} &  ${chi2}"
	    name=`echo "vbf2${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_vbf2}   | awk '{print $2}'`
	    chi2list_vbf2="${chi2list_vbf2} &  ${chi2}"
	    name=`echo "vbf3${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_vbf3}   | awk '{print $2}'`
	    chi2list_vbf3="${chi2list_vbf3} &  ${chi2}"

	    name=`echo "bkg${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_bkg}   | awk '{print $2}'`
	    chi2list_bkg="${chi2list_bkg} &  ${chi2}"
	    name=`echo "bkg2${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_bkg2}   | awk '{print $2}'`
	    chi2list_bkg2="${chi2list_bkg2} &  ${chi2}"
	    name=`echo "bkg3${function}pol${pol}${cuttheta}"`
	    chi2=`grep ${name} ${file_bkg3}   | awk '{print $2}'`
	    chi2list_bkg3="${chi2list_bkg3} &  ${chi2}"
	
fi	
      done #end loop pol
    done #end loop function
  echo "ggh  ${chi2list_ggh} \\\\"
  echo "ggh2  ${chi2list_ggh2} \\\\"
  echo "ggh3  ${chi2list_ggh3} \\\\"
  echo "vbf  ${chi2list_vbf} \\\\"
  echo "vbf2  ${chi2list_vbf2} \\\\"
  echo "vbf3  ${chi2list_vbf3} \\\\"
  echo "bkg  ${chi2list_bkg} \\\\"
  echo "bkg2  ${chi2list_bkg2} \\\\"
  echo "bkg3  ${chi2list_bkg3} \\\\"
  chi2list_ggh=""
  chi2list_ggh2=""
  chi2list_ggh3=""
  chi2list_vbf=""
  chi2list_vbf2=""
  chi2list_vbf3=""
  chi2list_bkg=""
  chi2list_bkg2=""
  chi2list_bkg3=""
  
    echo "\\hline"
    echo "\\end{tabular}"
    echo "\\begin{minipage}{0.33\\linewidth}"
    echo "\\includegraphics[width=\\linewidth]{/afs/cern.ch/work/c/cgoudet/private/plot/pdf/fit_gghtsallispol2${cuttheta}${buffcat}_reco.pdf}"
    echo "\\end{minipage}"
    echo "\\begin{minipage}{0.33\\linewidth}"
    echo "\\includegraphics[width=\\linewidth]{/afs/cern.ch/work/c/cgoudet/private/plot/pdf/fit_vbftsallispol1${cuttheta}${buffcat}_reco.pdf}"
    echo "\\end{minipage}"
    echo "\\begin{minipage}{0.33\\linewidth}"
    echo "\\includegraphics[width=\\linewidth]{/afs/cern.ch/work/c/cgoudet/private/plot/pdf/fit_bkgtsallispol2${cuttheta}${buffcat}_reco.pdf}"
    echo "\\end{minipage}"

    echo "\\end{figure}"
  done # end loop cut
for int in `seq 1 9` 
  do shift 
done
done #endo loop categ


echo "\\end{document}"




exit 0
