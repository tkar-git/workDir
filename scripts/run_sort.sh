#!/usr/bin/env bash
# date: 21.06.2025
# last modified: 21.06.2025
# author: Tamasi Kar
# email: kar@physi.uni-heidelberg.de
# ------------------------------------------------------------------------- #
ScriptDir="$PWD"
WorkDir="${ScriptDir}/.."
InstallScript1="setup_for_tachyon.sh"
exe=mu3eSort
DataDir=/testbeamdata/mu3e2025/data
AlignmentFile="$DataDir/test.sim1.root" 
RawDir="${DataDir}/raw"
# ---- CHANGE OutDir BASED ON SIM or DATA ---- #
OutDir="${DataDir}/rec"
RunListFile=${WorkDir}/log/tk_list.txt

source ${WorkDir}/../env-mu3e

while IFS= read -r i; do
    run=$(printf "%05d" $i)
    log_out=../log/sort_out_${run}
    echo "Running ${exe} for RUN: ${run}"
    InputFile=${RawDir}/run${run}.mid.lz4
    OutputFile=${OutDir}/run${run}.root
    echo "InputFile: ${InputFile}"
    echo "OutputFile: ${OutputFile}"
    $exe --alignment.file=$AlignmentFile $InputFile --output $OutputFile 
done < $RunListFile 
