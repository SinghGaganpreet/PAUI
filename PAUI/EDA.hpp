
#ifndef EDA_HPP
#define EDA_HPP

#include "bitalino.h"
#include <List>
#include "stdafx.h"

class EDA
{
private:
	static EDA* _instance;
	std::list<float> _listSignal;
	std::list<float> _sclList;
	std::list<float>::const_iterator _iterUniversal;

	bool firstTime = true;


	int frameCounter = 0, samplingFrequency = 1000;
	float movingAverage = 0, m_peakTimer = 0;

public:
	float _SCR = 0.0, _SD = 0.0, _SCL = 0.0;

	float displayThreshold = 0;
	static EDA* Instance();
	void processEDASignal(BITalino::VFrame);
	void processEDASignal(float frame[]);
};

#endif