
#include "TMath.h"
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

// needed for invariant cross-section calculation
// these depend on the target geometry & material
const double rho=1.78; // g/cc graphite
const double A=12.01; // g/mol Carbon
const double NA=6.022e23; // atoms/mol
const double dx=0.7;//cm target thickness
const double cm2_per_mb=1e-27; // conversion factor
const double sigma_factor=1.0/(rho/A * NA * dx * cm2_per_mb); // mb

//Constants:
const Int_t    Nhistos = 201;
const Int_t    pT_bins = 80;
const Double_t fpT     = 0.0125;
const Double_t lpT     = 2.0125;
//const Double_t DpT     = (lpT-fpT)/Double_t(pT_bins);
const Double_t fxF     = -0.1025;
const Double_t lxF     = 0.9025;
const Double_t DxF     = (lxF- fxF)/Double_t(Nhistos);
const Double_t DxF_neu = 0.01;

//Masses in MeVs:
const Double_t mPion    = 0.13957;
const Double_t mKaon    = 0.493667;
const Double_t mProton  = 0.938272;
const Double_t mNeutron = 0.939565;
const Double_t mKaon0   = 0.497611;

const Int_t Npart = 8; //6
const std::string spart[Npart] = {"pip","pim","kap","kam","klong","kshort","prt","neu"};

using std::cout;
using std::endl;

void CreateInvXS(Double_t mom, Int_t nincident, const char* infile, const char* outfile, double yield_tot, double frac_prod);
Double_t getxF(Int_t id);
Double_t getEcm(Double_t beam_mom);
Double_t getEnergy(Double_t beam_mom,Double_t xfey,Double_t ptrans,std::string partname);
Double_t getPz(Double_t beam_mom,Double_t xfey,Double_t ptrans,std::string partname);
Double_t getDPz(Double_t beam_mom,Double_t xfey,Double_t ptrans,std::string partname);

void CreateInvXS(Double_t mom, Double_t nincident, const char* infile, const char* outfile, double yield_tot, double frac_prod){
  cout<<"===>>>Running begin"<<endl;

  TFile* finput  = new TFile(infile,"READ");
  TFile* foutput = new TFile(outfile,"RECREATE");

  Double_t inel_xs = yield_tot*frac_prod;
  Double_t na49_trig_xs = 210.1; // trigger cross-section for neutron yields after NA49 acceptance cuts
  // std::ifstream ifs;
  // // expects fraction of prod / total yields
  // ifs.open(inel_yieldfile);
  // std::string line;
  // while (ifs.good()){
  //   std::getline(ifs, line);
  //   if(line.find("#") == 0) continue;
  //   if(!ifs.good()) continue;
  //   inel_xs = (double)std::atoi(line.c_str());
  // }
  inel_xs *= (double)sigma_factor/nincident;
  std::cout << inel_xs << std::endl;

  TH2* yield_xFpT[Npart];
  TH2* sigma_xFpT[Npart+2]; // for QE/fragmentation
  TH2* yield_xFpT_tot[2]; // for QE/fragmentation
  TH1D* yield_dndxf_neu;
  TH1D* yield_dndxf_neu_cut;
  TH1D* yield_dndxf_neu_prod;
  TH1D* yield_dndxf_neu_prod_cut;
  TH1D* dndxf_neu;
  TH1D* dndxf_neu_cut;
  TH1D* dndxf_neu_prod;
  TH1D* dndxf_neu_prod_cut;
  TH1D* hinel_frac;
  TH1D* htot_xs;

  for(Int_t ii=0;ii<Npart;ii++){
    yield_xFpT[ii] = (TH2*)finput->Get(Form("xFpT_%s",spart[ii].c_str()));
    if(ii >= 6){
      yield_xFpT_tot[ii-6] = (TH2*)finput->Get(Form("xFpT_tot_%s", spart[ii].c_str()));
    }
  }
  yield_dndxf_neu = (TH1D*)finput->Get("dndxf_neu");
  yield_dndxf_neu_cut = (TH1D*)finput->Get("dndxf_neu_cut");
  yield_dndxf_neu_prod = (TH1D*)finput->Get("dndxf_neu_prod");
  yield_dndxf_neu_prod_cut = (TH1D*)finput->Get("dndxf_neu_prod_cut");

  // copy the yields histogram into the cross-section histogram
  // we convert to cross-section later
  foutput->cd(0);
  for(Int_t ii=0;ii<Npart;ii++){
    sigma_xFpT[ii] = (TH2*) yield_xFpT[ii]->Clone(Form("sigma_xFpT_%s",spart[ii].c_str()));
    if(ii >= 6){
      sigma_xFpT[ii+2] = (TH2*)yield_xFpT_tot[ii-6]->Clone(Form("sigma_xFpT_tot_%s", spart[ii].c_str()));
    }
  }
  // get 1D from saved 1D yield histograms directly
  dndxf_neu = (TH1D*) yield_dndxf_neu->Clone("xs_dndxf_neu");
  dndxf_neu_cut = (TH1D*) yield_dndxf_neu_cut->Clone("xs_dndxf_neu_cut");
  dndxf_neu_prod = (TH1D*) yield_dndxf_neu_prod->Clone("xs_dndxf_neu_prod");
  dndxf_neu_prod_cut = (TH1D*) yield_dndxf_neu_prod_cut->Clone("xs_dndxf_neu_prod_cut");
  hinel_frac = new TH1D("inel_frac", "", 1, 0, 1);
  hinel_frac->SetBinContent(1, frac_prod);
  htot_xs = new TH1D("tot_xs", "", 1, 0, 1);
  htot_xs->SetBinContent(1, inel_xs/frac_prod);

  // convert the 2D yields histogram into a 2D cross-section histogram
  for(Int_t ii=0;ii<Npart+2;ii++){
    TH2* hxs=sigma_xFpT[ii];
    hxs->Sumw2();

    std::string part_name = (ii > 7) ? spart[ii-2] : spart[ii];
    if(ii == 7){
      for(Int_t ix=1; ix <= dndxf_neu->GetNbinsX(); ix++){
        // format for neutron histograms ppfx expects (fraction wrt total inelastic xsec)
        dndxf_neu->SetBinContent(ix, (dndxf_neu->GetBinContent(ix)/nincident)*sigma_factor/(DxF_neu*inel_xs));
        dndxf_neu_cut->SetBinContent(ix, (dndxf_neu_cut->GetBinContent(ix)/nincident)*sigma_factor/(DxF_neu*na49_trig_xs));
        dndxf_neu_prod->SetBinContent(ix, (dndxf_neu_prod->GetBinContent(ix)/nincident)*sigma_factor/(DxF_neu*inel_xs));
        dndxf_neu_prod_cut->SetBinContent(ix, (dndxf_neu_prod_cut->GetBinContent(ix)/nincident)*sigma_factor/(DxF_neu*na49_trig_xs));
      }
    }
    for(Int_t ix=1; ix<=hxs->GetNbinsX(); ix++){
      Double_t xfval = hxs->GetXaxis()->GetBinCenter(ix);
      Double_t DPzMid = getDPz(mom, xfval, 1.0, part_name);
      //
      Double_t dndxf_neu_prod_bin = 0.;
      for(Int_t ipt=1; ipt<=hxs->GetNbinsY(); ipt++){

        Double_t ptval  = hxs->GetYaxis()->GetBinCenter(ipt);
        Double_t DpT = hxs->GetYaxis()->GetBinWidth(ipt);
        Double_t pTup   = ptval + DpT/2.0;
        Double_t pTdown = ptval - DpT/2.0;
        Double_t DpT2   = pow(pTup,2.)-pow(pTdown,2.);

        Double_t Ener   = getEnergy(mom,xfval, ptval,part_name);
        Double_t yield  = hxs->GetBinContent(ix,ipt);
        Double_t err_yield  = hxs->GetBinError(ix,ipt);

        Double_t DPz    = getDPz(mom,xfval,ptval,part_name);
        Double_t DP3    = TMath::Pi()*DPz*DpT2;
        Double_t invXS  = Ener/DP3 * (yield/nincident)*sigma_factor;
        Double_t err_invXS = Ener/DP3 * (err_yield/nincident)*sigma_factor;
        hxs->SetBinContent(ix,ipt,invXS);
        hxs->SetBinError(ix,ipt,err_invXS);

        Double_t beamEcm = getEcm(mom);
      }
    }
  }


  foutput->Write();
  foutput->Close();
  cout<<"===>>>Running end"<<endl;

}

//Get xF value from xF histo number:
Double_t getxF(Int_t id){
  return fxF + (Double_t(id)+0.5)*DxF;
}

Double_t getEcm(Double_t beam_mom){
  Double_t beam_ener = sqrt(pow(beam_mom,2.0)+pow(mProton,2.0));
  Double_t Ecm = sqrt(2.*pow(mProton,2.0)+2.*beam_ener*mProton);
  return Ecm;
}

//
Double_t getEnergy(Double_t beam_mom,Double_t xfey,Double_t ptrans,std::string partname){

  Double_t mPart = 0;
  if(partname=="pip" || partname=="pim")mPart = mPion;
  else if(partname=="kap" || partname=="kam")mPart = mKaon;
  else if(partname=="klong" || partname=="kshort")mPart = mKaon0;
  else if(partname=="prt")mPart = mProton;
  else if(partname=="neu")mPart = mNeutron;
  else {
    cout<<"Error here... no particle under study (p, n, K+/-, pi+/-)"<<endl;
    return 1000.;
  }

  Double_t beam_ener = sqrt(pow(beam_mom,2.0)+pow(mProton,2.0));
  Double_t beta= sqrt(pow(beam_ener,2.0)-pow(mProton,2.0))/(beam_ener+mProton);
  Double_t gamma  = 1./sqrt(1.-pow(beta,2.0));
  Double_t Ecm = sqrt(2.*pow(mProton,2.0)+2.*beam_ener*mProton);
  Double_t pl=xfey*Ecm/2.;
  Double_t EnerPartCM = sqrt(pow(pl,2.0)+pow(ptrans,2.0)+pow(mPart,2.0));
  return gamma*(EnerPartCM+beta*pl);
}
///
Double_t getPz(Double_t beam_mom,Double_t xfey,Double_t ptrans,std::string partname){
  Double_t mPart = 0;
  if(partname=="pip" || partname=="pim")mPart = mPion;
  else if(partname=="kap" || partname=="kam")mPart = mKaon;
  else if(partname=="klong" || partname=="kshort")mPart = mKaon0;
  else if(partname=="prt")mPart = mProton;
  else if(partname=="neu")mPart = mNeutron;
  else {
    cout<<"Error here... no particle under study (p, n, K+/-, pi+/-)"<<endl;
    return 1000.;
  }

  Double_t beam_ener = sqrt(pow(beam_mom,2.0)+pow(mProton,2.0));
  Double_t Ecm = sqrt(2.*pow(mProton,2.0)+2.*beam_ener*mProton);
  Double_t beta= sqrt(pow(beam_ener,2.0)-pow(mProton,2.0))/(beam_ener+mProton);
  Double_t gamma  = 1./sqrt(1.0-pow(beta,2.0));
  Double_t pl=xfey*Ecm/2.;
  Double_t EnerPartCM = sqrt(pow(pl,2.0)+pow(ptrans,2.0)+pow(mPart,2.0));
  return gamma*(beta*EnerPartCM+pl);
}

///
Double_t getDPz(Double_t beam_mom,Double_t xfey,Double_t ptrans,std::string partname){

  Double_t xF_l  = xfey-0.5*DxF;
  Double_t xF_r =  xfey+0.5*DxF;

  Double_t Pzup   = getPz(beam_mom,xF_r,ptrans,partname);
  Double_t Pzdown = getPz(beam_mom,xF_l,ptrans,partname);
  return   Pzup - Pzdown;
}

# ifndef __CINT__
int main(int argc, const char* argv[]){
  double tmom = atof(argv[1]);
  double ninc = atof(argv[2]);
  double yield_tot = atof(argv[5]);
  double frac_prod = atof(argv[6]);
  bool doff = true;
  CreateInvXS(tmom,ninc,argv[3],argv[4],yield_tot,frac_prod);
  return 0;
}
# endif
