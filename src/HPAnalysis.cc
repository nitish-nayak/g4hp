
#include <vector>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <stdlib.h>

//Root 
#include <TSystem.h>        
#include <TStopwatch.h>    
#include <TFile.h>          
#include <TTree.h>

//GEANT4 
#include "globals.hh"
#include "G4ios.hh"
#include "G4Track.hh"
#include "G4SteppingManager.hh"
#include "G4ThreeVector.hh"
#include "G4TrajectoryContainer.hh"
#include "G4RunManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4Navigator.hh"
#include "G4TransportationManager.hh"
#include "G4Run.hh"
#include "G4Proton.hh"

//g4hp
#include "HPAnalysis.hh"

using namespace std;

HPAnalysis* HPAnalysis::instance = 0;

//------------------------------------------------------------------------------------
HPAnalysis::HPAnalysis()
{
#ifdef G4ANALYSIS_USE
#endif
 }
//------------------------------------------------------------------------------------
HPAnalysis::~HPAnalysis()
{ 


#ifdef G4ANALYSIS_USE
#endif
}
//------------------------------------------------------------------------------------
HPAnalysis* HPAnalysis::getInstance()
{
  if (instance == 0) instance = new HPAnalysis;
  return instance;
}
//------------------------------------------------------------------------------------
void HPAnalysis::book(G4long id0,G4long id1)
{
  sprintf(NtupleFileName,"p_%dGeV_C_%04d.root",int(id0),int(id1));

  //Uncomment, omplete the path for tuples and recompile: 
  //sprintf(NtupleFileName,"/minerva/data/users/laliaga/HadProd/.../Tuples/p_%dGeV_C_%04d.root",id0,id1);

  G4cout<<"==>>Using "<<id0<<" & "<<id1<<" as random generator numbers"<<G4endl; 

  FileNtuple = new TFile(NtupleFileName, "RECREATE","hadron from p+C ntuple");   
  ProdTree = new TTree("pCinfo","g4HP info from p+C");
  
  ProdTree->Branch("npart",&g4Proddata.NPart,"NPart/I");
  ProdTree->Branch("pdg", &g4Proddata.PDG,"PDG[NPart]/I");
  ProdTree->Branch("x",  &g4Proddata.X,"X[NPart][3]/D");
  ProdTree->Branch("p",  &g4Proddata.P,"P[NPart][4]/D");
  ProdTree->Branch("xf", &g4Proddata.XF,"XF[NPart]/D");
  ProdTree->Branch("pt", &g4Proddata.PT,"PT[NPart]/D");
  ProdTree->Branch("ff", &g4Proddata.FF,"FF[NPart]/O");
}


//------------------------------------------------------------------------------------
void HPAnalysis::finish()
{
    FileNtuple->cd();
     
    ProdTree->Write();
    FileNtuple->Close();
    delete FileNtuple;
}

//------------------------------------------------------------------------------------

void HPAnalysis::FillNtuple(std::vector<TrackInfo_t> trackInfoVec)
{
  G4RunManager* pRunManager = G4RunManager::GetRunManager();
  g4Proddata.NPart= trackInfoVec.size();

  Int_t partNum   = 0;
  Double_t XF,PT,Ecm,PL,beta,gamma,Pxx,Pyy,Pzz,PartE;
  Double_t BeamEnergy = enerPrimGen; //got from Primary Generator
  Double_t massProton = CLHEP::proton_mass_c2;

  std::vector<TrackInfo_t>::iterator iteTrackInfo = trackInfoVec.begin();
  for(;iteTrackInfo != trackInfoVec.end();iteTrackInfo++){  
    PartE = (*iteTrackInfo).Mom.E();
    Pxx = (*iteTrackInfo).Mom.X();
    Pyy = (*iteTrackInfo).Mom.Y();
    Pzz = (*iteTrackInfo).Mom.Z();
    PT    = sqrt(pow(Pxx,2.0)+pow(Pyy,2.0));
    Ecm   = sqrt(2.0*pow(massProton,2.0)+2.*BeamEnergy*massProton);
    beta  = sqrt(pow(BeamEnergy,2.0)-pow(massProton,2.0))/(BeamEnergy+massProton);
    gamma = 1./sqrt(1.-pow(beta,2.0));
    PL    = gamma*(Pzz-beta*PartE);    
    XF    = 2.*PL/Ecm;

    g4Proddata.PDG[partNum] = (*iteTrackInfo).PDGcode;
    g4Proddata.X[partNum][0]= (*iteTrackInfo).Pos.X();
    g4Proddata.X[partNum][1]= (*iteTrackInfo).Pos.Y();
    g4Proddata.X[partNum][2]= (*iteTrackInfo).Pos.Z();
    g4Proddata.P[partNum][0]= Pxx;
    g4Proddata.P[partNum][1]= Pyy;
    g4Proddata.P[partNum][2]= Pzz;
    g4Proddata.P[partNum][3]= PartE;
    g4Proddata.PT[partNum]  = PT;
    g4Proddata.XF[partNum]  = XF;
    g4Proddata.FF[partNum]  = (*iteTrackInfo).FromFast;
     
    partNum++; 
 }
     if (g4Proddata.NPart>0)WriteNtuple();
}
void HPAnalysis::GetPrimGenInfo(Double_t enerPrim,G4ParticleDefinition* Part){
  enerPrimGen = enerPrim; 
  particle    = Part;

}
void HPAnalysis::WriteNtuple(){
    
    ProdTree->Fill();
    
}
