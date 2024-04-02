#!/bin/bash

dirstr=$1
target=$2
parts=("pip" "pim" "kap" "kam" "prt" "neu")

for part in "${parts[@]}"; do
    echo "$dirstr"/"$part"_"$target"/xs_hists.root >> ../ana/tmp_input_inel.txt
done

cd ../ana

echo "Saving inelastic cross-sections for target "$target
./CreatePPFXInel tmp_input_inel.txt $target

echo "Cleaning up"
rm tmp_input_inel.txt
