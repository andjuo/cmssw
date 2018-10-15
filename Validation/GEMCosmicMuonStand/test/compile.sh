#!/bin/bash
#cmsenv
if [ "$1" == "local" ] ; then
    cd ../../
else
    cd ../../../
fi
pwd
echo
scram b -j 4
