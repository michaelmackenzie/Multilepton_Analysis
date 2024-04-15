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
elif user == 'mmackenz': #use personal eos space
    output_dir = '/store/user/mimacken/histograms'
    location   = 'lpc'
else:  #use personal eos space
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
tags = [] #tag of list types (data, mc)
vetoes = []
sampleTags = [] #tag of individual datasets
sampleVetoes = ["Embed-MuMu", "Embed-EE", "MuonEGRun"]
configs = []

for s in samplesToSubmit:
    if s[:4] in doYears:
        tagged = False if len(tags) > 0 else True
        for tag in tags:
            if tag == "" or tag in s: tagged = True
        if not tagged: continue
        vetoed = False
        for veto in vetoes:
            if veto != "" and veto in s: vetoed = True
        if vetoed: continue
        sample_configs = sampleDict[s]
        samples_add = []
        for sample in sample_configs:
            tagged = False if len(sampleTags) > 0 else True
            for tag in sampleTags:
                if tag == "" or tag in sample: tagged = True
            if not tagged: continue
            vetoed = False
            for veto in sampleVetoes:
                if veto != "" and veto in sample: vetoed = True
            if vetoed: continue
            if dryRun: print "Adding file", sample
            samples_add.append(sample)
        configs += samples_add

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
