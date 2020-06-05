#!/bin/bash
#export ROOTSYS=/lustrehome/fsimone/root/
#source $ROOTSYS/bin/thisroot.sh 

echo "run TMVA from ROOT > 6.14 (CMSSW_10_6_8)"

echo "root -l -b MVA_muonid.cpp\(\"barrel\"\)"
root -l -b MVA_muonid.cpp\(\"barrel\"\)

echo "root -l -b MVA_muonid.cpp\(\"endcap\"\)"
root -l -b MVA_muonid.cpp\(\"endcap\"\)

#echo "root -l -b MVA_muonid.cpp\(\"barrelendcap\"\)"
#root -l -b MVA_muonid.cpp\(\"barrelendcap\"\)
