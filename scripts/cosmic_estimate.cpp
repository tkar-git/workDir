// -------------------------------------------------------- //
// Estimate the number of cosmics from simulations
// compile:
// g++ -o est_cosmics cosmic_estimate.cpp `root-config --cflags --glibs` -DDEBUG

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TLine.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <array>

//------------------ CONSTANTS ---------------------------------------//

const float cosmic_rate = 100.0; // 100Hz/m2
const float one_min = 60.;		 // 1min
const float sensor_effs = 1.0;	 // assuming 50%
const int n_hists = 3;
const char* trk_tree_name = "TrackTree"; // name of gttfOutput TTree
char cut[1023] = "";
std::array<float,2> chi2_cuts = {1e3, 2e2};

const char* sort_dir = "/testbeamdata/mu3e2025/data/sim/cosmic";
const char* trk_dir = "/testbeamdata/mu3e2025/analysis/kar/repos/tripletSim/out";
const char* out_dir = "../pdf";

char sort_file_name[1023];
char trk_file_name[1023];
char pdf_output[1023];


//-----------------------------------------------------------------//

std::array<TH1F*, n_hists> hist_list;

std::array<std::array<int,n_hists>, 2> cosmic_estimate(TTree* &t_trk, int &ncombs)
{
	char branch_name[256];
	snprintf(branch_name, sizeof(branch_name), "ncomb%d>>h", ncombs);

	t_trk->Draw(branch_name);
	TH1F* h = (TH1F*)gPad->GetPrimitive("h");
	int max_ncombX = h->FindLastBinAbove(0.); 
	int min = 1;
	int nBins_ncombX = max_ncombX - min;

#ifdef DEBUG
	std::cout << " branch name: " << branch_name <<std::endl;
	std::cout << " max_ncombX: " << max_ncombX <<std::endl;
#endif


	std::array<int, n_hists> n_frames;
	std::array<int, n_hists> n_tracks;


	for(int i = 0; i < n_hists; ++i)
	{
		char hist_name[256];
		char hist_title[1023];
		if(i == 0) 
		{
			snprintf(hist_name, sizeof(hist_name), "hncomb%d", ncombs);
			snprintf(hist_title, sizeof(hist_title), "%d-hit combinations", ncombs);

		}
		else
		{
			snprintf(hist_name, sizeof(hist_name), "hncomb%d_chi2_%.01f", ncombs, chi2_cuts[i-1]);
			snprintf(hist_title, sizeof(hist_title), "%d-hit combinations with chi2 < %.01f; ncombX;", ncombs, chi2_cuts[i-1]);
			snprintf(cut, sizeof(cut), "chi2_glob < %.01f", chi2_cuts[i-1]);
		}

		snprintf(branch_name, sizeof(branch_name), "ncomb%d>>%s", ncombs, hist_name);

#ifdef DEBUG
		std::cout << " branch name: " << branch_name <<std::endl;
		std::cout << " hist_name: " << hist_name <<std::endl;
		std::cout << " hist_title: " << hist_title <<std::endl;
#endif


		hist_list[i] = new TH1F(hist_name, hist_title, nBins_ncombX, min, max_ncombX);
		n_tracks[i] = t_trk->Draw(branch_name, cut);
		n_frames[i] = 0;
		for(int j = 1; j < nBins_ncombX+1; ++j)
		{
			n_frames[i] += hist_list[i]->GetBinContent(j)/j;
#ifdef DEBUG
			std::cout<< "bin entries: " <<hist_list[i]->GetBinContent(j) << std::endl;
#endif
		}
#ifdef DEBUG
		std::cout << "n_frames [" << i << "] =  "<< n_frames[i] << std::endl;
#endif
	}

	return {n_frames,n_tracks};	
}

int main(int argc, char** argv)
{
	int ncombs; // number of hit combinations
	int run_number;

	if( argc != 3 ) 
	{
		std::cout<< "[ERROR]: Please pass the run number and the number of hit combinations as arguments" <<std::endl;
		std::cout<< "[Usage]: ./est_cosmics <run_number> <ncombs>" <<std::endl;
	}
	else 
	{
		run_number = std::stoi(argv[1]);
		ncombs = std::stoi(argv[2]);
	}
	sprintf(sort_file_name, "%s/run%05d.root", sort_dir, run_number);
	sprintf(trk_file_name, "%s/gttfOut_run%05d_%dhits.root", trk_dir, run_number, ncombs);
	sprintf(pdf_output, "%s/cosmic_estimate_run%05d_%dhits.pdf", out_dir, run_number, ncombs);

	TFile *f_srt = new TFile(sort_file_name,"READ");
	if(f_srt->IsZombie())
	{
		std::cout<<"[ERROR:] Sort File not found" <<std::endl;
		delete f_srt;
		return 1;
	}
	TTree *t_srt = (TTree*)f_srt->Get("mu3e");
	int tot_frames = t_srt->GetEntries();
	std::cout<<"Number of frames after sort: " << tot_frames << std::endl;

	TFile *f_trk = new TFile(trk_file_name,"READ");
	if(f_trk->IsZombie())
	{
		std::cout<<"[ERROR:] Input File not found" <<std::endl;
		delete f_trk;
		return 1;
	}
	TTree *t_trk = (TTree*)f_trk->Get(trk_tree_name);
	auto total_trks = t_trk->GetEntries();

#ifdef DEBUG
	std::cout<<"Number of tracks: " << total_trks << std::endl;
#endif
	std::array<std::array<int, n_hists>,2> nFrames_tracks = cosmic_estimate(t_trk, ncombs);
	std::array<float, n_hists> ncosmics;
	std::array<float, n_hists> nframes;
	std::array<float, n_hists> ntracks;
	float chi2[n_hists] = {-1, chi2_cuts[0], chi2_cuts[1]};

	// Draw Table
	TCanvas *c = new TCanvas("c","cut_flow");
	TLine line;
	TLatex lat;
	float TEXT_SIZE = 0.03;
	float TEXT_ALIGN_1 = 12;
	lat.SetTextFont(42);
	lat.SetTextSize(TEXT_SIZE);
	lat.SetTextAlign(TEXT_ALIGN_1);
	float Ylat, X1lat;
	float step = 0.086;
	float xstep = 0.15;
	Ylat = 0.96;
	X1lat = 0.05;

	// global variables
	lat.DrawLatex(X1lat, Ylat, Form("#total_{frames}: %d", tot_frames));
	lat.DrawLatex(1.5*xstep + X1lat, Ylat, "avg cosmic rate: 100 Hz/m^{2}");
	lat.DrawLatex(3.5*xstep + X1lat, Ylat, "sensor efficiency: 100%");
	line.SetLineStyle(7);
	line.DrawLine(X1lat, Ylat - 0.5*step, X1lat+0.75, Ylat-0.5*step);
	// table header
	lat.DrawLatex(X1lat, Ylat-1*step, "#chi2_{cuts}");
	line.SetLineStyle(1);
	line.DrawLine(X1lat, Ylat - 1.5*step, X1lat+0.75, Ylat-1.5*step);
	lat.DrawLatex(xstep + X1lat, Ylat-1*step, "#tracks");
	lat.DrawLatex(2*xstep + X1lat, Ylat-1*step, "#frames");
	lat.DrawLatex(3*xstep + X1lat, Ylat-1*step, "#frac{#frames}{#total_{frames}}");
	lat.DrawLatex(4*xstep + X1lat, Ylat-1*step, "estimate/minute");


	for(int i = 0; i < n_hists; ++i)
	{
		ntracks[i] = nFrames_tracks[1][i];
		nframes[i] = nFrames_tracks[0][i];
		ncosmics[i] = (nFrames_tracks[0][i]/(float)tot_frames) * cosmic_rate * one_min * sensor_effs;
		std::cout << "Number of tracks with cosmic  for chi2 cut = " << chi2[i] << " is " << nFrames_tracks[1][i] <<std::endl;
		std::cout << "Number of frames with cosmic  for chi2 cut = " << chi2[i] << " is " << nFrames_tracks[0][i] <<std::endl;
		std::cout << "Number of cosmics expected in 10 mins assuming total sensor efficiency of  " << sensor_effs << " is " << ncosmics[i]*10 <<std::endl;
		std::cout<<std::endl;

		delete hist_list[i];
		lat.DrawLatex(X1lat, Ylat-(i+2)*step, Form("%0.1f",chi2[i]));
		line.SetLineStyle(1);
		line.DrawLine(X1lat, Ylat - (i+2.5)*step, X1lat+0.75, Ylat-(i+2.5)*step);
		lat.DrawLatex(xstep + X1lat, Ylat-(i+2)*step, Form("%0.0f", ntracks[i]));
		lat.DrawLatex(2*xstep + X1lat, Ylat-(i+2)*step, Form("%0.0f", nframes[i]));
		lat.DrawLatex(3*xstep + X1lat, Ylat-(i+2)*step, Form("%0.2e", nframes[i]/(float)tot_frames));
		lat.DrawLatex(4*xstep + X1lat, Ylat-(i+2)*step, Form("%0.2f", ncosmics[i]));

	}

	c->Print(pdf_output);
	delete t_trk;
	delete f_trk;
	return 0;
}
