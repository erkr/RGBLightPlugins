#pragma once
#include <atlstr.h>

class PluginSettings {
public:
	virtual void SaveSettings();
	virtual bool ReadSettings();
	int  doBeepForExceptions = 0;
	int  ControlIdx = 0;
	int  TCold = 25;
	int  TWarm = 55; 
	int  THot = 60; 
	int  TCritical = 80;
	int  LoadBasedIntensity = 1; // true
	int  ILowPct = 20; // 0-100
	int  IHighPct = 100; // 0-100
	int  HUECold = 240; /*BLUE*/
	int  HRangeC2W = -240; /* towards RED*/
	int  HRangeW2H = 0; /*whole range RED */
	int  HRangeH2C = -20; /* towards violet-red */
private:
} ;

CString GetFullINIFilePath();

