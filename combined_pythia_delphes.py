#!/usr/bin/env python
import click
import subprocess
import os
import shlex

@click.command()
@click.argument('lhefile')
@click.argument('pythiacard')
@click.argument('delphescard')
@click.argument('delphesout')
def combined_run(lhefile,pythiacard,delphescard,delphesout):
    fifoname = '/tmp/combinedhepmc.fifo'
    os.mkfifo(fifoname)
    
    pythia = '/analysis/pythia_main/example_main {pythiacard} {hepmcfile} {lhefile}'.format(
        pythiacard = pythiacard,
        hepmcfile = fifoname,
        lhefile = lhefile
    )
    delphes = 'DelphesHepMC {delphescard} {delphesout} -'.format(
        delphescard = delphescard,
        delphesout = delphesout,
        hepmcfile = fifoname
    )

    p_pythia  = subprocess.Popen(shlex.split(pythia))
    p_delphes = subprocess.Popen(shlex.split(delphes), stdin = open(fifoname))
    
    p_pythia.wait()
    p_delphes.wait()

if __name__ == '__main__':
  combined_run()