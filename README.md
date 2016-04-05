runnable within docker: 

    docker build -t lukasheinrich/higgs-mc-studies-base base 
    docker build -t lukasheinrich/higgs-mc-studies .
    docker volume create --name yadageworkdir
---
    
    
    alias indocker='docker run -it -v yadageworkdir:/workdir lukasheinrich/higgs-mc-studies'
    indocker ./paramfromtemplate.py inputs/template_paramcard.dat /workdir/param.dat  -i '{kHzz: 0.5, kAzz: 0.5, kHww: 0.3, kAww: 0.3}'
    indocker ./scripts/setupgrid.sh /workdir/param.dat /workdir/grid.tar.gz
    indocker ./scripts/rungrid.sh /workdir/grid.tar.gz 1000 1234 /workdir/output.lhe

    {
    indocker /analysis/pythia_main/example_main /analysis/mainPythiaMLM.cmnd /workdir/output.hepmc /workdir/output.lhe
    indocker DelphesHepMC /analysis/template_cards/modified_delphes_card_ATLAS.tcl /workdir/output.root /workdir/output.hepmc
    } or {
    indocker ./combined_pythia_delphes.py /workdir/output.lhe /analysis/mainPythiaMLM.cmnd /analysis/template_cards/modified_delphes_card_ATLAS.tcl  /workdir/output.root
    }

    indocker ./scripts/runpost.sh /workdir/output.root /workdir/post.root
