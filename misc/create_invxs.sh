#!/bin/bash

# Make box around text @climagic
function box() { t="$1xxxx";c=${2:-=}; echo ${t//?/$c}; echo "$c $1 $c"; echo ${t//?/$c}; }

# dirstr="/exp/uboone/data/users/bnayak/ppfx/flugg_studies/new_g4_qe"
dirstr=$1

cd ..

ens=("158" "120" "110" "100" "90" "80" "70" "60" "50" "40" "31" "20" "12")
for en in "${ens[@]}"; do
    box "Energy "$en
    echo "Calculating Total Yields"
    sed -n "/Nentries/ { n; p }" "$dirstr"/incp_"$en"/yields_qe_"$en".txt  | awk -F' ' '{ print $6 }' | paste -sd+ - | bc > "$dirstr"/incp_"$en"/tot_yields_"$en".txt
    ./ana/CreateInvXS "$en" 20000000000 "$dirstr"/incp_"$en"/yields_"$en".root "$dirstr"/incp_"$en"/invxs_"$en".root "$dirstr"/incp_"$en"/tot_yields_"$en".txt
done

cd ana
for en in "${ens[@]}"; do
    if [ ! -d "PPFX" ]; then
        mkdir PPFX
    done
    box "Energy "$en
    ./CreatePPFX "$dirstr"/incp_"$en"/invxs_"$en".root "$dirstr"/incp_"$en"/yields_"$en".root "FTFP_BERT" "$en"
done
