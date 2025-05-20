Float_t LINE_WIDTH = 2.5;
Float_t TITLE_SIZE = 0.04;
Float_t MARKER_SIZE = 1.2;
Float_t AXISTITLE_OFFSET = 0.8;
Float_t YAXISTITLE_OFFSET = 1.2;
Float_t max_range;
Float_t min_range;
Float_t ENTRY_SEP = 0.05;
Float_t FILL_STYLE = 0;
Float_t BORDER_SIZE = 0;
Float_t TEXT_ALIGN = 32;
Float_t TEXT_ALIGN_1 = 12;
Float_t TEXT_FONT = 62;
Float_t TEXT_SIZE = 0.03;
Float_t Xl1 = 0.16, Yu1 = 0.87, Xl2 = 0.36, Yu2 = 0.95;
Float_t Xr1 = 0.5, Ym1 = 0.5, Xr2 = 0.899, Ym2 = 0.8;
Float_t LEFT_MARGIN = 0.15;
Float_t MARKER_STYLE = 6;


TLegend* leg = nullptr;

void Legends()
{
leg  = new TLegend(Xl1, Ym2, Xl2, Yu1);
leg->AddEntry((TObject*)0, "mu3e preliminary","");
leg->AddEntry((TObject*)0, "Cosmic Minimal setup","");
leg->SetFillStyle(FILL_STYLE);
leg->SetBorderSize(BORDER_SIZE);
leg->SetTextSize(TEXT_SIZE);
leg->SetTextFont(TEXT_FONT);
leg->SetTextAlign(kHAlignCenter+kVAlignCenter);
}
