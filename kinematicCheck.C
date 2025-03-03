#include "commonUtility.h"
#include <TChain.h>
#include <TTreeReader.h>
#include <TTreeReaderArray.h>
#include <TMath.h>
#include <TGraph2D.h>
#include <TRandom2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TF2.h>
#include <TH1.h>
#include <TGraphErrors.h>
#include <TMarker.h>
#include <Math/Functor.h>
#include <TPolyLine3D.h>
#include <Math/Vector3D.h>
#include <Fit/Fitter.h>
#include <cassert>
#include <TMinuit.h>
#include <TH3D.h>
#include <iostream>
#include <vector>
#include <TVector3.h>
#include <TPolyLine3D.h>
#include <TLorentzVector.h>

//void test(TString infile="podio_files/Pentaquark_hepmc_output_20241202_p275.0GeV_e18.0GeV_two_body_kinematics_eta1.9-8_100000evts_ip6_hidiv_275x18.root") {
void kinematicCheck(TString fname="output_tree_13k.root") {

  float ptcut1 = 20;
  float ptcut2 = ptcut1;


  TFile* infile =new TFile(fname);
    TTree* jetTree = (TTree*)infile->Get("jets");
   Int_t           nref;
   Float_t         j1pt;
   Float_t         j2pt;
   Float_t         j3pt;
   Float_t         j1eta;
   Float_t         j2eta;
   Float_t         j3eta;
   Float_t         j1phi;
   Float_t         j2phi;
   Float_t         j3phi;

   // List of branches
   TBranch        *b_nref;   //!
   TBranch        *b_j1pt;   //!
   TBranch        *b_j2pt;   //!
   TBranch        *b_j3pt;   //!
   TBranch        *b_j1eta;   //!
   TBranch        *b_j2eta;   //!
   TBranch        *b_j3eta;   //!
   TBranch        *b_j1phi;   //!
   TBranch        *b_j2phi;   //!
   TBranch        *b_j3phi;   //!

   jetTree->SetBranchAddress("nref", &nref, &b_nref);
   jetTree->SetBranchAddress("j1pt", &j1pt, &b_j1pt);
   jetTree->SetBranchAddress("j2pt", &j2pt, &b_j2pt);
   jetTree->SetBranchAddress("j3pt", &j3pt, &b_j3pt);
   jetTree->SetBranchAddress("j1eta", &j1eta, &b_j1eta);
   jetTree->SetBranchAddress("j2eta", &j2eta, &b_j2eta);
   jetTree->SetBranchAddress("j3eta", &j3eta, &b_j3eta);
   jetTree->SetBranchAddress("j1phi", &j1phi, &b_j1phi);
   jetTree->SetBranchAddress("j2phi", &j2phi, &b_j2phi);
   jetTree->SetBranchAddress("j3phi", &j3phi, &b_j3phi);
    

  
    // Gen Particle histograms
    TH1D *hPt1 = new TH1D("hpt1",";p_{T} (GeV);",100,0,100);   // pT spectrum of the leading jet 
    TH1D *hPt2 = (TH1D*)hPt1->Clone("hpt2");
    TH1D *hPt3 = (TH1D*)hPt1->Clone("hpt3");
    TH1D *hDphi12 = new TH1D("hDphi12","; #Delta #phi;",100,0,3.2);
    TH1D *hDeta12 = new TH1D("hDeta12","; #Delta #eta;",20,-5,5);
    TH1D *hDeta12_dphiCut = (TH1D*)hDeta12->Clone("hDeta12_dPhiCut");

    TH1D *hDeta13_dphiCut = (TH1D*)hDeta12->Clone("hDeta12_dPhiCut");

    int nEntries = jetTree->GetEntries();
    
    for ( int i=0 ; i< nEntries ; i++) { 
        jetTree->GetEntry(i);

	if (j1pt < ptcut1)
	  continue;
	if (j2pt < ptcut2)
	  continue;
	
	hPt1->Fill( j1pt);
        hPt2->Fill( j2pt);
        hPt3->Fill( j3pt);
	
	float dphi12 = getDphi(j1phi, j2phi);
	hDphi12->Fill(dphi12);

	hDeta12->Fill( j2eta - j1eta);
	if (dphi12 > 2/3. *3.141592)  {
	  hDeta12_dphiCut->Fill(j2eta - j1eta);
	  
	  if ( ( j3pt>0 )) // && ( fabs((j1eta+j2eta)/2. - 1) < 0.5 ) )
	    hDeta13_dphiCut->Fill(j3eta - (j1eta+j2eta)/2.);
	}
    }
    

    handsomeTH1(hPt1,1); 
    handsomeTH1(hPt2,2); 
    handsomeTH1(hPt3,4); 

    handsomeTH1(hDeta12, 1);
    handsomeTH1(hDeta12_dphiCut, 2);
    TCanvas *canvas1 = new TCanvas("canvas1", "Jet Transverse Momentum", 800, 600);
    hPt1->Draw();
    hPt2->Draw("same");
    hPt3->Draw("same");

    TCanvas *canvas2 = new TCanvas("canvas2", "",1200,400);
    canvas2->Divide(3,1);
    canvas2->cd(1);
    hDphi12->Draw();
    canvas2->cd(2);
    
    hDeta12->Draw("");
    hDeta12_dphiCut->Draw("same");
    canvas2->cd(3);
    hDeta13_dphiCut->Draw();

    
}
