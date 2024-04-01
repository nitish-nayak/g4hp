#!/bin/bash

en=$1
jobdir="/pnfs/uboone/scratch/users/bnayak/ppfx_newg4/g4hp/incp_"$en

echo "Number of Output files : "
fd -g "*.root" $jobdir | wc -l

cpdir="/exp/uboone/data/users/bnayak/ppfx/flugg_studies/new_g4_qe/incp_"$en

if [ ! -d "$cpdir" ]; then
    mkdir -p "$cpdir"
    mkdir -p "$cpdir"/parts
fi

# split jobs
echo "Making filelists.."
fd -g "*.root" $jobdir | split -l 100 -d

for i in {0..9}; do
    ./ana/CreateYields "$en" "x0"$i $cpdir"/parts/yields_part"$i".root" $cpdir"/parts/yields_qe_part"$i".txt" &
done

wait
echo "Done Creating Yields!"


# hadd files
hadd -f -k $cpdir"/yields_"$en".root" $cpdir"/parts/"*.root
cat $cpdir"/parts/"*.txt >> $cpdir"/yields_qe_"$en".txt"

# cleaning up
echo "Cleaning up"
rm -rf x0*
