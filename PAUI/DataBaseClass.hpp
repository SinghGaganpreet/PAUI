#pragma once


#ifndef DATA_BASE_CLASS_hpp
#define DATA_BASE_CLASS_hpp

#include "stdafx.h"
#include <string>
#include <map>
#include <vector>
#include "Frame.h"
#include "Face.h"
#include "ECG.hpp"
#include "EEG.hpp"
#include "EMG.hpp"
#include "EDA.hpp"
#include "CAM.hpp"
#include "FixationDataClass.hpp"
#include "WriteToFile.hpp"

class DBC
{
public:
	static DBC* Instance();
	DTF* dtfObj;

	// ECG data -----------
	struct ecgStruct
	{
		double _RMSSD = 0.0, _heartRate = 0.0;
		double _mean = 0.0, _SDNN = 0.0, _deltaMean = 0.0, _deltaSD = 0.0; //-- delta items are used for calculations on 2 successive RRI
		double _LF = 0.0, _HF = 0.0, _VLF = 0.0, _SVB = 0.0;
	};

	// EMG data -----------
	struct emgStruct
	{
		int totalTime = 0, numOfPeaks = 0, totalPeakTime = 0, numOfPlains = 0, totalPlainTime = 0;
		float maxPeakMagnitude = 0.0, currentpeakMag = 0.0;
		bool peakStart = false;
	};

	// EEG data -----------
	struct eegStruct
	{
		double delta = 0.0, theta = 0.0, alpha = 0.0, beta = 0.0, gamma = 0.0, engagement = 0.0;
	};

	// EDA data -----------
	struct edaStruct
	{
		float _SCR = 0.0, _SD = 0.0, _SCL = 0.0;
	};

	// CAM data -----------
	struct camStruct
	{
		affdex::Frame frame;
		std::map<affdex::FaceId, affdex::Face> faces;
	};

	struct dataBaseStruct	// Main Data base containing data from all three devices (Bitalion, Tobii, And Camera)
	{
		bool userPresent;
		bool eyesTracked;
		// Tobii
		FDC::gazeDataStruct gazeData;
		std::map<std::string, FDC::fixationStruct*> fixationMapData;
		// ECG
		ecgStruct ecgData;
		// EMG
		emgStruct emgData;
		// EEG
		eegStruct eegData;
		// EAD
		edaStruct edaData;
		//CAM
		camStruct camData;
	};
	
	std::map<std::string, dataBaseStruct> dataBaseMap;		// Main Data Base Map to hold timestamp and all data values from Bitalion, Tobii, and Camera
	std::map<std::string, dataBaseStruct>::iterator dataBaseIterator;
	dataBaseStruct dbsObj;

	inline void displayData()
	{		
		if (dtfObj == nullptr)
		{
			dtfObj = new DTF();
			string str = "HR;RMSSD;SDNN;LF;HF;SVB;SCL;SCR;SD;PEAKS;PLAINS;Alpha;Beta;Theta;Engagement\n";
			dtfObj->writeData(str, fileToWriteProcessed);
		}
		string str = "";
		for (dataBaseIterator = dataBaseMap.begin(); dataBaseIterator != dataBaseMap.end(); dataBaseIterator++)
		{
			
				printf("Time stamp: %s \n", dataBaseIterator->first.c_str());
				dataBaseStruct dbsObject = dataBaseIterator->second;
				printf("HR    : %0.2f  RMSSD  : %0.2f \n", dbsObject.ecgData._heartRate, dbsObject.ecgData._RMSSD);
				printf("PeakT : %d  PlainT : %d \n", dbsObject.emgData.totalPeakTime, dbsObject.emgData.totalPlainTime);

				str = to_string(dbsObject.ecgData._heartRate) + ";" + to_string(dbsObject.ecgData._RMSSD*1000.0) + ";" + to_string(dbsObject.ecgData._SDNN*1000.0)
					+ ";" + to_string(dbsObject.ecgData._LF*10000.0) + ";" + to_string(dbsObject.ecgData._HF*10000.0) + ";" + to_string(dbsObject.ecgData._SVB*1000.0)
					+ ";" + to_string(dbsObject.edaData._SCL) + ";" + to_string(dbsObject.edaData._SCR) + ";" + to_string(dbsObject.edaData._SD)
					+ ";" + to_string(dbsObject.emgData.numOfPeaks) + ";" + to_string(dbsObject.emgData.numOfPlains) + ";" + to_string(dbsObject.eegData.alpha) 
					+ ";" + to_string(dbsObject.eegData.beta) + ";" + to_string(dbsObject.eegData.theta) + ";" + to_string(dbsObject.eegData.engagement) +"\n";

				dtfObj->writeData(str, fileToWriteProcessed);


				std::map<std::string, FDC::fixationStruct*>::iterator fixationMapIterator = dbsObject.fixationMapData.begin();

				for (fixationMapIterator; fixationMapIterator != dbsObject.fixationMapData.end(); fixationMapIterator++)
				{
					FDC::fixationStruct *oldFD = fixationMapIterator->second;
					cout << fixationMapIterator->first << "\t\t" << oldFD->numberOfVisits << "\t\t\t" << oldFD->startTime << "\t\t" << oldFD->fixationDuration
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
					cout << "\n";
				}
				std::map<affdex::FaceId, affdex::Face> faces = dbsObject.camData.faces;
				for (auto & face_id_pair : faces)
				{
					Face f = face_id_pair.second;

					cout << "Attention : " << f.expressions.attention << " Age: " << f.appearance.age << endl;
				}
				cout << "---------------------------------------------------------------------------\n\n";
		}
	}

	void addECG(ECG obj)
	{
		dbsObj.ecgData._heartRate	= obj._heartRate;
		dbsObj.ecgData._SDNN		= obj._SDNN;
		dbsObj.ecgData._RMSSD		= obj._RMSSD;
		dbsObj.ecgData._VLF			= obj._VLF;
		dbsObj.ecgData._LF			= obj._LF;
		dbsObj.ecgData._HF			= obj._HF;
		dbsObj.ecgData._SVB			= obj._LF / obj._HF;
		dbsObj.ecgData._deltaMean	= obj._deltaMean;
		dbsObj.ecgData._deltaSD		= obj._deltaSD;
		dbsObj.ecgData._mean		= obj._mean;
	}

	void addEMG(EMG obj)
	{
		dbsObj.emgData.currentpeakMag	 = obj.currentpeakMag;
		dbsObj.emgData.maxPeakMagnitude = obj.maxPeakMagnitude;
		dbsObj.emgData.numOfPeaks		 = obj.numOfPeaks;
		dbsObj.emgData.numOfPlains		 = obj.numOfPlains;
		dbsObj.emgData.peakStart		 = obj.peakStart;
		dbsObj.emgData.totalPeakTime	 = obj.totalPeakTime;
		dbsObj.emgData.totalPlainTime	 = obj.totalPlainTime;
		dbsObj.emgData.totalTime		 = obj.totalTime;
	}

	void addEEG(EEG obj)
	{
		dbsObj.eegData.alpha		= obj.alpha;
		dbsObj.eegData.beta			= obj.beta;
		dbsObj.eegData.delta		= obj.delta;
		dbsObj.eegData.engagement	= obj.engagement;
		dbsObj.eegData.gamma		= obj.gamma;
		dbsObj.eegData.theta		= obj.theta;
	}

	void addEDA(EDA obj)
	{
		dbsObj.edaData._SCL = obj._SCL;
		dbsObj.edaData._SCR = obj._SCR;
		dbsObj.edaData._SD	= obj._SD;
	}

	void addEYE(FDC obj)
	{
		dbsObj.fixationMapData	= obj.fixationMap;
		dbsObj.gazeData			= obj.gdsObj;
	}

	void addCAM(CAM obj)
	{
		dbsObj.camData.frame = obj._frame;
		dbsObj.camData.faces = obj._faces;
	}

	void insertDBM()
	{
		dbsObj.userPresent = USER_PRESENT;
		dbsObj.eyesTracked = EYES_TRACKED;

		std::chrono::steady_clock::time_point current = std::chrono::steady_clock::now();
		string timeStamp = to_string(std::chrono::duration_cast<std::chrono::milliseconds>(current - startClock).count());

		dataBaseMap.insert(std::pair<std::string, DBC::dataBaseStruct>(timeStamp, dbsObj));
		//displayData();
		dataBaseMap.erase(timeStamp); // erasing 
	}

	/*inline void cleanDataBaseStruct()
	{
		dbsObj.ecgData._heartRate	 	= 0.0;
		dbsObj.ecgData._SDNN			= 0.0;
		dbsObj.ecgData._RMSSD			= 0.0;
		dbsObj.ecgData._VLF				= 0.0;
		dbsObj.ecgData._LF				= 0.0;
		dbsObj.ecgData._HF				= 0.0;
		dbsObj.ecgData._SVB				= 0.0;
		dbsObj.ecgData._deltaMean		= 0.0;
		dbsObj.ecgData._deltaSD			= 0.0;
		dbsObj.ecgData._mean			= 0.0;

		dbsObj.emgData.currentpeakMag	= 0.0;
		dbsObj.emgData.maxPeakMagnitude	= 0.0;
		dbsObj.emgData.numOfPeaks		= 0.0;
		dbsObj.emgData.numOfPlains		= 0.0;
		dbsObj.emgData.peakStart		= 0.0;
		dbsObj.emgData.totalPeakTime	= 0.0;
		dbsObj.emgData.totalPlainTime	= 0.0;
		dbsObj.emgData.totalTime		= 0.0;

		dbsObj.eegData.alpha			= 0.0;
		dbsObj.eegData.beta				= 0.0;
		dbsObj.eegData.delta			= 0.0;
		dbsObj.eegData.engagement		= 0.0;
		dbsObj.eegData.gamma			= 0.0;
		dbsObj.eegData.theta			= 0.0;

		dbsObj.fixationMapData			= NULL;
	}*/

private:
	static DBC* _instance;
};


DBC* DBC::_instance = 0;

DBC* DBC::Instance()
{
	if (_instance == 0)
	{
		_instance = new DBC();
	}
	return _instance;
}

#endif // !DATA_BASE_CLASS_hpp