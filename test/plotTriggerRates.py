#! /usr/bin/python
# shebang was previously /opt/local/bin/python


from ROOT import TH1D, TCanvas, TPad, TAxis, TFile, TLegend
import math

#gROOT.Reset()

class TriggerRate(object):
	"""
	Class that will plot one or more trigger rate histograms into the TPad provided.
	
	Author Mark Grimes (mark.grimes@bristol.ac.uk)
	Date 02/Sep/2013
	"""
	instantiationCount = 0 # Need this to get around root's stupid requirement for globally unique names
	
	def __init__( self, pad ) :
		self.instantiationCount=TriggerRate.instantiationCount
		TriggerRate.instantiationCount+=1

		self.histograms=[]
		# Create a legend. This might not actually be plotted if drawLegend is changed to False
		self.legend=TLegend( 0.59, 0.72*0.9, 0.99, 0.95*0.9 )
		self.legend.SetTextSize( 0.04 )
		self.drawLegend=True
		self.pad=pad
		#self.pad.cd()
		self.pad.SetGrid()
		self.pad.SetLogy()
		self.maxiumumBinHeight=0
		self.axisLabelSize=None
		self.axisTitleSize=None
		self.axisTitleOffset=None

	def add( self, histogram, legendTitle=None ):
		availableColours=[2,4,5,6,7,8,9,10]
		if legendTitle==None: legendTitle=histogram.GetTitle()
		self.histograms.append( histogram.Clone() )
		# Make sure the copy has a globally unique name
		self.histograms[-1].SetName( self.histograms[-1].GetName()+"_hist"+str(len(self.histograms))+"inst"+str(self.instantiationCount) )
		self.histograms[-1].SetDirectory( 0 ) # Make sure the copy isn't saved to disk
		self.histograms[-1].SetLineColor( availableColours[(len(self.histograms)-1)%len(availableColours)] )
		self.histograms[-1].SetMarkerColor( availableColours[(len(self.histograms)-1)%len(availableColours)] )
		self.histograms[-1].SetStats(0)
		if self.axisLabelSize==None:
			self.axisLabelSize=self.histograms[-1].GetXaxis().GetLabelSize()
		if self.axisTitleSize==None:
			self.axisTitleSize=self.histograms[-1].GetXaxis().GetTitleSize()
		if self.axisTitleOffset==None:
			self.axisTitleOffset=self.histograms[-1].GetYaxis().GetTitleOffset()
		self.histograms[-1].GetYaxis().SetTitle( "Rate/kHz" )
		self.legend.AddEntry( self.histograms[-1], legendTitle )
		# See if the histogram maximum is larger than the current maximum
		bin=self.histograms[-1].GetMaximumBin()
		try: max=self.histograms[-1].GetBinContent(bin)+self.histograms[-1].GetBinErrorUp(bin)
		except: max=self.histograms[-1].GetBinContent(bin)+self.histograms[-1].GetBinError(bin)
		if max>self.maxiumumBinHeight: self.maxiumumBinHeight=max
		
	def draw( self ) :
		self.pad.cd()
		for index in range(0,len(self.histograms)):
			self.histograms[index].SetMaximum( self.maxiumumBinHeight*1.2 )
			self.histograms[index].GetYaxis().SetTitleSize( self.axisTitleSize );
			self.histograms[index].GetYaxis().SetTitleOffset( self.axisTitleOffset );
			self.histograms[index].GetXaxis().SetTitleSize( self.axisTitleSize );
			if index==0: self.histograms[index].Draw()
			else: self.histograms[index].Draw("same")
		if self.drawLegend: self.legend.Draw()
		self.pad.Update()

class TriggerRateRatio(object):
	"""
	Class that will plot a ratio between two histograms into the TPad provided.
	Author Mark Grimes (mark.grimes@bristol.ac.uk)
	Date 14/Sep/2013
	"""
	instantiationCount = 0 # Need this to get around root's stupid requirement for globally unique names
	
	def __init__( self, pad ):
		self.instantiationCount=TriggerRateRatio.instantiationCount
		TriggerRateRatio.instantiationCount+=1

		self.ratioHistograms=[]
		self.maxiumumBinHeight=0;
		self.minimumBinHeight=999999999;

		self.pad=pad
		self.pad.SetGrid()
		
		self.axisLabelSize=None
		self.axisTitleSize=None
		self.axisTitleOffset=None
	
	def add( self, numeratorHistogram, denominatorHistogram, legendTitle=None ):
		availableColours=[1,2,4,5,6,7,8,9,10]
		
		numberOfBins=numeratorHistogram.GetXaxis().GetNbins()
		lowEdge=numeratorHistogram.GetXaxis().GetXmin()
		highEdge=numeratorHistogram.GetXaxis().GetXmax()
		self.ratioHistograms.append( TH1D( "TriggerRateRatio"+str(self.instantiationCount)+"_ratio", "", numberOfBins, lowEdge, highEdge ) )
		self.ratioHistograms[-1].SetDirectory( 0 ) # Make sure the copy isn't saved to disk
		self.ratioHistograms[-1].SetLineColor( availableColours[(len(self.ratioHistograms)-1)%len(availableColours)] )
		self.ratioHistograms[-1].SetMarkerColor( availableColours[(len(self.ratioHistograms)-1)%len(availableColours)] )
		self.ratioHistograms[-1].SetStats(0)
		self.ratioHistograms[-1].GetXaxis().SetTitle( "Threshold" )
		self.ratioHistograms[-1].GetYaxis().SetTitle( "Ratio " )
		#self.ratioHistograms[-1].GetYaxis().SetTitleOffset( self.numerator.GetYaxis().GetTitleOffset()/padRatio )
		if self.axisLabelSize==None:
			self.axisLabelSize=self.ratioHistograms[-1].GetXaxis().GetLabelSize()
		if self.axisTitleSize==None:
			self.axisTitleSize=self.ratioHistograms[-1].GetXaxis().GetTitleSize();
		if self.axisTitleOffset==None:
			self.axisTitleOffset=self.ratioHistograms[-1].GetYaxis().GetTitleOffset()


		for binNumber in range( 1, numberOfBins+1 ):
			try : 
				rateRatio=numeratorHistogram.GetBinContent(binNumber)/denominatorHistogram.GetBinContent(binNumber)
				error=rateRatio*math.sqrt( math.pow(numeratorHistogram.GetBinError(binNumber)/numeratorHistogram.GetBinContent(binNumber),2) + math.pow(denominatorHistogram.GetBinError(binNumber)/denominatorHistogram.GetBinContent(binNumber),2) )
			except ZeroDivisionError:
				rateRatio = 0
				error = 0
			self.ratioHistograms[-1].SetBinContent( binNumber, rateRatio )
			self.ratioHistograms[-1].SetBinError( binNumber, error )
		# Work out the maximum and minimums
		bin=self.ratioHistograms[-1].GetMaximumBin()
		try: max=self.ratioHistograms[-1].GetBinContent(bin)+self.ratioHistograms[-1].GetBinErrorUp(bin)
		except: max=self.ratioHistograms[-1].GetBinContent(bin)+self.ratioHistograms[-1].GetBinError(bin)
		if max>self.maxiumumBinHeight: self.maxiumumBinHeight=max
		bin=self.ratioHistograms[-1].GetMinimumBin()
		try: min=self.ratioHistograms[-1].GetBinContent(bin)+self.ratioHistograms[-1].GetBinErrorDown(bin)
		except: min=self.ratioHistograms[-1].GetBinContent(bin)+self.ratioHistograms[-1].GetBinError(bin)
		if min<self.minimumBinHeight: self.minimumBinHeight=min


	def draw(self):
		self.pad.cd()
		for index in range(0,len(self.ratioHistograms)):
			self.ratioHistograms[index].SetMaximum( self.maxiumumBinHeight*1.2 )
			self.ratioHistograms[index].SetMinimum( self.minimumBinHeight*0.8 )
			self.ratioHistograms[index].GetYaxis().SetTitleSize( self.axisTitleSize );
			self.ratioHistograms[index].GetXaxis().SetTitleSize( self.axisTitleSize );
			self.ratioHistograms[index].GetYaxis().SetLabelSize( self.axisLabelSize );
			self.ratioHistograms[index].GetXaxis().SetLabelSize( self.axisLabelSize );
			self.ratioHistograms[index].GetYaxis().SetTitleOffset( self.axisTitleOffset );
			if index==0: self.ratioHistograms[index].Draw()
			else: self.ratioHistograms[index].Draw("same")
		self.pad.Update()

class TriggerRateComparisonPlot(object):
	"""
	Class that puts a TriggerRate object and TriggerRateRatio object on the same page.
	"""
	instantiationCount = 0 # Need this to get around root's stupid requirement for globally unique names

	def __init__(self):
		self.instantiationCount=TriggerRateComparisonPlot.instantiationCount
		TriggerRateComparisonPlot.instantiationCount+=1

		splitPoint=0.33
		self.padRatio=(1-splitPoint)/splitPoint

		self.canvas=TCanvas()
		# Use the instantiation count to give every instance unique names for the pads
		self.ratePad=TPad( "TriggerRateComparisonPlot("+str(self.instantiationCount)+").ratePad", "L1 rate", 0, splitPoint, 1, 1 )
		self.ratioPad=TPad( "TriggerRateComparisonPlot("+str(self.instantiationCount)+").ratioPad", "L1 ratio", 0, 0, 1, splitPoint )
		self.ratePad.Draw()
		self.ratioPad.Draw()
		self.ratioPad.SetTopMargin(0)
		self.ratioPad.SetBottomMargin( self.ratePad.GetBottomMargin()*self.padRatio*1.1 ) # A little 10% extra because some is cut off
		self.ratePad.SetBottomMargin(0.1) # was zero before I wanted to see the axis

		self.ratePlot=TriggerRate(self.ratePad)
		self.ratioPlot=TriggerRateRatio(self.ratioPad)
		
		

	def add( self, histogram, legendName=None ):
		self.ratePlot.add( histogram, legendName )
		if len(self.ratePlot.histograms)>1:
			self.ratioPlot.add( self.ratePlot.histograms[0], histogram )
			
	def addByTriggerName( self, file, triggerName, legendName=None ):
		# Try the old format with the name used in L1Menu2015.C macro
		histogram=file.Get( "h_"+triggerName+"_byThreshold" )
		if histogram != None : return self.add( histogram, legendName )
		
		# Try the name given when there is more than one threshold and they're all scaled
		histogram=file.Get( "L1_"+triggerName+"_v_allThresholdsScaled" )
		if histogram != None : return self.add( histogram, legendName )
		
		# Try the name when there is only one threshold
		histogram=file.Get( "L1_"+triggerName+"_v_threshold1" )
		if histogram != None : return self.add( histogram, legendName )
	
	def draw(self):
		self.ratePlot.axisTitleSize=0.053
		self.ratioPlot.axisLabelSize=self.ratePlot.axisLabelSize*self.padRatio
		self.ratioPlot.axisTitleSize=self.ratePlot.axisTitleSize*self.padRatio
		self.ratioPlot.axisTitleOffset=self.ratePlot.axisTitleOffset/self.padRatio
		self.ratePlot.draw()
		self.ratioPlot.draw()
		self.canvas.Update()




#triggers = ["SingleEG"]
triggers = ["SingleEG","SingleIsoEG","SingleMu",
"SingleIsoMu","SingleIsoTau","isoEG_EG","isoMu_Mu","isoTau_Tau",
"isoEG_Mu","isoMu_EG","isoMu_Tau","SingleJetC","DoubleJet",
"SingleMu_CJet",
"SingleIsoEG_HTM","SingleMu_HTM","HTM","HTT",
"QuadJetC","SixJet","SingleIsoEG_CJet","SingleIsoEG_CJet2","SingleTau","isoEG_Tau"
]

plots = []

#newFile=TFile.Open("/home/xtaldaq/CMSSWReleases/CMSSW_5_3_4/src/rateHistograms-logicTest.root")
#oldFile=TFile.Open("/home/xtaldaq/MenuGenerationFiles/oldResults/L1RateHist_14TeV100PU_25ns50bxMC_FallbackThr1_rates-newBinning.root")
newFile=TFile.Open("/home/xtaldaq/MenuGenerationFiles/oldResults/rateHistograms-fullSample.root")
oldFile=TFile.Open("/home/xtaldaq/CMSSWReleases/CMSSW_5_3_4/src/rateHistograms.root")
newFileTitle="FullSample file"
oldFileTitle="ReducedSample file"

for trigger in triggers:
	plots.append( TriggerRateComparisonPlot() )
#	if trigger!="SingleIsoEG_CJet":
#		plots[-1].addByTriggerName( newFile, trigger, newFileTitle )
#	else:
#		plots[-1].add( newFile.Get("L1_SingleIsoEG_CJet2_v_allThresholdsScaled"), newFileTitle )
	plots[-1].addByTriggerName( newFile, trigger, newFileTitle )
	plots[-1].addByTriggerName( oldFile, trigger, oldFileTitle )
	plots[-1].draw()
	# Add an extra member to say where to save, in case I choose to do so later
	plots[-1].saveFilename=trigger+"_rateVsThreshold.pdf"
	#plots[-1].canvas.SaveAs( trigger+"_rateVsThreshold.pdf" )

