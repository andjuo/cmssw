set -x
nEvents=$1
if [ ${#nEvents} -eq 0 ] ; then nEvents=3; fi
inpFiles=$2
if [ ${#inpFiles} -eq 0 ] ; then inpFiles="file:run000000_Cosmic_CERNQC8_2018-11-09_chunk_321.root"; fi
runNum=$3
if [ ${#runNum} -eq 0 ] ; then runNum=1; fi
cmsRun unpackData-QC8tryDebug.py inputFiles=${inpFiles} localMode=True maxEvents=${nEvents} edm=True runNum=${runNum} #dumpRaw=True

cp gem_EDM-qc8specDebug.root gem_EDM-qc8specDebug-runNum${runNum}.root
