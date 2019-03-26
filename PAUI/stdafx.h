// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef STDAFX_h
#define STDAFX_h

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <string>
#include <chrono>
#include <ctime>
#include <ctime>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifdef _WIN32 // 32-bit or 64-bit Windows

#include <winsock2.h>

#endif

// ############################################################### //
// ################# Global variables and macros ################# //
// ############################################################### //

#define BITALINO 0;		// 1000 Hz
#define TOBII 1;		// 90 Hz
#define CAMERA 0;		// 30 Hz
#define CAM_FROM_FILE 0;

#define DISPLAY_GRAPH 0;
#define DISPLAY_GRAPH_FULL 0;
#define FROM_FILE 0;
#define RECORD_BITALINO 0;

#define PROCESS_ECG 0;
#define PROCESS_EMG 0;
#define PROCESS_EEG 0;
#define PROCESS_EDA 0;

static const bool displayCam = false;
//static const bool 

//########## Eye Data ##################//
static float SCREEN_WIDTH = 0.0, SCREEN_HEIGHT = 0.0;
static float CURRENT_X = 0.0, CURRENT_Y = 0.0;
static bool USER_PRESENT = false, EYES_TRACKED = false;

static const int ECG_INDEX				= 0;	// 0 when real time otherwise start from 1
static const int EMG_INDEX				= 1;
static const int EDA_INDEX				= 2;
static const int EEG_INDEX				= 3;
static const int samplingFrequency		= 1000;
static const int frameLength			= 100;
static const int processingFreq			= 2000;	// In Hz
static const int processingDelay		= 1000000 / processingFreq;	// In milli seconds
static		 int stressedStatus			= 0;
static		 int workloadStatus			= 0;
static		 int currentState			= 0; // 0 for Idel, -1 for Resting, and 1 for Testing
static		 int averagingWindow		= 3000;	// Use 0 to get all readings or 3000 to get average data of 3 seconds, and so on. Only tested with 3000 though.


static const char  splitWith		= ';';
static char* fileToWriteBitalino	= "C:\\Users\\samsung-ISR\\Desktop\\PAUI-(M-ITI)\\Sample-Signals\\Amarit_BIT_";
static char* fileToWriteCAM			= "C:\\Users\\samsung-ISR\\Desktop\\PAUI-(M-ITI)\\Sample-Signals\\Amarit_CAM_";
static char* fileToWriteEYE			= "C:\\Users\\samsung-ISR\\Desktop\\PAUI-(M-ITI)\\Sample-Signals\\Amarit_EYE_";
static char* fileToWriteProcessed	= "C:\\Users\\samsung-ISR\\Desktop\\PAUI-(M-ITI)\\Sample-Signals\\Amarit_Processed_BIT_CAM_";

static char* fileToReadBitalino		= "C:\\Users\\samsung-ISR\\Desktop\\PAUI-(M-ITI)\\Sample-Signals\\---.txt";

static const std::chrono::steady_clock::time_point startClock = std::chrono::steady_clock::now();


#endif