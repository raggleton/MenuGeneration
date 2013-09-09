MenuGeneration
==============

L1 trigger menu generation project

To include in a CMSSW build (assuming you have initialised it for git somehow):

    cd $CMSSW_BASE/src
    git submodule add git@github.com:kknb1056/MenuGeneration.git L1Trigger/MenuGeneration
    
This will put the package in L1Trigger/MenuGeneration. Also requires UserCode/L1TriggerDPG, but as far as I can tell that hasn't been transferred over to git. So you need to use a version from CVS:

    cvs co UserCode/L1TriggerDPG

There is some documentation in doc/menuGenerationDocumentation.doc which doxygen can make into a webpage. To do that in CMSSW you also need the Documentation/ReferenceManualScripts package in $CMSSW_BASE/src or it won't work, then just execute "scram b doc". The files will be put in $CMSSW_BASE/doc.

*Instructions from scratch*
I'm having issues getting "git cms-addpkg" to work, so full instructions from scratch using cvs where necessary instead:

    scram project CMSSW CMSSW_6_0_1
    cd CMSSW_6_0_1/src
    cmsenv
    git init
    git submodule add git@github.com:kknb1056/MenuGeneration.git L1Trigger/MenuGeneration
    cvs co UserCode/L1TriggerDPG
    cvs co Documentation/ReferenceManualScripts
    scram b -j 6
    scram b doc
    
This will build the package and create the documentation. Note that L1Trigger/MenuGeneration is not particularly dependant on the CMSSW version. The version of UserCode/L1TriggerDPG is more important. To view the documentation:

    firefox $CMSSW_BASE/doc/html
