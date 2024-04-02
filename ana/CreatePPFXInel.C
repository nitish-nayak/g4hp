#include <vector>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include "TChain.h"
#include "TH1.h"
#include "TH2.h"
#include "TChainElement.h"
#include "TFile.h"
#include "TObjArray.h"
#include "TSystem.h"

using namespace std;

const Int_t Npart = 6; //4; //8; //6
const char* part[Npart]  = {"pip","pim","kap","kam","prt","neu"};

TH1D* getvhist(TFile* fIn);

void CreatePPFXInel(const char* xsfiles, const char* target){

  TObjArray piplist(0);
  TObjArray pimlist(0);
  TObjArray kaplist(0);
  TObjArray kamlist(0);
  TObjArray prtlist(0);
  TObjArray neulist(0);

  cout<<"Checking the invariant cross section files..."<<endl;
  std::ifstream ifs;
  ifs.open(xsfiles);
  std::string line;
  std::vector<std::string> filenames; // expect same order as part[Npart] but can provide dummy files
  while (ifs.good()) {
    getline(ifs,line);
    if(line.find(".root")>10000)continue;
    filenames.push_back(line);
    std::cout<<"Adding file : "<<line<<std::endl;
  }
  ifs.close();

  for(int jj=0;jj<Npart;jj++){

    std::string xsfile = filenames[jj];

    // just root being root. this returns true if it doesn't exist
    if(gSystem->AccessPathName(xsfile.c_str(), kFileExists)) {
      std::cout  << "File does not exist" << std::endl;
      continue;
    }

    TFile* f = new TFile(xsfile.c_str(),"READ");
    TH1D* htemp = getvhist(f);

    if(!htemp) continue;
    htemp->SetName(TString::Format("h%s_%s",part[jj], target));
    htemp->GetYaxis()->SetTitle("Absorption Cross-Section (mb)");

    if(jj==0)piplist.Add(htemp);
    if(jj==1)pimlist.Add(htemp);
    if(jj==2)kaplist.Add(htemp);
    if(jj==3)kamlist.Add(htemp);
    if(jj==4)prtlist.Add(htemp);
    if(jj==5)neulist.Add(htemp);

  }

  TFile* inel_xs = new TFile("PPFX/InelXS_geant4.root", "UPDATE");
  if(!inel_xs->GetDirectory("pip"))
    inel_xs->mkdir("pip");
  inel_xs->cd("pip");
  if(piplist.GetEntries()) piplist.Write();
  inel_xs->cd();

  if(!inel_xs->GetDirectory("pim"))
    inel_xs->mkdir("pim");
  inel_xs->cd("pim");
  if(pimlist.GetEntries()) pimlist.Write();
  inel_xs->cd();

  if(!inel_xs->GetDirectory("kap"))
    inel_xs->mkdir("kap");
  inel_xs->cd("kap");
  if(kaplist.GetEntries()) kaplist.Write();
  inel_xs->cd();

  if(!inel_xs->GetDirectory("kam"))
    inel_xs->mkdir("kam");
  inel_xs->cd("kam");
  if(kamlist.GetEntries()) kamlist.Write();
  inel_xs->cd();

  if(!inel_xs->GetDirectory("prt"))
    inel_xs->mkdir("prt");
  inel_xs->cd("prt");
  if(prtlist.GetEntries()) prtlist.Write();
  inel_xs->cd();

  if(!inel_xs->GetDirectory("neu"))
    inel_xs->mkdir("neu");
  inel_xs->cd("neu");
  if(neulist.GetEntries()) neulist.Write();
  inel_xs->cd();

  inel_xs->Close();
  std::cout<<"Just made a new PPFX directory. Check it out!"<<std::endl;
}

TH1D* getvhist(TFile* fIn){
  //get histogram from file
  TH1D* vel;
  TH1D* vinel;
  vinel = (TH1D*)fIn->Get("tot_xs");
  vel = (TH1D*)fIn->Get("el_xs");
  vinel->Add(vel, -1.);

  return vinel;
}

int main(int argc, const char* argv[]){
  CreatePPFXInel(argv[1], argv[2]);
  return 0;
}
