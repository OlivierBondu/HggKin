#!/bin/bash

batchdir=`pwd`

source ~cgoudet/534.sh

cp /afs/cern.ch/work/c/cgoudet/private/data/kin_dist.root ${batchdir}
cp /afs/cern.ch/work/c/cgoudet/private/codes/HggKin/fit_dist2.exe ${batchdir}


cd ${batchdir}



./fit_dist2.exe  2>logerror.txt 1>log.txt

rm kin_dist.root
cp * /afs/cern.ch/work/c/cgoudet/public/BatchDump/.

