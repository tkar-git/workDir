#!/usr/bin/env bash
# date: 24.06.2025
# last modified: 24.06.2025
# author: Tamasi Kar
# email: kar@physi.uni-heidelberg.de
# ------------------------------------------------------------------------- #
ScriptDir="$PWD"
WorkDir="${ScriptDir}/.."
DataDir=/testbeamdata/mu3e2025/data
RawDir="${DataDir}/raw"
# ---- CHANGE OutDir BASED ON SIM or DATA ---- #
#RunListFile=${WorkDir}/log/tk_list.txt
RunListFile=${RawDir}/list.txt
cd $RawDir
while IFS= read -r i; do
    run=$(printf "%05d" $i)
    if [ ! -e  run${run}.json ]; then
        if [ ! -e  run${run}.mid ]; then
            lz4 -d run${run}.mid.lz4
        fi
        odbhist -e run${run}.mid
        mv $RawDir/run${run}.odb $RawDir/run${run}.json
        rm run${run}.mid
    fi
done < $RunListFile 
cd $ScriptDir
