#!/bin/bash

# Make box around text @climagic
function box() { t="$1xxxx";c=${2:-=}; echo ${t//?/$c}; echo "$c $1 $c"; echo ${t//?/$c}; }

for i in $(seq 1 1 10); do
    minen=$(( ($i-1)*16 ))
    maxen=$(( $i*16 ))
    box "Submitting 160 jobs for scanning in range ("$minen", "$maxen")"
    ./ProcessG4Inel.py --particle kaon+ --target C --outdir=/pnfs/uboone/scratch/users/bnayak/ppfx_newg4/g4inel/kap_C/maxen_"$maxen"/ --min_energy "$minen" --max_energy "$maxen" --nevts 1000000 --njobs 160
done

