#!/bin/bash

# Make box around text @climagic
function box() { t="$1xxxx";c=${2:-=}; echo ${t//?/$c}; echo "$c $1 $c"; echo ${t//?/$c}; }

dirstr="/exp/uboone/data/users/bnayak/ppfx/flugg_studies/new_g4_qe"
# dirstr=$1

cd ..

ens=("158" "120" "110" "100" "90" "80" "70" "60" "50" "40" "31" "20" "12")
for en in "${ens[@]}"; do
    box "Energy "$en
    echo "Calculating Total Yields"
    yield_prod=`sed -n "/Nentries/ { n; p }" "$dirstr"/incp_"$en"/yields_qe_"$en".txt  | awk -F' ' '{ print $6 }' | paste -sd+ - | bc`
    yield_tot=`sed -n "/Nentries/ { n; p }" "$dirstr"/incp_"$en"/yields_qe_"$en".txt  | awk -F' ' '{ print $2 }' | paste -sd+ - | bc`
    frac_prod=`echo "scale=10 ; $yield_prod / $yield_tot" | bc`
    echo $frac_prod" # fraction of prod yields" > "$dirstr"/incp_"$en"/tot_prod_yields_"$en".txt
    echo $yield_tot" # total yields" >> "$dirstr"/incp_"$en"/tot_prod_yields_"$en".txt
    ./ana/CreateInvXS "$en" 20000000000 "$dirstr"/incp_"$en"/yields_"$en".root "$dirstr"/incp_"$en"/invxs_"$en".root "$yield_tot" "$frac_prod"
done

cd ana
if [ ! -d "PPFX" ]; then
    mkdir PPFX
fi

for en in "${ens[@]}"; do
    box "Energy "$en
    ./CreatePPFXHP "$dirstr"/incp_"$en"/invxs_"$en".root "$dirstr"/incp_"$en"/yields_"$en".root "FTFP_BERT" "$en"
done
