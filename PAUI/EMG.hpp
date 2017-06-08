

#ifndef EMG_HPP
#define EMG_HPP

#include "OGLGraph.h"
#include "stdafx.h"

class EMG
{
private:
	static EMG* _instance;
	OGLGraph* graphObj;

	std::list<float> _listSignal;
	std::list<float> _oEMGList, _pEMGList, _meanEMGList, _pEMG150List;
	std::list<float>::const_iterator _it;

	double _mean			= 0.0;
	double _SD				= 0.0; 
	double _sum				= 0.0;

	bool firstTime			= true;
	
	int frameCounter		= 0;

	float _signalArray[5];

public:
	float displayThreshold = 0;
	// EMG Data 
	int totalTime = 0, numOfPeaks = 0, totalPeakTime = 0, numOfPlains = 0, totalPlainTime = 0;
	float maxPeakMagnitude = 0.0, currentpeakMag = 0.0;
	bool peakStart = false;

	static EMG* Instance();
	void processEMGSignal(BITalino::VFrame);
	void processEMGSignal(double frame[]);
	void processEMGSignal(void);
	inline void processingBlock();
};

#endif