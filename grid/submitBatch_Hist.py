#! /usr/bin/env python
import BatchMaster as bm
from file_dictionary import add_samples

import os, sys

dryRun = False

# -----------------------------
# Specify parameters
# -----------------------------

user = os.getenv('USER')
host = os.getenv('HOSTNAME')

print 'Using user %s on host %s' % (user, host)

executable = 'execBatch.sh'
analyzer   = 'CLFVHistMaker'
stage_dir  = 'batch'
if 'lxplus' in host: #use SMP space
    output_dir = '/store/group/phys_smp/ZLFV/histograms'
    location   = 'lxplus'
elif user == 'mmackenz':
    output_dir = '/store/user/mimacken/histograms'
    location   = 'lpc'
else:
    output_dir = '/store/user/%s/histograms' % (user)
    location   = 'lpc'

print 'location = %s, output_dir = %s' % (location, output_dir)


# -----------------------------
# Set job configurations.  
# -----------------------------
sampleDict = {}
add_samples(sampleDict)


# -----------------------------
# submit to batch
# -----------------------------

samplesToSubmit = sampleDict.keys()

samplesToSubmit.sort()
# doYears = ["2016"]
doYears = ["2016", "2017", "2018"]
sampleTag = ""
sampleVeto = ""
configs = []

for s in samplesToSubmit:
    if s[:4] in doYears and (sampleTag == "" or sampleTag in s) and (sampleVeto == "" or sampleVeto not in s):
        configs += sampleDict[s]

if configs == []:
    print "No datasets to submit!"
    exit()

batchMaster = bm.BatchMaster(
    analyzer       = analyzer,
    config_list    = configs,
    stage_dir      = stage_dir,
    output_dir     = output_dir,
    executable     = executable,
    location       = location
)

#ensure there's a symbolic link 'batch' to put the tarball in
if not os.path.exists("batch") :
    if 'lxplus' in host:
        if not os.path.exists("~/private/batch") :
            os.makedirs("~/private/batch")
        os.symlink("~/private/batch", "batch")
    else: #LPC
        if not os.path.exists("~/nobackup/batch") :
            os.makedirs("~/nobackup/batch")
        os.symlink("~/nobackup/batch", "batch")
    print "Created symbolic link to ~/nobackup/batch"

if not os.path.isfile('%s/batch_hist_exclude.txt' % (os.getenv('CMSSW_BASE'))):
    os.system('cp batch_hist_exclude.txt ${CMSSW_BASE}/')

batchMaster.submit_to_batch(doSubmit=(not dryRun))
