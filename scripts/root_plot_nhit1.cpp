// Macro to plot root branches into a pdf file
// Arguments:
// 1. input_file: name of the input root file
// 2. output_file: name of the output pdf file
// date: 10.04.2025
// author: Tamasi Kar
// to compile:
// g++ -o plotnhit2 root_plot_nhit2.cpp `root-config --cflags --glibs`   
// ./plotgttx out/output_460_3hits.root gttf_run00460_3hits.pdf     
// ./plotnhit2 out/output_good_3hits.root gttf_good_3hits.pdf     
//------------------------------------------------//
#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TMath.h"
#include "TGaxis.h"
#include "TPaletteAxis.h"

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <vector>

#include "plot_props.h"
//std::string input_tree_name = "tree";
std::string input_tree_name = "TrackTree";


void WriteToPdf(TTree *trk, std::string output_file_name)
{
  int MaxEntries = 10000;
  MaxEntries = trk->GetEntries();
  /////////////////////////////////////////////////
  // cuts for triplets
  // const double chi2cut=49;
  const double qualcut=9;

  // cuts for quadruplets
  //  const double chi2cut=249;
  // const double qualcut=19;

  ////////////////////////////////////////////////


  char char_cut[1023];
  char char_plot[1023];
  //char char_entry[1023];

  int arr_layer[5]={1,1,0,0,1};
  int arr_ladder[5]={1,2,1,6,7};

  //  sprintf(char_cut,"comb_type==-1 && qual_track<%.0f",qualcut);  

  //int nevent=100000;
  //sprintf(char_entry,"%i",nevent);

  char out_file_open[1023];
  sprintf(out_file_open,"%s(",output_file_name.c_str());
  char out_file_[1023];
  sprintf(out_file_,"%s",output_file_name.c_str());
  char out_file_close[1023];
  sprintf(out_file_close,"%s)",output_file_name.c_str());

  TCanvas *S = new TCanvas();
  TGaxis::SetMaxDigits(2);
  gStyle->SetPadLeftMargin(0.09);
  gStyle->SetPadRightMargin(0.12);
  gStyle->SetOptStat(0);
  //gROOT->ForceStyle();
  //gStyle->SetPalette(kCMYK);
  //gStyle->SetPalette(kPastel);
  gStyle->SetPalette(kLightTemperature);
  //TColor::InvertPalette();
  Legends();


  // example binning  FIRST ONE
  int nbins = 100;
  double min = -50.0;
  double max = 50.0;

  S->Clear();
  S->Divide(3,1);
  //TH1F *h1 = new TH1F("h1","h1",nbins,min,max);
  S->cd(1);
  trk->Draw("hit_x","","");
  //h1->SetTitle("Hit X");
  //h1->GetXaxis()->SetTitle("Hit X [mm]");
  //h1->Draw();//Draw("hist");
  S->cd(2);
  trk->Draw("hit_y","","");
  //h1->SetTitle("Hit Y");
  //h1->GetXaxis()->SetTitle("Hit Y [mm]");
  //h1->Draw();
  S->cd(3);
  trk->Draw("hit_z","","");
  S->Print(out_file_open); // HERE OPEN
  //delete h1;

  nbins=300;
  int min_= -82.78; //(20.66 + 0.04) + (20.66mm*6 + 0.04*5) + (0.04 + 20.66)
  int max_= 82.78;
  int chip_min = 0;
  int chip_max = 8; // chips are from 1-6 (low edge 1 -- up edge 7) +-1 for visibility(white space) 
  int ladd_min = 0;
  int ladd_max = 10; // in total 8 ladders for inner layer. +-1 white space
  min = 0;
  max = (ladd_max) * 250; //max row


  // plot default without custom binning
  S->Clear();
  S->Divide(2,1);
  S->cd(1);
  gPad->SetLeftMargin(0.07);
  gPad->SetRightMargin(0.18);
  TH2F *h2_ = new TH2F("h2_","",nbins,min_,max_,nbins,min,max);
  trk->Draw("(hit_ladder_id + 1)*250 + hit_row:hit_z>>h2_","hit_layer_id == 0","colz ");
  //h2_->SetTitle("Hit row_global vs Hit z (inner vertex layer)");
  h2_->GetXaxis()->SetTitle("Hit z [mm]");
  h2_->GetYaxis()->SetTitle("Hit row global");
  h2_->GetYaxis()->SetTitleOffset(1.0);
  h2_->GetXaxis()->CenterTitle();
  h2_->GetYaxis()->CenterTitle();

  gPad->Update();
  TPaletteAxis *palette = (TPaletteAxis*)h2_->GetListOfFunctions()->FindObject("palette");
  // the following lines move the palette. Choose the values you need for the position.
  palette->SetX1NDC(0.91);
  palette->SetX2NDC(0.94);
  palette->SetY1NDC(0.1);
  palette->SetY2NDC(0.9);
  gPad->Modified();
  gPad->Update();

  TGaxis* axis_top = new TGaxis(min_, max, max_, max, chip_min, chip_max, 510, "-");
  axis_top->SetTitle("chip_id");
  axis_top->SetTitleOffset(1.17);
  axis_top->SetLabelOffset(0.02);
  axis_top->CenterTitle();
  //axis_top->SetLabelSize(0.08);
  axis_top->SetTickSize(0.01);
  axis_top->SetLabelFont(42);
  axis_top->SetTitleFont(42);
  axis_top->Draw();

  int nXGrid = (chip_max - chip_min) + 1;
  double tick_step = (max_ - min_)/nXGrid;
  for(int i = 2; i < nXGrid; ++i)
  {
    double x_ = chip_min + i*(chip_max -chip_min)/nXGrid;
    double x = min_ + (x_ - chip_min)/(chip_max - chip_min)*(max_ - min_);//i * tick_step;
    TLine* xgrid_line = new TLine(x, min, x, max);
    xgrid_line->SetLineStyle(2);
    xgrid_line->SetLineColor(kGray);
    xgrid_line->Draw("same");
  }

  //TGaxis* axis_right = new TGaxis(max_, max, max_, min, ladd_min, ladd_max, 510, "-L");
  TGaxis* axis_right = new TGaxis(max_, min, max_, max, ladd_min, ladd_max, 510, "+R");
  axis_right->SetTitle("ladder_id");
  axis_right->SetTitleOffset(0.8);
  axis_right->SetLabelOffset(0.04);
  axis_right->CenterTitle();
  axis_right->SetLabelFont(42);
  axis_right->SetTitleFont(42);
  //axis_right->SetLabelSize(0.08);
  //axis_right->SetTickSize(0.03);
  axis_right->Draw();

  int nYGrid = (ladd_max - ladd_min) + 1;
  tick_step = (max - min)/nYGrid;
  for(int i = 2; i < nYGrid; ++i)
  {
    double y_ = ladd_min + i*(ladd_max - ladd_min)/nYGrid;
    double y = min + (y_ - ladd_min)/(ladd_max - ladd_min)*(max - min);//i * tick_step;
    TLine* ygrid_line = new TLine(min_, y, max_, y);
    ygrid_line->SetLineStyle(2);
    ygrid_line->SetLineColor(kGray);
    ygrid_line->Draw("same");
  }
  leg->Draw();

  S->cd(2);
  ladd_max = 12;
  max = ladd_max *250;

  gPad->SetLeftMargin(0.08);
  gPad->SetRightMargin(0.17);

  TH2F *h2__ = new TH2F("h2__","",nbins,min_,max_,nbins,min,max);
  trk->Draw("(hit_ladder_id + 1)*250 + hit_row:hit_z>>h2__","hit_layer_id == 1","colz ");
  // h2__->SetTitle("Hit row_global vs Hit z (outer vertex layer)");
  h2__->GetXaxis()->SetTitle("Hit z [mm]");
  h2__->GetYaxis()->SetTitle("Hit row global");
  h2__->GetYaxis()->SetTitleOffset(1.0);
  h2__->GetXaxis()->CenterTitle();
  h2__->GetYaxis()->CenterTitle();

  gPad->Update();
  palette = (TPaletteAxis*)h2__->GetListOfFunctions()->FindObject("palette");
  // the following lines move the palette. Choose the values you need for the position.
  palette->SetX1NDC(0.91);
  palette->SetX2NDC(0.94);
  palette->SetY1NDC(0.1);
  palette->SetY2NDC(0.9);
  gPad->Modified();
  gPad->Update();

  axis_top = new TGaxis(min_, max, max_, max, chip_min, chip_max, 510, "-");
  axis_top->SetTitle("chip_id");
  axis_top->SetTitleOffset(1.19);
  axis_top->SetLabelOffset(0.02);
  axis_top->CenterTitle();
  axis_top->SetLabelFont(42);
  axis_top->SetTitleFont(42);
  axis_top->Draw();

  for(int i = 2; i < nXGrid; ++i)
  {
    double x_ = chip_min + i*(chip_max -chip_min)/nXGrid;
    double x = min_ + (x_ - chip_min)/(chip_max - chip_min)*(max_ - min_);//i * tick_step;
    TLine* xgrid_line = new TLine(x, min, x, max);
    xgrid_line->SetLineStyle(2);
    xgrid_line->SetLineColor(kGray);
    xgrid_line->Draw("same");
  }

  nYGrid = (ladd_max - ladd_min) + 1;
  tick_step = (max - min)/nYGrid;
  axis_right = new TGaxis(max_, min, max_, max, ladd_min, ladd_max, 510, "+R");
  axis_right->SetTitle("ladder_id");
  axis_right->SetTitleOffset(0.89);
  axis_right->SetLabelOffset(0.04);
  axis_right->CenterTitle();
  axis_right->SetLabelFont(42);
  axis_right->SetTitleFont(42);
  axis_right->Draw();

  for(int i = 2; i < nYGrid; ++i)
  {
    double y_ = ladd_min + i*(ladd_max - ladd_min)/nYGrid;
    double y = min + (y_ - ladd_min)/(ladd_max - ladd_min)*(max - min);//i * tick_step;
    TLine* ygrid_line = new TLine(min_, y, max_, y);
    ygrid_line->SetLineStyle(2);
    ygrid_line->SetLineColor(kGray);
    ygrid_line->Draw("same");
  }
  leg->Draw();
  S->Print(out_file_);//pg3
  delete h2_;
  delete h2__;

  // --------------- colz hit maps for two layers ----------------------//

  constexpr int nLayers = 2;
  int nLadders = 8;// change in loop to 10 if layer_id == 1
  constexpr int nChips = 3; // this is for half ladders and upstream.

  char hist_name[256];
  char hist_title[1023];

  for(int i = 0; i < nLayers; ++i)
  {
    if(i == 1) nLadders = 10;
    for(int j = 0; j < nLadders; ++j)
    {
      S->Clear();
      S->Divide(3,1);
      TH2F* hists[nChips];
      for(int k = 0; k < nChips; ++k)
      {
	sprintf(hist_name, "hChip%d", k+1);
	sprintf(hist_title, "Chip %d, Ladder %d, Layer %d; col; row", k+1, j+1, i+1);
	hists[k] = new TH2F(hist_name, hist_title, 256, -0.5, 255.5, 250, -0.5, 249.5);	
	gDirectory->Add(hists[k]);

	sprintf(char_cut,"hit_layer_id==%d && hit_ladder_id==%d && hit_chip_id==%d", i, j, k+1);

	trk->Draw(Form("hit_row:hit_col>>%s",hist_name),char_cut,"goff");

	S->cd(k+1);
	hists[k]->GetYaxis()->SetTitleOffset(1.0);
	hists[k]->GetXaxis()->CenterTitle();
	hists[k]->GetYaxis()->CenterTitle();
	hists[k]->Draw("colz");
	gPad->Modified();
	gPad->Update();
      }//nChip
      S->Update();
      S->Print(out_file_);
      for(int k = 0; k < nChips; ++k) delete hists[k];
    }//nLadder    
  }//nLayers

  //
  S->Clear();

  nbins=200;
  min = -50.0;
  max = 50.0;

  TH2F *h2 = new TH2F("h2","h2",nbins,min,max,nbins,min,max);
  trk->Draw("hit_y:hit_x>>h2","","",MaxEntries);
  h2->SetTitle("Hit Y vs Hit X");
  h2->GetXaxis()->SetTitle("Hit X [mm]");
  h2->GetYaxis()->SetTitle("Hit Y [mm]");
  h2->SetMarkerStyle(20);
  h2->SetMarkerSize(0.5);
  h2->SetMarkerColor(kRed);
  //h2->Draw("SCAT");
  h2->Draw("colz");
  S->Print(out_file_);// pg2
  delete h2; 

  nbins=100;
  min = -50.0;
  max = 50.0;
  // plot default without custom binning
  S->Clear();
  trk->Draw("hit_x:hit_z","","",MaxEntries);
  S->Print(out_file_);//pg3

  S->Clear();
  trk->Draw("hit_y:hit_z","","",MaxEntries);
  S->Print(out_file_);//pg3

  S->Clear();
  trk->Draw("hit_layer_id");
  S->Print(out_file_);

  S->Clear();
  trk->Draw("hit_layer_id:hit_ladder_id","","BOX ",MaxEntries);
  S->Print(out_file_);


  // ladders outer and inner layers
  S->Clear();
  S->Divide(2,1);
  S->cd(1);
  trk->Draw("hit_ladder_id","hit_layer_id==1");

  S->cd(2);
  trk->Draw("hit_ladder_id","hit_layer_id==0");
  S->Print(out_file_); 
  gDirectory->ls(); 
  gDirectory->Delete("htemp;*");
  gDirectory->Delete("Graph;*");
  gDirectory->ls();
  //
  S->Clear();
  S->Divide(3,2);
  S->cd(1);
  trk->Draw("hit_x:hit_z","hit_ladder_id==1&&hit_layer_id==1","",MaxEntries);
  S->cd(2);
  trk->Draw("hit_x:hit_z","hit_ladder_id==2&&hit_layer_id==1","",MaxEntries);
  S->cd(3);
  trk->Draw("hit_x:hit_z","hit_ladder_id==1&&hit_layer_id==0","",MaxEntries);

  S->cd(4);
  trk->Draw("hit_x:hit_z","hit_ladder_id==6&&hit_layer_id==0","",MaxEntries);
  S->cd(5);
  trk->Draw("hit_x:hit_z","hit_ladder_id==7&&hit_layer_id==1","",MaxEntries);
  S->Print(out_file_);
  gDirectory->ls(); 
  gDirectory->Delete("htemp;*");
  gDirectory->Delete("Graph;*");
  gDirectory->ls();

  //
  S->Clear();
  S->Divide(3,2);
  S->cd(1);
  trk->Draw("hit_x:hit_z","hit_ladder_id==1&&hit_layer_id==1","BOX ",MaxEntries);
  S->cd(2);
  trk->Draw("hit_x:hit_z","hit_ladder_id==2&&hit_layer_id==1","BOX ",MaxEntries);
  S->cd(3);
  trk->Draw("hit_x:hit_z","hit_ladder_id==1&&hit_layer_id==0","BOX ",MaxEntries);

  S->cd(4);
  trk->Draw("hit_x:hit_z","hit_ladder_id==6&&hit_layer_id==0","BOX ",MaxEntries);
  S->cd(5);
  trk->Draw("hit_x:hit_z","hit_ladder_id==7&&hit_layer_id==1","BOX ",MaxEntries);
  S->Print(out_file_); 
  gDirectory->ls(); 
  gDirectory->Delete("htemp;*");
  gDirectory->Delete("Graph;*");
  gDirectory->ls();


  ///////////////////////////////////

  S->Clear();

  min = 0.0;
  max = 1e6;
  nbins = 1000;
  TH1F *h4 = new TH1F("h4","h4",nbins,min,max);
  trk->Draw("event >>h4");
  h4->SetTitle("event");
  h4->GetXaxis()->SetTitle("Event");
  // set log scale
  //S->SetLogy();
  h4->Draw();//Draw("hist");
  S->Print(out_file_); // pg1
  delete h4;

  S->Clear();
  min = 0.0;
  max = 100;
  nbins = 100;
  TH1F *h5 = new TH1F("h5","h5",nbins,min,max);
  trk->Draw("(event%100)>>h5");
  h5->SetTitle("event");
  h5->GetXaxis()->SetTitle("Event (mod %100)");
  // set log scale
  //S->SetLogy();
  h5->Draw();//Draw("hist");
  S->Print(out_file_); 
  delete h5;

  S->Clear();
  trk->Draw("event:run_id","","BOX");
  S->Print(out_file_);

  gDirectory->Delete("htemp;*");
  gDirectory->Delete("Graph;*");   
  min = 0.0;
  max = 1.5e8;
  nbins = 1000;
  TH1F *h14 = new TH1F("h14","h14",nbins,min,max);
  trk->Draw("hit_timestamp >>h14");
  h14->SetTitle("hit ts");
  h14->GetXaxis()->SetTitle("Hit Timestamp (8ns)");
  // set log scale
  //S->SetLogy();
  h14->Draw();//Draw("hist");
  S->Print(out_file_); // pg1
  delete h14;
  min = 0.0;
  max = 100;
  nbins = 100;
  TH1F *h15 = new TH1F("h15","h15",nbins,min,max);
  trk->Draw("(hit_timestamp%100)>>h15");
  h15->SetTitle("hit ts");
  h15->GetXaxis()->SetTitle("Hit Timestamp (mod %100) (8ns)");
  // set log scale
  //S->SetLogy();
  h15->Draw();//Draw("hist");
  S->Print(out_file_); 
  delete h15;

  trk->Draw("run_id:hit_timeFPGA[0] >> h24_1","","BOX ");
  TH1F *h24_1 = (TH1F *)gPad->GetPrimitive("h24_1");
  h24_1->SetTitle("hit time (FPGA)");
  h24_1->GetXaxis()->SetTitle("Hit(1) Time FPGA (in 16us)");
  h24_1->GetYaxis()->SetTitle("Run number");

  S->Clear();
  h24_1->Draw("BOX");

  S->Print(out_file_);
  delete h24_1;

  S->Clear();
  min = 0.0;
  max = 100;
  nbins = 100;
  TH1F *h25 = new TH1F("h25","h25",nbins,min,max);
  trk->Draw("(hit_timeFPGA%100)>>h25");
  h25->SetTitle("hit time (FPGA)");
  h25->GetXaxis()->SetTitle("Hit Time FPGA (in 16us) (mod %100)");
  // set log scale
  //S->SetLogy();
  h25->Draw();//Draw("hist");
  S->Print(out_file_); 
  delete h25;

  gDirectory->Delete("htemp;*");
  gDirectory->Delete("Graph;*");   
  S->Clear();
  //TH2F *h3 = new TH2F("h3","h3",nbins,-max,-min,nbins,min,max);
  trk->Draw("event:hit_timestamp","event<100000","BOX ");
  S->Print(out_file_);

  gDirectory->Delete("htemp;*");
  gDirectory->Delete("Graph;*");   
  S->Clear();
  //TH2F *h3 = new TH2F("h3","h3",nbins,-max,-min,nbins,min,max);
  trk->Draw("event:hit_timeFPGA","event<100000","BOX ");
  S->Print(out_file_);

  gDirectory->Delete("htemp;*");
  gDirectory->Delete("Graph;*");   
  /**************************************/

  S->Clear();
  trk->Draw("frame_id");
  S->Print(out_file_);

  gDirectory->Delete("htemp;*");
  gDirectory->Delete("Graph;*");   
  S->Clear();
  trk->Draw("event:frame_id","","",MaxEntries);
  S->Print(out_file_);

  gDirectory->Delete("htemp;*");
  gDirectory->Delete("Graph;*");  


  // h3->SetTitle("Hit Y vs Hit X");
  // h3->GetXaxis()->SetTitle("Hit X [mm]");
  // h3->GetYaxis()->SetTitle("Hit Y [mm]");
  // h3->SetMarkerStyle(20);
  // h3->SetMarkerSize(1.5);
  // h3->Draw("SCAT");


  /**************************************/
  S->Clear();
  min = -5;
  max = 0;
  nbins = 100;
  TH1F *h9100 = new TH1F("h9100","h9100",nbins,min,max);
  trk->Draw("comb_type >> h9100","","");
  h9100->SetTitle("Hit Combination Type");
  h9100->GetXaxis()->SetTitle("Hit Combination Type");
  h9100->Draw();
  S->Print(out_file_);
  delete h9100;  
  // Combinatorics
  S->Clear();
  S->Divide(2,2);

  S->cd(1);
  gPad->SetLogy();
  min = 0.0;
  max = 300;
  nbins = 100;
  TH1F *h9101 = new TH1F("h9101","h9101",nbins,min,max);
  trk->Draw("ncomb2 >> h9101");
  h9101->SetTitle("Number of 2-hit combinations");
  h9101->GetXaxis()->SetTitle("2-hit combinations");
  h9101->Draw();

  S->cd(2);
  gPad->SetLogy();
  min = 0.0;
  max = 300;
  nbins = 100;
  TH1F *h9102 = new TH1F("h9102","h9102",nbins,min,max);
  trk->Draw("ncomb3 >> h9102");
  h9102->SetTitle("Number of 3-hit combinations");
  h9102->GetXaxis()->SetTitle("3-hit combinations");
  h9102->Draw();

  S->cd(3);
  gPad->SetLogy();
  min = 0.0;
  max = 300;
  nbins = 100;
  TH1F *h9103 = new TH1F("h9103","h9103",nbins,min,max);
  trk->Draw("ncomb4 >> h9103");
  h9103->SetTitle("Number of 4-hit combinations");
  h9103->GetXaxis()->SetTitle("4-hit combinations");
  // set log scale
  //S->SetLogy();
  h9103->Draw();

  S->Print(out_file_);

  delete h9101;  
  delete h9102;  
  delete h9103;  
  gDirectory->Delete("htemp;*");
  gDirectory->Delete("Graph;*"); 

  ///////////////////////////////////////////////////////
  // Number hits per cosmic layer
  // Log
  S->Clear();
  S->Divide(2,2);

  S->cd(1);
  gPad->SetLogy();
  trk->Draw("nhitCL0","","" );
  S->cd(2);
  gPad->SetLogy();
  trk->Draw("nhitCL1","","");
  S->cd(3);
  gPad->SetLogy();
  trk->Draw("nhitCL2","","");
  S->cd(4);
  gPad->SetLogy();
  trk->Draw("nhitCL3","","");

  S->Print(out_file_);

  gDirectory->ls(); 
  gDirectory->Delete("htemp;*");
  gDirectory->Delete("Graph;*");
  gDirectory->ls();
  ///////////////////////////////////////////////////////
  // Number hits per cosmic layer as function of z
  // Log
  S->Clear();
  S->Divide(2,2);

  S->cd(1);
  trk->Draw("nhitCL0:hit_z","hit_z>-1000&&hit_ladder_id<5&&hit_layer_id==1","BOX ", MaxEntries);
  S->cd(2);                                                                                   
  trk->Draw("nhitCL1:hit_z","hit_z>-1000&&hit_ladder_id<4&&hit_layer_id==0","BOX ", MaxEntries);
  S->cd(3);                                                                                  
  trk->Draw("nhitCL2:hit_z","hit_z>-1000&&hit_ladder_id>=4&&hit_layer_id==0","BOX ", MaxEntries);
  S->cd(4);                                                                                  
  trk->Draw("nhitCL3:hit_z","hit_z>-1000&&hit_ladder_id>=5&&hit_layer_id==1","BOX ", MaxEntries);

  S->Print(out_file_);

  gDirectory->ls(); 
  gDirectory->Delete("htemp;*");
  gDirectory->Delete("Graph;*");
  gDirectory->ls(); 
  ///////////////////////////////////////////////////////
  // Number hits per cosmic layer as function of z
  // Log
  S->Clear();
  S->Divide(2,2);

  S->cd(1);
  trk->Draw("nhitCL0:hit_z","comb_type==-1","BOX ", MaxEntries);
  S->cd(2);                                                  
  trk->Draw("nhitCL1:hit_z","comb_type==-2","BOX ", MaxEntries);
  S->cd(3);                                                  
  trk->Draw("nhitCL2:hit_z","comb_type==-3","BOX ", MaxEntries);
  S->cd(4);                                                  
  trk->Draw("nhitCL3:hit_z","comb_type==-4","BOX ", MaxEntries);

  S->Print(out_file_);

  gDirectory->Delete("htemp;*");
  gDirectory->Delete("Graph;*");
  ///////////////////////////////////////////////////////
  // Number hits per cosmic layer as function of hit_tot
  // Log
  S->Clear();
  S->Divide(2,2);

  S->cd(1);
  trk->Draw("nhitCL0:hit_tot","comb_type==-1","BOX ", MaxEntries);
  S->cd(2);                                                    
  trk->Draw("nhitCL1:hit_tot","comb_type==-2","BOX ", MaxEntries);
  S->cd(3);                                                    
  trk->Draw("nhitCL2:hit_tot","comb_type==-3","BOX ", MaxEntries);
  S->cd(4);                                                    
  trk->Draw("nhitCL3:hit_tot","comb_type==-4","BOX ", MaxEntries);

  S->Print(out_file_);

  gDirectory->Delete("htemp;*");
  gDirectory->Delete("Graph;*"); 

  ///////////////////////////////////////////////////////
  // Number hits per cosmic layer as function of hit_tot
  // Log
  S->Clear();
  S->Divide(2,2);

  S->cd(1);
  trk->Draw("nhitCL0:hit_tot","comb_type==-1&&nhitCL0<20","BOX ", MaxEntries);
  S->cd(2);                                                                
  trk->Draw("nhitCL1:hit_tot","comb_type==-2&&nhitCL1<20","BOX ", MaxEntries);
  S->cd(3);                                                                
  trk->Draw("nhitCL2:hit_tot","comb_type==-3&&nhitCL2<20","BOX ", MaxEntries);
  S->cd(4);                                                                
  trk->Draw("nhitCL3:hit_tot","comb_type==-4&&nhitCL3<20","BOX ", MaxEntries);

  S->Print(out_file_);

  gDirectory->Delete("htemp;*");
  gDirectory->Delete("Graph;*"); 

  ///////////////////////////////////////////////////////
  // Number hits per cosmic layer as function of ts2
  // Log
  S->Clear();
  S->Divide(2,2);

  S->cd(1);
  trk->Draw("nhitCL0:hit_ts2","comb_type==-1","BOX ", MaxEntries);
  S->cd(2);                                                    
  trk->Draw("nhitCL1:hit_ts2","comb_type==-2","BOX ", MaxEntries);
  S->cd(3);                                                    
  trk->Draw("nhitCL2:hit_ts2","comb_type==-3","BOX ", MaxEntries);
  S->cd(4);                                                    
  trk->Draw("nhitCL3:hit_ts2","comb_type==-4","BOX ", MaxEntries);

  S->Print(out_file_);

  gDirectory->Delete("htemp;*");
  gDirectory->Delete("Graph;*");  
  ///////////////////////////////////////////////////////
  // Number hits per cosmic layer as function of ts2
  // Log
  S->Clear();
  S->Divide(2,2);

  S->cd(1);
  trk->Draw("nhitCL0:hit_ts2","comb_type==-1&&nhitCL0<20","BOX ", MaxEntries);
  S->cd(2);                                                                
  trk->Draw("nhitCL1:hit_ts2","comb_type==-2&&nhitCL1<20","BOX ", MaxEntries);
  S->cd(3);                                                                
  trk->Draw("nhitCL2:hit_ts2","comb_type==-3&&nhitCL2<20","BOX ", MaxEntries);
  S->cd(4);                                                                
  trk->Draw("nhitCL3:hit_ts2","comb_type==-4&&nhitCL3<20","BOX ", MaxEntries);

  S->Print(out_file_);

  gDirectory->Delete("htemp;*");
  gDirectory->Delete("Graph;*"); 

  ///////////////////////////////////////////////////////
  //  hit_tot versus z
  S->Clear();
  S->Divide(2,2);

  S->cd(1);
  trk->Draw("hit_tot:hit_z","comb_type==-1&&hit_z>-1000","BOX ", MaxEntries);
  S->cd(2);                                                               
  trk->Draw("hit_tot:hit_z","comb_type==-2&&hit_z>-1000","BOX ", MaxEntries);
  S->cd(3);                                                               
  trk->Draw("hit_tot:hit_z","comb_type==-3&&hit_z>-1000","BOX ", MaxEntries);
  S->cd(4);                                                               
  trk->Draw("hit_tot:hit_z","comb_type==-4&&hit_z>-1000","BOX ", MaxEntries);

  S->Print(out_file_);

  gDirectory->Delete("htemp;*");
  gDirectory->Delete("Graph;*");  

  ///////////////////////////////////////////////////////
  // ts2 versus z
  S->Clear();
  S->Divide(2,2);

  S->cd(1);
  trk->Draw("hit_ts2:hit_z","comb_type==-1","BOX ", MaxEntries);
  S->cd(2);                                                  
  trk->Draw("hit_ts2:hit_z","comb_type==-2","BOX ", MaxEntries);
  S->cd(3);                                                  
  trk->Draw("hit_ts2:hit_z","comb_type==-3","BOX ", MaxEntries);
  S->cd(4);
  trk->Draw("hit_ts2:hit_z","comb_type==-4","BOX ", MaxEntries);

  S->Print(out_file_);

  gDirectory->Delete("htemp;*");
  gDirectory->Delete("Graph;*"); 

  ///////////////////////////////////////////////////////
  // ts2 versus z
  S->Clear();
  S->Divide(2,2);

  S->cd(1);
  trk->Draw("hit_ts2:hit_z","comb_type==-1&&hit_z>-1000","BOX ", MaxEntries);
  S->cd(2);                                                               
  trk->Draw("hit_ts2:hit_z","comb_type==-2&&hit_z>-1000","BOX ", MaxEntries);
  S->cd(3);                                                               
  trk->Draw("hit_ts2:hit_z","comb_type==-3&&hit_z>-1000","BOX ", MaxEntries);
  S->cd(4);
  trk->Draw("hit_ts2:hit_z","comb_type==-4&&hit_z>-1000","BOX ", MaxEntries);

  S->Print(out_file_);

  gDirectory->Delete("htemp;*");
  gDirectory->Delete("Graph;*");  

  ///////////////////////////////////////////////////////
  // Number hits per cosmic layer as function of run
  // Log
  S->Clear();
  S->Divide(2,2);

  S->cd(1);
  trk->Draw("nhitCL0:run_id","","BOX ", MaxEntries);
  S->cd(2);                                      
  trk->Draw("nhitCL1:run_id","","BOX ", MaxEntries);
  S->cd(3);                                      
  trk->Draw("nhitCL2:run_id","","BOX ", MaxEntries);
  S->cd(4);
  trk->Draw("nhitCL3:run_id","","BOX ", MaxEntries);

  S->Print(out_file_);

  gDirectory->Delete("htemp;*");
  gDirectory->Delete("Graph;*"); 
  ///////////////////////////////////////////////////////

  S->Clear();

  min = 0.0;
  max = 35;
  nbins = 35;
  TH1F *h9104 = new TH1F("h9104","h9104",nbins,min,max);
  trk->Draw("qual_track >> h9104",""," ");
  h9104->SetTitle("Track quality (edge quality hits encoded)");
  h9104->GetXaxis()->SetTitle("Track quality");
  // set log scale
  //S->SetLogy();
  h9104->Draw();//Draw("hist");
  S->Print(out_file_);

  delete h9104;  
  gDirectory->Delete("htemp;*");
  gDirectory->Delete("Graph;*");  
  nbins = 100;
  ///////////////////////////////////////////////////////
  // nommber combinations vs track quality  
  S->Clear();
  S->Divide(2,2);

  S->cd(1);
  trk->Draw("ncomb2:qual_track","","BOX ",MaxEntries);
  S->cd(2);
  trk->Draw("ncomb3:qual_track","","BOX ",MaxEntries);
  S->cd(3);
  trk->Draw("ncomb4:qual_track","","BOX ",MaxEntries);

  S->Print(out_file_);

  gDirectory->Delete("htemp;*");
  gDirectory->Delete("Graph;*");  
  ///////////////////////////////////////////////////////
  // Hit Multiplicity vs Timestamp  
  S->Clear();
  S->Divide(2,2);

  S->cd(1);
  trk->Draw("nhitCL0:frame_id","","BOX", MaxEntries);
  S->cd(2);                                        
  trk->Draw("nhitCL1:frame_id","","BOX", MaxEntries);
  S->cd(3);                                        
  trk->Draw("nhitCL2:frame_id","","BOX", MaxEntries);
  S->cd(4);
  trk->Draw("nhitCL3:frame_id","","BOX", MaxEntries);

  S->Print(out_file_);

  gDirectory->Delete("htemp;*");
  gDirectory->Delete("Graph;*"); 

  ///////////////////////////////////////////////////////
  // number combinations vs Timestamp  
  S->Clear();
  S->Divide(2,2);

  S->cd(1);
  trk->Draw("ncomb2:frame_id","","BOX", MaxEntries);
  S->cd(2);
  trk->Draw("ncomb3:frame_id","","BOX", MaxEntries);
  S->cd(3);
  trk->Draw("ncomb4:frame_id","","BOX", MaxEntries);

  S->Print(out_file_);


  ///////////////////////////////////////////////////////



  /////////////////////////////
  // tot

  sprintf(char_cut,"comb_type==-1");  
  trk->Draw("hit_tot>>htot0all",char_cut,"");
  TH1F *htot0all = (TH1F *)gPad->GetPrimitive("htot0all");
  htot0all->GetXaxis()->SetTitle("tot/128 [ns]");

  sprintf(char_cut,"comb_type==-1 && qual_track<%.0f",qualcut);  
  trk->Draw("hit_tot>>htot0core",char_cut,"");
  TH1F *htot0core = (TH1F *)gPad->GetPrimitive("htot0core");
  htot0core->GetXaxis()->SetTitle("tot/128 [ns]");

  sprintf(char_cut,"comb_type==-2");  
  trk->Draw("hit_tot>>htot1all",char_cut,"");
  TH1F *htot1all = (TH1F *)gPad->GetPrimitive("htot1all");
  htot1all->GetXaxis()->SetTitle("tot/128 [ns]");

  sprintf(char_cut,"comb_type==-2 && qual_track<%.0f",qualcut);  
  trk->Draw("hit_tot>>htot1core",char_cut,"");
  TH1F *htot1core = (TH1F *)gPad->GetPrimitive("htot1core");
  htot1core->GetXaxis()->SetTitle("tot/128 [ns]");

  sprintf(char_cut,"comb_type==-3");  
  trk->Draw("hit_tot>>htot2all",char_cut,"");
  TH1F *htot2all = (TH1F *)gPad->GetPrimitive("htot2all");
  htot2all->GetXaxis()->SetTitle("tot/128 [ns]");

  sprintf(char_cut,"comb_type==-3 && qual_track<%.0f",qualcut);
  trk->Draw("hit_tot>>htot2core",char_cut,"");
  TH1F *htot2core = (TH1F *)gPad->GetPrimitive("htot2core");
  htot2core->GetXaxis()->SetTitle("tot/128 [ns]");

  sprintf(char_cut,"comb_type==-4");  
  trk->Draw("hit_tot>>htot3all",char_cut,"");
  TH1F *htot3all = (TH1F *)gPad->GetPrimitive("htot3all");
  htot3all->GetXaxis()->SetTitle("tot/128 [ns]");

  sprintf(char_cut,"comb_type==-4 && qual_track<%.0f",qualcut);  
  trk->Draw("hit_tot>>htot3core",char_cut,"");
  TH1F *htot3core = (TH1F *)gPad->GetPrimitive("htot3core");
  htot3core->GetXaxis()->SetTitle("tot/128 [ns]");


  S->Clear();
  S->Divide(2,2);

  S->cd(1);
  htot0all->Draw();
  htot0core->SetLineColor(kBlue);
  htot0core->Draw("same");

  S->cd(2);
  htot1all->Draw();
  htot1core->SetLineColor(kBlue);
  htot1core->Draw("same");

  S->cd(3);
  htot2all->Draw();
  htot2core->SetLineColor(kBlue);
  htot2core->Draw("same");

  S->cd(4);
  htot3all->Draw();
  htot3core->SetLineColor(kBlue);
  htot3core->Draw("same");

  S->Print(out_file_);

  gDirectory->Delete("htemp;*");
  gDirectory->Delete("Graph;*");
  // now with logy

  S->Clear();
  S->Divide(2,2);

  S->cd(1);
  gPad->SetLogy();
  htot0all->Draw();
  htot0core->SetLineColor(kBlue);
  htot0core->Draw("same");

  S->cd(2);
  gPad->SetLogy();
  htot1all->Draw();
  htot1core->SetLineColor(kBlue);
  htot1core->Draw("same");

  S->cd(3);
  gPad->SetLogy();
  htot2all->Draw();
  htot2core->SetLineColor(kBlue);
  htot2core->Draw("same");

  S->cd(4);
  gPad->SetLogy();
  htot3all->Draw();
  htot3core->SetLineColor(kBlue);
  htot3core->Draw("same");

  S->Print(out_file_);
  delete htot0all;
  delete htot1all;
  delete htot2all;
  delete htot3all;
  delete htot0core;
  delete htot1core;
  delete htot2core;
  delete htot3core;

  gDirectory->Delete("htemp;*");
  gDirectory->Delete("Graph;*"); 
  //////////////////////////////////////    
  // tot vs ladder
  S->Clear();
  gPad->SetLogy(0);
  S->Divide(2,2);
  S->cd(1);
  trk->Draw("hit_ladder_id + 1:hit_tot","comb_type==-1","colz",MaxEntries);
  S->cd(2);
  trk->Draw("hit_ladder_id + 1:hit_tot","comb_type==-2","colz",MaxEntries);
  S->cd(3);
  trk->Draw("hit_ladder_id + 1:hit_tot","comb_type==-3","colz",MaxEntries);
  S->cd(4);
  trk->Draw("hit_ladder_id + 1:hit_tot","comb_type==-4","colz",MaxEntries);
  S->Print(out_file_); 

  gDirectory->Delete("htemp;*");
  gDirectory->Delete("Graph;*"); 
  S->Clear();
  S->Divide(2,2);
  S->cd(1);
  sprintf(char_cut,"comb_type==-1 && qual_track<%.0f",qualcut);
  trk->Draw("hit_ladder_id + 1:hit_tot", char_cut ,"colz",MaxEntries);
  S->cd(2);
  sprintf(char_cut,"comb_type==-2 && qual_track<%.0f",qualcut);
  trk->Draw("hit_ladder_id + 1:hit_tot", char_cut ,"colz",MaxEntries);
  S->cd(3);
  sprintf(char_cut,"comb_type==-3 && qual_track<%.0f",qualcut);
  trk->Draw("hit_ladder_id + 1:hit_tot", char_cut ,"colz",MaxEntries);
  S->cd(4);
  sprintf(char_cut,"comb_type==-4 && qual_track<%.0f",qualcut);
  trk->Draw("hit_ladder_id + 1:hit_tot", char_cut ,"colz",MaxEntries);
  S->Print(out_file_); 
  gDirectory->Delete("htemp;*");
  gDirectory->Delete("Graph;*"); 
  // Empty page fpor convinience

  S->Print(out_file_close); 

}

int main(int argc, char *argv[])
{
  bool debug = false;
  std::string run_number;
  std::string input_file;
  std::string output_file;
  if(argc == 3)
  {
    input_file = argv[1];
    output_file = argv[2];
  } 
  else
{      
    std::cout << "Usage: ./plotroot  <input_file.root> <output_file.pdf> " << std::endl;
    return 1;
  }



  // READ ROOT FILE
  TFile *f = new TFile(input_file.c_str(), "READ");
  if(f->IsZombie())
  {
    std::cout << "Error: File " << input_file << " not found." << std::endl;
    return 1;
  }
  TTree *trk = (TTree*)f->Get(input_tree_name.c_str());
  if(trk == nullptr)
  {
    std::cout << "Error: Tree " << input_tree_name << " not found in file " << input_file << "." << std::endl;
    return 1;
  }
  std::cout<<"Input Tree has: " << trk->GetEntries() << " entries." << std::endl;
  trk->SetBranchStatus("C3D_glob", 0);
  trk->SetBranchStatus("sigmaC3D_glob", 0);
  gDirectory->Delete("TripletTree;*"); 
  gDirectory->Delete("TimingtTree;*"); 
  std::cout<<"Writing to pdf ....... \n";
  WriteToPdf(trk, output_file);
  f->Close();

  std::cout<<"Done writing to pdf ....... \n";
  return 0;
}
