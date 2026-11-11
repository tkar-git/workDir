#!/usr/bin/env bash

export SCRIPT_DIR=$PWD
echo $SCRIPT_DIR

# export DATA_DIR=/testbeamdata/psiprac2025
# export RAW_DIR="${DATA_DIR}/rawData"
# export SORT_DIR="${DATA_DIR}/sort"
# export DATA_DIR="/testbeamdata/mu3e2025/data"
export DATA_DIR="/testbeamdata/mu3e2026/data"
export RAW_DIR="${DATA_DIR}/raw/012"
# export SORT_DIR="${DATA_DIR}/sort/beam/v6.4.4"
# export SORT_DIR="${DATA_DIR}/sort/beam/daqBugFix"
export SORT_DIR="${DATA_DIR}/sort/011"
if [ ! -d "$SORT_DIR/log" ]; then
    mkdir -p "$SORT_DIR/log"
fi
source "${SCRIPT_DIR}/../../env-mu3e"

export CDB_PATH=${RAW_DIR}/../cdb/
export GLOBAL_TAG=datav6.3=2025V0      # ideal Geant4 alignment
# export GLOBAL_TAG=datav6.3=2025V1test  # metrology-improved alignment
# export GLOBAL_TAG=datav6.3=2025V2test  # cosmic's improved alignment
generate_cmds() 
{
    # for i in $(seq 452 1062) # change run range here
    # for i in $(seq 5787 6212) # change run range here
    # for i in $(seq 4500 6301) # change run range here
    # for i in $(seq 9971 9989) # change run range here
    for i in $(seq 12500 12500) # change run range here
    do
        RUN=$(printf "%05d" $i)
        echo "$RUN $j"
    done
}

export -f generate_cmds

# This function will be run in parallel
run_one() 
{
    RUN="$1"
    if [ -f "$RAW_DIR/run${RUN}.mid.lz4" ]
    then
        echo "[INFO] Running mu3eSort for RUN: $RUN"
        mu3eSort --input "$RAW_DIR/run${RUN}.mid.lz4" \
            --output "$SORT_DIR/run${RUN}_testSort0911.root" --sort.midas.ignoreEventIds=1 > "$SORT_DIR/log/sort_out_${RUN}" 2>&1
        if [ ! -f "$SORT_DIR/run${RUN}.root" ]
        then
            rm "$SORT_DIR/log/sort_out_${RUN}"
        else
            gzip "$SORT_DIR/log/sort_out_${RUN}"
        fi
    else
        echo "[WARNING] RAW File $RAW_DIR/run${RUN}.mid.lz4 does not exist"
    fi
}

export -f run_one

# Use GNU parallel
generate_cmds | parallel -j 10 --colsep ' ' run_one {1}
