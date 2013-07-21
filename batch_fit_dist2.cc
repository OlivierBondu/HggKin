#!/bin/bash

batchdir=`pwd`

source ~cgoudet/534.sh

cp /afs/cern.ch/work/c/cgoudet/private/data/kin_dist.root ${batchdir}
cp /afs/cern.ch/work/c/cgoudet/private/codes/HggKin2/fit_dist2.exe ${batchdir}


cd ${batchdir}

pwd
ls

./fit_dist2.exe  2>logerrorfitdist.txt 1>logfitdist.txt

rm dummy.root
rm kin_dist.root
cp * /afs/cern.ch/work/c/cgoudet/public/BatchDump/.

