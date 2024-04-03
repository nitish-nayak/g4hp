#!/bin/bash

en=$1
jobdir=$2
dirstr=$3
# jobdir="/pnfs/uboone/scratch/users/bnayak/ppfx_newg4/g4hp/incp_"$en

echo "Number of Output files : "
if command -v fd &> /dev/null; then
    fd -g "*.root" $jobdir | wc -l
else
    find "$jobdir" -name "*.root" | wc -l
fi

# cpdir="/exp/uboone/data/users/bnayak/ppfx/flugg_studies/new_g4_qe/incp_"$en
cpdir="$dirstr"/incp_"$en"

if [ ! -d "$cpdir" ]; then
    mkdir -p "$cpdir"
    mkdir -p "$cpdir"/parts
fi

cd ..

# split jobs
echo "Making filelists.."
if command -v fd &> /dev/null; then
    fd -g "*.root" $jobdir | split -l 100 -d
else
    find "$jobdir" -name "*.root" | split -l 100 -d
fi

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
