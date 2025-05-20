#! /usr/bin/env bash
#g++ -o est_cosmics cosmic_estimate.cpp `root-config --cflags --glibs` -DDEBUG
g++ -o est_cosmics cosmic_estimate.cpp `root-config --cflags --glibs`
run_number=1

for i in {3..4..1}
do
    ./est_cosmics ${run_number} $i
done
