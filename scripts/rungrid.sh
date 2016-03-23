#!/bin/sh
gridpack=$1
nevents=$2
seed=$3
outputlhe=$4

mkdir /gridpack
cd /gridpack/
cp -r $gridpack .
tar -xzvf $gridpack
./run.sh $nevents $seed
cat events.lhe.gz |gunzip -d > $outputlhe

