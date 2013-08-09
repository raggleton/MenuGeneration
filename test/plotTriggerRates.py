#! /usr/bin/python
# shebang was previously /opt/local/bin/python


from ROOT import TH1D, TCanvas, TPad, TAxis, TFile, TLegend
import math

#gROOT.Reset()


class TriggerRatePlot:
	"""Class that takes two trigger rate histograms, plots them and plots the ratio of the two underneath"""
	instantiationCount = 0
	canvas = None
	ratioPad = None
	ratePad = None
	ratio = None
	numerator = None
	denominator = None
	legend = None
	
	def __init__( self, titleAndFile1, titleAndFile2, triggerName ) :
		file1 = titleAndFile1[1]
		file2 = titleAndFile2[1]
		title1 = titleAndFile1[0]
		title2 = titleAndFile2[0]
		self.instantiationCount=TriggerRatePlot.instantiationCount
		TriggerRatePlot.instantiationCount+=1
		
		self.numerator=self.loadHistogram( file1, triggerName ).Clone()
		self.numerator.SetName( self.numerator.GetName()+"_numerator"+str(self.instantiationCount) )
		self.numerator.SetDirectory( 0 )
		if file2!=None :
			self.denominator=self.loadHistogram( file2, triggerName ).Clone()
			self.denominator.SetName( self.denominator.GetName()+"_denominator"+str(self.instantiationCount) )
			self.denominator.SetDirectory( 0 )
		
		
		splitPoint=0.33
		padRatio=(1-splitPoint)/splitPoint
		
		self.canvas=TCanvas()
		if self.denominator!=None : 
			self.ratePad=TPad( "ratePad"+str(self.instantiationCount), "L1 rate", 0, splitPoint, 1, 1 )
			self.ratioPad=TPad( "ratioPad"+str(self.instantiationCount), "L1 ratio", 0, 0, 1, splitPoint )
			self.ratioPad.SetGrid();
		else :
			self.ratePad=TPad( "ratePad"+str(self.instantiationCount), "L1 rate", 0, 1, 1, 1 )
			
		#self.numerator.GetXaxis().SetNdivisions(520);
		#self.numerator.GetYaxis().SetNdivisions(520);
		self.ratePad.SetGrid();
		
		if self.ratioPad!=None:
			self.ratioPad.SetTopMargin(0)
			self.ratioPad.SetBottomMargin( self.ratePad.GetBottomMargin()*padRatio*1.1 ) # A little 10% extra because some is cut off
		self.ratePad.SetBottomMargin(0.1) # was zero before I wanted to see the axis
		self.ratePad.Draw()
		if self.ratioPad!=None: self.ratioPad.Draw()
		self.ratePad.cd()
		
		self.ratePad.SetLogy()
#		histogramTitle=self.numerator.GetTitle()
#		try:
#			titleSplit=histogramTitle.split("_")
#			title=titleSplit[1];
#			for section in titleSplit[2:-1]: title=title+"_"+section
#			self.numerator.SetTitle( title )
#		except:
#			pass # leave the title as it is
		self.numerator.GetYaxis().SetTitle( "Rate/kHz" )
		self.numerator.SetStats(0)
		self.numerator.SetLineColor(2)
		self.numerator.SetMarkerColor(2)
		#self.numerator.GetYaxis().SetTitleSize( self.numerator.GetYaxis().GetTitleSize()*1.5 )
		#self.numerator.GetXaxis().SetTitleSize( self.numerator.GetXaxis().GetTitleSize()*1.5 )
		self.numerator.GetYaxis().SetTitleSize( 0.053 );
		self.numerator.GetXaxis().SetTitleSize( 0.053 );
		delayFirstDraw=False
		if self.denominator!=None :
			self.denominator.SetTitle( self.numerator.GetTitle() )
			self.denominator.GetYaxis().SetTitle( self.numerator.GetYaxis().GetTitle() )
			self.denominator.GetYaxis().SetTitleSize( self.numerator.GetYaxis().GetTitleSize() );
			self.denominator.GetXaxis().SetTitleSize( self.numerator.GetXaxis().GetTitleSize() );
			if self.denominator.GetBinContent(1) > self.numerator.GetBinContent(1) : delayFirstDraw=True
		if not delayFirstDraw : self.numerator.Draw()
		
		if self.denominator!=None :
			self.denominator.SetStats(0)
			self.denominator.SetLineColor(4)
			self.denominator.SetMarkerColor(4)
			if delayFirstDraw :
				self.denominator.Draw()
				self.numerator.Draw("same")
			else :
				self.denominator.Draw("same")
	
		
			self.ratio = TH1D( "ratio"+str(self.instantiationCount), "", self.numerator.GetXaxis().GetNbins(), self.numerator.GetXaxis().GetXmin(), self.numerator.GetXaxis().GetXmax()  )
			self.ratio.SetDirectory( 0 )
			for binNumber in range( 1, self.numerator.GetXaxis().GetNbins()+1 ):
				#print self.numerator.GetBinContent(binNumber),"/",self.denominator.GetBinContent(binNumber),"=",self.numerator.GetBinContent(binNumber)/self.denominator.GetBinContent(binNumber)
				try : 
					rateRatio=self.numerator.GetBinContent(binNumber)/self.denominator.GetBinContent(binNumber)
					error=rateRatio*math.sqrt( math.pow(self.numerator.GetBinError(binNumber)/self.numerator.GetBinContent(binNumber),2) + math.pow(self.denominator.GetBinError(binNumber)/self.denominator.GetBinContent(binNumber),2) )
				except ZeroDivisionError:
					rateRatio = 0
					error = 0
				self.ratio.SetBinContent( binNumber, rateRatio )
				self.ratio.SetBinError( binNumber, error )
		
			self.ratioPad.cd()
			self.ratio.SetStats(0)
			self.ratio.GetXaxis().SetLabelSize( self.numerator.GetXaxis().GetLabelSize()*padRatio )
			self.ratio.GetYaxis().SetLabelSize( self.numerator.GetYaxis().GetLabelSize()*padRatio )
			self.ratio.GetXaxis().SetTitle( "Threshold" )
			self.ratio.GetYaxis().SetTitle( "Ratio " )
			self.ratio.GetXaxis().SetTitleSize( self.numerator.GetXaxis().GetTitleSize()*padRatio )
			self.ratio.GetYaxis().SetTitleSize( self.numerator.GetYaxis().GetTitleSize()*padRatio )
			self.ratio.GetYaxis().SetTitleOffset( self.numerator.GetYaxis().GetTitleOffset()/padRatio )
			self.ratio.Draw()
		
		self.ratePad.cd()
		self.legend=TLegend( 0.59, 0.72*0.9, 0.99, 0.95*0.9 )
		self.legend.SetTextSize( 0.04 )
		self.legend.AddEntry( self.numerator, title1 )
		if self.denominator!=None : self.legend.AddEntry( self.denominator, title2 )
		self.legend.Draw()
	
	def loadHistogram( self, file, triggerName ) :
		histogram=file.Get( "h_"+triggerName+"_byThreshold" )
		if histogram != None : return histogram
		
		histogram=file.Get( "L1_"+trigger+"_v_allThresholdsScaled" )
		if histogram != None : return histogram
		
		histogram=file.Get( "L1_"+trigger+"_v_threshold1" )
		if histogram != None : return histogram
		
		print "Couldn't load file for trigger "+triggerName
		histogram=file.Get( "L1_SingleEG_v_threshold1" )
		if histogram != None : return histogram

		file.ls()
		raise Exception( "Coudln't load the histogram for trigger "+triggerName )
	
	def draw( self ) :
		self.canvas.cd()
		self.ratePad.Draw()
		self.ratioPad.Draw()
		self.ratePad.cd()
		self.numerator.Draw()
		self.denominator.Draw("same")
		self.ratioPad.cd()
		self.ratio.Draw()
		




newFile = ["New file",TFile.Open("/home/xtaldaq/CMSSWReleases/CMSSW_5_3_4/src/MarksStuff/MenuGeneration/reducedRateHistograms.root")]
oldFile = ["Old file",TFile.Open("/home/xtaldaq/L1RateHist_14TeV100PU_25ns50bxMC_FallbackThr1_rates.root")]
plots = []

#triggers = ["SingleEG"]
triggers = ["SingleEG","SingleIsoEG","SingleMu",
"SingleIsoMu","SingleTau","SingleIsoTau","isoEG_EG","isoMu_Mu","isoTau_Tau",
"isoEG_Mu","isoMu_EG","isoEG_Tau","isoMu_Tau",
"SingleJetC",
"DoubleJet","QuadJetC","SixJet","SingleIsoEG_CJet",
#"SingleMu_CJet",
"SingleIsoEG_HTM","SingleMu_HTM","HTM","HTT"
]
for trigger in triggers:
	plots.append( TriggerRatePlot( newFile, oldFile, trigger ) )
	plots[-1].canvas.SaveAs( trigger+"_rateVsThreshold.pdf" )
	#plots.append( TriggerRatePlot( data_8TeV_45PU_barrel, MC_8TeV_45PU_barrel, "h_"+trigger+"_byThreshold" ) )
	#plots[-1].canvas.SaveAs( trigger+"_45PU_barrelOnly.pdf" )


