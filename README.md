runnable within docker: 
    
    
    ./scripts/setupgrid.sh testCards/test_param.dat /workdir/grid.tar.gz
    ./scripts/rungrid.sh /workdir/grid.tar.gz 1000 1234 /workdir/output.lhe

    /analysis/pythia_main/example_main /analysis/mainPythiaMLM.cmnd /workdir/output.hepmc /workdir/output.lhe
    DelphesHepMC /analysis/cardsToSend/modified_delphes_card_ATLAS.tcl /workdir/output.root /workdir/output.hepmc
    ./scripts/runpost.sh /workdir/output.root /workdir/post.root