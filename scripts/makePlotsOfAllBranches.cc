/*

    Plot all the branches in a tree froma .root file

    Usage : 

    root -b -q 'makePlaotsOfAllBranches.cc("<pathToRootFile>","<nameOfTTreeInFile>","<prefixToThePicToSave>")"

    eg. : user@pc $ root -b -q 'makePlaotsOfAllBranches.cc("data/bmm5sample.root","Events","outputs/plots/test/") 
                

*/


void setCanvasStyle(TCanvas *c);
void setGlobalStyles();
void putCMStags(TCanvas *c);
void addLegend(TCanvas *c,TLegend **legend_cms=nullptr,TH1* hist=nullptr,string label="labelX",float x1=0.55,float y1=0.82,float x2=0.88, float y2=0.88, string options="lp");
void addText(TCanvas *c,float x=0.16,float y =0.72,string text="sample text",EColor col=kBlack,float fontSize=0.03,int font=42);
void saveCanvas(TCanvas *c1, string name="sample",string prefix="plots/", string type=".png");
void plotHist(TCanvas * c1,TH1* ahist,TString opts,EColor col=kBlue,int lwidth=2,bool putStats=0,bool clear=1,float maximum=1.15);

void makePlotsOfAllBranches(string fname="",string treeName="",string prefix="") 
{
   // string fname="/home/aravind/cernbox/work/bs2mumug/analysis2018/bmm5/outputs/photonDrDistribution.root";
   // string prefix="/home/aravind/cernbox/work/bs2mumug/analysis2018/bmm5/outputs/plots/";

    TFile * ifile= new TFile(fname.c_str(),"read");
    TTree * aTree = (TTree* ) ifile->Get(treeName.c_str());
    TCanvas *c1 = new TCanvas("c", "c",473,281,800,727);
	TLegend *alegend=nullptr;
	setGlobalStyles();
   
    const int nq=2;
    const int NBINS=20;
    Double_t xq[nq]={0.01,0.98};  // position where to compute the quantiles in [0,1]
    Double_t yq[nq];              // array to contain the quantiles

    string branchName,tmp;
    float min,max;
    TH1F* ahist;

    auto BranchList = (TObjArray *) aTree->GetListOfBranches();
    auto nentries= BranchList->GetEntries();
    for (int i=0;i<nentries;i++) 
    {
        branchName=BranchList->At(i)->GetName();
        std::cout<<" Ploting "<< branchName<<"  "<<i<<" / "<<nentries<<"\n"; 
        tmp=branchName+" >> th1_tmp";
        aTree->Draw( tmp.c_str() );
        
        auto hist_raw=(TH1*) gROOT->FindObject("th1_tmp");
        hist_raw->GetQuantiles(nq,yq,xq);
        delete hist_raw;
        min = yq[0]> 0 ? yq[0]*0.80 : yq[0]*1.20;
        max = yq[1]> 0 ? yq[1]*1.20 : yq[1]*0.80;

        ahist = new TH1F(branchName.c_str(),branchName.c_str(),NBINS,min,max);
        aTree->Project(ahist->GetName(),branchName.c_str());
	    
        if (ahist==nullptr) return;
        c1->Clear();
        
	    ahist->SetTitle(";; Arb. Unit");
	    //ahist->GetXaxis()->SetRangeUser(0., maxX);
	    //ahist->Scale(1.0/ahist->Integral());
	    auto maxY=ahist->GetMaximum()*1.3;
	    addLegend(c1,&alegend,ahist,branchName.c_str(),0.15,0.83,0.8,0.88);
	    plotHist(c1,ahist,"HIST",kBlue,2,1,1,maxY);
        alegend->Draw();
	    
        saveCanvas(c1,ahist->GetName(),prefix,".png");
	    saveCanvas(c1,ahist->GetName(),prefix,".pdf");
	    
	    delete alegend;alegend=nullptr;

    }
    return;
}



///                            PLOTING MACROS                        ///

void plotHist(TCanvas * c1,TH1* ahist,TString opts,EColor col=kBlue,int lwidth=2,bool putStats=0,bool clear=1,float maximum=1.15)
{
	
	c1->cd();
	
	ahist->SetLineWidth(lwidth);
	ahist->SetLineColor(col);
	if(maximum >-1e10) ahist->SetMaximum(maximum);
	if(not putStats) ahist->SetStats(0);
	
	if(clear)  ahist->Draw(opts);
	else       ahist->Draw("same");
    if(putStats){
    TPaveStats *st = (TPaveStats*)gROOT->FindObject("stats");
    if( st){
    st->SetX1NDC(0.1); //new x start position
    st->SetY1NDC(0.1);}
    }
	setCanvasStyle(c1);
	putCMStags(c1);

}

   //Related to the Plotting style
void setGlobalStyles()
{
   gStyle->SetLineWidth(2);
   

   /*
        kWhite  = 0,   kBlack  = 1,   kGray    = 920,  kRed    = 632,  kGreen  = 416,
        kBlue   = 600, kYellow = 400, kMagenta = 616,  kCyan   = 432,  kOrange = 800,
        kSpring = 820, kTeal   = 840, kAzure   =  860, kViolet = 880,  kPink   = 900
   */
   Int_t palette[6];
   palette[0] = 600;
   palette[1] = 632;
   palette[2] = 416;
   palette[3] = 432;
   palette[4] = 616;
   palette[5] = 1;

   gStyle->SetPalette(6,palette);
   gStyle->SetOptStat("eou");
   
   gStyle->SetStatX(0.93);
   gStyle->SetStatY(0.91);

}

void setCanvasStyle(TCanvas *c)
{
   //Canvas size according to the FTR
   c->SetCanvasSize(800,727);
   c->Range(-0.1,-0.1,50.0,1.2);
   c->SetFillColor(0);
   c->SetBorderMode(0);
   c->SetBorderSize(2);
   c->SetGridx();
   c->SetGridy();
   c->SetTickx(1);
   c->SetTicky(1);
   c->SetLeftMargin(0.12);
   c->SetRightMargin(0.05);
   c->SetTopMargin(0.08);
   c->SetBottomMargin(0.08);
   c->SetFrameLineWidth(2);
   c->SetFrameBorderMode(0);
   c->SetFrameLineWidth(2);
   c->SetFrameBorderMode(0);

}

void addText(TCanvas *c,string text="sample text",float x=0.16,float y =0.72,EColor col=kBlack,float fontSize=0.03,int font=42)
{
   c->cd();
 // Definit of Trigger name //
   TLatex * text_cms_trig = new TLatex(x,y,text.c_str());
   text_cms_trig->SetNDC();
   text_cms_trig->SetTextFont(font);
   text_cms_trig->SetTextSize(fontSize);
   text_cms_trig->SetTextColor(col);
   text_cms_trig->Draw("same");
   


}


void putCMStags(TCanvas *c)
{ 
 
   addText(c,"#bf{CMS RUN2} #it{Simulation Preliminary} ",0.125,0.94,kBlack,0.04,42);
   addText(c,"#bf{#sqrt{s} = 13 TeV}",0.75,0.94,kBlack,0.04,42);
 //  addText(c,"#it{Simulation Preliminary}",0.16,0.84,kBlack,0.04,42);
   return;
 // CMS Name Label on top bar//

}

void addLegend(TCanvas *c,TLegend ** alegend=nullptr,TH1* hist=nullptr,string label="labelX",float x1=0.55,float y1=0.82,float x2=0.88, float y2=0.88, string options="lp")
{
   c->cd();

 //Legend definiation//
   if(hist==nullptr) return ;

   if (*alegend==nullptr)
   	*alegend = new TLegend(x1,y1,x2,y2,"","NDC");
   auto legend_cms= *alegend;
   legend_cms->AddEntry(hist,label.c_str() , options.c_str());
   legend_cms->SetFillStyle(0);
   legend_cms->SetTextFont(42);
   legend_cms->SetTextSize(0.04);
   legend_cms->SetFillColor(0);
   legend_cms->SetBorderSize(0);
   legend_cms->Draw("same");

}
void saveCanvas(TCanvas *c1, string name="sample",string prefix="plots/", string type=".png")
{
   	c1->Update();
	c1->SaveAs((prefix+name+type).c_str());
}

