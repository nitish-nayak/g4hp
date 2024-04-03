#!/bin/bash

jobdir=$1
dirstr=$2

if command -v fd &> /dev/null; then
    ntot=`fd -g "*.root" $jobdir | wc -l`
else
    ntot=`find "$jobdir" -name "*.root" | wc -l`
fi
echo "Number of Input files : "$ntot

cpdir="$dirstr"/`basename "$jobdir"`

if [ ! -d "$cpdir" ]; then
    mkdir -p "$cpdir"
    mkdir -p "$cpdir"/parts
fi

cd ..

# split jobs
echo "Making filelists.."
if command -v fd &> /dev/null; then
    fd -g "*.root" $jobdir | split -l 160 -d
else
    find "$jobdir" -name "*.root" | split -l 160 -d
fi

for i in {0..9}; do
    ./ana/CreateInelXS "x0"$i $cpdir"/parts/xs_part"$i".root" $cpdir"/parts/xs_summary_part"$i".txt" &
done

wait
echo "Done!"

# hadd files
hadd -f -k $cpdir"/xs_hists.root" $cpdir"/parts/"*.root
cat $cpdir"/parts/"*.txt >> $cpdir"/xs_summary.txt"

# cleaning up
echo "Cleaning up"
rm -rf x0*
