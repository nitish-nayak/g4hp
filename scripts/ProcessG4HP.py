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
NJOBS           = 1
RUNID           = 1
OUTDIR          = "/pnfs/{EXPERIMENT}/persistent/users/{USER}/g4hp/test/".format(EXPERIMENT = os.getenv("EXPERIMENT"),
                                                                                       USER = os.getenv("USER"))
TEMPLATE        = "{0}/macros/hp_template.mac".format(PWD)
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
  os.makedirs(options.outdir)
  
  print "\nTarring up local area..."
  make_tarfile(TARFILE_NAME, ".")

  shutil.move(TARFILE_NAME,    cache_folder) # temp file -> remove it from pwd
  shutil.move(g4_macro,        cache_folder) # temp file -> remove it form pwd
  shutil.copy("g4hp_job.sh",   cache_folder)

  print "\nTarball of local area:", cache_folder + TARFILE_NAME

  logfile = options.outdir + "/g4hp_pC_e{INCENERGY}_r{RUN}_j\$PROCESS.log".format(INCENERGY = options.incenergy,
                                                                               RUN        = options.runid)
  outfile = "g4hp_pC_e{INCENERGY}_r{RUN}_j\$PROCESS.root".format(INCENERGY = options.incenergy,
                                                                 RUN       = options.runid)
  print "\nOutput logfile(s):",logfile
  print "\nOutput outfile(s):",outfile

  submit_command = ("jobsub_submit {GRID} {MEMORY} -N {NJOBS} -d G4HP {OUTDIR} "
      "-G {EXPERIMENT} "
      "-e RUNID={RUNID} "
      "-e OUTFILE={OUTFILE} "
      "-f {TARFILE} "
      "-L {LOGFILE} "
      "file://{CACHE}/g4hp_job.sh".format(
      GRID       = ("--OS=SL6 -g "
                    "--resource-provides=usage_model=DEDICATED,OPPORTUNISTIC "
                    "--role=Analysis "),
      MEMORY     = "--memory 200MB ",
      NJOBS      = options.njobs,
      OUTDIR     = options.outdir,
      EXPERIMENT = os.getenv("EXPERIMENT"),
      RUNID      = options.runid,
      OUTFILE    = outfile,
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
                help    = "Output flux histograms location. Default = %default")

  grid_group.add_option("--template",
                default = TEMPLATE,
                help    = "Input macro template. Default = %default")
   
  grid_group.add_option("--njobs",
        default = NJOBS,
        help = "Number of g4hp jobs. Default = %default")

  grid_group.add_option("--runid",
        default = RUNID,
        help = "Run ID. Default = %default")

  grid_group.add_option('--npots',
        default = NPOTS,
        help="Number of protons on target to simulate. Default = %default")

  g4hp_group    = optparse.OptionGroup(parser, "Specific G4HP Options")
  
  g4hp_group.add_option('--incenergy', default = INCENERGY,
       help="Energy of the particle projectile. Default = %default" )

  parser.add_option_group(grid_group)
  parser.add_option_group(g4hp_group)

  options, remainder = parser.parse_args()
  
  return options

def make_macro(options):
  template_filename = options.template
  template_string   = open(template_filename, 'r').read()
  template          = string.Template(template_string)
  
  macro_string = template.safe_substitute(
    {'INCENERGY':       options.incenergy,
     'NPOTS':           options.npots
     }
    )
  
  macro_name = "g4hp.mac"
  macro = open(macro_name, "w") 
  macro.write(macro_string)
  macro.close()

  return macro_name

def make_tarfile(output_filename, source_dir):
  tar = tarfile.open(output_filename, "w:gz")
  for i in os.listdir(source_dir):
    tar.add(i)
  tar.close()

if __name__ == "__main__":
  sys.exit(main())
