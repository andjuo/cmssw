set -x
cmsRun unpackData-GEM.py inputFiles=file:/eos/cms/store/group/dpg_gem/comm_gem/data_p5/run305026/run305026_ls0001_streamA_StorageManager.dat streamer=True skipEvents=0 maxEvents=1 edm=True
#cmsRun unpackData-GEM2.py inputFiles=file:gem_EDM-qc8spec-runNum111.root streamer=False skipEvents=0 maxEvents=100 edm=True process=RECO2
