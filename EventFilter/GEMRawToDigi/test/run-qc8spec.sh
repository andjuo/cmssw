set -x
#cmsRun unpackData-QC8try.py inputFiles=file:/afs/cern.ch/user/d/dorney/public/v3Hack/run000000_Cosmic_CERNQC8_2018-11-09_chunk_321.dat streamer=True skipEvents=0 maxEvents=1 edm=True
cmsRun unpackData-QC8try.py inputFiles="file:/tmp/andriusj/run000000_Cosmic_CERNQC8_2018-11-09_chunk_321.root,file:/tmp/andriusj/tmp.root" localMode=True skipEvents=0 maxEvents=3 edm=True #dumpRaw=True
