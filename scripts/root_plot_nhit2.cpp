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
#include <fstream>
#include <string>
#include <math.h>
#include <vector>
std::string input_tree_name = "TrackTree";

const int MaxEntries = 100000;

void WriteToPdf(TTree *trk, std::string output_file_name)
{
  /////////////////////////////////////////////////
  // cuts for triplets
  // const double chi2cut=49;
  const double qualcut=9;

  // cuts for quadruplets
  //  const double chi2cut=249;
  // const double qualcut=19;

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

  gStyle->SetOptStat(0);
  //gROOT->ForceStyle();


  
  // example binning
  int nbins = 100;
  double min = -50.0;
  double max = 50.0;
  
  TH1F *h1 = new TH1F("h1","h1",nbins,min,max);
  trk->Draw("hit_x>>h1");
  h1->SetTitle("Hit X");
  h1->GetXaxis()->SetTitle("Hit X [mm]");
  // set log scale
  //S->SetLogy();
  h1->Draw();//Draw("hist");
  S->Print(out_file_open); // pg1
  
  
  trk->SetMarkerStyle(20);
  trk->SetMarkerSize(0.5);
  
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
  h2->Draw("SCAT");
  //h2->Draw("colz");
  S->Print(out_file_);// pg2
  
  trk->SetMarkerStyle(20);
  trk->SetMarkerSize(0.3);
  
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
  trk->Draw("hit_layer_id:hit_ladder_id","","BOX");
  S->Print(out_file_);
  
  
  // ladders outer and inner layers
  S->Clear();
  S->Divide(2,1);
  S->cd(1);
  trk->Draw("hit_ladder_id","hit_layer_id==1");

  S->cd(2);
  trk->Draw("hit_ladder_id","hit_layer_id==0");
  S->Print(out_file_);
  

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
  
  
  S->Clear();
  trk->Draw("event:run_id");
  S->Print(out_file_);
  
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
  

  trk->Draw("run_id:hit_timeFPGA[0] >> h24_1","","BOX");
  TH1F *h24_1 = (TH1F *)gPad->GetPrimitive("h24_1");
  h24_1->SetTitle("hit time (FPGA)");
  h24_1->GetXaxis()->SetTitle("Hit(1) Time FPGA (in 16us)");
  h24_1->GetYaxis()->SetTitle("Run number");
  
  trk->Draw("run_id:hit_timeFPGA[1] >> h24_2","","BOX");
  TH1F *h24_2 = (TH1F *)gPad->GetPrimitive("h24_2");
  h24_2->SetTitle("hit time (FPGA)");
  h24_2->GetXaxis()->SetTitle("Hit(2) Time FPGA (in 16us)");
  h24_2->GetYaxis()->SetTitle("Run number");

  S->Clear();
  S->Divide(2,1);
  S->cd(1);
  h24_1->Draw();
  S->cd(2);
  h24_2->Draw();

  S->Print(out_file_);
  
  
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
  
  
  S->Clear();
  //TH2F *h3 = new TH2F("h3","h3",nbins,-max,-min,nbins,min,max);
  trk->Draw("event:hit_timestamp","event<100000","BOX");
  S->Print(out_file_);
  
  S->Clear();
  //TH2F *h3 = new TH2F("h3","h3",nbins,-max,-min,nbins,min,max);
  trk->Draw("event:hit_timeFPGA","event<100000","BOX");
  S->Print(out_file_);
  
  /**************************************/
  
  S->Clear();
  trk->Draw("frame_id");
  S->Print(out_file_);
  
  S->Clear();
  trk->Draw("event:frame_id");
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
  max = 40;
  nbins = 100;
  TH1F *h9100 = new TH1F("h9100","h9100",nbins,min,max);
  trk->Draw("comb_type >> h9100");
  h9100->SetTitle("Hit Combination Type");
  h9100->GetXaxis()->SetTitle("Hit Combination Type");
  h9100->Draw();
  S->Print(out_file_);
  
  // Combinatorics
  
  S->Clear();
  S->Divide(2,2);

  S->cd(1);
  min = 0.0;
  max = 300;
  nbins = 100;
  TH1F *h9101 = new TH1F("h9101","h9101",nbins,min,max);
  trk->Draw("ncomb2 >> h9101");
  h9101->SetTitle("Number of 2-hit combinations");
  h9101->GetXaxis()->SetTitle("2-hit combinations");
  h9101->Draw();

  S->cd(2);
  min = 0.0;
  max = 300;
  nbins = 100;
  TH1F *h9102 = new TH1F("h9102","h9102",nbins,min,max);
  trk->Draw("ncomb3 >> h9102");
  h9102->SetTitle("Number of 3-hit combinations");
  h9102->GetXaxis()->SetTitle("3-hit combinations");
  h9102->Draw();
  
  S->cd(3);
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

  // same with log scale
  S->Clear();
  S->Divide(2,2);

  S->cd(1);
  gPad->SetLogy();
  h9101->Draw();

  S->cd(2);
  gPad->SetLogy();
  h9102->Draw();

  S->cd(3);
  gPad->SetLogy();
  h9103->Draw();
  
  S->Print(out_file_);

  ///////////////////////////////////////////////////////
  // Number hits per cosmic layer
  S->Clear();
  S->Divide(2,2);

  S->cd(1);
  trk->Draw("nhitCL0","","");
  S->cd(2);
  trk->Draw("nhitCL1","","");
  S->cd(3);
  trk->Draw("nhitCL2","","");
  S->cd(4);
  trk->Draw("nhitCL3","","");

  S->Print(out_file_);

  // Log
  S->Clear();
  S->Divide(2,2);

  S->cd(1);
  gPad->SetLogy();
  trk->Draw("nhitCL0","","");
  S->cd(2);
  gPad->SetLogy();
  trk->Draw("nhitCL1","","");
  S->cd(3);
  gPad->SetLogy();
  trk->Draw("nhitCL2","","");
  S->cd(4);
  gPad->SetLogy();
  trk->Draw("nhitCL3","","");
  S->cd(1);

  S->Print(out_file_);
  
  ///////////////////////////////////////////////////////
  
  S->Clear();
  
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
  
  nbins = 100;
  ///////////////////////////////////////////////////////
  // nommber combinations vs track quality  
  S->Clear();
  S->Divide(2,2);

  S->cd(1);
  trk->Draw("ncomb2:qual_track","","BOX");
  S->cd(2);
  trk->Draw("ncomb3:qual_track","","BOX");
  S->cd(3);
  trk->Draw("ncomb4:qual_track","","BOX");

  S->Print(out_file_);
  
  ///////////////////////////////////////////////////////
  // Hit Multiplicity vs Timestamp  
  S->Clear();
  S->Divide(2,2);

  S->cd(1);
  trk->Draw("nhitCL0:frame_id","","BOX");
  S->cd(2);
  trk->Draw("nhitCL1:frame_id","","BOX");
  S->cd(3);
  trk->Draw("nhitCL2:frame_id","","BOX");
  S->cd(4);
  trk->Draw("nhitCL3:frame_id","","BOX");

  S->Print(out_file_);
  

  ///////////////////////////////////////////////////////
  // number combinations vs Timestamp  
  S->Clear();
  S->Divide(2,2);

  S->cd(1);
  trk->Draw("ncomb2:frame_id","","BOX");
  S->cd(2);
  trk->Draw("ncomb3:frame_id","","BOX");
  S->cd(3);
  trk->Draw("ncomb4:frame_id","","BOX");

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
  
  
  /////////////////////////////////////
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
  
  TH1F *h101 = new TH1F("h101","h101",nbins,min,max);
  trk->Draw("t_10 >> h101");
  h101->SetTitle("Time difference cosmic layers 1 - 0");
  h101->GetXaxis()->SetTitle("t_hit_1 - t_hit_0 in timestamps (8ns)");
  h101->Draw();//Draw("hist");
  S->Print(out_file_);
  
  sprintf(char_cut,"qual_track<%f",qualcut);
  sprintf(char_text,"Time difference in cosmic layers 1 - 0 (qual_track<%.0f)",qualcut);  
  TH1F *h1101 = new TH1F("h1101","h1101",nbins,min,max);
  trk->Draw("t_10 >> h1101",char_cut);
  h1101->SetTitle(char_text);
  h1101->GetXaxis()->SetTitle("t_hit_1 - t_hit_0 in timestamps (8ns)");
  h1101->Draw();//Draw("hist");
  S->Print(out_file_);

  
  // overlay with ALL
  h101->SetLineColor(kBlack);
  h101->SetLineWidth(1);
  h101->Draw();
  
  h1101->SetLineColor(kBlue);
  h1101->Draw("same");
  
  TLegend *leg101 =  new TLegend(0.11,0.6,0.42,0.96);
  leg101->SetFillStyle(0);
  leg101->SetBorderSize(0);
  leg101->SetHeader("","C");
  leg101->AddEntry(h101,"All","l");
  sprintf(char_text,"qual_track<%.0f",qualcut);
  leg101->AddEntry(h1101,char_text,"l");
  
  S->Print(out_file_);

  // overlay without ALL
  h1101->SetLineColor(kBlue);
  h1101->Draw();
  
  leg101->Draw();
  S->Print(out_file_);

  ////////////
  S->Clear();
  S->Divide(2,2);
  S->cd(1);
  trk->Draw("t_10","abs(t_10)<10000");
  S->cd(2);
  trk->Draw("t_10","1.0/ncomb2[0]*(abs(t_10)<10000)","HIST");
  S->cd(3);
  trk->Draw("t_10","(abs(t_10)<10000)/(nhitCL0*nhitCL1)","HIST");
  S->Print(out_file_);

  
  ////////////
  //     Time difference cosmic layers 3 - 2  (variable cuts)
  S->Clear();
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

  
  
  // overlay with ALL
  h321->SetLineColor(kBlack);
  h321->SetLineWidth(1);
  h321->Draw();
  
  h1321->SetLineColor(kBlue);
  h1321->Draw("same");
  
  TLegend *leg321 =  new TLegend(0.11,0.6,0.42,0.96);
  leg321->SetFillStyle(0);
  leg321->SetBorderSize(0);
  leg321->SetHeader("","C");
  leg321->AddEntry(h321,"All","l");
  sprintf(char_text,"qual_track<%.0f",qualcut);
  leg321->AddEntry(h1321,char_text,"l");
  
  S->Print(out_file_);
  
  // overlay without ALL
  h1321->SetLineColor(kBlue);
  h1321->Draw();
  
  leg321->Draw();
  S->Print(out_file_);
  

  ///////////////////////////////////////////////////////
  // Run Dependence
  S->Clear();
  S->Divide(2,1);
  S->cd(1);
  sprintf(char_cut,"qual_track<%f && abs(t_10)<400",qualcut);
  trk->Draw("run_id:t_10",char_cut,"BOX");
  S->cd(2);
  sprintf(char_cut,"qual_track<%f && abs(t_32)<400",qualcut);
  trk->Draw("run_id:t_32",char_cut,"BOX");

  S->Print(out_file_);
  

  ///////////////////////////////////////////////////////
  // Sensor Dependence
  S->Clear();
  S->Divide(1,2);
  S->cd(1);
  sprintf(char_cut,"qual_track<%f && abs(t_10)<400",qualcut);
  trk->Draw("hit_ladder_id[0]:t_10",char_cut,"BOX");
  S->cd(2);
  sprintf(char_cut,"qual_track<%f && abs(t_32)<400",qualcut);
  trk->Draw("hit_ladder_id[0]:t_32",char_cut,"BOX");

  S->Print(out_file_);
  
  ///////////////////////////////////////////////////////
  // Frame Dependence
  S->Clear();
  S->Divide(2,1);
  S->cd(1);
  sprintf(char_cut,"qual_track<%f && abs(t_10)<400",qualcut);
  trk->Draw("frame_id:t_10",char_cut,"BOX");

  S->cd(2);
  sprintf(char_cut,"qual_track<%f && abs(t_32)<400",qualcut);
  trk->Draw("frame_id:t_32",char_cut,"BOX");

  S->Print(out_file_);
  
  ///////////////////////////////////////////////////////
  // z-position Dependence
  S->Clear();
  S->Divide(2,2);
  S->cd(1);
  sprintf(char_cut,"qual_track<%f && abs(t_10)<400",qualcut);
  trk->Draw("hit_z[0]:t_10",char_cut,"BOX");
  S->cd(2);
  sprintf(char_cut,"qual_track<%f && abs(t_10)<400",qualcut);
  trk->Draw("hit_z[1]:t_10",char_cut,"BOX");

  S->cd(3);
  sprintf(char_cut,"qual_track<%f && abs(t_32)<400",qualcut);
  trk->Draw("hit_z[0]:t_32",char_cut,"BOX");
  S->cd(4);
  sprintf(char_cut,"qual_track<%f && abs(t_32)<400",qualcut);
  trk->Draw("hit_z[1]:t_32",char_cut,"BOX");

  S->Print(out_file_);
  

  ///////////////////////////////////////////////////////
  // trk_theta Dependence
  S->Clear();
  S->Divide(2,2);
  S->cd(1);
  sprintf(char_cut,"qual_track<%f && abs(t_10)<400",qualcut);
  trk->Draw("trk_theta:t_10",char_cut,"BOX");
  S->cd(2);
  sprintf(char_cut,"qual_track<%f && abs(t_10)<400",qualcut);
  trk->Draw("trk_theta:t_10",char_cut,"BOX");

  S->cd(3);
  sprintf(char_cut,"qual_track<%f && abs(t_32)<400",qualcut);
  trk->Draw("trk_theta:t_32",char_cut,"BOX");
  S->cd(4);
  sprintf(char_cut,"qual_track<%f && abs(t_32)<400",qualcut);
  trk->Draw("trk_theta:t_32",char_cut,"BOX");

  S->Print(out_file_);
  

  ///////////////////////////////////////////////////////
  // trk_phi Dependence
  S->Clear();
  S->Divide(2,2);
  S->cd(1);
  sprintf(char_cut,"qual_track<%f && abs(t_10)<400",qualcut);
  trk->Draw("trk_phi:t_10",char_cut,"BOX");
  S->cd(2);
  sprintf(char_cut,"qual_track<%f && abs(t_10)<400",qualcut);
  trk->Draw("trk_phi:t_10",char_cut,"BOX");

  S->cd(3);
  sprintf(char_cut,"qual_track<%f && abs(t_32)<400",qualcut);
  trk->Draw("trk_phi:t_32",char_cut,"BOX");
  S->cd(4);
  sprintf(char_cut,"qual_track<%f && abs(t_32)<400",qualcut);
  trk->Draw("trk_phi:t_32",char_cut,"BOX");

  S->Print(out_file_);
  

  ///////////////////////////////////////////////////////
  // phi_incline Dependence
  S->Clear();
  S->Divide(2,2);
  S->cd(1);
  sprintf(char_cut,"qual_track<%f && abs(t_10)<400",qualcut);
  trk->Draw("hit_phi_incline:t_10",char_cut,"BOX");
  S->cd(2);
  sprintf(char_cut,"qual_track<%f && abs(t_10)<400",qualcut);
  trk->Draw("hit_phi_incline:t_10",char_cut,"BOX");

  S->cd(3);
  sprintf(char_cut,"qual_track<%f && abs(t_32)<400",qualcut);
  trk->Draw("hit_phi_incline:t_32",char_cut,"BOX");
  S->cd(4);
  sprintf(char_cut,"qual_track<%f && abs(t_32)<400",qualcut);
  trk->Draw("hit_phi_incline:t_32",char_cut,"BOX");

  S->Print(out_file_);
  
  ///////////////////////////////////////////////////////
  // hit multiplcitiy Dependence
  S->Clear();
  S->Divide(2,2);
  S->cd(1);
  sprintf(char_cut,"qual_track<%f && abs(t_10)<400",qualcut);
  trk->Draw("nhitCL0:t_10",char_cut,"BOX");
  S->cd(2);
  sprintf(char_cut,"qual_track<%f && abs(t_10)<400",qualcut);
  trk->Draw("nhitCL1:t_10",char_cut,"BOX");

  S->cd(3);
  sprintf(char_cut,"qual_track<%f && abs(t_32)<400",qualcut);
  trk->Draw("nhitCL2:t_32",char_cut,"BOX");
  S->cd(4);
  sprintf(char_cut,"qual_track<%f && abs(t_32)<400",qualcut);
  trk->Draw("nhitCL3:t_32",char_cut,"BOX");

  S->Print(out_file_);
  

  ///////////////////////////////////////////////////////
  // hit multiplcitiy Dependence
  S->Clear();
  S->Divide(2,2);
  S->cd(1);
  gPad->SetLogy();
  sprintf(char_cut,"qual_track<%f && abs(t_10)<400",qualcut);
  trk->Draw("nhitCL0:t_10 >> hCL0t10(200,-400,400,500,0.0,1000.0)",char_cut,"BOX");
  S->cd(2);
  gPad->SetLogy();
  sprintf(char_cut,"qual_track<%f && abs(t_10)<400",qualcut);
  trk->Draw("nhitCL1:t_10 >> hCL1t10(200,-400,400,500,0.0,1000.0)",char_cut,"BOX");

  S->cd(3);
  gPad->SetLogy();
  sprintf(char_cut,"qual_track<%f && abs(t_32)<400",qualcut);
  trk->Draw("nhitCL2:t_32",char_cut,"BOX");
  S->cd(4);
  gPad->SetLogy();
  sprintf(char_cut,"qual_track<%f && abs(t_32)<400",qualcut);
  trk->Draw("nhitCL3:t_32",char_cut,"BOX");

  S->Print(out_file_);
  

  
  ///////////////////////////////////////////////////////
  // ncomb2 Dependence
  S->Clear();
  S->Divide(1,2);
  S->cd(1);
  sprintf(char_cut,"qual_track<%f && abs(t_10)<400",qualcut);
  trk->Draw("ncomb2:t_10",char_cut,"BOX");
  S->cd(2);
  sprintf(char_cut,"qual_track<%f && abs(t_32)<400",qualcut);
  trk->Draw("ncomb2:t_32",char_cut,"BOX");

  S->Print(out_file_);
  

  ///////////////////////////////////////////////////////
  // ncomb3 Dependence
  S->Clear();
  S->Divide(1,2);
  S->cd(1);
  sprintf(char_cut,"qual_track<%f && abs(t_10)<400",qualcut);
  trk->Draw("ncomb3:t_10",char_cut,"BOX");
  S->cd(2);
  sprintf(char_cut,"qual_track<%f && abs(t_32)<400",qualcut);
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
  
  
  
  /////////////////////////////
  // tot

  sprintf(char_cut,"comb_type==12",qualcut);  
  trk->Draw("hit_tot[0]>>htot0all",char_cut);
  TH1F *htot0all = (TH1F *)gPad->GetPrimitive("htot0all");

  sprintf(char_cut,"comb_type==12 && qual_track<%.0f",qualcut);  
  trk->Draw("hit_tot[0]>>htot0core",char_cut);
  TH1F *htot0core = (TH1F *)gPad->GetPrimitive("htot0core");

  sprintf(char_cut,"comb_type==12",qualcut);  
  trk->Draw("hit_tot[1]>>htot1all",char_cut);
  TH1F *htot1all = (TH1F *)gPad->GetPrimitive("htot1all");

  sprintf(char_cut,"comb_type==12 && qual_track<%.0f",qualcut);  
  trk->Draw("hit_tot[1]>>htot1core",char_cut);
  TH1F *htot1core = (TH1F *)gPad->GetPrimitive("htot1core");

  sprintf(char_cut,"comb_type==34",qualcut);  
  trk->Draw("hit_tot[0]>>htot2all",char_cut);
  TH1F *htot2all = (TH1F *)gPad->GetPrimitive("htot2all");
  
  sprintf(char_cut,"comb_type==34 && qual_track<%.0f",qualcut);  
  trk->Draw("hit_tot[0]>>htot2core",char_cut);
  TH1F *htot2core = (TH1F *)gPad->GetPrimitive("htot2core");

  sprintf(char_cut,"comb_type==34",qualcut);  
  trk->Draw("hit_tot[1]>>htot3all",char_cut);
  TH1F *htot3all = (TH1F *)gPad->GetPrimitive("htot3all");
  
  sprintf(char_cut,"comb_type==34 && qual_track<%.0f",qualcut);  
  trk->Draw("hit_tot[1]>>htot3core",char_cut);
  TH1F *htot3core = (TH1F *)gPad->GetPrimitive("htot3core");


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
  
  //////////////////////////////////////    
  // t_rms
  
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
  S->Divide(1,2);
  S->cd(1);
  sprintf(char_cut,"qual_track<%.0f && abs(t_10)<200 && hit_layer_id[0]==1 && hit_ladder_id[0]==1",qualcut);  
  trk->Draw("t_10",char_cut);
  S->cd(2);
  sprintf(char_cut,"qual_track<%.0f && abs(t_32)<200 && hit_layer_id[0]==1 && hit_ladder_id[0]==1",qualcut);  
  trk->Draw("t_32",char_cut);
  S->Print(out_file_);
  
  S->Clear();
  S->Divide(1,2);
  S->cd(1);
  sprintf(char_cut,"qual_track<%.0f && abs(t_10)<200 && hit_layer_id[0]==1 && hit_ladder_id[0]==2 ",qualcut);  
  trk->Draw("t_10",char_cut);
  S->cd(2);
  sprintf(char_cut,"qual_track<%.0f && abs(t_32)<200 && hit_layer_id[0]==1 && hit_ladder_id[0]==2 ",qualcut);  
  trk->Draw("t_32",char_cut);
  S->Print(out_file_);
  
  /////////////////////////////////////////////////////////////////////  

  S->Clear();
  S->Divide(2,1);
  S->cd(1);
  trk->Draw("ncomb2","","BOX");
  S->cd(2);
  trk->Draw("ncomb2:qual_track","","BOX");

  S->Print(out_file_);

  //
  S->Clear();
  S->Divide(2,1);
  S->cd(1);
  trk->Draw("trk_phi","","BOX");
  S->cd(2);
  trk->Draw("trk_theta","","BOX");

  S->Print(out_file_);
  
  //
  S->Clear();
  S->Divide(2,1);
  S->cd(1);
  trk->Draw("hit_phi_incline","","BOX");
  S->cd(2);
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
