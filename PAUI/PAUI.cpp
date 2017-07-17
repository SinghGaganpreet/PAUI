// PAUI.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <assert.h>
#include <stdlib.h>
#include <map>

#include <windows.h>   // Standard Header For Most Programs
#include <math.h>

#include <iostream>
#include <vector>
#include <sstream>
#include <iterator>
#include <cmath>
#include <conio.h>

#include <thread>	// for multi threading
#include <chrono>

#include "EYE.h"
#include "MinimalStatusNotifications.h"
#include "WriteToFile.hpp"
#include "ECG.hpp"
#include "EEG.hpp"
#include "EMG.hpp"
#include "EDA.hpp"
#include "CAM.hpp";
#include "DataBaseClass.hpp"

using namespace std;

OGLGraph* myGraph;
ECG* ecgObj;
EEG* eegObj;
EMG* emgObj;
EDA* edaObj;
DTF* dtfObj;
CAM* camObj;
DBC* dbcObj;
FDC* fdcObj;

ifstream myfile;
BITalino dev;

static int frameCounter = 0;
static float emgRatio = 96.0;
int gwECG, gwEMG, gwEEG, gwEDA;
char ftwb[100];

bool errorDisplayed = false;	// Stop displaying end of file error after displaying once
bool doProcessing	= true;

double ecgfromFile[frameLength];
double emgfromFile[frameLength];
double eegfromFile[frameLength];
double edafromFile[frameLength];

// ###################################################################### //
// ################# Initialization fuction for objects ################# //
// ###################################################################### //
inline void initialize()
{
	dbcObj = DBC::Instance();	// Object of DatabaseClass

	#if PROCESS_ECG == 1
		ecgObj = ECG::Instance();	// Object of ECG
	#endif
	#if PROCESS_EEG == 1
		eegObj = EEG::Instance();	// Object of EEG
	#endif
	#if PROCESS_EMG == 1
		emgObj = EMG::Instance();	// Object of EMG
	#endif
	#if PROCESS_EDA == 1
		edaObj = EDA::Instance();	// Object of EMG
	#endif
	#if CAMERA == 1
		camObj = CAM::Instance();	// Object of CAM
	#endif	
	#if TOBII == 1
		fdcObj = FDC::Instance();	// Object of FixationDataClass
	#endif // TOBII == 1
	#if RECORD_BITALINO == 1
		dtfObj = new DTF();
		string str = "#TimeStamp(in miliSec);analog0;analog1;analog2;analog3;analog4;analog5;digital0;digital1;digital2;digital3;Stress;Workload;State\n";

		time_t now = time(0);
		tm *ltm = localtime(&now);
		sprintf(ftwb, "%s%d.%d.%d-%d.%d.%d.txt", fileToWriteBitalino, ltm->tm_mday, ltm->tm_mon + 1, ltm->tm_year + 1900, ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
		dtfObj->writeData(str, ftwb);
	#endif
}

// ###################################################################### //
// ################# Reading data from file ############################# //
// ###################################################################### //
template<typename Out>
inline void split(const std::string &s, char delim, Out result) {
	std::stringstream ss;
	ss.str(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		*(result++) = item;
	}
}

inline std::string readFile()
{
	while (frameCounter < frameLength)
	{
		string line;
		std::vector<std::string> elems;

		if (myfile.is_open())
		{
			if (getline(myfile, line))
			{
				if (line.substr(0, 1) == "#")
					continue;

				split(line, splitWith, std::back_inserter(elems));

#if PROCESS_ECG == 1
				ecgfromFile[frameCounter] = stof(elems[ECG_INDEX]);
#endif //  PROCESS_ECG == 1
#if  PROCESS_EMG == 1
				emgfromFile[frameCounter] = stof(elems[EMG_INDEX]);
#endif //  PROCESS_EMG == 1
#if PROCESS_EEG == 1
				eegfromFile[frameCounter] = stof(elems[EEG_INDEX]);
#endif //  PROCESS_EEG == 1
#if PROCESS_EDA == 1
				edafromFile[frameCounter] = stof(elems[EDA_INDEX]);
#endif //  PROCESS_EDA == 1
				frameCounter++;

			}
			else
				return "Finish";
		}
		else return "Error";
	}
	return "Batch Read";
}

// ###################################################################### //
// ################# Reading data from bitalion ######################### //
// ###################################################################### //
inline BITalino::VFrame GetBitalinoFrames()
{
	std::string line;
	BITalino::VFrame frames(frameLength); // initialize the frames vector with 100 frames 
	dev.read(frames);
	return frames;
}

// ###################################################################### //
// ########## Insertion fuction to fill main Data base struct ########### //
// ###################################################################### //
void insertData()
{
	while (doProcessing)
	{
		//ECG
		#if PROCESS_ECG == 1
			dbcObj->addECG(*ecgObj);
		#endif
			// EMG
		#if  PROCESS_EMG == 1
			dbcObj->addEMG(*emgObj);
			emgRatio += 96;
			if (!emgObj->peakStart)
			{
				emgObj->numOfPeaks = 0;
				emgObj->numOfPlains = 0;
				emgObj->totalPlainTime = 0;
				emgObj->totalPeakTime = 0;
				emgObj->maxPeakMagnitude = 0;
				emgObj->currentpeakMag = 0;
				emgRatio = 96;
		}
		#endif	
			// EEG
		#if PROCESS_EEG == 1
			dbcObj->addEEG(*eegObj);
		#endif
			// EEG
		#if PROCESS_EDA == 1
			dbcObj->addEDA(*edaObj);
		#endif
			// EYE
		#if TOBII == 1
			dbcObj->addEYE(*fdcObj);
		#endif 
			// CAM
		#if CAMERA == 1
			dbcObj->addCAM(*camObj);
		#endif 

			dbcObj->insertDBM();

		std::this_thread::sleep_for(std::chrono::microseconds(processingFreq));	// Sleep for 500 micro Sec to run this thread at 2000Hz
	}
}

// ###################################################################### //
// ################# Thread functions ################################### //
// ###################################################################### //
inline void pECG()
{
#if PROCESS_ECG == 1
	ecgObj->processECGSignal(ecgfromFile);
#endif
}

inline void pEMG()
{
#if  PROCESS_EMG == 1
	emgObj->processEMGSignal(emgfromFile);
#endif //  PROCESS_EMG == 1
}

inline void pEEG()
{
#if PROCESS_EEG == 1
	eegObj->processEEGSignal(eegfromFile);
#endif //  PROCESS_EEG == 1
}

inline void pEDA()
{
	#if PROCESS_EDA == 1
		edaObj->processEDASignal(edafromFile);
	#endif //  PROCESS_EDA == 1
}

inline void pCAM(int argc, char** argv)
{
	camObj->processCAM(argc, argv, displayCam, fileToWriteCAM, startClock);
}

void pBitalino(void)   // Bitalino processing block
{	

	#if DISPLAY_GRAPH == 1
	#else
		while (doProcessing)
		{
	#endif
	
	#if FROM_FILE == 1	// When data is from File
		string line = "";
		line = readFile();
		if (line == "Error")
		{
			printf("Error reading file \n");
			return;
		}
		else if (line == "Finish")
		{
			errorDisplayed = errorDisplayed ? true : printf("Data from file finished! \n");
			errorDisplayed = true;
			return;
		}

		#if PROCESS_ECG == 1
			//ecgObj->processECGSignal(ecgfromFile);
			std::thread ecgThread(pECG);
		#endif //  PROCESS_ECG == 1
		#if  PROCESS_EMG == 1
			//emgObj->processEMGSignal(emgfromFile);
			std::thread emgThread(pEMG);
		#endif //  PROCESS_EMG == 1
		#if PROCESS_EEG == 1
			//eegObj->processEEGSignal(eegfromFile);
			std::thread eegThread(pEEG);
		#endif //  PROCESS_EEG == 1
		#if PROCESS_EDA == 1
			//edaObj->processEDASignal(edafromFile);
			std::thread edaThread(pEDA);
		#endif //  PROCESS_EDA == 1
			
		frameCounter = 0;

		#if PROCESS_ECG == 1
			ecgThread.join();
		#endif //  PROCESS_ECG == 1
		#if  PROCESS_EMG == 1
			emgThread.join();
		#endif //  PROCESS_EMG == 1
		#if PROCESS_EEG == 1
			eegThread.join();
		#endif //  PROCESS_EEG == 1
		#if PROCESS_EDA == 1
			edaThread.join();
		#endif //  PROCESS_EDA == 1

		//insertData();

	#else	// Else when data is from Bitalino
			BITalino::VFrame f = GetBitalinoFrames();

		//#############Commenting the procesing of Bialino objects for now to collect data at faster rate for initial testing. 
		//############Will do the processing at offline mode########################
		//#if  PROCESS_EMG == 1
		//	emgObj->processEMGSignal(f);
		//#endif //  PROCESS_EMG == 1
		//#if PROCESS_ECG == 1
		//	ecgObj->processECGSignal(f);
		//#endif // PROCESS_ECG == 1
		//#if PROCESS_EEG == 1
		//	eegObj->processEEGSignal(f);
		//#endif // PROCESS_EEG == 1
		//#if PROCESS_EDA == 1
		//	edaObj->processEDASignal(f);
		//#endif // PROCESS_EDA == 1

		#if RECORD_BITALINO == 1
			for (int i = 0; i < f.size(); i++)
			{
				std::chrono::steady_clock::time_point current = std::chrono::steady_clock::now();
				string str = to_string(std::chrono::duration_cast<std::chrono::milliseconds>(current - startClock).count()) + ";" + to_string(f[i].analog[0]) 
					+ ";" + to_string(f[i].analog[1]) + ";" + to_string(f[i].analog[2]) + ";" + to_string(f[i].analog[3]) + ";" + to_string(f[i].analog[4]) 
					+ ";" + to_string(f[i].analog[5]) + ";" + to_string(f[i].digital[0]) + ";" + to_string(f[i].digital[1]) + ";" + to_string(f[i].digital[2]) 
					+ ";" + to_string(f[i].digital[3]) + ";" + to_string(stressedStatus) + ";" + to_string(workloadStatus) + ";" + to_string(currentState) + "\n";
					
				dtfObj->writeData(str, ftwb);
			}
			/*if (stressedStatus == 1 || workloadStatus == 1)
			{
				stressedStatus = 0;
				workloadStatus = 0;
			}*/
		#endif		
	#endif

	#if DISPLAY_GRAPH == 1
	#else	
		}
	#endif
}

// ##################################################################### //
// #### Idle, Init, Keyboard, and Reshape functions for glut graph ##### //
// ##################################################################### //
void reshapeECG(int w, int h)   // Create The Reshape Function (the viewport)
{
	glutSetWindow(gwECG);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (float)w / (float)h, 0.1, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h);
}

void reshapeEMG(int w, int h)   // Create The Reshape Function (the viewport)
{
	glutSetWindow(gwEMG);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (float)w / (float)h, 0.1, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h);
}

void reshapeEEG(int w, int h)   // Create The Reshape Function (the viewport)
{
	glutSetWindow(gwEEG);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (float)w / (float)h, 0.1, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h);
}

void reshapeEDA(int w, int h)   // Create The Reshape Function (the viewport)
{
	glutSetWindow(gwEDA);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (float)w / (float)h, 0.1, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h);
}

void idle(void)
{
	glutPostRedisplay();
}

void init(GLvoid)     // Create Some Everyday Functions
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.f);				// Black Background
														//glClearDepth(1.0f);								// Depth Buffer Setup
	myGraph = OGLGraph::Instance();
	myGraph->setup(4050, 2000, 10, 10, 4, 6, 1, 1000, gwECG, gwEMG, gwEEG, gwEDA);
}

bool keypressed(void)
{
	return (_kbhit() != 0);
}

void keyboard(unsigned char key, int x, int y)  // Create Keyboard Function
{
	switch (key)
	{
	case 27:        // When Escape Is Pressed...
		#if FROM_FILE == 1
		#else
			dev.stop();
		#endif
		#if RECORD_BITALINO == 1
			dtfObj->fin();
		#endif
		
		doProcessing = false;
		exit(0);   // Exit The Program
		break;        // Ready For Next Case
	case 83:
		stressedStatus = 1;
		break;
	case 87:
		workloadStatus = 1;
		break;
	default:        // Now Wrap It Up
		break;
	}
}

// ################################################################### //
// ################# Main functions ################################## //
// ################################################################### //
void main(int argc, char** argv)   // Create Main Function For Bringing It All Together
{
	initialize();

	// ---------------------------------------------------------- //
	// Starting All three threads for Camera, Tobii, and Bitalino //
	// ---------------------------------------------------------- //
	
	#if CAMERA == 1
		std::thread camThread(pCAM, argc, argv);
	#endif

	#if TOBII == 1

		getWindow();
		printf("\n\nWidth: %f,\t Height: %f\n", SCREEN_WIDTH, SCREEN_HEIGHT);

		TX_CONTEXTHANDLE hContext = TX_EMPTY_HANDLE;
		TX_TICKET hConnectionStateChangedTicket = TX_INVALID_TICKET;
		TX_TICKET hEventHandlerTicket = TX_INVALID_TICKET;
		BOOL success;

		// initialize and enable the context that is our link to the EyeX Engine.
		success = txInitializeEyeX(TX_EYEXCOMPONENTOVERRIDEFLAG_NONE, NULL, NULL, NULL, NULL) == TX_RESULT_OK;
		success &= txCreateContext(&hContext, TX_FALSE) == TX_RESULT_OK;
		success &= InitializeGlobalInteractorSnapshot(hContext);
		success &= txRegisterConnectionStateChangedHandler(hContext, &hConnectionStateChangedTicket, OnEngineConnectionStateChanged2, NULL) == TX_RESULT_OK;
		success &= txRegisterEventHandler(hContext, &hEventHandlerTicket, HandleEvent, NULL) == TX_RESULT_OK;
		success &= txEnableConnection(hContext) == TX_RESULT_OK;

		// let the events flow until a key is pressed.
		if (success) {
			printf("Initialization was successful.\n");
		}
		else {
			printf("Initialization failed.\n");
		}

	#endif

	std::thread processingThread(insertData);

	#if BITALINO == 1
		try
		{
			#if FROM_FILE == 1
				myfile.open(fileToReadBitalino);
			#else
				puts("Connecting to device...");
				dev.open("20:16:07:18:15:45");  // device MAC address (Windows and Linux)
				puts("Connected to device. Press Enter to exit.");

				std::string ver = dev.version();    // get device version string
				printf("BITalino version: %s\n", ver.c_str());

				dev.battery(10);  // set battery threshold (optional)
				dev.start(1000, { 0, 1, 2, 3, 4, 5 });   // start acquisition of all channels at 1000 Hz
			#endif
		}
		catch (std::exception& e)
		{
			printf("ERROR!  %s", e.what());
			exit(0);
		}

		#if DISPLAY_GRAPH == 1
			glutInit(&argc, argv); // Erm Just Write It =)	
			glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // Display Mode
			glutInitWindowSize(900, 500); // If glutFullScreen wasn't called this is the window size 500, 250
	
			#if PROCESS_ECG == 1
				gwECG = glutCreateWindow("ECG Graph");
				init();
				glutDisplayFunc(pBitalino);
				glutReshapeFunc(reshapeECG);
				glutKeyboardFunc(keyboard);
			#endif
			#if PROCESS_EMG == 1
				gwEMG = glutCreateWindow("EMG Graph"); // Window Title (argv[0] for current directory as title)
				glutPositionWindow(50, 50);
				init();
				glutDisplayFunc(pBitalino);  // Matching Earlier Functions To Their Counterparts
				glutReshapeFunc(reshapeEMG);
				glutKeyboardFunc(keyboard);
			#endif		
			#if PROCESS_EEG == 1
				gwEEG = glutCreateWindow("EEG Graph"); // Window Title (argv[0] for current directory as title)
				glutPositionWindow(100, 100);
				init();
				glutDisplayFunc(pBitalino);  // Matching Earlier Functions To Their Counterparts
				glutReshapeFunc(reshapeEEG);
				glutKeyboardFunc(keyboard);
			#endif
			#if PROCESS_EDA == 1
				gwEDA = glutCreateWindow("EDA Graph"); // Window Title (argv[0] for current directory as title)
				glutPositionWindow(150, 150);
				init();
				glutDisplayFunc(pBitalino);  // Matching Earlier Functions To Their Counterparts
				glutReshapeFunc(reshapeEDA);
				glutKeyboardFunc(keyboard);
			#endif
	
			glutIdleFunc(idle);
			glutMainLoop();          // Initialize The Main Loop
		#else
			std::thread bitalinoThread(pBitalino);
		#endif
	#endif

NotEscape:printf("Press Esc key to exit.");
	char keyPressed = getch();
	if (keyPressed == 's')
	{
		stressedStatus = 1;
		workloadStatus = 0;
		printf("Stressed Activated!");
		goto NotEscape;
	}
	else if (keyPressed == 'w')
	{
		workloadStatus = 1;	
		stressedStatus = 0;
		printf("Workload Activated!");
		goto NotEscape;
	}
	else if (keyPressed == 'b')
	{
		workloadStatus = 1;
		stressedStatus = 1;
		printf("Both Activated!");
		goto NotEscape;
	}
	else if (keyPressed == 'd')
	{
		workloadStatus = 0;
		stressedStatus = 0;
		printf("Both Deactivated!");
		goto NotEscape;
	}
	else if (keyPressed == 'r')
	{
		currentState = -1;
		printf("In Resting Phase!");
		goto NotEscape;
	}
	else if (keyPressed == 't')
	{
		currentState = 1;
		printf("In Testing Phase!");
		goto NotEscape;
	}
	else if (keyPressed == 'i')
	{
		currentState = 0;
		printf("In Idle Phase!");
		goto NotEscape;
	}
	else if ((int)keyPressed == 27)
		doProcessing = false;

	#if DISPLAY_GRAPH == 1
	#else
		#if BITALINO == 1
			bitalinoThread.join();
		#endif
	#endif

	#if CAMERA == 1
		camThread.join();
	#endif

	// -------------------------------------------------------------------- //
	// Closing Tobii and waiting for Camera thread to finish processing //
	// -------------------------------------------------------------------- //
	#if TOBII == 1

		printf("Exiting.\n");

		// disable and delete the context.
		txDisableConnection(hContext);
		txReleaseObject(&g_hGlobalInteractorSnapshot);
		success = txShutdownContext(hContext, TX_CLEANUPTIMEOUT_DEFAULT, TX_FALSE) == TX_RESULT_OK;
		success &= txReleaseContext(&hContext) == TX_RESULT_OK;
		success &= txUninitializeEyeX() == TX_RESULT_OK;
		if (!success) {
			printf("EyeX could not be shut down cleanly. Did you remember to release all handles?\n");
		}
	#endif	
		
	processingThread.join();
}