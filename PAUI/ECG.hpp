

#ifndef ECG_hpp
#define ECG_hpp

#include "fftw3.h"
#include "OGLGraph.h"
#include "stdafx.h"

class ECG
{
private:
	static ECG* _instance;
	OGLGraph* graphObj;
	int samplingFrequency = 1000;
	bool m_peakFlag = false, applyBandpassFilter = false;

	double channelGainsECG = 100;
	double lowpassCutoffFrequency = 5, highpassCutoffFrequency = 40, previousSignal = 0, previousFilteredSignal_lp = 0, previousFilteredSignal_hp = 0;
	double m_maximum, m_peakThreshold = 0, m_peakTimer = 0, m_heartRates[100], currentHR = 0, m_pi = 3.14159265358979f, scale = 3;

	int arrayLengthRRI = 16, frameLengthCounter = 0, frequencyRRILimit = 23;
	
	int _rriFound = 0;

	std::list<double> arrayRRIForTimeDomain, arrayRRIForFrequencyDomain;
	double _preRRI = 0;

public:
	// ECG Data
	double _RRI = 0.0, _RMSSD = 0.0, _heartRate = 0.0;
	double _mean = 0.0, _SDNN = 0.0, _deltaMean = 0.0, _deltaSD = 0.0; //-- delta items are used for calculations on 2 successive RRI
	double _LF = 0.0, _HF = 0.0, _VLF = 0.0;

	float displayThreshold = 0;
	static ECG* Instance();
	void processECGSignal(BITalino::VFrame);
	void processECGSignal(double frame[]);
	inline void ECGTimeDomain();
	inline void ECGFrequencyDomain();
	inline void processingBlock(double F, double signal, int i);
	inline void finalizingThings();
	inline double applyBandPassFilter(double signal, int i);
};

#endif