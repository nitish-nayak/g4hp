#!/bin/bash

echo
echo "======== cd to CONDOR_DIR_INPUT ========"
cd $CONDOR_DIR_INPUT

echo
echo "======== ls ========"
ls

echo
echo "======== UNTARRING... ========"
tar xvfz local_install.tar.gz -C ./ > /dev/null

echo
echo "======== Done untarring. ls ========"
ls

echo
echo "======== SETUP G4, ROOT, ETC ========"
echo "source setup_g4hp_cvmfs.sh"
source setup_g4hp_cvmfs.sh

echo
echo "======== UPDATE MACRO WITH RUN NUMBER ========"
sed -i 's/\${JOBID}/'$PROCESS'/g' g4hp.mac

#OUTFILE="g4hp_pC_e${INCENERGY}_j${PROCESS}_r{RUN}.root"
sed -i 's/\${OUTNAMEFILE}/'$OUTFILE'/g' g4hp.mac

echo "RUNID=${RUNID}"
echo "PROCESS=$PROCESS"
echo "OUTFILE=$OUTFILE"

echo
echo "======== MACRO CONTENT ========"
cat g4hp.mac

echo
echo "======== EXECUTING g4hp ========"
echo "g4hp g4hp.mac"
g4hp g4hp.mac

echo
echo "Moving output to CONDOR_DIR_G4HP"
mv *.root $CONDOR_DIR_G4HP
