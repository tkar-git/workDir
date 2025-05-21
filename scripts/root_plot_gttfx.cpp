// Macro to plot root branches into a pdf file
// Arguments:
// 1. input_file: name of the input root file
// 2. output_file: name of the output pdf file
// date: 10.04.2025
// author: Tamasi Kar
// to compile:
// g++ -o plotgttf3 root_plot_gttfx.cpp `root-config --cflags --glibs` -DTRIHITS
// g++ -o plotgttf4 root_plot_gttfx.cpp `root-config --cflags --glibs`
//
// ./plotgttx out/output_460_3hits.root gttf_run00460_3hits.pdf     
// ./plotgttfx out/output_good_3hits.root gttf_good_3hits.pdf     
//------------------------------------------------//
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TMath.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TGaxis.h"
#include "TPaletteAxis.h"

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <vector>

#include "plot_props.h"

std::string input_tree_name = "TrackTree";


void WriteToPdf(TTree *trk, std::string output_file_name)
{
  int MaxEntries = 10000;
  MaxEntries = trk->GetEntries();
  /////////////////////////////////////////////////
#ifdef TRIHITS
  // cuts for triplets (3nhit)
  const double chi2cut=49;
  const double qualcut=9;
#else
  // cuts for quadruplets (4nhit)
  const double chi2cut=249;
  const double qualcut=19;
#endif
  ////////////////////////////////////////////////

  
  char char_cut[1023];
  char char_text[1023];
  
  char out_file_open[1023];
  sprintf(out_file_open,"%s(",output_file_name.c_str());
  char out_file_[1023];
  sprintf(out_file_,"%s",output_file_name.c_str());
  char out_file_close[1023];
  sprintf(out_file_close,"%s)",output_file_name.c_str());
  
  TCanvas *S = new TCanvas();

  gStyle->SetPadLeftMargin(0.09);
  gStyle->SetPadRightMargin(0.15);
  gStyle->SetOptStat(0);
  //gStyle->SetPalette(kInvertedDarkBodyRadiator);
  TColor::InvertPalette();
  Legends();

  
  // example binning
  int nbins = 100;
  double min = -50.0;
  double max = 50.0;
  
  TH1F *h1 = new TH1F("h1","h1",nbins,min,max);
  trk->Draw("hit_x>>h1");
  h1->SetTitle("Hit x");
  h1->GetXaxis()->SetTitle("Hit x [mm]");
  h1->GetXaxis()->CenterTitle();
  // set log scale
  //S->SetLogy();
  h1->Draw();//Draw("hist");
  leg->Draw();
  S->Print(out_file_open); // pg1
  
  
  trk->SetMarkerStyle(20);
  trk->SetMarkerSize(0.5);
  
  nbins=300;
  min = -50.0;
  max = 50.0;
  
  TH2F *h2 = new TH2F("h2","h2",nbins,min,max,nbins,min,max);
  trk->Draw("hit_y:hit_x>>h2","","",MaxEntries);
  h2->SetTitle("Hit y vs Hit z");
  h2->GetYaxis()->SetTitle("Hit y [mm]");
  h2->GetXaxis()->SetTitle("Hit x [mm]");
  h2->GetXaxis()->CenterTitle();
  h2->GetYaxis()->CenterTitle();
  h2->Draw("colz");
  leg->Draw();
  S->Print(out_file_);// pg2
  
  nbins=300;
  int min_= -82.78; //(20.66 + 0.04) + (20.66mm*6 + 0.04*5) + (0.04 + 20.66)
  int max_= 82.78;
  min = -TMath::Pi();
  max = TMath::Pi();
  int chip_min = 0;
  int chip_max = 8; // chips are from 1-6 (low edge 1 -- up edge 7) +-1 for visibility(white space) 
  int ladd_min = 0;
  int ladd_max = 10; // in total 8 ladders for inner layer. +-1 white space

  // plot default without custom binning
  S->Clear();
  S->Divide(2,1);
  S->cd(1);
  TH2F *h2_ = new TH2F("h2_","h2_",nbins,min_,max_,nbins,min,max);
  trk->Draw("atan2(hit_y, hit_x):hit_z>>h2_","hit_layer_id == 0","colz",MaxEntries);
  h2_->SetTitle("Hit phi vs Hit z (inner layer)");
  h2_->GetXaxis()->SetTitle("Hit z [mm]");
  h2_->GetYaxis()->SetTitle("Hit phi [rad]");
  h2_->GetXaxis()->CenterTitle();
  h2_->GetYaxis()->CenterTitle();
  gPad->Update();
  TPaletteAxis *palette = (TPaletteAxis*)h2_->GetListOfFunctions()->FindObject("palette");

  // the following lines move the palette. Choose the values you need for the position.
  palette->SetX1NDC(0.95);
  palette->SetX2NDC(0.98);
  palette->SetY1NDC(0.1);
  palette->SetY2NDC(0.9);
  gPad->Modified();
  gPad->Update();

  TGaxis* axis_top = new TGaxis(min_, max, max_, max, chip_min, chip_max, 510, "-");
  axis_top->SetTitle("chip_id");
  axis_top->SetTitleOffset(0.6);
  axis_top->SetLabelOffset(-0.01);
  axis_top->CenterTitle();
  //axis_top->SetLabelSize(0.08);
  //axis_top->SetTickSize(0.03);
  axis_top->Draw();
  int nXGrid = (chip_max - chip_min) + 1;
  double tick_step = (max_ - min_)/nXGrid;
  for(int i = 0; i <= nXGrid; ++i)
  {
    double x_ = chip_min + i*(chip_max -chip_min)/nXGrid;
    double x = min_ + (x_ - chip_min)/(chip_max - chip_min)*(max_ - min_);//i * tick_step;
	TLine* xgrid_line = new TLine(x, min, x, max);
	xgrid_line->SetLineStyle(1);
	xgrid_line->SetLineColor(kGray);
	xgrid_line->Draw("same");
  }

  TGaxis* axis_right = new TGaxis(max_, max, max_, min, ladd_min, ladd_max, 510, "-L");
  axis_right->SetTitle("ladder_id");
  axis_right->SetTitleOffset(0.75);
  axis_right->SetLabelOffset(0.01);
  axis_right->CenterTitle();
  axis_right->Draw();
  int nYGrid = (ladd_max - ladd_min) + 1;
  tick_step = (max - min)/nYGrid;
  for(int i = 0; i <= nYGrid; ++i)
  {
    double y_ = ladd_min + i*(ladd_max - ladd_min)/nYGrid;
    double y = min + (y_ - ladd_min)/(ladd_max - ladd_min)*(max - min);//i * tick_step;
    TLine* ygrid_line = new TLine(min_, y, max_, y);
    ygrid_line->SetLineStyle(1);
    ygrid_line->SetLineColor(kGray);
    ygrid_line->Draw("same");
  }
  leg->Draw();

  S->cd(2);
  TH2F *h2__ = new TH2F("h2__","h2__",nbins,min_,max_,nbins,min,max);
  trk->Draw("atan2(hit_y, hit_x):hit_z>>h2__","hit_layer_id == 1","colz",MaxEntries);
  h2__->SetTitle("Hit x vs Hit z (outer layer)");
  h2__->GetXaxis()->SetTitle("Hit z [mm]");
  h2__->GetYaxis()->SetTitle("Hit phi [rad]");
  h2__->GetYaxis()->SetTitleOffset(0.8);
  h2__->GetXaxis()->CenterTitle();
  h2__->GetYaxis()->CenterTitle();
  gPad->Update();
  palette = (TPaletteAxis*)h2__->GetListOfFunctions()->FindObject("palette");

  // the following lines move the palette. Choose the values you need for the position.
  palette->SetX1NDC(0.95);
  palette->SetX2NDC(0.98);
  palette->SetY1NDC(0.1);
  palette->SetY2NDC(0.9);
  gPad->Modified();
  gPad->Update();
  axis_top->Draw();
  for(int i = 0; i <= nXGrid; ++i)
  {
    double x_ = chip_min + i*(chip_max -chip_min)/nXGrid;
    double x = min_ + (x_ - chip_min)/(chip_max - chip_min)*(max_ - min_);//i * tick_step;
	TLine* xgrid_line = new TLine(x, min, x, max);
	xgrid_line->SetLineStyle(1);
	xgrid_line->SetLineColor(kGray);
	xgrid_line->Draw("same");
  }
  axis_right->Draw();
  ladd_max = 12;
  nYGrid = (ladd_max - ladd_min) + 1;
  tick_step = (max - min)/nYGrid;
  for(int i = 0; i <= nYGrid; ++i)
  {
    double y_ = ladd_min + i*(ladd_max - ladd_min)/nYGrid;
    double y = min + (y_ - ladd_min)/(ladd_max - ladd_min)*(max - min);//i * tick_step;
    TLine* ygrid_line = new TLine(min_, y, max_, y);
    ygrid_line->SetLineStyle(1);
    ygrid_line->SetLineColor(kGray);
    ygrid_line->Draw("same");
  }
  leg->Draw();
  S->Print(out_file_);//pg3
  //TH2F *h2_ = new TH2F("h2_","h2_",nbins,min,max,nbins,min,max);
  //trk->Draw("hit_y:hit_x>>h2_","ncomb4==1","same",MaxEntries);
  //h2_->SetTitle("Hit Y vs Hit X");
  //h2_->GetXaxis()->SetTitle("Hit X [mm]");
  //h2_->GetYaxis()->SetTitle("Hit Y [mm]");
  //h2_->SetMarkerStyle(20);
  //h2_->SetMarkerSize(0.3);
  //h2_->SetMarkerColor(kBlack);
  //h2_->Draw("SAME");
  ////h2_->Draw("colz");
  //S->Print(out_file_);// pg2
  
  min = -50.0;
  max = 50.0;
  TH1F* h1D = nullptr;
  TH2F* h2D = nullptr;
  // plot default without custom binning
  S->Clear();
  //TH2F *h3 = new TH2F("h3","h3",nbins,-max,-min,nbins,min,max);
  trk->Draw("hit_x:hit_z>>htemp(200, -70, 0.0, 200, -10, 25)","","colz",MaxEntries);
  h2D = (TH2F*)gPad->GetPrimitive("htemp");
  h2D->SetTitle("Hit x vs Hit z");
  h2D->GetXaxis()->SetTitle("Hit z [mm]");
  h2D->GetYaxis()->SetTitle("Hit x [mm]");
  h2D->GetXaxis()->CenterTitle();
  h2D->GetYaxis()->CenterTitle();
  // h3->SetMarkerStyle(20);
  // h3->SetMarkerSize(1.5);
  // h3->Draw("SCAT");
  leg->Draw();
  S->Print(out_file_);//pg3
  
  S->Clear();
  trk->Draw("hit_layer_id:hit_ladder_id","","BOX");
  leg->Draw();
  S->Print(out_file_);

  /*
  S->Clear();
  trk->Draw("hit_layer_id");
  S->Print(out_file_);
  */
  
  // check ladder combination for comb_type==1
  /*
  S->Clear();
  trk->Draw("hit_layer_id[0]:hit_ladder_id[0]","comb_type==1","BOX");
  S->Print(out_file_);
  
  S->Clear();
  trk->Draw("hit_layer_id[1]:hit_ladder_id[1]","comb_type==1","BOX");
  S->Print(out_file_);
  
  S->Clear();
  trk->Draw("hit_layer_id[2]:hit_ladder_id[2]","comb_type==1","BOX");
  S->Print(out_file_);
  */
  
  /*
  S->Divide(2,1);
  S->Clear();
  S->cd(1);
  trk->Draw("hit_ladder_id","hit_layer_id==1");
  
  S->cd(2);
  trk->Draw("hit_ladder_id","hit_layer_id==0");
  S->Print(out_file_);
  */
  
  //
  /*
  S->Clear();
  trk->Draw("hit_ladder_id[0]");
  S->Print(out_file_);
  
  S->Clear();
  trk->Draw("hit_ladder_id[1]");
  S->Print(out_file_);
  
  S->Clear();
  trk->Draw("hit_ladder_id[2]");
  S->Print(out_file_);
  */

  S->Clear();
  S->Divide(3,2);
  S->cd(1);
  trk->Draw("hit_x:hit_z","hit_ladder_id==1&&hit_layer_id==1","colz",MaxEntries);
  S->cd(2);
  trk->Draw("hit_x:hit_z","hit_ladder_id==2&&hit_layer_id==1","colz",MaxEntries);
  S->cd(3);
  trk->Draw("hit_x:hit_z","hit_ladder_id==1&&hit_layer_id==0","colz",MaxEntries);

  S->cd(4);
  trk->Draw("hit_x:hit_z","hit_ladder_id==6&&hit_layer_id==0","colz",MaxEntries);
  S->cd(5);
  trk->Draw("hit_x:hit_z","hit_ladder_id==7&&hit_layer_id==1","",MaxEntries);
  S->Print(out_file_);
  
  
  ///////////////////////////////////
  S->Clear();
  trk->Draw("run_id:event","","BOX");
  S->Print(out_file_);

  /////////////////////////

  trk->Draw("run_id:frame_id >> h24_1","","BOX");
  TH1F *h24_1 = (TH1F *)gPad->GetPrimitive("h24_1");
  h24_1->SetTitle("frame ID");
  h24_1->GetXaxis()->SetTitle("frame ID (in 8ns)");
  h24_1->GetYaxis()->SetTitle("Run number");
  
  S->Clear();
  h24_1->Draw("BOX");
  S->Print(out_file_);

  
  //
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

  min = 0.0;
  max = 100;
  nbins = 100;
  TH1F *h5 = new TH1F("h5","h5",nbins,min,max);
  trk->Draw("(event%100)>>h5");
  h5->SetTitle("event");
  h5->GetXaxis()->SetTitle("Event (mod %100)");
  // set log scale
  //S->SetLogy();

  S->Clear();
  S->Divide(2,1);
  S->cd(1);
  gPad->SetLogy();
  h4->Draw();//Draw("hist");
  S->cd(2);
  gPad->SetLogy();
  h5->Draw();//Draw("hist");

  S->Print(out_file_); 



  
  /////////////////////////////
  
  S->Clear();
  min = 0.0;
  max = 1.5e8;
  nbins = 1000;
  TH1F *h14 = new TH1F("h14","h14",nbins,min,max);
  trk->Draw("hit_timestamp >>h14");
  h14->SetTitle("hit ts");
  h14->GetXaxis()->SetTitle("Hit Timestamp");
  
  min = 0.0;
  max = 100;
  nbins = 100;
  TH1F *h15 = new TH1F("h15","h15",nbins,min,max);
  trk->Draw("(hit_timestamp%100)>>h15");
  h15->SetTitle("hit ts");
  h15->GetXaxis()->SetTitle("Hit Timestamp (mod %100)");
  // set log scale
  //S->SetLogy();

  S->Clear();
  S->Divide(2,1);

  S->cd(1);
  gPad->SetLogy();
  h14->Draw();
  S->cd(2);
  gPad->SetLogy();
  h15->Draw();

  S->Print(out_file_); 

  /////////////////////////
  min = 0.0;
  max = 1e7;
  nbins = 1000;
  TH1F *h24 = new TH1F("h24","h24",nbins,min,max);
  trk->Draw("hit_timeFPGA >>h24");
  h24->SetTitle("hit time (FPGA)");
  h24->GetXaxis()->SetTitle("Hit Time FPGA (in 16us)");
  // set log scale
  //S->SetLogy();
  h24->Draw();//Draw("hist");

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
  
  S->Clear();
  S->Divide(2,1);

  S->cd(1);
  gPad->SetLogy();
  h24->Draw();
  S->cd(2);
  gPad->SetLogy();
  h25->Draw();

  S->Print(out_file_); 

  
  
  ////////////////  
  
  S->Clear();
  //TH2F *h3 = new TH2F("h3","h3",nbins,-max,-min,nbins,min,max);
  trk->Draw("event:hit_timestamp","event<100000","BOX");
  S->Print(out_file_);
  
  S->Clear();
  //TH2F *h3 = new TH2F("h3","h3",nbins,-max,-min,nbins,min,max);
  trk->Draw("event:hit_timeFPGA","event<100000","BOX");
  S->Print(out_file_);
  
  //  event timestamp correlation of 5 sensors
  /*
  S->Clear();
  S->Divide(3,2);

  S->cd(1);
  trk->Draw("event:hit_timeFPGA[0]","event<100000 && hit_layer_id[0]==1 && hit_ladder_id[0]==1");
  S->cd(2);
  trk->Draw("event:hit_timeFPGA[0]","event<100000 && hit_layer_id[0]==1 && hit_ladder_id[0]==2");
  S->cd(3);
  trk->Draw("event:hit_timeFPGA[1]","event<100000 && hit_layer_id[1]==0 && hit_ladder_id[1]<=4");
  S->cd(4);
  trk->Draw("event:hit_timeFPGA[1]","event<100000 && hit_layer_id[1]==0 && hit_ladder_id[1]>4");
  S->cd(5);
  trk->Draw("event:hit_timeFPGA[2]","event<100000 && hit_layer_id[2]==1");

  S->Print(out_file_);
  */
  /**************************************/
  
  S->Clear();
  S->Divide(2,1);
  S->cd(1);
  trk->Draw("frame_id");
  S->cd(2);
  trk->Draw("event:frame_id");
  S->Print(out_file_);


  ///////////////////////////////////////////////////////
  // number combinations vs FPGA time  
  S->Clear();
  S->Divide(2,2);

  S->cd(1);
  gPad->SetLogy();
  trk->Draw("ncomb3:hit_timeFPGA","","BOX");
  S->cd(2);
  gPad->SetLogy();
  trk->Draw("ncomb4:hit_timeFPGA","","BOX");
  S->cd(3);
  gPad->SetLogy();
  trk->Draw("ncomb2:hit_timeFPGA","","BOX");
  S->cd(4);
  gPad->SetLogy();
  trk->Draw("(nhitCL0+nhitCL1+nhitCL2+nhitCL3):hit_timeFPGA","","BOX");

  S->Print(out_file_);
  
  // number combinations vs frame_id  
  
  S->Clear();
  S->Divide(2,2);

  S->cd(1);
  gPad->SetLogy();
  trk->Draw("ncomb3:frame_id","","BOX");
  S->cd(2);
  gPad->SetLogy();
  trk->Draw("ncomb4:frame_id","","BOX");
  S->cd(3);
  gPad->SetLogy();
  trk->Draw("ncomb2:frame_id","","BOX");
  S->cd(4);
  gPad->SetLogy();
  trk->Draw("(nhitCL0+nhitCL1+nhitCL2+nhitCL3):frame_id","","BOX");
  S->Print(out_file_);
  
 
  S->cd(1);
  gPad->SetLogy();
  trk->Draw("ncomb3:frame_id","","colz");
  S->cd(2);
  gPad->SetLogy();
  trk->Draw("ncomb4:frame_id","","colz");
  S->cd(3);
  gPad->SetLogy();
  trk->Draw("ncomb2:frame_id","","colz");
  S->cd(4);
  gPad->SetLogy();
  trk->Draw("(nhitCL0+nhitCL1+nhitCL2+nhitCL3):frame_id","","colz");
  S->Print(out_file_); 
  
  // h3->SetTitle("Hit Y vs Hit X");
  // h3->GetXaxis()->SetTitle("Hit X [mm]");
  // h3->GetYaxis()->SetTitle("Hit Y [mm]");
  // h3->SetMarkerStyle(20);
  // h3->SetMarkerSize(1.5);
  // h3->Draw("SCAT");
  
  
  /**************************************/
  S->Clear();
  min = 0.0;
  max = 6;
  nbins = 100;
  TH1F *h9101 = new TH1F("h9101","h9101",nbins,min,max);
  trk->Draw("comb_type >> h9101");
  h9101->SetTitle("Hit Combination Type");
  h9101->GetXaxis()->SetTitle("Hit Combination Type");
  h9101->Draw();
  S->Print(out_file_);
  
  // Combinatorics
  
  S->Clear();
  S->Divide(2,1);

  S->cd(1);
  gPad->SetLogy();
  min = 0.0;
  max = 30;
  nbins = 100;
  TH1F *h9102 = new TH1F("h9102","h9102",nbins,min,max);
  trk->Draw("ncomb3 >> h9102");
  h9102->SetTitle("Number of 3-hit combinations");
  h9102->GetXaxis()->SetTitle("3-hit combinations");
  h9102->Draw();
  
  S->cd(2);
  gPad->SetLogy();
  min = 0.0;
  max = 30;
  nbins = 100;
  TH1F *h9103 = new TH1F("h9103","h9103",nbins,min,max);
  trk->Draw("ncomb4 >> h9103");
  h9103->SetTitle("Number of 4-hit combinations");
  h9103->GetXaxis()->SetTitle("4-hit combinations");
  // set log scale
  //S->SetLogy();
  h9103->Draw();

  S->Print(out_file_);
  
  ///////////////////////////////////////////////////////
  
  S->Clear();
  gPad->SetLogy();
  
  min = 0.0;
  max = 35;
  nbins = 35;
  TH1F *h9104 = new TH1F("h9104","h9104",nbins,min,max);
  trk->Draw("qual_track >> h9104");
  h9104->SetTitle("Track quality (edge quality hits encoded)");
  h9104->GetXaxis()->SetTitle("Track quality");
  // set log scale
  //S->SetLogy();
  h9104->Draw();//Draw("hist");
  S->Print(out_file_);
  

  ///////////////////////////////////////////////////////
  // nommber combinations vs track quality  
  S->Clear();
  S->Divide(2,2);

  S->cd(1);
  gPad->SetLogy();
  trk->Draw("ncomb3:qual_track","","BOX");
  S->cd(2);
  gPad->SetLogy();
  trk->Draw("ncomb4:qual_track","","BOX");
  S->cd(3);
  gPad->SetLogy();
  trk->Draw("ncomb2:qual_track","","BOX");
  gPad->SetLogy();
  S->cd(4);
  trk->Draw("nhitCL0+nhitCL1+nhitCL2+nhitCL3:qual_track","","BOX");

  S->Print(out_file_);
  
  
  ///////////////////////////////////////////////////////
  
  S->Clear();
  S->Divide(2,2);
  
  S->cd(1);
  trk->Draw("t_10");
  S->cd(2);
  trk->Draw("t_10","abs(t_10)<1e7");
  S->cd(3);
  trk->Draw("t_10","abs(t_10)<1e5");
  S->cd(4);
  trk->Draw("t_10","abs(t_10)<1e3");
  
  S->Print(out_file_);
  
  //
  S->Clear();
  S->Divide(2,2);
  
  S->cd(1);
  trk->Draw("t_20");
  S->cd(2);
  trk->Draw("t_20","abs(t_20)<1e7");
  S->cd(3);
  trk->Draw("t_20","abs(t_20)<1e5");
  S->cd(4);
  trk->Draw("t_20","abs(t_20)<1e3");
  
  S->Print(out_file_);
  
  //
  S->Clear();
  S->Divide(2,2);
  
  S->cd(1);
  trk->Draw("t_30");
  S->cd(2);
  trk->Draw("t_30","abs(t_30)<1e7");
  S->cd(3);
  trk->Draw("t_30","abs(t_30)<1e5");
  S->cd(4);
  trk->Draw("t_30","abs(t_30)<1e3");
  
  S->Print(out_file_);
  
  /////////////////////////////////////
  // Not filled yet in tree
  /*
    S->Clear();
    S->Divide(2,2);
    S->cd(1);
    trk->Draw("t_21");
    S->cd(2);
    trk->Draw("t_21","abs(t_21)<1e7");
    S->cd(3);
    trk->Draw("t_21","abs(t_21)<1e5");
    S->cd(4);
    trk->Draw("t_21","abs(t_21)<1e3");
    S->Print(out_file_);
    
    S->Clear();
    S->Divide(2,2);
    
    S->cd(1);
    trk->Draw("t_31");
    S->cd(2);
    trk->Draw("t_31","abs(t_31)<1e7");
    S->cd(3);
    trk->Draw("t_31","abs(t_31)<1e5");
    S->cd(4);
    trk->Draw("t_31","abs(t_31)<1e3");
    S->Print(out_file_);
  */
  
  //
  S->Clear();
  S->Divide(2,2);
  
  S->cd(1);
  trk->Draw("t_32");
  S->cd(2);
  trk->Draw("t_32","abs(t_32)<1e7");
  S->cd(3);
  trk->Draw("t_32","abs(t_32)<1e5");
  S->cd(4);
  trk->Draw("t_32","abs(t_32)<1e3");
  
  S->Print(out_file_);
  
  //
  
  ///////////////////////////////////////////////////////
    
  //     Time difference cosmic layers 1 - 0
  S->Clear();
  nbins = 100;
  min = -400;
  max = 400;
  S->SetLogy(0);

  TH1F *h101 = new TH1F("h101","h101",nbins,min,max);
  trk->Draw("t_10 >> h101");
  h101->SetTitle("Time difference cosmic layers 1 - 0");
  h101->GetXaxis()->SetTitle("t_hit_1 - t_hit_0 in timestamps (8ns)");
  h101->Draw();//Draw("hist");
  S->Print(out_file_);

  ///
  
  sprintf(char_cut,"qual_track<%f",qualcut);
  sprintf(char_text,"Time difference in cosmic layers 1 - 0 (qual_track<%.0f)",qualcut);  
  TH1F *h1101 = new TH1F("h1101","h1101",nbins,min,max);
  trk->Draw("t_10 >> h1101",char_cut);
  h1101->SetTitle(char_text);
  h1101->GetXaxis()->SetTitle("t_hit_1 - t_hit_0 in timestamps (8ns)");
  h1101->Draw();//Draw("hist");
  S->Print(out_file_);

  sprintf(char_cut,"chi2_glob<%f",chi2cut);
  sprintf(char_text,"Time difference in cosmic layers 1 - 0 (chi2_glob<%.0f)",chi2cut);  
  TH1F *h1102 = new TH1F("h1102","h1102",nbins,min,max);
  trk->Draw("t_10 >> h1102",char_cut);
  h1102->SetTitle(char_text);
  h1102->GetXaxis()->SetTitle("t_hit_1 - t_hit_0 in timestamps (8ns)");
  h1102->Draw();//Draw("hist");
  S->Print(out_file_);
  
  sprintf(char_cut,"qual_track<%f&&chi2_glob<%f",qualcut,chi2cut);
  TH1F *h1103 = new TH1F("h1103","h1103",nbins,min,max);
  trk->Draw("t_10 >> h1103",char_cut);
  h1103->SetTitle("Time difference cosmic layers 1 - 0 (cuts combined)");
  h1103->GetXaxis()->SetTitle("t_hit_1 - t_hit_0 in timestamps (8ns)");
  h1103->Draw();//Draw("hist");
  S->Print(out_file_);
  
  // overlay with ALL
  h101->SetLineColor(kBlack);
  h101->SetLineWidth(1);
  h101->Draw();
  
  h1101->SetLineColor(kBlue);
  h1101->SetLineWidth(1);
  h1101->Draw("same");
  h1102->SetLineColor(kRed);
  h1102->SetLineWidth(1);
  h1102->Draw("same");
  h1103->SetLineColor(kViolet);
  h1103->SetLineWidth(2);
  h1103->Draw("same");
  
  TLegend *leg101 =  new TLegend(0.11,0.6,0.42,0.96);
  leg101->SetFillStyle(0);
  leg101->SetBorderSize(0);
  leg101->SetHeader("","C");
  leg101->AddEntry(h101,"All","l");
  sprintf(char_text,"qual_track<%.0f",qualcut);
  leg101->AddEntry(h1101,char_text,"l");
  sprintf(char_text,"chi2_glob<%.0f",chi2cut);
  leg101->AddEntry(h1102,char_text,"l");
  sprintf(char_text,"qual_track<%.0f & chi2_glob<%.0f",qualcut,chi2cut);
  leg101->AddEntry(h1103,char_text,"l");
  leg101->Draw();
  
  S->Print(out_file_);

  // overlay without ALL
  h1101->SetLineColor(kBlue);
  h1101->SetLineWidth(1);
  h1101->Draw();
  h1102->SetLineColor(kRed);
  h1102->SetLineWidth(1);
  h1102->Draw("same");
  h1103->SetLineColor(kViolet);
  h1103->SetLineWidth(2);
  h1103->Draw("same");
  
  leg101->Draw();
  S->Print(out_file_);


  //     Time difference cosmic layers 2 - 0
  nbins = 100;
  TH1F *h201 = new TH1F("h201","h201",nbins,min,max);
  trk->Draw("t_20 >> h201");
  h201->SetTitle("Time difference cosmic layers 2 - 0");
  h201->GetXaxis()->SetTitle("t_hit_2 - t_hit_0 in timestamps (8ns)");
  h201->Draw();//Draw("hist");
  S->Print(out_file_);
  
  sprintf(char_cut,"qual_track<%f",qualcut);
  sprintf(char_text,"Time difference in cosmic layers 2 - 0 (qual_track<%.0f)",qualcut);  
  TH1F *h1201 = new TH1F("h1201","h1201",nbins,min,max);
  trk->Draw("t_20 >> h1201",char_cut);
  h1201->SetTitle(char_text);
  h1201->GetXaxis()->SetTitle("t_hit_2 - t_hit_0 in timestamps (8ns)");
  h1201->Draw();//Draw("hist");
  S->Print(out_file_);

  sprintf(char_cut,"chi2_glob<%f",chi2cut);
  sprintf(char_text,"Time difference in cosmic layers 2 - 0 (chi2_glob<%.0f)",chi2cut);  
  TH1F *h1202 = new TH1F("h1202","h1202",nbins,min,max);
  trk->Draw("t_20 >> h1202",char_cut);
  h1202->SetTitle(char_text);
  h1202->GetXaxis()->SetTitle("t_hit_2 - t_hit_0 in timestamps (8ns)");
  h1202->Draw();//Draw("hist");
  S->Print(out_file_);
  
  sprintf(char_cut,"qual_track<%f&&chi2_glob<%f",qualcut,chi2cut);
  TH1F *h1203 = new TH1F("h1203","h1203",nbins,min,max);
  trk->Draw("t_20 >> h1203",char_cut);
  h1203->SetTitle("Time difference cosmic layers 2 - 0 (cuts combined)");
  h1203->GetXaxis()->SetTitle("t_hit_2 - t_hit_0 in timestamps (8ns)");
  h1203->Draw();//Draw("hist");
  S->Print(out_file_);
  
  // overlay with ALL
  h201->SetLineColor(kBlack);
  h201->SetLineWidth(1);
  h201->Draw();
  
  h1201->SetLineColor(kBlue);
  h1202->SetLineWidth(1);
  h1201->Draw("same");
  h1202->SetLineColor(kRed);
  h1203->SetLineWidth(1);
  h1202->Draw("same");
  h1203->SetLineColor(kViolet);
  h1203->SetLineWidth(2);
  h1203->Draw("same");
  
  TLegend *leg201 =  new TLegend(0.11,0.6,0.42,0.96);
  leg201->SetFillStyle(0);
  leg201->SetBorderSize(0);
  leg201->SetHeader("","C");
  leg201->AddEntry(h201,"All","l");
  sprintf(char_text,"qual_track<%.0f",qualcut);
  leg201->AddEntry(h1201,char_text,"l");
  sprintf(char_text,"chi2_glob<%.0f",chi2cut);
  leg201->AddEntry(h1202,char_text,"l");
  sprintf(char_text,"qual_track<%.0f & chi2_glob<%.0f",qualcut,chi2cut);
  leg201->AddEntry(h1203,char_text,"l");
  leg201->Draw();
  
  S->Print(out_file_);

  // overlay without ALL
  h1201->SetLineColor(kBlue);
  h1202->SetLineWidth(1);
  h1201->Draw();
  h1202->SetLineColor(kRed);
  h1203->SetLineWidth(1);
  h1202->Draw("same");
  h1203->SetLineColor(kViolet);
  h1203->SetLineWidth(2);
  h1203->Draw("same");
  
  leg201->Draw();
  S->Print(out_file_);
  
  
  //     Time difference cosmic layers 3 - 0  (variable cuts)
  nbins = 100;
  TH1F *h301 = new TH1F("h301","h301",nbins,min,max);
  trk->Draw("t_30 >> h301");
  h301->SetTitle("Time difference cosmic layers 3 - 0");
  h301->GetXaxis()->SetTitle("t_hit_3 - t_hit_0 in timestamps (8ns)");
  h301->Draw();//Draw("hist");
  S->Print(out_file_);
  
  sprintf(char_cut,"qual_track<%f",qualcut);
  sprintf(char_text,"Time difference in cosmic layers 3 - 0 (qual_track<%.0f)",qualcut);  
  TH1F *h1301 = new TH1F("h1301","h1301",nbins,min,max);
  trk->Draw("t_30 >> h1301",char_cut);
  h1301->SetTitle(char_text);
  h1301->GetXaxis()->SetTitle("t_hit_3 - t_hit_0 in timestamps (8ns)");
  h1301->Draw();//Draw("hist");
  S->Print(out_file_);

  sprintf(char_cut,"chi2_glob<%f",chi2cut);
  sprintf(char_text,"Time difference in cosmic layers 3 - 0 (chi2_glob<%.0f)",chi2cut);  
  TH1F *h1302 = new TH1F("h1302","h1302",nbins,min,max);
  trk->Draw("t_30 >> h1302",char_cut);
  h1302->SetTitle(char_text);
  h1302->GetXaxis()->SetTitle("t_hit_3 - t_hit_0 in timestamps (8ns)");
  h1302->Draw();//Draw("hist");
  S->Print(out_file_);
  
  sprintf(char_cut,"qual_track<%f&&chi2_glob<%f",qualcut,chi2cut);
  TH1F *h1303 = new TH1F("h1303","h1303",nbins,min,max);
  trk->Draw("t_30 >> h1303",char_cut);
  h1303->SetTitle("Time difference cosmic layers 3 - 0 (cuts combined)");
  h1303->GetXaxis()->SetTitle("t_hit_3 - t_hit_0 in timestamps (8ns)");
  h1303->Draw();//Draw("hist");
  S->Print(out_file_);
  
  // overlay with ALL
  h301->SetLineColor(kBlack);
  h301->SetLineWidth(1);
  h301->Draw();
  
  h1301->SetLineColor(kBlue);
  h1302->SetLineWidth(1);
  h1301->Draw("same");
  h1302->SetLineColor(kRed);
  h1303->SetLineWidth(1);
  h1302->Draw("same");
  h1303->SetLineColor(kViolet);
  h1303->SetLineWidth(2);
  h1303->Draw("same");
  
  TLegend *leg301 =  new TLegend(0.11,0.6,0.42,0.96);
  leg301->SetFillStyle(0);
  leg301->SetBorderSize(0);
  leg301->SetHeader("","C");
  leg301->AddEntry(h301,"All","l");
  sprintf(char_text,"qual_track<%.0f",qualcut);
  leg301->AddEntry(h1301,char_text,"l");
  sprintf(char_text,"chi2_glob<%.0f",chi2cut);
  leg301->AddEntry(h1302,char_text,"l");
  sprintf(char_text,"qual_track<%.0f & chi2_glob<%.0f",qualcut,chi2cut);
  leg301->AddEntry(h1303,char_text,"l");
  leg301->Draw();
  
  S->Print(out_file_);

  // overlay without ALL
  h1301->SetLineColor(kBlue);
  h1302->SetLineWidth(1);
  h1301->Draw();
  h1302->SetLineColor(kRed);
  h1303->SetLineWidth(1);
  h1302->Draw("same");
  h1303->SetLineColor(kViolet);
  h1303->SetLineWidth(2);
  h1303->Draw("same");
  
  leg301->Draw();
  S->Print(out_file_);
  
  
  ////////////
  //     Time difference cosmic layers 3 - 2  (variable cuts)
  nbins = 100;
  TH1F *h321 = new TH1F("h321","h321",nbins,min,max);
  trk->Draw("t_32 >> h321");
  h321->SetTitle("Time difference cosmic layers 3 - 2");
  h321->GetXaxis()->SetTitle("t_hit_3 - t_hit_2 in timestamps (8ns)");
  h321->Draw();//Draw("hist");
  S->Print(out_file_);
  
  sprintf(char_cut,"qual_track<%f",qualcut);
  sprintf(char_text,"Time difference in cosmic layers 3 - 2 (qual_track<%.0f)",qualcut);  
  TH1F *h1321 = new TH1F("h1321","h1321",nbins,min,max);
  trk->Draw("t_32 >> h1321",char_cut);
  h1321->SetTitle(char_text);
  h1321->GetXaxis()->SetTitle("t_hit_3 - t_hit_2 in timestamps (8ns)");
  h1321->Draw();//Draw("hist");
  S->Print(out_file_);

  sprintf(char_cut,"chi2_glob<%f",chi2cut);
  sprintf(char_text,"Time difference in cosmic layers 3 - 2 (chi2_glob<%.0f)",chi2cut);  
  TH1F *h1322 = new TH1F("h1322","h1322",nbins,min,max);
  trk->Draw("t_32 >> h1322",char_cut);
  h1322->SetTitle(char_text);
  h1322->GetXaxis()->SetTitle("t_hit_3 - t_hit_2 in timestamps (8ns)");
  h1322->Draw();//Draw("hist");
  S->Print(out_file_);
  
  sprintf(char_cut,"qual_track<%f&&chi2_glob<%f",qualcut,chi2cut);
  TH1F *h1323 = new TH1F("h1323","h1323",nbins,min,max);
  trk->Draw("t_32 >> h1323",char_cut);
  h1323->SetTitle("Time difference cosmic layers 3 - 2 (cuts combined)");
  h1323->GetXaxis()->SetTitle("t_hit_3 - t_hit_2 in timestamps (8ns)");
  h1323->Draw();//Draw("hist");
  S->Print(out_file_);
  
  // overlay with ALL
  h321->SetLineColor(kBlack);
  h321->SetLineWidth(1);
  h321->Draw();
  
  h1321->SetLineColor(kBlue);
  h1321->SetLineWidth(1);
  h1321->Draw("same");
  h1322->SetLineColor(kRed);
  h1322->SetLineWidth(1);
  h1322->Draw("same");
  h1323->SetLineColor(kViolet);
  h1323->SetLineWidth(2);
  h1323->Draw("same");
  
  TLegend *leg321 =  new TLegend(0.11,0.6,0.42,0.96);
  leg321->SetFillStyle(0);
  leg321->SetBorderSize(0);
  leg321->SetHeader("","C");
  leg321->AddEntry(h321,"All","l");
  sprintf(char_text,"qual_track<%.0f",qualcut);
  leg321->AddEntry(h1321,char_text,"l");
  sprintf(char_text,"chi2_glob<%.0f",chi2cut);
  leg321->AddEntry(h1322,char_text,"l");
  sprintf(char_text,"qual_track<%.0f & chi2_glob<%.0f",qualcut,chi2cut);
  leg321->AddEntry(h1323,char_text,"l");
  leg321->Draw();
  
  S->Print(out_file_);
  
  // overlay without ALL
  h1321->SetLineColor(kBlue);
  h1321->SetLineWidth(1);
  h1321->Draw();
  h1322->SetLineColor(kRed);
  h1322->SetLineWidth(1);
  h1322->Draw("same");
  h1323->SetLineColor(kViolet);
  h1323->SetLineWidth(2);
  h1323->Draw("same");
  
  leg321->Draw();
  S->Print(out_file_);
  

  ///////////////////////////////////////////////////////
  // TQ - chi2 ccorrelation
  S->Clear();
  S->Divide(2,2);
  S->cd(1);
  sprintf(char_cut,"qual_track<%f&&chi2_glob<%f && abs(t_10)<400",qualcut,chi2cut);
  trk->Draw("run_id:t_10",char_cut,"BOX");
  S->cd(2);
  sprintf(char_cut,"qual_track<%f&&chi2_glob<%f && abs(t_20)<400",qualcut,chi2cut);
  trk->Draw("run_id:t_20",char_cut,"BOX");
  S->cd(3);
  sprintf(char_cut,"qual_track<%f&&chi2_glob<%f && abs(t_30)<400",qualcut,chi2cut);
  trk->Draw("run_id:t_30",char_cut,"BOX");
  S->cd(4);
  sprintf(char_cut,"qual_track<%f&&chi2_glob<%f && abs(t_32)<400",qualcut,chi2cut);
  trk->Draw("run_id:t_32",char_cut,"BOX");

  S->Print(out_file_);
  

  ///////////////////////////////////////////////////////
  // Sensor Dependence
  S->Clear();
  S->Divide(2,2);
  S->cd(1);
  sprintf(char_cut,"qual_track<%f&&chi2_glob<%f && abs(t_10)<400",qualcut,chi2cut);
  trk->Draw("hit_ladder_id[0]:t_10",char_cut,"BOX");
  S->cd(2);
  sprintf(char_cut,"qual_track<%f&&chi2_glob<%f && abs(t_20)<400",qualcut,chi2cut);
  trk->Draw("hit_ladder_id[0]:t_20",char_cut,"BOX");
  S->cd(3);
  sprintf(char_cut,"qual_track<%f&&chi2_glob<%f && abs(t_30)<400",qualcut,chi2cut);
  trk->Draw("hit_ladder_id[0]:t_30",char_cut,"BOX");
  S->cd(4);
  sprintf(char_cut,"qual_track<%f&&chi2_glob<%f && abs(t_32)<400",qualcut,chi2cut);
  trk->Draw("hit_ladder_id[0]:t_32",char_cut,"BOX");

  S->Print(out_file_);
  
  ///////////////////////////////////////////////////////
  // Multiplicity Dependence
  S->Clear();
  S->Divide(2,2);
  S->cd(1);
  sprintf(char_cut,"qual_track<%f&&chi2_glob<%f && abs(t_10)<400",qualcut,chi2cut);
  trk->Draw("ncomb3:t_10",char_cut,"BOX");
  S->cd(2);
  sprintf(char_cut,"qual_track<%f&&chi2_glob<%f && abs(t_20)<400",qualcut,chi2cut);
  trk->Draw("ncomb3:t_20",char_cut,"BOX");
  S->cd(3);
  sprintf(char_cut,"qual_track<%f&&chi2_glob<%f && abs(t_30)<400",qualcut,chi2cut);
  trk->Draw("ncomb3:t_30",char_cut,"BOX");
  S->cd(4);
  sprintf(char_cut,"qual_track<%f&&chi2_glob<%f && abs(t_32)<400",qualcut,chi2cut);
  trk->Draw("ncomb3:t_32",char_cut,"BOX");

  S->Print(out_file_);
  


  ///////////////////////////////////////////////////////
  
  gStyle->SetOptStat(0);

  S->Clear();
  S->Divide(2,1);

  S->cd(1);
  // special
  nbins = 200;
  TH1F *h106 = new TH1F("h106","h106",nbins,min,max);
  trk->Draw("t_10 >> h106","hit_ladder_id[0]==1");
  h106->SetTitle("Time difference in cosmic layers 1-0 (ladder 2-2)");
  h106->GetXaxis()->SetTitle("t_hit_1 - t_hit_0 in timestamps (8ns)");
  h106->Draw();//Draw("hist");
  
  S->cd(2);
  nbins = 200;
  TH1F *h107 = new TH1F("h107","h107",nbins,min,max);
  trk->Draw("t_10 >> h107","hit_ladder_id[0]==2");
  h107->SetTitle("Time difference in cosmic layers 1-0 (ladder 2-3)");
  h107->GetXaxis()->SetTitle("t_hit_1 - t_hit_0 in timestamps (8ns)");
  h107->Draw();//Draw("hist");

  S->Print(out_file_);
  
  /////////////////////////////////////////////////////////////
  // timing difference with chi2 cut      chi2<1000
  S->Clear();

  nbins = 200;
  TH1F *h2101 = new TH1F("h2101","h2101",nbins,min,max);
  trk->Draw("t_10 >> h2101","chi2_glob<1000");
  h2101->SetTitle("Time difference t_hit_1 - t_hit_0 (chi2_glob<1000)");
  h2101->GetXaxis()->SetTitle("t_hit_1 - t_hit_0");
  
  TH1F *h2201 = new TH1F("h2201","h2201",nbins,min,max);
  trk->Draw("t_20 >> h2201","chi2_glob<1000");
  h2201->SetTitle("Time difference t_hit_2 - t_hit_0 (chi2_glob<1000)");
  h2201->GetXaxis()->SetTitle("t_hit_2 - t_hit_0");
  
  TH1F *h2301 = new TH1F("h2301","h2301",nbins,min,max);
  trk->Draw("t_30 >> h2301","chi2_glob<1000");
  h2301->SetTitle("Time difference t_hit_3 - t_hit_0 (chi2_glob<1000)");
  h2301->GetXaxis()->SetTitle("t_hit_3 - t_hit_0");
  
  TH1F *h2321 = new TH1F("h2321","h2321",nbins,min,max);
  trk->Draw("t_32 >> h2321","chi2_glob<1000");
  h2321->SetTitle("Time difference t_hit_3 - t_hit_2 (chi2_glob<1000)");
  h2321->GetXaxis()->SetTitle("t_hit_3 - t_hit_2");


  
  //
  
  S->Clear();    
  S->Divide(2,2);
  
  S->cd(1);
  h2101->Draw();//Draw("hist");
  S->cd(2);
  h2201->Draw();//Draw("hist");
  S->cd(3);
  h2301->Draw();//Draw("hist");
  S->cd(4);
  h2321->Draw();//Draw("hist");
  S->Print(out_file_);
  //
  
  
  // timing difference with chi2 cut      chi2<300
  nbins = 200;
  TH1F *h2102 = new TH1F("h2102","h2102",nbins,min,max);
  trk->Draw("t_10 >> h2102","chi2_glob<300");
  h2102->SetTitle("Time difference t_hit_1 - t_hit_0 (chi2_glob<300)");
  h2102->GetXaxis()->SetTitle("t_hit_1 - t_hit_0");
  
  TH1F *h2202 = new TH1F("h2202","h2202",nbins,min,max);
  trk->Draw("t_20 >> h2202","chi2_glob<300");
  h2202->SetTitle("Time difference t_hit_2 - t_hit_0 (chi2_glob<300)");
  h2202->GetXaxis()->SetTitle("t_hit_2 - t_hit_0");
  
  TH1F *h2302 = new TH1F("h2302","h2302",nbins,min,max);
  trk->Draw("t_30 >> h2302","chi2_glob<300");
  h2302->SetTitle("Time difference t_hit_3 - t_hit_0 (chi2_glob<300)");
  h2302->GetXaxis()->SetTitle("t_hit_3 - t_hit_0");
  
  TH1F *h2322 = new TH1F("h2322","h2322",nbins,min,max);
  trk->Draw("t_32 >> h2322","chi2_glob<300");
  h2322->SetTitle("Time difference t_hit_3 - t_hit_2 (chi2_glob<300)");
  h2322->GetXaxis()->SetTitle("t_hit_3 - t_hit_2");
  
  S->Clear();    
  S->Divide(2,2);
  
  S->cd(1);
  h2102->Draw();//Draw("hist");
  S->cd(2);
  h2202->Draw();//Draw("hist");
  S->cd(3);
  h2302->Draw();//Draw("hist");
  S->cd(4);
  h2322->Draw();//Draw("hist");
  S->Print(out_file_);
  //
  

  // timing difference with chi2 cut      chi2<300
  nbins = 200;
  TH1F *h2103 = new TH1F("h2103","h2103",nbins,min,max);
  trk->Draw("t_10 >> h2103","chi2_glob<100000");
  h2103->SetTitle("Time difference t_hit_1 - t_hit_0 (chi2_glob<100000)");
  h2103->GetXaxis()->SetTitle("t_hit_1 - t_hit_0");
  
  TH1F *h2203 = new TH1F("h2203","h2203",nbins,min,max);
  trk->Draw("t_20 >> h2203","chi2_glob<100000");
  h2203->SetTitle("Time difference t_hit_2 - t_hit_0 (chi2_glob<100000)");
  h2203->GetXaxis()->SetTitle("t_hit_2 - t_hit_0");
  
  TH1F *h2303 = new TH1F("h2303","h2303",nbins,min,max);
  trk->Draw("t_30 >> h2303","chi2_glob<100000");
  h2303->SetTitle("Time difference t_hit_3 - t_hit_0 (chi2_glob<100000)");
  h2303->GetXaxis()->SetTitle("t_hit_3 - t_hit_0");

  TH1F *h2323 = new TH1F("h2323","h2323",nbins,min,max);
  trk->Draw("t_32 >> h2323","chi2_glob<100000");
  h2323->SetTitle("Time difference t_hit_3 - t_hit_2 (chi2_glob<100000)");
  h2323->GetXaxis()->SetTitle("t_hit_3 - t_hit_2");
  
  S->Clear();    
  S->Divide(2,2);
  
  S->cd(1);
  h2103->Draw();//Draw("hist");
  S->cd(2);
  h2203->Draw();//Draw("hist");
  S->cd(3);
  h2303->Draw();//Draw("hist");
  S->cd(4);
  h2323->Draw();//Draw("hist");
  S->Print(out_file_);
  //
  
  
  
  //////////////////
  
  /* Obsolete
     S->Clear();    
  
  min = -50;
  max = 1000;
  nbins = 105;
  TH1F *h401 = new TH1F("h401","h401",nbins,min,max);
  trk->Draw("chi2_glob >> h401","chi2_glob<1000");
  h401->SetTitle("Chi2 of straight line fit (all 3-hit combinations)");
  h401->GetXaxis()->SetTitle("Chi2");
  h401->GetXaxis()->CenterTitle();
  h401->Draw();
  S->Print(out_file_);
  
  TH1F *h402 = new TH1F("h402","h402",nbins,min,max);
  trk->Draw("chi2_glob >> h402","chi2_glob<1000 && qual_track<10");
  h402->SetTitle("Chi2 of straight line fit (all 3-hit combinations)");
  h402->GetXaxis()->SetTitle("Chi2");
  h402->GetXaxis()->CenterTitle();
  
  h402->Draw();
  S->Print(out_file_);
  
  // combined
  
  h401->SetLineColor(kRed);
  h401->Draw();
  
  h402->SetLineColor(kBlue);
  h402->SetLineStyle(3);
  h402->Draw("same");
  S->Print(out_file_);
  
  */
  
  /////////////////////////////
  // CHi2 Zoom
  S->Clear();
  S->Divide(2,2);
  S->cd(1);
  trk->Draw("chi2_glob >> htemp1");
  TH1F *htemp1 = (TH1F *)gPad->GetPrimitive("htemp1");
  S->cd(2);
  trk->Draw("chi2_glob >> htemp2","abs(chi2_glob)<1e5");
  TH1F *htemp2 = (TH1F *)gPad->GetPrimitive("htemp2");
  S->cd(3);
  trk->Draw("chi2_glob >> htemp3","abs(chi2_glob)<1e4");
  TH1F *htemp3 = (TH1F *)gPad->GetPrimitive("htemp3");
  S->cd(4);
  trk->Draw("chi2_glob >> htemp4","abs(chi2_glob)<1e3");
  TH1F *htemp4 = (TH1F *)gPad->GetPrimitive("htemp4");
  S->Print(out_file_);
  
  
  // CHi2 Zoom   with quality cut
  
  S->Clear();
  S->Divide(2,2);
  S->cd(1);
  
  sprintf(char_cut,"qual_track<%.0f",qualcut);  
  trk->Draw("chi2_glob >> htemp1a",char_cut);
  TH1F *htemp1a = (TH1F *)gPad->GetPrimitive("htemp1a");
  S->cd(2);
  sprintf(char_cut,"qual_track<%.0f && abs(chi2_glob)<1e5",qualcut);  
  trk->Draw("chi2_glob >> htemp2a",char_cut);
  TH1F *htemp2a = (TH1F *)gPad->GetPrimitive("htemp2a");
  S->cd(3);
  sprintf(char_cut,"qual_track<%.0f && abs(chi2_glob)<1e4",qualcut);  
  trk->Draw("chi2_glob >> htemp3a",char_cut);
  TH1F *htemp3a = (TH1F *)gPad->GetPrimitive("htemp3a");
  S->cd(4);
  sprintf(char_cut,"qual_track<%.0f && abs(chi2_glob)<1e3",qualcut);  
  trk->Draw("chi2_glob >> htemp4a",char_cut);
  TH1F *htemp4a = (TH1F *)gPad->GetPrimitive("htemp4a");
  S->Print(out_file_);
  
  
  // correlation plots
  S->Clear();
  S->Divide(2,2);
  S->cd(1);
  sprintf(char_cut,"qual_track<%.0f && chi2_glob<1e5 && abs(t_10)<100000",qualcut);  
  trk->Draw("chi2_glob:t_10",char_cut,"BOX");
  S->cd(2);
  sprintf(char_cut,"qual_track<%.0f && chi2_glob<1e4 && abs(t_10)<10000",qualcut);  
  trk->Draw("chi2_glob:t_10",char_cut,"BOX");
  S->cd(3);
  sprintf(char_cut,"qual_track<%.0f && chi2_glob<1000 && abs(t_10)<1000",qualcut);  
  trk->Draw("chi2_glob:t_10",char_cut,"BOX");
  S->cd(4);
  sprintf(char_cut,"qual_track<%.0f && chi2_glob<300 && abs(t_10)<300",qualcut);  
  trk->Draw("chi2_glob:t_10",char_cut,"BOX");
  S->Print(out_file_);
  
  
  
  
  
  // CHi2  Overlay Zoom   with quality cut
  
  S->Clear();
  S->Divide(2,2);
  
  S->cd(1);
  gPad->SetLogy();
  htemp1->SetLineColor(kBlack);
  htemp1->Draw();
  htemp1a->SetLineColor(kBlue);
  htemp1a->Draw("same");
  
  TLegend *leg1 =  new TLegend(0.56,0.7,0.96,0.96);
  leg1->SetFillStyle(0);
  leg1->SetBorderSize(0);
  leg1->SetHeader("","C");
  leg1->AddEntry(htemp1,"All","l");
  leg1->AddEntry(htemp1a,"Core Tracks","l");
  leg1->Draw();
  
  /*
     // Get the X-axis title
     TString x_title = h->GetXaxis()->GetTitle();
  // Get the Y-axis title
  TString y_title = h->GetYaxis()->GetTitle();
  */
  
  S->cd(2);
  gPad->SetLogy();
  htemp2->SetLineColor(kBlack);
  htemp2->Draw();
  htemp2a->SetLineColor(kBlue);
  htemp2a->Draw("same");
  
  S->cd(3);
  gPad->SetLogy();
  htemp3->SetLineColor(kBlack);
  htemp3->Draw();
  htemp3a->SetLineColor(kBlue);
  htemp3a->Draw("same");
  
  S->cd(4);
  gPad->SetLogy();
  htemp4->SetLineColor(kBlack);
  htemp4->Draw();
  htemp4a->SetLineColor(kBlue);
  htemp4a->Draw("same");
  
  S->Print(out_file_);
  
  
  /////////////////////////////////////////////////////////
  
  //////////////////////////////////////////////////////////
  // good track quality
  S->Clear();
  S->Divide(2,2);
  S->cd(1);
  sprintf(char_cut,"qual_track<%.0f",qualcut);  
  trk->Draw("chi2_glob",char_cut);
  S->cd(2);
  sprintf(char_cut,"abs(chi2_glob)<1e4 && qual_track<%.0f",qualcut);  
  trk->Draw("chi2_glob",char_cut);
  S->cd(3);
  sprintf(char_cut,"abs(chi2_glob)<1e3 && qual_track<%.0f",qualcut);  
  trk->Draw("chi2_glob",char_cut);
  S->cd(4);
  sprintf(char_cut,"abs(chi2_glob)<1e2 && qual_track<%.0f",qualcut);  
  trk->Draw("chi2_glob",char_cut);
  S->Print(out_file_);
  
  
  
  ////////////////////////////////////////////////
  // Single Cuts    
  S->Clear();
  S->Divide(2,2);
  S->cd(1);
  trk->Draw("chi2_glob:t_rms","","BOX");
  S->cd(2);
  trk->Draw("chi2_glob:t_rms","qual_track<20","BOX");
  S->cd(3);
  trk->Draw("chi2_glob:t_rms","abs(chi2_glob)<1e4","BOX");
  S->cd(4);
  trk->Draw("chi2_glob:t_rms","chi2_glob<1000 && abs(t_rms)<1000","BOX");
  S->Print(out_file_);
  
  ///////////////////////////////
  // zoom in w/o track quality

  S->Clear();
  S->Divide(2,2);
  S->cd(1);
  trk->Draw("chi2_glob:t_rms>>hscat1","","BOX");
  TH1F *hscat1 = (TH1F *)gPad->GetPrimitive("hscat1");
  S->cd(2);
  trk->Draw("chi2_glob:t_rms>>hscat2","chi2_glob<10000 && abs(t_rms)<1000","BOX");
  TH1F *hscat2 = (TH1F *)gPad->GetPrimitive("hscat2");
  S->cd(3);
  trk->Draw("chi2_glob:t_rms>>hscat3","chi2_glob<1000 && abs(t_rms)<300","BOX");
  TH1F *hscat3 = (TH1F *)gPad->GetPrimitive("hscat3");
  S->cd(4);
  trk->Draw("chi2_glob:t_rms>>hscat4","chi2_glob<300 && abs(t_rms)<300","BOX");
  TH1F *hscat4 = (TH1F *)gPad->GetPrimitive("hscat4");
  S->Print(out_file_);
  
  // zoom in with track quality
  S->Clear();
  S->Divide(2,2);
  S->cd(1);
  sprintf(char_cut,"qual_track<%.0f",qualcut);  
  trk->Draw("chi2_glob:t_rms>>hscat1a",char_cut,"BOX");
  TH1F *hscat1a = (TH1F *)gPad->GetPrimitive("hscat1a");
  S->cd(2);
  sprintf(char_cut,"chi2_glob<10000 && abs(t_rms)<1000 && qual_track<%.0f",qualcut);  
  trk->Draw("chi2_glob:t_rms>>hscat2a",char_cut,"BOX");
  TH1F *hscat2a = (TH1F *)gPad->GetPrimitive("hscat2a");
  S->cd(3);
  sprintf(char_cut,"chi2_glob<1000 && abs(t_rms)<300 && qual_track<%.0f",qualcut);  
  trk->Draw("chi2_glob:t_rms>>hscat3a",char_cut,"BOX");
  TH1F *hscat3a = (TH1F *)gPad->GetPrimitive("hscat3a");
  S->cd(4);
  sprintf(char_cut,"chi2_glob<300 && abs(t_rms)<300 && qual_track<%.0f",qualcut);  
  trk->Draw("chi2_glob:t_rms>>hscat4a",char_cut,"BOX");
  TH1F *hscat4a = (TH1F *)gPad->GetPrimitive("hscat4a");
  S->Print(out_file_);
  
  
  
  // OVERLAY
  S->Clear();
  S->Divide(2,2);
  
  S->cd(1);
  hscat1->SetLineColor(kBlack);
  hscat1->SetMarkerStyle(25);
  hscat1->Draw("BOX");
  hscat1a->SetLineColor(kBlue);
  hscat1a->SetMarkerColor(kBlue);
  hscat1a->SetMarkerStyle(25);
  hscat1a->Draw("BOX same");
  
  TLegend *lscat1 =  new TLegend(0.56,0.7,0.96,0.96);
  lscat1->SetFillStyle(0);
  lscat1->SetBorderSize(0);
  lscat1->SetHeader("","C");
  lscat1->AddEntry(hscat1,"All","p");
  lscat1->AddEntry(hscat1a,"Core Tracks","p");
  lscat1->Draw();
  
  S->cd(2);
  hscat2->SetLineColor(kBlack);
  hscat2->Draw("BOX");
  hscat2a->SetLineColor(kBlue);
  hscat2a->Draw("BOX same");
  
  S->cd(3);
  hscat3->SetLineColor(kBlack);
  hscat3->Draw("BOX");
  hscat3a->SetLineColor(kBlue);
  hscat3a->Draw("BOX same");
  
  S->cd(4);
  hscat4->SetLineColor(kBlack);
  hscat4->Draw("BOX");
  hscat4a->SetLineColor(kBlue);
  hscat4a->Draw("BOX same");
  
  S->Print(out_file_);
  
  
  
  
  
  //////////////////////////////////////    
  S->Clear();
  
  S->Clear();    
  S->Divide(2,2);
  
  S->cd(1);
  sprintf(char_cut,"qual_track<%.0f",qualcut);  
  trk->Draw("t_rms",char_cut);
  S->cd(2);
  sprintf(char_cut,"abs(t_rms)<1e6  && qual_track<%.0f",qualcut);  
  trk->Draw("t_rms",char_cut);
  S->cd(3);
  sprintf(char_cut,"abs(t_rms)<1e4  && qual_track<%.0f",qualcut);  
  trk->Draw("t_rms",char_cut);
  S->cd(4);
  sprintf(char_cut,"abs(t_rms)<1e2  && qual_track<%.0f",qualcut);  
  trk->Draw("t_rms",char_cut);
  
  S->Print(out_file_);
  
  
  /////////////////////////////////////////////////
  // time differences for specific half-ladders
  
  S->Clear();
  S->Divide(2,2);
  S->cd(1);
  sprintf(char_cut,"qual_track<%.0f && abs(t_10)<200 && hit_layer_id[0]==1 && hit_ladder_id[0]==1",qualcut);  
  trk->Draw("t_10",char_cut);
  S->cd(2);
  sprintf(char_cut,"qual_track<%.0f && abs(t_20)<200 && hit_layer_id[0]==1 && hit_ladder_id[0]==1",qualcut);  
  trk->Draw("t_20",char_cut);
  S->cd(3);
  sprintf(char_cut,"qual_track<%.0f && abs(t_30)<200 && hit_layer_id[0]==1 && hit_ladder_id[0]==1",qualcut);  
  trk->Draw("t_30",char_cut);
  S->cd(4);
  sprintf(char_cut,"qual_track<%.0f && abs(t_32)<200 && hit_layer_id[0]==1 && hit_ladder_id[0]==1",qualcut);  
  trk->Draw("t_32",char_cut);
  S->Print(out_file_);
  
  S->Clear();
  S->Divide(2,2);
  S->cd(1);
  sprintf(char_cut,"qual_track<%.0f && abs(t_10)<200 && hit_layer_id[0]==1 && hit_ladder_id[0]==2 ",qualcut);  
  trk->Draw("t_10",char_cut);
  S->cd(2);
  sprintf(char_cut,"qual_track<%.0f && abs(t_20)<200 && hit_layer_id[0]==1 && hit_ladder_id[0]==2 ",qualcut);  
  trk->Draw("t_20",char_cut);
  S->cd(3);
  sprintf(char_cut,"qual_track<%.0f && abs(t_30)<200 && hit_layer_id[0]==1 && hit_ladder_id[0]==2 ",qualcut);  
  trk->Draw("t_30",char_cut);
  S->cd(4);
  sprintf(char_cut,"qual_track<%.0f && abs(t_32)<200 && hit_layer_id[0]==1 && hit_ladder_id[0]==2 ",qualcut);  
  trk->Draw("t_32",char_cut);
  S->Print(out_file_);
  
  /// MOVE TO SOMEWHERE ELSE
  /*
  S->Clear();
  trk->Draw("hit_ladder_id[0]:hit_layer_id[0]","abs(chi2_glob)<1e4 && qual_track<20","BOX");
  S->Print(out_file_);
  */
  
  
  ////////////////////////////////////////////////
  // Correlation between hit ladder id and timing differences in 3-hot combination
  S->Clear();
  S->Divide(2,2);
  S->cd(1);

  sprintf(char_cut,"qual_track<%.0f && hit_layer_id[0]==1 && abs(chi2_glob)<1e4 && t_10<1e8",qualcut);  
  trk->Draw("hit_ladder_id[0]:t_10",char_cut,"BOX");
  S->cd(2);
  sprintf(char_cut,"qual_track<%.0f && hit_layer_id[0]==1 && abs(chi2_glob)<1e4 && t_20<1e8",qualcut);  
  trk->Draw("hit_ladder_id[0]:t_20",char_cut,"BOX");
  S->cd(3);
  sprintf(char_cut,"qual_track<%.0f && hit_layer_id[0]==1 && abs(chi2_glob)<1e4 && t_30<1e8",qualcut);  
  trk->Draw("hit_ladder_id[0]:t_30",char_cut,"BOX");
  S->cd(4);
  sprintf(char_cut,"qual_track<%.0f && hit_layer_id[0]==1 && abs(chi2_glob)<1e4 && t_32<1e8",qualcut);  
  trk->Draw("hit_ladder_id[0]:t_32",char_cut,"BOX");
  S->Print(out_file_);
  
  /////////////////////////////////////
  S->Clear();
  trk->Draw("t_10:hit_ladder_id[0]","hit_layer_id[0]==1 && abs(t_rms)<1e3 && t_10<1e8","BOX");
  S->Print(out_file_);
  
  S->Clear();
  trk->Draw("t_10:hit_ladder_id[0]","hit_layer_id[0]==1 && abs(t_10)<1e3","BOX");
  S->Print(out_file_);


  ///////////////////////////////////////////////////////////////////////////////
  // tot Analysis
  ///////////////////////////////////////////////////////////////////////////////



  /////////////////////////////////////


#ifdef TRIHITS
  trk->Draw("hit_tot[0]>>htotw0all","1.0/ncomb3");
#else
  trk->Draw("hit_tot[0]>>htotw0all","1.0/ncomb4");
#endif
  TH1F *htotw0all = (TH1F *)gPad->GetPrimitive("htotw0all");
  sprintf(char_cut,"qual_track<%f&&chi2_glob<%f",qualcut,chi2cut);
  trk->Draw("hit_tot[0]>>htotw0good",char_cut);
  TH1F *htotw0good = (TH1F *)gPad->GetPrimitive("htotw0good");
  
#ifdef TRIHITS
  trk->Draw("hit_tot[1]>>htotw1all","1.0/ncomb3");
#else
  trk->Draw("hit_tot[1]>>htotw1all","1.0/ncomb4");
#endif
  TH1F *htotw1all = (TH1F *)gPad->GetPrimitive("htotw1all");
  sprintf(char_cut,"qual_track<%f&&chi2_glob<%f",qualcut,chi2cut);
  trk->Draw("hit_tot[1]>>htotw1good",char_cut);
  TH1F *htotw1good = (TH1F *)gPad->GetPrimitive("htotw1good");
  
#ifdef TRIHITS
  trk->Draw("hit_tot[2]>>htotw2all","1.0/ncomb3");
#else
  trk->Draw("hit_tot[2]>>htotw2all","1.0/ncomb4");
#endif
  TH1F *htotw2all = (TH1F *)gPad->GetPrimitive("htotw2all");
  sprintf(char_cut,"qual_track<%f&&chi2_glob<%f",qualcut,chi2cut);
  trk->Draw("hit_tot[2]>>htotw2good",char_cut);
  TH1F *htotw2good = (TH1F *)gPad->GetPrimitive("htotw2good");
  
#ifdef TRIHITS
  trk->Draw("hit_tot[3]>>htotw3all","1.0/ncomb3");
#else
  trk->Draw("hit_tot[3]>>htotw3all","1.0/ncomb4");
#endif
  TH1F *htotw3all = (TH1F *)gPad->GetPrimitive("htotw3all");
  sprintf(char_cut,"qual_track<%f&&chi2_glob<%f",qualcut,chi2cut);
  trk->Draw("hit_tot[3]>>htotw3good",char_cut);
  TH1F *htotw3good = (TH1F *)gPad->GetPrimitive("htotw3good");
  
  S->Clear();
  S->Divide(2,2);

  S->cd(1);
  htotw0all->Draw();
  htotw0good->SetLineColor(kViolet);
  htotw0good->Draw("same");

  S->cd(2);
  htotw1all->Draw();
  htotw1good->SetLineColor(kViolet);
  htotw1good->Draw("same");

  S->cd(3);
  htotw2all->Draw();
  htotw2good->SetLineColor(kViolet);
  htotw2good->Draw("same");

  S->cd(4);
  htotw3all->Draw();
  htotw3good->SetLineColor(kViolet);
  htotw3good->Draw("same");
  
  S->Print(out_file_);
  
  /*
  delete htot0all;
  delete htot0good;
  delete htot1all;
  delete htot1good;
  delete htot2all;
  delete htot2good;
  delete htot3all;
  delete htot3good;
  */


  /////////////////////////////////////
  
  trk->Draw("hit_tot[0]>>htot0all");
  TH1F *htot0all = (TH1F *)gPad->GetPrimitive("htot0all");
  sprintf(char_cut,"qual_track<%f&&chi2_glob<%f",qualcut,chi2cut);
  trk->Draw("hit_tot[0]>>htot0good",char_cut);
  TH1F *htot0good = (TH1F *)gPad->GetPrimitive("htot0good");
  
  trk->Draw("hit_tot[1]>>htot1all");
  TH1F *htot1all = (TH1F *)gPad->GetPrimitive("htot1all");
  sprintf(char_cut,"qual_track<%f&&chi2_glob<%f",qualcut,chi2cut);
  trk->Draw("hit_tot[1]>>htot1good",char_cut);
  TH1F *htot1good = (TH1F *)gPad->GetPrimitive("htot1good");
  
  trk->Draw("hit_tot[2]>>htot2all");
  TH1F *htot2all = (TH1F *)gPad->GetPrimitive("htot2all");
  sprintf(char_cut,"qual_track<%f&&chi2_glob<%f",qualcut,chi2cut);
  trk->Draw("hit_tot[2]>>htot2good",char_cut);
  TH1F *htot2good = (TH1F *)gPad->GetPrimitive("htot2good");
  
  trk->Draw("hit_tot[3]>>htot3all");
  TH1F *htot3all = (TH1F *)gPad->GetPrimitive("htot3all");
  sprintf(char_cut,"qual_track<%f&&chi2_glob<%f",qualcut,chi2cut);
  trk->Draw("hit_tot[3]>>htot3good",char_cut);
  TH1F *htot3good = (TH1F *)gPad->GetPrimitive("htot3good");
  
  S->Clear();
  S->Divide(2,2);

  S->cd(1);
  htot0all->Draw();
  htot0good->SetLineColor(kViolet);
  htot0good->Draw("same");

  S->cd(2);
  htot1all->Draw();
  htot1good->SetLineColor(kViolet);
  htot1good->Draw("same");

  S->cd(3);
  htot2all->Draw();
  htot2good->SetLineColor(kViolet);
  htot2good->Draw("same");

  S->cd(4);
  htot3all->Draw();
  htot3good->SetLineColor(kViolet);
  htot3good->Draw("same");
  
  S->Print(out_file_);


  /////////////////////////////////////////////

  


  /////////////////////////////////////


  
  ///////////////////////////////////////////////////////////////////////////////
  // 2-hit Analysis
  ///////////////////////////////////////////////////////////////////////////////

  //
  S->Clear();
  S->Divide(2,1);
  S->cd(1);
  trk->Draw("trk_phi","","");
  S->cd(2);
  gStyle->SetOptStat(11111);
  gStyle->SetOptFit(11111);
  trk->Draw("trk_theta>>h_theta(18,0.65,2.45)","","");
  TH1F *h_theta = (TH1F *)gPad->GetPrimitive("h_theta");
  h_theta->GetXaxis()->SetTitle("trk_theta");
  //TF1* fcos2 = new TF1("fcos2","[0]*TMath::Sq(cos([1]*x+[2]))", 0.9, 2.2);
  //TF1* fcos2 = new TF1("fcos2","[0]*TMath::Sq(cos([1]*x+[2]))", 0.7, 2.4);
  TF1* fsin2 = new TF1("fsin2","[0]*TMath::Power(sin(x + [1]), [2])", 0.7, 2.4);
  fsin2->SetParameter(0,7.6e1);
  fsin2->SetParameter(1,-4.1e-3);
  fsin2->SetParameter(2,8.7);
  h_theta->Fit(fsin2, "R");
  h_theta->Draw("pe");
  fsin2->Draw("same");
  S->Print(out_file_);
  
  //Theta
  S->Clear();
  S->Divide(2,1);
  S->cd(1);
  trk->Draw("trk_theta>>theta1e3(13,0.65,2.45)", "chi2_glob < 1e3");
  TH1F *h_theta1e3 = (TH1F *)gPad->GetPrimitive("theta1e3");
  h_theta1e3->GetXaxis()->SetTitle("trk_theta");
  h_theta1e3->Fit(fsin2,"R");
  h_theta1e3->Draw("pe");
  S->cd(2);
  trk->Draw("trk_theta>>theta2e2(13,0.65,2.45)", "chi2_glob < 2e2");
  TH1F *h_theta2e2 = (TH1F *)gPad->GetPrimitive("theta2e2");
  h_theta2e2->GetXaxis()->SetTitle("trk_theta");
  h_theta2e2->Fit(fsin2, "R");
  h_theta2e2->Draw("pe");
  S->Print(out_file_);
  
  //
  S->Clear();
  S->Divide(2,1);
  S->cd(1);
  trk->Draw("trk_theta>>thetaQ9(18,0.65,2.45)", "qual_track < 9");
  TH1F *h_thetaQ9 = (TH1F *)gPad->GetPrimitive("thetaQ9");
  h_thetaQ9->GetXaxis()->SetTitle("trk_theta");
  h_thetaQ9->Fit(fsin2,"R");
  h_thetaQ9->Draw("pe");
  S->cd(2);
  trk->Draw("trk_theta>>theta1e3Q9(13,0.65,2.45)", "chi2_glob < 1e3 && qual_track<9");
  TH1F *h_theta1e3Q9 = (TH1F *)gPad->GetPrimitive("theta1e3Q9");
  h_theta1e3Q9->GetXaxis()->SetTitle("trk_theta");
  h_theta1e3Q9->Fit(fsin2, "R");
  h_theta1e3Q9->Draw("pe");
  S->Print(out_file_);
  
  //Lambda
  S->Clear();
  S->Divide(2,1);
  S->cd(1);
  trk->Draw("(0.5*TMath::Pi() - trk_theta)","","");
  S->cd(2);
  gStyle->SetOptStat(11111);
  gStyle->SetOptFit(11111);
  trk->Draw("(0.5*TMath::Pi() - trk_theta)>>h_lambda(13,-1.0,1.0)","","");
  TH1F *h_lambda = (TH1F *)gPad->GetPrimitive("h_lambda");
  h_lambda->GetXaxis()->SetTitle("trk_lambda");
  TF1* fcos2 = new TF1("fcos2","[0]*TMath::Power(cos(x + [1]), [2])",-0.9, 0.9);
  fcos2->SetParameter(0,7.8e1);
  fcos2->SetParameter(1,5.7e-3);
  fcos2->SetParameter(2,8.6);
  h_lambda->Fit(fcos2, "R");
  h_lambda->Draw("pe");
  fcos2->Draw("same");
  S->Print(out_file_);
  
  //
  S->Clear();
  S->Divide(2,1);
  S->cd(1);
  trk->Draw("(0.5*TMath::Pi() - trk_theta)>>lambda1e3(13,-1.0,1.0)", "chi2_glob < 1e3");
  TH1F *h_lambda1e3 = (TH1F *)gPad->GetPrimitive("lambda1e3");
  h_lambda1e3->GetXaxis()->SetTitle("trk_lambda");
  h_lambda1e3->Fit(fcos2,"R");
  h_lambda1e3->Draw("pe");
  S->cd(2);
  trk->Draw("(0.5*TMath::Pi() - trk_theta)>>lambda2e2(13,-1.0,1.0)", "chi2_glob < 2e2");
  TH1F *h_lambda2e2 = (TH1F *)gPad->GetPrimitive("lambda2e2");
  h_lambda2e2->GetXaxis()->SetTitle("trk_lambda");
  h_lambda2e2->Fit(fcos2, "R");
  h_lambda2e2->Draw("pe");
  S->Print(out_file_);
  
  //
  S->Clear();
  S->Divide(2,1);
  S->cd(1);
  trk->Draw("(0.5*TMath::Pi() - trk_theta)>>lambdaQ9(13,-1.0,1.0)", "qual_track < 9");
  TH1F *h_lambdaQ9 = (TH1F *)gPad->GetPrimitive("lambdaQ9");
  h_lambdaQ9->GetXaxis()->SetTitle("trk_lambda");
  h_lambdaQ9->Fit(fcos2,"R");
  h_lambdaQ9->Draw("pe");
  S->cd(2);
  trk->Draw("(0.5*TMath::Pi() - trk_theta)>>lambda1e3Q9(13,-1.0,1.0)", "chi2_glob < 1e3 && qual_track<9");
  TH1F *h_lambda1e3Q9 = (TH1F *)gPad->GetPrimitive("lambda1e3Q9");
  h_lambda1e3Q9->GetXaxis()->SetTitle("trk_lambda");
  h_lambda1e3Q9->Fit(fcos2, "R");
  h_lambda1e3Q9->Draw("pe");
  S->Print(out_file_);

  //
  S->Clear();
  S->Divide(2,1);
  S->cd(1);
  gPad->SetLogy(1);
  trk->Draw("chi2_glob>>h_chi2(200,0,1e3)", "chi2_glob < 1e3");
  S->cd(2);
  gPad->SetLogy(1);
  trk->Draw("chi2_glob>>h_chi2_1(100,0,1e2)", "chi2_glob < 1e2");
  S->Print(out_file_);
  
  gStyle->SetOptStat(0);
  //
  S->Clear();
  S->SetLogy(0);
  S->Divide(2,1);
  S->cd(1);
  gPad->Clear();
  trk->Draw("hit_phi_incline","","BOX");
  S->cd(2);
  gPad->Clear();
  trk->Draw("ncomb2:hit_phi_incline","","BOX");

  S->Print(out_file_);
  

  
  
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
  
  std::cout<<"Writing to pdf ....... \n";
  WriteToPdf(trk, output_file);
  
  
  std::cout<<"Done writing to pdf ....... \n";
  return 0;
}
