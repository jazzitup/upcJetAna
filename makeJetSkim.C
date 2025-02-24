#include <iostream>
#include <vector>
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"

void makeJetSkim() {
  // 🔹 1. ROOT 파일 열기
  TFile *file = TFile::Open("HiForestMiniAOD_all.root");
  if (!file || file->IsZombie()) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    // 🔹 2. HltTree와 caloJetTree 가져오기
    TTree *hltTree = (TTree*)file->Get("hltanalysis/HltTree");
    TTree *caloJetTree = (TTree*)file->Get("ak4CaloJetAnalyzer/caloJetTree");

    if (!hltTree || !caloJetTree) {
        std::cerr << "Error: Trees not found in file!" << std::endl;
        return;
    }

    // 🔹 3. 트리거 브랜치 설정 (두 개의 트리거만 사용)
    int trigger1, trigger2;
    hltTree->SetBranchAddress("HLT_HIUPC_SingleJet8_ZDC1nXOR_MaxPixelCluster50000_v2", &trigger1);
    hltTree->SetBranchAddress("HLT_HIUPC_SingleJet8_NotMBHF2AND_MaxPixelCluster50000_v2", &trigger2);

    // 🔹 4. Jet 브랜치 설정 (물리적 특성만 사용)
    const int maxJets = 100;
    int nref; 
    Float_t jtpt[maxJets];
    Float_t jteta[maxJets];
    Float_t jtphi[maxJets];
    caloJetTree->SetBranchAddress("nref", &nref);
    caloJetTree->SetBranchAddress("jtpt", jtpt);
    caloJetTree->SetBranchAddress("jteta", jteta);
    caloJetTree->SetBranchAddress("jtphi", jtphi);
    //   Float_t         rawpt[35];   //[nref]
    //   Float_t         jtpt[35];   //[nref]
    //   Float_t         jteta[35];   //[nref]
    //   Float_t         jty[35];   //[nref]
    //   Float_t         jtphi[35];   //[nref]
    //   Float_t         jtpu[35];   //[nref]
    //   Float_t         jtm[35];   //[nref]
    //   Float_t         jtarea[35];   //[nref]

     // 🔹 4. Create Output File and New TTree
    TFile *outFile = new TFile("output_tree.root", "RECREATE");
    TTree *outTree = new TTree("selectedJets", "Filtered jet data");

    Float_t jet1pt=-1; // leading jet pT
    Float_t jet1eta=-1; // leading jet Eta
    Float_t jet1phi=-1; // leading jet Eta

    Float_t jet2pt=-11; // subleading jet pT
    Float_t jet2eta=-11; // subleading jet Eta
    Float_t jet2phi=-11; // subleading jet Eta

    Float_t jet3pt=-111; // third leading jet pT
    Float_t jet3eta=-111; // third leading jet Eta
    Float_t jet3phi=-111; // third leading jet Eta

    outTree->Branch("jet1pt", &jet1pt, "jet1pt/F");
    outTree->Branch("jet1eta", &jet1eta, "jet1eta/F");
    outTree->Branch("jet1phi", &jet1phi, "jet1phi/F");
    outTree->Branch("jet2pt", &jet2pt, "jet2pt/F");
    outTree->Branch("jet2eta", &jet2eta, "jet2eta/F");
    outTree->Branch("jet2phi", &jet2phi, "jet2phi/F");
    outTree->Branch("jet3pt", &jet3pt, "jet3pt/F");
    outTree->Branch("jet3eta", &jet3eta, "jet3eta/F");
    outTree->Branch("jet3phi", &jet3phi, "jet3phi/F");

    
    // 🔹 5. 히스토그램 정의 (매번 실행 시마다 새로 생성)
    TH1F *h_jtpt = new TH1F("h_jtpt", "Jet Transverse Momentum (jtpt > 1)", 100, 0, 40);
    TH1F *h_jteta = new TH1F("h_jteta", "Eta Distribution (jtpt > 1)", 50, -5, 5);
    TH1F *h_jtphi = new TH1F("h_jtphi", "Phi Distribution (jtpt > 1)", 50, -3.14, 3.14);
    TH2F *h_jtphi_vs_jteta = new TH2F("h_jtphi_vs_jteta", "Jet Phi vs Eta (jtpt > 1)", 
                                      50, -5, 5, 50, -3.14, 3.14);

    // 🔹 6. 이벤트 루프
    int numEvents = hltTree->GetEntries();
    for (int i = 0; i < numEvents; i++) {
        hltTree->GetEntry(i);
        caloJetTree->GetEntry(i);
	cout << " nref = " << nref << endl;
        // 🔸 트리거가 fire된 이벤트인지 확인 (둘 중 하나라도 fire되면 pass)
        if (trigger1 || trigger2) {
            // 🔸 Jet 정보 저장 (jtpt > 1인 경우만)
            for (int j = 0; j < nref; j++) {

	      if ( fabs(jteta[j]) > 2.4 ) continue;   // We account only jets within |eta|<2.4
	      
	      if (jtpt[j] > 1) {  // pT가 1보다 큰 jet만 처리
		h_jtpt->Fill(jtpt[j]);
		h_jteta->Fill(jteta[j]);
		h_jtphi->Fill(jtphi[j]);
		h_jtphi_vs_jteta->Fill(jteta[j], jtphi[j]); // 2D 히스토그램에도 추가
	      }
            }
        }
    }
    
    // 🔹 7. 히스토그램 그리기
    TCanvas *canvas1 = new TCanvas("canvas1", "Jet Transverse Momentum", 800, 600);
    h_jtpt->Draw();
    canvas1->SaveAs("jet_pt_distribution.png");

    TCanvas *canvas2 = new TCanvas("canvas2", "Jet Eta", 800, 600);
    h_jteta->Draw();
    canvas2->SaveAs("jet_eta_distribution.png");

    TCanvas *canvas3 = new TCanvas("canvas3", "Jet Phi", 800, 600);
    h_jtphi->Draw();
    canvas3->SaveAs("jet_phi_distribution.png");

    TCanvas *canvas4 = new TCanvas("canvas4", "Jet Phi vs Eta", 800, 600);
    h_jtphi_vs_jteta->Draw("colz");
    canvas4->SaveAs("jet_phi_vs_eta.png");

    // 🔹 8. 파일 닫기
    file->Close();
    delete file;
}
