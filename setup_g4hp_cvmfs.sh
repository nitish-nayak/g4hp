
source /cvmfs/fermilab.opensciencegrid.org/products/common/etc/setups
setup ifdhc

# For lcg-cp, which is not installed on all osg machines
#source /cvmfs/oasis.opensciencegrid.org/mis/osg-wn-client/3.2/current/el6-x86_64/setup.sh


if [ -z "${G4LBNE_IS_SETUP}" ]; then
    
    case ${HOSTNAME} in
        argoneut*)
            export GROUP=argoneut
            export EXPERIMENT=argoneut
            ;;
        uboone*)
            export GROUP=microboone
            export EXPERIMENT=uboone
            ;;
        lbne*)
            export GROUP=dune
            export EXPERIMENT=dune
            ;;
        dune*)
            export GROUP=dune
            export EXPERIMENT=dune
            ;;
    esac
    echo experiment is $GROUP

    
    export LBNE_PRODCUT_DIR=/cvmfs/larsoft.opensciencegrid.org/products/
    source /cvmfs/larsoft.opensciencegrid.org/products/setup
    setup cmake v3_14_3
#g4 v4_10_3_p03b clhep depend same as root version setup by dk2nu
    setup geant4 v4_10_3_p03b -q e15:prof
    setup fftw v3_3_6_pl2 -q prof
    setup python v2_7_14
    setup dk2nu v01_05_01b -q e15:prof
    unset G4BIN

##!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
##
## Set G4WORKDIR to g4hp directory, if not already set
##
##!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#    SETUPDIR=$(cd `dirname "${BASH_SOURCE[0]}"` && pwd)/`basename "${BASH_SOURCE[0]}"`
    SETUPDIR=$(cd `dirname "${BASH_SOURCE[0]}"` && pwd)
    echo "SETUPDIR is $SETUPDIR"
    #if [ -z "$G4WORKDIR" ]; then
#	export G4WORKDIR=`dirname $SETUPDIR`
#	echo "G4WORKDIR is not set... Setting it to "$G4WORKDIR 
#    fi
#
    export G4WORKDIR=`dirname ${SETUPDIR}`
    export G4LBNEWORKDIR=`dirname ${SETUPDIR}`
    export G4LBNE_DIR=`dirname $SETUPDIR`

    echo "G4WORKDIR: $G4WORKDIR"
    echo "G4LBNEWORKDIR: $G4LBNEWORKDIR"
    echo "G4LBNE_DIR: $G4LBNE_DIR"
    
    export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:$G4LIB/Linux-g++"

#    echo "*********************"
#    echo Locating ROOT...
#    env | grep root
#    echo "*********************"
#    echo Locating Geant4...
#    env | grep geant4
#    echo "*********************"
#    echo Locating CLHEP...
#    env | grep clhep
#    echo "*********************"
    
    export G4WORKDIR=`pwd`
    
    echo G4WORKDIR is ${G4WORKDIR}
    export LD_LIBRARY_PATH=${G4LBNE_DIR}:${LD_LIBRARY_PATH}
    export LD_LIBRARY_PATH=${G4LBNE_DIR}/shared_libraries:${LD_LIBRARY_PATH}
    export LD_LIBRARY_PATH=${G4WORKDIR}/build:${G4WORKDIR}/ana:${LD_LIBRARY_PATH}

    echo LD_LIBRARY_PATH is ${LD_LIBRARY_PATH} 


    
    #
    # Set a flag to suppress unnecessary re-executions of this script.
    #    
    #export G4LBNE_IS_SETUP=1

# Setup grid submit tools if not on a grid node
if [ -z "${_CONDOR_SCRATCH_DIR}" ]; then
    echo "_CONDOR_SCRATCH_DIR is not set... so I'm assuming we're not running on a grid node.... Setting up jobsub tools."
#    setup jobsub_tools
    setup jobsub_client
fi

fi
