MenuGeneration
==============

L1 trigger menu generation project

To include in a CMSSW build:

    cd $CMSSW_BASE/src
    git submodule add git@github.com:kknb1056/MenuGeneration.git L1Trigger/MenuGeneration
    
Also requires UserCode/L1TriggerDPG, but I'm still trying to figure out how to add that from git.

There is some documentation in doc/menuGenerationDocumentation.doc which doxygen can make into a webpage. To do that in CMSSW you also need the Documentation/ReferenceManualScripts package in $CMSSW_BASE/src or it won't work, then just execute "scram b doc". The files will be put in $CMSSW_BASE/doc.

So in principal this should be all you need to do, but I'm having issues getting git cms-addpkg to work:

scram project CMSSW CMSSW_<some version>
cd CMSSW_<some version>/src
cmsenv
git cms-addpkg UserCode/L1TriggerDPG
git submodule add git@github.com:kknb1056/MenuGeneration.git L1Trigger/MenuGeneration
scram b

To make the documentation

git cms-addpkg Documentation/ReferenceManualScripts
scram b doc
firefox ../doc/html
