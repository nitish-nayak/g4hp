#!/usr/bin/env python
##################################################
# g4hp grid job submitter
# Made to work with macros/HP_template.mac.
# June 25, 2020 (Leo)
##################################################
import os, optparse, random, shutil, tarfile, sys
import subprocess, string

CACHE_PNFS_AREA = "/pnfs/{EXPERIMENT}/scratch/users/{USER}/grid_cache/".format(EXPERIMENT = os.getenv("EXPERIMENT"),
                                                                               USER = os.getenv("USER"))
PWD = os.getenv("PWD")

##################################################
# Job Defaults
NPOTS           = 100
N_JOBS          = 1
RUN             = 1
OUTDIR          = "/pnfs/{EXPERIMENT}/persistent/users/{USER}/g4hp/test/".format(EXPERIMENT = os.getenv("EXPERIMENT"),
                                                                                       USER = os.getenv("USER"))
TEMPLATE        = "{0}/macros/hp_template.mac".format(PWD)
FILETAG         = ""
TARFILE_NAME    = "local_install.tar.gz"

##################################################
# g4hp Defaults
INCENERGY       = 158

##################################################
def main():
  options = get_options()
  
  g4_macro = make_macro(options)

  # scratch /pnfs area from which to send tarfile to grid
  cache_folder = CACHE_PNFS_AREA + str(random.randint(10000,99999)) + "/"
  os.mkdir(cache_folder)

  print "\nTarring up local area..."
  make_tarfile(TARFILE_NAME, ".")

  shutil.move(TARFILE_NAME,    cache_folder) # temp file -> remove it from pwd
  shutil.move(g4_macro,        cache_folder) # temp file -> remove it form pwd
  shutil.copy("g4hp_job.sh",   cache_folder)

  print "\nTarball of local area:", cache_folder + TARFILE_NAME

  logfile = options.outdir + "/g4hp_pC_e{INCENERGY}_{RUN}_\$PROCESS.log".format(INCENERGY = options.inc_energy,
                                                                               RUN        = options.run_number)
#  outname = "g4hp_pC_e{INCENERGY}_{RUN}_\$PROCESS.root".format(INCENERGY = options.incenergy,
#                                                                               RUN        = options.run_number)
  print "\nOutput logfile(s):",logfile

  submit_command = ("jobsub_submit {GRID} {MEMORY} -N {NJOBS} -d G4HP {OUTDIR} "
      "-G {EXPERIMENT} "
      "-e INCENERGY={INCENERGY} "
      "-e NPOTS={NPOTS} "      
      "-e RUN={RUN} "
      "-f {TARFILE} "
      "-L {LOGFILE} "
      "file://{CACHE}/g4hp_job.sh".format(
      GRID       = ("--OS=SL6 -g "
                    "--resource-provides=usage_model=DEDICATED,OPPORTUNISTIC "
                    "--role=Analysis "),
      MEMORY     = "--memory 200MB ",
      NJOBS      = options.n_jobs,
      OUTDIR     = options.outdir,
      EXPERIMENT = os.getenv("EXPERIMENT"),
      INCENERGY  = options.inc_energy,
      NPOTS      = options.npots,
      RUN        = options.run_number,
      TARFILE    = cache_folder + TARFILE_NAME,
      LOGFILE    = logfile,
      CACHE      = cache_folder)
  )

  #Ship it
  print "\nSubmitting to grid:\n"+submit_command+"\n"
  status = subprocess.call(submit_command, shell=True)

def get_options():
  parser       = optparse.OptionParser(usage="usage: %prog [options]")
  grid_group   = optparse.OptionGroup(parser, "Grid Options")

  grid_group.add_option("--outdir",
                default = OUTDIR,
                help    = "Output flux histograms location. Default = %default.")

  grid_group.add_option("--template",
                default = TEMPLATE,
                help    = "Input macro template. Default = %default.")
   
  grid_group.add_option("--n_jobs",
        default = N_JOBS,
        help = "Number of g4numi jobs. Default = %default.")

  grid_group.add_option("--run_number",
        default = RUN,
        help = "Tag on the end of outfiles. Doubles as random # seed. Default = %default.")

  grid_group.add_option('--npots',
        default = NPOTS,
        help="Number of protons on target to simulate. Default = %default.")

  g4hp_group    = optparse.OptionGroup(parser, "Specific G4HP Options")
  
  g4hp_group.add_option('--inc_energy', default = INCENERGY,
       help="Eneggy of the particel projectile. Default = %default." )

  parser.add_option_group(grid_group)
  parser.add_option_group(g4hp_group)

  options, remainder = parser.parse_args()
  
  return options

def make_macro(options):
  template_filename = options.template
  template_string   = open(template_filename, 'r').read()
  template          = string.Template(template_string)

#  macro_string = template.safe_substitute(
#    {'INCENERGY':       options.inc_energy,
#     'OUTNAMEFILE':     outname,
#     'NEVTS':           options.nevts
#     }
#    )
  
  macro_name = "g4hp.mac"
  macro = open(macro_name, "w") 
#  macro.write(macro_string)
  macro.close()

  return macro_name

def make_tarfile(output_filename, source_dir):
  tar = tarfile.open(output_filename, "w:gz")
  for i in os.listdir(source_dir):
    tar.add(i)
  tar.close()

if __name__ == "__main__":
  sys.exit(main())
