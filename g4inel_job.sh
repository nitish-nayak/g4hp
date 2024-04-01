#!/bin/bash

echo
echo "======== cd to CONDOR_DIR_INPUT ========"
cd $CONDOR_DIR_INPUT
pwd

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
echo "======== SETUP ROOT, BOOST and DK2NU ========"
echo "source setup_g4hp_cvmfs.sh"
source setup_g4hp_cvmfs.sh

echo "PHYSICS=${PHYSICS}"
echo "PARTICLE=${PARTICLE}"
echo "PPARTICLE=${PPARTICLE}"
echo "MIN_ENERGY=${MIN_ENERGY}"
echo "MAX_ENERGY=${MAX_ENERGY}"
echo "STEPSIZE=${STEPSIZE}"
echo "TARGET=${TARGET}"
echo "NEVTS=${NEVTS}"
echo "NUMRUN=${NUMRUN}"
echo "TUPLEFILE=${TUPLEFILE}"
echo "PROCESS=$PROCESS"

dirOut="tmp_griddir${RANDOM}"

#making the run number:
THISPROCESS=${PROCESS}
run=$(( 1000000 * ${NUMRUN}  + THISPROCESS))


ENERGY1=$(echo "${MIN_ENERGY} + 10 * $STEPSIZE * $PROCESS" | bc )
ENERGY2=$(echo "${MIN_ENERGY} + 10 * $STEPSIZE * ($PROCESS + 1) - $STEPSIZE" | bc )

echo "Going from "$ENERGY1" to "$ENERGY2" in steps of "$STEPSIZE

STEP=0
for ENERGY in $(seq "${ENERGY1}" "${STEPSIZE}" "${ENERGY2}"); do
    ENERGYFILE="inel_start"${ENERGY1}"_step"${STEP}"_inc"${PPARTICLE}".root"
    echo "./g4hp -k ${dirOut} -t ${TARGET} -p ${PARTICLE} -n ${NEVTS} -f ${ENERGYFILE} -x ${run} -e ${ENERGY} -l ${PHYSICS} -y"
    time ./g4hp -k ${dirOut} -t ${TARGET} -p ${PARTICLE} -n ${NEVTS} -f ${ENERGYFILE} -x ${run} -e ${ENERGY} -l ${PHYSICS} -y
    STEP=$(($STEP + 1))
done

echo "Current content of the outdir:"
ls ${dirOut}/

echo "Hadding files for various scanned incident energies"
hadd -f -k ${dirOut}/"${TUPLEFILE}" ${dirOut}/inel_start*_step*_inc*.root

echo "Removing smaller root files"
rm ${dirOut}/inel_start*_step*_inc*.root


echo "Current content of the dir:"
ls
echo
echo "Moving output to CONDOR_DIR_G4HP: "
echo "=> CONDOR_DIR_G4HP: $CONDOR_DIR_G4HP"
mv ${dirOut}/*.root $CONDOR_DIR_G4HP
