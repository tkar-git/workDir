#!/usr/bin/env bash
. ./runall.sh --run anaComba run_fit_noB --range 1099 1099 >../log/outCombaFit_1099
./runx_root_plot >../log/outRootPlotX_1099  # here the run number has to be entered manually
./run_cosmic_estimate.sh >../log/outCosmicEst_00001
