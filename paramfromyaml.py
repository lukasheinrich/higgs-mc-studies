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
@click.option('--madgraph/--no-madgraph', default = False)
@click.argument('modelname')
@click.argument('outfile')
def paramfromyaml(inyaml,fileyaml,modelname,outfile,madgraph):
    if madgraph:
        madgraphexec = 'mg5'
        modelpath = '{}/../models'.format(os.path.dirname(os.path.realpath(distutils.spawn.find_executable(madgraphexec))))
        sys.path.append(modelpath)

    mod = importlib.import_module('{}'.format(modelname))
    pars = {p.name:p for p in mod.parameters.all_parameters if p.nature=='external'}
    pardata = None
    if not inyaml and not fileyaml:
        click.secho('need to provide either -i or -f option', fg = 'red')
        raise click.Abort()
    if inyaml:
        pardata = yaml.load(inyaml)
    if fileyaml:
        pardata = yaml.load(sys.stdin if inyaml == '-' else open(fileyaml))
    
    for k,v in pardata.iteritems():
        pars[k].value = v


    outfilename = outfile
    actual_stdout = sys.stdout
    if outfile == '-':
        tempfile.TemporaryFile()
        tmpfile = tempfile.NamedTemporaryFile()
        outfilename = tmpfile.name
        #hack because UFO writes stuff to stdout when printing :-/
        sys.stdout = open(os.devnull,'w')
        
    mod.write_param_card.ParamCardWriter(filename = outfilename, list_of_parameters = pars.values(), generic = True)

    if outfile == '-':
        actual_stdout.write(open(outfilename).read())
    

if __name__ == '__main__':
    paramfromyaml()