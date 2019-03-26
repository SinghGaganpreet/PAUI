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
#include "UDP.hpp"
//#include "FixationDataClass.hpp"
#include "WriteToFile.hpp"


//#include <dlib/svm_threaded.h>
//#include <iostream>

//using namespace std;
//using namespace dlib;


class DBC
{
public:
	static DBC* Instance();
	DTF* dtfObj;

	// ECG data -----------
	struct ecgStruct
	{
		double _RMSSD = 0.0, _heartRate = 0.0, _SDNN = 0.0;
		//double _mean = 0.0,  _deltaMean = 0.0, _deltaSD = 0.0; //-- delta items are used for calculations on 2 successive RRI
		double _LF = 0.0, _HF = 0.0, _VLF = 0.0, _SVB = 0.0;
	};

	// EMG data -----------
	struct emgStruct
	{
		int numOfPeaks = 0; // totalPeakTime = 0, numOfPlains = 0, totalPlainTime = 0, totalTime = 0;
		/*float maxPeakMagnitude = 0.0, currentpeakMag = 0.0;
		bool peakStart = false;*/
	};

	// EEG data -----------
	struct eegStruct
	{
		double delta = 0.0, theta = 0.0, alpha = 0.0, beta = 0.0, gamma = 0.0, engagement = 0.0;
	};

	// EDA data -----------
	struct edaStruct
	{
		float _SD = 0.0, _SCL = 0.0, _SCR = 0.0;
	};

	// CAM data -----------
	/*struct camStruct
	{
		affdex::Frame frame;
		std::map<affdex::FaceId, affdex::Face> faces;
	};*/

	struct dataBaseStruct	// Main Data base containing data from all three devices (Bitalion, Tobii, And Camera)
	{
		bool userPresent;
		bool eyesTracked;
		// Tobii
		FDC::gazeDataStruct gazeData;
		//std::map<std::string, FDC::fixationStruct*> fixationMapData;
		// ECG
		ecgStruct ecgData;
		// EMG
		emgStruct emgData;
		// EEG
		eegStruct eegData;
		// EAD
		edaStruct edaData;
		//CAM
		//camStruct camData;
	};
	
	std::map<std::string, dataBaseStruct> dataBaseMap;		// Main Data Base Map to hold timestamp and all data values from Bitalion, Tobii, and Camera
	std::map<std::string, dataBaseStruct>::iterator dataBaseIterator;
	dataBaseStruct dbsObj;
	string camData;
	long int preTimeStamp = 0;
	int averageCounter = 0, averageCam = 0;
	char ftwb[100];

	/*typedef dlib::matrix<double, 0, 1> column_vector;
	column_vector weights;*/

	inline void classifyEmotion()	// Classify emotions and write data to file
	{		
		if (dtfObj == nullptr)
		{			
			//dlib::deserialize("saved_function.dat") >> weights;

			dtfObj = new DTF();
			string str = "#HR;RMSSD;SDNN;LF;HF;SVB;SCL;SD;PEAKS;Alpha;Beta;Theta;Engagement;Anger;Contempt;Disgust;Engagement;Fear;Joy;Sadness;Surprise;Valence;Attention;Browfurrow;BrowRaise;Cheekraise;Chinraise;Dimpler;Eyeclosure;Eyewiden;InnerBrowraise;"
				"Jawdrop;Lidtighten;Lipcornerdepressor;Lippress;Lippucker;Lipstretch;Lipsuck;Mouthopen;Nosewrinkle;Smile;Smirk;Upperlipraise;Pitch;Roll;Yaw\n";

			time_t now = time(0);
			tm *ltm = localtime(&now);
			sprintf(ftwb, "%s%d.%d.%d-%d.%d.%d.txt", fileToWriteProcessed, ltm->tm_mday, ltm->tm_mon + 1, ltm->tm_year + 1900, ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
			dtfObj->writeData(str, ftwb);
			//dtfObj->writeData(str, fileToWriteProcessed);
		}
		string str = "";
		for (dataBaseIterator = dataBaseMap.begin(); dataBaseIterator != dataBaseMap.end(); dataBaseIterator++)
		{			
			dataBaseStruct dbsObject = dataBaseIterator->second;
			printf("HR    : %0.2f  RMSSD  : %0.2f \n", dbsObject.ecgData._heartRate, dbsObject.ecgData._RMSSD);

			str = to_string(dbsObject.ecgData._heartRate) + ";" + to_string(dbsObject.ecgData._RMSSD) + ";" + to_string(dbsObject.ecgData._SDNN)
				+ ";" + to_string(dbsObject.ecgData._LF) + ";" + to_string(dbsObject.ecgData._HF) + ";" + to_string(dbsObject.ecgData._SVB)
				+ ";" + to_string(dbsObject.edaData._SCL) + ";" + to_string(dbsObject.edaData._SD)
				+ ";" + to_string(dbsObject.emgData.numOfPeaks) + ";" + to_string(dbsObject.eegData.alpha)
				+ ";" + to_string(dbsObject.eegData.beta) + ";" + to_string(dbsObject.eegData.theta) + ";" + to_string(dbsObject.eegData.engagement) + ";" + camData +"\n";

			dtfObj->writeData(str, ftwb);

			/*std::map<std::string, FDC::fixationStruct*>::iterator fixationMapIterator = dbsObject.fixationMapData.begin();

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
			cout << "---------------------------------------------------------------------------\n\n";*/
		}
	}

	int oldLineRead = 0;
	void insertDBM(int lineRead)
	{
		averageCounter++;
		dbsObj.userPresent = USER_PRESENT;
		dbsObj.eyesTracked = EYES_TRACKED;

		std::chrono::steady_clock::time_point current = std::chrono::steady_clock::now();
		string timeStamp = to_string(std::chrono::duration_cast<std::chrono::milliseconds>(current - startClock).count());
		
		#if FROM_FILE == 1
			if ((lineRead - oldLineRead) >= averagingWindow)
			{
				getAverage();
				dataBaseMap.insert(std::pair<std::string, DBC::dataBaseStruct>(timeStamp, dbsObj));
				classifyEmotion();

				printf("Line Read %d \t\t Average Count: %d\n", (lineRead - oldLineRead), averageCounter);
				oldLineRead = lineRead;
				averageCounter = 0;
				averageCam = 0;
				camData = "";
				preTimeStamp = stol(timeStamp);
				cleanData();
			}
		#else
			if (stol(timeStamp) - preTimeStamp >= averagingWindow)
			{
				getAverage();
				dataBaseMap.insert(std::pair<std::string, DBC::dataBaseStruct>(timeStamp, dbsObj));
				classifyEmotion();

				//printf("Time: %d \t\t Average Count: %d \n", stol(timeStamp) - preTimeStamp, averageCounter);
				oldLineRead = lineRead;
				averageCounter = 0;
				averageCam = 0;
				camData = "";
				preTimeStamp = stol(timeStamp);
				cleanData();
			}
		#endif
		dataBaseMap.erase(timeStamp); // erasing Map data, to keep data from only one time stamp at one time.
	}

	void addECG(ECG obj)
	{
		dbsObj.ecgData._heartRate	+= obj._heartRate;
		dbsObj.ecgData._SDNN		+= obj._SDNN;
		dbsObj.ecgData._RMSSD		+= obj._RMSSD;
		dbsObj.ecgData._VLF			+= obj._VLF;
		dbsObj.ecgData._LF			+= obj._LF;
		dbsObj.ecgData._HF			+= obj._HF;
		//dbsObj.ecgData._SVB			+= (obj._LF / obj._HF);  Because we are putting _SVB value after averaging _LF and _HF
		/*dbsObj.ecgData._deltaMean	+= obj._deltaMean;
		dbsObj.ecgData._deltaSD		+= obj._deltaSD;
		dbsObj.ecgData._mean		+= obj._mean;*/
	}

	void addEMG(EMG obj)
	{
		/*dbsObj.emgData.currentpeakMag	 += obj.currentpeakMag;
		dbsObj.emgData.maxPeakMagnitude	 += obj.maxPeakMagnitude;*/
		dbsObj.emgData.numOfPeaks		 += obj.numOfPeaks;
		//dbsObj.emgData.numOfPlains		 += obj.numOfPlains;
		//dbsObj.emgData.peakStart		 += obj.peakStart;
		//dbsObj.emgData.totalPeakTime	 += obj.totalPeakTime;
		////dbsObj.emgData.totalPlainTime	 += obj.totalPlainTime;
		//dbsObj.emgData.totalTime		 += obj.totalTime;
	}

	void addEEG(EEG obj)
	{
		dbsObj.eegData.alpha		+= obj.alpha;
		dbsObj.eegData.beta			+= obj.beta;
		dbsObj.eegData.delta		+= obj.delta;
		dbsObj.eegData.engagement	+= obj.engagement;
		dbsObj.eegData.gamma		+= obj.gamma;
		dbsObj.eegData.theta		+= obj.theta;
	}

	void addEDA(EDA obj)
	{
		dbsObj.edaData._SCL += obj._SCL;
		dbsObj.edaData._SCR += obj._SCR;
		dbsObj.edaData._SD	+= obj._SD;
	}

	void addEYE(FDC obj)
	{
		//dbsObj.fixationMapData	= obj.fixationMap;
		dbsObj.gazeData = obj.gdsObj;
		//string str = "EyePos,4," + to_string(SCREEN_WIDTH) +"," + to_string(SCREEN_HEIGHT) +"," + to_string(CURRENT_X) + "," + to_string(CURRENT_Y);
		char data[400];
		sprintf(data, "EyePos,4,%0.2f,%0.2f,%0.2f,%0.2f", SCREEN_WIDTH, SCREEN_HEIGHT, CURRENT_X, CURRENT_Y);
		UDP* udpObj = new UDP();
		udpObj->publishUdpData(data);
	}

	string ss;
	float camArray[33];
	void addCAM(CAM obj)		// When running in real time
	{
		/*dbsObj.camData.frame = obj._frame;
		dbsObj.camData.faces = obj._faces;*/

		for (auto & face_id_pair : obj._faces)
		{
			Face f = face_id_pair.second;

			camArray[0] += f.emotions.anger;
			camArray[1] += f.emotions.contempt;
			camArray[2] += f.emotions.disgust;
			camArray[3] += f.emotions.engagement;
			camArray[4] += f.emotions.fear;
			camArray[5] += f.emotions.joy;
			camArray[6] += f.emotions.sadness;
			camArray[7] += f.emotions.surprise;
			camArray[8] += f.emotions.valence;
			camArray[9] += f.expressions.attention;
			camArray[10] += f.expressions.browFurrow;
			camArray[11] += f.expressions.browRaise;
			camArray[12] += f.expressions.cheekRaise;
			camArray[13] += f.expressions.chinRaise;
			camArray[14] += f.expressions.dimpler;
			camArray[15] += f.expressions.eyeClosure;
			camArray[16] += f.expressions.eyeWiden;
			camArray[17] += f.expressions.innerBrowRaise;
			camArray[18] += f.expressions.jawDrop;
			camArray[19] += f.expressions.lidTighten;
			camArray[20] += f.expressions.lipCornerDepressor;
			camArray[21] += f.expressions.lipPress;
			camArray[22] += f.expressions.lipPucker;
			camArray[23] += f.expressions.lipStretch;
			camArray[24] += f.expressions.lipSuck;
			camArray[25] += f.expressions.mouthOpen;
			camArray[26] += f.expressions.noseWrinkle;
			camArray[27] += f.expressions.smile;
			camArray[28] += f.expressions.smirk;
			camArray[29] += f.expressions.upperLipRaise;
			camArray[30] += f.measurements.orientation.pitch;
			camArray[31] += f.measurements.orientation.roll;
			camArray[32] += f.measurements.orientation.yaw;

		}
		averageCam++;
	}

	void addCAM(string obj)	// When CAM data is from file
	{
		camData = obj;
	}

	inline void getAverage()
	{
		dbsObj.ecgData._heartRate		= (dbsObj.ecgData._heartRate / averageCounter);
		dbsObj.ecgData._SDNN			= (dbsObj.ecgData._SDNN / averageCounter);
		dbsObj.ecgData._RMSSD			= (dbsObj.ecgData._RMSSD / averageCounter);
		dbsObj.ecgData._VLF				= (dbsObj.ecgData._VLF / averageCounter);
		dbsObj.ecgData._LF				= (dbsObj.ecgData._LF / averageCounter);
		dbsObj.ecgData._HF				= (dbsObj.ecgData._HF / averageCounter);
		dbsObj.ecgData._SVB				= (dbsObj.ecgData._LF / dbsObj.ecgData._HF); // (dbsObj.ecgData._SVB / averageCounter);
		/*dbsObj.ecgData._deltaMean		= (dbsObj.ecgData._deltaMean / averageCounter);
		dbsObj.ecgData._deltaSD			= (dbsObj.ecgData._deltaSD / averageCounter);
		dbsObj.ecgData._mean			= (dbsObj.ecgData._mean / averageCounter);*/

		/*dbsObj.emgData.currentpeakMag	= (dbsObj.emgData.currentpeakMag / averageCounter);
		dbsObj.emgData.maxPeakMagnitude = (dbsObj.emgData.maxPeakMagnitude / averageCounter);*/
		dbsObj.emgData.numOfPeaks		= (dbsObj.emgData.numOfPeaks / averageCounter);
		/*dbsObj.emgData.numOfPlains		= (dbsObj.emgData.numOfPlains / averageCounter);
		dbsObj.emgData.peakStart		= (dbsObj.emgData.peakStart / averageCounter);
		dbsObj.emgData.totalPeakTime	= (dbsObj.emgData.totalPeakTime / averageCounter);
		dbsObj.emgData.totalPlainTime	= (dbsObj.emgData.totalPlainTime / averageCounter);
		dbsObj.emgData.totalTime		= (dbsObj.emgData.totalTime / averageCounter);*/

		dbsObj.eegData.alpha			= (dbsObj.eegData.alpha / averageCounter);
		dbsObj.eegData.beta				= (dbsObj.eegData.beta / averageCounter);
		dbsObj.eegData.delta			= (dbsObj.eegData.delta / averageCounter);
		dbsObj.eegData.engagement		= (dbsObj.eegData.engagement / averageCounter);
		dbsObj.eegData.gamma			= (dbsObj.eegData.gamma / averageCounter);
		dbsObj.eegData.theta			= (dbsObj.eegData.theta / averageCounter);

		dbsObj.edaData._SCL				= (dbsObj.edaData._SCL / averageCounter);
		dbsObj.edaData._SCR				= (dbsObj.edaData._SCR / averageCounter);
		dbsObj.edaData._SD				= (dbsObj.edaData._SD / averageCounter);


		for (int i = 0; i < 33; i++)
		{
			camArray[i] /= averageCam;
			camData += to_string(camArray[i]) + ";";
			camArray[0] = 0;
		}
	}
	
	inline void cleanData()
	{
		dbsObj.ecgData._heartRate = 0;
		dbsObj.ecgData._SDNN = 0;
		dbsObj.ecgData._RMSSD = 0;
		dbsObj.ecgData._VLF = 0;
		dbsObj.ecgData._LF = 0;
		dbsObj.ecgData._HF = 0;
		dbsObj.ecgData._SVB = 0;
		/*dbsObj.ecgData._deltaMean = 0;
		dbsObj.ecgData._deltaSD = 0;
		dbsObj.ecgData._mean = 0;*/
	
		/*dbsObj.emgData.currentpeakMag = 0;
		dbsObj.emgData.maxPeakMagnitude = 0;*/
		dbsObj.emgData.numOfPeaks = 0;
		/*dbsObj.emgData.numOfPlains = 0;
		dbsObj.emgData.peakStart = 0;
		dbsObj.emgData.totalPeakTime = 0;
		dbsObj.emgData.totalPlainTime = 0;
		dbsObj.emgData.totalTime = 0;*/
	
		dbsObj.eegData.alpha = 0;
		dbsObj.eegData.beta = 0;
		dbsObj.eegData.delta = 0;
		dbsObj.eegData.engagement = 0;
		dbsObj.eegData.gamma = 0;
		dbsObj.eegData.theta = 0;
	
		dbsObj.edaData._SCL = 0;
		dbsObj.edaData._SCR = 0;
		dbsObj.edaData._SD = 0;
	}

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