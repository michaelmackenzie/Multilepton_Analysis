import ROOT
import os, sys
import shutil
import subprocess
import argparse
from importlib import import_module

from PhysicsTools.NanoAODTools.postprocessing.framework.postprocessor import PostProcessor

from PhysicsTools.NanoAODTools.postprocessing.framework.datamodel import Collection
from PhysicsTools.NanoAODTools.postprocessing.framework.datamodel import Object
from PhysicsTools.NanoAODTools.postprocessing.framework.eventloop import Module

ROOT.PyConfig.IgnoreCommandLineOptions = True


#---------------------------------#
p = argparse.ArgumentParser(description='Select whether to download MC or data')
p.add_argument('input_dir'  , help='Type e.g. input file path')
p.add_argument('--outdir'   , help='Type e.g. output file path', default="", required=False)
p.add_argument('--mc_data'  , help='Type MC or Data for only MC or Data', default="", required=False)
p.add_argument('--year'     , help='Specific year to process',default="", required=False)
p.add_argument('--tag'      , help='Dataset tag list to process',default="", required=False)
p.add_argument('--veto'     , help='Comma separated list of tags to not process (e.g. DY,ttbar,Embed)',default="", required=False)
p.add_argument('--dryrun'   , help='Setup merging without running', action='store_true', required=False)
p.add_argument('--dosingle' , help='Merge first dataset only', action='store_true', required=False)
p.add_argument('--usedirect', help='Use the direct EOS path instead of XROOTD', action='store_true', required=False)
p.add_argument('--copylocal', help='Copy files locally to merge', action='store_true', required=False)
p.add_argument('--segments' , help='Use job segments rather than a merged ntuple', action='store_true', required=False)
p.add_argument('--onlylink' , help='Link segments instead of copying them over', action='store_true', required=False)
p.add_argument('--mergeseg' , help='Merge existing segment files', action='store_true', required=False)
p.add_argument('--jsononly' , help='Only process lumi json files', action='store_true', required=False)
p.add_argument('--skipjson' , help='Skip lumi json files', action='store_true', required=False)
p.add_argument('--verbose'  , help='Print additional information', action='store_true', required=False)

args = p.parse_args()

inputpath  = args.input_dir
outputpath = args.outdir
mc_data    = args.mc_data
year       = args.year
tag        = args.tag.split(',')
veto       = args.veto.split(',')
dryrun     = args.dryrun
dosingle   = args.dosingle
usedirect  = args.usedirect
copylocal  = args.copylocal
segments   = args.segments
onlylink   = args.onlylink
mergeseg   = args.mergeseg
jsononly   = args.jsononly
skipjson   = args.skipjson
verbose    = args.verbose

if copylocal and segments:
    print "Can't use both --copylocal and --segments!"
    exit()
if onlylink and not segments:
    print "--onlylink is only defined when using --segments!"
    exit()
if jsononly and skipjson:
    print "Can't use --jsononly and --skipjson"
    exit()

if inputpath[-1:] != '/':
    inputpath = inputpath + '/'

if outputpath == "":
    outputpath = "lfvanalysis_rootfiles/"
doDataMC = 0
if mc_data == "MC":
    print "Only processing MC..."
    doDataMC = -1
elif mc_data == "Data":
    print "Only processing Data..."
    doDataMC = 1
elif mc_data != "":
    print "Unrecognized Data/MC option! Ignoring it..."
if year != "":
    print "Processing only year tag", year

if outputpath[-1] != '/': outputpath += '/'
print "Using input path %s and output path %s" % (inputpath, outputpath)

#---------------------------------#

user = os.environ.get('USER')
hostname = os.environ.get('HOSTNAME')
host = 'lxplus' if 'lxplus' in hostname else 'lpc'


if host == 'lpc':
    #make a directory to store merged files temporarily
    if not os.path.exists("batch/temp_root/") :
        os.makedirs("batch/temp_root")
    tmp_dir = 'batch/temp_root/'
else:
    tmp_dir = os.environ.get('TMPDIR') + '/'

#Get the list of files in the batch output directory
if usedirect:
    if host == 'lxplus':
        eospath = '/eos/cms/store/group/phys_smp/ZLFV/'
    else:
        eospath = '/eos/uscms/store/user/%s/' % (user)
else:
    if host == 'lxplus':
        eospath = 'root://eoscms.cern.ch//store/group/phys_smp/ZLFV/'
    else:
        eospath = 'root://cmseos.fnal.gov//store/user/%s/' % (user)
        
    # inputpath = eospath + inputpath
if verbose:
    print "Processing input path", inputpath, "to output path", outputpath

#list_dirs = os.listdir(inputpath) #list all first files to get 1 per dataset
if host == 'lxplus':
    #FIXME: remove non-root files from search list
    command = 'eos root://eoscms.cern.ch ls /store/group/phys_smp/ZLFV/%s' % (inputpath)
else:
    command = 'eos root://cmseos.fnal.gov ls /store/user/%s/%s' % (user, inputpath)

if verbose:
    print "Running command", command
process = subprocess.Popen(command.split(), stdout=subprocess.PIPE)
stdout, stderr = process.communicate()
list_dirs = stdout.split('\n')

list_processed = []

#process each input dataset, skipping files of already processed datasets
for dirname in list_dirs:
    if verbose:
        print "Found file", dirname
    if dirname == "":
        continue
    if mergeseg:
        if '.' in dirname: continue
    elif dirname[-4:] != 'root':
        continue

    if mergeseg:
        outputname = dirname
    else:
        samplename = dirname.split("_")
        outputname = ""
        for index in range(len(samplename)-1):
            if index > 0:
                outputname = outputname + "_"
            outputname = outputname + samplename[index]

    if verbose:
        print "Using output name", outputname
    #determine if the file is data or embedding for special processing (e.g. lumi files)
    isData = "SingleElectron" in dirname or "SingleMuon" in dirname
    isEmbed = "Embed-" in dirname

    #skip datasets not being considered in the processing
    if (doDataMC < 0 and isData) or (doDataMC > 0 and not isData):
        continue
    if year != "" and year not in outputname:
        continue
    if outputname in list_processed:
        continue
    tagged = False if len(tag) > 0 else True
    for tag_i in tag:
        if tag_i in outputname:
            tagged = True
    if not tagged:
        continue
    skip_dataset = False
    for veto_tag in veto:
        if veto_tag != "" and veto_tag in outputname:
            skip_dataset = True
            break
    if skip_dataset:
        continue

    #accept the dataset to process
    list_processed.append(outputname)
    print "Processing dataset", outputname

    #get the list of ntuple files to merge
    if mergeseg:
        inputname = outputname + '/'
    else:
        if host == 'lxplus': #can't use wildcard in lxplus 'ls' command
            inputname  = outputname + "_"
        else:
            inputname  = outputname + "_*.root"
    outputname = outputname.replace("output_" ,"")
    luminame = 'lumis_'+outputname

    if not jsononly:
        outputname = outputname + ".root"
        if host == 'lxplus':
            if usedirect:
                lscommand = 'ls /eos/cms/store/group/phys_smp/ZLFV/%s' % (inputpath)
            else:
                lscommand = 'eos root://eoscms.cern.ch ls /store/group/phys_smp/ZLFV/%s' % (inputpath)
        else:
            lscommand = 'eos root://cmseos.fnal.gov ls /store/user/%s/%s%s' % (user, inputpath, inputname)
        if verbose: print "ls command:", lscommand
        process = subprocess.Popen(lscommand.split(), stdout=subprocess.PIPE)
        stdout, stderr = process.communicate()
        inputlist = stdout.split('\n')
        if host == 'lxplus': inputlist = [l for l in inputlist if inputname in l and '.root' in l]
        if mergeseg: inputlist = [inputname + l for l in inputlist]
        if usedirect:
            if host == 'lxplus':
                inputlist = ['/eos/cms/store/group/phys_smp/ZLFV/%s%s' % (inputpath, s) for s in inputlist]
            else:
                inputlist = ['/eos/uscms/store/user/%s/%s%s' % (user, inputpath, s) for s in inputlist]
        else:
            if host == 'lxplus':
                inputlist = ['root://eoscms.cern.ch//store/group/phys_smp/ZLFV/%s%s' % (inputpath, s) for s in inputlist]
            else:
                inputlist = ['root://cmseos.fnal.gov//store/user/%s/%s%s' % (user, inputpath, s) for s in inputlist]

        if copylocal:
            #write the inputs into the temp space to merge them locally
            local_list = []
            for infile in inputlist:
                local_copy = 'xrdcp -f %s %s' % (infile, tmp_dir)
                print local_copy
                if not dryrun:
                    os.system(local_copy)
                local_list.append('%s%s' % (tmp_dir, infile.split('/')[-1]))
            inputlist = local_list

        ###################################
        # Process the job segments
        if segments:
            ###################################
            # Use the job segment ntuples

            dataset_dir = '/eos/cms/store/group/phys_smp/ZLFV/' if host == 'lxplus' else '/eos/uscms/store/user/%s/' % (user)
            dataset_dir = dataset_dir + outputpath + outputname[:-5]
            if verbose:
                print "If needed, making directory %s" % (dataset_dir)
            os.system('[ ! -d %s ] && mkdir %s' % (dataset_dir, dataset_dir))
            # remove previous ntuples
            if not dryrun:
                os.system('rm %s/*' % (dataset_dir))
                for data_file in inputlist:
                    if not ".root" in data_file: continue
                    if verbose:
                        print "Adding segment file %s" % (data_file)
                    if onlylink:
                        print "Links not yet implemented!"
                    else:
                        if usedirect:
                            segment_copy = 'cp %s %s/' % (data_file, dataset_dir)
                        else:
                            redir = 'root://eoscms.cern.ch/' if host == 'lxplus' else 'root://cmseos.fnal.gov/'
                            segment_copy = 'xrdcp %s%s %s/' % (redir, data_file[data_file.index('/store'):], dataset_dir)
                        if verbose:
                            print "Segment copy: %s" % (segment_copy)    
                        os.system(segment_copy)

        else:
            ###################################
            # Merge the dataset files
            hadd_command = "time ../haddnano.py " + tmp_dir + outputname
            if inputlist == []:
                print "Not datafiles found for dataset", outputname
                if dosingle:
                    exit()
                continue
            #ensure only root files are considered, adding each to the merge command
            for data_file in inputlist:
                if ".root" in data_file:
                    hadd_command = hadd_command + " " + data_file

            print "Merging into output dataset", outputname
            if not dryrun:
                os.system(hadd_command)
            else:
                print hadd_command
                if dosingle:
                    exit()
                continue

            #remove the input files if copied over after merging
            if copylocal:
                for infile in inputlist:
                    os.remove(infile)

            # Copy back the merged data:
            copy_command = 'time xrdcp -f ' + tmp_dir + outputname
            if host == 'lxplus':
                copy_command = copy_command + ' root://eoscms.cern.ch//store/group/phys_smp/ZLFV/' + outputpath
            else:
                copy_command = copy_command + ' root://cmseos.fnal.gov//store/user/' + user +'/'  + outputpath
            print copy_command
            os.system(copy_command)
            os.remove("%s%s" % (tmp_dir, outputname))

    ######################################################
    # Process lumi files, if relevant
    if (isData or isEmbed) and not skipjson:
        if host == 'lxplus':
            lumi_command = 'python ../scripts/combine_json.py /eos/cms/store/group/phys_smp/ZLFV/' + inputpath + luminame + '_'
            lumi_command += ' --out_name /eos/cms/store/group/phys_smp/ZLFV/' + outputpath + luminame + '.txt'
        else:
            lumi_command = 'python ../scripts/combine_json.py /eos/uscms/store/user/' + user + '/' + inputpath + luminame + '_'
            lumi_command += ' --out_name /eos/uscms/store/user/' + user + '/' + outputpath + luminame + '.txt'
        print lumi_command
        os.system(lumi_command)

    if dosingle:
        break

if dryrun:
    print "Completed dryrun!"
    exit()

            
print "All done!"
