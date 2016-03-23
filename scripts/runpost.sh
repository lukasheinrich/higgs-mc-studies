#!/bin/sh
delphesroot=$1
postroot=$2


cd /code/delphes/
cp /analysis/post_delphes/* .
make
./analyzer $delphesroot $postroot
