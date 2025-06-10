#!/usr/bin/env bash
# ------------------------------------------------------------------------- #
# Description:
# This script compiles the projects specified using --compile
# and runs the executables specified with --run for run range 
# specified by --range (same number should be provided twice 
# if you want to run on a single run)
# A match of the provided arguments is made to a list sepcified in
# Projects. Executables
#
# Arguments: 
# --compile ProjName1 ProjName2 ...
# --run exe1 exe2 exe3 --range int1 int2
#
# Usage: ./runall.sh --compile mu3e anacombi tripletSim \
# -- run mu3eSort anaComba run_fit_noB --range 551 553
#
# date: 29.04.2025
# last modified: 10.06.2025
# author: Tamasi Kar
# email: kar@physi.uni-heidelberg.de
# ------------------------------------------------------------------------- #

ScriptDir="$PWD"
WorkDir="${ScriptDir}/.."
Projects=("mu3e" "anacomba" "tripletSim")
Executables=("mu3eSort" "anaComba" "run_fit_noB")
InstallScript1="setup_for_tachyon.sh"
InstallScript2="re_install.sh"

DataDir=/testbeamdata/mu3e2025/data
AlignmentFile="$DataDir/test.sim1.root"
RawDir="${DataDir}/raw"
# ---- CHANGE OutDir BASED ON SIM or DATA ---- #
OutDir="${DataDir}/rec"
#OutDir="${WorkDir}/out"
#OutDir="${DataDir}/sim/cosmic"
NComb=(1 4) # Number of combinations range

Compile=""
Execute=""
Range=""
Projects_=()
Executables_=()
RunRange_=()

# Parse named arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --compile)
            Compile="compile"
            shift
            while [[ $# -gt 0 && ! $1 =~ ^-- ]]; do
                Projects_+=("$1")
                shift
            done
            if [[ ${#Projects_[@]} -eq 0 ]]; then
                echo "Error: --compile requires at least 1 argument."
                exit 1
            fi
            ;;
        --run)
            Execute="execute"
            shift
            while [[ $# -gt 0 && ! $1 =~ ^-- ]]; do
                Executables_+=("$1")
                shift
            done
            if [[ ${#Executables_[@]} -eq 0 ]]; then
                echo "Error: --run requires at least 1 argument."
                exit 1
            fi
            ;;
        --range)
            Range="range"
            shift
            while [[ $# -gt 0 && ! $1 =~ ^-- ]]; do
                if ! [[ "$1" =~ ^[0-9]+$ ]]; then
                    echo "[INVALID]: Please enter a valid integer number"
                    exit 1
                else
                    RunRange_+=("$1")
                    shift
                fi
            done
            if [[ ${#RunRange_[@]} -le 1 ]]; then
                echo "Error: --range requires at least 2 integer numbers"
                exit 1
            fi
            ;;
        *)
            echo "Unknown argument: $1"
            exit 1
            ;;
    esac
done
# Function to compile the project
# Arguments: ProjName
# Usage: compile_project ProjName
# Example: compile_project mu3e
# ----------------------------------------------------------- #
compile_project() {
    local ProjName=$1
    local ProjDir="${WorkDir}/../${ProjName}"

    if [[ ! " ${Projects[@]} " =~ " ${ProjName} " ]]; then
        echo "[INFO]: Unknown project ${ProjName}"
        return
    fi

    if [ ! -d "$ProjDir" ]; then
        echo "[INFO]: NO PROJECT ${ProjDir}"
        return
    fi

    echo
    echo "[INFO]: =============== Compiling ${ProjName} ==================="
    case $ProjName in
        "${Projects[0]}")
            echo "[INFO]: cd ${ProjDir}/.."
            echo "[INFO]: source $InstallScript1"
            cd "$ProjDir/.."
            source "$InstallScript1"
            ;;
        "${Projects[1]}")
            echo "[INFO]: cd ${ProjDir}"
            echo "[INFO]: source $InstallScript2"
            cd "$ProjDir/.."
            source $InstallScript2
            ;;
        "${Projects[2]}")
            echo "[INFO]: cd ${ProjDir}"
            cd "$ProjDir"
            make clean
            make fitting
            ;;
    esac
}
# ------------------------------------------------------------ #
# Function to execute the project executables
# Arguments: exe run
# Usage: execute_project exe run
# Example: execute_project mu3eSort run
# ------------------------------------------------------------ #

execute_project() {
    local exe="$1"
    local run=$2
    cd $WorkDir
    echo
    echo "[INFO]: =============== Running ${exe}  for RUN ${run} ==================="

    case $exe in
        "${Executables[0]}")
            source ${WorkDir}/../env-mu3e
            echo "Running ${exe} for RUN: ${run}"
            # run on midas files to produce root out similar to simulation output
            $exe --alignment.file=$AlignmentFile $RawDir/run${run}.mid.lz4 --output $OutDir/run${run}.root
            ;;
        "${Executables[1]}")
            echo "source ${WorkDir}/../env-anaComba"
            source ${WorkDir}/../env-anaComba
            echo $PATH
            root-config --version
            for j in $(seq ${NComb[0]} ${NComb[1]})
            do
                echo "Running ${exe} for RUN: ${run} with ${j} hits"
                $exe $OutDir/run${run}.root $OutDir/gttfInp_run${run}_${j}hits.root $j
            done
            ;;
        "${Executables[2]}")
            for j in $(seq ${NComb[0]} ${NComb[1]})
            do
                echo "Running ${exe} for RUN: ${run} with ${j} hits"
                ${WorkDir}/../"${Projects[2]}"/$exe $OutDir/gttfInp_run${run}_${j}hits.root gttfOut_run${run}_${j}hits.root $j
            done
            ;;
    esac
}

# -------------------------- Main Script starts here ---------------------------- #
if [ "$Compile" == "compile" ]; then
    for ProjName in "${Projects_[@]}"; do
        compile_project "$ProjName"
    done
    echo
    echo "[INFO]: =========================== Compiling done ==================================="
    cd $ScriptDir
    echo
fi

if [ "$Execute" != "execute"  ] || [ "$Range" != "range" ]; then
    echo "[INVALID:] Please parse arguments for --run and --range."
    exit 1
else
    for exe in "${Executables_[@]}"
    do
        if [[ ! " ${Executables[@]} " =~ " ${exe} " ]]; then
            echo "[INFO]: Unknown Executable ${exe}"
            exit 1
        fi
        for i in $(seq "${RunRange_[0]}" "${RunRange_[1]}")
        do
            run=$(printf "%05d" $i)
            execute_project "$exe" "$run"
        done
    done
    echo
    echo "[INFO]: =========================== Running done ==================================="
    cd $ScriptDir
fi
