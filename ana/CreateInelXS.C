#include <iostream>
#include <fstream>
#include "TTree.h"
#include "TChain.h"
#include "TBranch.h"
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"

#include <string>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <iomanip>

#include "hptuple.hh"

using namespace std;

const Int_t KE_bins = 16001;
const Double_t low_KE = -0.005;
const Double_t high_KE = 160.005;

const double NA=6.022e23; // atoms/mol
const double dx=0.7;//cm target thickness
const double cm2_per_mb=1e-27; // conversion factor

void CreateInelXS(const char* infiles, const char* out_histfile, const char* out_QEfile){

  TFile* foutput = new TFile(out_histfile,"RECREATE");

  std::ofstream qeinfo;
  qeinfo.open(out_QEfile);

  TChain* evts = new TChain("hAinfoTree");
  TChain* setup = new TChain("setup");
  HPTuple*  hAinfo  = new HPTuple;
  double inc_energy;

  std::ifstream ifs;
  ifs.open(infiles);
  std::string line;
  while (ifs.good()) {
    getline(ifs,line);
    if(line.find(".root")>10000)continue;
    evts->Add(line.c_str());
    setup->Add(line.c_str());
    std::cout<<"Adding ntuple at : "<<line<<std::endl;
  }
  ifs.close();

  evts->SetBranchAddress("hAinfo",&hAinfo);
  evts->SetBranchAddress("inc_energy", &inc_energy);
  std::cout<< (evts->GetNtrees()) <<" files were added to the chain"<<std::endl;

  qeinfo<<"Processing "<< (evts->GetNtrees()) <<" trees in the chain"<<std::endl;
  qeinfo<<"#Nentries Entries el_like qe_like frag_like prod_entries"<<std::endl;

  double energy, tgt_A, density;
  int nincident;
  setup->SetBranchAddress("energy", &energy);
  setup->SetBranchAddress("A", &tgt_A);
  setup->SetBranchAddress("density", &density);
  setup->SetBranchAddress("nof_events", &nincident);

  //Counting quasielastics:
  Int_t  prod_tot      = 0;
  Int_t  qe_tot        = 0;
  Int_t  frag_tot      = 0;
  Int_t  el_tot        = 0;

  foutput->cd(0);

  TH1D* hnorm = new TH1D("xs_norm", ";Kinematic Energy (GeV); Normalization", KE_bins, low_KE, high_KE);
  std::cout << "Looping over setup tree first.." << std::endl;
  for(long int jentry=0; jentry<(int)setup->GetEntries(); jentry++) {
    int nb = setup->GetEntry(jentry);
    double sigma_factor=1.0/(density/tgt_A * NA * dx * cm2_per_mb); // mb
    hnorm->Fill(energy, (double)nincident/sigma_factor);
  }

  TH1D* htot_xs = new TH1D("tot_xs", ";Kinematic Energy (GeV); Total Cross-Section (mb)", KE_bins, low_KE, high_KE);
  TH1D* hel_xs = new TH1D("el_xs", ";Kinematic Energy (GeV); Elastic Cross-Section (mb)", KE_bins, low_KE, high_KE);
  TH1D* hqe_xs = new TH1D("qe_xs", ";Kinematic Energy (GeV); Quasi-Elastic Cross-Section (mb)", KE_bins, low_KE, high_KE);
  TH1D* hfrag_xs = new TH1D("frag_xs", ";Kinematic Energy (GeV); Fragmentation Cross-Section (mb)", KE_bins, low_KE, high_KE);
  TH1D* hprod_xs = new TH1D("prod_xs", ";Kinematic Energy (GeV); Production Cross-Section (mb)", KE_bins, low_KE, high_KE);

  //GetEntries and Loop:
  int TEntries = 0;
  int nentries  = (int)evts->GetEntries();
  std::cout<<"Entries "<<nentries<<std::endl;

  for(long int jentry=0;jentry<nentries;jentry++) {
    TEntries++;
    Int_t countpiK      = 0;
    Int_t countNucleons = 0;
    Int_t countFragments= 0;

    if(jentry%100000==0)std::cout<<"Entry "<<jentry/1000<<" k"<<std::endl;
    int nb = evts->GetEntry(jentry);

    int npart = int(hAinfo->prodpart.size());

    //looking for new particles and nucleons:
    for(int ipart=0;ipart<npart;ipart++){
      int pdg = hAinfo->prodpart[ipart].pdg;
      if(abs(pdg)==211 || abs(pdg)==321)countpiK++;
      if(pdg==111)countpiK++;
      if(pdg==130 || pdg==310 )countpiK++;
      if(pdg==2212||pdg==2112)countNucleons++;
      if(pdg>1000000000) countFragments++;
    }

    //we look for characterize the event:
    bool qe_event=false;
    // sometimes QEL-like events have a nuclear fragment, like C11 in the Final State
    // sometimes not
    if(countpiK == 0 && countNucleons==2 && countFragments<=1){ qe_tot++; qe_event=true;}

    bool frag_event=false;
    // events with multiple nuclear fragments in the FS
    // or events with more than 2 nucleons and one or more fragments
    if((countpiK ==0 && countFragments>1 && countNucleons>0)
      || (countpiK ==0 && countFragments>0 && countNucleons>2)){
      frag_tot++;
      frag_event=true;
    }

    bool el_event=false;
    if(countpiK==0 && countFragments==1 && countNucleons==1){el_tot++; el_event=true;}

    //production count:
    bool prod_event=false;
    if(countpiK>0){ prod_tot++; prod_event=true;}

    // Fill yields first
    htot_xs->Fill(inc_energy/1000.);
    if(el_event)   { hel_xs->Fill(inc_energy/1000.); }
    if(qe_event)   { hqe_xs->Fill(inc_energy/1000.); }
    if(frag_event) { hfrag_xs->Fill(inc_energy/1000.); }
    if(prod_event) { hprod_xs->Fill(inc_energy/1000.); }
  }

  // root takes care of 0 in the denominator for me
  htot_xs->Divide(hnorm);
  hel_xs->Divide(hnorm);
  hqe_xs->Divide(hnorm);
  hfrag_xs->Divide(hnorm);
  hprod_xs->Divide(hnorm);

  qeinfo<<nentries<<"    "<<TEntries<<"    "<<el_tot<<"     "<<qe_tot<<"    "<<frag_tot<<"     "<<prod_tot<<std::endl;
  qeinfo.close();
  foutput->Write();
  foutput->Close();
  std::cout<<"===>>>Running end"<<std::endl;
}

# ifndef __CINT__
int main(int argc, const char* argv[]){
  CreateInelXS(argv[1],argv[2],argv[3]);
  return 0;
}
# endif
