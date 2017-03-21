#!/usr/bin/env python
import click
import yaml
import sys
import tempfile
import os
import distutils.spawn
import importlib

@click.command()
@click.option('-i','--inyaml')
@click.option('-f','--fileyaml')
@click.option('-p', '--prefactor', default=1, help='Prefactor for the couplings')
@click.argument('template')
@click.argument('outfile')
def paramfromyaml(inyaml,fileyaml,template,outfile):

    if not inyaml and not fileyaml:
        click.secho('need to provide either -i or -f option', fg = 'red')
        raise click.Abort()
    if inyaml:
        pardata = yaml.load(inyaml)
    if fileyaml:
        pardata = yaml.load(sys.stdin if inyaml == '-' else open(fileyaml))
    
    outfilename = outfile

    
    pardata = {'value_'+k:prefactor*v for k,v in pardata.iteritems()}
    formatted = open(template).read().format(**pardata)

    outputfile = sys.stdout if outfile == '-' else open(outfile,'w')

    outputfile.write(formatted)
    

if __name__ == '__main__':
    paramfromyaml()
