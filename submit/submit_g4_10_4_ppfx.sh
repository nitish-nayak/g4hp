#!/bin/bash

cd ..

ens=("158" "120" "110" "100" "90" "80" "70" "60" "50" "40" "31" "20" "12")
for en in $ens; do
    ./ProcessG4HP.py --njobs=1000 --nevts=20000000 --energy="$en" --outdir=/pnfs/uboone/scratch/users/bnayak/ppfx_newg4/g4hp/incp_"$en"/
done
