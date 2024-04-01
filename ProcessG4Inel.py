#!/usr/bin/env python
##################################################
# g4hp grid job submitter
# contact Leo Aliaga (laliaga@fnal.gov) for any comment
##################################################
import os, optparse, random, shutil, tarfile, sys
import subprocess, string

CACHE_PNFS_AREA = "/pnfs/{EXPERIMENT}/scratch/users/{USER}/grid_cache/".format(EXPERIMENT = os.getenv("EXPERIMENT"),USER = os.getenv("USER"))
PWD = os.getenv("PWD")

##################################################
# Job Defaults
##################################################
NJOBS       = 2
OUTDIR       = "/pnfs/{EXPERIMENT}/persistent/users/{USER}/g4hp/".format(EXPERIMENT = os.getenv("EXPERIMENT"),USER = os.getenv("USER"))
NUMRUN       = 1
TARFILE_NAME = "local_install.tar.gz"
#####
MIN_ENERGY   = "0"   # 0 -> 20 in steps of 1 (computed based on NJOBS)
MAX_ENERGY   = "20" # 0 -> 20 in steps of 1 (computed based on NJOBS)
TARGET       = "C"
PARTICLE     = "pi+"
NEVTS        = "10000"
PHYSICS      = "FTFP_BERT"
##################################################
# geant particle gun definitions -> ppfx style names
particle_keys = {"pi+" : "pip", "pi-" : "pim", "kaon+" : "kap", "kaon-" : "kam", "proton" : "prt", "neutron" : "neu"}

def main():
  options = get_options()

  cache_folder = CACHE_PNFS_AREA + str(random.randint(10000,99999)) + "/"
  os.makedirs(cache_folder)

  os.makedirs(options.outdir)

  print("\nTarring up local area...")
  make_tarfile(TARFILE_NAME, ".")

  shutil.move(TARFILE_NAME,    cache_folder)
  shutil.copy("g4inel_job.sh", cache_folder)

  print("\nTarball of local area:", cache_folder + TARFILE_NAME)
  if int(options.njobs) <= 1:
        print("Number of jobs needs to be more than 1")
        sys.exit(1)
  stepsize = (int(options.max_energy) - int(options.min_energy)) * 0.1 / int(options.njobs)


  logfile = options.outdir + "/g4hp_p{PPARTICLE}_e{MIN_ENERGY}_e{MAX_ENERGY}_s{STEPSIZE}_t{TARGET}_e{NEVTS}_r{NUMRUN}_l{PHYSICS}_\$PROCESS.log".format(PPARTICLE=particle_keys[options.particle],
                                                                                                                        MIN_ENERGY=options.min_energy,
                                                                                                                        MAX_ENERGY=options.max_energy,
                                                                                                                        STEPSIZE=stepsize,
                                                                                                                        TARGET=options.target,
                                                                                                                        NEVTS=options.nevts,
                                                                                                                        NUMRUN=options.numrun,
                                                                                                                        PHYSICS=options.physics)
  tuplefile = "g4hp_p{PPARTICLE}_e{MIN_ENERGY}_e{MAX_ENERGY}_s{STEPSIZE}_t{TARGET}_e{NEVTS}_r{NUMRUN}_l{PHYSICS}_\$PROCESS.root".format(PPARTICLE=particle_keys[options.particle],
                                                                                                         MIN_ENERGY=options.min_energy,
                                                                                                         MAX_ENERGY=options.max_energy,
                                                                                                         STEPSIZE=stepsize,
                                                                                                         TARGET=options.target,
                                                                                                         NEVTS=options.nevts,
                                                                                                         NUMRUN=options.numrun,
                                                                                                         PHYSICS=options.physics)

  print("\nOutput logfile(s):",logfile)

  submit_command = ("jobsub_submit {GRID} {MEMORY} -N {NJOBS} -d G4HP {OUTDIR} "
      "-G uboone --expected-lifetime=48h "
      "-e PHYSICS={PHYSICS} "
      "-e PARTICLE={PARTICLE} "
      "-e PPARTICLE={PPARTICLE} "
      "-e MIN_ENERGY={MIN_ENERGY} "
      "-e MAX_ENERGY={MAX_ENERGY} "
      "-e STEPSIZE={STEPSIZE} "
      "-e TARGET={TARGET} "
      "-e NEVTS={NEVTS} "
      "-e NUMRUN={NUMRUN} "
      "-e TUPLEFILE={TUPLEFILE} "
      "-f {TARFILE} "
      "-L {LOGFILE} "
      "file://{CACHE}/g4inel_job.sh".format(
      GRID       = ("--singularity-image=/cvmfs/singularity.opensciencegrid.org/fermilab/fnal-wn-sl7:latest "
                    "--resource-provides=usage_model=DEDICATED,OPPORTUNISTIC,ONSITE "
                    "--role=Analysis "),
      MEMORY     = "--memory 1200MB ",
      NJOBS      = options.njobs,
      OUTDIR     = options.outdir,
      PARTICLE   = options.particle,
      PPARTICLE  = particle_keys[options.particle],
      MIN_ENERGY = options.min_energy,
      MAX_ENERGY = options.max_energy,
      STEPSIZE   = stepsize,
      PHYSICS    = options.physics,
      TARGET     = options.target,
      NEVTS      = options.nevts,
      NUMRUN     = options.numrun,
      TUPLEFILE  = tuplefile,
      TARFILE    = cache_folder + TARFILE_NAME,
      LOGFILE    = logfile,
      CACHE      = cache_folder)
  )

  #Ship it
  print("\nSubmitting to grid:\n"+submit_command+"\n")
  status = subprocess.call(submit_command, shell=True)

def get_options():
  parser       = optparse.OptionParser(usage="usage: %prog [options]")
  grid_group   = optparse.OptionGroup(parser, "Grid Options")

  grid_group.add_option("--outdir",
                        default = OUTDIR,
                        help    = "Output flux histograms location. Default = %default.")

  grid_group.add_option("--njobs",
                        default = NJOBS,
                        help = "Number of g4hp jobs. Default = %default.")

  physics_group   = optparse.OptionGroup(parser, "Physics Options")

  physics_group.add_option('--physics', default = PHYSICS,
                        help="Select physics list.Default = %default.")

  beam_group   = optparse.OptionGroup(parser, "Beam Options")

  beam_group.add_option('--particle', default = PARTICLE,
                        help="Select incident particle.Default = %default.")

  beam_group.add_option('--min_energy', default = MIN_ENERGY,
                        help="Select min incident energy.Default = %default.")
  beam_group.add_option('--max_energy', default = MAX_ENERGY,
                        help="Select max incident energy.Default = %default.")

  #  beam_group.add_option('--stepsize', default = STEPSIZE,
  #                        help="Select steps for scanning energies.Default = %default")

  beam_group.add_option('--nevts', default = NEVTS,
                        help="Select number of events.Default = %default.")

  target_group   = optparse.OptionGroup(parser, "Target Options")

  target_group.add_option('--target', default = TARGET,
                        help="Select target type.Default = %default.")

  run_group   = optparse.OptionGroup(parser, "Run Options")

  run_group.add_option('--numrun', default = NUMRUN,
                         help="Run number numrun.Default = %default.")


  parser.add_option_group(grid_group)
  parser.add_option_group(physics_group)
  parser.add_option_group(beam_group)
  parser.add_option_group(target_group)
  parser.add_option_group(run_group)

  options, remainder = parser.parse_args()

  return options

def make_tarfile(output_filename, source_dir):
  tar = tarfile.open(output_filename, "w:gz")
  for i in os.listdir(source_dir):
    tar.add(i)
  tar.close()

if __name__ == "__main__":
  sys.exit(main())
