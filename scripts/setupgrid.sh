#!/bin/sh
inputpar=$1
outputdest=$2

./steermadgraph.py ./template_cards/generic_proc_card_mg5_pp_x0_jj4l_vbf_LO.dat ./inputs/gridpack_run.dat $inputpar out.lhe -e 1000000
cp madgraphrun/output_gridpack.tar.gz $outputdest
