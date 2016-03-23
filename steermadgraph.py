#!/usr/bin/env python
import click
import subprocess
import shlex

@click.command()
@click.argument('proccard')
@click.argument('runcard')
@click.argument('paramcard')
@click.argument('outputlhe')
@click.option('-e','--events', default = 10000)
def runmadgraph(proccard,runcard,paramcard,outputlhe,events):
    outputdir   = 'madgraphrun'
    runcardtmpl = 'steer.dat'
    runcardname = 'mg5runcard.dat'

    with open(runcardtmpl) as template:
        with open(runcardname,'w') as filled:
            filled.write(template.read().format(PROC    = proccard,
     						                    RUN     = runcard,
                                                OUTPUT  = outputdir,
                                                PARAM   = paramcard,
                                                NEVENTS = events))
    subprocess.check_call(shlex.split('mg5 -f {}'.format(runcardname)))
    with open(outputlhe,'w') as lhefile:
        subprocess.check_call('cat {}/Events/{}/unweighted_events.lhe.gz | gunzip -d'.format(outputdir,'output'), shell = True, stdout = lhefile)

if __name__ == '__main__':
    runmadgraph()
