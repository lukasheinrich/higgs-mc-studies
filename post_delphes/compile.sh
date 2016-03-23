MY_DELPHES_DIR=/Users/cyril/Travail/NecessaryApps/delphes

cp analyzer.* ${MY_DELPHES_DIR}
cp Makefile ${MY_DELPHES_DIR}
cp libloader.C ${MY_DELPHES_DIR}
cd ${MY_DELPHES_DIR}

make