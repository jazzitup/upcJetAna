#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <iostream>
#include <vector>
#include <algorithm>

void processJets() {
  // USE TChain!! 
  TChain *hltTree = new TChain("hltanalysis/HltTree");
  TChain *caloJetTree = new TChain("ak4CaloJetAnalyzer/caloJetTree");
  
  // When you use a local file:
  //hltTree->Add("HiForestMiniAOD_all.root");  
  //  caloJetTree->Add("HiForestMiniAOD_all.root");

  // When you use eos files:
  hltTree->Add("/eos/cms/store/group/phys_heavyions/jdlang/run3_2023Data_Jan2024ReReco/Run3_2023UPC_375697/*/*/*/*/*.root");
  caloJetTree->Add("/eos/cms/store/group/phys_heavyions/jdlang/run3_2023Data_Jan2024ReReco/Run3_2023UPC_375697/*/*/*/*/*.root");
  //->Add("/eos/cms/store/group/phys_heavyions/jdlang/run3_2023Data_Jan2024ReReco/Run3_2023UPC_375415/HIForward13/crab_Run3_2023UPC_Jan2024ReReco_375415_HIForward13/250213_174525/0000/*.root");
  
  
  /*  TFile *file = TFile::Open("HiForestMiniAOD_all.root");
      if (!file || file->IsZombie()) {
      std::cerr << "Error opening file!" << std::endl;
      return;
      }*/
  
  // 🔹 2. Retrieve Trees
  //TTree *hltTree = (TTree*)file->Get("hltanalysis/HltTree");
  //  TTree *caloJetTree = (TTree*)file->Get("ak4CaloJetAnalyzer/caloJetTree");
  
  //  if (!hltTree || !caloJetTree) {
  //    std::cerr << "Error: Trees not found in file!" << std::endl;
  //    return;
  //  }
  
  // 🔹 3. Set Branches
  int trigger1, trigger2;
  hltTree->SetBranchAddress("HLT_HIUPC_SingleJet8_ZDC1nXOR_MaxPixelCluster50000_v2", &trigger1);
  hltTree->SetBranchAddress("HLT_HIUPC_SingleJet8_NotMBHF2AND_MaxPixelCluster50000_v2", &trigger2);
  
  const int maxJets = 1000;
  int nref;
  Float_t jtpt[maxJets];
  Float_t jteta[maxJets];
  Float_t jtphi[maxJets];
  
    caloJetTree->SetBranchAddress("nref", &nref);
    caloJetTree->SetBranchAddress("jtpt", jtpt);
    caloJetTree->SetBranchAddress("jteta", jteta);
    caloJetTree->SetBranchAddress("jtphi", jtphi);

    // 🔹 4. Create Output File and New TTree
    TFile *outFile = new TFile("output_tree_375697_all.root", "RECREATE");
    TTree *outTree = new TTree("jets", "Filtered jet data");

    int selected_nref;
    Float_t j1pt, j2pt, j3pt;  // Top 3 highest jtpt
    Float_t j1eta, j2eta, j3eta;  // Corresponding jteta values
    Float_t j1phi, j2phi, j3phi;  // Corresponding jtphi values

    outTree->Branch("nref", &selected_nref, "nref/I");
    outTree->Branch("j1pt", &j1pt, "j1pt/F");
    outTree->Branch("j2pt", &j2pt, "j2pt/F");
    outTree->Branch("j3pt", &j3pt, "j3pt/F");
    outTree->Branch("j1eta", &j1eta, "j1eta/F");
    outTree->Branch("j2eta", &j2eta, "j2eta/F");
    outTree->Branch("j3eta", &j3eta, "j3eta/F");
    outTree->Branch("j1phi", &j1phi, "j1phi/F");
    outTree->Branch("j2phi", &j2phi, "j2phi/F");
    outTree->Branch("j3phi", &j3phi, "j3phi/F");

    // 🔹 5. Event Loop
    int numEvents = hltTree->GetEntries();
    cout << "total events: " << numEvents << endl;
    for (int i = 0; i < numEvents; i++) {
      if ( i%100000 == 0 )
	cout << "working on " << i << "/"<<numEvents<< "th event..." << endl;
      
      hltTree->GetEntry(i);
      caloJetTree->GetEntry(i);
      
      // Reset variables for each event
      selected_nref = 0;
      j1pt = j2pt = j3pt = -999;
        j1eta = j2eta = j3eta = -999;
        j1phi = j2phi = j3phi = -999;

	std::vector<std::tuple<float, float, float>> selected_jets;  // Store (jtpt, jteta, jtphi)

        if (trigger1 || trigger2) {  // If at least one trigger is fired
	  for (int j = 0; j < nref; j++) {
	    if (jtpt[j] > 1 && fabs(jteta[j]) < 2.4) {  // Apply conditions
	      selected_jets.push_back(std::make_tuple(jtpt[j], jteta[j], jtphi[j]));
	      selected_nref++;
	    }
	  }
	  
	  if (!selected_jets.empty()) {
	    // Sort jets in descending order by jtpt
	    std::sort(selected_jets.begin(), selected_jets.end(), 
		      [](const std::tuple<float, float, float>& a, const std::tuple<float, float, float>& b) {
			  return std::get<0>(a) > std::get<0>(b);  // Sort by first value (jtpt)
		      });
	    
	    // Assign top 3 values
	    if (selected_jets.size() > 0) {
	      j1pt = std::get<0>(selected_jets[0]);
	      j1eta = std::get<1>(selected_jets[0]);
	      j1phi = std::get<2>(selected_jets[0]);
	    }
	    if (selected_jets.size() > 1) {
	      j2pt = std::get<0>(selected_jets[1]);
	      j2eta = std::get<1>(selected_jets[1]);
              j2phi = std::get<2>(selected_jets[1]);

	    }
	    
	    if (selected_jets.size() > 2) {
	      j3pt = std::get<0>(selected_jets[2]);
              j3eta = std::get<1>(selected_jets[2]);
              j3phi = std::get<2>(selected_jets[2]);
	    }
	    
	    // Save the event in the output tree
	    outTree->Fill();
	  }
        }
    }
    
    // 🔹 6. Write and Close Output File
    outFile->cd();
    outTree->Write();
    outFile->Close();
    
    std::cout << "Output saved to output_tree.root" << std::endl;
    
}
