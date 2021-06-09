#!/bin/bash

source /cvmfs/sft.cern.ch/lcg/views/LCG_98python3/x86_64-centos7-gcc10-opt/setup.sh

filename=${4}_${2}_${1}

rm -rf job_${filename}
mkdir job_${filename}
cd ./job_${filename}

/afs/desy.de/user/d/dudarboh/FCAL/LUCAS/build/Lucas -o ${filename} -p ${2} -m ${3} -r ${4}

rm -f ../${filename}.root
mv ${filename}.root ..
cd ..
rm -rf job_${filename}
