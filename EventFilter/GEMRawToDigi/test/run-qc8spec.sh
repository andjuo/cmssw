set -x
##cmsRun unpackData-QC8try.py inputFiles=file:/eos/cms/store/group/dpg_gem/comm_gem/data_p5/run305026/run305026_ls0001_streamA_StorageManager.dat streamer=True skipEvents=0 maxEvents=1 edm=True
#cmsRun unpackData-QC8try.py inputFiles=file:/afs/cern.ch/user/d/dorney/public/v3Hack/run000000_Cosmic_CERNQC8_2018-11-09_chunk_321.dat streamer=True skipEvents=0 maxEvents=1 edm=True
cmsRun unpackData-QC8try.py inputFiles=file:/tmp/andriusj/run325180_ls0102_streamA_StorageManager.dat localMode=True skipEvents=0 maxEvents=3 edm=True #dumpRaw=True
