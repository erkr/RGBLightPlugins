#pragma once

#include <atlstr.h>
#include <atlsafe.h>
#include "MysticLight_SDK.h"

/*  CMysticLightCtrl
	Inspired by example console app from SimoDax on Github: https://github.com/SimoDax/MSI-mystic-light-tool/blob/master/MSI-mystic-light-tool.cpp
	Moved everything into a class to properly handle recources
	Class also hides the usage of COM related types like BSTR
	This Demo uses the fist device, first LED; change it to your needs

	Usage:
	  // First call
	  - Connect(optional:style); // this call connects and initializes the SDK dll, stores the current settings, and by default selects a style that allows color changes
	  // do your stuff using:
	  Set/Get methods
	  // Last Call
	  - Disconnect(optional:restore); // this call optionally restored the original settings and then releases all resources
*/

class CMysticLightCtrl
{
public:
	bool Connect(unsigned int deviceIdx=0); // first call. 
	bool IsConnected() { return MLinstance != NULL; };
	void Disconnect();  // last call to cleanup
	// these methods default to the first LED on the first device (MotherBoard)
	bool SetLedStyle(CString newStyle=L"Steady", int LedIdx=0); // Note that not all styles allow to change colors. The default L"Steady" will do!
	bool SetLedRGB(DWORD R, DWORD G, DWORD B, int LedIdx = 0);
	bool GetLedRGB(DWORD *R, DWORD *G, DWORD *B, int LedIdx = 0);
	bool SetLedHSV(int H, double S, double V, int LedIdx = 0);
	bool GetLedHSV(int *H, double *S, double *V, int LedIdx = 0);
	// didn't yet need the other methods; 

	CString GetLastError();
	int NrOfLEDS();

private:
	HINSTANCE MLinstance=NULL;
	CString lastError;
	BSTR deviceName;
	int nrOfLEDs=0;

protected:
	CComSafeArray<BSTR> devices;	//Wrapper class to use safearrays in C++ (https://msdn.microsoft.com/en-us/magazine/mt795188.aspx)
			  					    //according to MSI docs GetDeviceInfo should return a safearray of BSTR for the devices..
	CComSafeArray<BSTR> ledCount;	//.. and a safearray of BSTR for the led count in each device

	LPMLAPI_Initialize _Initialize;
	LPMLAPI_GetErrorMessage _GetErrorMessage;
	LPMLAPI_GetDeviceInfo _GetDeviceInfo;
	LPMLAPI_GetLedInfo _GetLedInfo;
	LPMLAPI_GetLedColor _GetLedColor;		//coloUr is better tbh..
	LPMLAPI_GetLedStyle	_GetLedStyle;
	LPMLAPI_GetLedMaxBright _GetLedMaxBright;
	LPMLAPI_GetLedBright _GetLedBright;
	LPMLAPI_GetLedMaxSpeed _GetLedMaxSpeed;
	LPMLAPI_GetLedSpeed _GetLedSpeed;
	LPMLAPI_SetLedColor _SetLedColor;
	LPMLAPI_SetLedStyle _SetLedStyle;
	LPMLAPI_SetLedBright _SetLedBright;
	LPMLAPI_SetLedSpeed _SetLedSpeed;
	void  rgb2hsv(DWORD R, DWORD G, DWORD B, int *H, double *S, double *V);
	void  hsv2rgb(int H, double S, double V, DWORD *R, DWORD *G, DWORD *B);
	void  SetLastError(CString msg, int status);
	void  SetLastError(CString msg);

	void range255(DWORD &a) { if (a < 0) a = 0; if (a > 255) a = 255; }
	void range1(double& a) { if (a < 0.0) a = 0.0; if (a > 1.0) a = 1.0; }
};



