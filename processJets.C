#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <TChain.h>

//void processJets(TString inputDir="/eos/cms/store/group/phys_heavyions/jdlang/run3_2023Data_Jan2024ReReco/Run3_2023UPC_375697/HIForward13/*/*/*/*.root", TString outputFile="output375697_HiForward13.root") {
void processJets(TString inputDir="forest_000.root", TString outputFile="output_tree_L1SignleJet8trig.root") {
  // USE TChain!! 
  TChain *hltTree = new TChain("hltanalysis/HltTree");
  TChain *jetTree = new TChain("ak4PFJetAnalyzer/t");
  TChain *trackTree = new TChain("ppTracks/trackTree");
  

  // When you use eos files:
  hltTree->Add(inputDir); 
  jetTree->Add(inputDir);
  trackTree->Add(inputDir);
  
  // 🔹 3. Set Branches
  int trigger1, trigger2, trigSingleJet8;
  int notMBTrig;
  hltTree->SetBranchAddress("HLT_HIUPC_SingleJet8_ZDC1nXOR_MaxPixelCluster50000_v2", &trigger1);
  hltTree->SetBranchAddress("HLT_HIUPC_SingleJet8_NotMBHF2AND_MaxPixelCluster50000_v2", &trigger2);
  hltTree->SetBranchAddress("L1_SingleJet8_NotMinimumBiasHF2_AND_BptxAND", &trigSingleJet8);
  hltTree->SetBranchAddress("L1_NotMinimumBiasHF2_AND_BptxAND", &notMBTrig);
  
  const int maxJets = 1000;
  int nref;
  Float_t jtpt[maxJets];
  Float_t jteta[maxJets];
  Float_t jtphi[maxJets];
  
  jetTree->SetBranchAddress("nref", &nref);
  jetTree->SetBranchAddress("jtpt", jtpt);
  jetTree->SetBranchAddress("jteta", jteta);
  jetTree->SetBranchAddress("jtphi", jtphi);
  
  const int maxTrks= 2000;
   Int_t nTrk;
   vector<float> *trkPt = nullptr;
   vector<float> *trkEta = nullptr;
   vector<float>   *trkPhi= nullptr;
   vector<bool>    *highPurity= nullptr;
   vector<float>   *trkDzFirstVtx= nullptr;
   vector<float>   *trkDzErrFirstVtx= nullptr;
   vector<float>   *trkDxyFirstVtx= nullptr;
   vector<float>   *trkDxyErrFirstVtx= nullptr;

   TBranch        *b_nTrk;   //!
   TBranch        *b_trkPt;   //!
   TBranch        *b_trkPtError;   //!
   TBranch        *b_trkEta;   //!
   TBranch        *b_trkPhi;   //!
   TBranch        *b_highPurity;   //!
   TBranch        *b_trkDzFirstVtx;   //!
   TBranch        *b_trkDzErrFirstVtx;   //!
   TBranch        *b_trkDxyFirstVtx;   //!
   TBranch        *b_trkDxyErrFirstVtx;   //!
   
   trackTree->SetBranchAddress("nTrk", &nTrk, &b_nTrk);
   trackTree->SetBranchAddress("trkPt", &trkPt, &b_trkPt);
   trackTree->SetBranchAddress("trkEta", &trkEta, &b_trkEta);
   trackTree->SetBranchAddress("trkPhi", &trkPhi, &b_trkPhi);
   trackTree->SetBranchAddress("highPurity", &highPurity, &b_highPurity);
   trackTree->SetBranchAddress("trkDzFirstVtx", &trkDzFirstVtx, &b_trkDzFirstVtx);
   trackTree->SetBranchAddress("trkDzErrFirstVtx", &trkDzErrFirstVtx, &b_trkDzErrFirstVtx);
   trackTree->SetBranchAddress("trkDxyFirstVtx", &trkDxyFirstVtx, &b_trkDxyFirstVtx);
   trackTree->SetBranchAddress("trkDxyErrFirstVtx", &trkDxyErrFirstVtx, &b_trkDxyErrFirstVtx);

   
  // 🔹 4. Create Output File and New TTree
    TFile *outFile = new TFile(outputFile,"RECREATE");
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

    TTree *outTrkTree = new TTree("trks", "Filtered track data");
    int output_nTrk;
    Float_t output_trkPt[2000];
    Float_t output_trkEta[2000];
    Float_t output_trkPhi[2000];
    outTrkTree->Branch("nTrk", &output_nTrk, "nTrk/I");
    outTrkTree->Branch("trkPt", output_trkPt, "trkPt[nTrk]/F");
    outTrkTree->Branch("trkEta", output_trkEta, "trkEta[nTrk]/F");
    outTrkTree->Branch("trkPhi", output_trkPhi, "trkPhi[nTrk]/F");
    
    // 🔹 5. Event Loop
    int numEvents = hltTree->GetEntries();
    cout << "total events: " << numEvents << endl;
    //    for (int i = 0; i < 10000; i++) {
    for (int i = 0; i < numEvents; i++) {
      if ( i%100000 == 0 )
	cout << "working on " << i << "/"<<numEvents<< "th event..." << endl;
      
      hltTree->GetEntry(i);
      jetTree->GetEntry(i);
      trackTree->GetEntry(i);
      
      // Reset variables for each event
      selected_nref = 0;
      j1pt = j2pt = j3pt = -999;
      j1eta = j2eta = j3eta = -999;
      j1phi = j2phi = j3phi = -999;
      
      std::vector<std::tuple<float, float, float>> selected_jets;  // Store (jtpt, jteta, jtphi)
      
      //        if (trigger1 || trigger2) {  // If at least one trigger is fired
      //      cout << "notMBTrig = " << notMBTrig << endl;
      if (trigSingleJet8) {  // If at least one trigger is fired
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
	  
	  
	}

	cout << "nTrk  = " << nTrk << endl;
	output_nTrk = 0;
	for (int itrk = 0; itrk < nTrk; itrk ++) {
	  bool iPurity = highPurity->at(itrk);
	  float iTrkDz = trkDzFirstVtx->at(itrk);
	  float iTrkDzErr = trkDzErrFirstVtx->at(itrk);
	  float iTrkDxy = trkDxyFirstVtx->at(itrk);
	  float iTrkDxyErr = trkDxyErrFirstVtx->at(itrk);
	  float iPt = trkPt->at(itrk);
	  float iEta = trkEta->at(itrk);
	  float iPhi = trkPhi->at(itrk);
	  
	  if ( iPurity != true )
	    continue;
	  if ( iPt < 0.05)
	    continue;
	  if ( fabs(iTrkDz/iTrkDzErr) > 3.0 ) 
	    continue;
	  if ( fabs(iTrkDxy/iTrkDxyErr) > 3.0 ) 
	    continue;

	  output_trkPt[output_nTrk] = iPt;
	  output_trkEta[output_nTrk] = iEta;
	  output_trkPhi[output_nTrk] = iPhi;
	  
	  output_nTrk++;
	  //	  cout << "iTrkDz/iTrkDzErr = " << iTrkDz/iTrkDzErr << endl;

	  
	  //	  std::cout << "Entry " << i << ", Track " << j << ": trkPt * trkEta = " << trkPtEta << std::endl;
	}
	 
      }
      // Save the event in the output tree
      outTree->Fill();
      outTrkTree->Fill();
    }
    
    // 🔹 6. Write and Close Output File
    outFile->cd();
    outTree->Write();
    outTrkTree->Write();
    outFile->Close();
    
        std::cout << "Output saved to output_tree_L1SignleJet8trig.root" << std::endl;
    
}
