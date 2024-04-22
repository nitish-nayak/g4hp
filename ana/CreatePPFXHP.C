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
using namespace std;
const Int_t Npart = 8; //4; //8; //6
//const char* part[Npart]  = {"pip","pim","kap","kam"};
const char* part[Npart]  = {"pip","pim","kap","kam","klong","kshort","prt","neu"};
TH2D* getvhist(TFile* fIn, int ipart);
TH2D* getvtothist(TFile* fIn, int ipart);
void CreatePPFXHP(const char* invxsfile, const char* yieldfile, const char* physicslist, const char* incE){
  //take a text file with the paths of all the G4HP simulations and convert them into
  //QGSP ntuples for PPFX

  //expects that the text file is organized by energy already

  //invxsfiles: text file with all invxs file paths
  //yieldfiles: text file with all yields file paths
  //physicslist: the physics list used in the G4HP simulations (i.e. FTFP_BERT, QGSP_BERT, etc.)

  TObjArray piplist(0);
  TObjArray pimlist(0);
  TObjArray kaplist(0);
  TObjArray kamlist(0);
  TObjArray klonglist(0);
  TObjArray kshortlist(0);
  TObjArray prtlist(0);
  TObjArray neulist(0);
  TObjArray yieldneulist(0);
  TObjArray totprodlist(0);
  TObjArray qe_prtlist(0);
  TObjArray qe_neulist(0);

  cout<<"Checking the invariant cross section files..."<<endl;
  //cout<<""<<endl;

  //  TChain* chain = new TChain("hAinfoTree");
  //  std::ifstream ifs;
  //  ifs.open(invxsfiles);
  //  std::string line;
  // // if (ifs.fail()) std::cout<<"fail"<<std::endl;
  // // if (ifs.good()) std::cout<<"good"<<std::endl;
  // // if (ifs.eof()) std::cout<<"eof"<<std::endl;
  // // if (ifs.bad()) std::cout<<"bad"<<std::endl;
  //
  //  while (ifs.good()) {
  //    getline(ifs,line);
  //    //std::cout<<line<<endl;
  //    if(line.find(".root")>10000)continue; //std::cout<<"hey"<<std::endl; //continue;
  //    chain->Add(line.c_str());
  //    std::cout<<"Adding ntuple at : "<<line<<std::endl;
  //  }
  //  ifs.close();
  //
  //  cout<<endl;
  //  cout<<"Checking the yields file..."<<endl;
  //  //cout<<""<<endl;
  //
  //  TChain* chain_y = new TChain("yieldsTree");
  //  std::ifstream ifs_y;
  //  ifs_y.open(yieldfiles);
  //  std::string line_y;
  //  while (ifs_y.good()) {
  //
  //        getline(ifs_y,line_y);
  //        //std::cout<<line_y<<endl;
  //        if(line_y.find(".root")>10000)continue;
  //        chain_y->Add(line_y.c_str());
  //        std::cout<<"Adding yields ntuple at : "<<line_y<<std::endl;
  //  }
  //  ifs_y.close();
  //
  //  cout<<endl;
  //  std::cout<< (chain->GetNtrees()) <<" files were added to the invxs chain"<<std::endl;
  //  std::cout<< (chain_y->GetNtrees()) <<" files were added to the yields chain"<<std::endl;
  //  cout<<endl;
  //
  //  int ntrees = (int)chain->GetNtrees();
  //
  //  //loop over TFiles
  //  TObjArray *fileElements=chain->GetListOfFiles();
  //  TIter next(fileElements);
  //  TChainElement *chEl=0;
  // while (( chEl=(TChainElement*)next() )) {
  // TFile* f = new TFile(chEl->GetTitle(),"UPDATE");
  TFile* f = new TFile(invxsfile,"READ");
  //cout<<chEl->GetTitle()<<endl;
  //cout<<f->GetName()<<endl;
  //
  for(int jj=0;jj<Npart;jj++){
    TH2D* htemp = getvhist(f,jj);

    //find number in string title
    //       string c = f->GetName();
    // //cout<<c<<endl;
    //             size_t pos = c.find("xs_e");
    //       //cout<<pos<<endl;
    //             string name = c.substr(pos+4);
    // //cout<<name<<endl;
    //             string name_end = ".root";
    // size_t pos_root = name.find(name_end);
    // name.erase(pos_root,name_end.length());
    // //cout<<name<<endl;
    //             const char* newname = name.c_str();
    // //cout<<newname<<endl;

    htemp->SetName(TString::Format("xF_pT_%sGeV",incE));
    if(jj == 6){
      TH2D* htemp2 = getvtothist(f,jj);
      TH2D* frac_prod = (TH2D*)htemp->Clone(TString::Format("frac_prod_xF_pT_%sGeV",incE));
      frac_prod->Divide(htemp2);
      qe_prtlist.Add(frac_prod);
    }

    //cout<<jj<<endl;

    //get each particle and add them to array which is arranged by energy
    if(jj==0)piplist.Add(htemp);
    if(jj==1)pimlist.Add(htemp);
    if(jj==2)kaplist.Add(htemp);
    if(jj==3)kamlist.Add(htemp);
    if(jj==4)klonglist.Add(htemp);
    if(jj==5)kshortlist.Add(htemp);
    if(jj==6)prtlist.Add(htemp);
    if(jj==7)neulist.Add(htemp);

  }
  // }
  //loop over TFiles for yields
  // TObjArray *fileElements_y=chain_y->GetListOfFiles();
  // TIter next_y(fileElements_y);
  // TChainElement *chEl_y=0;
  // while (( chEl_y=(TChainElement*)next_y() )) {
  // TFile* g = new TFile(chEl_y->GetTitle(),"UPDATE");
  // TFile* g = new TFile(yieldfile,"READ");
  TH1D* htemp_y;
  TH1D* htemp_neu_qe;
  f->GetObject("xs_dndxf_neu_prod_cut",htemp_y);
  f->GetObject("xs_dndxf_neu_cut",htemp_neu_qe);
  TH1D* frac_prod_neu = (TH1D*)htemp_y->Clone(TString::Format("frac_prod_xf_%sGeV", incE));
  frac_prod_neu->Divide(htemp_neu_qe);
  qe_neulist.Add(frac_prod_neu);
  //find number in string title
  // string c_y = g->GetName();
  // //cout<<c_y<<endl;
  // size_t pos_y = c_y.find("yields_e");
  // string name_y = c_y.substr(pos_y+8);
  // //cout<<name_y<<endl;
  // string name_end_y = ".root";
  // size_t pos_root_y = name_y.find(name_end_y);
  // name_y.erase(pos_root_y,name_end_y.length());
  // //cout<<name_y<<endl;
  // const char* newname_y = name_y.c_str();
  // //cout<<newname_y<<endl;
  htemp_y->SetName(TString::Format("dndxf_%sGeV",incE));
  yieldneulist.Add(htemp_y);

  TH1D* htemp_z;
  TH1D* htemp_tot_xs;
  f->GetObject("inel_frac", htemp_z);
  f->GetObject("tot_xs", htemp_tot_xs);
  htemp_z->SetName(TString::Format("frac_prod_xs_%sGeV", incE));
  htemp_tot_xs->SetName(TString::Format("tot_xs_%sGeV", incE));
  totprodlist.Add(htemp_z);
  totprodlist.Add(htemp_tot_xs);
  // }
  //make new directory
  // mkdir("PPFX",0777);

  //make new ROOT files
  TFile* invxs_pip = new TFile(Form("PPFX/invxs_pip_%s.root",physicslist),"UPDATE");
  piplist.Write();
  TFile* invxs_pim = new TFile(Form("PPFX/invxs_pim_%s.root",physicslist),"UPDATE");
  pimlist.Write();
  TFile* invxs_kap = new TFile(Form("PPFX/invxs_kap_%s.root",physicslist),"UPDATE");
  kaplist.Write();
  TFile* invxs_kam = new TFile(Form("PPFX/invxs_kam_%s.root",physicslist),"UPDATE");
  kamlist.Write();
  TFile* invxs_klong = new TFile(Form("PPFX/invxs_klong_%s.root",physicslist),"UPDATE");
  klonglist.Write();
  TFile* invxs_kshort = new TFile(Form("PPFX/invxs_kshort_%s.root",physicslist),"UPDATE");
  kshortlist.Write();
  TFile* invxs_prt = new TFile(Form("PPFX/invxs_prt_%s.root",physicslist),"UPDATE");
  prtlist.Write();
  TFile* invxs_neu = new TFile(Form("PPFX/invxs_neu_%s.root",physicslist),"UPDATE");
  neulist.Write();
  TFile* yield_neu = new TFile(Form("PPFX/yield_neu_%s.root",physicslist),"UPDATE");
  yieldneulist.Write();
  TFile* frac_prod = new TFile(Form("PPFX/frac_prod_%s.root",physicslist),"UPDATE");
  totprodlist.Write();
  TFile* qe_prt_f = new TFile("PPFX/invxs_qe_corr_prt.root", "UPDATE");
  qe_prtlist.Write();
  TFile* qe_neu_f = new TFile("PPFX/yield_qe_corr_neu.root", "UPDATE");
  qe_neulist.Write();

  invxs_pip->Close();
  invxs_pim->Close();
  invxs_kap->Close();
  invxs_kam->Close();
  invxs_klong->Close();
  invxs_kshort->Close();
  invxs_prt->Close();
  invxs_neu->Close();
  yield_neu->Close();
  frac_prod->Close();
  qe_prt_f->Close();
  qe_neu_f->Close();

  std::cout<<"Just made a new PPFX directory. Check it out!"<<std::endl;
}

TH2D* getvhist(TFile* fIn, int ipart){
  //get histogram from file
  TH2D* vtmp;
  fIn->GetObject(Form("sigma_xFpT_%s",part[ipart]),vtmp);
  return vtmp;
}

TH2D* getvtothist(TFile* fIn, int ipart){
  //get histogram from file
  TH2D* vtmp;
  fIn->GetObject(Form("sigma_xFpT_tot_%s",part[ipart]),vtmp);
  return vtmp;
}

int main(int argc, const char* argv[]){
  CreatePPFXHP(argv[1],argv[2],argv[3],argv[4]);
  return 0;
}


