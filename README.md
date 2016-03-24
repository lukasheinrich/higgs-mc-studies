runnable within docker: 

    docker build -t lukasheinrich/higgs-mc-studies-base base 
    docker build -t lukasheinrich/higgs-mc-studies .

---
    
    
    alias indocker='docker run -it -v yadageworkdir:/workdir lukasheinrich/higgs-mc-studies'
    indocker ./scripts/setupgrid.sh testCards/test_param.dat /workdir/grid.tar.gz
    indocker ./scripts/rungrid.sh /workdir/grid.tar.gz 1000 1234 /workdir/output.lhe
    indocker /analysis/pythia_main/example_main /analysis/mainPythiaMLM.cmnd /workdir output.hepmc /workdir/output.lhe
    indocker DelphesHepMC /analysis/template_cards/modified_delphes_card_ATLAS.tcl /workdir/output.root /workdir/output.hepmc
    indocker ./scripts/runpost.sh /workdir/output.root /workdir/post.root
