
#ifndef FIXATION_DATA_CLASS_hpp
#define FIXATION_DATA_CLASS_hpp

#include "stdafx.h"

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include "WriteToFile.hpp"

using namespace std;

#define VISULIZE_FIXATION_DATA 0;




class FDC
{
private:
	inline void writeData()
	{
		if (dtfObj == nullptr)
		{
			dtfObj = new DTF();
			string data = "#TimeStamp(in miliSec);avgFixationDuration;biggestFixationAt;biggestFixationDuration;maxVisitedAt;maxVisitsCount;numberOfFixations;repeatedFixations;totalFixationDuration;totalTime\n";
			dtfObj->writeData(data, fileToWriteEYE);
		}
		else
		{
			std::chrono::steady_clock::time_point current = std::chrono::steady_clock::now();
			string data = to_string(std::chrono::duration_cast<std::chrono::milliseconds>(current - startClock).count()) + ";" + to_string(gdsObj.avgFixationDuration) + ";" + gdsObj.biggestFixationAt
				+ ";" + to_string(gdsObj.biggestFixationDuration) + ";" + gdsObj.maxVisitedAt + ";" + to_string(gdsObj.maxVisitsCount) + ";" + to_string(gdsObj.numberOfFixations)
				+ ";" + to_string(gdsObj.repeatedFixations) + ";" + to_string(gdsObj.totalFixationDuration) + ";" + gdsObj.totalTime + "\n";

			dtfObj->writeData(data, fileToWriteEYE);
		}
	}

	template<typename Out>
	inline void split(const std::string &s, char delim, Out result) {
		std::stringstream ss;
		ss.str(s);
		std::string item;
		while (std::getline(ss, item, delim)) {
			*(result++) = item;
		}
	}

	inline void updateAfterVector(int afterX, int afterY)
	{

		map<string, fixationStruct*>::iterator it = fixationMap.find(_beforeThis);

		if (it != fixationMap.end())
		{
			fixationStruct *oldFD = it->second;
			fixationStruct *newFD = new fixationStruct;
			newFD->numberOfVisits = oldFD->numberOfVisits + 1;
			newFD->startTime = oldFD->startTime;
			newFD->fixationDuration = oldFD->fixationDuration;
			newFD->totalFixationDuration = oldFD->totalFixationDuration;
			newFD->totalIntervalBetweenVisits = oldFD->totalIntervalBetweenVisits;
			newFD->vecBeforeX = oldFD->vecBeforeX;
			newFD->vecBeforeY = oldFD->vecBeforeY;
			newFD->vecBeforeCount = oldFD->vecBeforeCount;

			int i = 0;
			bool afterUpdated = false;
			vector<int>::const_iterator afterIt = oldFD->vecAfterX.begin();

			for (afterIt; afterIt != oldFD->vecAfterX.end(); afterIt++)
			{
				if ((oldFD->vecAfterX[i] < afterX + 5 && oldFD->vecAfterX[i] > afterX - 5) || (oldFD->vecAfterY[i] < afterY + 5 && oldFD->vecAfterY[i]> afterY - 5))
				{
					oldFD->vecAfterCount[i] += 1;
					afterUpdated = true;
					break;
				}
				i++;
			}
			if (!afterUpdated)
			{
				oldFD->vecAfterX.push_back(afterX);
				oldFD->vecAfterY.push_back(afterY);
				oldFD->vecAfterCount.push_back(1);
			}
			newFD->vecAfterX = oldFD->vecAfterX;
			newFD->vecAfterY = oldFD->vecAfterY;
			newFD->vecAfterCount = oldFD->vecAfterCount;
		}
	}
	static FDC* _instance;
	int fixationMapSizeLimit = 10, matchingRange = 5;
	string _beforeThis = "0_0";
	DTF *dtfObj;

public:
	std::chrono::steady_clock::time_point eyeTrackingStart;
	struct gazeDataStruct
	{
		int			numberOfFixations		= 0;
		int			totalFixationDuration	= 0;
		float		avgFixationDuration		= 0.0;
		short		repeatedFixations		= 0;
		std::string	totalTime				= "";

		int			biggestFixationDuration = 0;
		std::string	biggestFixationAt		= "";

		int			maxVisitsCount			= 0;		// At one fixation
		std::string	maxVisitedAt			= "";
	};

	struct fixationStruct
	{
		short		numberOfVisits;							// How many time this area got fixation
		float		startTime;								// in sec, 
		int			fixationDuration;						// in msec, number of seconds last fixation stayed
		int			totalFixationDuration;					// in msec, sum of all fixation durations
		int			totalIntervalBetweenVisits;				// in msec, sum of time between all visit
		vector<int>	vecBeforeX, vecBeforeY, vecBeforeCount;	// Places visted before
		vector<int>	vecAfterX, vecAfterY, vecAfterCount;	// Places visited after

															//float	averageTimeBetweenVisits;	// in sec, = totalIntervalBetweenVisits / (numberOfVisits - 1), keeping it commented and calculate this in realtime to save storage
	};

	gazeDataStruct gdsObj;
	map<string, fixationStruct*> fixationMap;	// Fixation id as string along with fixation data
	std::map<std::string, fixationStruct*>::iterator fixationMapIterator;

	static FDC* Instance();

	void updateFixationMap(string id, float startTime, float fixationDuration, string afterThis)
	{
		gdsObj.numberOfFixations++;
		gdsObj.totalFixationDuration += fixationDuration;
		gdsObj.avgFixationDuration = (float) gdsObj.totalFixationDuration / (float)gdsObj.numberOfFixations;

		std::chrono::steady_clock::time_point current = std::chrono::steady_clock::now();
		gdsObj.totalTime = to_string(std::chrono::duration_cast<std::chrono::milliseconds>(current - eyeTrackingStart).count());

		/*map<string, fixationStruct*>::iterator it = fixationMap.find(id);*/
		map<string, fixationStruct*>::iterator it = fixationMap.begin();

		std::vector<std::string> idXY;
		split(id, '_', std::back_inserter(idXY));
		int x = stoi(idXY[0]);
		int y = stoi(idXY[1]);

		updateAfterVector(x, y);

		std::vector<std::string> beforeXY;
		split(_beforeThis, '_', std::back_inserter(beforeXY));
		int beforeX = stoi(beforeXY[0]);
		int beforeY = stoi(beforeXY[1]);

		for (it; it != fixationMap.end(); it++)
		{
			std::vector<std::string> coordinates;
			split(it->first, '_', std::back_inserter(coordinates));
			if ((stoi(coordinates[0]) < (x + matchingRange) && stoi(coordinates[0]) > (x - matchingRange)) || (stoi(coordinates[1]) < (y + matchingRange) && stoi(coordinates[1]) > (y - matchingRange)))
				break;
		}
		if (it != fixationMap.end())	// In case of repeated fixation
		{
			gdsObj.repeatedFixations++;	// Increase repeated fixation counter

			fixationStruct *oldFD = it->second;
			fixationStruct *newFD = new fixationStruct;
			newFD->numberOfVisits = oldFD->numberOfVisits + 1;
			newFD->startTime = startTime;
			newFD->fixationDuration = fixationDuration;
			newFD->totalFixationDuration = oldFD->fixationDuration + fixationDuration;
			newFD->totalIntervalBetweenVisits = oldFD->totalIntervalBetweenVisits + (startTime - (oldFD->startTime + oldFD->fixationDuration));

			int i = 0;
			bool beforeUpdated = false;
			vector<int>::const_iterator beforeIt = oldFD->vecBeforeX.begin();

			for (beforeIt; beforeIt != oldFD->vecBeforeX.end(); beforeIt++)
			{
				if ((oldFD->vecBeforeX[i] < beforeX + 5 && oldFD->vecBeforeX[i] > beforeX - 5) || (oldFD->vecBeforeY[i] < beforeY + 5 && oldFD->vecBeforeY[i]> beforeY - 5))
				{
					oldFD->vecBeforeCount[i] += 1;
					beforeUpdated = true;
					break;
				}
				i++;
			}
			if (!beforeUpdated)
			{
				oldFD->vecBeforeX.push_back(beforeX);
				oldFD->vecBeforeY.push_back(beforeY);
				oldFD->vecBeforeCount.push_back(1);
			}
			newFD->vecBeforeX = oldFD->vecBeforeX;
			newFD->vecBeforeY = oldFD->vecBeforeY;
			newFD->vecBeforeCount = oldFD->vecBeforeCount;
			newFD->vecAfterX = oldFD->vecAfterX;
			newFD->vecAfterY = oldFD->vecAfterY;
			newFD->vecAfterCount = oldFD->vecAfterCount;

			id = it->first;
			fixationMap.erase(it->first);
			fixationMap.insert(std::pair<std::string, fixationStruct*>(id, newFD));

			// Biggest fixation details
			if (gdsObj.biggestFixationDuration < newFD->totalFixationDuration)
			{
				gdsObj.biggestFixationDuration = newFD->totalFixationDuration;
				gdsObj.biggestFixationAt = id;
			}
			// Maximum visits detail
			if (gdsObj.maxVisitsCount < newFD->numberOfVisits)
			{
				gdsObj.maxVisitsCount = newFD->numberOfVisits;
				gdsObj.maxVisitedAt = id;
			}
		}
		else	// In case of new fixation
		{
			fixationStruct *newFD = new fixationStruct;
			newFD->numberOfVisits = 1;
			newFD->startTime = startTime;
			newFD->numberOfVisits = 1;
			newFD->fixationDuration = fixationDuration;
			newFD->totalFixationDuration = fixationDuration;
			newFD->totalIntervalBetweenVisits = 0.0;
			newFD->vecBeforeX.push_back(beforeX);
			newFD->vecBeforeY.push_back(beforeY);
			newFD->vecBeforeCount.push_back(1);
			newFD->vecAfterX.push_back(0);
			newFD->vecAfterY.push_back(0);
			newFD->vecAfterCount.push_back(0);
			//newFD->afterThis					= afterThis + ":1";

			fixationMap.insert(std::pair<std::string, fixationStruct*>(id, newFD));

			// Biggest fixation details
			if (gdsObj.biggestFixationDuration < newFD->totalFixationDuration)
			{
				gdsObj.biggestFixationDuration = newFD->totalFixationDuration;
				gdsObj.biggestFixationAt = id;
			}
		}
		_beforeThis = id;

		it = fixationMap.begin();
		if (fixationMap.size() > fixationMapSizeLimit)
		{
			it = fixationMap.begin();
			fixationMap.erase(it);
		}
#if VISULIZE_FIXATION_DATA == 1
		it = fixationMap.begin();
		cout << "id\t\t VisCunt\t\t StartT\t\t FixaT\t\t TotalFT\t IntervalBV\n";
		for (it; it != fixationMap.end(); it++)
		{
			fixationStruct *oldFD = it->second;
			cout << it->first << "\t\t" << oldFD->numberOfVisits << "\t\t\t" << oldFD->startTime << "\t\t" << oldFD->fixationDuration
				<< "\t\t" << oldFD->totalFixationDuration << "\t\t" << oldFD->totalIntervalBetweenVisits << endl;
			vector<int>::const_iterator beforeIt = oldFD->vecBeforeX.begin();
			int i = 0;
			cout << "Before:\t";
			for (beforeIt; beforeIt != oldFD->vecBeforeX.end(); beforeIt++)
			{
				cout << oldFD->vecBeforeX[i] << "_" << oldFD->vecBeforeY[i] << "\t" << oldFD->vecBeforeCount[i] << " -- ";
				i++;
			}
			cout << "\n";
			vector<int>::const_iterator afterIt = oldFD->vecAfterX.begin();
			i = 0;
			cout << "After:\t";
			for (afterIt; afterIt != oldFD->vecAfterX.end(); afterIt++)
			{
				cout << oldFD->vecAfterX[i] << "_" << oldFD->vecAfterY[i] << "\t" << oldFD->vecAfterCount[i] << " -- ";
				i++;
			}
			cout << "\n\n";
		}
		cout << "---------------------------------------------------------------------------\n\n";
#endif

		writeData();
	}


};

FDC* FDC::_instance = 0;

FDC* FDC::Instance()
{
	if (_instance == 0)
	{
		_instance = new FDC();
	}
	return _instance;

}


#endif // !FIXATION_DATA_CLASS_hpp
