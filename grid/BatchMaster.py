import sys, os, glob, subprocess, fileinput, math, datetime
from subprocess import PIPE, Popen

def get_current_time():
    now = datetime.datetime.now()
    currentTime = '{0:02d}{1:02d}{2:02d}_{3:02d}{4:02d}{5:02d}'.format(now.year, now.month, now.day, now.hour, now.minute, now.second)
    return currentTime

def make_directory(filePath, clear = True):
    if not os.path.exists(filePath):
        os.system('mkdir -p '+filePath)
    if clear and len(os.listdir(filePath)) != 0:
        os.system('rm '+filePath+'/*')

def inputFiles_from_txt(txt):
    ftxt = open(txt)
    inputFiles = ftxt.readlines()
    inputFiles = [f.strip() for f in inputFiles]
    return inputFiles


def cmdline(command):
    process = Popen(
        args=command,
        stdout=PIPE,
        shell=True
    )
    return process.communicate()[0]


class BatchMaster():
    '''A tool for submitting batch jobs'''
    def __init__(self, analyzer, config_list, stage_dir, output_dir,
                 executable='execBatch.sh', location='lpc', maxFilesPerJob = -1,
                 memory = 4800, disk = 5000000, queue = 'tomorrow'):
        self._current     = os.path.abspath('.')

        self._analyzer       = analyzer
        self._config_list    = config_list
        self._stage_dir      = stage_dir
        self._output_dir     = output_dir

        self._executable     = executable
        self._location       = location
        self._memory         = memory
        self._disk           = disk
        self._queue          = queue

    #-------------------------------------------------------------------------------------------------------------------
    def make_batch_lpc(self, cfg, suffix):
        '''
        Prepares for submission to lpc.  Does the following:

        1. Write batch configuration file
        '''

        if self._location == 'lpc':
            output_dir = 'root://cmseos.fnal.gov/' + self._output_dir
            print output_dir
        elif self._location == 'lxplus' and self._location[:4] != '/eos':
            output_dir = 'root://eoscms.cern.ch/' + self._output_dir
        else:
            output_dir = self._output_dir

        ## Writing the batch config file
        batch_tmp = open('batchJob_{0}.jdl'.format(suffix), 'w')
        batch_tmp.write('Universe              = vanilla\n')
        batch_tmp.write('Should_Transfer_Files = YES\n')
        batch_tmp.write('WhenToTransferOutput  = ON_EXIT\n')
        batch_tmp.write('Notification          = Never\n')


        if self._location in ['lpc', 'lxplus']:
            batch_tmp.write('Requirements          = OpSys == "LINUX"&& (Arch != "DUMMY" )\n')
            batch_tmp.write('request_disk          = %i\n' % (self._disk)) # 10 GB to xrdcp temp nanoAOD xrootd reading
            batch_tmp.write('request_memory        = %i\n' % (self._memory))
            if self._location == 'lxplus':
                # batch_tmp.write('+MaxRuntime           = 1440\n') #FIXME: remove if not needed
                batch_tmp.write('+JobFlavour           = \"%s\"\n' % (self._queue)) #see https://twiki.cern.ch/twiki/bin/view/ABPComputing/LxbatchHTCondor#Queue_Flavours

        batch_tmp.write('\n')

        ### set output directory
        batch_tmp.write('Arguments             = {0} {1} \n'.format(cfg, output_dir))
        batch_tmp.write('Executable            = {0}\n'.format(self._executable))
        batch_tmp.write('Transfer_Input_Files  = source.tar.gz\n')
        batch_tmp.write('Output                = reports/{0}_$(Cluster)_$(Process).stdout\n'.format(suffix))
        batch_tmp.write('Error                 = reports/{0}_$(Cluster)_$(Process).stderr\n'.format(suffix))
        batch_tmp.write('Log                   = reports/{0}_$(Cluster)_$(Process).log   \n'.format(suffix))
        batch_tmp.write('Queue\n\n')

        batch_tmp.close()
        

    def submit_to_batch(self, doSubmit=True):
        '''
        Submits batch jobs to scheduler.  Currently only works
        for condor-based batch systems.
        '''
        #  set stage dir
        print 'Running on {0}'.format(self._location)
        print 'Setting up stage directory...'
        current_time = get_current_time()
        self._stage_dir  = '{0}/{1}_{2}'.format(self._stage_dir, self._analyzer, current_time)
        make_directory(self._stage_dir, clear=False)

        # set output dir
        print 'Setting up output directory...'
        self._output_dir  = '{0}/{1}_{2}'.format(self._output_dir, self._analyzer, current_time)
        if self._location == 'lpc':
            make_directory('/eos/uscms/' + self._output_dir, clear=False)
        elif self._location == 'lxplus':
            make_directory('/eos/cms/' + self._output_dir, clear=False)

        # tar cmssw 
        print 'Creating tarball of current workspace in {0}'.format(self._stage_dir)
        if os.getenv('CMSSW_BASE') == '':
            print 'You must source the CMSSW environment you are working in...'
            exit()
        else:
            cmssw_version = os.getenv('CMSSW_BASE').split('/')[-1]
            if doSubmit:
                os.system('tar czf {0}/source.tar.gz -X $CMSSW_BASE/batch_hist_exclude.txt -C $CMSSW_BASE/.. {1}'.format(self._stage_dir, cmssw_version))

        subprocess.call('cp {0} {1}'.format(self._executable, self._stage_dir), shell=True)
        os.chdir(self._stage_dir)
        make_directory('reports', clear=False)
    

        # submit
        print 'Ready to submit to batch system {0}!'.format(self._location)
        if self._location in ['lpc', 'lxplus']:
            for cfg in self._config_list:
                print "\n\n", cfg
                suffix = cfg.replace("LFVAnalysis_", "")
                suffix = suffix.replace(".root", "")
                self.make_batch_lpc(cfg, suffix)
                if doSubmit:
                    subprocess.call('condor_submit batchJob_{0}.jdl'.format(suffix), shell=True)
