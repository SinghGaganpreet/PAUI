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

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifdef _WIN32 // 32-bit or 64-bit Windows

#include <winsock2.h>

#endif

// ############################################################### //
// ################# Global variables and macros ################# //
// ############################################################### //

#define BITALINO 1;		// 1000 Hz
#define TOBII 0;		// 90 Hz
#define CAMERA 0;		// 30 Hz

#define DISPLAY_GRAPH 0;
#define DISPLAY_GRAPH_FULL 0;
#define FROM_FILE 1;
#define RECORD_BITALINO 0;

#define PROCESS_ECG 1;
#define PROCESS_EMG 1;
#define PROCESS_EEG 0;
#define PROCESS_EDA 0;

static const bool displayCam = true;
//static const bool 

static float SCREEN_WIDTH = 0.0, SCREEN_HEIGHT = 0.0;
static bool USER_PRESENT = false, EYES_TRACKED = false;

static const int ECG_INDEX				= 1;
static const int EMG_INDEX				= 2;
static const int EEG_INDEX				= 3;
static const int EDA_INDEX				= 0;
static const int samplingFrequency		= 1000;
static const int frameLength			= 100;
static const int processingFreq			= 2000;	// In Hz
static const int processingDelay		= 1000000 / processingFreq;	// In milli seconds

static const char  splitWith			= ';';
static const char* fileToWriteBitalino	= "D:\\PAUI-(M-ITI)\\Sample-Signals\\ECG-Athanas-Exertion1_.txt";
static const char* fileToWriteCAM		= "D:\\PAUI-(M-ITI)\\Sample-Signals\\CAM1.txt";
static const char* fileToWriteEYE		= "D:\\PAUI-(M-ITI)\\Sample-Signals\\EYE1.txt";
static const char* fileToReadBitalino	= "D:\\PAUI-(M-ITI)\\Sample-Signals\\ECG-EMG-EEG.txt";

static const std::chrono::steady_clock::time_point startClock = std::chrono::steady_clock::now();


#endif