#include "HPConfig.hh"
#include "HPTargets.hh"
#include <iostream>

HPConfig :: HPConfig (int argc, char **argv) : 
  physicsList("QGSP_BERT"), nEvents("100000"), runNumber("1"), isConfirmed(false)
{
  if (argc == 1) usage();
  
  char option;

  while ((option = getopt (argc, argv, "t:a:z:d:r:p:e:l:n:x:f:k:yh")) != -1)
    switch (option)
    {
      case 't': setTarget(optarg); break;
      case 'a': target.A = atof(optarg); break;
      case 'z': target.Z = atoi(optarg); break;
      case 'd': target.density = atof(optarg); break;
      case 'r': target.radius = atof(optarg); break;
      case 'p': beam.particle = optarg; break;
      case 'e': beam.energy = optarg; break;
      case 'l': physicsList = optarg; break;
      case 'n': nEvents = optarg; break;
      case 'x': runNumber = optarg; break;
      case 'f': outputFile = optarg; break;
      case 'k': outputDir = optarg; break;
      case 'y': isConfirmed = true; break;
      case 'h': usage();
      case '?': if (optopt == 'c' or isprint (optopt)) usage();
      default: usage();
    }

  //use default output file name, if not defined by user
  if (!outputFile.length()) outputFile = beam.particle + "_" +
                    beam.energy + "_" + target.name + "_" + physicsList + "_" + runNumber + ".root";

  //if output dir not defined by user, use $G4HP_ROOTDIR
  //or default one if $G4HP_ROOTDIR is not defined
  if (!outputDir.length())
  {
     if (getenv("G4HP_ROOTDIR")) outputDir = getenv("G4HP_ROOTDIR");
     else outputDir = "g4hp_root_files";
  }

  //show summary of simulation setup (unless -y was used)
  if (!isConfirmed) checkConf();

  //created folder for output
  FILE *fp = popen(("mkdir -p " + outputDir).c_str(), "w"); pclose (fp);
}

void HPConfig :: setTarget (const G4String &targetName)
{
  map <G4String, Target> :: iterator it = HPTargets.find(targetName);

  if (it != HPTargets.end()) target = HPTargets[targetName];
  else target.name = targetName;
}

void HPConfig :: usage () const
{
  std::cout << "\nUsage:\n";
  std::cout << "\t -t [target name], e.g. C, Al\n";
  std::cout << "\t -a [mass number]\n";
  std::cout << "\t -z [atomic number]\n";
  std::cout << "\t -d [density] in gm/cm3\n";
  std::cout << "\t -r [radius] in cm, optional (default = 0.3cm)]\n";
  std::cout << "\n \t Note: you do not need to define 'a', 'z' and 'd' \n"
            << "\t if 't' is on the list of predfined targets "
            << "(include/HPTargets.hh)\n\n";
  std::cout << "\t -p [beam particle], e.g. proton\n";
  std::cout << "\t -e [beam energy], in GeV\n";
  std::cout << "\t -l [physics list], optional (default = QGSP_BERT) \n";
  std::cout << "\n\t -n [number of events], optional (default = 100k)\n";
  std::cout << "\n\t -x [run number], optional (default = 1)\n";
  std::cout << "\n\t -f [output ROOT file name], optional "
            << "(default = particle_energy_target_runnumber.root)\n";
  std::cout << "\n\t -k [output directory], optional "
            << "(default = $G4HP_ROOTDIR or pwd/g4hp_root_files)\n";
  std::cout << "\n\t -y to skip checking configuration\n\n";

  exit(1);
}

void HPConfig :: checkConf () const
{
  target.print (); beam.print ();
  std::cout << "\nPhysics List = " << physicsList << "\n";
  std::cout << "\nNumber of events = " << nEvents << "\n";
  std::cout << "\nRun number = " << runNumber << "\n";
  std::cout << "\nOutput file = " << outputDir << "/" << outputFile << "\n\n";
  std::cout << "Do you want to proceed ('y' or 'n')? "
            << "[Note: you can avoid this step using '-y' option].\t";

  char proceed;
  do std::cin >> proceed; while (proceed != 'y' and proceed != 'n');

  if (proceed == 'n') exit(2);
}

TTree* HPConfig :: createTree () const
{
  TTree *tree = new TTree ("setup", "Simulation setup");
  tree->Branch ("particle", new std::string(beam.particle));
  tree->Branch ("energy", new double(atof(beam.energy))); 
  tree->Branch ("target", new std::string(target.name));
  tree->Branch ("A", new double(target.A));
  tree->Branch ("Z", new int(target.Z));
  tree->Branch ("density", new double(target.density));
  tree->Branch ("radius", new double(target.radius));
  tree->Branch ("physics", new std::string(physicsList));
  tree->Branch ("nof_events", new int(atoi(nEvents.c_str())));
  tree->Fill();
  return tree;
}

void Target :: print () const
{
  std::cout << "\nTarget = " << name << "\n";
  std::cout << "A = " << A << "\n";
  std::cout << "Z = " << Z << "\n";
  std::cout << "Density = " << density << "\n";
  std::cout << "Radius = " << radius << "\n";
}

void Beam :: print () const
{
  std::cout << "\nIncident particle = " << particle << "\n";
  std::cout << "Energy = " << energy << "\n";
}
